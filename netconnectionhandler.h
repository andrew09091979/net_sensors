#ifndef NETCONNECTIONHANDLER_H
#define NETCONNECTIONHANDLER_H

#include <mutex>
#include <queue>
#include <condition_variable>
#include <thread>
#include "worker.h"
#include "clientservice.h"

template<class D>
class netconnectionhandler : public worker<D>
{
    typedef worker<D> WORKER;

    int sock;
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

    if (sock != -1)
    {
        clientservice<D> cs = clientservice<D>(sock, wrk);
        std::thread thr(cs);
        *wrk << D(0, std::string("[netconnectionhandler] starting clientservice"));
        thr.detach();
    }
    else
    {
        *wrk << D(0, std::string("[netconnectionhandler] sock received = -1"));
    }

    return worker<D>::HANDLE_FAILED;
}

#endif // NETCONNECTIONHANDLER_H
