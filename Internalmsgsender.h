#ifndef InternalmsgSENDER_H
#define InternalmsgSENDER_H

#include "Internalmsgreceiver.h"
#include "Internalmsgrouter.h"

template<class D>
class Internalmsgsender
{    
protected:
    typedef Internalmsgreceiver<D> WORKER;
    Internalmsgrouter<D> * const Internalmsg_router;

public:
    Internalmsgsender(Internalmsgrouter<D> * const Internalmsg_router_);

    void send_internal_msg(D msg_);
    void send_internal_msg(INTNLMSG::RECEIVER recv_, unsigned int val_, std::string msg_);
};

template<class D>
Internalmsgsender<D>::Internalmsgsender(Internalmsgrouter<D> * const Internalmsg_router_) : Internalmsg_router(Internalmsg_router_)
{
}

template<class D>
void Internalmsgsender<D>::send_internal_msg(D msg_)
{
    (*Internalmsg_router) << msg_;
}

template<class D>
void Internalmsgsender<D>::send_internal_msg(INTNLMSG::RECEIVER recv_, unsigned int val_, std::string msg_)
{
    D msg(recv_, val_, std::move(msg_));
    (*Internalmsg_router) << msg;
}

#endif // InternalmsgSENDER_H
