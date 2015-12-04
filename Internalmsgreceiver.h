#ifndef WORKER_H
#define WORKER_H

#include <memory>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <iostream>
#include "Internalmsg.h"
//#include "Device.h"

template <class D>
class Internalmsgreceiver
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

    Internalmsgreceiver(INTNLMSG::RECEIVER iam_) : iam(iam_), mtx_p(new std::mutex),
                                                  message_queue_p(new std::queue<D>),
                                                  data_cond_p(new std::condition_variable),
                                                  stop(false)
    {
        myname = INTNLMSG::receivers_names[iam];
    }

    virtual ~Internalmsgreceiver();
    void EnqueMsg(D data);
    Internalmsgreceiver<D> &operator <<(const D &data);
    void MainLoop();
    virtual void operator ()();
    virtual HANDLE_RES HandleMsg(D data) = 0;
    INTNLMSG::RECEIVER get_type() const;
    std::string getname() const;
    void stopthread();
};

template <class D>
Internalmsgreceiver<D>::~Internalmsgreceiver()
{
}

template<class D>
Internalmsgreceiver<D> &Internalmsgreceiver<D>::operator <<(const D &data)
{
    std::lock_guard<std::mutex> lk(*mtx_p);
    message_queue_p->push(std::move(data));
    data_cond_p->notify_one();

    return *this;
}

template<class D>
void Internalmsgreceiver<D>::EnqueMsg(D data)
{
    std::lock_guard<std::mutex> lk(*mtx_p);
    message_queue_p->push(std::move(data));
    data_cond_p->notify_one();
}

template<class D>
void Internalmsgreceiver<D>::MainLoop()
{
//    while(!stop)
//    {
//        std::unique_lock<std::mutex> lk(*mtx_p);
//        data_cond_p->wait(lk, [&]{return !message_queue_p->empty();});
//        D data=message_queue_p->front();
//        message_queue_p->pop();
//        lk.unlock();
//        HandleMsg(data);
//    }
//    std::cout << "[Internalmsgreceiver] " << myname << " stopped" << std::endl;
}

template<class D>
void Internalmsgreceiver<D>::operator ()()
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
            std::cout << "closing main thread of " + myname << std::endl;
            return;
        }
    }
}

template<class D>
INTNLMSG::RECEIVER Internalmsgreceiver<D>::get_type() const
{
    return iam;
}

template<class D>
void Internalmsgreceiver<D>::stopthread()
{
    std::cout << "in stopthread for " + myname << std::endl;
    stop = true;

    if (myname == "INTERNL_MSG_ROUTER")
        stop = true;
    D dummy_msg = D(INTNLMSG::RECV_BROADCAST, -1, std::move(std::string("exit")));
    EnqueMsg(dummy_msg);
}

template <class D>
std::string Internalmsgreceiver<D>::getname() const
{
    return myname;
}

//template <class X>
//class XYZ
//{

//};

//template <class D>
//class internalmsgreceiver_dev : public Internalmsgreceiver<D>
//{
//    Device<D> *const dev;

//public:
//    internalmsgreceiver_dev(Device<D> *const dev_, INTNLMSG::RECEIVER iam_) : Internalmsgreceiver<D>(iam_),
//                                                                        dev(dev_)

//    {
//    }

//    typename Internalmsgreceiver<D>::HANDLE_RES HandleMsg(D data)
//    {
//        typename Internalmsgreceiver<D>::HANDLE_RES res;
//        res = dev->HandleInternalMsg(std::move(data));
//        return res;
//    }
//};

#endif // WORKER_H
