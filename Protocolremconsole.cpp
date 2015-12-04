#include "Protocolremconsole.h"

Protocolremconsole::Protocolremconsole(std::shared_ptr<Connectionhandler> conn) : Protocol(conn),
                                                                                  START_MARKER('#'),
                                                                                  START_MARKER_LEN(1),
                                                                                  MSG_SIZE_LEN(2),
                                                                                  numBytesToRead(0)
{

}

int Protocolremconsole::exchangeCycle(const arraywrapper<char> &msg,
                                      arraywrapper<char> &response)
{
    int res = 0;
    return res;
}

int Protocolremconsole::getDeviceName(std::string &devName)
{
    int res = -1;

    return res;
}

int Protocolremconsole::getDeviceConfig(std::string &devConfig)
{
    int res = -1;

    return res;
}

int Protocolremconsole::getData(const int param, arraywrapper<char> &data)
{
    int res = -1;

    return res;
}

int Protocolremconsole::getCommand(const int param, std::string &cmd)
{
    int res = -1;
    conn_handl->read_line(cmd);
    return res;
}

int Protocolremconsole::sendData(arraywrapper<char> &data)
{
    int res = -1;
    std::lock_guard<std::mutex> lk(sendmtx);

    if (this->conn_handl->send_nbytes(data.at(0), data.get_size()))
    {
        res = 0;
    }
    return res;
}

int Protocolremconsole::shutdown()
{
    return conn_handl->shutdownconn();
}
