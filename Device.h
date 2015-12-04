#ifndef Device_H
#define Device_H

#include <thread>
#include "Internalmsgreceiver.h"
#include "Internalmsgsender.h"
#include "Internalmsgrouter.h"

template<class D>
class Device : public Internalmsgsender<D>
{
protected:
    typedef typename Internalmsgreceiver<D>::HANDLE_RES INTMSGRES;
    typedef Internalmsgreceiver<D> WORKER;
    Internalmsgrouter<D> *Internalmsg_router;
    unsigned int pollperiod;
public:
    Device(Internalmsgrouter<D> * const Internalmsg_router_) : Internalmsgsender<D>(Internalmsg_router_), pollperiod(1){}
    virtual void operator()() = 0;
    virtual INTMSGRES HandleInternalMsg(D data) = 0;
    virtual ~Device() {}
};

#endif // Device_H
