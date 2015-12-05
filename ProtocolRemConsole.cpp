#include "ProtocolRemConsole.h"

ProtocolRemConsole::ProtocolRemConsole(std::shared_ptr<ConnectionHandler> conn) : Protocol(conn),
                                                                                  START_MARKER('#'),
                                                                                  START_MARKER_LEN(1),
                                                                                  MSG_SIZE_LEN(2),
                                                                                  numBytesToRead(0)
{

}

int ProtocolRemConsole::exchangeCycle(const arraywrapper<char> &msg,
                                      arraywrapper<char> &response)
{
    int res = 0;
    return res;
}

int ProtocolRemConsole::getDeviceName(std::string &devName)
{
    int res = -1;

    return res;
}

int ProtocolRemConsole::getDeviceConfig(std::string &devConfig)
{
    int res = -1;

    return res;
}

int ProtocolRemConsole::getData(const int param, arraywrapper<char> &data)
{
    int res = -1;

    return res;
}

int ProtocolRemConsole::getCommand(const int param, std::string &cmd)
{
    int res = -1;
    conn_handl->read_line(cmd);
    return res;
}

int ProtocolRemConsole::sendData(arraywrapper<char> &data)
{
    int res = -1;
    std::lock_guard<std::mutex> lk(sendmtx);

    if (this->conn_handl->send_nbytes(data.at(0), data.get_size()))
    {
        res = 0;
    }
    return res;
}

int ProtocolRemConsole::shutdown()
{
    return conn_handl->shutdownconn();
}
