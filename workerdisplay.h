#ifndef WORKERDISPLAY_H
#define WORKERDISPLAY_H

#include <condition_variable>
#include <mutex>
#include <queue>
#include <iostream>
//#include "modulemanager.h"
#include "internlmsgreceiver.h"
#include "internlmsg.h"
#include "internlmsgrouter.h"

template <class D>
class WorkerDisplay : public internlmsgreceiver<D>
{
    internlmsgrouter<D> * const internlmsg_router;
public:
    WorkerDisplay(internlmsgrouter<D> * const internlmsg_router_);
    typename internlmsgreceiver<D>::HANDLE_RES HandleMsg(D data);
    virtual ~WorkerDisplay();
};

template<class D>
WorkerDisplay<D>::WorkerDisplay(internlmsgrouter<D> * const internlmsg_router_) :
                                                                    internlmsgreceiver<D>(INTNLMSG::RECV_DISPLAY),
                                                                    internlmsg_router(internlmsg_router_)
{
}

template<class D>
typename internlmsgreceiver<D>::HANDLE_RES WorkerDisplay<D>::HandleMsg(D data)
{
    typename internlmsgreceiver<D>::HANDLE_RES res = internlmsgreceiver<D>::HANDLE_OK;
    int command = data.getval();

    switch (command)
    {
        case INTNLMSG::SHOW_MESSAGE:
        {
            std::cout << data.getmsg() << std::endl;
        }
        break;

        case INTNLMSG::SHUTDOWN_ALL:
        {
            internlmsg_router->deregister_receiver(this);
            this->stopthread();
        }
        break;
    }

    return res;
}

template<class D>
WorkerDisplay<D>::~WorkerDisplay()
{
}

#endif // WORKERDISPLAY_H
