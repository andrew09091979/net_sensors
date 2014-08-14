#ifndef PROTOCOLREMCONSOLE_H
#define PROTOCOLREMCONSOLE_H

#include <protocol.h>
#include <string>

class protocolremconsole : public protocol<char>
{
    enum MESSAGE_TO_SENSOR
    {
        GET_DEVICE_NAME = 0x30,
        GET_DEVICE_CONFIG = 0x31,
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

    int exchangeCycle(const arraywrapper<char> &msg, arraywrapper<char> &response);
public:
    protocolremconsole(std::shared_ptr<connectionhandler> conn);

    int getDeviceName(std::string &devName);
    int getDeviceConfig(std::string &devConfig);
    int getData(const int param, arraywrapper<char> &data);
    int getCommand(const int param, std::string &cmd);
    int sendData(arraywrapper<char> &data);
    int shutdown();
    ~protocolremconsole() {}
};

#endif // PROTOCOLREMCONSOLE_H