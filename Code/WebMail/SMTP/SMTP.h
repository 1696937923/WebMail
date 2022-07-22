#include <sys/socket.h>
#include <string>

class SMTP
{
public:
    //返回给客户端smtp响应的消息
    int ReturnMessage(int, int, const char *);

    //接受服务器返回的消息
    int RecvMessage(int);

    int ConnectReturn(int);
    int Connect(const char *);
    int AuthLogin();
    int PushEmail();
    int Quit();

private:
    int SetSenderEmail();
    int SetReceiverEmail();
    int SetData();
};