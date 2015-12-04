﻿#ifndef DeviceREMCONSOLE_H
#define DeviceREMCONSOLE_H

#include <thread>
#include <functional>
#include "Device.h"
#include "Protocol.h"
#include "Internalmsgreceiver.h"
#include "Internalmsgrouter.h"

template <class D>
class Deviceremconsole;

template <class D>
class Internalmsgreceivr : public Internalmsgreceiver<D>
{
    Device<D> *const dev;

public:
    Internalmsgreceivr(Device<D> * const dev_, INTNLMSG::RECEIVER iam_) : Internalmsgreceiver<D>(iam_),
                                                                         dev(dev_)

    {

    }

    ~Internalmsgreceivr()
    {

    }

    typename Internalmsgreceiver<D>::HANDLE_RES HandleMsg(D data)
    {
        if (dev != nullptr)
        {
            return dev->HandleInternalMsg(std::move(data));
        }
        else
        {
            return Internalmsgreceiver<D>::HANDLE_FAILED;
        }
    }
};


template <class D>
class Deviceremconsole : public Device<D>
{
    enum STATE
    {
        INITIAL,
        WORK,
        SHUTDOWN
    };

    typedef typename Device<D>::INTMSGRES INTMSGRES;
    const char * cmd_received;
    const char * unknown_cmd;
    const char * num_of_devs_demanded;
    std::vector<std::string> commands;
    bool stop;
    bool shutdown_ordered;
    STATE state;
    Internalmsgrouter<D> * const Internalmsg_router;
    std::shared_ptr<Protocol<char> > protocol_dev;
    const std::string devName;
    Internalmsgreceivr<D> *imr_ptr;

public:
    Deviceremconsole(Internalmsgrouter<D> * const Internalmsg_router_,
                    std::shared_ptr<Protocol<char> > protocol_);
    ~Deviceremconsole()
    {

    }
    void operator()();
    INTMSGRES HandleInternalMsg(D data);
};

template <class D>
Deviceremconsole<D>::Deviceremconsole(Internalmsgrouter<D> * const Internalmsg_router_,
                                      std::shared_ptr<Protocol<char> > protocol_) :
                                                                   Device<D>(Internalmsg_router_),
                                                                   cmd_received("[remote console] - command received: "),
                                                                   unknown_cmd("[remote console] - unknown command\n"),
                                                                   num_of_devs_demanded("[remote console] - number of Devices demanded\n"),
                                                                   stop(false),
                                                                   shutdown_ordered(false),
                                                                   state(STATE::INITIAL),
                                                                   Internalmsg_router(Internalmsg_router_),
                                                                   protocol_dev(protocol_),
                                                                   devName("remote console")
{
    commands .push_back(std::string("Devices"));
    commands .push_back(std::string("shutdown"));
}

template <class D>
void Deviceremconsole<D>::operator ()()
{
    Internalmsgreceivr<D> internalmsgreceiver(this, INTNLMSG::RECV_Device);
    imr_ptr = &internalmsgreceiver;
    std::reference_wrapper<Internalmsgreceivr<D> > rv = std::reference_wrapper<Internalmsgreceivr<D> >(internalmsgreceiver);
    std::thread thrd = std::thread(rv);
    Internalmsg_router->register_receiver(imr_ptr);

    while (!stop)
    {
        if (shutdown_ordered)
        {
            state = SHUTDOWN;
        }

        switch (state)
        {
            case INITIAL:
            {
                char cmds_str[] = "commands available:\nDevices\nshutdown\nexit\n";
                arraywrapper<char> cmds = arraywrapper<char>(strlen(cmds_str) + 1);
                cmds.zero_mem();
                strcpy(cmds.at(0), cmds_str);
                protocol_dev->sendData(cmds);
                state = WORK;
            }
            break;

            case WORK:
            {
                std::string command;
                protocol_dev->getCommand(1, command);

                this->send_internal_msg(INTNLMSG::RECV_DISPLAY, INTNLMSG::SHOW_MESSAGE,
                                       std::move(std::string(cmd_received) + command));

                if (command.compare(std::string("exit")) == 0)
                {
                    state = SHUTDOWN;
                }
                else if(command.compare(std::string("Devices")) == 0)
                {
                    this->send_internal_msg(INTNLMSG::RECV_DISPLAY, INTNLMSG::SHOW_MESSAGE,
                                           std::move(std::string("num_of_devs_demanded")));
                    this->send_internal_msg(INTNLMSG::RECV_Device_MANAGER, INTNLMSG::GET_NUM_OF_DEVS,
                                           std::move(std::string("num_of_devs_demanded")));
                }
                else if(command.compare(std::string("shutdown")) == 0)
                {
                    this->send_internal_msg(INTNLMSG::RECV_Device_MANAGER, INTNLMSG::SHUTDOWN_ALL,
                                           std::move(std::string("shutdown")));
                }
                else
                {
                    arraywrapper<char> cmd = arraywrapper<char>(strlen(unknown_cmd) + 1);
                    cmd.zero_mem();
                    strcpy(cmd.at(0), unknown_cmd);
                    protocol_dev->sendData(cmd);
                }
            }
            break;

            case SHUTDOWN:
            {
                stop = true;
                protocol_dev->shutdown();
                this->send_internal_msg(INTNLMSG::RECV_DISPLAY, INTNLMSG::SHOW_MESSAGE,
                                       std::move(devName + std::string(" - shutdown")));
                this->send_internal_msg(INTNLMSG::RECV_Device_MANAGER, INTNLMSG::Device_SHUTDOWN,
                                       std::move(devName + std::string(" - shutdown")));
                Internalmsg_router->deregister_receiver(imr_ptr);
                this->imr_ptr->stopthread();

                if (thrd.joinable())
                    thrd.join();
            }
            break;

            default:
            break;
        }
    }
}

template <class D>
typename Deviceremconsole<D>::INTMSGRES Deviceremconsole<D>::HandleInternalMsg(D data)
{
    typename Deviceremconsole<D>::INTMSGRES res = Internalmsgreceiver<D>::HANDLE_FAILED;
    int command = data.getval();

    switch (command)
    {
        case INTNLMSG::SHUTDOWN_ALL:
        {
            shutdown_ordered = true;
        }
        break;

        case INTNLMSG::GET_NUM_OF_DEVS://num_of_devs_demanded
        {
            arraywrapper<char> msg(data.getsize());
            memcpy(msg.at(0), data.getmsg(), data.getsize());
            protocol_dev->sendData(msg);
        }
        break;
    }

    return res;
}
#endif // DeviceREMCONSOLE_H
