TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    internlmsg.cpp \
    connectionhandler.cpp \
    protooolandroiddev.cpp

QMAKE_CXXFLAGS += -std=c++0x

LIBS += -pthread

HEADERS += \
    workerdisplay.h \
    netlistener.h \
    netconnectionhandler.h \
    message.h \
    clientservice.h \
    internlmsg.h \
    internlmsgsender.h \
    internlmsgreceiver.h \
    modulemanager.h \
    protocol.h \
    arraywrapper.h \
    protocolandroiddev.h \
    device.h \
    connectionhandler.h \
    deviceandroid.h

