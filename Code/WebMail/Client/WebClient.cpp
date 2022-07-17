#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h> //sockaddr_in函数库
#include <arpa/inet.h>  //inet_addr函数库
#include <thread>
using namespace std;

#define BUFFERSIZE 1024

const char *Host = "127.0.0.1";
const uint16_t Sock_Port = 6789;

struct LOGIN_INFO;    //登录信息
struct REGISTER_INFO; //注册信息
struct QUERY_INFO;    //查询邮件信息
struct PULL_INFO;     //拉取邮件信息
struct BACK_INFO;     //返回信息
struct EMAIL;         //邮件信息

class WebClient
{
public:
    void Ui();

private:
    char buffer[BUFFERSIZE];
    int Connect();
    void Recv(int, BACK_INFO &);
    int Login(struct LOGIN_INFO);
    int Register(struct REGISTER_INFO);
    int Send_Email(struct EMAIL);
    int Query_Email(struct QUERY_INFO);
    int Pull_Email(struct PULL_INFO);
    static void ListeningEmail(int);
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

void WebClient::Ui()
{
    string input;
    while (1)
    {
        cout << "please input: ";
        cin >> input;
        if (input == "login")
        {
            struct LOGIN_INFO login_info;
            strncpy(login_info.email, "1@email.com", 12);
            strncpy(login_info.password, "123456", 7);
            Login(login_info);
        }
        else if (input == "register")
        {
            struct REGISTER_INFO register_info;
            strncpy(register_info.email, "6@email.com", 12);
            strncpy(register_info.password, "123456", 7);
            Register(register_info);
        }
        else if (input == "send")
        {
            struct EMAIL email;
            strncpy(email.sender, "1@email.com", 48);
            strncpy(email.receiver, "1@email.com", 48);
            strncpy(email.subject, "你好呀", 128);
            strncpy(email.date, "2022-7-16", 32);
            strncpy(email.message, "hello", 512);
            for (int i = 0; i < 1; i++)
            {
                Send_Email(email);
            }
        }
    }
}

void WebClient::Recv(int socket_fd, BACK_INFO &back_info)
{
    memset(buffer, 0, sizeof(buffer));
    recv(socket_fd, buffer, sizeof(BACK_INFO), 0);
    memcpy(&back_info, buffer, sizeof(BACK_INFO));
}

int WebClient::Connect()
{
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in s_addr;
    s_addr.sin_family = AF_INET;
    s_addr.sin_addr.s_addr = inet_addr(Host);
    s_addr.sin_port = htons(Sock_Port);
    connect(socket_fd, (struct sockaddr *)&s_addr, sizeof(s_addr));

    struct BACK_INFO back_info;
    Recv(socket_fd, back_info);
    cout << back_info.info << endl;

    if (back_info.code < 0)
        return -1;
    return socket_fd;
}

int WebClient::Login(struct LOGIN_INFO login_info)
{
    int socket_fd = Connect();
    send(socket_fd, "Login", 6, 0);

    struct BACK_INFO back_info;
    Recv(socket_fd, back_info);
    cout << back_info.info << endl;
    if (back_info.code < 0)
        return -1;

    send(socket_fd, &login_info, sizeof(LOGIN_INFO), 0);
    Recv(socket_fd, back_info);
    cout << back_info.info << endl;

    if (back_info.code < 0)
        return -1;
    else
    {
        thread th(ListeningEmail, socket_fd);
        th.detach();
        return 1;
    }
}

int WebClient::Register(struct REGISTER_INFO register_info)
{
    int socket_fd = Connect();
    send(socket_fd, "Register", 9, 0);

    struct BACK_INFO back_info;
    Recv(socket_fd, back_info);
    cout << back_info.info << endl;
    if (back_info.code < 0)
        return -1;

    send(socket_fd, &register_info, sizeof(REGISTER_INFO), 0);
    Recv(socket_fd, back_info);
    cout << back_info.info << endl;

    close(socket_fd);
    if (back_info.code < 0)
        return -1;
    else
        return 1;
}

int WebClient::Send_Email(struct EMAIL email)
{
    int socket_fd = Connect();
    send(socket_fd, "Send Email", 11, 0);

    struct BACK_INFO back_info;
    Recv(socket_fd, back_info);
    cout << back_info.info << endl;
    if (back_info.code < 0)
        return -1;

    send(socket_fd, &email, sizeof(EMAIL), 0);
    Recv(socket_fd, back_info);
    cout << back_info.info << endl;
    close(socket_fd);
    if (back_info.code < 0)
        return -1;
    else
        return 1;
}

int WebClient::Query_Email(struct QUERY_INFO query_info)
{
}

int WebClient::Pull_Email(struct PULL_INFO pull_info)
{
}

void WebClient::ListeningEmail(int socket_fd)
{
    char buffer[16];
    while (1) //循环接受服务器端消息
    {
        memset(buffer, 0, sizeof(buffer));
        int ret = recv(socket_fd, buffer, sizeof(buffer), 0);
        if (ret <= 0)
        {
            cout << "(INFO):Server Close!!" << endl;
            return;
        }
        cout << "(INFO)Receive an email: " << buffer << endl;
    }
}

int main()
{
    WebClient wc;
    wc.Ui();
    return 0;
}