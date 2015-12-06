#ifndef Device_H
#define Device_H

#include <thread>
#include "InternalMsgReceiver.h"
#include "InternalMsgSender.h"
#include "InternalMsgRouter.h"

template<class D>
class Device : public InternalMsgSender<D>
{
protected:
    typedef typename InternalMsgReceiver<D>::HANDLE_RES INTERNAL_MSG_HANDLE_RES;
    typedef InternalMsgReceiver<D> WORKER;
    InternalMsgRouter<D> *Internalmsg_router;
    unsigned int pollperiod;
public:
    Device(InternalMsgRouter<D> * const Internalmsg_router_) : InternalMsgSender<D>(Internalmsg_router_), pollperiod(1){}
    virtual void operator()() = 0;
    virtual INTERNAL_MSG_HANDLE_RES HandleInternalMsg(D data) = 0;
    virtual ~Device() {}
};

#endif // Device_H
