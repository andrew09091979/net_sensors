#ifndef WORKERDISPLAY_H
#define WORKERDISPLAY_H

#include <condition_variable>
#include <mutex>
#include <queue>
#include <iostream>
#include "internlmsgreceiver.h"
#include "internlmsg.h"

template <class D>
class WorkerDisplay : public internlmsgreceiver<D>
{
public:
    WorkerDisplay();
    typename internlmsgreceiver<D>::HANDLE_RES HandleMsg(D data);
    virtual ~WorkerDisplay();
};

template<class D>
WorkerDisplay<D>::WorkerDisplay() : internlmsgreceiver<D>(INTNLMSG::RECV_DISPLAY)
{
}

template<class D>
typename internlmsgreceiver<D>::HANDLE_RES WorkerDisplay<D>::HandleMsg(D data)
{
    typename internlmsgreceiver<D>::HANDLE_RES res = internlmsgreceiver<D>::HANDLE_OK;

    std::cout << data.getmsg() << std::endl;

    return res;
}

template<class D>
WorkerDisplay<D>::~WorkerDisplay()
{
}

#endif // WORKERDISPLAY_H
