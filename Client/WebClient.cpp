#include <iostream>
#include <string>

#include "WebClient.h"
#include "EmailUi.h"
#include "ui_email.h"
#include "MainWindowUi.h"

#include "ExchangeKey.cpp"
#include "AES.cpp"

const char * Host = "127.0.0.1";
const uint16_t Sock_Port = 6789;
const uint16_t Smtp_Port = 6666;
int count = 1;

int WebClient::Recv(int socket_fd, BACK_INFO &back_info)
{
    char buffer[BUFFERSIZE];
    memset(buffer, 0, sizeof(buffer));

    int ret = recv(socket_fd, buffer, sizeof(BACK_INFO), 0);
    if (ret <= 0)
    {
        std::cout << "[INFO]Client: Recv Msg error!" << std::endl;
        return -1;
    }

    memset(&back_info, 0, sizeof(BACK_INFO));
    memcpy(&back_info, buffer, sizeof(BACK_INFO));
    std::cout << "[INFO]Client: Recv Msg from server->[" << back_info.info << "]!" << std::endl;
    return 1;
}

template <typename T>
int WebClient::Send(int socket_fd, T msg)
{
    int ret = send(socket_fd, &msg, sizeof(T), 0);
    if (ret < 0)
    {
        std::cout << "[INFO]Client: Send Msg error!" << std::endl;
        return -1;
    }
    return 1;
}

int WebClient::Connect()
{
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in s_addr;
    s_addr.sin_family = AF_INET;
    s_addr.sin_addr.s_addr = inet_addr(Host);
    s_addr.sin_port = htons(Sock_Port);

    int ret = connect(socket_fd, (struct sockaddr *)&s_addr, sizeof(s_addr));
    if (ret < 0)
    {
        std::cout << "[INFO]Client: Connect Server error!" << std::endl;
        return -1;
    }

    struct BACK_INFO back_info;
    ret = Recv(socket_fd, back_info);
    if (ret < 0 || back_info.code < 0)
        return -1;

    return socket_fd;
}

int WebClient::ConnectSmtp()
{
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in s_addr;
    s_addr.sin_family = AF_INET;
    s_addr.sin_addr.s_addr = inet_addr(Host);
    s_addr.sin_port = htons(Smtp_Port);

    int ret = connect(socket_fd, (struct sockaddr *)&s_addr, sizeof(s_addr));
    if (ret < 0)
    {
        std::cout << "[INFO]Client: Connect Smtp Server error!" << std::endl;
        return -1;
    }

    char buffer[BUFFERSIZE];
    memset(buffer, 0, sizeof(buffer));

    ret = recv(socket_fd, buffer, sizeof(buffer), 0);
    if (ret <= 0)
        return -1;

    return socket_fd;
}

int WebClient::SendTag(int socket_fd, char *tag, size_t size)
{
    int ret = send(socket_fd, tag, size, 0);
    if (ret < 0)
    {
        std::cout << "[INFO]Client: Send Tag error!" << std::endl;
        return -1;
    }

    struct BACK_INFO back_info;
    ret = Recv(socket_fd, back_info);
    if (ret < 0 || back_info.code == BACKFAIL_CODE)
        return -1;

    return 1;
}

int WebClient::Login(int socket_fd, struct LOGIN_INFO login_info, void *Ui)
{
    int ret = SendTag(socket_fd, LOGIN, strlen(LOGIN)); //发送tag
    if (ret < 0)
        return -1;

    ret = Send(socket_fd, login_info); //发送登录信息
    if (ret < 0)
        return -1;

    struct BACK_INFO back_info;
    ret = Recv(socket_fd, back_info); //接受登录返回消息
    if (ret < 0 || back_info.code == BACKFAIL_CODE)
        return -1;

    ret = Recv(socket_fd, back_info); //接受登录返回消息
    if (ret < 0)
        return -1;
    int unread_num = back_info.code;

    std::thread th(ListeningEmail, this, socket_fd, Ui); //开启一个线程
    th.detach();

    return unread_num;
}

