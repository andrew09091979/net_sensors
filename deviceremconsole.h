#ifndef DEVICEREMCONSOLE_H
#define DEVICEREMCONSOLE_H

#include "device.h"
#include "modulemanager.h"
#include "protocol.h"
#include "internlmsgreceiver.h"

template <class D>
class deviceremconsole : public device<D>
{
    enum STATE
    {
        INITIAL,
        WORK,
        SHUTDOWN
    };
    const char * cmd_received;
    std::vector<std::string> commands;
    bool stop;
    STATE state;
    const modulemanager<D> * const mod_mgr;
    std::shared_ptr<protocol<char> > protocol_dev;
    const std::string devName;
public:
    deviceremconsole(const modulemanager<D> * const mod_mgr_,
           std::shared_ptr<protocol<char> > protocol_);
    ~deviceremconsole(){}
    void operator()();
    typename internlmsgreceiver<D>::HANDLE_RES HandleMsg(D data);
};

template <class D>
deviceremconsole<D>::deviceremconsole(const modulemanager<D> * const mod_mgr_,
                                      std::shared_ptr<protocol<char> > protocol_) :
                                                                   cmd_received("[remote console] - command received: "),
                                                                   stop(false),
                                                                   state(STATE::INITIAL),
                                                                   mod_mgr(mod_mgr_),
                                                                   protocol_dev(protocol_),
                                                                   devName("remote console")
{
    std::vector<INTNLMSG::RECEIVER> receivers_to_get;
    receivers_to_get.push_back(INTNLMSG::RECEIVER::RECV_DISPLAY);
    receivers_to_get.push_back(INTNLMSG::RECEIVER::RECV_DEVICE_MANAGER);
    mod_mgr->get_receivers(receivers_to_get, this->workers);
    commands .push_back(std::string("devices"));
    commands .push_back(std::string("shutdown"));
}

template <class D>
void deviceremconsole<D>::operator ()()
{
    while (!stop)
    {
        switch (state)
        {
            case INITIAL:
            {
                char cmds_str[] = "commands available:\ndevices\nshutdown\n";
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
                    state = SHUTDOWN;
            }
            break;

            case SHUTDOWN:
            {
                stop = true;
                protocol_dev->shutdown();
                this->send_internl_msg(INTNLMSG::RECV_DISPLAY, 0,
                                       std::move(devName + std::string("- shutdown")));
                this->send_internl_msg(INTNLMSG::RECV_DEVICE_MANAGER, 0,
                                       std::move(devName + std::string("- shutdown")));
            }
            break;

            default:
            break;
        }
    }
}

template <class D>
typename internlmsgreceiver<D>::HANDLE_RES deviceremconsole<D>::HandleMsg(D data)
{
    typename internlmsgreceiver<D>::HANDLE_RES res = internlmsgreceiver<D>::HANDLE_FAILED;

    return res;
}

#endif // DEVICEREMCONSOLE_H
