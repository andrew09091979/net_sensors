#include <sys/ioctl.h>
#include <cstring>
#include "Connectionhandler.h"

Connectionhandler::Connectionhandler(int sock_) : sock(sock_), READ_TIMEOUT_SEC(2), MAX_ATTEMPTS_TO_READ(5)
{
}

Connectionhandler::~Connectionhandler()
{

}

bool Connectionhandler::read_nbytes(char * const bfr, const ssize_t bytes_to_read)
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

bool Connectionhandler::read_line(std::string &bfr)
{
    bool res = false;
    bool end_marker_found = false;
    char end_marker[] = "\r\n";
    int curr_compare = 0;
    int bytes_available = 0;
    int sel_res = 0;
    int attempts = 0;
    char *bfr_for_recv = nullptr;
    int size_of_bfr = 0;
    char *bfr_ptr = nullptr;

    ssize_t received = 0;

    fd_set readmask;
    fd_set sockonly;
    FD_ZERO(&sockonly);
    FD_SET(sock, &sockonly);
    readmask = sockonly;

    timeval tv;
    tv.tv_sec = READ_TIMEOUT_SEC;
    tv.tv_usec = 0;

    while ((attempts < MAX_ATTEMPTS_TO_READ) && !end_marker_found)
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
                ioctl(sock, FIONREAD, &bytes_available);
                bfr_ptr = new char[bytes_available + size_of_bfr];
                memset(bfr_ptr, 0, bytes_available + size_of_bfr);

                if (bfr_for_recv != nullptr)
                {
                    memcpy(bfr_ptr, bfr_for_recv, size_of_bfr);
                    delete [] bfr_for_recv;
                }
                bfr_for_recv = bfr_ptr;
                size_of_bfr += bytes_available;

                for (int cnt = 0; cnt < bytes_available; ++cnt)
                {
                    received = recv(sock, bfr_ptr, 1, 0);

                    if (*bfr_ptr == end_marker[curr_compare])
                    {
                        ++curr_compare;
                    }
                    else
                    {
                        curr_compare = 0;
                    }
                    ++bfr_ptr;

                    if (curr_compare == strlen(end_marker))
                    {
                        cnt = bytes_available; //end marker found - stop receiving
                        end_marker_found = true;
                        res = true;
                        bfr_for_recv[size_of_bfr-1] = 0x0;
                        bfr_for_recv[size_of_bfr-2] = 0x0;
                        bfr = std::string(bfr_for_recv);
                    }
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

    if (attempts >= MAX_ATTEMPTS_TO_READ)
        res = false;

    return res;
}

bool Connectionhandler::send_nbytes(const char * const bfr, const ssize_t bytes_to_send)
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

int Connectionhandler::shutdownconn()
{
    return shutdown(sock, SHUT_RDWR);
}
