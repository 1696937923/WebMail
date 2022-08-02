#include "EmailUi.h"
#include "ui_email.h"
#include <iostream>
#include <WebClient.h>

Email::Email(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Email)
{
    ui->setupUi(this);
    this->setStyleSheet("background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(251,102,102, 200), stop:1 rgba(20,196,188, 210));");
    ui->BackBtn->setStyleSheet("QPushButton{font: 25 13pt '微软雅黑 Light';color: rgb(255,255,255);background-color: rgb(20,196,188);"
                                "border: none;border-radius:15px;}"
                                "QPushButton:hover{background-color: rgb(22,218,208);}"
                                "QPushButton:pressed{background-color: rgb(17,171,164);}");
    ui->DelEmailBtn->setStyleSheet("QPushButton{font: 25 13pt '微软雅黑 Light';color: rgb(255,255,255);background-color: rgb(20,196,188);"
                                "border: none;border-radius:15px;}"
                                "QPushButton:hover{background-color: rgb(22,218,208);}"//hover
                                "QPushButton:pressed{background-color: rgb(17,171,164);}");//pressed
    ui->listWidget->setStyleSheet("background-color: rgb(255, 255, 255);border-radius:15px;");
    ui->listWidget->setSpacing(5);
    ui->info_label->hide();
}

Email::~Email()
{
    delete ui;
}

void Email::ReciveEmailUiShow(const char *user)
{
    WebClient webclient;
    struct PULL_INFO pull_info;

    memset(&pull_info, 0, sizeof(PULL_INFO));
    strncpy(this->CurrentUser, user, strlen(user));
    strncpy(pull_info.email, user, strlen(user));

    int socket_fd = webclient.Connect();
    if (socket_fd < 0){
       return;
    }

    int ret = webclient.Pull_Email(socket_fd, pull_info, ui);
    if(ret < 0){
        std::cout << "Pull Email Error!" << std::endl;
    }

    this->show();
}

void Email::on_BackBtn_clicked()
{    
    this->hide();
    emit BackUserUi();
}

void Email::on_DelEmailBtn_clicked()
{

    //先判断当前是否选中，如果没有选中则提示
        if(!ui->listWidget->currentItem()){
            std::cout<<"警告请先选中当前项再进行删除!"<<std::endl;
            return;
        }

        //获取当前想要删除的子项
        QListWidgetItem * delItem = ui->listWidget->takeItem(ui->listWidget->currentRow());
        std::cout<<delItem->checkState();
        if(delItem)//注意需要手动删除
            delete delItem;
}
