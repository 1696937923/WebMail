#include "UserUi.h"
#include "ui_userui.h"
#include "WebClient.h"

UserUi::UserUi(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UserUi)
{
    ui->setupUi(this);
    this->setStyleSheet("background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(251,102,102, 200), stop:1 rgba(20,196,188, 210));");


    ui->EmailBtn->setStyleSheet("QPushButton{font: 25 13pt '微软雅黑 Light';color: rgb(255,255,255);background-color: rgb(20,196,188);"
                                "border: none;border-radius:15px;}"
                                "QPushButton:hover{background-color: rgb(22,218,208);}"//hover
                                "QPushButton:pressed{background-color: rgb(17,171,164);}");//pressed
    ui->SendBtn->setStyleSheet("QPushButton{font: 25 13pt '微软雅黑 Light';color: rgb(255,255,255);background-color: rgb(20,196,188);"
                               "border: none;border-radius:15px;}"
                               "QPushButton:hover{background-color: rgb(22,218,208);}"
                               "QPushButton:pressed{background-color: rgb(17,171,164);}");
    ui->LogoutBtn->setStyleSheet("QPushButton{font: 25 13pt '微软雅黑 Light';color: rgb(255,255,255);background-color: rgb(20,196,188);"
                                "border: none;border-radius:15px;}"
                                "QPushButton:hover{background-color: rgb(22,218,208);}"
                                "QPushButton:pressed{background-color: rgb(17,171,164);}");

    ui->graphicsView->setStyleSheet("background-color: rgb(255, 255, 255);border-radius:15px;");
    ui->label->setStyleSheet("background-color: rgb(255, 255, 255);");
    ui->label_2->setStyleSheet("background-color: rgb(255, 255, 255);");
    ui->label_3->setStyleSheet("background-color: rgb(255, 255, 255);");
    ui->info_label->setStyleSheet("background-color: rgb(255, 255, 255);");
    ui->receiver->setStyleSheet("background-color: rgb(255, 255, 255);");
    ui->subject->setStyleSheet("background-color: rgb(255, 255, 255);");
    ui->content->setStyleSheet("background-color: rgb(255, 255, 255);");

    timer = new QTimer(this);
    connect(this->timer, &QTimer::timeout, [=](){
        ui->info_label->hide();
        this->timer->stop();
    });
}

UserUi::~UserUi()
{
    delete ui;
}

void UserUi::ReciveUserUiShow(int login_fd, const char * user, const char * passwd, int email_num)
{
    memset(this->CurrentUser, 0, sizeof(this->CurrentUser));
    strncpy(this->CurrentUser, user, strlen(user));
    memset(this->Password, 0, sizeof(this->Password));
    strncpy(this->Password, passwd, strlen(passwd));

    this->login_fd = login_fd;
    this->show();

    ui->EmailBtn->setText(QString("收件箱(%1)").arg(email_num));
}

void UserUi::ReciveBackUiShow()
{
    this->show();
}

void UserUi::ReciveNewEmail()
{
    std::string str = ui->EmailBtn->text().toStdString();
    int index1 = str.find_first_of('(');
    int index2 = str.find_last_of(')');
    int num = atoi(str.substr(index1+1, index2-index1-1).c_str())+1;

    ui->EmailBtn->setText(QString("收件箱(%1)").arg(num));
}

void UserUi::on_SendBtn_clicked()
{
    WebClient webclient;
    MyStruct::EMAIL email;

    memset(&email, 0, sizeof(MyStruct::EMAIL));
    email.set_sender(this->CurrentUser);
    email.set_receiver(ui->receiver->text().toStdString());
    email.set_subject(ui->subject->text().toStdString());
    email.set_date("");
    email.set_content(ui->content->toPlainText().toStdString());

    int socket_fd = webclient.ConnectSmtp();
    if (socket_fd < 0){
        ui->info_label->show();
        ui->info_label->setText("发送失败");
        ui->info_label->setStyleSheet("color: rgb(255,0,0);""background-color: rgb(255, 255, 255);");
        timer->start(2000);

        return;
    }

    int nagle_status = 1; // 1 - on, 0 - off
    setsockopt(socket_fd, IPPROTO_TCP, TCP_NODELAY, (char *)&nagle_status, sizeof(int));

    int ret = webclient.Send_Email(socket_fd, email, this->Password);
    if(ret < 0){
        ui->info_label->show();
        ui->info_label->setText("发送失败");
        ui->info_label->setStyleSheet("color: rgb(255,0,0);""background-color: rgb(255, 255, 255);");
        timer->start(2000);

        return;
    }

    ui->subject->clear();
    ui->content->clear();

    ui->info_label->show();
    ui->info_label->setText("发送成功");
    ui->info_label->setStyleSheet("color: rgb(0,255,0);""background-color: rgb(255, 255, 255);");
    timer->start(2000);
}

void UserUi::on_LogoutBtn_clicked()
{
    WebClient webclient;
    struct LOGOUT_INFO logout_info;

    memset(&logout_info, 0, sizeof(LOGOUT_INFO));
    strncpy(logout_info.email, this->CurrentUser, strlen(this->CurrentUser));

    int ret = webclient.Logout(this->login_fd, logout_info);
    if(ret < 0){
        ui->info_label->show();
        ui->info_label->setText("退出失败");
        ui->info_label->setStyleSheet("color: rgb(255,0,0);""background-color: rgb(255, 255, 255);");
        timer->start(2000);

        return;
    }

    memset(this->CurrentUser, 0, sizeof(this->CurrentUser));
    memset(this->Password, 0, sizeof(this->Password));

    this->hide();
    emit ShowMainUi();
}

void UserUi::on_EmailBtn_clicked()
{
    ui->EmailBtn->setText("收件箱(0)");

    this->hide();
    emit ShowEmailUi(this->CurrentUser);
}
