#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <arraywrapper.h>
#include <memory>
#include "connectionhandler.h"

template <class D>
class protocol
{
protected:
    std::shared_ptr<connectionhandler> conn_handl;

public:
    protocol(std::shared_ptr<connectionhandler> conn) : conn_handl(conn){}
    virtual int getDeviceName(std::string &devName) = 0;
    virtual int getDeviceConfig(std::string &devConfig) = 0;
    virtual int getData(const int param, arraywrapper<char> &data) = 0;
    virtual int getCommand(const int param, std::string &cmd) = 0;
    virtual int sendData(arraywrapper<char> &data) = 0;
    virtual int shutdown() = 0;
    virtual ~protocol(){}
};

#endif // PROTOCOL_H
