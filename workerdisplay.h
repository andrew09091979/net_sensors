#ifndef WORKERDISPLAY_H
#define WORKERDISPLAY_H

#include <condition_variable>
#include <mutex>
#include <queue>
#include <iostream>
#include "worker.h"
#include "internlmsg.h"

template <class D>
class WorkerDisplay : public worker<D>
{
public:
    WorkerDisplay();
    typename worker<D>::HANDLE_RES HandleMsg(D data);
    virtual ~WorkerDisplay();
};

template<class D>
WorkerDisplay<D>::WorkerDisplay() : worker<D>(INTNLMSG::RECV_DISPLAY)
{
}

template<class D>
typename worker<D>::HANDLE_RES WorkerDisplay<D>::HandleMsg(D data)
{
    typename worker<D>::HANDLE_RES res = worker<D>::HANDLE_OK;

    std::cout << data.getmsg() << std::endl;

    return res;
}

template<class D>
WorkerDisplay<D>::~WorkerDisplay()
{
}

#endif // WORKERDISPLAY_H
