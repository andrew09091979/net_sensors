#include "protocolremconsole.h"

protocolremconsole::protocolremconsole(std::shared_ptr<connectionhandler> conn) : protocol(conn),
                                                                                  START_MARKER('#'),
                                                                                  START_MARKER_LEN(1),
                                                                                  MSG_SIZE_LEN(2),
                                                                                  numBytesToRead(0)
{

}

int protocolremconsole::exchangeCycle(const arraywrapper<char> &msg,
                                      arraywrapper<char> &response)
{
    int res = 0;
    return res;
}

int protocolremconsole::getDeviceName(std::string &devName)
{
    int res = -1;

    return res;
}

int protocolremconsole::getDeviceConfig(std::string &devConfig)
{
    int res = -1;

    return res;
}

int protocolremconsole::getData(const int param, arraywrapper<char> &data)
{
    int res = -1;

    return res;
}

int protocolremconsole::getCommand(const int param, arraywrapper<char> &data)
{
    int res = -1;

    return res;
}

int protocolremconsole::sendData(arraywrapper<char> &data)
{
    int res = -1;

    if (this->conn_handl->send_nbytes(data.at(0), data.get_size()))
    {
        res = 0;
    }
    return res;
}

int protocolremconsole::shutdown()
{
    return conn_handl->shutdownconn();
}
