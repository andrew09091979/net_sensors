#ifndef DeviceMANAGER_H
#define DeviceMANAGER_H

#include <map>
#include "InternalMsgReceiver.h"
#include "InternalMsgSender.h"
#include "InternalMsgRouter.h"
#include "InternalMsg.h"

template<class D>
class DeviceManager  : public InternalMsgReceiver<D>, public InternalMsgSender<D>
{
    typedef typename InternalMsgReceiver<D>::HANDLE_RES (DeviceManager::*HANDLER)(D);
    std::map<INTERNALMESSAGE::MSG_TYPE, HANDLER> handleInternalMsg;

    int numOfDevices;
    InternalMsgRouter<D> * const Internalmsg_router;
    const char *numOfDevs;
    const char *shutdown;
    typename InternalMsgReceiver<D>::HANDLE_RES HandleDeviceAdded(D data);
    typename InternalMsgReceiver<D>::HANDLE_RES HandleGetNumOfDevs(D data);
    typename InternalMsgReceiver<D>::HANDLE_RES HandleDeviceShutDown(D data);
    typename InternalMsgReceiver<D>::HANDLE_RES HandleShutdownAll(D data);
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
    handleInternalMsg.insert(std::pair<INTERNALMESSAGE::MSG_TYPE, HANDLER>(INTERNALMESSAGE::DEVICE_ADDED,
                                                                           &DeviceManager<D>::HandleDeviceAdded));
    handleInternalMsg.insert(std::pair<INTERNALMESSAGE::MSG_TYPE, HANDLER>(INTERNALMESSAGE::GET_NUM_OF_DEVS,
                                                                           &DeviceManager<D>::HandleGetNumOfDevs));
    handleInternalMsg.insert(std::pair<INTERNALMESSAGE::MSG_TYPE, HANDLER>(INTERNALMESSAGE::DEVICE_SHUTDOWN,
                                                                           &DeviceManager<D>::HandleDeviceShutDown));
    handleInternalMsg.insert(std::pair<INTERNALMESSAGE::MSG_TYPE, HANDLER>(INTERNALMESSAGE::SHUTDOWN_ALL,
                                                                           &DeviceManager<D>::HandleShutdownAll));
    Internalmsg_router->register_receiver(this);
}

template<class D>
typename InternalMsgReceiver<D>::HANDLE_RES DeviceManager<D>::HandleMsg(D data)
{
    INTERNALMESSAGE::MSG_TYPE whatHappened = INTERNALMESSAGE::MSG_TYPE(data.getval());
    HANDLER hnd = handleInternalMsg.at(whatHappened);
    (this->*hnd)(data);

    return InternalMsgReceiver<D>::HANDLE_OK;
}
template<class D>
typename InternalMsgReceiver<D>::HANDLE_RES DeviceManager<D>::HandleDeviceAdded(D data)
{
    ++numOfDevices;

    return InternalMsgReceiver<D>::HANDLE_OK;
}

template<class D>
typename InternalMsgReceiver<D>::HANDLE_RES DeviceManager<D>::HandleGetNumOfDevs(D data)
{
    this->send_internal_msg(INTERNALMESSAGE::RECV_DEVICE, INTERNALMESSAGE::GET_NUM_OF_DEVS,
                            std::string(numOfDevs) + std::to_string(numOfDevices) + std::string("\n"));

    return InternalMsgReceiver<D>::HANDLE_OK;
}

template<class D>
typename InternalMsgReceiver<D>::HANDLE_RES DeviceManager<D>::HandleDeviceShutDown(D data)
{
    --numOfDevices;

    return InternalMsgReceiver<D>::HANDLE_OK;
}

template<class D>
typename InternalMsgReceiver<D>::HANDLE_RES DeviceManager<D>::HandleShutdownAll(D data)
{
    this->Internalmsg_router->deregister_receiver(this);
    this->send_internal_msg(INTERNALMESSAGE::RECV_DISPLAY, INTERNALMESSAGE::SHOW_MESSAGE, std::string(shutdown));
    this->send_internal_msg(INTERNALMESSAGE::RECV_BROADCAST, INTERNALMESSAGE::SHUTDOWN_ALL, std::string(shutdown));
//        this->send_internal_msg(INTERNALMESSAGE::RECV_Netlistener, INTERNALMESSAGE::SHUTDOWN_ALL, std::string(shutdown));
//        this->send_internal_msg(INTERNALMESSAGE::RECV_DEVICE, INTERNALMESSAGE::SHUTDOWN_ALL, std::string(shutdown));
//        this->send_internal_msg(INTERNALMESSAGE::RECV_DISPLAY, INTERNALMESSAGE::SHOW_MESSAGE, std::string(shutdown));
    this->stopthread();

    return InternalMsgReceiver<D>::HANDLE_OK;
}

#endif // DeviceMANAGER_H
