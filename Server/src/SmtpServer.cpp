#include "SmtpServer.h"

#include <iostream>
#include <string.h>
#include "AES.cpp"

int SMTPSERVER::ReturnToClient(int socket_fd, int code, const char *msg)
{
    int ret;
    char back_msg[128], data[128];
    sprintf(back_msg, "%d %s\n", code, msg);
    sprintf(data, "%d %s", code, msg);

    //返回状具体信息
    ret = send(socket_fd, back_msg, strlen(back_msg), 0);
    if (ret < 0)
    {
        std::cout << "[INFO]Server: Return SMTP message->[" << data << "] error!" << std::endl;
        return -1;
    }

    std::cout << "[INFO]SMTP Server: Return SMTP message->[" << data << "] success!" << std::endl;
    return 1;
}

int SMTPSERVER::RecvFromClient(int socket_fd, char *data, size_t size, int type)
{
    char buffer[BUFFERSIZE] = {0};
    memset(buffer, 0, sizeof(buffer));
    memset(data, 0, size);

    int ret = recv(socket_fd, buffer, size, 0);
    if (ret <= 0)
    {
        std::cout << "[INFO]Server: Recv SMTP message error!" << std::endl;
        return -1;
    }

    std::string tmp(buffer);
    if (type == 1 && tmp.back() == '\n')
        strncpy(data, buffer, strlen(buffer) - 2);
    else
        strncpy(data, buffer, strlen(buffer));

    if (strncmp(data, QUIT, strlen(QUIT)) == 0)
    {
        std::cout << "[INFO]Server: SMTP Client Quit!" << std::endl;
        return 0;
    }

    return ret;
}

int SMTPSERVER::RecvCommand(int socket_fd, char *tag, char *msg)
{
    char buffer[80];
    memset(tag, 0, 16);
    memset(msg, 0, 64);

    int ret = RecvFromClient(socket_fd, buffer, sizeof(buffer), 1);
    if (ret <= 0)
        return ret;

    std::string tmp = buffer;
    int index = tmp.find_first_of(':');
    if (index == -1)
    {
        //只有命令，没有内容
        strncpy(tag, tmp.c_str(), 16);
        return 1;
    }
    else
    {
        //既有命令，又有内容
        strncpy(tag, tmp.substr(0, index).c_str(), 16);
        strncpy(msg, tmp.substr(index).c_str(), 64);
    }

    return 2;
}

void SMTPSERVER::ParseEmail(std::string data, char *subject, char *content)
{
    int index = data.find_first_of("\r\n");
    if (index == -1)
        return;

    std::string first_line = data.substr(0, index);
    int m_index = first_line.find_first_of(":");
    if (m_index != -1 && strncmp("subject", first_line.substr(0, m_index).c_str(), m_index) == 0)
    {
        int len = first_line.length() - m_index - 1;
        strncpy(subject, first_line.substr(m_index + 1, len).c_str(), len);
        strncpy(content, data.substr(index + 2).c_str(), data.length() - index - 2);
    }
    else
        strncpy(content, data.c_str(), data.length());
}

int SMTPSERVER::RecvEmailData(int socket_fd, char *subject, char *content, std::string Aes_Key)
{
    AES aes;
    char buffer[600] = {0};
    std::string data;

    int ret = RecvFromClient(socket_fd, buffer, sizeof(buffer), 0);
    if (ret <= 0)
        return ret;

    while (strncmp(".\r\n", buffer, strlen(buffer)) != 0)
    {
        data.append(buffer, strlen(buffer));
        if (data.length() >= 5 && strncmp("\r\n.\r\n", data.substr(data.length() - 5).c_str(), 5) == 0)
        {
            data = data.substr(0, data.length() - 3);
            break;
        }

        ret = RecvFromClient(socket_fd, buffer, sizeof(buffer), 0);
        if (ret <= 0)
            return ret;
    }

    std::string aes_data = aes.Decrypt(data.c_str(), Aes_Key.c_str());
    ParseEmail(aes_data, subject, content);
    return 1;
}

int SMTPSERVER::GetEmailFromCmd(char *in, char *out, size_t size)
{
    std::string tmp = in;
    memset(out, 0, size);

    int index1 = tmp.find_first_of('<');
    int index2 = tmp.find_last_of('>');
    if (index1 == -1 || index2 == -1)
        return -1;

    strncpy(out, tmp.substr(index1 + 1, index2 - index1 - 1).c_str(), index2 - index1 - 1);
    return 1;
}

int SMTPSERVER::Deal_Connect(int socket_fd)
{
    ReturnToClient(socket_fd, 220, "Connect ok!");
}

int SMTPSERVER::Deal_Helo(int socket_fd)
{
    int ret;
    ret = RecvCommand(socket_fd, Msg_Tag, Msg);
    if (ret <= 0)
        return ret;

    while (strncmp(HELO, Msg_Tag, strlen(Msg_Tag)) != 0)
    {
        ReturnToClient(socket_fd, 503, "Send helo first!");
        ret = RecvCommand(socket_fd, Msg_Tag, Msg);
        if (ret <= 0)
            return ret;
    }

    ReturnToClient(socket_fd, 250, "Helo ok!"); //返回helo
    return 1;
}

