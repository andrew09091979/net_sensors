#ifndef WORKERDISPLAY_H
#define WORKERDISPLAY_H

#include <condition_variable>
#include <mutex>
#include <queue>
#include <iostream>
//#include "modulemanager.h"
#include "InternalMsgReceiver.h"
#include "InternalMsg.h"
#include "InternalMsgRouter.h"

template <class D>
class WorkerDisplay : public InternalMsgReceiver<D>
{
    InternalMsgRouter<D> * const Internalmsg_router;
public:
    WorkerDisplay(InternalMsgRouter<D> * const internalmsg_router_);
    typename InternalMsgReceiver<D>::HANDLE_RES HandleMsg(D data);
    virtual ~WorkerDisplay();
};

template<class D>
WorkerDisplay<D>::WorkerDisplay(InternalMsgRouter<D> * const internalmsg_router_) :
                                                                    InternalMsgReceiver<D>(INTERNALMESSAGE::RECV_DISPLAY),
                                                                    Internalmsg_router(internalmsg_router_)
{
    Internalmsg_router->register_receiver(this);
}

template<class D>
typename InternalMsgReceiver<D>::HANDLE_RES WorkerDisplay<D>::HandleMsg(D data)
{
    typename InternalMsgReceiver<D>::HANDLE_RES res = InternalMsgReceiver<D>::HANDLE_OK;
    int command = data.getval();

    switch (command)
    {
        case INTERNALMESSAGE::SHOW_MESSAGE:
        {
            std::cout << data.getmsg() << std::endl;
        }
        break;

        case INTERNALMESSAGE::SHUTDOWN_ALL:
        {
            Internalmsg_router->deregister_receiver(this);
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
