#ifndef ARRAYWRAPPER_H
#define ARRAYWRAPPER_H

#include <memory.h>

template <class D>
class arraywrapper
{
    D * arr;
    unsigned size;

public:
    arraywrapper() : arr(nullptr), size(0)
    {
    }

    arraywrapper(unsigned size_) : size(size_)
    {
        arr = new D[size];
    }

    arraywrapper(arraywrapper&& oth)
    {
        arr = oth.arr;
        oth.arr = nullptr;
        size = oth.size;
    }

    ~arraywrapper()
    {
        delete [] arr;
    }

    void operator =(arraywrapper&& oth)
    {
        arr = oth.arr;
        oth.arr = nullptr;
        size = oth.size;
    }

    const D* at(unsigned pos) const
    {
        if ((pos < size) && (arr != nullptr))
            return &arr[pos];
        else
            return nullptr;
    }

    D* at(unsigned pos)
    {
        if ((pos < size) && (arr != nullptr))
            return &arr[pos];
        else
            return nullptr;
    }

    D& operator[](unsigned pos)
    {
        if ((pos < size) && (arr != nullptr))
            return arr[pos];
        else
            return nullptr;
    }

    unsigned get_size() const
    {
        return size;
    }

    void zero_mem()
    {
        if (arr != nullptr)
            memset(arr, 0, size);
    }
};
#endif // ARRAYWRAPPER_H
