#ifndef NETLISTENER_H
#define NETLISTENER_H

#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <vector>
#include "modulemanager.h"
#include "internlmsgreceiver.h"
#include "internlmsg.h"
#include "internlmsgsender.h"

template<class D>
class netlistener : public internlmsgsender<D>
{
    typedef internlmsgreceiver<D> WORKER;
    const INTNLMSG::RECEIVER iam;
    const modulemanager<D> * const mod_mgr;
    const char * incoming_conn;
    const char * listen_started;
    const char * listen_error;
    const char * bind_error;
    const char * sock_creation_error;
    const char * accept_error;
    int sockToListen, sock;
    bool stop;

public:
    netlistener(const modulemanager<D> * const mod_mgr_);
    void MainLoop();

    void operator()();
};

template<class D>
netlistener<D>::netlistener(const modulemanager<D> * const mod_mgr_) :
                                                    iam(INTNLMSG::RECV_NETLISTENER),
                                                    mod_mgr(mod_mgr_),
                                                    incoming_conn("[netlistener] incoming connection"),
                                                    listen_started("[netlistener] listening mode started"),
                                                    listen_error("[netlistener] listen error"),
                                                    bind_error("[netlistener] bind error"),
                                                    sock_creation_error("[netlistener] socket creation error"),
                                                    accept_error("[netlistener] accept error"),
                                                    stop(false)
{
    std::vector<INTNLMSG::RECEIVER> receivers_to_get;
    receivers_to_get.push_back(INTNLMSG::RECEIVER::RECV_DISPLAY);
    receivers_to_get.push_back(INTNLMSG::RECEIVER::RECV_NETCONNHANDLER);
    mod_mgr->get_receivers(receivers_to_get, this->workers);

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
                this->send_internl_msg(INTNLMSG::RECV_DISPLAY, 0, std::move(std::string(listen_started)));
            }
            else
            {
                this->send_internl_msg(INTNLMSG::RECV_DISPLAY, 0, std::move(std::string(listen_error)));
            }
        }
        else
        {
            this->send_internl_msg(INTNLMSG::RECV_DISPLAY, 0, std::move(std::string(bind_error)));
        }
    }
    else
    {
        this->send_internl_msg(INTNLMSG::RECV_DISPLAY, 0, std::move(std::string(sock_creation_error)));
    }
}

template<class D>
void netlistener<D>::MainLoop()
{

}

template<class D>
void netlistener<D>::operator()()
{
    while (!stop)
    {
        struct sockaddr_in addr;
        size_t len = 0;
        memset(&addr, 0, sizeof(addr));

        sock = accept(sockToListen, (struct sockaddr*)&addr, &len);

        if (sock != -1)
        {
            this->send_internl_msg(INTNLMSG::RECV_DISPLAY, 0, std::move(std::string(incoming_conn)));
            this->send_internl_msg(INTNLMSG::RECV_NETCONNHANDLER, sock, std::move(std::string(incoming_conn)));
        }
        else
        {
            this->send_internl_msg(INTNLMSG::RECV_DISPLAY, 0, std::move(std::string(accept_error)));

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
}

#endif // NETLISTENER_H
