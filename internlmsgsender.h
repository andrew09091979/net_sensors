#ifndef INTERNLMSGSENDER_H
#define INTERNLMSGSENDER_H

#include "internlmsgreceiver.h"
#include "internlmsgrouter.h"

template<class D>
class internlmsgsender
{    
protected:
    typedef internlmsgreceiver<D> WORKER;
//    std::vector<WORKER *> workers;
    internlmsgrouter<D> * const internlmsg_router;

public:
    internlmsgsender(internlmsgrouter<D> * const internlmsg_router_);

    void send_internl_msg(D msg_);
    void send_internl_msg(INTNLMSG::RECEIVER recv_, unsigned int val_, std::string msg_);
//    void add_worker(WORKER * const wrk_);
//    void remove_worker(WORKER * const wrk_);
};

template<class D>
internlmsgsender<D>::internlmsgsender(internlmsgrouter<D> * const internlmsg_router_) : internlmsg_router(internlmsg_router_)
{
}

template<class D>
void internlmsgsender<D>::send_internl_msg(D msg_)
{
//    typename std::vector<WORKER *>::iterator it = workers.begin();

//    for(;it != workers.end(); ++it)
//        *(*it) << msg_;
    (*internlmsg_router) << msg_;
}

template<class D>
void internlmsgsender<D>::send_internl_msg(INTNLMSG::RECEIVER recv_, unsigned int val_, std::string msg_)
{
    D msg(recv_, val_, std::move(msg_));

//    typename std::vector<WORKER *>::iterator it = workers.begin();

//    for(;it != workers.end(); ++it)
//        *(*it) << msg;

    (*internlmsg_router) << msg;
}

//template<class D>
//void internlmsgsender<D>::add_worker(WORKER * const wrk_)
//{
//    workers.push_back(wrk_);
//}

//template<class D>
//void internlmsgsender<D>::remove_worker(WORKER * const wrk_)
//{

//}

#endif // INTERNLMSGSENDER_H
