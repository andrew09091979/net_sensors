#ifndef DEVICEREMCONSOLE_H
#define DEVICEREMCONSOLE_H

#include <thread>
#include <functional>
#include "device.h"
#include "protocol.h"
#include "internlmsgreceiver.h"
#include "internlmsgrouter.h"

template <class D>
class deviceremconsole;

template <class D>
class internlmsgreceivr : public internlmsgreceiver<D>
{
    device<D> *const dev;

public:
    internlmsgreceivr(device<D> * const dev_, INTNLMSG::RECEIVER iam_) : dev(dev_),
                                                                         internlmsgreceiver<D>(iam_)
    {

    }

    ~internlmsgreceivr()
    {

    }

    typename internlmsgreceiver<D>::HANDLE_RES HandleMsg(D data)
    {
        if (dev != nullptr)
        {
            return dev->HandleInternalMsg(std::move(data));
        }
        else
        {
            return internlmsgreceiver<D>::HANDLE_FAILED;
        }
    }
};


template <class D>
class deviceremconsole : public device<D>
{
    enum STATE
    {
        INITIAL,
        WORK,
        SHUTDOWN
    };

    typedef typename device<D>::INTMSGRES INTMSGRES;
    const char * cmd_received;
    const char * unknown_cmd;
    const char * num_of_devs_demanded;
    std::vector<std::string> commands;
    bool stop;
    bool shutdown_ordered;
    STATE state;
    internlmsgrouter<D> * const internlmsg_router;
    std::shared_ptr<protocol<char> > protocol_dev;
    const std::string devName;
    internlmsgreceivr<D> *imr_ptr;

public:
    deviceremconsole(internlmsgrouter<D> * const internlmsg_router_,
                    std::shared_ptr<protocol<char> > protocol_);
    ~deviceremconsole()
    {

    }
    void operator()();
    INTMSGRES HandleInternalMsg(D data);
};

template <class D>
deviceremconsole<D>::deviceremconsole(internlmsgrouter<D> * const internlmsg_router_,
                                      std::shared_ptr<protocol<char> > protocol_) :
                                                                   device<D>(internlmsg_router_),
                                                                   cmd_received("[remote console] - command received: "),
                                                                   unknown_cmd("[remote console] - unknown command\n"),
                                                                   num_of_devs_demanded("[remote console] - number of devices demanded\n"),
                                                                   stop(false),
                                                                   state(STATE::INITIAL),
                                                                   internlmsg_router(internlmsg_router_),
                                                                   protocol_dev(protocol_),
                                                                   devName("remote console")
{
    commands .push_back(std::string("devices"));
    commands .push_back(std::string("shutdown"));
}

template <class D>
void deviceremconsole<D>::operator ()()
{
    internlmsgreceivr<D> internalmsgreceiver(this, INTNLMSG::RECV_DEVICE);
    imr_ptr = &internalmsgreceiver;
    std::reference_wrapper<internlmsgreceivr<D> > rv = std::reference_wrapper<internlmsgreceivr<D> >(internalmsgreceiver);
    std::thread thrd = std::thread(rv);
    internlmsg_router->register_receiver(imr_ptr);

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
                char cmds_str[] = "commands available:\ndevices\nshutdown\nexit\n";
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

                this->send_internl_msg(INTNLMSG::RECV_DISPLAY, INTNLMSG::SHOW_MESSAGE,
                                       std::move(std::string(cmd_received) + command));

                if (command.compare(std::string("exit")) == 0)
                {
                    state = SHUTDOWN;
                }
                else if(command.compare(std::string("devices")) == 0)
                {
                    this->send_internl_msg(INTNLMSG::RECV_DISPLAY, INTNLMSG::SHOW_MESSAGE,
                                           std::move(std::string("num_of_devs_demanded")));
                    this->send_internl_msg(INTNLMSG::RECV_DEVICE_MANAGER, INTNLMSG::GET_NUM_OF_DEVS,
                                           std::move(std::string("num_of_devs_demanded")));
                }
                else if(command.compare(std::string("shutdown")) == 0)
                {
                    this->send_internl_msg(INTNLMSG::RECV_DISPLAY, INTNLMSG::SHOW_MESSAGE,
                                           std::move(std::string("shutdown")));
                    this->send_internl_msg(INTNLMSG::RECV_DEVICE_MANAGER, INTNLMSG::SHUTDOWN_ALL,
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
                this->send_internl_msg(INTNLMSG::RECV_DISPLAY, INTNLMSG::SHOW_MESSAGE,
                                       std::move(devName + std::string(" - shutdown")));
                this->send_internl_msg(INTNLMSG::RECV_DEVICE_MANAGER, INTNLMSG::DEVICE_SHUTDOWN,
                                       std::move(devName + std::string(" - shutdown")));
                internlmsg_router->deregister_receiver(imr_ptr);
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
typename deviceremconsole<D>::INTMSGRES deviceremconsole<D>::HandleInternalMsg(D data)
{
    typename deviceremconsole<D>::INTMSGRES res = internlmsgreceiver<D>::HANDLE_FAILED;
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
#endif // DEVICEREMCONSOLE_H
