/********************************************************************************
** Form generated from reading UI file 'MainWindow.ui'
**
** Created by: Qt User Interface Compiler version 5.11.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QLabel *label;
    QGraphicsView *graphicsView;
    QLineEdit *email;
    QLineEdit *password;
    QPushButton *RegisterBtn;
    QPushButton *LoginBtn;
    QLabel *info_label;
    QToolBar *mainToolBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(619, 664);
        MainWindow->setLayoutDirection(Qt::LeftToRight);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        label = new QLabel(centralWidget);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(260, 150, 101, 41));
        QFont font;
        font.setPointSize(25);
        label->setFont(font);
        graphicsView = new QGraphicsView(centralWidget);
        graphicsView->setObjectName(QStringLiteral("graphicsView"));
        graphicsView->setGeometry(QRect(80, 70, 461, 481));
        email = new QLineEdit(centralWidget);
        email->setObjectName(QStringLiteral("email"));
        email->setGeometry(QRect(140, 230, 339, 41));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(email->sizePolicy().hasHeightForWidth());
        email->setSizePolicy(sizePolicy);
        email->setMinimumSize(QSize(331, 41));
        email->setEchoMode(QLineEdit::Normal);
        email->setAlignment(Qt::AlignJustify|Qt::AlignVCenter);
        password = new QLineEdit(centralWidget);
        password->setObjectName(QStringLiteral("password"));
        password->setGeometry(QRect(140, 300, 339, 41));
        sizePolicy.setHeightForWidth(password->sizePolicy().hasHeightForWidth());
        password->setSizePolicy(sizePolicy);
        password->setMinimumSize(QSize(331, 41));
        password->setEchoMode(QLineEdit::Password);
        password->setAlignment(Qt::AlignJustify|Qt::AlignVCenter);
        password->setCursorMoveStyle(Qt::VisualMoveStyle);
        RegisterBtn = new QPushButton(centralWidget);
        RegisterBtn->setObjectName(QStringLiteral("RegisterBtn"));
        RegisterBtn->setGeometry(QRect(140, 460, 339, 41));
        sizePolicy.setHeightForWidth(RegisterBtn->sizePolicy().hasHeightForWidth());
        RegisterBtn->setSizePolicy(sizePolicy);
        RegisterBtn->setMinimumSize(QSize(331, 41));
        LoginBtn = new QPushButton(centralWidget);
        LoginBtn->setObjectName(QStringLiteral("LoginBtn"));
        LoginBtn->setGeometry(QRect(140, 390, 339, 41));
        sizePolicy.setHeightForWidth(LoginBtn->sizePolicy().hasHeightForWidth());
        LoginBtn->setSizePolicy(sizePolicy);
        LoginBtn->setMinimumSize(QSize(331, 41));
        info_label = new QLabel(centralWidget);
        info_label->setObjectName(QStringLiteral("info_label"));
        info_label->setGeometry(QRect(270, 200, 71, 20));
        info_label->setAlignment(Qt::AlignCenter);
        MainWindow->setCentralWidget(centralWidget);
        graphicsView->raise();
        label->raise();
        email->raise();
        password->raise();
        RegisterBtn->raise();
        LoginBtn->raise();
        info_label->raise();
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "\347\231\273\345\275\225", nullptr));
        label->setText(QApplication::translate("MainWindow", "LOGIN", nullptr));
#ifndef QT_NO_TOOLTIP
        email->setToolTip(QApplication::translate("MainWindow", "<html><head/><body><p><br/></p></body></html>", nullptr));
#endif // QT_NO_TOOLTIP
        email->setText(QApplication::translate("MainWindow", "1@email.com", nullptr));
        email->setPlaceholderText(QApplication::translate("MainWindow", "Input Email", nullptr));
        password->setText(QApplication::translate("MainWindow", "123456", nullptr));
        password->setPlaceholderText(QApplication::translate("MainWindow", "Input Password", nullptr));
        RegisterBtn->setText(QApplication::translate("MainWindow", "Register", nullptr));
        LoginBtn->setText(QApplication::translate("MainWindow", "Login", nullptr));
        info_label->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
