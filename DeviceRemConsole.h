#ifndef DeviceREMCONSOLE_H
#define DeviceREMCONSOLE_H

#include <thread>
#include <functional>
#include "Device.h"
#include "Protocol.h"
#include "InternalMsgReceiver.h"
#include "InternalMsgRouter.h"

template <class D>
class DeviceRemConsole;

template <class D>
class Internalmsgreceivr : public InternalMsgReceiver<D>
{
    Device<D> *const dev;

public:
    Internalmsgreceivr(Device<D> * const dev_, INTERNALMESSAGE::RECEIVER iam_) : InternalMsgReceiver<D>(iam_),
                                                                         dev(dev_)

    {

    }

    ~Internalmsgreceivr()
    {

    }

    typename InternalMsgReceiver<D>::HANDLE_RES HandleMsg(D data)
    {
        if (dev != nullptr)
        {
            return dev->HandleInternalMsg(std::move(data));
        }
        else
        {
            return InternalMsgReceiver<D>::HANDLE_FAILED;
        }
    }
};


template <class D>
class DeviceRemConsole : public Device<D>
{
    enum STATE
    {
        INITIAL,
        WORK,
        SHUTDOWN
    };

    typedef typename Device<D>::INTERNAL_MSG_HANDLE_RES INTMSGRES;
    const char * cmd_received;
    const char * unknown_cmd;
    const char * num_of_devs_demanded;
    std::vector<std::string> commands;
    bool stop;
    bool shutdown_ordered;
    STATE state;
    InternalMsgRouter<D> * const Internalmsg_router;
    std::shared_ptr<Protocol<char> > protocol_dev;
    const std::string devName;
    Internalmsgreceivr<D> *imr_ptr;

public:
    DeviceRemConsole(InternalMsgRouter<D> * const Internalmsg_router_,
                    std::shared_ptr<Protocol<char> > protocol_);
    ~DeviceRemConsole()
    {

    }
    void operator()();
    INTMSGRES HandleInternalMsg(D data);
};

template <class D>
DeviceRemConsole<D>::DeviceRemConsole(InternalMsgRouter<D> * const Internalmsg_router_,
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
    commands.push_back(std::string("devices"));
    commands.push_back(std::string("shutdown"));
}

template <class D>
void DeviceRemConsole<D>::operator ()()
{
    Internalmsgreceivr<D> InternalMsgReceiver(this, INTERNALMESSAGE::RECV_DEVICE);
    imr_ptr = &InternalMsgReceiver;
    std::reference_wrapper<Internalmsgreceivr<D> > rv = std::reference_wrapper<Internalmsgreceivr<D> >(InternalMsgReceiver);
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

                this->send_internal_msg(INTERNALMESSAGE::RECV_DISPLAY, INTERNALMESSAGE::SHOW_MESSAGE,
                                       std::move(std::string(cmd_received) + command));

                if (command.compare(std::string("exit")) == 0)
                {
                    state = SHUTDOWN;
                }
                else if(command.compare(std::string("devices")) == 0)
                {
                    this->send_internal_msg(INTERNALMESSAGE::RECV_DISPLAY, INTERNALMESSAGE::SHOW_MESSAGE,
                                           std::move(std::string("num_of_devs_demanded")));
                    this->send_internal_msg(INTERNALMESSAGE::RECV_DEVICE_MANAGER, INTERNALMESSAGE::GET_NUM_OF_DEVS,
                                           std::move(std::string("num_of_devs_demanded")));
                }
                else if(command.compare(std::string("shutdown")) == 0)
                {
                    this->send_internal_msg(INTERNALMESSAGE::RECV_DEVICE_MANAGER, INTERNALMESSAGE::SHUTDOWN_ALL,
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
                this->send_internal_msg(INTERNALMESSAGE::RECV_DISPLAY, INTERNALMESSAGE::SHOW_MESSAGE,
                                       std::move(devName + std::string(" - shutdown")));
                this->send_internal_msg(INTERNALMESSAGE::RECV_DEVICE_MANAGER, INTERNALMESSAGE::DEVICE_SHUTDOWN,
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
typename DeviceRemConsole<D>::INTMSGRES DeviceRemConsole<D>::HandleInternalMsg(D data)
{
    typename DeviceRemConsole<D>::INTMSGRES res = InternalMsgReceiver<D>::HANDLE_FAILED;
    int command = data.getval();

    switch (command)
    {
        case INTERNALMESSAGE::SHUTDOWN_ALL:
        {
            shutdown_ordered = true;
        }
        break;

        case INTERNALMESSAGE::GET_NUM_OF_DEVS://num_of_devs_demanded
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
