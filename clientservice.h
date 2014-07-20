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

template <class D>
class arr_wrap
{
    D * arr;
    unsigned size;
public:
    arr_wrap(unsigned size_) : size(size_)
    {
        arr = new D[size];
    }

    ~arr_wrap()
    {
        delete [] arr;
    }

    D* at(unsigned pos)
    {
        if (pos < size)
            return &arr[pos];
        else
            return nullptr;
    }

    D& operator[](unsigned pos)
    {
        if (pos < size)
            return arr[pos];
        else
            return nullptr;
    }

    unsigned get_size() const
    {
        return size;
    }
};

template<class D>
class clientservice : public internlmsgsender<D>
{
    typedef internlmsgreceiver<D> WORKER;

    enum STATE
    {
        INITIAL,
        WAITING_FOR_INIT_ANSWER,
        WAITING_FOR_ANSWER,
        SEND_REQUEST,
        CLOSE
    };

    enum MESSAGE_TO_SENSOR
    {
        GREETING,
        SEND_ME_VALUES,
        GET_FROM_ME_VALUES
    };

    int sock;
    const modulemanager<D> * const mod_mgr;
    bool stop;
    STATE state;

    const char * waiting_for_answer;
    const char * sending_hello;
    const char * sending_request;
    const char * answer_received;
    const char * starting_byte_received;
    const char * message_size_received;
    const char * closing_conn;
    const char * recv_error;
    const char * header_read_error;

    bool read_nbytes(char * const bfr, const ssize_t bytes_to_read);
    bool send_nbytes(char * const bfr, const ssize_t bytes_to_send);
    short unsigned int read_header();

public:
   clientservice(int sock_, const modulemanager<D> * const mod_mgr_);
   void operator()();
};

template<class D>
clientservice<D>::clientservice(int sock_, const modulemanager<D> * const mod_mgr_) :
                                                            sock(sock_),
                                                            mod_mgr(mod_mgr_),
                                                            stop(false),
                                                            state(INITIAL),
                                                            waiting_for_answer("[clientservice] waiting for answer"),
                                                            sending_hello("[clientservice] sending hello"),
                                                            sending_request("[clientservice] sending request"),
                                                            answer_received("[clientservice] answer received: "),
                                                            starting_byte_received("[clientservice] starting byte received"),
                                                            message_size_received("[clientservice] message size received: "),
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
    char chBfr[100000];

    while(!stop)
    {
        switch (state)
        {
            case INITIAL:
            {
//                int res = 0;
//                this->send_internl_msg(INTNLMSG::RECV_DISPLAY, 0, std::string(sending_hello));
//                std::string hello("hello!\n");

//                res = send(sock, hello.c_str() , hello.length(), 0);
//                state = WAITING_FOR_INIT_ANSWER;
                state = SEND_REQUEST;
            }
            break;
            case WAITING_FOR_INIT_ANSWER:
            {
                ssize_t msg_size = read_header();

                if (msg_size > 0)
                {
                    arr_wrap<char> bfr(msg_size);
                    memset(bfr.at(0), 0, bfr.get_size());

                    if (read_nbytes(bfr.at(0), msg_size))
                    {
                        this->send_internl_msg(INTNLMSG::RECV_DISPLAY, 0,
                                               std::move(std::string(answer_received) + std::string(bfr.at(0))));
                    }
                    else
                    {
                        this->send_internl_msg(INTNLMSG::RECV_DISPLAY, 0, std::move(std::string(recv_error)));
                    }
                }
                else
                {
                    this->send_internl_msg(INTNLMSG::RECV_DISPLAY, 0, std::move(std::string(header_read_error)));
                }

                state = CLOSE;
            }
            break;
            case SEND_REQUEST:
            {
                this->send_internl_msg(INTNLMSG::RECV_DISPLAY, 0, std::move(std::string(sending_request)));
                arr_wrap<char> bfr(4);
                *bfr.at(0) = '#';
                *bfr.at(1) = 0x0;
                *bfr.at(2) = 0x1;
                *bfr.at(3) = 0x31;

                if (send_nbytes(bfr.at(0), bfr.get_size()))
                {
                    this->send_internl_msg(INTNLMSG::RECV_DISPLAY, 0, std::move(std::string("send_nbytes ok")));
                    state = WAITING_FOR_ANSWER;
                }
                else
                {
                    this->send_internl_msg(INTNLMSG::RECV_DISPLAY, 0, std::move(std::string("send_nbytes failed")));
                    state = CLOSE;
                }
            }
            break;
            case WAITING_FOR_ANSWER:
            {
                this->send_internl_msg(INTNLMSG::RECV_DISPLAY, 0, std::move(std::string(waiting_for_answer)));
                memset(chBfr, 0, 2);

                if (read_nbytes(chBfr, 1))
                {
                    if (chBfr[0] == '#')
                    {
                        this->send_internl_msg(INTNLMSG::RECV_DISPLAY, 0,
                                            std::move(std::string(starting_byte_received)));
                        memset(chBfr, 0, 4);

                        if (read_nbytes(chBfr, 2))
                        {
                            short size = *(short*)chBfr;
                            this->send_internl_msg(INTNLMSG::RECV_DISPLAY, 0,
                                                   std::move(std::string(message_size_received) + std::to_string(size)));

                            if (read_nbytes(chBfr, size))
                            {
                                this->send_internl_msg(INTNLMSG::RECV_DISPLAY, 0,
                                                       std::move(std::string(answer_received) + std::string(chBfr)));
                                state = SEND_REQUEST;
                            }
                            else
                            {
                                this->send_internl_msg(INTNLMSG::RECV_DISPLAY, 0,
                                                       std::move(std::string(recv_error)));
                                state = CLOSE;
                            }
                        }
                        else
                        {
                            this->send_internl_msg(INTNLMSG::RECV_DISPLAY, 0,
                                                   std::move(std::string(recv_error)));
                            state = CLOSE;
                        }
                    }
                    else
                    {

                    }
                }
                else
                {
                    this->send_internl_msg(INTNLMSG::RECV_DISPLAY, 0,
                                           std::move(std::string(recv_error)));
                    state = CLOSE;
                }

            }
            break;
            case CLOSE:
            {
                this->send_internl_msg(INTNLMSG::RECV_DISPLAY, 0, std::move(std::string(closing_conn)));
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

    read_nbytes(size, 2);
    res = *(short unsigned int *)size;

    return res;
}

template<class D>
bool clientservice<D>::read_nbytes(char * const bfr, const ssize_t bytes_to_read)
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
           bfr_ptr += received;
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

template<class D>
bool clientservice<D>::send_nbytes(char * const bfr, const ssize_t bytes_to_send)
{
    bool res = false;
    char *bfr_ptr = bfr;
    ssize_t remain_bytes = bytes_to_send;
    ssize_t sent = 0;

    while (remain_bytes > 0)
    {
        sent = send(sock, bfr_ptr, remain_bytes, MSG_NOSIGNAL);

        if (sent > 0)
        {
            remain_bytes -= sent;
            bfr_ptr += sent;
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
