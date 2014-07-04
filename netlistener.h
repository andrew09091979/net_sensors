#ifndef NETLISTENER_H
#define NETLISTENER_H

#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <vector>
#include "worker.h"

template<class D>
class netlistener
{
    typedef worker<D> WORKER;
    WORKER * const wrk;
    std::vector<WORKER *> workers;
    int sockToListen, sock;
    bool stop;
    void notify_all(const D &msg);
public:
    netlistener(WORKER * const wrk_);
    void AddWorker(WORKER * const wrk_);
    void RemoveWorker(WORKER * const wrk_);
    void MainLoop();

    void operator()();
};

template<class D>
netlistener<D>::netlistener(WORKER * const wrk_) : wrk(wrk_), stop(false)
{
    workers.push_back(wrk);

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
                D msg = D(0, std::string("[netlistener] listening mode started"));
                notify_all(msg);
            }
            else
            {
                D msg = D(-1, std::string("[netlistener] listen error"));
                notify_all(msg);
            }
        }
        else
        {
            D msg = D(-1, std::string("[netlistener] bind error"));
            notify_all(msg);
        }
    }
    else
    {
        D msg = D(-1, std::string("[netlistener] socket creation error"));
        notify_all(msg);
    }
}

template<class D>
void netlistener<D>::AddWorker(WORKER * const wrk_)
{
    workers.push_back(wrk_);
}

template<class D>
void netlistener<D>::RemoveWorker(WORKER * const wrk_)
{
//    workers.
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
            D msg = D(sock, std::string("[netlistener] Incoming connection"));
            notify_all(msg);
        }
        else
        {
            D msg = D(-1, std::string("[netlistener] accept error"));
            notify_all(msg);

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

template<class D>
void netlistener<D>::notify_all(const D &msg)
{
    typename std::vector<WORKER *>::iterator it = workers.begin();

    for(;it != workers.end(); ++it)
        *(*it) << msg;
}

#endif // NETLISTENER_H
