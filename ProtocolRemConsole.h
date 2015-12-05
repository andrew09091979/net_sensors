#ifndef PROTOCOLREMCONSOLE_H
#define PROTOCOLREMCONSOLE_H

#include <string>
#include <mutex>
#include "Protocol.h"


class ProtocolRemConsole : public Protocol<char>
{
    enum MESSAGE_TO_SENSOR
    {
        GET_Device_NAME = 0x30,
        GET_Device_CONFIG = 0x31,
        GET_VALUES_IN_STRING = 0x32,
        GET_VALUES = 0x33,
        GET_FROM_ME_VALUES
    };

    enum STATE
    {
        SEND_REQUEST,
        READ_START_MARKER,
        READ_MSG_SIZE,
        READ_MSG,
        FINISHED
    };

    const char START_MARKER;
    const int START_MARKER_LEN;
    const int MSG_SIZE_LEN;

    int numBytesToRead;
    std::mutex sendmtx;
    int exchangeCycle(const arraywrapper<char> &msg, arraywrapper<char> &response);
public:
    ProtocolRemConsole(std::shared_ptr<Connectionhandler> conn);

    int getDeviceName(std::string &devName);
    int getDeviceConfig(std::string &devConfig);
    int getData(const int param, arraywrapper<char> &data);
    int getCommand(const int param, std::string &cmd);
    int sendData(arraywrapper<char> &data);
    int shutdown();
    ~ProtocolRemConsole() {}
};

#endif // PROTOCOLREMCONSOLE_H
