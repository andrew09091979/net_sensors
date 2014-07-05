#ifndef NETLISTENER_H
#define NETLISTENER_H

#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <vector>
#include "worker.h"
#include "internlmsg.h"
#include "internlmsgsender.h"

template<class D>
class netlistener : public internlmsgsender<D>
{
    typedef worker<D> WORKER;
    const INTNLMSG::RECEIVER iam;
    const char * incoming_conn;
    const char * listen_started;
    const char * listen_error;
    const char * bind_error;
    const char * sock_creation_error;
    const char * accept_error;
//    WORKER * const wrk;
//    std::vector<WORKER *> workers;
    int sockToListen, sock;
    bool stop;

public:
    netlistener(WORKER * const wrk_);
    void MainLoop();

    void operator()();
};

template<class D>
netlistener<D>::netlistener(WORKER * const wrk_) : internlmsgsender<D>(wrk_),
                                                    iam(INTNLMSG::RECV_NETLISTENER),
                                                    incoming_conn("[netlistener] incoming connection"),
                                                    listen_started("[netlistener] listening mode started"),
                                                    listen_error("[netlistener] listen error"),
                                                    bind_error("[netlistener] bind error"),
                                                    sock_creation_error("[netlistener] socket creation error"),
                                                    accept_error("[netlistener] accept error"),
                                                    stop(false)
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
                D msg = D(INTNLMSG::RECV_DISPLAY, strlen(listen_started),
                          0, std::move(std::string(listen_started)));
                send_internl_msg(std::move(msg));
            }
            else
            {
                D msg = D(INTNLMSG::RECV_DISPLAY, strlen(listen_started),
                          0, std::move(std::string(listen_error)));
                send_internl_msg(std::move(msg));
            }
        }
        else
        {
            D msg = D(INTNLMSG::RECV_DISPLAY, strlen(listen_started),
                      0, std::move(std::string(bind_error)));
            send_internl_msg(std::move(msg));
        }
    }
    else
    {
        D msg = D(INTNLMSG::RECV_DISPLAY, strlen(listen_started),
                  0, std::move(std::string(sock_creation_error)));
        send_internl_msg(std::move(msg));
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
            D msg = D(INTNLMSG::RECV_DISPLAY, strlen(incoming_conn), 0, std::string(incoming_conn));
            send_internl_msg(std::move(msg));

            D msg1 = D(INTNLMSG::RECV_NETCONNHANDLER, strlen(incoming_conn),
                       sock, std::string(incoming_conn));
            send_internl_msg(std::move(msg1));
        }
        else
        {
            D msg = D(INTNLMSG::RECV_DISPLAY, 10, 0, accept_error);
            send_internl_msg(std::move(msg));

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
