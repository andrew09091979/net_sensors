#ifndef Internalmsgrouter_H
#define Internalmsgrouter_H

#include <algorithm>
#include "InternalMsgReceiver.h"

template <class D>
class InternalMsgRouter : public InternalMsgReceiver<D>
{
    std::vector<InternalMsgReceiver<D> *> receivers;
    mutable std::mutex mtx;
    bool bStop;
public:
    InternalMsgRouter();
    void register_receiver(InternalMsgReceiver<D> * recv_);
    void deregister_receiver(InternalMsgReceiver<D> * recv_);
    void get_receivers(const std::vector<INTERNALMESSAGE::RECEIVER> &iam_,
                      std::vector<InternalMsgReceiver<D> *>& tofill) const;
    typename InternalMsgReceiver<D>::HANDLE_RES HandleMsg(D data);
};

template<class D>
InternalMsgRouter<D>::InternalMsgRouter() : InternalMsgReceiver<D>(INTERNALMESSAGE::RECV_INTERNL_MSG_ROUTER),
                                          bStop(false)
{
}

template<class D>
typename InternalMsgReceiver<D>::HANDLE_RES InternalMsgRouter<D>::HandleMsg(D data)
{
    typename InternalMsgReceiver<D>::HANDLE_RES res = InternalMsgReceiver<D>::HANDLE_FAILED;

    std::lock_guard<std::mutex> lk(mtx);

    typename std::vector<InternalMsgReceiver<D> *>::const_iterator it;

    for (it = receivers.begin(); it != receivers.end(); ++it)
    {
        if ((*it)->get_type() == data.getreceiver() || (data.getreceiver() == INTERNALMESSAGE::RECV_BROADCAST))
        {
            *(*it) << data;
            res = InternalMsgReceiver<D>::HANDLE_OK;
        }
    }
    if (data.getreceiver() == INTERNALMESSAGE::RECV_BROADCAST)
    {
        int command = data.getval();

        switch (command)
        {
            case INTERNALMESSAGE::SHUTDOWN_ALL:
            {
                bStop = true;
            }
            break;
        }
    }
    return res;
}

template<class D>
void InternalMsgRouter<D>::register_receiver(InternalMsgReceiver<D> *recv_)
{
    std::lock_guard<std::mutex> lk(mtx);
    receivers.push_back(recv_);

    typename std::vector<InternalMsgReceiver<D> *>::const_iterator it;

    for (it = receivers.begin(); it != receivers.end(); ++it)
    {
        if ((*it)->get_type() == INTERNALMESSAGE::RECV_DISPLAY)
        {
            D msg(INTERNALMESSAGE::RECV_DISPLAY, INTERNALMESSAGE::SHOW_MESSAGE,
                  std::string("[InternalMsgRouter] - Device of type ") + recv_->getname()
                                + std::string(" registered"));
            *(*it) << msg;
        }
    }
}

template<class D>
void InternalMsgRouter<D>::get_receivers(const std::vector<INTERNALMESSAGE::RECEIVER> &iam_,
                                    std::vector<InternalMsgReceiver<D> *> &tofill) const
{
    typename std::vector<InternalMsgReceiver<D> *>::const_iterator it;
    typename std::vector<INTERNALMESSAGE::RECEIVER>::const_iterator it1;

    std::lock_guard<std::mutex> lk(mtx);

    for (it1 = iam_.begin(); it1 != iam_.end(); ++it1)
    {
        for (it = receivers.begin(); it != receivers.end(); ++it)
        {
            if ((*it)->get_type() == *it1)
            {
                tofill.push_back(*it);
            }
        }
    }
}

template <class D>
void InternalMsgRouter<D>::deregister_receiver(InternalMsgReceiver<D> *recv_)
{
//    std::string module_ptr = std::string("[InternalMsgRouter] = ") + std::to_string(int(recv_));
    bool found_and_erased = false;

    std::lock_guard<std::mutex> lk(mtx);
    typename std::vector<InternalMsgReceiver<D> *>::iterator it1;

    for (it1 = receivers.begin(); it1 != receivers.end(); ++it1)
    {
        if ((*it1)->get_type() == INTERNALMESSAGE::RECV_DISPLAY)
        {
            D msg(INTERNALMESSAGE::RECV_DISPLAY, INTERNALMESSAGE::SHOW_MESSAGE,
                  std::string("[InternalMsgRouter] - removing Device of type ") + recv_->getname());
            *(*it1) << msg;
        }
    }
//    InternalMsgReceiver<D> *elem;
    typename std::vector<InternalMsgReceiver<D> *>::iterator it;

    it = find(receivers.begin(), receivers.end(), recv_);

    if (it != receivers.end())
    {
        receivers.erase(it);
        found_and_erased = true;
    }
    if (found_and_erased)
    {
        typename std::vector<InternalMsgReceiver<D> *>::iterator it;
        for (it = receivers.begin(); it != receivers.end(); ++it)
        {
            if ((*it)->get_type() == INTERNALMESSAGE::RECV_DISPLAY)
            {
                D msg(INTERNALMESSAGE::RECV_DISPLAY, INTERNALMESSAGE::SHOW_MESSAGE,
                      std::string("[InternalMsgRouter] - Device of type ") + recv_->getname()
                                    + std::string(" removed"));
                *(*it) << msg;
            }
        }
    }

    if (bStop)
    {
        if ((receivers.size() == 0))
        {
            this->stopthread();
        }
    }
}
#endif // Internalmsgrouter_H
