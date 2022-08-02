#include <iostream>

#include "WebServer.h"
#include "AES.cpp"
#include "SHA.cpp"
#include "ExchangeKey.cpp"

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

struct LOGOUT_INFO
{
    char email[48];
};

struct PULL_INFO
{
    char email[48];
};

struct BACK_INFO
{
    int code;
    char info[512];
};

WebServer::WebServer()
{
    // Run();
}

WebServer::~WebServer()
{
    tp.stop();
}

void WebServer::Run()
{
    mysql.start(host, user, passwd, db, Db_Port); //数据库连接
    redis.start(host, Redis_Port);                //连接redis
    tp.start();                                   //开启线程池
    Start_Service();                              //开启socket端口监听
}

void WebServer::Start_Service()
{
    struct epoll_event events[EPOLLSIZE];
    normal_fd = Start_TcpServer(host, NormalServer_Port, NORMAL_FD);
    smtp_fd = Start_TcpServer(host, SmtpServer_Port, SMTP_FD);

    int epoll_fd = epoll_create(EPOLLSIZE); // 1.初始化

    fcntl(normal_fd, F_SETFL, fcntl(normal_fd, F_GETFL) | O_NONBLOCK); //设置为非阻塞
    Epoll_Ctl(epoll_fd, normal_fd, EPOLL_CTL_ADD, EPOLLIN, NORMAL_FD); // 2.add要监听的描述符
    fcntl(smtp_fd, F_SETFL, fcntl(smtp_fd, F_GETFL) | O_NONBLOCK);     //设置为非阻塞
    Epoll_Ctl(epoll_fd, smtp_fd, EPOLL_CTL_ADD, EPOLLIN, SMTP_FD);     // add要监听的描述符

    while (true)
    {
        int event_cnt = epoll_wait(epoll_fd, events, EPOLLSIZE, -1); // 3.返回有消息的事件数量，且事件存放在events中
        if (event_cnt < 0)
        {
            std::cout << "(INFO)Epoll: epoll_wait error!" << std::endl;
            return;
        }

        Handle_Events(epoll_fd, events, event_cnt);
    }
}

int WebServer::Start_TcpServer(const char *host, const uint16_t port, int type)
{
    struct sockaddr_in s_addr;
    memset(&s_addr, 0, sizeof(sockaddr_in));

    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    s_addr.sin_family = AF_INET;
    s_addr.sin_addr.s_addr = inet_addr(host);
    s_addr.sin_port = htons(port); //主机字节序转为网络字节序，s代表short

    if (bind(socket_fd, (struct sockaddr *)&s_addr, sizeof(sockaddr_in)) < 0)
    {
        std::cout << "[INFO]Server: Port Bind error!" << std::endl;
        return -1;
    }
    listen(socket_fd, 40);

    if (type == NORMAL_FD)
        std::cout << "[INFO]Server: Normal Server start successful!" << std::endl;
    else if (type == SMTP_FD)
        std::cout << "[INFO]Server: SMTP Server start successful!" << std::endl;

    return socket_fd;
}

int WebServer::BackMsg(int fd, int code, const char *info)
{
    struct BACK_INFO back_info;

    memset(&back_info, 0, sizeof(BACK_INFO));
    back_info.code = code;
    strncpy(back_info.info, info, strlen(info));

    if (send(fd, &back_info, sizeof(BACK_INFO), 0) < 0)
    {
        std::cout << "[INFO]Server: Send Back Msg error!" << std::endl;
        return -1;
    }

    std::cout << "[INFO]Normal Server: Return message->[" << info << "] success!" << std::endl;
    return 1;
}

template <class T>
int WebServer::RecvMsg(int fd, char *buffer, T &info)
{
    memset(buffer, 0, sizeof(buffer));
    int ret = recv(fd, buffer, sizeof(T), 0);
    if (ret <= 0)
        return -1;

    memcpy(&info, buffer, sizeof(T));
    return 1;
}

void WebServer::Handle_Events(int epoll_fd, struct epoll_event *events, int num)
{
    int fd, type;

    for (int i = 0; i < num; i++)
    {
        fd = (int)(events[i].data.u64);         //获得响应事件的描述符
        type = (int)(events[i].data.u64 >> 32); //获得响应事件的消息类型

        if ((fd == normal_fd) && events[i].events & EPOLLIN) //对events和EPOLLIN“与”操作，判断这个文件描述符是否有input事件
            Do_Accept(epoll_fd, fd, NORMAL_FD);
        else if ((fd == smtp_fd) && events[i].events & EPOLLIN)
            Do_Accept(epoll_fd, fd, SMTP_FD);
        else if (type == NORMAL_FD)
        {
            Do_NormalServer(epoll_fd, fd, this);
            // tp.addTask(Do_NormalServer, epoll_fd, fd, this);
        }
        else if (type == SMTP_FD)
        {
            Do_SmtpServer(epoll_fd, fd, this);
            // tp.addTask(Do_SmtpServer, epoll_fd, fd, this);
        }
    }
}

