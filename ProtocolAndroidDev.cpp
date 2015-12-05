#include "ProtocolAndroidDev.h"

ProtocolAndroidDev::ProtocolAndroidDev(std::shared_ptr<Connectionhandler> conn) : Protocol(conn),
                                                                                  START_MARKER('#'),
                                                                                  START_MARKER_LEN(1),
                                                                                  MSG_SIZE_LEN(2),
                                                                                  numBytesToRead(0)
{

}

int ProtocolAndroidDev::exchangeCycle(const arraywrapper<char> &msg, arraywrapper<char> &response)
{
    bool cycleComplete = false;
    STATE state = SEND_REQUEST;
    short size = 0;
    int res = 0;

    while (!cycleComplete)
    {
        switch (state)
        {
            case SEND_REQUEST:
            {
                if (this->conn_handl->send_nbytes(msg.at(0), msg.get_size()))
                {
                    state = READ_START_MARKER;
                }
                else
                {
                    res = -1;
                    state = FINISHED;
                }
            }
            break;
            case READ_START_MARKER:
            {
                char startMarker;

                if (this->conn_handl->read_nbytes(&startMarker, START_MARKER_LEN))
                {
                    if (startMarker == START_MARKER)
                    {
                        state = READ_MSG_SIZE;
                    }
                }
                else
                {
                    res = -1;
                    state = FINISHED;
                }
            }
            break;
            case READ_MSG_SIZE:
            {
                char msgSize[MSG_SIZE_LEN];

                if (this->conn_handl->read_nbytes(&msgSize[0], MSG_SIZE_LEN))
                {
                    size = *(short*)msgSize;
                    state = READ_MSG;
                }
                else
                {
                    res = -1;
                    state = FINISHED;
                }
            }
            break;
            case READ_MSG:
            {
                arraywrapper<char> resp(size+1);
                resp.zero_mem();

                if (this->conn_handl->read_nbytes(resp.at(0), size))
                {
                    response = std::move(resp);
                    res = 0;
                }
                else
                {
                    res = -1;
                }
                state = FINISHED;
            }
            break;
            case FINISHED:
            {
                cycleComplete = true;
            }
            break;
        }
    }
    return res;
}

int ProtocolAndroidDev::getDeviceName(std::string &devName)
{
    int res = -1;
    arraywrapper<char> msg(4);
    arraywrapper<char> response;
    *msg.at(0) = START_MARKER;
    *msg.at(1) = 0x0;
    *msg.at(2) = 0x1;
    *msg.at(3) = GET_Device_NAME;

    res = exchangeCycle(msg, response);

    if (res != -1)
    {
        devName = response.at(0);
    }
    return res;
}

int ProtocolAndroidDev::getDeviceConfig(std::string &devConfig)
{
    int res = -1;
    arraywrapper<char> msg(4);
    arraywrapper<char> response;

    *msg.at(0) = START_MARKER;
    *msg.at(1) = 0x0;
    *msg.at(2) = 0x1;
    *msg.at(3) = GET_Device_CONFIG;

    res = exchangeCycle(msg, response);

    if (res != -1)
    {
        devConfig = response.at(0);
    }
    return res;
}

int ProtocolAndroidDev::getData(const int param, arraywrapper<char> &data)
{
    int res = -1;
    arraywrapper<char> msg(4);

    *msg.at(0) = START_MARKER;
    *msg.at(1) = 0x0;
    *msg.at(2) = 0x1;
    *msg.at(3) = GET_VALUES_IN_STRING;

    res = exchangeCycle(msg, data);

    return res;
}

int ProtocolAndroidDev::getCommand(const int param, std::string &cmd)
{
    int res = -1;

    return res;
}

int ProtocolAndroidDev::sendData(arraywrapper<char> &data)
{
    int res = -1;

    return res;
}
int ProtocolAndroidDev::shutdown()
{
    return conn_handl->shutdownconn();
}
