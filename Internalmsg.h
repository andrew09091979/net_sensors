#ifndef Internalmsg_H
#define Internalmsg_H

#include <string>

namespace INTNLMSG
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
        RECV_Device,
        RECV_Device_MANAGER,
        RECV_INTERNL_MSG_ROUTER,
        RECV_BROADCAST
    };
    enum MSG_TYPE
    {
        SHOW_MESSAGE,
        Device_ADDED,
        Device_SHUTDOWN,
        GET_NUM_OF_DEVS,
        SHUTDOWN_ALL
    };
}
class Internalmsg
{
    INTNLMSG::RECEIVER recv;
    unsigned int size;
    unsigned int val;
    char *msg;

public:
    Internalmsg();
    Internalmsg(INTNLMSG::RECEIVER recv_, unsigned int size_,
               unsigned int val_, char *msg_);
    Internalmsg(INTNLMSG::RECEIVER recv_, unsigned int val_, std::string msg_);
    Internalmsg(Internalmsg&& oth);
    Internalmsg(const Internalmsg& oth);
    ~Internalmsg();
    INTNLMSG::RECEIVER getreceiver() const;
    unsigned int getsize() const;
    char *getmsg() const;
    unsigned int getval() const;
    void setrecv(INTNLMSG::RECEIVER recv_);
    void setsize(unsigned int size_);
    void setval(unsigned int val_);
    void setmsg(char *const msg_);
    void setmsg(std::string msg_);
};

#endif // Internalmsg_H
