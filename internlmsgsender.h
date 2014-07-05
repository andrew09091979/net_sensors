#ifndef INTERNLMSGSENDER_H
#define INTERNLMSGSENDER_H
#include "worker.h"

template<class D>
class internlmsgsender
{    
protected:
    typedef worker<D> WORKER;
    std::vector<WORKER *> workers;

public:
    internlmsgsender(WORKER *wrk_);

    void send_internl_msg(D msg);
    void add_worker(WORKER * const wrk_);
    void remove_worker(WORKER * const wrk_);
};

template<class D>
internlmsgsender<D>::internlmsgsender(WORKER *wrk_)
{
    workers.push_back(wrk_);
}

template<class D>
void internlmsgsender<D>::send_internl_msg(D msg)
{
    typename std::vector<WORKER *>::iterator it = workers.begin();

    for(;it != workers.end(); ++it)
        *(*it) << msg;
}

template<class D>
void internlmsgsender<D>::add_worker(WORKER * const wrk_)
{
    workers.push_back(wrk_);
}

template<class D>
void internlmsgsender<D>::remove_worker(WORKER * const wrk_)
{

}

#endif // INTERNLMSGSENDER_H
