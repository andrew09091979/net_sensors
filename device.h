#ifndef DEVICE_H
#define DEVICE_H

#include <thread>
#include "internlmsgreceiver.h"
#include "internlmsgsender.h"

template<class D>
class device : public internlmsgsender<D>
{
protected:
    typedef typename internlmsgreceiver<D>::HANDLE_RES INTMSGRES;

public:
    device(){}
    virtual void operator()() = 0;
    virtual INTMSGRES HandleInternalMsg(D data) = 0;
    virtual ~device() {}
};

#endif // DEVICE_H
