#ifndef CLIENTSERVICE_H
#define CLIENTSERVICE_H

#include <sys/socket.h>
#include <vector>
#include <cstring>
#include "worker.h"

template<class D>
class clientservice
{
    typedef worker<D> WORKER;

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
    std::vector<WORKER *> workers;
    void notify_all(const D &msg);

public:
   clientservice(int sock_, WORKER * wrk_);
   void operator()();
};

template<class D>
clientservice<D>::clientservice(int sock_, WORKER * wrk_) : state(INITIAL), sock(sock_)
{
    workers.push_back(wrk_);
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
                D msg = D(0, std::string("[clientservice] sending hello"));
                notify_all(msg);
                std::string hello("hello!\n");
                res = send(sock, hello.c_str() , hello.length(), 0);
                state = WAITING_FOR_ANSWER;
            }
            break;

            case WAITING_FOR_ANSWER:
            {
                D msg = D(0, std::string("[clientservice] waiting for answer"));
                notify_all(msg);
                memset(chBfr, 0, 2);
                recv(sock, chBfr, 2, 0);
                msg = D(0, std::string("[clientservice] received message:") +
                            std::string(chBfr));
                notify_all(msg);
                state = CLOSE;
            }
            break;
            case CLOSE:
            {
                D msg = D(0, std::string("[clientservice] closing connection"));
                notify_all(msg);
                close(sock);
                stop = true;
            }
            break;
            default:
            break;
        }
    }
}

template<class D>
void clientservice<D>::notify_all(const D &msg)
{
    typename std::vector<WORKER *>::iterator it = workers.begin();

    for(;it != workers.end(); ++it)
        *(*it) << msg;
}
#endif // CLIENTSERVICE_H
