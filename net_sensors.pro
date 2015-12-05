TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    InternalMsg.cpp \
    ConnectionHandler.cpp \
    ProtocolRemConsole.cpp \
    ProtocolAndroidDev.cpp

QMAKE_CXXFLAGS += -std=c++0x

LIBS += -pthread

HEADERS += \
    arraywrapper.h \
    Device.h \
    Connectionhandler.h \
    Protocol.h \
    WorkerDisplay.h \
    DeviceAndroid.h \
    DeviceManager.h \
    DeviceRemConsole.h \
    InternalMsg.h \
    InternalMsgReceiver.h \
    InternalMsgRouter.h \
    InternalMsgSender.h \
    NetListener.h \
    NetConnectionHandler.h \
    ProtocolAndroidDev.h \
    ProtocolRemConsole.h

