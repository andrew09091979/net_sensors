#ifndef NETLISTENER_H
#define NETLISTENER_H

#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <vector>
//#include "modulemanager.h"
#include "internlmsgreceiver.h"
#include "internlmsg.h"
#include "internlmsgrouter.h"
#include "internlmsgsender.h"

template<class D>
class netlistener : public internlmsgsender<D>
{
    typedef internlmsgreceiver<D> WORKER;
    typedef typename internlmsgreceiver<D>::HANDLE_RES INTMSGRES;

    class internlmsgreceivr : public internlmsgreceiver<D>
    {
        netlistener<D> *const dev;

    public:
        internlmsgreceivr(netlistener<D> *const dev_, INTNLMSG::RECEIVER iam_) : internlmsgreceiver<D>(iam_),
                                                                                 dev(dev_)
        {
        }

        typename internlmsgreceiver<D>::HANDLE_RES HandleMsg(D data)
        {
            typename internlmsgreceiver<D>::HANDLE_RES res;
            res = dev->HandleInternalMsg(std::move(data));
            return res;
        }
    };

    const INTNLMSG::RECEIVER iam;
    const char * incoming_conn;
    const char * listen_started;
    const char * listen_error;
    const char * bind_error;
    const char * sock_creation_error;
    const char * accept_error;
    const char * netlistener_stopped;
    internlmsgreceivr *imr_ptr;
    int sockToListen, sock;
    bool shutdown_ordered;

public:
    netlistener(internlmsgrouter<D> * const internlmsg_router_);
    void MainLoop();
    INTMSGRES HandleInternalMsg(D data);
    void operator()();
};

template<class D>
netlistener<D>::netlistener(internlmsgrouter<D> * const internlmsg_router_) :
                                                    internlmsgsender<D>(internlmsg_router_),
                                                    iam(INTNLMSG::RECV_NETLISTENER),
                                                    incoming_conn("[netlistener] incoming connection"),
                                                    listen_started("[netlistener] listening mode started"),
                                                    listen_error("[netlistener] listen error"),
                                                    bind_error("[netlistener] bind error"),
                                                    sock_creation_error("[netlistener] socket creation error"),
                                                    accept_error("[netlistener] accept error"),
                                                    netlistener_stopped("[netlistener] stopped"),
                                                    shutdown_ordered(false)
{
    struct sockaddr_in addr;
    const int on = 1;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(7503);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    // create socket
    sockToListen = socket(AF_INET, SOCK_STREAM, 0);

    if (sockToListen != -1)
    {
        setsockopt(sockToListen, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

        // bind address
        if (bind(sockToListen, (struct sockaddr*) &addr, sizeof(addr)) != -1)
        {
            // change socket's state to LISTEN
            if (listen(sockToListen, 5) != -1)
            {
                this->send_internl_msg(INTNLMSG::RECV_DISPLAY, INTNLMSG::SHOW_MESSAGE,
                                       std::move(std::string(listen_started)));
            }
            else
            {
                this->send_internl_msg(INTNLMSG::RECV_DISPLAY, INTNLMSG::SHOW_MESSAGE,
                                       std::move(std::string(listen_error)));
            }
        }
        else
        {
            this->send_internl_msg(INTNLMSG::RECV_DISPLAY, INTNLMSG::SHOW_MESSAGE,
                                   std::move(std::string(bind_error)));
        }
    }
    else
    {
        this->send_internl_msg(INTNLMSG::RECV_DISPLAY, INTNLMSG::SHOW_MESSAGE,
                               std::move(std::string(sock_creation_error)));
    }
}

template<class D>
void netlistener<D>::MainLoop()
{

}

template<class D>
void netlistener<D>::operator()()
{
    internlmsgreceivr internalmsgreceiver(this, INTNLMSG::RECV_DEVICE);
    imr_ptr = &internalmsgreceiver;
    std::reference_wrapper<internlmsgreceivr> rv = std::reference_wrapper<internlmsgreceivr>(internalmsgreceiver);
    std::thread thrd = std::thread(rv);
    this->internlmsg_router->register_receiver(imr_ptr);

    while (!shutdown_ordered)
    {
        struct sockaddr_in addr;
        size_t len = 0;
        memset(&addr, 0, sizeof(addr));

        sock = accept(sockToListen, (struct sockaddr*)&addr, &len);

        if (sock != -1)
        {
            this->send_internl_msg(INTNLMSG::RECV_DISPLAY, INTNLMSG::SHOW_MESSAGE,
                                   std::move(std::string(incoming_conn)));
            this->send_internl_msg(INTNLMSG::RECV_NETCONNHANDLER, sock, std::move(std::string(incoming_conn)));
        }
        else
        {
            this->send_internl_msg(INTNLMSG::RECV_DISPLAY, INTNLMSG::SHOW_MESSAGE, std::move(std::string(accept_error)));

            if (errno == EAGAIN || errno == EWOULDBLOCK)
            {
//                usleep(ACCEPT_INTERVAL_USEC);
            }
            else
            {
                break;
            }
        }
    }
    this->internlmsg_router->deregister_receiver(imr_ptr);
    imr_ptr->stopthread();

    if (thrd.joinable())
        thrd.join();

    this->send_internl_msg(INTNLMSG::RECV_DISPLAY, INTNLMSG::SHOW_MESSAGE,
                           std::move(std::string(netlistener_stopped)));
}

template <class D>
typename netlistener<D>::INTMSGRES netlistener<D>::HandleInternalMsg(D data)
{
    typename netlistener<D>::INTMSGRES res = internlmsgreceiver<D>::HANDLE_FAILED;
    int command = data.getval();

    switch (command)
    {
        case INTNLMSG::SHUTDOWN_ALL:
        {
           shutdown_ordered = true;
           close(sockToListen);
        }
        break;
    }
    return res;
}

#endif // NETLISTENER_H
