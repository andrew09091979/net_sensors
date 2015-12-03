#include "internlmsg.h"
#include <cstring>

namespace INTNLMSG
{
    std::string receivers_names[] = { std::string("DISPLAY"),
                                      std::string("DATABASE"),
                                      std::string("FILE"),
                                      std::string("CLIENTSERVICE"),
                                      std::string("NETLISTENER"),
                                      std::string("NETCONNHANDLER"),
                                      std::string("DEVICE"),
                                      std::string("DEVICE_MANAGER"),
                                      std::string("INTERNL_MSG_ROUTER"),
                                      std::string("BROADCAST")};
}
internlmsg::internlmsg()
{

}

internlmsg::internlmsg(INTNLMSG::RECEIVER recv_, unsigned int size_,
                        unsigned int val_, char *msg_) :
                      recv(recv_), size(size_), val(val_), msg(msg_)
{

}

internlmsg::internlmsg(INTNLMSG::RECEIVER recv_, unsigned int val_, std::string msg_) :
                                                                                recv(recv_), val(val_)
{
    size = msg_.length() + 1;
    msg = new char[size];
    memset(msg, 0, size);
    memcpy(msg, msg_.c_str(), size);
}

internlmsg::internlmsg(internlmsg&& oth) : recv(oth.recv), size(oth.size),
                                            val(oth.val), msg(oth.msg)
{
    oth.msg = nullptr;
}

internlmsg::internlmsg(const internlmsg& oth) : recv(oth.recv), size(oth.size),
                                                 val(oth.val)
{
    msg = new char[size];
    memset(msg, 0, size);
    memcpy(msg, oth.msg, size);
}

internlmsg::~internlmsg()
{
    if (msg != nullptr)
        delete [] msg;
}

INTNLMSG::RECEIVER internlmsg::getreceiver() const
{
    return recv;
}

unsigned int internlmsg::getsize() const
{
    return size;
}

char * internlmsg::getmsg() const
{
    return msg;
}

unsigned int internlmsg::getval() const
{
    return val;
}

void internlmsg::setrecv(INTNLMSG::RECEIVER recv_)
{
    recv = recv_;
}

void internlmsg::setsize(unsigned int size_)
{
    size = size_;
}

void internlmsg::setval(unsigned int val_)
{
    val = val_;
}

void internlmsg::setmsg(char * const msg_)
{
    msg = msg_;
}

void internlmsg::setmsg(std::string msg_)
{
    size = msg_.length() + 1;
    msg = new char[size];
    memset(msg, 0, size);
    memcpy(msg, msg_.c_str(), size);
}
