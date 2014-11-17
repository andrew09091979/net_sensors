#ifndef DEVICEMANAGER_H
#define DEVICEMANAGER_H

#include "internlmsgreceiver.h"
#include "internlmsgsender.h"
#include "internlmsg.h"
#include "modulemanager.h"

template<class D>
class devicemanager  : public internlmsgreceiver<D>, public internlmsgsender<D>
{
    const modulemanager<D> * const mod_mgr;
    int numOfDevices;
    const char *numOfDevs;

    bool stop;
public:
    devicemanager(const modulemanager<D> * const mod_mgr_);
    typename internlmsgreceiver<D>::HANDLE_RES HandleMsg(D data);
    ~devicemanager() {}
};

template<class D>
devicemanager<D>::devicemanager(const modulemanager<D> * const mod_mgr_):
                                        internlmsgreceiver<D>(INTNLMSG::RECV_DEVICE_MANAGER),
                                        mod_mgr(mod_mgr_),
                                        numOfDevices(0),
                                        numOfDevs("[device manager] number of devices: ")
{
    std::vector<INTNLMSG::RECEIVER> receivers_to_get;
    receivers_to_get.push_back(INTNLMSG::RECEIVER::RECV_DISPLAY);
    mod_mgr->get_receivers(receivers_to_get, this->workers);
}

template<class D>
typename internlmsgreceiver<D>::HANDLE_RES devicemanager<D>::HandleMsg(D data)
{
    int whatHappened = data.getval();

    if (whatHappened == 0)//devive closed
    {
        --numOfDevices;
    }
    else if(whatHappened == 1)//device connected
    {
        ++numOfDevices;
    }
    else if(whatHappened == 3)//remote console asked number of devices
    {
        this->workers.clear();
        std::vector<INTNLMSG::RECEIVER> receivers_to_get;
        receivers_to_get.push_back(INTNLMSG::RECEIVER::RECV_DISPLAY);
        receivers_to_get.push_back(INTNLMSG::RECEIVER::RECV_DEVICE);
        mod_mgr->get_receivers(receivers_to_get, this->workers);

        this->send_internl_msg(INTNLMSG::RECV_DEVICE, 3, std::string(numOfDevs)
                                 + std::to_string(numOfDevices));
    }
    this->send_internl_msg(INTNLMSG::RECV_DISPLAY, 0, std::string(numOfDevs)
                           + std::to_string(numOfDevices));// + std::string("\n"));

    return internlmsgreceiver<D>::HANDLE_OK;
}

#endif // DEVICEMANAGER_H
