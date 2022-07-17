#include <iostream>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <mysql/mysql.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <hiredis/hiredis.h>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include "../ThreadPool/ThreadPool.cpp"
#include "../Redis/Redis.cpp"
// #include "../Serialize/Serialize.cpp"
// #include <threads.h>
using namespace std;
// 1.用户刚登录时，先获取消息队列中未读的消息数并返回给用户，然后订阅特定频道来实时监听用户的新邮件，
// 有新邮件时便通知用户，当用户下线时，便取消对频道的订阅
// 2.完成剩下功能
// 3.加锁
// 4.加密

#define BUFFERSIZE 1024
#define EPOLLSIZE 512
const char *Host = "127.0.0.1";
const char *User = "root";
const char *Passwd = "123456";
const char *Db_Name = "WebMail";
const unsigned int Db_Port = 3306;
const uint16_t Sock_Port = 6789;
const unsigned int Redis_Port = 6379;

struct LOGIN_INFO;    //登录信息
struct REGISTER_INFO; //注册信息
struct QUERY_INFO;    //查询邮件信息
struct PULL_INFO;     //拉取邮件信息
struct BACK_INFO;     //返回信息
struct EMAIL;         //邮件信息

class WebServer
{
public:
    WebServer();
    ~WebServer();
    void Run(); //启动服务器

private:
    MYSQL mysql;
    Redis redis;
    int socket_fd;
    ThreadPool tp;
    Serialize se;

    void Db_Connect();    //连接mysql数据库
    void Start_Service(); //开启服务器监听

    void Send(int, int, string);
    template <class T>
    void Recv(int, char *, T &);

    void Epoll_Ctl(int, int, int, int);
    void Handle_Events(int, struct epoll_event *, int, int);
    void Do_Accept(int, int);
    void Do_Deal(int, int, char *);

    static int Logout();
    static int Login(struct LOGIN_INFO, MYSQL);
    static int Register(struct REGISTER_INFO, MYSQL);
    static int Send_Email(struct EMAIL, MYSQL, Redis);
    static int Query_Email(struct QUERY_INFO, MYSQL);
    static int Pull_Email(struct PULL_INFO, MYSQL);
    static void ReturnMessage(Redis, const char *, const char *); //监听用户新消息,并且将消息发送给客户端
};

struct REGISTER_INFO
{
    char email[48];
    char password[32];
};

struct LOGIN_INFO
{
    char email[48];
    char password[32];
};

struct QUERY_INFO
{
};

struct PULL_INFO
{
};

struct BACK_INFO
{
    int code;
    char info[48];
};

struct EMAIL
{
    char sender[48];
    char receiver[48];
    char subject[128];
    char date[32];
    char message[512];
};

WebServer::WebServer()
{
    // Run();
}

WebServer::~WebServer()
{
    mysql_close(&mysql);
    tp.stop();
    close(socket_fd);
}

void WebServer::Run()
{
    Db_Connect();                  //数据库连接
    redis.start(Host, Redis_Port); //连接redis
    tp.start();                    //开启线程池
    Start_Service();               //开启socket端口监听
}

void WebServer::Db_Connect()
{
    mysql_init(&mysql);
    if (!mysql_real_connect(&mysql, Host, User, Passwd, Db_Name, Db_Port, NULL, 0))
    {
        cout << "(INFO)Db: Connect error! -> " << mysql_error(&mysql) << endl;
        return;
    }
    cout << "(INFO)Db: Connect successed!" << endl;
}

void WebServer::Start_Service()
{
    struct epoll_event events[EPOLLSIZE];
    struct sockaddr_in s_addr;
    memset(&s_addr, 0, sizeof(sockaddr_in));

    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    s_addr.sin_family = AF_INET;
    s_addr.sin_addr.s_addr = inet_addr(Host);
    s_addr.sin_port = htons(Sock_Port); //主机字节序转为网络字节序，s代表short

    if (bind(socket_fd, (struct sockaddr *)&s_addr, sizeof(sockaddr_in)) < 0)
    {
        cout << "(INFO)Server: Bind error!" << endl;
        return;
    }
    listen(socket_fd, 40);
    cout << "(INFO)Server: Waiting for client!" << endl;

    int epoll_fd = epoll_create(EPOLLSIZE);                            // 1.初始化
    fcntl(socket_fd, F_SETFL, fcntl(socket_fd, F_GETFL) | O_NONBLOCK); //设置为非阻塞
    Epoll_Ctl(epoll_fd, socket_fd, EPOLL_CTL_ADD, EPOLLIN);

    while (true)
    {
        int event_cnt = epoll_wait(epoll_fd, events, EPOLLSIZE, -1); // 3.返回有消息的事件数量，且事件存放在events中
        if (event_cnt < 0)
        {
            cout << "(INFO)Epoll: epoll_wait error!" << endl;
            return;
        }
        Handle_Events(epoll_fd, events, event_cnt, socket_fd);
    }
}

