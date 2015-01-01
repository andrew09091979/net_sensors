#ifndef WORKER_H
#define WORKER_H

#include <memory>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <iostream>
#include "internlmsg.h"
//#include "device.h"

template <class D>
class internlmsgreceiver
{
protected:
    const INTNLMSG::RECEIVER iam;
    std::shared_ptr<std::mutex> mtx_p;
    std::shared_ptr<std::queue<D>> message_queue_p;
    std::shared_ptr<std::condition_variable> data_cond_p;
    bool stop;
    unsigned long address;
    std::string myname;
public:

    enum HANDLE_RES
    {
        HANDLE_OK,
        HANDLE_FAILED
    };

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
    std::string getname() const;
    void stopthread();
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
        HandleMsg(data);
    }
    std::cout << "[internlmsgreceiver] - stopped" << std::endl;
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

        if (!stop)
        {
            HandleMsg(std::move(data));
        }
        else
        {
            break;
        }
    }
    std::cout << "[internlmsgreceiver] - stopped" << std::endl;
}

template<class D>
INTNLMSG::RECEIVER internlmsgreceiver<D>::get_type() const
{
    return iam;
}

template<class D>
void internlmsgreceiver<D>::stopthread()
{
    stop = true;
    D dummy_msg = D(INTNLMSG::RECV_BROADCAST, -1, std::move(std::string("exit")));
    EnqueMsg(dummy_msg);
}

template <class D>
std::string internlmsgreceiver<D>::getname() const
{
    return myname;
}

//template <class X>
//class XYZ
//{

//};

//template <class D>
//class internalmsgreceiver_dev : public internlmsgreceiver<D>
//{
//    device<D> *const dev;

//public:
//    internalmsgreceiver_dev(device<D> *const dev_, INTNLMSG::RECEIVER iam_) : internlmsgreceiver<D>(iam_),
//                                                                        dev(dev_)

//    {
//    }

//    typename internlmsgreceiver<D>::HANDLE_RES HandleMsg(D data)
//    {
//        typename internlmsgreceiver<D>::HANDLE_RES res;
//        res = dev->HandleInternalMsg(std::move(data));
//        return res;
//    }
//};

#endif // WORKER_H
