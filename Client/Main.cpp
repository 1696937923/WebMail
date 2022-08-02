#include "MainWindowUi.h"
#include "UserUi.h"
#include "EmailUi.h"
#include "RegisterUi.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow mw;
    UserUi uu;
    RegisterUi ru;
    Email eu;

    mw.show();

    QObject::connect(&mw, SIGNAL(ShowUserUi(int, const char *, const char *, int)),
                     &uu,SLOT(ReciveUserUiShow(int, const char *, const char *, int))); //login
    QObject::connect(&mw, SIGNAL(ShowRegisterUi()),&ru,SLOT(ReciveRegisterUiShow())); //in register
    QObject::connect(&ru, SIGNAL(ShowMainUi()),&mw,SLOT(ReciveMainUiShow())); //register back
    QObject::connect(&uu, SIGNAL(ShowEmailUi(const char *)),&eu,SLOT(ReciveEmailUiShow(const char *))); //in email box
    QObject::connect(&eu, SIGNAL(BackUserUi()),&uu,SLOT(ReciveBackUiShow())); //email box back
    QObject::connect(&uu, SIGNAL(ShowMainUi()),&mw,SLOT(ReciveMainUiShow())); //logout
    QObject::connect(&mw, SIGNAL(NewEmail()),&uu,SLOT(ReciveNewEmail())); //New Email come


    return a.exec();
}
