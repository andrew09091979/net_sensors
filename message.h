#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>

template <class D>
class message
{
    D val;
    std::string msg;
public:
    message(D v, const std::string &msg_);
    message(const message<D> &msg_);
    message& operator=(const message& msg_);
    operator int() const;
    operator std::string() const;
};

template<class D>
message<D>::message(D v, const std::string& msg_) : val(v), msg(msg_)
{
}
template<class D>
message<D>::message(const message<D> &msg_) : val(msg_.val), msg(msg_.msg)
{
}

template<class D>
message<D>& message<D>::operator=(const message& msg_)
{
    val = msg_.val;
    msg = msg_.msg;
    return *this;
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
