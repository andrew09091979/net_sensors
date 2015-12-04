#include "Internalmsg.h"
#include <cstring>

namespace INTNLMSG
{
    std::string receivers_names[] = { std::string("DISPLAY"),
                                      std::string("DATABASE"),
                                      std::string("FILE"),
                                      std::string("CLIENTSERVICE"),
                                      std::string("Netlistener"),
                                      std::string("NETCONNHANDLER"),
                                      std::string("Device"),
                                      std::string("Device_MANAGER"),
                                      std::string("INTERNL_MSG_ROUTER"),
                                      std::string("BROADCAST")};
}
Internalmsg::Internalmsg()
{

}

Internalmsg::Internalmsg(INTNLMSG::RECEIVER recv_, unsigned int size_,
                        unsigned int val_, char *msg_) :
                      recv(recv_), size(size_), val(val_), msg(msg_)
{

}

Internalmsg::Internalmsg(INTNLMSG::RECEIVER recv_, unsigned int val_, std::string msg_) :
                                                                                recv(recv_), val(val_)
{
    size = msg_.length() + 1;
    msg = new char[size];
    memset(msg, 0, size);
    memcpy(msg, msg_.c_str(), size);
}

Internalmsg::Internalmsg(Internalmsg&& oth) : recv(oth.recv), size(oth.size),
                                            val(oth.val), msg(oth.msg)
{
    oth.msg = nullptr;
}

Internalmsg::Internalmsg(const Internalmsg& oth) : recv(oth.recv), size(oth.size),
                                                 val(oth.val)
{
    msg = new char[size];
    memset(msg, 0, size);
    memcpy(msg, oth.msg, size);
}

Internalmsg::~Internalmsg()
{
    if (msg != nullptr)
        delete [] msg;
}

INTNLMSG::RECEIVER Internalmsg::getreceiver() const
{
    return recv;
}

unsigned int Internalmsg::getsize() const
{
    return size;
}

char * Internalmsg::getmsg() const
{
    return msg;
}

unsigned int Internalmsg::getval() const
{
    return val;
}

void Internalmsg::setrecv(INTNLMSG::RECEIVER recv_)
{
    recv = recv_;
}

void Internalmsg::setsize(unsigned int size_)
{
    size = size_;
}

void Internalmsg::setval(unsigned int val_)
{
    val = val_;
}

void Internalmsg::setmsg(char * const msg_)
{
    msg = msg_;
}

void Internalmsg::setmsg(std::string msg_)
{
    size = msg_.length() + 1;
    msg = new char[size];
    memset(msg, 0, size);
    memcpy(msg, msg_.c_str(), size);
}
