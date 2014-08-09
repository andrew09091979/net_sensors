#ifndef NETCONNECTIONHANDLER_H
#define NETCONNECTIONHANDLER_H

#include <mutex>
#include <queue>
#include <condition_variable>
#include <thread>
#include "modulemanager.h"
#include "internlmsgreceiver.h"
#include "internlmsgsender.h"
#include "internlmsg.h"
#include "deviceandroid.h"
#include "protocolandroiddev.h"
#include "connectionhandler.h"

template<class D>
class netconnectionhandler : public internlmsgreceiver<D>, public internlmsgsender<D>
{
    typedef internlmsgreceiver<D> WORKER;
    const modulemanager<D> * const mod_mgr;
    const char * start_clientserv;
    const char * sock_is_invalid;
    int sock;

public:
    typename internlmsgreceiver<D>::HANDLE_RES HandleMsg(D data);
    netconnectionhandler(const modulemanager<D> * const mod_mgr_);
    ~netconnectionhandler(){}
};

template<class D>
netconnectionhandler<D>::netconnectionhandler(const modulemanager<D> * const mod_mgr_) :
                                                    internlmsgreceiver<D>(INTNLMSG::RECV_NETCONNHANDLER),
                                                    mod_mgr(mod_mgr_),
                                                    start_clientserv("[netconnectionhandler] starting device"),
                                                    sock_is_invalid("[netconnectionhandler] socket is invalid")
{
    std::vector<INTNLMSG::RECEIVER> receivers_to_get;
    receivers_to_get.push_back(INTNLMSG::RECEIVER::RECV_DISPLAY);
    receivers_to_get.push_back(INTNLMSG::RECEIVER::RECV_DEVICE_MANAGER);
    mod_mgr->get_receivers(receivers_to_get, this->workers);
}

template<class D>
typename internlmsgreceiver<D>::HANDLE_RES netconnectionhandler<D>::HandleMsg(D data)
{
    sock = data.getval();

    if (sock != -1)
    {
        std::shared_ptr<connectionhandler> conn(new connectionhandler(sock));
        std::shared_ptr<protocol<char>> prot(new protocolandroiddev(conn));
        deviceandroid<D> dev(mod_mgr, prot);

        this->send_internl_msg(INTNLMSG::RECV_DISPLAY, 0, std::move(std::string(start_clientserv)));
        this->send_internl_msg(INTNLMSG::RECV_DEVICE_MANAGER, 1, std::move(std::string(start_clientserv)));

        std::thread thr(dev);
        thr.detach();
    }
    else
    {
        this->send_internl_msg(INTNLMSG::RECV_DISPLAY, 0, std::move(std::string(sock_is_invalid)));
    }

    return internlmsgreceiver<D>::HANDLE_FAILED;
}

#endif // NETCONNECTIONHANDLER_H
