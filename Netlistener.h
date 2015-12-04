#ifndef Netlistener_H
#define Netlistener_H

#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <vector>
#include <unistd.h>
//#include "modulemanager.h"
#include "Internalmsgreceiver.h"
#include "Internalmsg.h"
#include "Internalmsgrouter.h"
#include "Internalmsgsender.h"

template<class D>
class Netlistener : public Internalmsgsender<D>
{
    typedef Internalmsgreceiver<D> WORKER;
    typedef typename Internalmsgreceiver<D>::HANDLE_RES INTMSGRES;

    class Internalmsgreceivr : public Internalmsgreceiver<D>
    {
        Netlistener<D> *const dev;

    public:
        Internalmsgreceivr(Netlistener<D> *const dev_, INTNLMSG::RECEIVER iam_) : Internalmsgreceiver<D>(iam_),
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

    const INTNLMSG::RECEIVER iam;
    const char * incoming_conn;
    const char * listen_started;
    const char * listen_error;
    const char * bind_error;
    const char * sock_creation_error;
    const char * accept_error;
    const char * Netlistener_stopped;
    Internalmsgreceivr *imr_ptr;
    int sockToListen, sock;
    bool shutdown_ordered;

public:
    Netlistener(Internalmsgrouter<D> * const Internalmsg_router_);
    void MainLoop();
    INTMSGRES HandleInternalMsg(D data);
    void operator()();
};

template<class D>
Netlistener<D>::Netlistener(Internalmsgrouter<D> * const Internalmsg_router_) :
                                                    Internalmsgsender<D>(Internalmsg_router_),
                                                    iam(INTNLMSG::RECV_Netlistener),
                                                    incoming_conn("[Netlistener] incoming connection"),
                                                    listen_started("[Netlistener] listening mode started"),
                                                    listen_error("[Netlistener] listen error"),
                                                    bind_error("[Netlistener] bind error"),
                                                    sock_creation_error("[Netlistener] socket creation error"),
                                                    accept_error("[Netlistener] accept error"),
                                                    Netlistener_stopped("[Netlistener] stopped"),
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
                this->send_internal_msg(INTNLMSG::RECV_DISPLAY, INTNLMSG::SHOW_MESSAGE,
                                       std::move(std::string(listen_started)));
            }
            else
            {
                this->send_internal_msg(INTNLMSG::RECV_DISPLAY, INTNLMSG::SHOW_MESSAGE,
                                       std::move(std::string(listen_error)));
            }
        }
        else
        {
            this->send_internal_msg(INTNLMSG::RECV_DISPLAY, INTNLMSG::SHOW_MESSAGE,
                                   std::move(std::string(bind_error)));
        }
    }
    else
    {
        this->send_internal_msg(INTNLMSG::RECV_DISPLAY, INTNLMSG::SHOW_MESSAGE,
                               std::move(std::string(sock_creation_error)));
    }
}

template<class D>
void Netlistener<D>::MainLoop()
{

}

template<class D>
void Netlistener<D>::operator()()
{
    Internalmsgreceivr internalmsgreceiver(this, INTNLMSG::RECV_Device);
    imr_ptr = &internalmsgreceiver;
    std::reference_wrapper<Internalmsgreceivr> rv = std::reference_wrapper<Internalmsgreceivr>(internalmsgreceiver);
    std::thread thrd = std::thread(rv);
    this->Internalmsg_router->register_receiver(imr_ptr);

    while (!shutdown_ordered)
    {
        struct sockaddr_in addr;
        size_t len = 0;
        memset(&addr, 0, sizeof(addr));

        sock = accept(sockToListen, (struct sockaddr*)&addr, &len);

        if (sock != -1)
        {
            this->send_internal_msg(INTNLMSG::RECV_DISPLAY, INTNLMSG::SHOW_MESSAGE,
                                   std::move(std::string(incoming_conn)));
            this->send_internal_msg(INTNLMSG::RECV_NETCONNHANDLER, sock, std::move(std::string(incoming_conn)));
        }
        else
        {
            this->send_internal_msg(INTNLMSG::RECV_DISPLAY, INTNLMSG::SHOW_MESSAGE, std::move(std::string(accept_error)));

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
    this->Internalmsg_router->deregister_receiver(imr_ptr);
    imr_ptr->stopthread();

    if (thrd.joinable())
        thrd.join();

    this->send_internal_msg(INTNLMSG::RECV_DISPLAY, INTNLMSG::SHOW_MESSAGE,
                           std::move(std::string(Netlistener_stopped)));
}

template <class D>
typename Netlistener<D>::INTMSGRES Netlistener<D>::HandleInternalMsg(D data)
{
    typename Netlistener<D>::INTMSGRES res = Internalmsgreceiver<D>::HANDLE_FAILED;
    int command = data.getval();

    switch (command)
    {
        case INTNLMSG::SHUTDOWN_ALL:
        {
           shutdown_ordered = true;
           shutdown(sockToListen, SHUT_RDWR);//terminate accept
        }
        break;
    }
    return res;
}

#endif // Netlistener_H
