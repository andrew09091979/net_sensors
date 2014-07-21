#ifndef PROTOCOLANDROIDDEV_H
#define PROTOCOLANDROIDDEV_H

#include <protocol.h>

class protocolandroiddev : public protocol<char>
{
    enum STATE
    {
        INITIAL,
        WAITING_FOR_INIT_ANSWER,
        WAITING_FOR_ANSWER,
        SEND_REQUEST,
        CLOSE
    };

    enum MESSAGE_TO_SENSOR
    {
        GREETING,
        SEND_ME_VALUES,
        GET_FROM_ME_VALUES
    };

    const char START_BYTE;
public:
    protocolandroiddev() : START_BYTE('#')
    {

    }

    arraywrapper<char> wrapMessage(const arraywrapper<char> &msg)
    {
        arraywrapper<char> msgWrapped(msg.get_size() + 3);

        return msgWrapped;
    }

    ~protocolandroiddev() {}
};
#endif // PROTOCOLANDROIDDEV_H
