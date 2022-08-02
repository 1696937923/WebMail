/********************************************************************************
** Form generated from reading UI file 'User.ui'
**
** Created by: Qt User Interface Compiler version 5.11.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_USER_H
#define UI_USER_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_UserUi
{
public:
    QLineEdit *receiver;
    QTextEdit *content;
    QLineEdit *subject;
    QPushButton *SendBtn;
    QPushButton *EmailBtn;
    QPushButton *LogoutBtn;
    QLabel *label;
    QLabel *label_2;
    QLabel *label_3;
    QGraphicsView *graphicsView;
    QLabel *info_label;

    void setupUi(QWidget *UserUi)
    {
        if (UserUi->objectName().isEmpty())
            UserUi->setObjectName(QStringLiteral("UserUi"));
        UserUi->resize(697, 749);
        QFont font;
        font.setPointSize(12);
        UserUi->setFont(font);
        receiver = new QLineEdit(UserUi);
        receiver->setObjectName(QStringLiteral("receiver"));
        receiver->setGeometry(QRect(130, 100, 451, 41));
        receiver->setFont(font);
        content = new QTextEdit(UserUi);
        content->setObjectName(QStringLiteral("content"));
        content->setGeometry(QRect(130, 240, 451, 351));
        subject = new QLineEdit(UserUi);
        subject->setObjectName(QStringLiteral("subject"));
        subject->setGeometry(QRect(130, 170, 451, 41));
        subject->setFont(font);
        SendBtn = new QPushButton(UserUi);
        SendBtn->setObjectName(QStringLiteral("SendBtn"));
        SendBtn->setGeometry(QRect(130, 630, 131, 41));
        EmailBtn = new QPushButton(UserUi);
        EmailBtn->setObjectName(QStringLiteral("EmailBtn"));
        EmailBtn->setGeometry(QRect(290, 630, 131, 41));
        LogoutBtn = new QPushButton(UserUi);
        LogoutBtn->setObjectName(QStringLiteral("LogoutBtn"));
        LogoutBtn->setGeometry(QRect(450, 630, 131, 41));
        label = new QLabel(UserUi);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(70, 110, 58, 21));
        label_2 = new QLabel(UserUi);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(80, 180, 41, 21));
        label_3 = new QLabel(UserUi);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(80, 240, 41, 21));
        graphicsView = new QGraphicsView(UserUi);
        graphicsView->setObjectName(QStringLiteral("graphicsView"));
        graphicsView->setGeometry(QRect(50, 50, 601, 661));
        info_label = new QLabel(UserUi);
        info_label->setObjectName(QStringLiteral("info_label"));
        info_label->setGeometry(QRect(307, 70, 81, 20));
        QFont font1;
        font1.setPointSize(11);
        info_label->setFont(font1);
        info_label->setAlignment(Qt::AlignCenter);
        graphicsView->raise();
        receiver->raise();
        content->raise();
        subject->raise();
        SendBtn->raise();
        EmailBtn->raise();
        LogoutBtn->raise();
        label->raise();
        label_2->raise();
        label_3->raise();
        info_label->raise();

        retranslateUi(UserUi);

        QMetaObject::connectSlotsByName(UserUi);
    } // setupUi

    void retranslateUi(QWidget *UserUi)
    {
        UserUi->setWindowTitle(QApplication::translate("UserUi", "\345\206\231\351\202\256\344\273\266", nullptr));
        receiver->setText(QString());
        subject->setText(QString());
        SendBtn->setText(QApplication::translate("UserUi", "\345\217\221\351\200\201", nullptr));
        EmailBtn->setText(QApplication::translate("UserUi", "\346\224\266\344\273\266\347\256\261", nullptr));
        LogoutBtn->setText(QApplication::translate("UserUi", "\351\200\200\345\207\272", nullptr));
        label->setText(QApplication::translate("UserUi", "<html><head/><body><p><span style=\" font-size:12pt; font-weight:600;\">\346\224\266\344\273\266\344\272\272</span></p></body></html>", nullptr));
        label_2->setText(QApplication::translate("UserUi", "<html><head/><body><p><span style=\" font-size:12pt; font-weight:600;\">\344\270\273\351\242\230</span></p></body></html>", nullptr));
        label_3->setText(QApplication::translate("UserUi", "<html><head/><body><p><span style=\" font-size:12pt; font-weight:600;\">\346\255\243\346\226\207</span></p></body></html>", nullptr));
        info_label->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class UserUi: public Ui_UserUi {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_USER_H
