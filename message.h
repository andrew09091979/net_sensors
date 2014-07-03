#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>

template <class D>
class message
{
    D val;
    const std::string msg;
public:
    message(D v, const std::string &msg_);
    operator int() const;
    operator std::string() const;
};

template<class D>
message<D>::message(D v, const std::string& msg_) : val(v), msg(msg_)
{
}

template<class D>
message<D>::operator int() const
{
    return int(val);
}

template<class D>
message<D>::operator std::string() const
{
    return msg;
}
#endif // MESSAGE_H
