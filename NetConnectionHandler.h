#ifndef Netconnectionhandler_H
#define Netconnectionhandler_H

#include <mutex>
#include <queue>
#include <condition_variable>
#include <thread>
#include <map>
#include "InternalMsgReceiver.h"
#include "InternalMsgSender.h"
#include "InternalMsgRouter.h"
#include "InternalMsg.h"
#include "DeviceAndroid.h"
#include "DeviceRemConsole.h"
#include "ProtocolAndroidDev.h"
#include "ProtocolRemConsole.h"
#include "ConnectionHandler.h"

template<class D>
class NetConnectionHandler : public InternalMsgReceiver<D>, public InternalMsgSender<D>
{
    typedef InternalMsgReceiver<D> WORKER;
    typedef typename InternalMsgReceiver<D>::HANDLE_RES (NetConnectionHandler::*DEVICE_FABRIC)();
    InternalMsgRouter<D> * const Internalmsg_router;
    const char * start_clientserv;
    const char * sock_is_invalid;
    const char * invalid_dev_type;
    const char * cannot_get_dev_type;
    int sock;
    std::map<char, DEVICE_FABRIC> dev_fabric;
    typename InternalMsgReceiver<D>::HANDLE_RES createDeviceAndroid();
    typename InternalMsgReceiver<D>::HANDLE_RES createDeviceRemConsole();
    typename InternalMsgReceiver<D>::HANDLE_RES invalidDeviceType();
public:
    typename InternalMsgReceiver<D>::HANDLE_RES HandleMsg(D data);
    NetConnectionHandler(InternalMsgRouter<D> * const Internalmsg_router_);
    ~NetConnectionHandler(){}
};

template<class D>
NetConnectionHandler<D>::NetConnectionHandler(InternalMsgRouter<D> * const Internalmsg_router_) :
                                                    InternalMsgReceiver<D>(INTERNALMESSAGE::RECV_NETCONNHANDLER),
                                                    InternalMsgSender<D>(Internalmsg_router_),
                                                    Internalmsg_router(Internalmsg_router_),
                                                    start_clientserv("[NetConnectionHandler] starting Device"),
                                                    sock_is_invalid("[NetConnectionHandler] socket is invalid"),
                                                    invalid_dev_type("[NetConnectionHandler] invalid Device type"),
                                                    cannot_get_dev_type("[NetConnectionHandler] cannot get Device type")
{
    dev_fabric.insert(std::pair<char, DEVICE_FABRIC>('1', &NetConnectionHandler<D>::createDeviceAndroid));
    dev_fabric.insert(std::pair<char, DEVICE_FABRIC>('2', &NetConnectionHandler<D>::createDeviceRemConsole));
    Internalmsg_router->register_receiver(this);
}

template<class D>
typename InternalMsgReceiver<D>::HANDLE_RES NetConnectionHandler<D>::HandleMsg(D data)
{
    typename InternalMsgReceiver<D>::HANDLE_RES res = InternalMsgReceiver<D>::HANDLE_FAILED;

    if (data.getreceiver() == INTERNALMESSAGE::RECV_NETCONNHANDLER)
    {
        sock = data.getval();

        if (sock != -1)
        {
            ConnectionHandler ch(sock);
            char dev_type[3];

            if (ch.read_nbytes(dev_type, 3))
            {
                try
                {
                    DEVICE_FABRIC dev_fab = dev_fabric.at(dev_type[0]);
                    (this->*dev_fab)();
                }
                catch (const std::out_of_range& oor)
                {
                    this->invalidDeviceType();
                }
            }
            else
            {
                this->send_internal_msg(INTERNALMESSAGE::RECV_DISPLAY, INTERNALMESSAGE::SHOW_MESSAGE,
                                       std::move(std::string(cannot_get_dev_type)));
            }
        }
        else
        {
            this->send_internal_msg(INTERNALMESSAGE::RECV_DISPLAY, INTERNALMESSAGE::SHOW_MESSAGE, std::move(std::string(sock_is_invalid)));
        }
    }
    else if (data.getreceiver() == INTERNALMESSAGE::RECV_BROADCAST)
    {
        int command = data.getval();

        switch (command)
        {
            case INTERNALMESSAGE::SHUTDOWN_ALL:
            {
                Internalmsg_router->deregister_receiver(this);
                this->stopthread();
            }
            break;

            case INTERNALMESSAGE::GET_NUM_OF_DEVS://num_of_devs_demanded
            {
            }
            break;
        }
    }
    return res;
}

template<class D>
typename InternalMsgReceiver<D>::HANDLE_RES NetConnectionHandler<D>::createDeviceAndroid()
{
    std::shared_ptr<ConnectionHandler> conn(new ConnectionHandler(sock));
    std::shared_ptr<Protocol<char>> prot(new ProtocolAndroidDev(conn));
    DeviceAndroid<D> dev(Internalmsg_router, prot);

    this->send_internal_msg(INTERNALMESSAGE::RECV_DISPLAY, INTERNALMESSAGE::SHOW_MESSAGE,
                           std::move(std::string(start_clientserv)));
    this->send_internal_msg(INTERNALMESSAGE::RECV_DEVICE_MANAGER, INTERNALMESSAGE::DEVICE_ADDED,
                           std::move(std::string(start_clientserv)));
    //std::thread thr = std::thread(std::reference_wrapper<DeviceAndroid<D>>(dev));
    std::thread thr = std::thread(dev);
    thr.detach();
    return InternalMsgReceiver<D>::HANDLE_OK;
}

template<class D>
typename InternalMsgReceiver<D>::HANDLE_RES NetConnectionHandler<D>::createDeviceRemConsole()
{
    std::shared_ptr<ConnectionHandler> conn(new ConnectionHandler(sock));
    std::shared_ptr<Protocol<char>> prot(new ProtocolRemConsole(conn));
    DeviceRemConsole<D> dev(Internalmsg_router, prot);

    this->send_internal_msg(INTERNALMESSAGE::RECV_DISPLAY, INTERNALMESSAGE::SHOW_MESSAGE,
                           std::move(std::string(start_clientserv)));
    this->send_internal_msg(INTERNALMESSAGE::RECV_DEVICE_MANAGER, INTERNALMESSAGE::DEVICE_ADDED,
                           std::move(std::string(start_clientserv)));
    //std::thread thr = std::thread(std::reference_wrapper<DeviceRemConsole<D>>(dev));
    std::thread thr = std::thread(dev);
    thr.detach();
    return InternalMsgReceiver<D>::HANDLE_OK;
}

template<class D>
typename InternalMsgReceiver<D>::HANDLE_RES NetConnectionHandler<D>::invalidDeviceType()
{
    this->send_internal_msg(INTERNALMESSAGE::RECV_DISPLAY, INTERNALMESSAGE::SHOW_MESSAGE,
                           std::move(std::string(invalid_dev_type)));

    return InternalMsgReceiver<D>::HANDLE_OK;
}
#endif // Netconnectionhandler_H
