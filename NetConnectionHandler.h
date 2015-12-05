#ifndef Netconnectionhandler_H
#define Netconnectionhandler_H

#include <mutex>
#include <queue>
#include <condition_variable>
#include <thread>
#include "InternalMsgReceiver.h"
#include "InternalMsgSender.h"
#include "InternalMsgRouter.h"
#include "InternalMsg.h"
#include "DeviceAndroid.h"
#include "DeviceRemConsole.h"
#include "ProtocolAndroidDev.h"
#include "ProtocolRemConsole.h"
#include "Connectionhandler.h"

template<class D>
class NetConnectionHandler : public InternalMsgReceiver<D>, public InternalMsgSender<D>
{
    typedef InternalMsgReceiver<D> WORKER;
    InternalMsgRouter<D> * const Internalmsg_router;
    const char * start_clientserv;
    const char * sock_is_invalid;
    const char * invalid_dev_type;
    const char * cannot_get_dev_type;
    int sock;

public:
    typename InternalMsgReceiver<D>::HANDLE_RES HandleMsg(D data);
    NetConnectionHandler(InternalMsgRouter<D> * const Internalmsg_router_);
    ~NetConnectionHandler(){}
};

template<class D>
NetConnectionHandler<D>::NetConnectionHandler(InternalMsgRouter<D> * const Internalmsg_router_) :
                                                    InternalMsgReceiver<D>(INTNLMSG::RECV_NETCONNHANDLER),
                                                    InternalMsgSender<D>(Internalmsg_router_),
                                                    Internalmsg_router(Internalmsg_router_),
                                                    start_clientserv("[NetConnectionHandler] starting Device"),
                                                    sock_is_invalid("[NetConnectionHandler] socket is invalid"),
                                                    invalid_dev_type("[NetConnectionHandler] invalid Device type"),
                                                    cannot_get_dev_type("[NetConnectionHandler] cannot get Device type")
{
    Internalmsg_router->register_receiver(this);
}

template<class D>
typename InternalMsgReceiver<D>::HANDLE_RES NetConnectionHandler<D>::HandleMsg(D data)
{
    typename InternalMsgReceiver<D>::HANDLE_RES res = InternalMsgReceiver<D>::HANDLE_FAILED;

    if (data.getreceiver() == INTNLMSG::RECV_NETCONNHANDLER)
    {
        sock = data.getval();

        if (sock != -1)
        {
            Connectionhandler ch(sock);
            char dev_type[3];

            if (ch.read_nbytes(dev_type, 3))
            {
                switch (dev_type[0])
                {
                    case 0x31:
                    {
                        std::shared_ptr<Connectionhandler> conn(new Connectionhandler(sock));
                        std::shared_ptr<Protocol<char>> prot(new ProtocolAndroidDev(conn));
                        DeviceAndroid<D> dev(Internalmsg_router, prot);

                        this->send_internal_msg(INTNLMSG::RECV_DISPLAY, INTNLMSG::SHOW_MESSAGE,
                                               std::move(std::string(start_clientserv)));
                        this->send_internal_msg(INTNLMSG::RECV_DEVICE_MANAGER, INTNLMSG::DEVICE_ADDED,
                                               std::move(std::string(start_clientserv)));
                        //std::thread thr = std::thread(std::reference_wrapper<DeviceAndroid<D>>(dev));
                        std::thread thr = std::thread(dev);
                        thr.detach();
                        res = InternalMsgReceiver<D>::HANDLE_OK;
                    }
                    break;
                    case 0x32:
                    {
                        std::shared_ptr<Connectionhandler> conn(new Connectionhandler(sock));
                        std::shared_ptr<Protocol<char>> prot(new ProtocolRemConsole(conn));
                        DeviceRemConsole<D> dev(Internalmsg_router, prot);

                        this->send_internal_msg(INTNLMSG::RECV_DISPLAY, INTNLMSG::SHOW_MESSAGE,
                                               std::move(std::string(start_clientserv)));
                        this->send_internal_msg(INTNLMSG::RECV_DEVICE_MANAGER, INTNLMSG::DEVICE_ADDED,
                                               std::move(std::string(start_clientserv)));
                        //std::thread thr = std::thread(std::reference_wrapper<DeviceRemConsole<D>>(dev));
                        std::thread thr = std::thread(dev);
                        thr.detach();
                        res = InternalMsgReceiver<D>::HANDLE_OK;
                    }
                    break;
                    default:
                        this->send_internal_msg(INTNLMSG::RECV_DISPLAY, INTNLMSG::SHOW_MESSAGE,
                                               std::move(std::string(invalid_dev_type)));
                    break;
                }
            }
            else
            {
                this->send_internal_msg(INTNLMSG::RECV_DISPLAY, INTNLMSG::SHOW_MESSAGE,
                                       std::move(std::string(cannot_get_dev_type)));
            }
        }
        else
        {
            this->send_internal_msg(INTNLMSG::RECV_DISPLAY, INTNLMSG::SHOW_MESSAGE, std::move(std::string(sock_is_invalid)));
        }
    }
    else if (data.getreceiver() == INTNLMSG::RECV_BROADCAST)
    {
        int command = data.getval();

        switch (command)
        {
            case INTNLMSG::SHUTDOWN_ALL:
            {
                Internalmsg_router->deregister_receiver(this);
                this->stopthread();
            }
            break;

            case INTNLMSG::GET_NUM_OF_DEVS://num_of_devs_demanded
            {
            }
            break;
        }
    }
    return res;
}

#endif // Netconnectionhandler_H
