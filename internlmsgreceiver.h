#ifndef WORKER_H
#define WORKER_H

#include <memory>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <iostream>
#include "internlmsg.h"

template <class D>
class internlmsgreceiver
{
protected:
    enum HANDLE_RES
    {
        HANDLE_OK,
        HANDLE_FAILED
    };

    const INTNLMSG::RECEIVER iam;
    std::shared_ptr<std::mutex> mtx_p;
    std::shared_ptr<std::queue<D>> message_queue_p;
    std::shared_ptr<std::condition_variable> data_cond_p;
    bool stop;

public:
    internlmsgreceiver(INTNLMSG::RECEIVER iam_) : iam(iam_), mtx_p(new std::mutex),
                                                  message_queue_p(new std::queue<D>),
                                                  data_cond_p(new std::condition_variable),
                                                  stop(false){}

    virtual ~internlmsgreceiver();
    void EnqueMsg(D data);
    internlmsgreceiver<D> &operator <<(const D &data);
    void MainLoop();
    virtual void operator ()();
    virtual HANDLE_RES HandleMsg(D data) = 0;
    INTNLMSG::RECEIVER get_type() const;
};

template <class D>
internlmsgreceiver<D>::~internlmsgreceiver()
{
}

template<class D>
internlmsgreceiver<D> &internlmsgreceiver<D>::operator <<(const D &data)
{
    std::lock_guard<std::mutex> lk(*mtx_p);
    message_queue_p->push(std::move(data));
    data_cond_p->notify_one();

    return *this;
}

template<class D>
void internlmsgreceiver<D>::EnqueMsg(D data)
{
    std::lock_guard<std::mutex> lk(*mtx_p);
    message_queue_p->push(std::move(data));
    data_cond_p->notify_one();
}

template<class D>
void internlmsgreceiver<D>::MainLoop()
{
    while(!stop)
    {
        std::unique_lock<std::mutex> lk(*mtx_p);
        data_cond_p->wait(lk, [&]{return !message_queue_p->empty();});
        D data=message_queue_p->front();
        message_queue_p->pop();
        lk.unlock();

        if (data.getrecv == iam)
            HandleMsg(data);
    }
}

template<class D>
void internlmsgreceiver<D>::operator ()()
{
    while(!stop)
    {
        std::unique_lock<std::mutex> lk(*mtx_p);
        data_cond_p->wait(lk, [&]{return !message_queue_p->empty();});
        D data(std::move(message_queue_p->front()));
        message_queue_p->pop();
        lk.unlock();

        if (data.getreceiver() == iam)
            HandleMsg(std::move(data));
    }
}

template<class D>
INTNLMSG::RECEIVER internlmsgreceiver<D>::get_type() const
{
    return iam;
}


#endif // WORKER_H
