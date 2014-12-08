#ifndef NETCONNECTIONHANDLER_H
#define NETCONNECTIONHANDLER_H

#include <mutex>
#include <queue>
#include <condition_variable>
#include <thread>
#include "internlmsgreceiver.h"
#include "internlmsgsender.h"
#include "internlmsgrouter.h"
#include "internlmsg.h"
#include "deviceandroid.h"
#include "deviceremconsole.h"
#include "protocolandroiddev.h"
#include "protocolremconsole.h"
#include "connectionhandler.h"

template<class D>
class netconnectionhandler : public internlmsgreceiver<D>, public internlmsgsender<D>
{
    typedef internlmsgreceiver<D> WORKER;
    internlmsgrouter<D> * const internlmsg_router;
    const char * start_clientserv;
    const char * sock_is_invalid;
    const char * invalid_dev_type;
    const char * cannot_get_dev_type;
    int sock;

public:
    typename internlmsgreceiver<D>::HANDLE_RES HandleMsg(D data);
    netconnectionhandler(internlmsgrouter<D> * const internlmsg_router_);
    ~netconnectionhandler(){}
};

template<class D>
netconnectionhandler<D>::netconnectionhandler(internlmsgrouter<D> * const internlmsg_router_) :
                                                    internlmsgreceiver<D>(INTNLMSG::RECV_NETCONNHANDLER),
                                                    internlmsgsender<D>(internlmsg_router_),
                                                    internlmsg_router(internlmsg_router_),
                                                    start_clientserv("[netconnectionhandler] starting device"),
                                                    sock_is_invalid("[netconnectionhandler] socket is invalid"),
                                                    invalid_dev_type("[netconnectionhandler] invalid device type"),
                                                    cannot_get_dev_type("[netconnectionhandler] cannot get device type")
{
}

template<class D>
typename internlmsgreceiver<D>::HANDLE_RES netconnectionhandler<D>::HandleMsg(D data)
{
    typename internlmsgreceiver<D>::HANDLE_RES res = internlmsgreceiver<D>::HANDLE_FAILED;

    if (data.getreceiver() == INTNLMSG::RECV_NETCONNHANDLER)
    {
        sock = data.getval();

        if (sock != -1)
        {
            connectionhandler ch(sock);
            char dev_type[3];

            if (ch.read_nbytes(dev_type, 3))
            {
                switch (dev_type[0])
                {
                    case 0x31:
                    {
                        std::shared_ptr<connectionhandler> conn(new connectionhandler(sock));
                        std::shared_ptr<protocol<char>> prot(new protocolandroiddev(conn));
                        deviceandroid<D> dev(internlmsg_router, prot);

                        this->send_internl_msg(INTNLMSG::RECV_DISPLAY, INTNLMSG::SHOW_MESSAGE,
                                               std::move(std::string(start_clientserv)));
                        this->send_internl_msg(INTNLMSG::RECV_DEVICE_MANAGER, INTNLMSG::DEVICE_ADDED,
                                               std::move(std::string(start_clientserv)));
                        //std::thread thr = std::thread(std::reference_wrapper<deviceandroid<D>>(dev));
                        std::thread thr = std::thread(dev);
                        thr.detach();
                        res = internlmsgreceiver<D>::HANDLE_OK;
                    }
                    break;
                    case 0x32:
                    {
                        std::shared_ptr<connectionhandler> conn(new connectionhandler(sock));
                        std::shared_ptr<protocol<char>> prot(new protocolremconsole(conn));
                        deviceremconsole<D> dev(internlmsg_router, prot);

                        this->send_internl_msg(INTNLMSG::RECV_DISPLAY, INTNLMSG::SHOW_MESSAGE,
                                               std::move(std::string(start_clientserv)));
                        this->send_internl_msg(INTNLMSG::RECV_DEVICE_MANAGER, INTNLMSG::DEVICE_ADDED,
                                               std::move(std::string(start_clientserv)));
                        //std::thread thr = std::thread(std::reference_wrapper<deviceremconsole<D>>(dev));
                        std::thread thr = std::thread(dev);
                        thr.detach();
                        res = internlmsgreceiver<D>::HANDLE_OK;
                    }
                    break;
                    default:
                        this->send_internl_msg(INTNLMSG::RECV_DISPLAY, INTNLMSG::SHOW_MESSAGE,
                                               std::move(std::string(invalid_dev_type)));
                    break;
                }
            }
            else
            {
                this->send_internl_msg(INTNLMSG::RECV_DISPLAY, INTNLMSG::SHOW_MESSAGE,
                                       std::move(std::string(cannot_get_dev_type)));
            }
        }
        else
        {
            this->send_internl_msg(INTNLMSG::RECV_DISPLAY, INTNLMSG::SHOW_MESSAGE, std::move(std::string(sock_is_invalid)));
        }
    }
    return res;
}

#endif // NETCONNECTIONHANDLER_H
