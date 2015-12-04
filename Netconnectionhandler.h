#ifndef Netconnectionhandler_H
#define Netconnectionhandler_H

#include <mutex>
#include <queue>
#include <condition_variable>
#include <thread>
#include "Internalmsgreceiver.h"
#include "Internalmsgsender.h"
#include "Internalmsgrouter.h"
#include "Internalmsg.h"
#include "Deviceandroid.h"
#include "Deviceremconsole.h"
#include "Protocolandroiddev.h"
#include "Protocolremconsole.h"
#include "Connectionhandler.h"

template<class D>
class Netconnectionhandler : public Internalmsgreceiver<D>, public Internalmsgsender<D>
{
    typedef Internalmsgreceiver<D> WORKER;
    Internalmsgrouter<D> * const Internalmsg_router;
    const char * start_clientserv;
    const char * sock_is_invalid;
    const char * invalid_dev_type;
    const char * cannot_get_dev_type;
    int sock;

public:
    typename Internalmsgreceiver<D>::HANDLE_RES HandleMsg(D data);
    Netconnectionhandler(Internalmsgrouter<D> * const Internalmsg_router_);
    ~Netconnectionhandler(){}
};

template<class D>
Netconnectionhandler<D>::Netconnectionhandler(Internalmsgrouter<D> * const Internalmsg_router_) :
                                                    Internalmsgreceiver<D>(INTNLMSG::RECV_NETCONNHANDLER),
                                                    Internalmsgsender<D>(Internalmsg_router_),
                                                    Internalmsg_router(Internalmsg_router_),
                                                    start_clientserv("[Netconnectionhandler] starting Device"),
                                                    sock_is_invalid("[Netconnectionhandler] socket is invalid"),
                                                    invalid_dev_type("[Netconnectionhandler] invalid Device type"),
                                                    cannot_get_dev_type("[Netconnectionhandler] cannot get Device type")
{
    Internalmsg_router->register_receiver(this);
}

template<class D>
typename Internalmsgreceiver<D>::HANDLE_RES Netconnectionhandler<D>::HandleMsg(D data)
{
    typename Internalmsgreceiver<D>::HANDLE_RES res = Internalmsgreceiver<D>::HANDLE_FAILED;

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
                        std::shared_ptr<Protocol<char>> prot(new Protocolandroiddev(conn));
                        Deviceandroid<D> dev(Internalmsg_router, prot);

                        this->send_internal_msg(INTNLMSG::RECV_DISPLAY, INTNLMSG::SHOW_MESSAGE,
                                               std::move(std::string(start_clientserv)));
                        this->send_internal_msg(INTNLMSG::RECV_Device_MANAGER, INTNLMSG::Device_ADDED,
                                               std::move(std::string(start_clientserv)));
                        //std::thread thr = std::thread(std::reference_wrapper<Deviceandroid<D>>(dev));
                        std::thread thr = std::thread(dev);
                        thr.detach();
                        res = Internalmsgreceiver<D>::HANDLE_OK;
                    }
                    break;
                    case 0x32:
                    {
                        std::shared_ptr<Connectionhandler> conn(new Connectionhandler(sock));
                        std::shared_ptr<Protocol<char>> prot(new Protocolremconsole(conn));
                        Deviceremconsole<D> dev(Internalmsg_router, prot);

                        this->send_internal_msg(INTNLMSG::RECV_DISPLAY, INTNLMSG::SHOW_MESSAGE,
                                               std::move(std::string(start_clientserv)));
                        this->send_internal_msg(INTNLMSG::RECV_Device_MANAGER, INTNLMSG::Device_ADDED,
                                               std::move(std::string(start_clientserv)));
                        //std::thread thr = std::thread(std::reference_wrapper<Deviceremconsole<D>>(dev));
                        std::thread thr = std::thread(dev);
                        thr.detach();
                        res = Internalmsgreceiver<D>::HANDLE_OK;
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
