#ifndef PROTOCOLANDROIDDEV_H
#define PROTOCOLANDROIDDEV_H

#include <protocol.h>
#include <string>

class protocolandroiddev : public protocol<char>
{
    enum MESSAGE_TO_SENSOR
    {
        GREETING,
        SEND_ME_VALUES,
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
    protocolandroiddev(std::shared_ptr<connectionhandler> conn);

    int getDeviceName(std::string &devName);
    int getData(const int param, arraywrapper<char> &data);
    int shutdown();
    ~protocolandroiddev() {}
};
#endif // PROTOCOLANDROIDDEV_H
