#include "MainWindowUi.h"
#include "ui_mainwindow.h"
#include <iostream>
#include "WebClient.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    this->setStyleSheet("background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(251,102,102, 200), stop:1 rgba(20,196,188, 210));");
    ui->setupUi(this);
    ui->password->setStyleSheet("font: 25 13pt '微软雅黑 Light';" //字体
                                "color: rgb(31,31,31);"		//字体颜色
                                "padding-left:15px;"       //内边距-字体缩进
                                "background-color: rgb(255, 255, 255);" //背景颜色
                                "border:2px solid rgb(20,196,188);border-radius:15px;");//边框粗细-颜色-圆角设置
    ui->email->setStyleSheet("font: 25 13pt '微软雅黑 Light';color: rgb(31,31,31);"
                             "padding-left:15px;background-color: rgb(255, 255, 255);"
                             "border:2px solid rgb(20,196,188);border-radius:15px;");
    ui->LoginBtn->setStyleSheet("QPushButton{font: 25 13pt '微软雅黑 Light';color: rgb(255,255,255);background-color: rgb(20,196,188);"
                                    "border: none;border-radius:15px;}"
                                    "QPushButton:hover{background-color: rgb(22,218,208);}"//hover
                                    "QPushButton:pressed{background-color: rgb(17,171,164);}");//pressed
    ui->RegisterBtn->setStyleSheet("QPushButton{font: 25 13pt '微软雅黑 Light';color: rgb(255,255,255);background-color: rgb(20,196,188);"
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

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::ReciveMainUiShow()
{
    this->show();
}

void MainWindow::on_LoginBtn_clicked()
{
    WebClient webclient;
    struct LOGIN_INFO login_info;
    memset(&login_info, 0, sizeof(LOGIN_INFO));

    QString email = ui->email->text();
    QString password = ui->password->text();

    strncpy(login_info.email, email.toStdString().c_str(), email.length());
    strncpy(login_info.password, password.toStdString().c_str(), password.length());

    int socket_fd = webclient.Connect();
    if (socket_fd < 0){
        ui->info_label->show();
        ui->info_label->setText("登录失败");
        ui->info_label->setStyleSheet("color: rgb(255,0,0);""background-color: rgb(255, 255, 255);");
        this->timer->start(2000);

        return;
    }

    int ret = webclient.Login(socket_fd, login_info, this);
    if(ret < 0){
        ui->info_label->show();
        ui->info_label->setText("登录失败");
        ui->info_label->setStyleSheet("color: rgb(255,0,0);""background-color: rgb(255, 255, 255);");
        this->timer->start(2000);

        return;
    }

    ui->info_label->show();
    ui->info_label->setText("登录成功");
    ui->info_label->setStyleSheet("color: rgb(0,255,0);""background-color: rgb(255, 255, 255);");
    this->timer->start(2000);

    ui->password->clear();

    this->hide();
    emit ShowUserUi(socket_fd, login_info.email, login_info.password, ret);
}

void MainWindow::on_RegisterBtn_clicked()
{
    this->hide();
    emit ShowRegisterUi();
}
