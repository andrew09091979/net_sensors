#include "connectionhandler.h"


connectionhandler::connectionhandler(int sock_) : sock(sock_), READ_TIMEOUT_SEC(2), MAX_ATTEMPTS_TO_READ(5)
{
}

connectionhandler::~connectionhandler()
{

}

bool connectionhandler::read_nbytes(char * const bfr, const ssize_t bytes_to_read)
{
    bool res = false;
    int sel_res = 0;
    int attempts = 0;
    char *bfr_ptr = bfr;
    ssize_t remain_bytes = bytes_to_read;
    ssize_t received = 0;

    fd_set readmask;
    fd_set sockonly;
    FD_ZERO(&sockonly);
    FD_SET(sock, &sockonly);
    readmask = sockonly;

    timeval tv;
    tv.tv_sec = READ_TIMEOUT_SEC;
    tv.tv_usec = 0;

    while ((remain_bytes > 0) && (attempts < MAX_ATTEMPTS_TO_READ))
    {
        sel_res = select(sock + 1, &readmask, 0, 0, &tv);

        if (sel_res < 0)
        {
            break;//error, stop reading
        }
        else if (sel_res == 0)
        {
            ++attempts;//timeout
        }
        else if (sel_res > 0)
        {
            if (FD_ISSET(sock, &readmask))
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
            else
            {
                break;//error, stop reading
            }
        }

        readmask = sockonly;
        tv.tv_sec = READ_TIMEOUT_SEC;
        tv.tv_usec = 0;
    }

    if (remain_bytes <= 0)
        res = true;

    return res;
}

bool connectionhandler::send_nbytes(const char * const bfr, const ssize_t bytes_to_send)
{
    bool res = false;
    const char *bfr_ptr = bfr;
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
