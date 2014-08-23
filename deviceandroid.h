#ifndef DEVICEANDROID_H
#define DEVICEANDROID_H

#include <memory>
#include <vector>
#include "device.h"
#include "arraywrapper.h"
#include "internlmsgreceiver.h"
#include "internlmsgsender.h"
#include "modulemanager.h"
#include "protocolandroiddev.h"
#include "connectionhandler.h"

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
class deviceandroid : public device<D>
{
    enum STATE
    {
        INITIAL,
        WORK,
        SHUTDOWN
    };

    class internlmsgreceivr : public internlmsgreceiver<D>
    {
        device<D> *const dev;

    public:
        internlmsgreceivr(device<D> * const dev_, INTNLMSG::RECEIVER iam_) : dev(dev_),
                                                                             internlmsgreceiver<D>(iam_)
        {

        }

        typename internlmsgreceiver<D>::HANDLE_RES HandleMsg(D data)
        {
            return dev->HandleInternalMsg(std::move(data));
        }
    };

    typedef internlmsgreceiver<D> WORKER;
    typedef typename device<D>::INTMSGRES INTMSGRES;
    modulemanager<D> * const mod_mgr;
    std::shared_ptr<protocol<char> > protocol_dev;
    std::string dev_name;

    bool stop;
    STATE state;
    internlmsgreceivr internalmsgreceiver;
    DEVCFG devConfig;
    std::string devName;
    std::string devCfg;
public:
    deviceandroid(modulemanager<D> * const mod_mgr_,
           std::shared_ptr<protocol<char> > protocol_);
    ~deviceandroid(){}
    void operator()();
    INTMSGRES HandleInternalMsg(D data);
};

template <class D>
deviceandroid<D>::deviceandroid(modulemanager<D> * const mod_mgr_,
                  std::shared_ptr<protocol<char> > protocol_) : mod_mgr(mod_mgr_),
                                                              protocol_dev(protocol_),
                                                              stop(false),
                                                              state(INITIAL),
                                                              internalmsgreceiver(this, INTNLMSG::RECV_DEVICE),
                                                              //state(WORK),
                                                              devName("Unknown device")
{

}

template<class D>
void deviceandroid<D>::operator()()
{
    std::thread thrd = std::thread(std::reference_wrapper<internlmsgreceivr>(internalmsgreceiver));
    thrd.detach();
    mod_mgr->register_receiver(&this->internalmsgreceiver);
    std::vector<INTNLMSG::RECEIVER> receivers_to_get;
    receivers_to_get.push_back(INTNLMSG::RECEIVER::RECV_DISPLAY);
    receivers_to_get.push_back(INTNLMSG::RECEIVER::RECV_DEVICE_MANAGER);
    mod_mgr->get_receivers(receivers_to_get, this->workers);

    while(!stop)
    {
        switch (state)
        {
            case INITIAL:
            {
                if (protocol_dev->getDeviceName(devName) == -1)
                {
                    this->send_internl_msg(INTNLMSG::RECV_DISPLAY, 0,
                                           std::move(devName + std::string(" - can't get device name")));
                    state = SHUTDOWN;
                }
                else
                {
                    this->send_internl_msg(INTNLMSG::RECV_DISPLAY, 0,
                                           std::move(devName + std::string(" - got device name")));
                    state = WORK;
                }
                if (protocol_dev->getDeviceConfig(devCfg) == -1)
                {
                    this->send_internl_msg(INTNLMSG::RECV_DISPLAY, 0,
                                           std::move(devName + std::string(" - can't get device name")));
                    state = SHUTDOWN;
                }
                else
                {
                    this->send_internl_msg(INTNLMSG::RECV_DISPLAY, 0,
                                           std::move(devName + std::string(" - got device config - ") + devCfg));
                    state = WORK;
                }
            }
            break;

            case WORK:
            {
                arraywrapper<char> data;

                if (protocol_dev->getData(0, data) == -1)
                {
                    this->send_internl_msg(INTNLMSG::RECV_DISPLAY, 0,
                                           std::move(devName + std::string(" - can't get data")));
                    state = SHUTDOWN;
                }
                else
                {
                    this->send_internl_msg(INTNLMSG::RECV_DISPLAY, 0,
                                           std::move(devName + std::string(" - ") +
                                                     std::string(data.at(0))));
                }
            }
            break;

            case SHUTDOWN:
            {
                protocol_dev->shutdown();
                stop = true;
                this->send_internl_msg(INTNLMSG::RECV_DISPLAY, 0,
                                       std::move(devName + std::string("- shutdown")));
                this->send_internl_msg(INTNLMSG::RECV_DEVICE_MANAGER, 0,
                                       std::move(devName + std::string("- shutdown")));
                this->internalmsgreceiver << D(INTNLMSG::RECV_DEVICE, -1, std::move(std::string("exit")));
            }
            break;
        }
    }
}

template <class D>
typename deviceandroid<D>::INTMSGRES deviceandroid<D>::HandleInternalMsg(D data)
{
    typename deviceandroid<D>::INTMSGRES res = internlmsgreceiver<D>::HANDLE_FAILED;
    int command = data.getval();

    switch (command)
    {
        case -1:
        {
            this->internalmsgreceiver.stopthread();
        }
        break;

        case 3://num_of_devs_demanded
        {
        }
        break;
    }

    return res;
}
#endif // DEVICEANDROID_H
