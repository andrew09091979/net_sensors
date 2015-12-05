#include "InternalMsg.h"
#include <cstring>

namespace INTNLMSG
{
    std::string receivers_names[] = { std::string("DISPLAY"),
                                      std::string("DATABASE"),
                                      std::string("FILE"),
                                      std::string("CLIENTSERVICE"),
                                      std::string("NetListener"),
                                      std::string("NETCONNHANDLER"),
                                      std::string("Device"),
                                      std::string("Device_MANAGER"),
                                      std::string("INTERNL_MSG_ROUTER"),
                                      std::string("BROADCAST")};
}
InternalMsg::InternalMsg()
{

}

InternalMsg::InternalMsg(INTNLMSG::RECEIVER recv_, unsigned int size_,
                        unsigned int val_, char *msg_) :
                      recv(recv_), size(size_), val(val_), msg(msg_)
{

}

InternalMsg::InternalMsg(INTNLMSG::RECEIVER recv_, unsigned int val_, std::string msg_) :
                                                                                recv(recv_), val(val_)
{
    size = msg_.length() + 1;
    msg = new char[size];
    memset(msg, 0, size);
    memcpy(msg, msg_.c_str(), size);
}

InternalMsg::InternalMsg(InternalMsg&& oth) : recv(oth.recv), size(oth.size),
                                            val(oth.val), msg(oth.msg)
{
    oth.msg = nullptr;
}

InternalMsg::InternalMsg(const InternalMsg& oth) : recv(oth.recv), size(oth.size),
                                                 val(oth.val)
{
    msg = new char[size];
    memset(msg, 0, size);
    memcpy(msg, oth.msg, size);
}

InternalMsg::~InternalMsg()
{
    if (msg != nullptr)
        delete [] msg;
}

INTNLMSG::RECEIVER InternalMsg::getreceiver() const
{
    return recv;
}

unsigned int InternalMsg::getsize() const
{
    return size;
}

char * InternalMsg::getmsg() const
{
    return msg;
}

unsigned int InternalMsg::getval() const
{
    return val;
}

void InternalMsg::setrecv(INTNLMSG::RECEIVER recv_)
{
    recv = recv_;
}

void InternalMsg::setsize(unsigned int size_)
{
    size = size_;
}

void InternalMsg::setval(unsigned int val_)
{
    val = val_;
}

void InternalMsg::setmsg(char * const msg_)
{
    msg = msg_;
}

void InternalMsg::setmsg(std::string msg_)
{
    size = msg_.length() + 1;
    msg = new char[size];
    memset(msg, 0, size);
    memcpy(msg, msg_.c_str(), size);
}
