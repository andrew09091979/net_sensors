#ifndef NETCONNECTIONHANDLER_H
#define NETCONNECTIONHANDLER_H
#include <sys/socket.h>
#include <mutex>
#include <queue>
#include <condition_variable>
#include "worker.h"

template<class D>
class netconnectionhandler : public worker<D>
{
    bool stop;
    int sock;
    struct sockaddr_in sa_local;
    std::mutex mtx;
    std::queue<D> message_queue;
    std::condition_variable data_cond;

public:
    void EnqueMsg(const D &data);
    worker<D> &operator << (const D &data);
    typename worker<D>::HANDLE_RES HandleMsg(const D &data);
    void MainLoop();

    netconnectionhandler(){}
    worker<D>& GetWorker();
};

template<class D>
worker<D> &netconnectionhandler<D>::operator <<(const D &data)
{

    std::lock_guard<std::mutex> lk(mtx);
    message_queue.push(data);
    data_cond.notify_one();

    return *this;
}

template<class D>
void netconnectionhandler<D>::EnqueMsg(const D &data)
{

    std::lock_guard<std::mutex> lk(mtx);
    message_queue.push(data);
    data_cond.notify_one();
}

template<class D>
typename worker<D>::HANDLE_RES netconnectionhandler<D>::HandleMsg(const D &data)
{
    std::cout << "netconnectionhandler received a message" << std::endl;
    return worker<D>::HANDLE_FAILED;
}

template<class D>
void netconnectionhandler<D>::MainLoop()
{
    while(!stop)
    {
        std::unique_lock<std::mutex> lk(mtx);
        data_cond.wait(lk, [&]{return !message_queue.empty();});
        D data=message_queue.front();
        message_queue.pop();
        lk.unlock();
        HandleMsg(data);
    }
}

#endif // NETCONNECTIONHANDLER_H
