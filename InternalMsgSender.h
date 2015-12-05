#ifndef InternalmsgSENDER_H
#define InternalmsgSENDER_H

#include "InternalMsgReceiver.h"
#include "InternalMsgRouter.h"

template<class D>
class InternalMsgSender
{    
protected:
    typedef InternalMsgReceiver<D> WORKER;
    InternalMsgRouter<D> * const internalMsgRouter;

public:
    InternalMsgSender(InternalMsgRouter<D> * const internalMsgRouter_);

    void send_internal_msg(D msg_);
    void send_internal_msg(INTERNALMESSAGE::RECEIVER recv_, unsigned int val_, std::string msg_);
};

template<class D>
InternalMsgSender<D>::InternalMsgSender(InternalMsgRouter<D> * const internalMsgRouter_) : internalMsgRouter(internalMsgRouter_)
{
}

template<class D>
void InternalMsgSender<D>::send_internal_msg(D msg_)
{
    (*internalMsgRouter) << msg_;
}

template<class D>
void InternalMsgSender<D>::send_internal_msg(INTERNALMESSAGE::RECEIVER recv_, unsigned int val_, std::string msg_)
{
    D msg(recv_, val_, std::move(msg_));
    (*internalMsgRouter) << msg;
}

#endif // InternalmsgSENDER_H
