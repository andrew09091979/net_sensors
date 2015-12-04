#ifndef Connectionhandler_H
#define Connectionhandler_H

#include <sys/types.h>
#include <sys/socket.h>
#include <string>

class Connectionhandler
{
    int sock;
    const int READ_TIMEOUT_SEC;
    const int MAX_ATTEMPTS_TO_READ;

public:
    Connectionhandler(int sock_);
    ~Connectionhandler();

    bool read_nbytes(char * const bfr, const ssize_t bytes_to_read);
    bool read_line(std::string &bfr);
    bool send_nbytes(const char * const bfr, const ssize_t bytes_to_send);
    int shutdownconn();
};

#endif // Connectionhandler_H
