#ifndef INTERNLMSGSENDER_H
#define INTERNLMSGSENDER_H

#include "internlmsgreceiver.h"
#include "internlmsgrouter.h"

template<class D>
class internlmsgsender
{    
protected:
    typedef internlmsgreceiver<D> WORKER;
    internlmsgrouter<D> * const internlmsg_router;

public:
    internlmsgsender(internlmsgrouter<D> * const internlmsg_router_);

    void send_internal_msg(D msg_);
    void send_internal_msg(INTNLMSG::RECEIVER recv_, unsigned int val_, std::string msg_);
};

template<class D>
internlmsgsender<D>::internlmsgsender(internlmsgrouter<D> * const internlmsg_router_) : internlmsg_router(internlmsg_router_)
{
}

template<class D>
void internlmsgsender<D>::send_internal_msg(D msg_)
{
    (*internlmsg_router) << msg_;
}

template<class D>
void internlmsgsender<D>::send_internal_msg(INTNLMSG::RECEIVER recv_, unsigned int val_, std::string msg_)
{
    D msg(recv_, val_, std::move(msg_));
    (*internlmsg_router) << msg;
}

#endif // INTERNLMSGSENDER_H
