/********************************************************************************
** Form generated from reading UI file 'Email.ui'
**
** Created by: Qt User Interface Compiler version 5.11.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_EMAIL_H
#define UI_EMAIL_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Email
{
public:
    QPushButton *BackBtn;
    QListWidget *listWidget;
    QPushButton *DelEmailBtn;
    QLabel *info_label;

    void setupUi(QWidget *Email)
    {
        if (Email->objectName().isEmpty())
            Email->setObjectName(QStringLiteral("Email"));
        Email->resize(697, 749);
        BackBtn = new QPushButton(Email);
        BackBtn->setObjectName(QStringLiteral("BackBtn"));
        BackBtn->setGeometry(QRect(50, 670, 131, 41));
        listWidget = new QListWidget(Email);
        listWidget->setObjectName(QStringLiteral("listWidget"));
        listWidget->setGeometry(QRect(50, 60, 591, 581));
        QFont font;
        font.setPointSize(12);
        listWidget->setFont(font);
        DelEmailBtn = new QPushButton(Email);
        DelEmailBtn->setObjectName(QStringLiteral("DelEmailBtn"));
        DelEmailBtn->setGeometry(QRect(510, 670, 131, 41));
        info_label = new QLabel(Email);
        info_label->setObjectName(QStringLiteral("info_label"));
        info_label->setGeometry(QRect(290, 20, 101, 21));
        QFont font1;
        font1.setPointSize(11);
        info_label->setFont(font1);

        retranslateUi(Email);

        QMetaObject::connectSlotsByName(Email);
    } // setupUi

    void retranslateUi(QWidget *Email)
    {
        Email->setWindowTitle(QApplication::translate("Email", "\346\224\266\344\273\266\347\256\261", nullptr));
        BackBtn->setText(QApplication::translate("Email", "\350\277\224\345\233\236", nullptr));
        DelEmailBtn->setText(QApplication::translate("Email", "\345\210\240\351\231\244\346\211\200\351\200\211\351\202\256\344\273\266", nullptr));
        info_label->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class Email: public Ui_Email {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_EMAIL_H
