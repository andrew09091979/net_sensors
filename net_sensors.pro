TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    Internalmsg.cpp \
    Protocolremconsole.cpp \
    Protooolandroiddev.cpp \
    Connectionhandler.cpp

QMAKE_CXXFLAGS += -std=c++0x

LIBS += -pthread

HEADERS += \
    workerdisplay.h \
    arraywrapper.h \
    Internalmsgrouter.h \
    Internalmsgreceiver.h \
    Internalmsg.h \
    Internalmsgsender.h \
    Netlistener.h \
    Device.h \
    Deviceandroid.h \
    Deviceremconsole.h \
    Devicemanager.h \
    Connectionhandler.h \
    Netconnectionhandler.h \
    Protocol.h \
    Protocolandroiddev.h \
    Protocolremconsole.h

