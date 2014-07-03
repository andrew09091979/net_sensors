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
    WORKER &wrk;
    int sockToListen, sock;
    bool stop;

public:
    netlistener(worker<D>& wrk_);
    void MainLoop();
};

template<class D>
netlistener<D>::netlistener(WORKER &wrk_) : wrk(wrk_)
{
    struct sockaddr_in addr;
    const int on = 1;

    addr.sin_family = AF_INET;
    addr.sin_port = htons(7502);
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
                std::cout << "sockToListen successfully turned to listening mode" << std::endl;
            }
            else
            {
                std::cout << "listen error" << std::endl;
            }
        }
        else
        {
            std::cout << "bind error" << std::endl;
        }
    }
    else
    {
        std::cout << "socket creation error" << std::endl;
    }
}

template<class D>
void netlistener<D>::MainLoop()
{
    while (!stop)
    {
        struct sockaddr_in addr;
        size_t len = 0;
        memset(&addr, 0, sizeof(addr));
        std::cout << "calling accept" << std::endl;
        sock = accept(sockToListen, (struct sockaddr*)&addr, &len);

        if (sock != -1)
        {
            wrk << D(sock);
        }
        else
        {
            std::cout << "accept returned -1" << std::endl;

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
