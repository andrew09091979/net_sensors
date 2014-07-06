#ifndef NETCONNECTIONHANDLER_H
#define NETCONNECTIONHANDLER_H

#include <mutex>
#include <queue>
#include <condition_variable>
#include <thread>
#include "internlmsgreceiver.h"
#include "clientservice.h"
#include "internlmsgsender.h"
#include "internlmsg.h"

template<class D>
class netconnectionhandler : public internlmsgreceiver<D>, public internlmsgsender<D>
{
    typedef internlmsgreceiver<D> WORKER;

    int sock;
//    WORKER * const wrk;

public:
    typename internlmsgreceiver<D>::HANDLE_RES HandleMsg(D data);
    netconnectionhandler(WORKER * const wrk_) :  internlmsgreceiver<D>(INTNLMSG::RECV_NETCONNHANDLER),
                                                    internlmsgsender<D>(wrk_){}
    ~netconnectionhandler(){}
};

template<class D>
typename internlmsgreceiver<D>::HANDLE_RES netconnectionhandler<D>::HandleMsg(D data)
{
    sock = data.getval();

    if (sock != -1)
    {
        clientservice<D> cs = clientservice<D>(sock, this->workers.front());
        std::thread thr(cs);
//        *wrk << D(0, std::string("[netconnectionhandler] starting clientservice"));
        thr.detach();
    }
    else
    {
//        *wrk << D(0, std::string("[netconnectionhandler] sock received = -1"));
    }

    return internlmsgreceiver<D>::HANDLE_FAILED;
}

#endif // NETCONNECTIONHANDLER_H
