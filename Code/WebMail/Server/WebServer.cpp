#include "WebServer.h"

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
        std::cout << "(INFO)Db: Connect error! -> " << mysql_error(&mysql) << std::endl;
        return;
    }
    std::cout << "(INFO)Db: Connect successed!" << std::endl;
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
        std::cout << "(INFO)Server: Bind error!" << std::endl;
        return;
    }
    listen(socket_fd, 40);
    std::cout << "(INFO)Server: Waiting for client!" << std::endl;

    int epoll_fd = epoll_create(EPOLLSIZE);                            // 1.初始化
    fcntl(socket_fd, F_SETFL, fcntl(socket_fd, F_GETFL) | O_NONBLOCK); //设置为非阻塞
    Epoll_Ctl(epoll_fd, socket_fd, EPOLL_CTL_ADD, EPOLLIN);

    while (true)
    {
        int event_cnt = epoll_wait(epoll_fd, events, EPOLLSIZE, -1); // 3.返回有消息的事件数量，且事件存放在events中
        if (event_cnt < 0)
        {
            std::cout << "(INFO)Epoll: epoll_wait error!" << std::endl;
            return;
        }

        Handle_Events(epoll_fd, events, event_cnt, socket_fd);
    }
}

void WebServer::Send(int fd, int code, std::string info)
{
    struct BACK_INFO back_info;
    back_info.code = code;
    strncpy(back_info.info, (char *)info.c_str(), info.length() + 1);
    if (send(fd, &back_info, sizeof(BACK_INFO), 0) < 0)
        std::cout << "(INFO)Server: Send error!" << std::endl;
}

template <class T>
void WebServer::Recv(int fd, char *buffer, T &info)
{
    memset(buffer, 0, sizeof(buffer));
    recv(fd, buffer, sizeof(T), 0);
    memcpy(&info, buffer, sizeof(T));
}

void WebServer::ReturnMessage(Redis redis, const char *user_email, char *email_id)
{
    std::cout << "-----" << user_email << " recv an email----- " << email_id << std::endl;
    MyStruct::SESSION session = redis.GetSession(user_email);

    if (session.login_flag() == 1)
    {
        int fd = session.socket_fd();
        int ret = send(fd, email_id, sizeof(email_id), 0);
        if (ret < 0)
        {
            std::cout << "(INFO)Server: Send email error!" << std::endl;
            return;
        }
    }
    else
    {
        std::cout << "(INFO)ReturnEmail: User '" << user_email << "' not login!" << std::endl;
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
    std::cout << inet_ntoa(c_addr.sin_addr) << ":" << c_addr.sin_port << " is connected!!" << std::endl;

    smtp.ConnectReturn(accept_fd);

    // Send(accept_fd, 1, "Back: Connect OK!");
    Epoll_Ctl(epoll_fd, accept_fd, EPOLL_CTL_ADD, EPOLLIN);
}

void WebServer::Epoll_Ctl(int epoll_fd, int fd, int op, int dir)
{
    if (op == EPOLL_CTL_DEL)
    {
        std::cout << "(INFO)Server: A Client close!" << std::endl;
    }

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
    if (strncmp(Msg_Tag, "Auth Login", 11) == 0)
    {
        // char username[32];
        // smtp.ReturnMessage(fd, 334, "username\n");

        // smtp.RecvMessage(fd, username);

        // Send(fd, 1, "Back: Tag Ok!");
        // struct LOGIN_INFO login_info;
        // Recv(fd, buffer, login_info);

        std::future<int> rt = tp.addTask(Login, login_info, mysql); //分配线程池任务
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

        std::future<int> rt = tp.addTask(Register, register_info, mysql);
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

        std::future<int> rt = tp.addTask(Send_Email, email, mysql, redis);
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
        std::cout << "(INFO)Client: Error msg!" << std::endl;
    }
}

int Logout()
{
}

int WebServer::Login(struct LOGIN_INFO login_info, MYSQL mysql)
{
    std::string query_sql = "select id,password from User where email = '" + (std::string)login_info.email + "'";
    mysql_query(&mysql, query_sql.c_str());
    MYSQL_RES *res = mysql_store_result(&mysql);
    MYSQL_ROW row = mysql_fetch_row(res);
    if (strcmp(row[1], login_info.password) != 0)
    {
        std::cout << "(INFO)Login: Password error!" << std::endl;
        mysql_free_result(res);
        return -1;
    }
    std::cout << "(INFO)Login: Login successed!" << std::endl;

    int user_id = atoi(row[0]);
    mysql_free_result(res);
    return user_id;
}

int WebServer::Register(struct REGISTER_INFO register_info, MYSQL mysql)
{
    std::string query_sql = "select * from User where email = '" + (std::string)register_info.email + "'";
    mysql_query(&mysql, query_sql.c_str());
    MYSQL_RES *res = mysql_store_result(&mysql);
    if (res->row_count > 0)
    {
        std::cout << "(INFO)Register: User exists!" << std::endl;
        mysql_free_result(res);
        return -1;
    }
    mysql_free_result(res);

    std::string insert_sql = "insert into User values(NULL, '" + (std::string)register_info.email + "','" + (std::string)register_info.password + "')";
    if (mysql_query(&mysql, insert_sql.c_str()))
    {
        std::cout << "(INFO)Register: Register error!" << std::endl;
        return -2;
    }
    std::cout << "(INFO)Register: Register successed!" << std::endl;

    return 1;
}

int WebServer::Send_Email(struct EMAIL email, MYSQL mysql, Redis redis)
{
    mysql_query(&mysql, "START TRANSACTION");
    std::string insert_sql = "insert into Email values(NULL, '" + (std::string)email.sender +
                             "','" + (std::string)email.receiver + "','" + (std::string)email.subject +
                             "','" + (std::string)email.date + "','" + (std::string)email.message + "')";

    if (mysql_query(&mysql, insert_sql.c_str()))
    {
        std::cout << "(INFO)Send_Email: Insert error!" << std::endl;
        return -2;
    }

    std::string query_sql = "select LAST_INSERT_ID()";
    char email_id[16];
    mysql_query(&mysql, query_sql.c_str());

    MYSQL_RES *res = mysql_store_result(&mysql);
    char *e_id = mysql_fetch_row(res)[0];
    strncpy(email_id, e_id, strlen(e_id) + 1);
    mysql_free_result(res);

    if (redis.PushEmail(email.receiver, email_id) < 0)
    {
        std::cout << "(INFO)Send_Email: Send error!" << std::endl;
        mysql_rollback(&mysql);
        return -1;
    }
    std::cout << "(INFO)Send_Email: Send successed!" << std::endl;

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
    std::cout << "(INFO)Main: Exit!" << std::endl;
    return 0;
}