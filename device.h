#ifndef DEVICE_H
#define DEVICE_H

#include <thread>
#include "internlmsgreceiver.h"
#include "internlmsgsender.h"
#include "internlmsgrouter.h"

template<class D>
class device : public internlmsgsender<D>
{
protected:
    typedef typename internlmsgreceiver<D>::HANDLE_RES INTMSGRES;
    typedef internlmsgreceiver<D> WORKER;
    internlmsgrouter<D> *internlmsg_router;
public:
    device(internlmsgrouter<D> * const internlmsg_router_) : internlmsgsender<D>(internlmsg_router_){}
    virtual void operator()() = 0;
    virtual INTMSGRES HandleInternalMsg(D data) = 0;
    virtual ~device() {}
};

#endif // DEVICE_H
