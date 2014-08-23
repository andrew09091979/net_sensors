#ifndef MODULEMANAGER_H
#define MODULEMANAGER_H

#include <algorithm>
#include "internlmsgreceiver.h"

template <class D>
class modulemanager
{
    std::vector<internlmsgreceiver<D> *> receivers;
    mutable std::mutex mtx;

public:
    modulemanager();
    void register_receiver(internlmsgreceiver<D> * recv_);
    void deregister_receiver(internlmsgreceiver<D> * recv_);
    void get_receivers(const std::vector<INTNLMSG::RECEIVER> &iam_,
                      std::vector<internlmsgreceiver<D> *>& tofill) const;
};

template<class D>
modulemanager<D>::modulemanager()
{

}

template<class D>
void modulemanager<D>::register_receiver(internlmsgreceiver<D> *recv_)
{
    std::lock_guard<std::mutex> lk(mtx);
    receivers.push_back(recv_);

    typename std::vector<internlmsgreceiver<D> *>::const_iterator it;

    for (it = receivers.begin(); it != receivers.end(); ++it)
    {
        D msg(INTNLMSG::RECV_DEVICE, 4, std::string("device added"));
        *(*it) << msg;
    }
}

template<class D>
void modulemanager<D>::get_receivers(const std::vector<INTNLMSG::RECEIVER> &iam_,
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
void modulemanager<D>::deregister_receiver(internlmsgreceiver<D> *recv_)
{
    std::lock_guard<std::mutex> lk(mtx);
    typename std::vector<INTNLMSG::RECEIVER>::const_iterator it;

    it = std::find(receivers.begin(), receivers.end(), recv);

    if (it != receivers.end())
        receivers.erase(it);
}

#endif // MODULEMANAGER_H
