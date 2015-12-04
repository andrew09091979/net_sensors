#ifndef Internalmsgrouter_H
#define Internalmsgrouter_H

#include <algorithm>
#include "Internalmsgreceiver.h"

template <class D>
class Internalmsgrouter : public Internalmsgreceiver<D>
{
    std::vector<Internalmsgreceiver<D> *> receivers;
    mutable std::mutex mtx;
    bool bStop;
public:
    Internalmsgrouter();
    void register_receiver(Internalmsgreceiver<D> * recv_);
    void deregister_receiver(Internalmsgreceiver<D> * recv_);
    void get_receivers(const std::vector<INTNLMSG::RECEIVER> &iam_,
                      std::vector<Internalmsgreceiver<D> *>& tofill) const;
    typename Internalmsgreceiver<D>::HANDLE_RES HandleMsg(D data);
};

template<class D>
Internalmsgrouter<D>::Internalmsgrouter() : Internalmsgreceiver<D>(INTNLMSG::RECV_INTERNL_MSG_ROUTER),
                                          bStop(false)
{
}

template<class D>
typename Internalmsgreceiver<D>::HANDLE_RES Internalmsgrouter<D>::HandleMsg(D data)
{
    typename Internalmsgreceiver<D>::HANDLE_RES res = Internalmsgreceiver<D>::HANDLE_FAILED;

    std::lock_guard<std::mutex> lk(mtx);

    typename std::vector<Internalmsgreceiver<D> *>::const_iterator it;

    for (it = receivers.begin(); it != receivers.end(); ++it)
    {
        if ((*it)->get_type() == data.getreceiver() || (data.getreceiver() == INTNLMSG::RECV_BROADCAST))
        {
            *(*it) << data;
            res = Internalmsgreceiver<D>::HANDLE_OK;
        }
    }
    if (data.getreceiver() == INTNLMSG::RECV_BROADCAST)
    {
        int command = data.getval();

        switch (command)
        {
            case INTNLMSG::SHUTDOWN_ALL:
            {
                bStop = true;
            }
            break;
        }
    }
    return res;
}

template<class D>
void Internalmsgrouter<D>::register_receiver(Internalmsgreceiver<D> *recv_)
{
    std::lock_guard<std::mutex> lk(mtx);
    receivers.push_back(recv_);

    typename std::vector<Internalmsgreceiver<D> *>::const_iterator it;

    for (it = receivers.begin(); it != receivers.end(); ++it)
    {
        if ((*it)->get_type() == INTNLMSG::RECV_DISPLAY)
        {
            D msg(INTNLMSG::RECV_DISPLAY, INTNLMSG::SHOW_MESSAGE,
                  std::string("[Internalmsgrouter] - Device of type ") + recv_->getname()
                                + std::string(" registered"));
            *(*it) << msg;
        }
    }
}

template<class D>
void Internalmsgrouter<D>::get_receivers(const std::vector<INTNLMSG::RECEIVER> &iam_,
                                    std::vector<Internalmsgreceiver<D> *> &tofill) const
{
    typename std::vector<Internalmsgreceiver<D> *>::const_iterator it;
    typename std::vector<INTNLMSG::RECEIVER>::const_iterator it1;

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
void Internalmsgrouter<D>::deregister_receiver(Internalmsgreceiver<D> *recv_)
{
//    std::string module_ptr = std::string("[Internalmsgrouter] = ") + std::to_string(int(recv_));
    bool found_and_erased = false;

    std::lock_guard<std::mutex> lk(mtx);
    typename std::vector<Internalmsgreceiver<D> *>::iterator it1;

    for (it1 = receivers.begin(); it1 != receivers.end(); ++it1)
    {
        if ((*it1)->get_type() == INTNLMSG::RECV_DISPLAY)
        {
            D msg(INTNLMSG::RECV_DISPLAY, INTNLMSG::SHOW_MESSAGE,
                  std::string("[Internalmsgrouter] - removing Device of type ") + recv_->getname());
            *(*it1) << msg;
        }
    }
//    Internalmsgreceiver<D> *elem;
    typename std::vector<Internalmsgreceiver<D> *>::iterator it;

    it = find(receivers.begin(), receivers.end(), recv_);

    if (it != receivers.end())
    {
        receivers.erase(it);
        found_and_erased = true;
    }
    if (found_and_erased)
    {
        typename std::vector<Internalmsgreceiver<D> *>::iterator it;
        for (it = receivers.begin(); it != receivers.end(); ++it)
        {
            if ((*it)->get_type() == INTNLMSG::RECV_DISPLAY)
            {
                D msg(INTNLMSG::RECV_DISPLAY, INTNLMSG::SHOW_MESSAGE,
                      std::string("[Internalmsgrouter] - Device of type ") + recv_->getname()
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
