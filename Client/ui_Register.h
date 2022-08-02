/********************************************************************************
** Form generated from reading UI file 'Register.ui'
**
** Created by: Qt User Interface Compiler version 5.11.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_REGISTER_H
#define UI_REGISTER_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_RegisterUi
{
public:
    QGraphicsView *graphicsView;
    QPushButton *BackBtn;
    QLineEdit *email;
    QLabel *label;
    QPushButton *RegisterBtn;
    QLineEdit *password;
    QLabel *info_label;

    void setupUi(QWidget *RegisterUi)
    {
        if (RegisterUi->objectName().isEmpty())
            RegisterUi->setObjectName(QStringLiteral("RegisterUi"));
        RegisterUi->resize(619, 664);
        graphicsView = new QGraphicsView(RegisterUi);
        graphicsView->setObjectName(QStringLiteral("graphicsView"));
        graphicsView->setGeometry(QRect(80, 80, 461, 481));
        BackBtn = new QPushButton(RegisterUi);
        BackBtn->setObjectName(QStringLiteral("BackBtn"));
        BackBtn->setGeometry(QRect(140, 470, 339, 41));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(BackBtn->sizePolicy().hasHeightForWidth());
        BackBtn->setSizePolicy(sizePolicy);
        BackBtn->setMinimumSize(QSize(331, 41));
        email = new QLineEdit(RegisterUi);
        email->setObjectName(QStringLiteral("email"));
        email->setGeometry(QRect(140, 240, 339, 41));
        sizePolicy.setHeightForWidth(email->sizePolicy().hasHeightForWidth());
        email->setSizePolicy(sizePolicy);
        email->setMinimumSize(QSize(331, 41));
        email->setEchoMode(QLineEdit::Normal);
        email->setAlignment(Qt::AlignJustify|Qt::AlignVCenter);
        label = new QLabel(RegisterUi);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(250, 160, 131, 41));
        QFont font;
        font.setPointSize(25);
        label->setFont(font);
        RegisterBtn = new QPushButton(RegisterUi);
        RegisterBtn->setObjectName(QStringLiteral("RegisterBtn"));
        RegisterBtn->setGeometry(QRect(140, 400, 339, 41));
        sizePolicy.setHeightForWidth(RegisterBtn->sizePolicy().hasHeightForWidth());
        RegisterBtn->setSizePolicy(sizePolicy);
        RegisterBtn->setMinimumSize(QSize(331, 41));
        password = new QLineEdit(RegisterUi);
        password->setObjectName(QStringLiteral("password"));
        password->setGeometry(QRect(140, 310, 339, 41));
        sizePolicy.setHeightForWidth(password->sizePolicy().hasHeightForWidth());
        password->setSizePolicy(sizePolicy);
        password->setMinimumSize(QSize(331, 41));
        password->setEchoMode(QLineEdit::Password);
        password->setAlignment(Qt::AlignJustify|Qt::AlignVCenter);
        password->setCursorMoveStyle(Qt::VisualMoveStyle);
        info_label = new QLabel(RegisterUi);
        info_label->setObjectName(QStringLiteral("info_label"));
        info_label->setGeometry(QRect(250, 210, 121, 20));
        info_label->setAlignment(Qt::AlignCenter);

        retranslateUi(RegisterUi);

        QMetaObject::connectSlotsByName(RegisterUi);
    } // setupUi

    void retranslateUi(QWidget *RegisterUi)
    {
        RegisterUi->setWindowTitle(QApplication::translate("RegisterUi", "\346\263\250\345\206\214", nullptr));
        BackBtn->setText(QApplication::translate("RegisterUi", "Back", nullptr));
#ifndef QT_NO_TOOLTIP
        email->setToolTip(QApplication::translate("RegisterUi", "<html><head/><body><p><br/></p></body></html>", nullptr));
#endif // QT_NO_TOOLTIP
        email->setText(QString());
        email->setPlaceholderText(QApplication::translate("RegisterUi", "Input Email", nullptr));
        label->setText(QApplication::translate("RegisterUi", "Register", nullptr));
        RegisterBtn->setText(QApplication::translate("RegisterUi", "Register", nullptr));
        password->setPlaceholderText(QApplication::translate("RegisterUi", "Input Password", nullptr));
        info_label->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class RegisterUi: public Ui_RegisterUi {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_REGISTER_H
