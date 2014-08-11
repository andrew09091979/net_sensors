#ifndef CONNECTIONHANDLER_H
#define CONNECTIONHANDLER_H

#include <sys/types.h>
#include <sys/socket.h>

class connectionhandler
{
    int sock;
    const int READ_TIMEOUT_SEC;
    const int MAX_ATTEMPTS_TO_READ;

public:
    connectionhandler(int sock_);
    ~connectionhandler();

    bool read_nbytes(char * const bfr, const ssize_t bytes_to_read);
    bool send_nbytes(const char * const bfr, const ssize_t bytes_to_send);
    int shutdownconn();
};

#endif // CONNECTIONHANDLER_H
