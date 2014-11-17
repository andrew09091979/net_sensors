#ifndef NETCONNECTIONHANDLER_H
#define NETCONNECTIONHANDLER_H

#include <mutex>
#include <queue>
#include <condition_variable>
#include <thread>
#include "modulemanager.h"
#include "internlmsgreceiver.h"
#include "internlmsgsender.h"
#include "internlmsg.h"
#include "deviceandroid.h"
#include "deviceremconsole.h"
#include "protocolandroiddev.h"
#include "protocolremconsole.h"
#include "connectionhandler.h"

template<class D>
class netconnectionhandler : public internlmsgreceiver<D>, public internlmsgsender<D>
{
    typedef internlmsgreceiver<D> WORKER;
    modulemanager<D> * const mod_mgr;
    const char * start_clientserv;
    const char * sock_is_invalid;
    const char * invalid_dev_type;
    const char * cannot_get_dev_type;
    int sock;

public:
    typename internlmsgreceiver<D>::HANDLE_RES HandleMsg(D data);
    netconnectionhandler(modulemanager<D> * const mod_mgr_);
    ~netconnectionhandler(){}
};

template<class D>
netconnectionhandler<D>::netconnectionhandler(modulemanager<D> * const mod_mgr_) :
                                                    internlmsgreceiver<D>(INTNLMSG::RECV_NETCONNHANDLER),
                                                    mod_mgr(mod_mgr_),
                                                    start_clientserv("[netconnectionhandler] starting device"),
                                                    sock_is_invalid("[netconnectionhandler] socket is invalid"),
                                                    invalid_dev_type("[netconnectionhandler] invalid device type"),
                                                    cannot_get_dev_type("[netconnectionhandler] cannot get device type")
{
    std::vector<INTNLMSG::RECEIVER> receivers_to_get;
    receivers_to_get.push_back(INTNLMSG::RECEIVER::RECV_DISPLAY);
    receivers_to_get.push_back(INTNLMSG::RECEIVER::RECV_DEVICE_MANAGER);
    mod_mgr->get_receivers(receivers_to_get, this->workers);
}

template<class D>
typename internlmsgreceiver<D>::HANDLE_RES netconnectionhandler<D>::HandleMsg(D data)
{
    typename internlmsgreceiver<D>::HANDLE_RES res = internlmsgreceiver<D>::HANDLE_FAILED;

    if (data.getreceiver() == INTNLMSG::RECV_NETCONNHANDLER)
    {
        sock = data.getval();

        if (sock != -1)
        {
            connectionhandler ch(sock);
            char dev_type[3];

            if (ch.read_nbytes(dev_type, 3))
            {
                switch (dev_type[0])
                {
                    case 0x31:
                    {
                        std::shared_ptr<connectionhandler> conn(new connectionhandler(sock));
                        std::shared_ptr<protocol<char>> prot(new protocolandroiddev(conn));
                        deviceandroid<D> dev(mod_mgr, prot);

                        this->send_internl_msg(INTNLMSG::RECV_DISPLAY, 0,
                                               std::move(std::string(start_clientserv)));
                        this->send_internl_msg(INTNLMSG::RECV_DEVICE_MANAGER, 1,
                                               std::move(std::string(start_clientserv)));
                        //std::thread thr = std::thread(std::reference_wrapper<deviceandroid<D>>(dev));
                        std::thread thr = std::thread(dev);
                        thr.detach();
                        res = internlmsgreceiver<D>::HANDLE_OK;
                    }
                    break;
                    case 0x32:
                    {
                        std::shared_ptr<connectionhandler> conn(new connectionhandler(sock));
                        std::shared_ptr<protocol<char>> prot(new protocolremconsole(conn));
                        deviceremconsole<D> dev(mod_mgr, prot);

                        this->send_internl_msg(INTNLMSG::RECV_DISPLAY, 0,
                                               std::move(std::string(start_clientserv)));
                        this->send_internl_msg(INTNLMSG::RECV_DEVICE_MANAGER, 1,
                                               std::move(std::string(start_clientserv)));
                        //std::thread thr = std::thread(std::reference_wrapper<deviceremconsole<D>>(dev));
                        std::thread thr = std::thread(dev);
                        thr.detach();
                        res = internlmsgreceiver<D>::HANDLE_OK;
                    }
                    break;
                    default:
                        this->send_internl_msg(INTNLMSG::RECV_DISPLAY, 0,
                                               std::move(std::string(invalid_dev_type)));
                    break;
                }
            }
            else
            {
                this->send_internl_msg(INTNLMSG::RECV_DISPLAY, 0,
                                       std::move(std::string(cannot_get_dev_type)));
            }
        }
        else
        {
            this->send_internl_msg(INTNLMSG::RECV_DISPLAY, 0, std::move(std::string(sock_is_invalid)));
        }
    }
    return res;
}

#endif // NETCONNECTIONHANDLER_H
