#ifndef DEVICE_H
#define DEVICE_H

#include "internlmsgreceiver.h"
#include "internlmsgsender.h"

template<class D>
class device  : public internlmsgreceiver<D>, public internlmsgsender<D>
{
protected:
//    typedef
public:
    device() : internlmsgreceiver<D>(INTNLMSG::RECV_DEVICE){}
    virtual void operator()() = 0;
    virtual ~device() {}
};

#endif // DEVICE_H
