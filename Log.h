#ifndef LOG_H
#define LOG_H

#include <condition_variable>
#include <mutex>
#include <queue>
#include <iostream>
#include <fstream>
//#include "modulemanager.h"
#include "InternalMsgReceiver.h"
#include "InternalMsg.h"
#include "InternalMsgRouter.h"

template <class D>
class Log : public InternalMsgReceiver<D>
{
    InternalMsgRouter<D> * const internalmsg_router;
    std::ofstream log_stream;

public:
    Log(InternalMsgRouter<D> * const internalmsg_router_, std::string log_file_name_);
    typename InternalMsgReceiver<D>::HANDLE_RES HandleMsg(D data);
    virtual ~Log();
};

template<class D>
Log<D>::Log(InternalMsgRouter<D> * const internalmsg_router_, std::string log_file_name_) :
                                            InternalMsgReceiver<D>(INTERNALMESSAGE::RECV_FILE),
                                            internalmsg_router(internalmsg_router_),
                                            log_stream(log_file_name_)
{
    internalmsg_router->register_receiver(this);
}

template<class D>
typename InternalMsgReceiver<D>::HANDLE_RES Log<D>::HandleMsg(D data)
{
    typename InternalMsgReceiver<D>::HANDLE_RES res = InternalMsgReceiver<D>::HANDLE_OK;
    int command = data.getval();

    switch (command)
    {
        case INTERNALMESSAGE::STORE_MESSAGE:
        {
            log_stream << data.getmsg() << std::endl;
        }
        break;

        case INTERNALMESSAGE::SHUTDOWN_ALL:
        {
            internalmsg_router->deregister_receiver(this);
            log_stream << "shutdown all";
            this->stopthread();
        }
        break;
    }

    return res;
}

template<class D>
Log<D>::~Log()
{
    log_stream.close();
}

#endif // LOG_H
