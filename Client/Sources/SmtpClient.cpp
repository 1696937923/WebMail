#include "SmtpClient.h"

#include <iostream>
#include <string.h>
#include "AES.h"

int SMTPCLIENT::SendSmtpEmail(int socket_fd, MyStruct::EMAIL email, char *password, std::string Aes_Key)
{
    if (Do_Helo(socket_fd) <= 0 || Do_AuthLogin(socket_fd, email, password, Aes_Key) <= 0 ||
        Do_MailFrom(socket_fd, email) <= 0 || Do_RcptTo(socket_fd, email) <= 0 ||
        Do_Data(socket_fd, email, Aes_Key) <= 0)
    {
        std::cout << "[INFO]Smtp Client: Send Email error!" << std::endl;
        return -1;
    }

    return 1;
}

int SMTPCLIENT::ParseBackMsg(char *msg)
{
    std::string tmp = msg;
    int index = tmp.find_first_of('  ');
    if (index == -1)
        return -1;

    int code = atoi(tmp.substr(0, index).c_str());
    std::cout << "code:" << code << std::endl;
    return code;
}

int SMTPCLIENT::SendToServer(int socket_fd, const char *msg)
{
    char buffer[BUFFERSIZE] = {0};
    sprintf(buffer, "%s\r\n", msg);

    int ret = send(socket_fd, buffer, strlen(buffer), 0);
    if (ret <= 0)
    {
        std::cout << "[INFO]Smtp Client: Send SMTP message error!" << std::endl;
        return -1;
    }

    return 1;
}

int SMTPCLIENT::RecvFromServer(int socket_fd, char *msg, size_t size)
{
    char buffer[BUFFERSIZE];

    memset(buffer, 0, sizeof(buffer));
    int ret = recv(socket_fd, buffer, sizeof(buffer), 0);
    if (ret <= 0)
    {
        std::cout << "[INFO]Smtp Client: Recv SMTP message error!" << std::endl;
        return -1;
    }

    memset(msg, 0, size);
    strncpy(msg, buffer, strlen(buffer));
    std::cout << "[INFO]Smtp Client: Recv Msg from Smtp server->" << buffer << "" << std::endl;
    return 1;
}

int SMTPCLIENT::Do_Helo(int socket_fd)
{
    int ret = SendToServer(socket_fd, HELO);
    if (ret <= 0)
        return -1;

    char buffer[BUFFERSIZE];
    ret = RecvFromServer(socket_fd, buffer, sizeof(buffer));
    if (ret <= 0)
        return -1;

    int code = ParseBackMsg(buffer);
    if (code != 250)
    {
        std::cout << "[INFO]Smtp Client: Helo error!" << std::endl;
        return -1;
    }

    return 1;
}

int SMTPCLIENT::Do_AuthLogin(int socket_fd, MyStruct::EMAIL email, char *password, std::string Aes_Key)
{
    int ret = SendToServer(socket_fd, AUTHLOGIN);
    if (ret <= 0)
        return -1;

    char buffer[BUFFERSIZE];
    ret = RecvFromServer(socket_fd, buffer, sizeof(buffer)); // recv:username
    if (ret <= 0)
        return -1;

    ret = SendToServer(socket_fd, (char *)email.sender().c_str()); // send:username
    if (ret <= 0)
        return -1;

    ret = RecvFromServer(socket_fd, buffer, sizeof(buffer)); // recv:password
    if (ret <= 0)
        return -1;

    AES aes;
    std::string aes_password = aes.Encrypt(password, Aes_Key.c_str());
    ret = SendToServer(socket_fd, aes_password.c_str()); // send:password
    if (ret <= 0)
        return -1;

    ret = RecvFromServer(socket_fd, buffer, sizeof(buffer)); // recv: login back
    if (ret <= 0)
        return -1;

    int code = ParseBackMsg(buffer);
    if (code != 235)
    {
        std::cout << "[INFO]Smtp Client: AuthLogin error!" << std::endl;
        return -1;
    }

    return 1;
}

int SMTPCLIENT::Do_MailFrom(int socket_fd, MyStruct::EMAIL email)
{
    char buffer[BUFFERSIZE];
    memset(buffer, 0, sizeof(buffer));
    sprintf(buffer, "mail from:<%s>", email.sender().c_str());

    int ret = SendToServer(socket_fd, buffer);
    if (ret <= 0)
        return -1;

    ret = RecvFromServer(socket_fd, buffer, sizeof(buffer));
    if (ret <= 0)
        return -1;

    int code = ParseBackMsg(buffer);
    if (code != 250)
    {
        std::cout << "[INFO]Smtp Client: MailFrom error!" << std::endl;
        return -1;
    }

    return 1;
}

int SMTPCLIENT::Do_RcptTo(int socket_fd, MyStruct::EMAIL email)
{
    char buffer[BUFFERSIZE];
    memset(buffer, 0, sizeof(buffer));
    sprintf(buffer, "rcpt to:<%s>", email.receiver().c_str());

    int ret = SendToServer(socket_fd, buffer);
    if (ret <= 0)
        return -1;

    ret = RecvFromServer(socket_fd, buffer, sizeof(buffer));
    if (ret <= 0)
        return -1;

    int code = ParseBackMsg(buffer);
    if (code != 250)
    {
        std::cout << "[INFO]Smtp Client: RcptTo error!" << std::endl;
        return -1;
    }

    return 1;
}

int SMTPCLIENT::Do_Data(int socket_fd, MyStruct::EMAIL email, std::string Aes_Key)
{
    int ret = SendToServer(socket_fd, DATA);
    if (ret <= 0)
        return -1;

    char buffer[BUFFERSIZE];
    ret = RecvFromServer(socket_fd, buffer, sizeof(buffer));
    if (ret <= 0)
        return -1;

    int code = ParseBackMsg(buffer);
    if (code != 354)
    {
        std::cout << "[INFO]Smtp Client: Data error!" << std::endl;
        return -1;
    }

    char content[1024] = {0};
    AES aes;
    snprintf(content, sizeof(content), "subject:%s\r\n%s", email.subject().c_str(), email.content().c_str());
    std::string aes_content = aes.Encrypt(content, Aes_Key.c_str());
    ret = SendToServer(socket_fd, aes_content.c_str()); // send email subject
    if (ret <= 0)
        return -1;

    ret = SendToServer(socket_fd, "."); // send .
    if (ret <= 0)
        return -1;

    ret = RecvFromServer(socket_fd, buffer, sizeof(buffer));
    if (ret <= 0)
        return -1;

    code = ParseBackMsg(buffer);
    if (code != 250)
    {
        std::cout << "[INFO]Smtp Client: Data error!" << std::endl;
        return -1;
    }

    return 1;
}
