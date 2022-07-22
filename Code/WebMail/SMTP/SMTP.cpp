#include "SMTP.h"
#include <iostream>
#include <string.h>

using namespace std;

int SMTP::ReturnMessage(int socket_fd, int code, const char *msg)
{
    int ret;
    char c_code[4];
    sprintf(c_code, "%d", code);

    //返回状态码
    ret = send(socket_fd, c_code, sizeof(c_code), 0);
    if (ret < 0)
    {
        return -1;
    }

    //返回具体信息
    ret = send(socket_fd, msg, strlen(msg) + 1, 0);
    if (ret < 0)
    {
        return -1;
    }

    return 1;
}

int SMTP::RecvMessage(int socket_fd)
{
}

int SMTP::ConnectReturn(int socket_fd)
{
    int ret = ReturnMessage(socket_fd, 220, "Connect ok!\n");
    if (ret < 0)
    {
        std::cout << "[INFO]Server: Return Connect message error!" << std::endl;
    }
    std::cout << "[INFO]Server: Return Connect message success!" << std::endl;
}

int main()
{
    SMTP smtp;
    smtp.ReturnMessage(1, 220, "m");
}