#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>  //sockaddr_in函数库
#include <netinet/tcp.h> //TCP_NODELAY
#include <arpa/inet.h>   //inet_addr函数库
#include <thread>

#include "SmtpClient.h"
#include "Base64.h"

#define BUFFERSIZE 1024
#define PULLEMAIL_CODE 520
#define NEWEMAIL_CODE 250
#define BACKSUCCESS_CODE 220
#define BACKFAIL_CODE 230

#define LOGIN "login"
#define LOGOUT "logout"
#define REGISTER "register"
#define PULL "pull"

struct LOGIN_INFO;    //登录信息
struct REGISTER_INFO; //注册信息
struct PULL_INFO;     //拉取邮件信息
struct BACK_INFO;     //返回信息
struct LOGOUT_INFO;

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

class WebClient
{
public:
    void Ui();
    int Connect();
    int ConnectSmtp();
    int Login(int, struct LOGIN_INFO, void *);
    int Logout(int, struct LOGOUT_INFO);
    int Register(int, struct REGISTER_INFO);
    int Pull_Email(int, struct PULL_INFO, void*);

    int Send_Email(int, MyStruct::EMAIL, const char *);

private:
    Serialize se;
    Base64 bs64;
    SMTPCLIENT smtp;

    int Recv(int, BACK_INFO &);
    template <typename T>
    int Send(int, T);
    int SendTag(int, char *, size_t);

    void Display_Email(MyStruct::EMAIL, void *);
    static void ListeningEmail(WebClient *, int, void *);

};
