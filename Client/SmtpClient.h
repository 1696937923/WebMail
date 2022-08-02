#include <sys/socket.h>

#include "Serialize.cpp"

#define BUFFERSIZE 1024
#define SUCCESS 1
#define ERROR 0
#define HELO "helo"
#define AUTHLOGIN "auth login"
#define MAILFROM "mail from"
#define RCPTTO "rcpt to"
#define DATA "data"
#define QUIT "quit"

class SMTPCLIENT
{
public:
    // send smtp protocol msg
    int SendSmtpEmail(int, MyStruct::EMAIL, char *, std::string);

private:
    int Do_Helo(int);
    int Do_AuthLogin(int, MyStruct::EMAIL, char *, std::string);
    int Do_MailFrom(int, MyStruct::EMAIL);
    int Do_RcptTo(int, MyStruct::EMAIL);
    int Do_Data(int, MyStruct::EMAIL, std::string);

    int ParseBackMsg(char *);
    int SendToServer(int, const char *);
    int RecvFromServer(int, char *, size_t);
};