void WebServer::Send(int fd, int code, string info)
{
    struct BACK_INFO back_info;
    back_info.code = code;
    strncpy(back_info.info, (char *)info.c_str(), info.length() + 1);
    if (send(fd, &back_info, sizeof(BACK_INFO), 0) < 0)
        cout << "(INFO)Server: Send error!" << endl;
}

template <class T>
void WebServer::Recv(int fd, char *buffer, T &info)
{
    memset(buffer, 0, sizeof(buffer));
    recv(fd, buffer, sizeof(T), 0);
    memcpy(&info, buffer, sizeof(T));
}

void WebServer::ReturnMessage(Redis redis, const char *user_email, const char *email_id)
{
    cout << "-----" << user_email << " recv an email----- " << email_id << endl;
    MyStruct::SESSION session = redis.GetSession(user_email);

    cout << session.login_flag() << "--" << session.socket_fd() << endl;
    if (session.login_flag() == 1)
    {
        int fd = session.socket_fd();
        int ret = send(fd, email_id, sizeof(email_id), 0);
        if (ret < 0)
        {
            cout << "(INFO)Server: Send email error!" << endl;
            return;
        }
    }
    else
    {
        cout << "(INFO)ReturnEmail: User '" << user_email << "' not login!" << endl;
        return;
    }
}

void WebServer::Handle_Events(int epoll_fd, struct epoll_event *events, int num, int socket_fd)
{
    int i, fd;
    char buffer[BUFFERSIZE];
    for (i = 0; i < num; i++)
    {
        fd = events[i].data.fd;                              //获得响应事件的描述符
        if ((fd == socket_fd) && events[i].events & EPOLLIN) //对events和EPOLLIN“与”操作，判断这个文件描述符是否有input事件
            Do_Accept(epoll_fd, fd);
        else
            Do_Deal(epoll_fd, fd, buffer);
    }
}

void WebServer::Do_Accept(int epoll_fd, int socket_fd)
{
    struct sockaddr_in c_addr;
    socklen_t len = sizeof(sockaddr_in);

    int accept_fd = accept(socket_fd, (struct sockaddr *)&c_addr, &len);
    cout << inet_ntoa(c_addr.sin_addr) << ":" << c_addr.sin_port << " is connected!!" << endl;

    Send(accept_fd, 1, "Back: Connect OK!");
    Epoll_Ctl(epoll_fd, accept_fd, EPOLL_CTL_ADD, EPOLLIN);
}

void WebServer::Epoll_Ctl(int epoll_fd, int fd, int op, int dir)
{
    if (op == EPOLL_CTL_DEL)
        cout << "(INFO)Server: A Client close!" << endl;
    struct epoll_event ev;
    ev.data.fd = fd;
    ev.events = dir;
    epoll_ctl(epoll_fd, op, fd, &ev); //继续添加事件
}

