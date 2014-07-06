#ifndef CLIENTSERVICE_H
#define CLIENTSERVICE_H

#include <sys/socket.h>
#include <vector>
#include <cstring>
#include "modulemanager.h"
#include "internlmsgreceiver.h"
#include "internlmsg.h"
#include "internlmsgsender.h"

template<class D>
class clientservice : public internlmsgsender<D>
{
    typedef internlmsgreceiver<D> WORKER;
    const char * waiting_for_answer;
    const char * sending_hello;
    const char * answer_received;
    const char * closing_conn;
    const modulemanager<D> * const mod_mgr;
    enum STATE
    {
        INITIAL,
        WAITING_FOR_ANSWER,
        CLOSE
    };

    enum MESSAGE_TO_SENSOR
    {
        GREETING,
        SEND_ME_VALUES,
        GET_FROM_ME_VALUES
    };

    STATE state;
    int sock;
    bool stop;

public:
   clientservice(int sock_, const modulemanager<D> * const mod_mgr_);
   void operator()();
};

template<class D>
clientservice<D>::clientservice(int sock_, const modulemanager<D> * const mod_mgr_) :
                                                            mod_mgr(mod_mgr_),
                                                            state(INITIAL), sock(sock_),
                                                            waiting_for_answer("[clientservice] waiting for answer"),
                                                            sending_hello("[clientservice] sending hello"),
                                                            answer_received("[clientservice] answer received: "),
                                                            closing_conn("[clientservice] closing connection")
{
    std::vector<INTNLMSG::RECEIVER> receivers_to_get;
    receivers_to_get.push_back(INTNLMSG::RECEIVER::RECV_DISPLAY);
    mod_mgr->get_receivers(receivers_to_get, this->workers);
}

template<class D>
void clientservice<D>::operator()()
{
    char chBfr[100];

    while(!stop)
    {
        switch (state)
        {
            case INITIAL:
            {
                int res = 0;
                D msg = D(INTNLMSG::RECV_DISPLAY, 0, std::move(std::string(sending_hello)));
                send_internl_msg(std::move(msg));
                std::string hello("hello!\n");
                res = send(sock, hello.c_str() , hello.length(), 0);
                state = WAITING_FOR_ANSWER;
            }
            break;

            case WAITING_FOR_ANSWER:
            {
                D msg = D(INTNLMSG::RECV_DISPLAY, 0, std::move(std::string(waiting_for_answer)));
                send_internl_msg(std::move(msg));
                memset(chBfr, 0, 2);
                recv(sock, chBfr, 2, 0);
                msg.setmsg(std::move(std::string(answer_received) + std::string(chBfr)));
                send_internl_msg(std::move(msg));
                state = CLOSE;
            }
            break;
            case CLOSE:
            {
                D msg = D(INTNLMSG::RECV_DISPLAY, 0, std::move(std::string(closing_conn)));
                send_internl_msg(std::move(msg));
                close(sock);
                stop = true;
            }
            break;
            default:
            break;
        }
    }
}
#endif // CLIENTSERVICE_H
