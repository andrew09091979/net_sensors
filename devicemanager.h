#ifndef DEVICEMANAGER_H
#define DEVICEMANAGER_H

#include "internlmsgreceiver.h"
#include "internlmsgsender.h"
#include "internlmsgrouter.h"
#include "internlmsg.h"

template<class D>
class devicemanager  : public internlmsgreceiver<D>, public internlmsgsender<D>
{
    int numOfDevices;
    const char *numOfDevs;
    const char *shutdown;

    bool stop;    
public:
    devicemanager(internlmsgrouter<D> * const internlmsg_router_);
    typename internlmsgreceiver<D>::HANDLE_RES HandleMsg(D data);
    ~devicemanager() {}
};

template<class D>
devicemanager<D>::devicemanager(internlmsgrouter<D> * const internlmsg_router_):
                                        internlmsgreceiver<D>(INTNLMSG::RECV_DEVICE_MANAGER),
                                        internlmsgsender<D>(internlmsg_router_),
                                        numOfDevices(0),
                                        numOfDevs("[device manager] number of devices: "),
                                        shutdown("[device manager] shutting down application")
{
}

template<class D>
typename internlmsgreceiver<D>::HANDLE_RES devicemanager<D>::HandleMsg(D data)
{
    int whatHappened = data.getval();

    if (whatHappened == INTNLMSG::DEVICE_SHUTDOWN)//devive closed
    {
        --numOfDevices;
    }
    else if(whatHappened == INTNLMSG::DEVICE_ADDED)//device connected
    {
        ++numOfDevices;
    }
    else if(whatHappened == INTNLMSG::GET_NUM_OF_DEVS)//remote console asked number of devices
    {
        this->send_internl_msg(INTNLMSG::RECV_DEVICE, INTNLMSG::GET_NUM_OF_DEVS,
                               std::string(numOfDevs) + std::to_string(numOfDevices));
    }
    else if(whatHappened == INTNLMSG::SHUTDOWN_ALL)//remote console ordered to shutdown application
    {
        this->internlmsg_router->deregister_receiver(this);
        this->send_internl_msg(INTNLMSG::RECV_BROADCAST, INTNLMSG::SHUTDOWN_ALL, std::string(shutdown));
//        this->send_internl_msg(INTNLMSG::RECV_NETLISTENER, INTNLMSG::SHUTDOWN_ALL, std::string(shutdown));
//        this->send_internl_msg(INTNLMSG::RECV_DEVICE, INTNLMSG::SHUTDOWN_ALL, std::string(shutdown));
//        this->send_internl_msg(INTNLMSG::RECV_DISPLAY, INTNLMSG::SHOW_MESSAGE, std::string(shutdown));
        this->stopthread();
    }

    return internlmsgreceiver<D>::HANDLE_OK;
}

#endif // DEVICEMANAGER_H
