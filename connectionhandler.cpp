#include "connectionhandler.h"


connectionhandler::connectionhandler(int sock_) : sock(sock_)
{

}

connectionhandler::~connectionhandler()
{

}

bool connectionhandler::read_nbytes(char * const bfr, const ssize_t bytes_to_read)
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

bool connectionhandler::send_nbytes(char * const bfr, const ssize_t bytes_to_send)
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

int connectionhandler::shutdownconn()
{
    return shutdown(sock, SHUT_RDWR);
}
