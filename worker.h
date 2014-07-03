#ifndef WORKER_H
#define WORKER_H

template <class D>
class worker
{
protected:
    enum HANDLE_RES
    {
        HANDLE_OK,
        HANDLE_FAILED
    };
public:
    virtual void EnqueMsg(const D &data) = 0;
    virtual worker<D> &operator <<(const D &data) = 0;
    virtual void MainLoop() = 0;
    virtual HANDLE_RES HandleMsg(const D &data) = 0;
    virtual ~worker(){}
};

#endif // WORKER_H
