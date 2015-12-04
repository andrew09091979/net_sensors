#ifndef WORKERDISPLAY_H
#define WORKERDISPLAY_H

#include <condition_variable>
#include <mutex>
#include <queue>
#include <iostream>
//#include "modulemanager.h"
#include "Internalmsgreceiver.h"
#include "Internalmsg.h"
#include "Internalmsgrouter.h"

template <class D>
class WorkerDisplay : public Internalmsgreceiver<D>
{
    Internalmsgrouter<D> * const Internalmsg_router;
public:
    WorkerDisplay(Internalmsgrouter<D> * const Internalmsg_router_);
    typename Internalmsgreceiver<D>::HANDLE_RES HandleMsg(D data);
    virtual ~WorkerDisplay();
};

template<class D>
WorkerDisplay<D>::WorkerDisplay(Internalmsgrouter<D> * const Internalmsg_router_) :
                                                                    Internalmsgreceiver<D>(INTNLMSG::RECV_DISPLAY),
                                                                    Internalmsg_router(Internalmsg_router_)
{
    Internalmsg_router->register_receiver(this);
}

template<class D>
typename Internalmsgreceiver<D>::HANDLE_RES WorkerDisplay<D>::HandleMsg(D data)
{
    typename Internalmsgreceiver<D>::HANDLE_RES res = Internalmsgreceiver<D>::HANDLE_OK;
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
