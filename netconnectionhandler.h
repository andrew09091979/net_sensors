#ifndef NETCONNECTIONHANDLER_H
#define NETCONNECTIONHANDLER_H

#include <mutex>
#include <queue>
#include <condition_variable>
#include "worker.h"

template<class D>
class netconnectionhandler : public worker<D>
{
    typedef worker<D> WORKER;

    int sock;
    std::mutex mtx;
    std::queue<D> message_queue;
    std::condition_variable data_cond;
    WORKER * const wrk;

public:
    typename worker<D>::HANDLE_RES HandleMsg(const D &data);
    netconnectionhandler(WORKER * const wrk_) : wrk(wrk_){}
    ~netconnectionhandler(){}
};

template<class D>
typename worker<D>::HANDLE_RES netconnectionhandler<D>::HandleMsg(const D &data)
{
    sock = data;
    std::string hello("Hello\n");

    if (sock != -1)
    {
        *wrk << D(0, std::string("[netconnectionhandler] closing connection"));
        send(sock, hello.c_str() , hello.length(), 0);
        close(sock);
    }
    else
    {
        *wrk << D(0, std::string("[netconnectionhandler] sock received = -1"));
    }

    return worker<D>::HANDLE_FAILED;
}

#endif // NETCONNECTIONHANDLER_H
