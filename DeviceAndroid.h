#ifndef DeviceANDROID_H
#define DeviceANDROID_H

#include <memory>
#include <vector>
#include <unistd.h>
#include <map>
#include "Device.h"
#include "arraywrapper.h"
#include "InternalMsgReceiver.h"
#include "InternalMsgSender.h"
#include "InternalMsgRouter.h"
#include "ProtocolAndroidDev.h"
#include "ConnectionHandler.h"

#define MESSAGE_CORRUPTED "Message corrupted"
#define SENDING_MSG "Sending message"
#define RECEIVING_MSG "Receiving message"
#define ERROR_SENDING_MSG "Message sending failed"
#define ERROR_RECEIVING_MSG "Message receiving failed"

struct CHANCFG
{
    std::string chanName;
    float max_val;
    float min_val;
};

struct DEVCFG
{
    std::string devName;
    std::vector<CHANCFG> chanCfg;
};

template <class D>
class DeviceAndroid : public Device<D>
{
    enum STATE
    {
        INITIAL,
        WORK,
        SLEEP,
        SHUTDOWN
    };

    class Internalmsgreceivr : public InternalMsgReceiver<D>
    {
        Device<D> *const dev;

    public:
        Internalmsgreceivr(Device<D> *const dev_, INTERNALMESSAGE::RECEIVER iam_) : InternalMsgReceiver<D>(iam_),
                                                                            dev(dev_)

        {
        }

        typename InternalMsgReceiver<D>::HANDLE_RES HandleMsg(D data)
        {
            typename InternalMsgReceiver<D>::HANDLE_RES res;
            res = dev->HandleInternalMsg(std::move(data));
            return res;
        }
    };

    typedef InternalMsgReceiver<D> WORKER;
    typedef typename Device<D>::INTERNAL_MSG_HANDLE_RES INTMSGRES;
    typedef bool (DeviceAndroid::*HANDLER)();
    InternalMsgRouter<D> * const internalmsg_router;
    std::shared_ptr<Protocol<char> > protocol_dev;
    std::string dev_name;
    Internalmsgreceivr *imr_ptr;
    bool stop;
    bool shutdown_ordered;
    STATE state;
    DEVCFG devConfig;
    std::string devName;
    std::string devCfg;

    std::map<STATE,HANDLER> handleState;
    bool doInitial();
    bool doWork();
    bool doSleep();
    bool doShutdown();
public:
    DeviceAndroid(InternalMsgRouter<D> * const internalmsg_router_,
           std::shared_ptr<Protocol<char> > protocol_);
    ~DeviceAndroid(){}
    void operator()();
    INTMSGRES HandleInternalMsg(D data);
};

template <class D>
DeviceAndroid<D>::DeviceAndroid(InternalMsgRouter<D> * const internalmsg_router_,
                                std::shared_ptr<Protocol<char> > protocol_) :
                                                              Device<D>(internalmsg_router_),
                                                              internalmsg_router(internalmsg_router_),
                                                              protocol_dev(protocol_),
                                                              imr_ptr(nullptr),
                                                              stop(false),
                                                              shutdown_ordered(false),
                                                              state(INITIAL),
                                                              devName("Unknown Device")
{
    handleState.insert(std::pair<STATE, HANDLER>(INITIAL, &DeviceAndroid<D>::doInitial));
    handleState.insert(std::pair<STATE, HANDLER>(WORK, &DeviceAndroid<D>::doWork));
    handleState.insert(std::pair<STATE, HANDLER>(SLEEP, &DeviceAndroid<D>::doSleep));
    handleState.insert(std::pair<STATE, HANDLER>(SHUTDOWN, &DeviceAndroid<D>::doShutdown));
}

template<class D>
void DeviceAndroid<D>::operator()()
{
//    std::cout << "starting Dev thread" << std::endl;
    Internalmsgreceivr internalMsgReceiver(this, INTERNALMESSAGE::RECV_DEVICE);
    imr_ptr = &internalMsgReceiver;
    std::reference_wrapper<Internalmsgreceivr> rv = std::reference_wrapper<Internalmsgreceivr>(internalMsgReceiver);
    std::thread internal_msg_receiver_thrd= std::thread(rv);
    internalmsg_router->register_receiver(imr_ptr);

    while(!stop)
    {
        if (shutdown_ordered)
        {
            state = SHUTDOWN;
        }

        try
        {
            HANDLER hnd = handleState.at(state);
            (this->*hnd)();
        }
        catch(std::out_of_range & oor)
        {

        }
    }
    if (internal_msg_receiver_thrd.joinable())
        internal_msg_receiver_thrd.join();
}

