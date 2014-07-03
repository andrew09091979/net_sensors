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
    std::queue<D> data_q;
    std::mutex mtx;
    std::condition_variable cond;
    bool stop;
public:
    WorkerDisplay();
    void EnqueMsg(const D &data);
    virtual worker<D> &operator <<(const D &data);
    void MainLoop();
    int HandleMsg(const D &data);
    virtual ~WorkerDisplay();
};

template<class D>
WorkerDisplay<D>::WorkerDisplay() : stop(false)
{
}

template<class D>
void WorkerDisplay<D>::EnqueMsg(const D &data)
{
    std::lock_guard<std::mutex> grd(mtx);
    data_q.push(data);
    cond.notify_one();
}

template<class D>
worker<D>& WorkerDisplay<D>::operator <<(const D &data)
{
    this->EnqueMsg(data);
    return *this;
}

template<class D>
void WorkerDisplay<D>::MainLoop()
{
    while (!stop)
    {
        std::unique_lock<std::mutex> lk(mtx);
        cond.wait(lk, [this]{return !data_q.empty();});
        D data = data_q.front();
        data_q.pop();
        lk.unlock();
        HandleMsg(data);
    }
    std::cout << "MainLoop finished" << std::endl;
}

template<class D>
int WorkerDisplay<D>::HandleMsg(const D &data)
{
    int res = 0;

    std::cout << data << std::endl;

    if (data == 33)
        stop = true;

    return res;
}

template<class D>
WorkerDisplay<D>::~WorkerDisplay()
{
}

#endif // WORKERDISPLAY_H
