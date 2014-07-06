#ifndef INTERNLMSG_H
#define INTERNLMSG_H

#include <string>

namespace INTNLMSG
{
    enum RECEIVER
    {
        RECV_DISPLAY,
        RECV_DATABASE,
        RECV_FILE,
        RECV_CLIENTSERVICE,
        RECV_NETLISTENER,
        RECV_NETCONNHANDLER
    };
}
class internlmsg
{
    INTNLMSG::RECEIVER recv;
    unsigned int size;
    unsigned int val;
    char *msg;

public:
    internlmsg();
    internlmsg(INTNLMSG::RECEIVER recv_, unsigned int size_,
               unsigned int val_, char *msg_);
    internlmsg(INTNLMSG::RECEIVER recv_, unsigned int val_, std::string msg_);
    internlmsg(internlmsg&& oth);
    internlmsg(const internlmsg& oth);
    ~internlmsg();
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

#endif // INTERNLMSG_H
