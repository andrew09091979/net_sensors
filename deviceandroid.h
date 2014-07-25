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

template <class D>
class deviceandroid : public device<D>
{
    enum STATE
    {
        INITIAL,
        WORK,
        SHUTDOWN
    };

    typedef internlmsgreceiver<D> WORKER;
    const modulemanager<D> * const mod_mgr;
    std::shared_ptr<protocol<char> > protocol_dev;
    std::string dev_name;

    bool stop;
    STATE state;
    std::string devName;
public:
    deviceandroid(const modulemanager<D> * const mod_mgr_,
           std::shared_ptr<protocol<char> > protocol_);
    ~deviceandroid(){}
    void operator()();
//    typename internlmsgreceiver<D>::HANDLE_RES HandleMsg(D data);
};

template <class D>
deviceandroid<D>::deviceandroid(const modulemanager<D> * const mod_mgr_,
                  std::shared_ptr<protocol<char> > protocol_) : mod_mgr(mod_mgr_),
                                                              protocol_dev(protocol_),
                                                              stop(false),
                                                              state(INITIAL),
                                                              //state(WORK),
                                                              devName("Unknown device")
{
    std::vector<INTNLMSG::RECEIVER> receivers_to_get;
    receivers_to_get.push_back(INTNLMSG::RECEIVER::RECV_DISPLAY);
    mod_mgr->get_receivers(receivers_to_get, this->workers);
}

template<class D>
void deviceandroid<D>::operator()()
{
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
            }
            break;
        }
    }
}

//template <class D>
//typename internlmsgreceiver<D>::HANDLE_RES deviceandroid<D>::HandleMsg(D data)
//{
//    return internlmsgreceiver<D>::HANDLE_OK;
//}
#endif // DEVICEANDROID_H
