#include <sys/socket.h>
#include <sys/epoll.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>
#include <fcntl.h>

#include "ThreadPool.cpp"
#include "SmtpServer.h"

#define BUFFERSIZE 1024
#define EPOLLSIZE 512
#define SMTP_FD 1
#define NORMAL_FD 0
#define LOGIN "login"
#define LOGOUT "logout"
#define REGISTER "register"
#define PULL "pull"

#define PULLEMAIL_CODE 520
#define NEWEMAIL_CODE 250
#define BACKSUCCESS_CODE 220
#define BACKFAIL_CODE 230

const char *host = "127.0.0.1";
const char *user = "root";
const char *passwd = "123456";
const char *db = "WebMail";
const unsigned int Db_Port = 3306;
const uint16_t NormalServer_Port = 6789;
const uint16_t SmtpServer_Port = 6666;
const unsigned int Redis_Port = 6379;

struct LOGIN_INFO;    //登录信息
struct LOGOUT_INFO;   //登出信息
struct REGISTER_INFO; //注册信息
struct PULL_INFO;     //拉取邮件信息
struct BACK_INFO;     //返回信息

class WebServer
{
public:
    WebServer();
    ~WebServer();
    void Run(); //启动服务器

private:
    int normal_fd;
    int smtp_fd;

    Mysql mysql;
    Redis redis;
    SMTPSERVER smtp;
    ThreadPool tp;
    Serialize se;

    void Start_Service(); //开启服务器监听
    int Start_TcpServer(const char *, const uint16_t, int);

    void Epoll_Ctl(int, int, int, int, int);
    void Handle_Events(int, struct epoll_event *, int);
    void Do_Accept(int, int, int);

    static void Do_NormalServer(int, int, void *);
    static void Do_SmtpServer(int, int, void *);

    int BackMsg(int, int, const char *);
    template <class T>
    int RecvMsg(int, char *, T &);

    void Login(int, int);
    void Register(int, int);
    void PullEmail(int, int, std::string);
    void Logout(int, int);
    void ReturnMessage(Redis, std::string); //监听用户新消息,并且将消息发送给客户端
};