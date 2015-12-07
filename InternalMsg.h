#ifndef Internalmsg_H
#define Internalmsg_H

#include <string>

namespace INTERNALMESSAGE
{

    extern std::string receivers_names[];

    enum RECEIVER
    {
        RECV_DISPLAY,
        RECV_DATABASE,
        RECV_FILE,
        RECV_CLIENTSERVICE,
        RECV_Netlistener,
        RECV_NETCONNHANDLER,
        RECV_DEVICE,
        RECV_DEVICE_MANAGER,
        RECV_INTERNL_MSG_ROUTER,
        RECV_BROADCAST
    };
    enum MSG_TYPE
    {
        SHOW_MESSAGE,
        STORE_MESSAGE,
        DEVICE_ADDED,
        DEVICE_SHUTDOWN,
        GET_NUM_OF_DEVS,
        SHUTDOWN_ALL
    };
}
class InternalMsg
{
    INTERNALMESSAGE::RECEIVER recv;
    unsigned int size;
    unsigned int val;
    char *msg;

public:
    InternalMsg();
    InternalMsg(INTERNALMESSAGE::RECEIVER recv_, unsigned int size_,
               unsigned int val_, char *msg_);
    InternalMsg(INTERNALMESSAGE::RECEIVER recv_, unsigned int val_, std::string msg_);
    InternalMsg(InternalMsg&& oth);
    InternalMsg(const InternalMsg& oth);
    ~InternalMsg();
    INTERNALMESSAGE::RECEIVER getreceiver() const;
    unsigned int getsize() const;
    char *getmsg() const;
    unsigned int getval() const;
    void setrecv(INTERNALMESSAGE::RECEIVER recv_);
    void setsize(unsigned int size_);
    void setval(unsigned int val_);
    void setmsg(char *const msg_);
    void setmsg(std::string msg_);
};

#endif // Internalmsg_H