void WebServer::Do_Deal(int epoll_fd, int fd, char *buffer)
{
    char Msg_Tag[16];
    if (recv(fd, Msg_Tag, sizeof(Msg_Tag), 0) <= 0) //客户端关闭了连接
    {
        Epoll_Ctl(epoll_fd, fd, EPOLL_CTL_DEL, EPOLLIN);
        return;
    }
    if (strncmp(Msg_Tag, "Login", 5) == 0)
    {
        Send(fd, 1, "Back: Tag Ok!");
        struct LOGIN_INFO login_info;
        Recv(fd, buffer, login_info);

        future<int> rt = tp.addTask(Login, login_info, mysql); //分配线程池任务
        int code = rt.get();
        if (code > 0)
        {
            Send(fd, code, "Back: Login successed!");

            MyStruct::SESSION session;
            session.set_login_flag(1);
            session.set_socket_fd(fd);
            redis.SetSession(login_info.email, session);
        }
        else
            Send(fd, code, "Back: Password error!");
    }
    else if (strncmp(Msg_Tag, "Register", 8) == 0)
    {
        Send(fd, 1, "Back: Tag Ok!");
        struct REGISTER_INFO register_info;
        Recv(fd, buffer, register_info);

        future<int> rt = tp.addTask(Register, register_info, mysql);
        int code = rt.get();
        if (code == 1)
            Send(fd, code, "Back: Register successed!");
        else
            Send(fd, code, "Back: Register error!");
        Epoll_Ctl(epoll_fd, fd, EPOLL_CTL_DEL, EPOLLIN);
    }
    else if (strncmp(Msg_Tag, "Send Email", 11) == 0)
    {
        Send(fd, 1, "Back: Tag Ok!");
        struct EMAIL email;
        Recv(fd, buffer, email);

        future<int> rt = tp.addTask(Send_Email, email, mysql, redis);
        int code = rt.get();
        if (code == 1)
            Send(fd, code, "Back: Send Email successed!");
        else
            Send(fd, code, "Back: Send Email error!");
    }
    else if (strncmp(Msg_Tag, "Query", 5) == 0)
    {
        Send(fd, 1, "Back: Tag Ok!");
        struct QUERY_INFO query_info;
        Recv(fd, buffer, query_info);

        tp.addTask(Query_Email, query_info, mysql);
    }
    else if (strncmp(Msg_Tag, "Pull", 4) == 0)
    {
        Send(fd, 1, "Back: Tag Ok!");
        struct PULL_INFO pull_info;
        Recv(fd, buffer, pull_info);

        tp.addTask(Pull_Email, pull_info, mysql);
    }
    else
    {
        Send(fd, -1, "Tag error!");
        cout << "(INFO)Client: Error msg!" << endl;
    }
}

int Logout()
{
}

int WebServer::Login(struct LOGIN_INFO login_info, MYSQL mysql)
{
    string query_sql = "select id,password from User where email = '" + (string)login_info.email + "'";
    mysql_query(&mysql, query_sql.c_str());
    MYSQL_RES *res = mysql_store_result(&mysql);
    MYSQL_ROW row = mysql_fetch_row(res);
    if (strcmp(row[1], login_info.password) != 0)
    {
        cout << "(INFO)Login: Password error!" << endl;
        mysql_free_result(res);
        return -1;
    }
    cout << "(INFO)Login: Login successed!" << endl;
    int user_id = atoi(row[0]);
    mysql_free_result(res);
    return user_id;
}

int WebServer::Register(struct REGISTER_INFO register_info, MYSQL mysql)
{
    string query_sql = "select * from User where email = '" + (string)register_info.email + "'";
    mysql_query(&mysql, query_sql.c_str());
    MYSQL_RES *res = mysql_store_result(&mysql);
    if (res->row_count > 0)
    {
        cout << "(INFO)Register: User exists!" << endl;
        mysql_free_result(res);
        return -1;
    }
    mysql_free_result(res);

    string insert_sql = "insert into User values(NULL, '" + (string)register_info.email + "','" + (string)register_info.password + "')";
    if (mysql_query(&mysql, insert_sql.c_str()))
    {
        cout << "(INFO)Register: Register error!" << endl;
        return -2;
    }
    cout << "(INFO)Register: Register successed!" << endl;
    return 1;
}

int WebServer::Send_Email(struct EMAIL email, MYSQL mysql, Redis redis)
{
    mysql_query(&mysql, "START TRANSACTION");
    string insert_sql = "insert into Email values(NULL, '" + (string)email.sender + "','" + (string)email.receiver +
                        "','" + (string)email.subject + "','" + (string)email.date + "','" + (string)email.message + "')";

    if (mysql_query(&mysql, insert_sql.c_str()))
    {
        cout << "(INFO)Send_Email: Insert error!" << endl;
        return -2;
    }
    cout << "DbInsert OK" << endl; //

    string query_sql = "select LAST_INSERT_ID()";
    mysql_query(&mysql, query_sql.c_str());
    MYSQL_RES *res = mysql_store_result(&mysql);
    char *email_id = mysql_fetch_row(res)[0];
    mysql_free_result(res);

    if (redis.PushEmail(email.receiver, email_id) < 0)
    {
        cout << "(INFO)Send_Email: Send error!" << endl;
        mysql_rollback(&mysql);
        return -1;
    }
    cout << "(INFO)Send_Email: Send successed!" << endl;

    ReturnMessage(redis, email.receiver, email_id);
    mysql_commit(&mysql);
    return 1;
}

int WebServer::Query_Email(struct QUERY_INFO query_info, MYSQL mysql)
{
    return 1;
}

int WebServer::Pull_Email(struct PULL_INFO pull_info, MYSQL mysql)
{
    return 1;
}

int main()
{
    WebServer webserver;
    webserver.Run();
    cout << "(INFO)Main: Exit!" << endl;
    return 0;
}