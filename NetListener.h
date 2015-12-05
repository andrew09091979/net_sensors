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
#include "InternalMsgReceiver.h"
#include "InternalMsg.h"
#include "InternalMsgRouter.h"
#include "InternalMsgSender.h"

template<class D>
class NetListener : public InternalMsgSender<D>
{
    typedef InternalMsgReceiver<D> WORKER;
    typedef typename InternalMsgReceiver<D>::HANDLE_RES INTMSGRES;

    class Internalmsgreceivr : public InternalMsgReceiver<D>
    {
        NetListener<D> *const dev;

    public:
        Internalmsgreceivr(NetListener<D> *const dev_, INTERNALMESSAGE::RECEIVER iam_) : InternalMsgReceiver<D>(iam_),
                                                                                 dev(dev_)
        {
        }

        typename InternalMsgReceiver<D>::HANDLE_RES HandleMsg(D data)
        {
            typename InternalMsgReceiver<D>::HANDLE_RES res;
            res = dev->HandleInternalMsg(std::move(data));
            return res;
        }
    };

    const INTERNALMESSAGE::RECEIVER iam;
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
    NetListener(InternalMsgRouter<D> * const Internalmsg_router_);
    void MainLoop();
    INTMSGRES HandleInternalMsg(D data);
    void operator()();
};

template<class D>
NetListener<D>::NetListener(InternalMsgRouter<D> * const Internalmsg_router_) :
                                                    InternalMsgSender<D>(Internalmsg_router_),
                                                    iam(INTERNALMESSAGE::RECV_Netlistener),
                                                    incoming_conn("[NetListener] incoming connection"),
                                                    listen_started("[NetListener] listening mode started"),
                                                    listen_error("[NetListener] listen error"),
                                                    bind_error("[NetListener] bind error"),
                                                    sock_creation_error("[NetListener] socket creation error"),
                                                    accept_error("[NetListener] accept error"),
                                                    Netlistener_stopped("[NetListener] stopped"),
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
                this->send_internal_msg(INTERNALMESSAGE::RECV_DISPLAY, INTERNALMESSAGE::SHOW_MESSAGE,
                                       std::move(std::string(listen_started)));
            }
            else
            {
                this->send_internal_msg(INTERNALMESSAGE::RECV_DISPLAY, INTERNALMESSAGE::SHOW_MESSAGE,
                                       std::move(std::string(listen_error)));
            }
        }
        else
        {
            this->send_internal_msg(INTERNALMESSAGE::RECV_DISPLAY, INTERNALMESSAGE::SHOW_MESSAGE,
                                   std::move(std::string(bind_error)));
        }
    }
    else
    {
        this->send_internal_msg(INTERNALMESSAGE::RECV_DISPLAY, INTERNALMESSAGE::SHOW_MESSAGE,
                               std::move(std::string(sock_creation_error)));
    }
}

template<class D>
void NetListener<D>::MainLoop()
{

}

template<class D>
void NetListener<D>::operator()()
{
    Internalmsgreceivr InternalMsgReceiver(this, INTERNALMESSAGE::RECV_DEVICE);
    imr_ptr = &InternalMsgReceiver;
    std::reference_wrapper<Internalmsgreceivr> rv = std::reference_wrapper<Internalmsgreceivr>(InternalMsgReceiver);
    std::thread thrd = std::thread(rv);
    this->internalMsgRouter->register_receiver(imr_ptr);

    while (!shutdown_ordered)
    {
        struct sockaddr_in addr;
        size_t len = 0;
        memset(&addr, 0, sizeof(addr));

        sock = accept(sockToListen, (struct sockaddr*)&addr, &len);

        if (sock != -1)
        {
            this->send_internal_msg(INTERNALMESSAGE::RECV_DISPLAY, INTERNALMESSAGE::SHOW_MESSAGE,
                                   std::move(std::string(incoming_conn)));
            this->send_internal_msg(INTERNALMESSAGE::RECV_NETCONNHANDLER, sock, std::move(std::string(incoming_conn)));
        }
        else
        {
            this->send_internal_msg(INTERNALMESSAGE::RECV_DISPLAY, INTERNALMESSAGE::SHOW_MESSAGE, std::move(std::string(accept_error)));

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
    this->internalMsgRouter->deregister_receiver(imr_ptr);
    imr_ptr->stopthread();

    if (thrd.joinable())
        thrd.join();

    this->send_internal_msg(INTERNALMESSAGE::RECV_DISPLAY, INTERNALMESSAGE::SHOW_MESSAGE,
                           std::move(std::string(Netlistener_stopped)));
}

template <class D>
typename NetListener<D>::INTMSGRES NetListener<D>::HandleInternalMsg(D data)
{
    typename NetListener<D>::INTMSGRES res = InternalMsgReceiver<D>::HANDLE_FAILED;
    int command = data.getval();

    switch (command)
    {
        case INTERNALMESSAGE::SHUTDOWN_ALL:
        {
           shutdown_ordered = true;
           shutdown(sockToListen, SHUT_RDWR);//terminate accept
        }
        break;
    }
    return res;
}

#endif // Netlistener_H
