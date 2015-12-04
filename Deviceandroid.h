#ifndef DeviceANDROID_H
#define DeviceANDROID_H

#include <memory>
#include <vector>
#include <unistd.h>
#include "Device.h"
#include "arraywrapper.h"
#include "Internalmsgreceiver.h"
#include "Internalmsgsender.h"
#include "Internalmsgrouter.h"
#include "Protocolandroiddev.h"
#include "Connectionhandler.h"

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
class Deviceandroid : public Device<D>
{
    enum STATE
    {
        INITIAL,
        WORK,
        SLEEP,
        SHUTDOWN
    };

    class Internalmsgreceivr : public Internalmsgreceiver<D>
    {
        Device<D> *const dev;

    public:
        Internalmsgreceivr(Device<D> *const dev_, INTNLMSG::RECEIVER iam_) : Internalmsgreceiver<D>(iam_),
                                                                            dev(dev_)

        {
        }

        typename Internalmsgreceiver<D>::HANDLE_RES HandleMsg(D data)
        {
            typename Internalmsgreceiver<D>::HANDLE_RES res;
            res = dev->HandleInternalMsg(std::move(data));
            return res;
        }
    };

    typedef Internalmsgreceiver<D> WORKER;
    typedef typename Device<D>::INTMSGRES INTMSGRES;
    Internalmsgrouter<D> * const Internalmsg_router;
    std::shared_ptr<Protocol<char> > protocol_dev;
    std::string dev_name;
    Internalmsgreceivr *imr_ptr;
    bool stop;
    bool shutdown_ordered;
    STATE state;
    DEVCFG devConfig;
    std::string devName;
    std::string devCfg;
public:
    Deviceandroid(Internalmsgrouter<D> * const Internalmsg_router_,
           std::shared_ptr<Protocol<char> > protocol_);
    ~Deviceandroid(){}
    void operator()();
    INTMSGRES HandleInternalMsg(D data);
};

template <class D>
Deviceandroid<D>::Deviceandroid(Internalmsgrouter<D> * const Internalmsg_router_,
                                std::shared_ptr<Protocol<char> > protocol_) :
                                                              Device<D>(Internalmsg_router_),
                                                              Internalmsg_router(Internalmsg_router_),
                                                              protocol_dev(protocol_),
                                                              imr_ptr(nullptr),
                                                              stop(false),
                                                              shutdown_ordered(false),
                                                              state(INITIAL),
                                                              devName("Unknown Device")
{

}

template<class D>
void Deviceandroid<D>::operator()()
{
//    std::cout << "starting Dev thread" << std::endl;
    Internalmsgreceivr internalmsgreceiver(this, INTNLMSG::RECV_Device);
    imr_ptr = &internalmsgreceiver;
    std::reference_wrapper<Internalmsgreceivr> rv = std::reference_wrapper<Internalmsgreceivr>(internalmsgreceiver);
    std::thread thrd = std::thread(rv);
    Internalmsg_router->register_receiver(imr_ptr);

    while(!stop)
    {
        if (shutdown_ordered)
        {
            state = SHUTDOWN;
        }

        switch (state)
        {
            case INITIAL:
            {
                if (protocol_dev->getDeviceName(devName) == -1)
                {
                    this->send_internal_msg(INTNLMSG::RECV_DISPLAY, INTNLMSG::SHOW_MESSAGE,
                                           std::move(devName + std::string(" - can't get Device name")));
                    state = SHUTDOWN;
                }
                else
                {
                    this->send_internal_msg(INTNLMSG::RECV_DISPLAY, INTNLMSG::SHOW_MESSAGE,
                                           std::move(devName + std::string(" - got Device name")));
                    state = WORK;
                }
                if (protocol_dev->getDeviceConfig(devCfg) == -1)
                {
                    this->send_internal_msg(INTNLMSG::RECV_DISPLAY, INTNLMSG::SHOW_MESSAGE,
                                           std::move(devName + std::string(" - can't get Device name")));
                    state = SHUTDOWN;
                }
                else
                {
                    this->send_internal_msg(INTNLMSG::RECV_DISPLAY, INTNLMSG::SHOW_MESSAGE,
                                           std::move(devName + std::string(" - got Device config - ") + devCfg));
                    state = WORK;
                }
            }
            break;

            case WORK:
            {
                arraywrapper<char> data;

                if (protocol_dev->getData(0, data) == -1)
                {
                    this->send_internal_msg(INTNLMSG::RECV_DISPLAY, INTNLMSG::SHOW_MESSAGE,
                                           std::move(devName + std::string(" - can't get data")));
                    state = SHUTDOWN;
                }
                else
                {
                    this->send_internal_msg(INTNLMSG::RECV_DISPLAY, INTNLMSG::SHOW_MESSAGE,
                                           std::move(devName + std::string("\n") +
                                                     std::string(data.at(0))));
                    state = SLEEP;
                }
            }
            break;
            case SLEEP:
            {
                sleep(1);
                state = WORK;
            }
            break;
            case SHUTDOWN:
            {
                protocol_dev->shutdown();
                stop = true;
                this->send_internal_msg(INTNLMSG::RECV_DISPLAY, INTNLMSG::SHOW_MESSAGE,
                                       std::move(devName + std::string("- shutdown")));
                this->send_internal_msg(INTNLMSG::RECV_Device_MANAGER, INTNLMSG::Device_SHUTDOWN,
                                       std::move(devName + std::string("- shutdown")));
                Internalmsg_router->deregister_receiver(imr_ptr);
                this->imr_ptr->stopthread();

                if (thrd.joinable())
                    thrd.join();
            }
            break;
        }
    }
}

template <class D>
typename Deviceandroid<D>::INTMSGRES Deviceandroid<D>::HandleInternalMsg(D data)
{
    typename Deviceandroid<D>::INTMSGRES res = Internalmsgreceiver<D>::HANDLE_FAILED;
    int command = data.getval();

    switch (command)
    {
        case INTNLMSG::SHUTDOWN_ALL:
        {
            shutdown_ordered = true;
        }
        break;

        case INTNLMSG::GET_NUM_OF_DEVS://num_of_devs_demanded
        {
        }
        break;
    }

    return res;
}
#endif // DeviceANDROID_H
