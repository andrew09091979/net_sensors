#ifndef CLIENTSERVICE_H
#define CLIENTSERVICE_H

#include <sys/socket.h>
#include <vector>
#include <cstring>
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
   clientservice(int sock_, WORKER * wrk_);
   void operator()();
};

template<class D>
clientservice<D>::clientservice(int sock_, WORKER * wrk_) : internlmsgsender<D>(wrk_), state(INITIAL), sock(sock_),
                                                            waiting_for_answer("[clientservice] waiting for answer"),
                                                            sending_hello("[clientservice] sending hello"),
                                                            answer_received("[clientservice] answer received: "),
                                                            closing_conn("[clientservice] closing connection")
{
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
                D msg = D(INTNLMSG::RECV_DISPLAY, strlen(sending_hello),
                          0, std::move(std::string(sending_hello)));
                send_internl_msg(std::move(msg));
                std::string hello("hello!\n");
                res = send(sock, hello.c_str() , hello.length(), 0);
                state = WAITING_FOR_ANSWER;
            }
            break;

            case WAITING_FOR_ANSWER:
            {
                D msg = D(INTNLMSG::RECV_DISPLAY, strlen(waiting_for_answer),
                          0, std::move(std::string(waiting_for_answer)));
                send_internl_msg(std::move(msg));
                memset(chBfr, 0, 2);
                recv(sock, chBfr, 2, 0);
                msg.setsize(strlen(answer_received));
                msg.setmsg(std::move(std::string(answer_received)));
                send_internl_msg(std::move(msg));
                state = CLOSE;
            }
            break;
            case CLOSE:
            {
                D msg = D(INTNLMSG::RECV_DISPLAY, strlen(closing_conn),
                          0, std::move(std::string(closing_conn)));
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
