#ifndef DeviceMANAGER_H
#define DeviceMANAGER_H

#include "Internalmsgreceiver.h"
#include "Internalmsgsender.h"
#include "Internalmsgrouter.h"
#include "Internalmsg.h"

template<class D>
class Devicemanager  : public Internalmsgreceiver<D>, public Internalmsgsender<D>
{
    int numOfDevices;
    Internalmsgrouter<D> * const Internalmsg_router;
    const char *numOfDevs;
    const char *shutdown;

    bool stop;    
public:
    Devicemanager(Internalmsgrouter<D> * const Internalmsg_router_);
    typename Internalmsgreceiver<D>::HANDLE_RES HandleMsg(D data);
    ~Devicemanager() {}
};

template<class D>
Devicemanager<D>::Devicemanager(Internalmsgrouter<D> * const Internalmsg_router_):
                                        Internalmsgreceiver<D>(INTNLMSG::RECV_Device_MANAGER),
                                        Internalmsgsender<D>(Internalmsg_router_),
                                        numOfDevices(0),
                                        Internalmsg_router(Internalmsg_router_),
                                        numOfDevs("[Device manager] number of Devices: "),
                                        shutdown("[Device manager] shutting down application")
{
    Internalmsg_router->register_receiver(this);
}

template<class D>
typename Internalmsgreceiver<D>::HANDLE_RES Devicemanager<D>::HandleMsg(D data)
{
    int whatHappened = data.getval();

    if (whatHappened == INTNLMSG::Device_SHUTDOWN)//devive closed
    {
        --numOfDevices;
    }
    else if(whatHappened == INTNLMSG::Device_ADDED)//Device connected
    {
        ++numOfDevices;
    }
    else if(whatHappened == INTNLMSG::GET_NUM_OF_DEVS)//remote console asked number of Devices
    {
        this->send_internal_msg(INTNLMSG::RECV_Device, INTNLMSG::GET_NUM_OF_DEVS,
                               std::string(numOfDevs) + std::to_string(numOfDevices));
    }
    else if(whatHappened == INTNLMSG::SHUTDOWN_ALL)//remote console ordered to shutdown application
    {
        this->Internalmsg_router->deregister_receiver(this);
        this->send_internal_msg(INTNLMSG::RECV_DISPLAY, INTNLMSG::SHOW_MESSAGE, std::string(shutdown));
        this->send_internal_msg(INTNLMSG::RECV_BROADCAST, INTNLMSG::SHUTDOWN_ALL, std::string(shutdown));
//        this->send_internal_msg(INTNLMSG::RECV_Netlistener, INTNLMSG::SHUTDOWN_ALL, std::string(shutdown));
//        this->send_internal_msg(INTNLMSG::RECV_Device, INTNLMSG::SHUTDOWN_ALL, std::string(shutdown));
//        this->send_internal_msg(INTNLMSG::RECV_DISPLAY, INTNLMSG::SHOW_MESSAGE, std::string(shutdown));
        this->stopthread();
    }

    return Internalmsgreceiver<D>::HANDLE_OK;
}

#endif // DeviceMANAGER_H
