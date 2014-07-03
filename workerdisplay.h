#ifndef WORKERDISPLAY_H
#define WORKERDISPLAY_H

#include <condition_variable>
#include <mutex>
#include <queue>
#include <iostream>
#include "worker.h"

template <class D>
class WorkerDisplay : public worker<D>
{
public:
    WorkerDisplay();
    typename worker<D>::HANDLE_RES HandleMsg(const D &data);
    virtual ~WorkerDisplay();
};

template<class D>
WorkerDisplay<D>::WorkerDisplay()
{
}

template<class D>
typename worker<D>::HANDLE_RES WorkerDisplay<D>::HandleMsg(const D &data)
{
    typename worker<D>::HANDLE_RES res = worker<D>::HANDLE_OK;

    std::cout << std::string(data) << std::endl;

    if (data == -1)
        this->stop = true;

    return res;
}

template<class D>
WorkerDisplay<D>::~WorkerDisplay()
{
}

#endif // WORKERDISPLAY_H
