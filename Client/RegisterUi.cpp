#include "RegisterUi.h"
#include "ui_registerui.h"
#include "WebClient.h"

RegisterUi::RegisterUi(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RegisterUi)
{
    ui->setupUi(this);
    this->setStyleSheet("background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(251,102,102, 200), stop:1 rgba(20,196,188, 210));");

    ui->password->setStyleSheet("font: 25 13pt '微软雅黑 Light';" //字体
                                "color: rgb(31,31,31);"		//字体颜色
                                "padding-left:15px;"       //内边距-字体缩进
                                "background-color: rgb(255, 255, 255);" //背景颜色
                                "border:2px solid rgb(20,196,188);border-radius:15px;");//边框粗细-颜色-圆角设置
    ui->email->setStyleSheet("font: 25 13pt '微软雅黑 Light';color: rgb(31,31,31);"
                             "padding-left:15px;background-color: rgb(255, 255, 255);"
                             "border:2px solid rgb(20,196,188);border-radius:15px;");
    ui->RegisterBtn->setStyleSheet("QPushButton{font: 25 13pt '微软雅黑 Light';color: rgb(255,255,255);background-color: rgb(20,196,188);"
                                    "border: none;border-radius:15px;}"
                                    "QPushButton:hover{background-color: rgb(22,218,208);}"//hover
                                    "QPushButton:pressed{background-color: rgb(17,171,164);}");//pressed
    ui->BackBtn->setStyleSheet("QPushButton{font: 25 13pt '微软雅黑 Light';color: rgb(255,255,255);background-color: rgb(20,196,188);"
                                    "border: none;border-radius:15px;}"
                                    "QPushButton:hover{background-color: rgb(22,218,208);}"//hover
                                    "QPushButton:pressed{background-color: rgb(17,171,164);}");//pressed
    ui->graphicsView->setStyleSheet("background-color: rgb(255, 255, 255);border-radius:15px;");
    ui->label->setStyleSheet("background-color: rgb(255, 255, 255);");
    ui->info_label->setStyleSheet("background-color: rgb(255, 255, 255);");

    timer = new QTimer(this);
    connect(this->timer, &QTimer::timeout, [=](){
        ui->info_label->hide();
        this->timer->stop();
    });
}

RegisterUi::~RegisterUi()
{
    delete ui;
}

void RegisterUi::ReciveRegisterUiShow()
{
    this->show();
}

void RegisterUi::on_BackBtn_clicked()
{
    this->hide();
    emit ShowMainUi();
}

void RegisterUi::on_RegisterBtn_clicked()
{
    WebClient webclient;
    struct REGISTER_INFO register_info;

    QString email = ui->email->text();
    QString password = ui->password->text();

    if(email.toStdString().find_first_of('@') == -1 || password.length() < 6)
    {
        ui->info_label->setText("用户名或密码不合法");
        ui->info_label->setStyleSheet("color: rgb(255,0,0);""background-color: rgb(255, 255, 255);");
        return ;
    }

    memset(&register_info, 0, sizeof(REGISTER_INFO));
    strncpy(register_info.email, email.toStdString().c_str(), email.length());
    strncpy(register_info.password, password.toStdString().c_str(), password.length());

    int socket_fd = webclient.Connect();
    if (socket_fd < 0){
        ui->info_label->show();
        ui->info_label->setText("注册失败");
        ui->info_label->setStyleSheet("color: rgb(255,0,0);""background-color: rgb(255, 255, 255);");
        this->timer->start(2000);

        return;
    }

    int ret = webclient.Register(socket_fd, register_info);
    if(ret == -1){
        ui->info_label->show();
        ui->info_label->setText("用户已存在");
        ui->info_label->setStyleSheet("color: rgb(255,0,0);""background-color: rgb(255, 255, 255);");
        this->timer->start(2000);

        return;
    }
    ui->info_label->show();
    ui->info_label->setText("注册成功");
    ui->info_label->setStyleSheet("color: rgb(0,255,0);""background-color: rgb(255, 255, 255);");
    this->timer->start(2000);

    ui->password->clear();
}