int WebClient::Logout(int socket_fd, struct LOGOUT_INFO logout_info)
{
    int ret = send(socket_fd, LOGOUT, strlen(LOGOUT), 0);
    if (ret < 0)
        return -1;

    ret = Send(socket_fd, logout_info);
    if (ret < 0)
        return -1;

    return 1;
}

int WebClient::Register(int socket_fd, struct REGISTER_INFO register_info)
{
    int ret = SendTag(socket_fd, REGISTER, strlen(REGISTER)); //发送tag
    if (ret < 0)
        return -1;

    ret = Send(socket_fd, register_info); //发送登录信息
    if (ret < 0)
        return -1;

    struct BACK_INFO back_info;
    ret = Recv(socket_fd, back_info);
    if (ret < 0 || back_info.code == BACKFAIL_CODE)
        return -1;

    return 1;
}

int WebClient::Pull_Email(int socket_fd, struct PULL_INFO pull_info, void *Ui)
{
    AES aes;
    ExchangeKey ex;

    int ret = SendTag(socket_fd, PULL, strlen(PULL)); //发送tag
    if (ret < 0)
        return -1;

    std::string Aes_Key = ex.ExchangeAsClient(socket_fd);

    ret = Send(socket_fd, pull_info); //发送pull信息
    if (ret < 0)
        return -1;

    struct BACK_INFO back_info;
    ret = Recv(socket_fd, back_info);
    //如果ret<0，则代表接受失败，若大于等于0，则表示未读邮件数量
    if (ret < 0 || back_info.code == BACKFAIL_CODE)
        return -1;

    int emial_num = atoi(back_info.info);
    for (int i = 0; i < emial_num; i++)
    {
        ret = Recv(socket_fd, back_info);
        if (back_info.code == PULLEMAIL_CODE)
        {
            std::string aes_email = aes.Decrypt(back_info.info, Aes_Key.c_str());
            std::string email_str = bs64.Base64Decode(aes_email);
            MyStruct::EMAIL email = se.unserialize<MyStruct::EMAIL>(email_str);
            //显示email
            Display_Email(email, Ui);
        }
    }

    return 1;
}

void WebClient::Display_Email(MyStruct::EMAIL email, void *Ui)
{
    Ui::Email *ui = (Ui::Email *)Ui;
    QListWidgetItem * item = new QListWidgetItem;
    QColor qc;
    qc.setRgb(0,255,255);

    char data[1024] = {0};
    sprintf(data, "主题：%s\n发件人：%s\n时间：%s\n邮件正文：%s"
                 "\n                                                "
                 "                                                 "
                 "                                                 "
                 "#%d", email.subject().c_str(), email.sender().c_str(),
                        email.date().c_str(), email.content().c_str(), count);

    item->setText(QString(data)); //设置内容
    item->setBackgroundColor(qc);

    ui->listWidget->insertItem(0, item);//最后将初始化好的子项插入到listWidget控件中
    count++;
}

void WebClient::ListeningEmail(WebClient *webclient, int socket_fd, void *Ui)
{
    std::cout << "------------start listening for email---------------" << std::endl;
    MainWindow *mw = (MainWindow *)Ui;
    while (1) //循环接受服务器端消息
    {
        struct BACK_INFO back_info;
        int ret = webclient->Recv(socket_fd, back_info);
        if (ret <= 0)
        {
            std::cout << "[INFO]Client: Server close!" << std::endl;
            return;
        }
        else if (back_info.code == NEWEMAIL_CODE)
        {
            std::cout << "[INFO]Client: Receive an email!" << std::endl;
            emit mw->NewEmail();
        }
    }
    std::cout << "------------end listening for email---------------" << std::endl;
}

int WebClient::Send_Email(int socket_fd, MyStruct::EMAIL email, const char * password)
{
    ExchangeKey ex;
    std::string Aes_Key = ex.ExchangeAsClient(socket_fd);

    int ret = smtp.SendSmtpEmail(socket_fd, email, (char *)password, Aes_Key);
    return ret;
}
