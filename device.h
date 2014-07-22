#ifndef DEVICE_H
#define DEVICE_H

#include <memory>
#include <vector>
#include "arraywrapper.h"
#include "internlmsgreceiver.h"
#include "internlmsgsender.h"
#include "modulemanager.h"
#include "protocolandroiddev.h"
#include "connectionhandler.h"

template <class D>
class device : public internlmsgreceiver<D>, public internlmsgsender<D>
{
    typedef internlmsgreceiver<D> WORKER;
    const modulemanager<D> * const mod_mgr;
    std::shared_ptr<protocol> protocol_dev;
    std::shared_ptr<connectionhandler> conn_handler;
    std::string dev_name;

    const char * start_clientserv;
    const char * sock_is_invalid;

public:
    device(const modulemanager<D> * const mod_mgr_,
           std::shared_ptr<protocol> protocol_,
           std::shared_ptr<connectionhandler> conn_);
    ~device(){}
    void operator()();
    typename internlmsgreceiver<D>::HANDLE_RES HandleMsg(D data);
};

template <class D>
device<D>::device(const modulemanager<D> * const mod_mgr_,
                  std::shared_ptr<protocol> protocol_,
                  std::shared_ptr<connectionhandler> conn_) : mod_mgr(mod_mgr_),
                                                              protocol_dev(protocol_),
                                                              conn_handler(conn_)
{
    std::vector<INTNLMSG::RECEIVER> receivers_to_get;
    receivers_to_get.push_back(INTNLMSG::RECEIVER::RECV_DISPLAY);
    mod_mgr->get_receivers(receivers_to_get, this->workers);
}

template<class D>
void device<D>::operator()()
{
    char chBfr[100000];

    while(!stop)
    {
        switch (state)
        {
            case protocolandroiddev::INITIAL:
            {
//                int res = 0;
//                this->send_internl_msg(INTNLMSG::RECV_DISPLAY, 0, std::string(sending_hello));
//                std::string hello("hello!\n");

//                res = send(sock, hello.c_str() , hello.length(), 0);
//                state = WAITING_FOR_INIT_ANSWER;
                state = protocolandroiddev::SEND_REQUEST;
            }
            break;
            case protocolandroiddev::WAITING_FOR_INIT_ANSWER:
            {

                state = protocolandroiddev::CLOSE;
            }
            break;
            case protocolandroiddev::SEND_REQUEST:
            {
                this->send_internl_msg(INTNLMSG::RECV_DISPLAY, 0, std::move(std::string(sending_request)));
                arraywrapper<char> bfr(4);
                *bfr.at(0) = '#';
                *bfr.at(1) = 0x0;
                *bfr.at(2) = 0x1;
                *bfr.at(3) = 0x31;

                if (conn_->send_nbytes(bfr.at(0), bfr.get_size()))
                {
                    this->send_internl_msg(INTNLMSG::RECV_DISPLAY, 0, std::move(std::string("send_nbytes ok")));
                    state = protocolandroiddev::WAITING_FOR_ANSWER;
                }
                else
                {
                    this->send_internl_msg(INTNLMSG::RECV_DISPLAY, 0, std::move(std::string("send_nbytes failed")));
                    state = protocolandroiddev::CLOSE;
                }
            }
            break;
            case protocolandroiddev::WAITING_FOR_ANSWER:
            {
                this->send_internl_msg(INTNLMSG::RECV_DISPLAY, 0, std::move(std::string(waiting_for_answer)));
                memset(chBfr, 0, 2);

                if (conn_->read_nbytes(chBfr, 1))
                {
                    if (chBfr[0] == '#')
                    {
                        this->send_internl_msg(INTNLMSG::RECV_DISPLAY, 0,
                                            std::move(std::string(starting_byte_received)));
                        memset(chBfr, 0, 4);

                        if (conn_->read_nbytes(chBfr, 2))
                        {
                            short size = *(short*)chBfr;
                            this->send_internl_msg(INTNLMSG::RECV_DISPLAY, 0,
                                                   std::move(std::string(message_size_received) + std::to_string(size)));

                            if (conn_->read_nbytes(chBfr, size))
                            {
                                this->send_internl_msg(INTNLMSG::RECV_DISPLAY, 0,
                                                       std::move(std::string(answer_received) + std::string(chBfr)));
                                state = protocolandroiddev::SEND_REQUEST;
                            }
                            else
                            {
                                this->send_internl_msg(INTNLMSG::RECV_DISPLAY, 0,
                                                       std::move(std::string(recv_error)));
                                state = protocolandroiddev::CLOSE;
                            }
                        }
                        else
                        {
                            this->send_internl_msg(INTNLMSG::RECV_DISPLAY, 0,
                                                   std::move(std::string(recv_error)));
                            state = protocolandroiddev::CLOSE;
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
                    state = protocolandroiddev::CLOSE;
                }

            }
            break;
            case protocolandroiddev::CLOSE:
            {
                this->send_internl_msg(INTNLMSG::RECV_DISPLAY, 0, std::move(std::string(closing_conn)));
                conn_->shutdown();
                stop = true;
            }
            break;
            default:
            break;
        }
    }
}

template <class D>
typename internlmsgreceiver<D>::HANDLE_RES device<D>::HandleMsg(D data)
{
    return internlmsgreceiver<D>::HANDLE_OK;
}

#endif // DEVICE_H
