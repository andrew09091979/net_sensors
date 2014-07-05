#ifndef WORKER_H
#define WORKER_H

#include <mutex>
#include <queue>
#include <condition_variable>
#include <iostream>
#include "internlmsg.h"

template <class D>
class worker
{
protected:
    enum HANDLE_RES
    {
        HANDLE_OK,
        HANDLE_FAILED
    };

    const INTNLMSG::RECEIVER iam;
    std::mutex mtx;
    std::queue<D> message_queue;
    std::condition_variable data_cond;
    bool stop;

public:
    worker(INTNLMSG::RECEIVER iam_) : iam(iam_), stop(false){}
    void EnqueMsg(D data);
    worker<D> &operator <<(const D &data);
    void MainLoop();
    virtual void operator ()();
    virtual HANDLE_RES HandleMsg(D data) = 0;
    virtual ~worker(){}
};

template<class D>
worker<D> &worker<D>::operator <<(const D &data)
{
    std::lock_guard<std::mutex> lk(mtx);
    message_queue.push(std::move(data));
    data_cond.notify_one();

    return *this;
}

template<class D>
void worker<D>::EnqueMsg(D data)
{
    std::lock_guard<std::mutex> lk(mtx);
    message_queue.push(std::move(data));
    data_cond.notify_one();
}

template<class D>
void worker<D>::MainLoop()
{
    while(!stop)
    {
        std::unique_lock<std::mutex> lk(mtx);
        data_cond.wait(lk, [&]{return !message_queue.empty();});
        D data=message_queue.front();
        message_queue.pop();
        lk.unlock();

        if (data.getrecv == iam)
            HandleMsg(data);
    }
}

template<class D>
void worker<D>::operator ()()
{
    while(!stop)
    {
        std::unique_lock<std::mutex> lk(mtx);
        data_cond.wait(lk, [&]{return !message_queue.empty();});
        D data(std::move(message_queue.front()));
        message_queue.pop();
        lk.unlock();

        if (data.getreceiver() == iam)
            HandleMsg(std::move(data));
    }
}

#endif // WORKER_H