void WebServer::Do_Accept(int epoll_fd, int fd, int type)
{
    struct sockaddr_in c_addr;
    socklen_t len = sizeof(sockaddr_in);

    int accept_fd = accept(fd, (struct sockaddr *)&c_addr, &len);

    if (type == NORMAL_FD)
    {
        std::cout << "[INFO]Server: " << inet_ntoa(c_addr.sin_addr) << ":" << c_addr.sin_port << " is connected NormalServer!!" << std::endl;
        BackMsg(accept_fd, BACKSUCCESS_CODE, "Connect OK!");
    }
    else if (type == SMTP_FD)
    {
        std::cout << "[INFO]Server: " << inet_ntoa(c_addr.sin_addr) << ":" << c_addr.sin_port << " is connected SmtpServer!!" << std::endl;
        smtp.Deal_Connect(accept_fd);
    }

    Epoll_Ctl(epoll_fd, accept_fd, EPOLL_CTL_ADD, EPOLLIN, type);
}

void WebServer::Epoll_Ctl(int epoll_fd, int fd, int op, int dir, int type)
{
    struct epoll_event ev;
    ev.events = dir;
    ev.data.u64 = (uint64_t)(((uint64_t)(type) << 32) | (uint64_t)(int)fd);
    epoll_ctl(epoll_fd, op, fd, &ev); //继续添加事件

    if (op == EPOLL_CTL_DEL)
        std::cout << "[INFO]Server: A Client close!" << std::endl;
}

void WebServer::Do_NormalServer(int epoll_fd, int fd, void *ws)
{
    WebServer *webserver = (WebServer *)ws;
    char Msg_Tag[16];
    memset(Msg_Tag, 0, sizeof(Msg_Tag));

    if (recv(fd, Msg_Tag, sizeof(Msg_Tag), 0) <= 0) //客户端关闭了连接
    {
        webserver->Epoll_Ctl(epoll_fd, fd, EPOLL_CTL_DEL, EPOLLIN, NORMAL_FD);
        return;
    }

    if (strncmp(Msg_Tag, LOGIN, strlen(LOGIN)) == 0) //长链接
    {
        webserver->BackMsg(fd, BACKSUCCESS_CODE, "Tag Ok!");
        webserver->Login(epoll_fd, fd);
    }
    else if (strncmp(Msg_Tag, REGISTER, strlen(REGISTER)) == 0)
    {
        webserver->BackMsg(fd, BACKSUCCESS_CODE, "Tag Ok!");
        webserver->Register(epoll_fd, fd);
    }
    else if (strncmp(Msg_Tag, PULL, strlen(PULL)) == 0)
    {
        webserver->BackMsg(fd, BACKSUCCESS_CODE, "Tag Ok!");

        ExchangeKey ex;
        std::string Aes_Key = ex.ExchangeAsServer(fd);

        webserver->PullEmail(epoll_fd, fd, Aes_Key);
    }
    else if (strncmp(Msg_Tag, LOGOUT, strlen(LOGOUT)) == 0)
    {
        webserver->Logout(epoll_fd, fd);
    }
    else
    {
        webserver->BackMsg(fd, BACKFAIL_CODE, "Tag error!");
        std::cout << "[INFO]Server: Error msg tag!" << std::endl;
    }
}

void WebServer::Do_SmtpServer(int epoll_fd, int fd, void *ws)
{
    ExchangeKey ex;
    std::string Aes_Key = ex.ExchangeAsServer(fd);
    WebServer *webserver = (WebServer *)ws;
    MyStruct::EMAIL email;

    // 1.处理helo指令
    int ret = webserver->smtp.Deal_Helo(fd);
    if (ret <= 0)
    {
        webserver->Epoll_Ctl(epoll_fd, fd, EPOLL_CTL_DEL, EPOLLIN, SMTP_FD);
        return;
    }

    // 2.处理auth login指令
    ret = webserver->smtp.Deal_AuthLogin(fd, webserver->mysql, &email, Aes_Key);
    if (ret <= 0)
    {
        webserver->Epoll_Ctl(epoll_fd, fd, EPOLL_CTL_DEL, EPOLLIN, SMTP_FD);
        return;
    }

    // 3.处理mail from指令
    ret = webserver->smtp.Deal_MailFrom(fd, email);
    if (ret <= 0)
    {
        webserver->Epoll_Ctl(epoll_fd, fd, EPOLL_CTL_DEL, EPOLLIN, SMTP_FD);
        return;
    }

    // 4.rcpt to指令
    ret = webserver->smtp.Deal_RcptTo(fd, webserver->mysql, &email);
    if (ret <= 0)
    {
        webserver->Epoll_Ctl(epoll_fd, fd, EPOLL_CTL_DEL, EPOLLIN, SMTP_FD);
        return;
    }

    // 5.处理data指令,并且将邮件发到邮件队列中
    ret = webserver->smtp.Deal_Data(fd, webserver->mysql, webserver->redis, &email, Aes_Key); //返回邮件在数据库的id
    if (ret <= 0)
    {
        webserver->Epoll_Ctl(epoll_fd, fd, EPOLL_CTL_DEL, EPOLLIN, SMTP_FD);
        return;
    }

    webserver->ReturnMessage(webserver->redis, email.receiver());
    webserver->Epoll_Ctl(epoll_fd, fd, EPOLL_CTL_DEL, EPOLLIN, SMTP_FD);
}

