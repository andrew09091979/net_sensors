#ifndef CLIENTSERVICE_H
#define CLIENTSERVICE_H

#include <sys/socket.h>
#include <vector>
#include <cstring>
#include "modulemanager.h"
#include "message.h"
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
    const char * recv_error;
    const char * header_read_error;

    const modulemanager<D> * const mod_mgr;
    enum STATE
    {
        INITIAL,
        WAITING_FOR_INIT_ANSWER,
        WAITING_FOR_ANSWER,
        POST_REQUEST,
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


    bool read_nbytes(const ssize_t bytes_to_read, char * bfr);
    short unsigned int read_header();
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
                                                            closing_conn("[clientservice] closing connection"),
                                                            recv_error("[clientservice] cannot receive message"),
                                                            header_read_error("[clientservice] cannot read header")
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
                state = WAITING_FOR_INIT_ANSWER;
            }
            break;
            case WAITING_FOR_INIT_ANSWER:
            {
                ssize_t msg_size = read_header();

                if (msg_size > 0)
                {
                    char * bfr = new char[msg_size];
                    memset(bfr, 0, msg_size);

                    if (read_nbytes(msg_size, bfr))
                    {
                        D msg = D(INTNLMSG::RECV_DISPLAY, 0, std::move(std::string(answer_received) + std::string(bfr)));
                        send_internl_msg(std::move(msg));
                    }
                    else
                    {
                        D msg = D(INTNLMSG::RECV_DISPLAY, 0, std::move(std::string(recv_error)));
                        send_internl_msg(std::move(msg));
                    }
                    delete [] bfr;
                }
                else
                {
                    D msg = D(INTNLMSG::RECV_DISPLAY, 0, std::move(std::string(header_read_error)));
                    send_internl_msg(std::move(msg));
                }

                state = CLOSE;
            }
            break;
            case POST_REQUEST:
            {
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

template<class D>
short unsigned int clientservice<D>::read_header()
{
    char size[2];
    short unsigned int res;
    read_nbytes(2, size);
    res = *(short unsigned int *)size;

    return res;
}

template<class D>
bool clientservice<D>::read_nbytes(const ssize_t bytes_to_read, char *bfr)
{
    bool res = false;
    char *bfr_ptr = bfr;
    ssize_t remain_bytes = bytes_to_read;
    ssize_t received = 0;

    while (remain_bytes > 0)
    {
        received = recv(sock, bfr_ptr, remain_bytes, 0);

        if (received > 0)
        {
           remain_bytes -= received;
        }
        else
        {
            break;
        }
    }

    if (remain_bytes <= 0)
        res = true;

    return res;
}
#endif // CLIENTSERVICE_H
