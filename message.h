#ifndef MESSAGE_H
#define MESSAGE_H

template <class D>
class message
{
    D val;
public:
    message(D v);
    operator int() const;
};

template<class D>
message<D>::message(D v) : val(v)
{
}

template<class D>
message<D>::operator int() const
{
    return int(val);
}

#endif // MESSAGE_H