void WebServer::ReturnMessage(Redis redis, std::string user_email)
{
    std::cout << "-----" << user_email << " recv an email----- " << std::endl;
    MyStruct::SESSION session = redis.GetSession(user_email.c_str());

    if (session.login_flag() != 1)
    {
        std::cout << "[INFO]ReturnEmail: User '" << user_email << "' not login!" << std::endl;
        return;
    }

    int fd = session.socket_fd();
    BackMsg(fd, NEWEMAIL_CODE, "A New Email");
}

void WebServer::Login(int epoll_fd, int fd)
{
    char buffer[BUFFERSIZE];
    struct LOGIN_INFO login_info;

    // recv登录信息
    memset(&login_info, 0, sizeof(LOGIN_INFO));
    int ret = RecvMsg(fd, buffer, login_info);
    if (ret <= 0)
    {
        Epoll_Ctl(epoll_fd, fd, EPOLL_CTL_DEL, EPOLLIN, NORMAL_FD);
        return;
    }

    //可能是其他邮件服务器域名：

    int code = mysql.Login(login_info.email, login_info.password);
    if (code > 0)
    {
        BackMsg(fd, BACKSUCCESS_CODE, "Login successful!");

        MyStruct::SESSION session;
        session.set_login_flag(1);
        session.set_socket_fd(fd);
        redis.SetSession(login_info.email, session);

        int unread_num = redis.QueryEmailNum(login_info.email);
        BackMsg(fd, unread_num, "Unread Email Num");
    }
    else
        BackMsg(fd, BACKFAIL_CODE, "Username or Password error!");
}

void WebServer::Register(int epoll_fd, int fd)
{
    char buffer[BUFFERSIZE];
    struct REGISTER_INFO register_info;

    memset(&register_info, 0, sizeof(REGISTER_INFO));
    int ret = RecvMsg(fd, buffer, register_info);
    if (ret <= 0)
    {
        Epoll_Ctl(epoll_fd, fd, EPOLL_CTL_DEL, EPOLLIN, NORMAL_FD);
        return;
    }

    int code = mysql.Register(register_info.email, register_info.password);
    if (code == 1)
        BackMsg(fd, BACKSUCCESS_CODE, "Register successful!");
    else
        BackMsg(fd, BACKFAIL_CODE, "User exists!");

    //注册完毕后关闭连接
    Epoll_Ctl(epoll_fd, fd, EPOLL_CTL_DEL, EPOLLIN, NORMAL_FD);
}

void WebServer::PullEmail(int epoll_fd, int fd, std::string Aes_Key)
{
    AES aes;
    char buffer[BUFFERSIZE], tmp[8] = {0};
    struct PULL_INFO pull_info;

    // recv拉取邮件的信息
    memset(&pull_info, 0, sizeof(PULL_INFO));
    int ret = RecvMsg(fd, buffer, pull_info);
    if (ret <= 0)
    {
        Epoll_Ctl(epoll_fd, fd, EPOLL_CTL_DEL, EPOLLIN, NORMAL_FD);
        return;
    }

    int email_num = redis.QueryEmailNum(pull_info.email);
    sprintf(tmp, "%d", email_num);

    BackMsg(fd, PULLEMAIL_CODE, tmp);
    for (int i = 0; i < email_num; i++)
    {
        //返回序列化再bs64编码后的结构体
        std::string email = redis.PullEmail(pull_info.email);
        std::string aes_email = aes.Encrypt(email.c_str(), Aes_Key.c_str());
        //返回给客户端邮件
        BackMsg(fd, PULLEMAIL_CODE, aes_email.c_str());
    }

    // pull邮件后关闭连接
    Epoll_Ctl(epoll_fd, fd, EPOLL_CTL_DEL, EPOLLIN, NORMAL_FD);
}

void WebServer::Logout(int epoll_fd, int fd)
{
    char buffer[BUFFERSIZE];
    struct LOGOUT_INFO logout_info;

    memset(&logout_info, 0, sizeof(LOGOUT_INFO));
    int ret = RecvMsg(fd, buffer, logout_info);
    if (ret <= 0)
        return;

    MyStruct::SESSION session;
    session.set_login_flag(0);
    int code = redis.SetSession(logout_info.email, session);
    if (code < 0)
        std::cout << "[INFO]Server: Logout error!" << std::endl;

    Epoll_Ctl(epoll_fd, fd, EPOLL_CTL_DEL, EPOLLIN, NORMAL_FD);
}