int SMTPSERVER::Deal_AuthLogin(int socket_fd, Mysql mysql, MyStruct::EMAIL *email, std::string Aes_Key)
{
    AES aes;
    int ret;
    while (1)
    {
        ret = RecvCommand(socket_fd, Msg_Tag, Msg);
        if (ret <= 0)
            return ret;

        while (strncmp(AUTHLOGIN, Msg_Tag, strlen(Msg_Tag)) != 0)
        {
            ReturnToClient(socket_fd, 503, "Command error!");
            ret = RecvCommand(socket_fd, Msg_Tag, Msg);
            if (ret <= 0)
                return ret;
        }

        //接受用户名
        ReturnToClient(socket_fd, 334, "Input username:");
        ret = RecvFromClient(socket_fd, username, 48, 1);
        if (ret <= 0)
            return ret;

        //接受密码
        ReturnToClient(socket_fd, 334, "Input password:");
        ret = RecvFromClient(socket_fd, password, 48, 1);
        if (ret <= 0)
            return ret;

        std::string aes_password = aes.Decrypt(password, Aes_Key.c_str());
        std::cout << username << aes_password << std::endl;
        //账号密码验证
        ret = mysql.Login(username, aes_password.c_str());
        if (ret < 0)
        {
            ReturnToClient(socket_fd, 535, "Authentication error");
            return -2;
        }
        else
        {
            email->clear_sender();
            email->set_sender(username);

            ReturnToClient(socket_fd, 235, "Authentication successful");
            break;
        }
    }

    return 1;
}

int SMTPSERVER::Deal_MailFrom(int socket_fd, MyStruct::EMAIL email)
{
    int ret;
    while (1)
    {
        ret = RecvCommand(socket_fd, Msg_Tag, Msg); //接收mail from
        if (ret <= 0)
            return ret;

        while (strncmp(MAILFROM, Msg_Tag, strlen(Msg_Tag)) != 0)
        {
            ReturnToClient(socket_fd, 503, "Command error!");
            ret = RecvCommand(socket_fd, Msg_Tag, Msg);
            if (ret <= 0)
                return ret;
        }

        ret = GetEmailFromCmd(Msg, sender, sizeof(sender)); //获取发送者邮箱
        if (ret == 1 && strncmp(email.sender().c_str(), sender, strlen(sender)) == 0)
        {
            ReturnToClient(socket_fd, 250, "Mail ok!");
            break;
        }
        else
            ReturnToClient(socket_fd, 553, "Mail is different from login mail!");
    }
    return 1;
}

int SMTPSERVER::Deal_RcptTo(int socket_fd, Mysql mysql, MyStruct::EMAIL *email)
{
    int ret;
    while (1)
    {
        ret = RecvCommand(socket_fd, Msg_Tag, Msg); //接收rcpt to
        if (ret <= 0)
            return ret;

        while (strncmp(RCPTTO, Msg_Tag, strlen(Msg_Tag)) != 0)
        {
            ReturnToClient(socket_fd, 503, "Command error!");
            ret = RecvCommand(socket_fd, Msg_Tag, Msg);
            if (ret <= 0)
                return ret;
        }

        ret = GetEmailFromCmd(Msg, receiver, sizeof(receiver)); //获取接受者邮箱
        if (ret == 1 && mysql.QueryUser(receiver) == 1)
        {
            email->clear_receiver();
            email->set_receiver(receiver);

            ReturnToClient(socket_fd, 250, "Mail ok!");
            break;
        }
        else
        {
            ReturnToClient(socket_fd, 550, "User is not exits!");
            return -1;
        }
    }
    return 1;
}

int SMTPSERVER::Deal_Data(int socket_fd, Mysql mysql, Redis redis, MyStruct::EMAIL *email, std::string Aes_Key)
{
    char time[32];
    int ret = RecvCommand(socket_fd, Msg_Tag, Msg); //接收data指令
    if (ret <= 0)
        return -1;

    while (strncmp(DATA, Msg_Tag, strlen(Msg_Tag)) != 0)
    {
        ReturnToClient(socket_fd, 503, "Command error!");
        ret = RecvCommand(socket_fd, Msg_Tag, Msg);
        if (ret <= 0)
            return -1;
    }
    ReturnToClient(socket_fd, 354, "End data with <CR><LF>.<CR><LF>");

    //从data中解析出subject和content
    memset(subject, 0, sizeof(subject));
    memset(content, 0, sizeof(content));
    ret = RecvEmailData(socket_fd, subject, content, Aes_Key);
    if (ret <= 0)
        return -1;

    GetLocalTime(time);
    email->set_subject(subject);
    email->set_content(content);
    email->set_date(time);

    //将解析出来的邮件发送除出去: 1.存入Mysql 2.存入Redis
    ret = mysql.Insert_Email(redis, *email);
    if (ret < 0)
    {
        ReturnToClient(socket_fd, 255, "Send email error!");
        return -2;
    }

    ReturnToClient(socket_fd, 250, "Send email successful!");
    return 1;
}

void SMTPSERVER::GetLocalTime(char *local_time)
{
    time_t t = time(NULL);
    struct tm *local = localtime(&t);

    strftime(local_time, 32, "%Y年%m月%d %H:%M:%S", local);
}
