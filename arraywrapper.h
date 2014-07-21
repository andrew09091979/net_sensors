#ifndef ARRAYWRAPPER_H
#define ARRAYWRAPPER_H

template <class D>
class arraywrapper
{
    D * arr;
    unsigned size;
public:
    arraywrapper(unsigned size_) : size(size_)
    {
        arr = new D[size];
    }

    arraywrapper(arraywrapper&& oth)
    {
        arr = oth.arr;
        oth.arr = nullptr;
    }

    ~arraywrapper()
    {
        delete [] arr;
    }

    D* at(unsigned pos)
    {
        if (pos < size)
            return &arr[pos];
        else
            return nullptr;
    }

    D& operator[](unsigned pos)
    {
        if (pos < size)
            return arr[pos];
        else
            return nullptr;
    }

    unsigned get_size() const
    {
        return size;
    }
};
#endif // ARRAYWRAPPER_H
