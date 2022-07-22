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

#include "../ThreadPool/ThreadPool.h"
#include "../Redis/Redis.h"
#include "../Crypto/AES.h"
#include "../Crypto/RSA.h"
#include "../Crypto/SHA.h"
#include "../SMTP/SMTP.h"

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
    SMTP smtp;
    int socket_fd;
    ThreadPool tp;
    Serialize se;

    void Db_Connect();    //连接mysql数据库
    void Start_Service(); //开启服务器监听

    void Send(int, int, std::string);
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
    static void ReturnMessage(Redis, const char *, char *); //监听用户新消息,并且将消息发送给客户端
};