template <class D>
typename DeviceAndroid<D>::INTMSGRES DeviceAndroid<D>::HandleInternalMsg(D data)
{
    typename DeviceAndroid<D>::INTMSGRES res = InternalMsgReceiver<D>::HANDLE_FAILED;
    int command = data.getval();

    switch (command)
    {
        case INTERNALMESSAGE::SHUTDOWN_ALL:
        {
            shutdown_ordered = true;
        }
        break;

        case INTERNALMESSAGE::GET_NUM_OF_DEVS://num_of_devs_demanded
        {
        }
        break;
    }

    return res;
}

template <class D>
bool DeviceAndroid<D>::doInitial()
{
    bool res = false;

    if (protocol_dev->getDeviceName(devName) == -1)
    {
        this->send_internal_msg(INTERNALMESSAGE::RECV_DISPLAY, INTERNALMESSAGE::SHOW_MESSAGE,
                               std::move(devName + std::string(" - can't get Device name")));
        state = SHUTDOWN;
    }
    else
    {
        this->send_internal_msg(INTERNALMESSAGE::RECV_DISPLAY, INTERNALMESSAGE::SHOW_MESSAGE,
                               std::move(devName + std::string(" - got Device name")));
        state = WORK;
    }
    if (protocol_dev->getDeviceConfig(devCfg) == -1)
    {
        this->send_internal_msg(INTERNALMESSAGE::RECV_DISPLAY, INTERNALMESSAGE::SHOW_MESSAGE,
                               std::move(devName + std::string(" - can't get Device name")));
        state = SHUTDOWN;
    }
    else
    {
        this->send_internal_msg(INTERNALMESSAGE::RECV_DISPLAY, INTERNALMESSAGE::SHOW_MESSAGE,
                               std::move(devName + std::string(" - got Device config - ") + devCfg));
        state = WORK;
    }
    return res;
}

template <class D>
bool DeviceAndroid<D>::doWork()
{
    bool res = false;
    arraywrapper<char> data;

    if (protocol_dev->getData(0, data) == -1)
    {
        this->send_internal_msg(INTERNALMESSAGE::RECV_DISPLAY, INTERNALMESSAGE::SHOW_MESSAGE,
                               std::move(devName + std::string(" - can't get data")));
        state = SHUTDOWN;
    }
    else
    {
        this->send_internal_msg(INTERNALMESSAGE::RECV_DISPLAY, INTERNALMESSAGE::SHOW_MESSAGE,
                               std::move(devName + std::string("\n") +
                                         std::string(data.at(0))));
        this->send_internal_msg(INTERNALMESSAGE::RECV_FILE, INTERNALMESSAGE::STORE_MESSAGE,
                                std::move(devName + std::string("\n") +
                                          std::string(data.at(0))));
        state = SLEEP;
    }
    return res;
}

template <class D>
bool DeviceAndroid<D>::doSleep()
{
    bool res = false;

    std::this_thread::sleep_for(std::chrono::seconds(1));
    state = WORK;
    return res;
}

template <class D>
bool DeviceAndroid<D>::doShutdown()
{
    bool res = false;

    protocol_dev->shutdown();
    stop = true;
    this->send_internal_msg(INTERNALMESSAGE::RECV_DISPLAY, INTERNALMESSAGE::SHOW_MESSAGE,
                           std::move(devName + std::string("- shutdown")));
    this->send_internal_msg(INTERNALMESSAGE::RECV_DEVICE_MANAGER, INTERNALMESSAGE::DEVICE_SHUTDOWN,
                           std::move(devName + std::string("- shutdown")));
    internalmsg_router->deregister_receiver(imr_ptr);
    this->imr_ptr->stopthread();

    return res;
}
#endif // DeviceANDROID_H
