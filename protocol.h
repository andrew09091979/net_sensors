#ifndef PROTOCOL_H
#define PROTOCOL_H
#include <arraywrapper.h>

template <class D>
class protocol
{
public:
    virtual arraywrapper<D> wrapMessage(const arraywrapper<D> &msg) = 0;
    virtual ~protocol(){}
};

#endif // PROTOCOL_H
