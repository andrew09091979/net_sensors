#ifndef WORKER_H
#define WORKER_H

#include <memory>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <iostream>
#include "InternalMsg.h"
//#include "Device.h"

template <class D>
class InternalMsgReceiver
{
protected:
    const INTERNALMESSAGE::RECEIVER iam;
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

    InternalMsgReceiver(INTERNALMESSAGE::RECEIVER iam_) : iam(iam_), mtx_p(new std::mutex),
                                                  message_queue_p(new std::queue<D>),
                                                  data_cond_p(new std::condition_variable),
                                                  stop(false)
    {
        myname = INTERNALMESSAGE::receivers_names[iam];
    }

    virtual ~InternalMsgReceiver();
    void EnqueMsg(D data);
    InternalMsgReceiver<D> &operator <<(const D &data);
    void MainLoop();
    virtual void operator ()();
    virtual HANDLE_RES HandleMsg(D data) = 0;
    INTERNALMESSAGE::RECEIVER get_type() const;
    std::string getname() const;
    void stopthread();
};

template <class D>
InternalMsgReceiver<D>::~InternalMsgReceiver()
{
}

template<class D>
InternalMsgReceiver<D> &InternalMsgReceiver<D>::operator <<(const D &data)
{
    std::lock_guard<std::mutex> lk(*mtx_p);
    message_queue_p->push(std::move(data));
    data_cond_p->notify_one();

    return *this;
}

template<class D>
void InternalMsgReceiver<D>::EnqueMsg(D data)
{
    std::lock_guard<std::mutex> lk(*mtx_p);
    message_queue_p->push(std::move(data));
    data_cond_p->notify_one();
}

template<class D>
void InternalMsgReceiver<D>::MainLoop()
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
//    std::cout << "[InternalMsgReceiver] " << myname << " stopped" << std::endl;
}

template<class D>
void InternalMsgReceiver<D>::operator ()()
{
    HANDLE_RES res;
    while(!stop)
    {
        std::unique_lock<std::mutex> lk(*mtx_p);
        data_cond_p->wait(lk, [&]{return !message_queue_p->empty();});

        D data(std::move(message_queue_p->front()));
        message_queue_p->pop();
        lk.unlock();

        if (!stop)
        {
            res = HandleMsg(std::move(data));
        }
        else
        {
            std::cout << "closing main thread of " + myname << std::endl;
            return;
        }
    }
}

template<class D>
INTERNALMESSAGE::RECEIVER InternalMsgReceiver<D>::get_type() const
{
    return iam;
}

template<class D>
void InternalMsgReceiver<D>::stopthread()
{
    std::cout << "in stopthread for " + myname << std::endl;
    stop = true;

    if (myname == "INTERNL_MSG_ROUTER")
        stop = true;
    D dummy_msg = D(INTERNALMESSAGE::RECV_BROADCAST, -1, std::move(std::string("exit")));
    EnqueMsg(dummy_msg);
}

template <class D>
std::string InternalMsgReceiver<D>::getname() const
{
    return myname;
}

//template <class X>
//class XYZ
//{

//};

//template <class D>
//class internalmsgreceiver_dev : public InternalMsgReceiver<D>
//{
//    Device<D> *const dev;

//public:
//    internalmsgreceiver_dev(Device<D> *const dev_, INTERNALMESSAGE::RECEIVER iam_) : InternalMsgReceiver<D>(iam_),
//                                                                        dev(dev_)

//    {
//    }

//    typename InternalMsgReceiver<D>::HANDLE_RES HandleMsg(D data)
//    {
//        typename InternalMsgReceiver<D>::HANDLE_RES res;
//        res = dev->HandleInternalMsg(std::move(data));
//        return res;
//    }
//};

#endif // WORKER_H
