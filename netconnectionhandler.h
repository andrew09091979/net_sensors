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
#include "clientservice.h"

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
                                                    start_clientserv("[netconnectionhandler] starting clientservice"),
                                                    sock_is_invalid("[netconnectionhandler] socket is invalid")
{
    std::vector<INTNLMSG::RECEIVER> receivers_to_get;
    receivers_to_get.push_back(INTNLMSG::RECEIVER::RECV_DISPLAY);
    mod_mgr->get_receivers(receivers_to_get, this->workers);
}

template<class D>
typename internlmsgreceiver<D>::HANDLE_RES netconnectionhandler<D>::HandleMsg(D data)
{
    sock = data.getval();

    if (sock != -1)
    {
        clientservice<D> cs = clientservice<D>(sock, mod_mgr);
        D msg = D(INTNLMSG::RECV_DISPLAY, 0, std::string(start_clientserv));
        send_internl_msg(std::move(msg));

        std::thread thr(cs);
        thr.detach();
    }
    else
    {
        D msg = D(INTNLMSG::RECV_DISPLAY, 0, std::string(sock_is_invalid));
        send_internl_msg(std::move(msg));
    }

    return internlmsgreceiver<D>::HANDLE_FAILED;
}

#endif // NETCONNECTIONHANDLER_H
