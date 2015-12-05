#ifndef DeviceMANAGER_H
#define DeviceMANAGER_H

#include "InternalMsgReceiver.h"
#include "InternalMsgSender.h"
#include "InternalMsgRouter.h"
#include "InternalMsg.h"

template<class D>
class DeviceManager  : public InternalMsgReceiver<D>, public InternalMsgSender<D>
{
    int numOfDevices;
    InternalMsgRouter<D> * const Internalmsg_router;
    const char *numOfDevs;
    const char *shutdown;

    bool stop;    
public:
    DeviceManager(InternalMsgRouter<D> * const Internalmsg_router_);
    typename InternalMsgReceiver<D>::HANDLE_RES HandleMsg(D data);
    ~DeviceManager() {}
};

template<class D>
DeviceManager<D>::DeviceManager(InternalMsgRouter<D> * const Internalmsg_router_):
                                        InternalMsgReceiver<D>(INTERNALMESSAGE::RECV_DEVICE_MANAGER),
                                        InternalMsgSender<D>(Internalmsg_router_),
                                        numOfDevices(0),
                                        Internalmsg_router(Internalmsg_router_),
                                        numOfDevs("[Device manager] number of Devices: "),
                                        shutdown("[Device manager] shutting down application")
{
    Internalmsg_router->register_receiver(this);
}

template<class D>
typename InternalMsgReceiver<D>::HANDLE_RES DeviceManager<D>::HandleMsg(D data)
{
    int whatHappened = data.getval();

    if (whatHappened == INTERNALMESSAGE::DEVICE_SHUTDOWN)//devive closed
    {
        --numOfDevices;
    }
    else if(whatHappened == INTERNALMESSAGE::DEVICE_ADDED)//Device connected
    {
        ++numOfDevices;
    }
    else if(whatHappened == INTERNALMESSAGE::GET_NUM_OF_DEVS)//remote console asked number of Devices
    {
        this->send_internal_msg(INTERNALMESSAGE::RECV_DEVICE, INTERNALMESSAGE::GET_NUM_OF_DEVS,
                                std::string(numOfDevs) + std::to_string(numOfDevices) + std::string("\n"));
    }
    else if(whatHappened == INTERNALMESSAGE::SHUTDOWN_ALL)//remote console ordered to shutdown application
    {
        this->Internalmsg_router->deregister_receiver(this);
        this->send_internal_msg(INTERNALMESSAGE::RECV_DISPLAY, INTERNALMESSAGE::SHOW_MESSAGE, std::string(shutdown));
        this->send_internal_msg(INTERNALMESSAGE::RECV_BROADCAST, INTERNALMESSAGE::SHUTDOWN_ALL, std::string(shutdown));
//        this->send_internal_msg(INTERNALMESSAGE::RECV_Netlistener, INTERNALMESSAGE::SHUTDOWN_ALL, std::string(shutdown));
//        this->send_internal_msg(INTERNALMESSAGE::RECV_DEVICE, INTERNALMESSAGE::SHUTDOWN_ALL, std::string(shutdown));
//        this->send_internal_msg(INTERNALMESSAGE::RECV_DISPLAY, INTERNALMESSAGE::SHOW_MESSAGE, std::string(shutdown));
        this->stopthread();
    }

    return InternalMsgReceiver<D>::HANDLE_OK;
}

#endif // DeviceMANAGER_H
