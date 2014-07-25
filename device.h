#ifndef DEVICE_H
#define DEVICE_H

#include "internlmsgreceiver.h"
#include "internlmsgsender.h"

template<class D>
class device  :/* public internlmsgreceiver<D>, */public internlmsgsender<D>
{
public:
//    device() : internlmsgreceiver<D>(INTNLMSG::RECV_DEVICE){}
    virtual void operator()() = 0;
//    typename internlmsgreceiver<D>::HANDLE_RES HandleMsg(D data) = 0;
    virtual ~device() {}
};

#endif // DEVICE_H
