#ifndef USERUI_H
#define USERUI_H

#include <QWidget>
#include <QTimer>

namespace Ui {
class UserUi;
}

class UserUi : public QWidget
{
    Q_OBJECT

public:
    explicit UserUi(QWidget *parent = nullptr);
    ~UserUi();

private:
    Ui::UserUi *ui;
    QTimer *timer;
    char CurrentUser[48];
    char Password[32];
    int login_fd;

private slots:
    void ReciveUserUiShow(int, const char *, const char *, int);//与发射信号的函数相关联的槽函数

    void ReciveBackUiShow();

    void ReciveNewEmail();

    void on_SendBtn_clicked();

    void on_LogoutBtn_clicked();

    void on_EmailBtn_clicked();

signals:
    void ShowEmailUi(const char *);
    void ShowMainUi();
};

#endif // USERUI_H
