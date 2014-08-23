#ifndef DEVICEREMCONSOLE_H
#define DEVICEREMCONSOLE_H

#include <thread>
#include <functional>
#include "device.h"
#include "modulemanager.h"
#include "protocol.h"
#include "internlmsgreceiver.h"

template <class D>
class deviceremconsole;

template <class D>
class internlmsgreceivr : public internlmsgreceiver<D>
{
    std::shared_ptr<device<D>> devptr;

public:
    internlmsgreceivr(device<D> * const dev_, INTNLMSG::RECEIVER iam_) : devptr(dev_),
                                                                         internlmsgreceiver<D>(iam_)
    {

    }

    ~internlmsgreceivr()
    {

    }

    typename internlmsgreceiver<D>::HANDLE_RES HandleMsg(D data)
    {
        if (devptr != nullptr)
        {
            return devptr->HandleInternalMsg(std::move(data));
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
    STATE state;

    modulemanager<D> * const mod_mgr;
    std::shared_ptr<protocol<char> > protocol_dev;
    const std::string devName;

public:
    deviceremconsole(modulemanager<D> * const mod_mgr_,
                    std::shared_ptr<protocol<char> > protocol_);
    ~deviceremconsole()
    {

    }
    void operator()();
    INTMSGRES HandleInternalMsg(D data);
};

template <class D>
deviceremconsole<D>::deviceremconsole(modulemanager<D> * const mod_mgr_,
                                      std::shared_ptr<protocol<char> > protocol_) :
                                                                   cmd_received("[remote console] - command received: "),
                                                                   unknown_cmd("[remote console] - unknown command\n"),
                                                                   num_of_devs_demanded("[remote console] - number of devices demanded\n"),
                                                                   stop(false),
                                                                   state(STATE::INITIAL),
                                                                   mod_mgr(mod_mgr_),
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
//    std::thread thrd = std::thread(std::reference_wrapper<internlmsgreceivr<D>>(internalmsgreceiver));
    std::thread thrd = std::thread(internalmsgreceiver);
//    mod_mgr->register_receiver(&this->internalmsgreceiver);
    mod_mgr->register_receiver(&internalmsgreceiver);
    std::vector<INTNLMSG::RECEIVER> receivers_to_get;
    receivers_to_get.push_back(INTNLMSG::RECEIVER::RECV_DISPLAY);
    receivers_to_get.push_back(INTNLMSG::RECEIVER::RECV_DEVICE_MANAGER);
    mod_mgr->get_receivers(receivers_to_get, this->workers);

    while (!stop)
    {
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

                this->send_internl_msg(INTNLMSG::RECV_DISPLAY, 0,
                                       std::move(std::string(cmd_received) + command));

                if (command.compare(std::string("exit")) == 0)
                {
                    state = SHUTDOWN;
                }
                else if(command.compare(std::string("devices")) == 0)
                {
                    this->send_internl_msg(INTNLMSG::RECV_DISPLAY, 0,
                                           std::move(std::string("num_of_devs_demanded")));
                    this->send_internl_msg(INTNLMSG::RECV_DEVICE_MANAGER, 3,
                                           std::move(std::string("num_of_devs_demanded")));
                }
                else if(command.compare(std::string("shutdown")) == 0)
                {

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
                this->send_internl_msg(INTNLMSG::RECV_DISPLAY, 0,
                                       std::move(devName + std::string(" - shutdown")));
                this->send_internl_msg(INTNLMSG::RECV_DEVICE_MANAGER, 0,
                                       std::move(devName + std::string(" - shutdown")));
                //this->internalmsgreceiver << D(INTNLMSG::RECV_DEVICE, -1, std::move(std::string("exit")));
                internalmsgreceiver << D(INTNLMSG::RECV_DEVICE, -1, std::move(std::string("exit")));
            }
            break;

            default:
            break;
        }
    }
    if (thrd.joinable())
        thrd.join();
}

template <class D>
typename deviceremconsole<D>::INTMSGRES deviceremconsole<D>::HandleInternalMsg(D data)
{
    typename deviceremconsole<D>::INTMSGRES res = internlmsgreceiver<D>::HANDLE_FAILED;
    int command = data.getval();

    switch (command)
    {
        case -1:
        {
            //this->internalmsgreceiver.stopthread();
        }
        break;

        case 3://num_of_devs_demanded
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
