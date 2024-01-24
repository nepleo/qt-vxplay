QT += widgets opengl core gui multimedia

TEMPLATE = app

TARGET = xplay

CONFIG += qt thread

LIBS += -L"$$PWD/../lib"

INCLUDEPATH += $$PWD/../include

DEPENDPATH += .

MOC_DIR += .

OBJECTS_DIR += debug

UI_DIR += .

RCC_DIR += .

HEADERS += \
    ./ivideocall.h \
    ./resample.h \
    ./xaudioplay.h \
    ./xaudiothread.h \
    ./xdecode.h \
    ./xdemux.h \
    ./xvideothread.h \
    ./xplay.h \
    ./xvideowidget.h \
    ./xdemuxthread.h \
    ./xdecodethread.h

SOURCES += \
    ./main.cpp \
    ./resample.cpp \
    ./xaudioplay.cpp \
    ./xaudiothread.cpp \
    ./xdecode.cpp \
    ./xdemux.cpp \
    ./xplay.cpp \
    ./xvideothread.cpp \
    ./xvideowidget.cpp \
    ./xdemuxthread.cpp \
    ./xdecodethread.cpp

FORMS += ./xplay.ui

RESOURCES += xplay.qrc
