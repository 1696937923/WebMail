#include <iostream>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <mysql/mysql.h>
#include <string.h>
// #include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>
#include <cstdlib>
#include <fcntl.h>
using namespace std;

#define BUFFERSIZE 1024
#define EPOLLSIZE 512
const char *Host = "127.0.0.1";
const char *User = "root";
const char *Passwd = "123456";
const char *Db_Name = "WebMail";
const unsigned int Db_Port = 3306;
const uint16_t Sock_Port = 6789;

class WebServer
{
public:
    void Run(); //启动服务器

private:
    MYSQL mysql;
    MYSQL_RES *res;
    MYSQL_ROW row;
    struct REGISTER_INFO;
    struct LOGIN_INFO;
    struct QUERY_INFO;
    struct PULL_INFO;
    struct BACK_INFO;

    void Db_Connect();    //连接mysql数据库
    void Start_Service(); //开启服务器监听
    void Handle_Events(int, struct epoll_event *, int, int);
    void Do_Accept(int, int);
    void Do_Deal(int, int, char *);
    int Login(struct LOGIN_INFO);
    int Register(struct REGISTER_INFO);
    int Query_Email(struct QUERY_INFO);
    int Pull_Email(struct PULL_INFO);
};

struct WebServer::REGISTER_INFO
{
    char email[48];
    char password[32];
};

struct WebServer::LOGIN_INFO
{
    char email[48];
    char password[32];
};

struct WebServer::QUERY_INFO
{
};

struct WebServer::PULL_INFO
{
};

struct WebServer::BACK_INFO
{
    int code;
    char info[48];
};

void WebServer::Run()
{
    Db_Connect();
    Start_Service();
}

void WebServer::Db_Connect()
{
    mysql_init(&mysql);
    if (!mysql_real_connect(&mysql, Host, User, Passwd, Db_Name, Db_Port, NULL, 0))
    {
        cout << "(INFO)Db: Connect error! -> " << mysql_error(&mysql) << endl;
        return;
    }
    cout << "(INFO)Db: Connect correct!" << endl;
}

void WebServer::Start_Service()
{
    struct epoll_event events[EPOLLSIZE];
    struct epoll_event ev;
    struct sockaddr_in s_addr;
    memset(&s_addr, 0, sizeof(sockaddr_in));

    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    s_addr.sin_family = AF_INET;
    s_addr.sin_addr.s_addr = inet_addr(Host);
    s_addr.sin_port = htons(Sock_Port); //主机字节序转为网络字节序，s代表short

    if (bind(socket_fd, (struct sockaddr *)&s_addr, sizeof(sockaddr_in)) < 0)
    {
        cout << "(INFO)Server: Bind error!" << endl;
        return;
    }
    listen(socket_fd, 40);

    int epoll_fd = epoll_create(EPOLLSIZE);                            // 1.初始化
    fcntl(socket_fd, F_SETFL, fcntl(socket_fd, F_GETFL) | O_NONBLOCK); //设置为非阻塞

    ev.events = EPOLLIN; //表示输入事件
    ev.data.fd = socket_fd;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, socket_fd, &ev); // 2.添加事件，EPOLL_CTL_ADD表示添加事件

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

    // inet_aton(const char *straddr, struct in_addr *addrp);
    // 把字符串形式的“192 .168.1.123”转为网络能识别的格式
    // inet_ntoa(struct in_addr inaddr);
    // 把网络格式的ip地址转为字符串形式
}

void WebServer::Do_Accept(int epoll_fd, int socket_fd)
{
    struct sockaddr_in c_addr;
    socklen_t len = sizeof(sockaddr_in);
    struct epoll_event ev;
    memset(&c_addr, 0, (size_t)len);

    int accept_fd = accept(socket_fd, (struct sockaddr *)&c_addr, &len);
    cout << inet_ntoa(c_addr.sin_addr) << ":" << c_addr.sin_port << " is connected!!" << endl;

    send(accept_fd, "ok", 3, 0);
    // fcntl(accept_fd, F_SETFL, fcntl(accept_fd, F_GETFL) | O_NONBLOCK); //设置为非阻塞
    ev.data.fd = accept_fd;
    ev.events = EPOLLIN;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, accept_fd, &ev); //继续添加事件
}

void WebServer::Do_Deal(int i, int fd, char *buffer)
{
    memset(buffer, 0, sizeof(buffer));

    read(fd, buffer, sizeof(buffer));
    // write(fd, buffer, strlen(buffer));
    memset(buffer, 0, sizeof(buffer));
    snprintf(buffer, sizeof(buffer), "%d", i);
    for (int j = 0; j < 50; j++)
    {
        send(fd, buffer, sizeof(buffer), 0);
        sleep(0.5);
    }
    // int ret = read(fd, buffer, sizeof(buffer) - 1);
    // cout << buffer << endl;
    // write(fd, "ok", 3);
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
            Do_Deal(i, fd, buffer);
    }
}

int WebServer::Login(struct LOGIN_INFO login_info)
{
    string query_sql = "select password from User where email = '" + (string)login_info.email + "'";
    mysql_query(&mysql, query_sql.c_str());
    res = mysql_store_result(&mysql);
    row = mysql_fetch_row(res);

    if (row[0] != login_info.password)
    {
        cout << "(INFO)Login: Password error!" << endl;
        mysql_free_result(res);
        return -1;
    }
    cout << "(INFO)Login: Login successed!" << endl;
    mysql_free_result(res);
    return 1;
}

int WebServer::Register(struct REGISTER_INFO register_info)
{
    string query_sql = "select * from User where email = '" + (string)register_info.email + "'";
    mysql_query(&mysql, query_sql.c_str());
    res = mysql_store_result(&mysql);
    if (res->row_count > 0)
    {
        cout << "(INFO)Register: User exists!" << endl;
        mysql_free_result(res);
        return -1;
    }
    mysql_free_result(res);

    string insert_sql = "insert into User values(NULL, '" + (string)register_info.email + "','" + (string)register_info.password + "')";
    cout << insert_sql << endl;
    if (mysql_query(&mysql, insert_sql.c_str()))
    {
        cout << "(INFO)Register: Register error!" << endl;
        return -2;
    }
    cout << "(INFO)Register: Register successed!" << endl;
    return 1;
}

int WebServer::Query_Email(struct QUERY_INFO query_info)
{
    return 1;
}

int WebServer::Pull_Email(struct PULL_INFO pull_info)
{
    return 1;
}

int main()
{
    WebServer webserver;
    webserver.Run();
}