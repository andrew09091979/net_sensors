#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <arraywrapper.h>
#include <memory>
#include "ConnectionHandler.h"

template <class D>
class Protocol
{
protected:
    std::shared_ptr<ConnectionHandler> conn_handl;

public:
    Protocol(std::shared_ptr<ConnectionHandler> conn) : conn_handl(conn){}
    virtual int getDeviceName(std::string &devName) = 0;
    virtual int getDeviceConfig(std::string &devConfig) = 0;
    virtual int getData(const int param, arraywrapper<char> &data) = 0;
    virtual int getCommand(const int param, std::string &cmd) = 0;
    virtual int sendData(arraywrapper<char> &data) = 0;
    virtual int shutdown() = 0;
    virtual ~Protocol(){}
};

#endif // PROTOCOL_H
