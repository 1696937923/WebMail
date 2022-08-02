#-------------------------------------------------
#
# Project created by QtCreator 2022-08-02T22:29:31
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = WebClient
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
    AES.cpp \
    Base64.cpp \
    EmailUi.cpp \
    ExchangeKey.cpp \
    MainWindowUi.cpp \
    msg.pb.cc \
    RegisterUi.cpp \
    Serialize.cpp \
    SmtpClient.cpp \
    UserUi.cpp \
    WebClient.cpp \
    Main.cpp \
    RSA.cpp \

HEADERS += \
    AES.h \
    Base64.h \
    EmailUi.h \
    MainWindowUi.h \
    msg.pb.h \
    RegisterUi.h \
    RSA.h \
    SmtpClient.h \
    UserUi.h \
    WebClient.h \
    ui_email.h \
    ui_mainwindow.h \
    ui_registerui.h \
    ui_userui.h

FORMS += \
    Email.ui \
    MainWindow.ui \
    Register.ui \
    User.ui

LIBS += -lprotobuf -lcrypto -lpthread

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    msg.proto \
    RSA_Public_Key.txt
