#include <sys/socket.h>
#include <time.h>
#include "Mysql.h"

#define BUFFERSIZE 1024
#define SUCCESS 1
#define ERROR 0
#define HELO "helo"
#define AUTHLOGIN "auth login"
#define MAILFROM "mail from"
#define RCPTTO "rcpt to"
#define DATA "data"
#define QUIT "quit"

class SMTPSERVER
{
public:
    // parse smtp protocol
    int Deal_Connect(int);
    int Deal_Helo(int);
    int Deal_AuthLogin(int, Mysql, MyStruct::EMAIL *, std::string);
    int Deal_MailFrom(int, MyStruct::EMAIL);
    int Deal_RcptTo(int, Mysql, MyStruct::EMAIL *);
    int Deal_Data(int, Mysql, Redis, MyStruct::EMAIL *, std::string);

private:
    char Msg_Tag[16], Msg[64];
    char password[80], username[48];
    char sender[48], receiver[48];
    char subject[128], content[512];

    //返回给客户端smtp响应的消息
    int ReturnToClient(int, int, const char *);

    //接受返回的消息
    int RecvFromClient(int, char *, size_t, int);

    //服务器接受客户端指令
    int RecvCommand(int, char *, char *);

    //提取邮件内容，以.截止
    int RecvEmailData(int, char *, char *, std::string);

    //从<xxxxxxx xx.com>中提取 xxxxxxx xx.com 内容
    int GetEmailFromCmd(char *, char *, size_t);

    void ParseEmail(std::string, char *, char *);

    void GetLocalTime(char *);
};