#ifndef INTERNLMSGROUTER_H
#define INTERNLMSGROUTER_H

#include <algorithm>
#include "internlmsgreceiver.h"

template <class D>
class internlmsgrouter : public internlmsgreceiver<D>
{
    std::vector<internlmsgreceiver<D> *> receivers;
    mutable std::mutex mtx;

public:
    internlmsgrouter();
    void register_receiver(internlmsgreceiver<D> * recv_);
    void deregister_receiver(internlmsgreceiver<D> * recv_);
    void get_receivers(const std::vector<INTNLMSG::RECEIVER> &iam_,
                      std::vector<internlmsgreceiver<D> *>& tofill) const;
    typename internlmsgreceiver<D>::HANDLE_RES HandleMsg(D data);
};

template<class D>
internlmsgrouter<D>::internlmsgrouter() : internlmsgreceiver<D>(INTNLMSG::RECV_INTERNL_MSG_ROUTER)
{

}

template<class D>
typename internlmsgreceiver<D>::HANDLE_RES internlmsgrouter<D>::HandleMsg(D data)
{
    typename internlmsgreceiver<D>::HANDLE_RES res = internlmsgreceiver<D>::HANDLE_FAILED;

    std::lock_guard<std::mutex> lk(mtx);

    typename std::vector<internlmsgreceiver<D> *>::const_iterator it;

    for (it = receivers.begin(); it != receivers.end(); ++it)
    {
        if ((*it)->get_type() == data.getreceiver() || (data.getreceiver() == INTNLMSG::RECV_BROADCAST))
        {
            *(*it) << data;
            res = internlmsgreceiver<D>::HANDLE_OK;
        }
    }
    return res;
}

template<class D>
void internlmsgrouter<D>::register_receiver(internlmsgreceiver<D> *recv_)
{
    std::lock_guard<std::mutex> lk(mtx);
    receivers.push_back(recv_);

    typename std::vector<internlmsgreceiver<D> *>::const_iterator it;

    for (it = receivers.begin(); it != receivers.end(); ++it)
    {
        D msg(INTNLMSG::RECV_BROADCAST, 4, std::string("[internlmsgrouter] - device added"));
        *(*it) << msg;
    }
}

template<class D>
void internlmsgrouter<D>::get_receivers(const std::vector<INTNLMSG::RECEIVER> &iam_,
                                    std::vector<internlmsgreceiver<D> *> &tofill) const
{
    typename std::vector<internlmsgreceiver<D> *>::const_iterator it;
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
void internlmsgrouter<D>::deregister_receiver(internlmsgreceiver<D> *recv_)
{
    std::string module_ptr = std::string("[module to delete] = ") + std::to_string(int(recv_));
    bool found_and_erased = false;

    std::lock_guard<std::mutex> lk(mtx);

    internlmsgreceiver<D> *elem;
    typename std::vector<internlmsgreceiver<D> *>::iterator it;

    it = find(receivers.begin(), receivers.end(), recv_);

    if (it != receivers.end())
    {
        receivers.erase(it);
        found_and_erased = true;
    }
    if (found_and_erased)
    {
        typename std::vector<internlmsgreceiver<D> *>::iterator it;
        for (it = receivers.begin(); it != receivers.end(); ++it)
        {
            D msg(INTNLMSG::RECV_BROADCAST, 5, module_ptr);
            *(*it) << msg;
        }
    }
}
#endif // INTERNLMSGROUTER_H
