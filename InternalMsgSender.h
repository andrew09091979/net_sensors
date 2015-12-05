#ifndef InternalmsgSENDER_H
#define InternalmsgSENDER_H

#include "InternalMsgReceiver.h"
#include "InternalMsgRouter.h"

template<class D>
class InternalMsgSender
{    
protected:
    typedef InternalMsgReceiver<D> WORKER;
    InternalMsgRouter<D> * const Internalmsg_router;

public:
    InternalMsgSender(InternalMsgRouter<D> * const Internalmsg_router_);

    void send_internal_msg(D msg_);
    void send_internal_msg(INTNLMSG::RECEIVER recv_, unsigned int val_, std::string msg_);
};

template<class D>
InternalMsgSender<D>::InternalMsgSender(InternalMsgRouter<D> * const Internalmsg_router_) : Internalmsg_router(Internalmsg_router_)
{
}

template<class D>
void InternalMsgSender<D>::send_internal_msg(D msg_)
{
    (*Internalmsg_router) << msg_;
}

template<class D>
void InternalMsgSender<D>::send_internal_msg(INTNLMSG::RECEIVER recv_, unsigned int val_, std::string msg_)
{
    D msg(recv_, val_, std::move(msg_));
    (*Internalmsg_router) << msg;
}

#endif // InternalmsgSENDER_H
