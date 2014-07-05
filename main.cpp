#include <iostream>
#include <thread>
#include <functional>
#include "netlistener.h"
#include "netconnectionhandler.h"
#include "message.h"
#include "workerdisplay.h"
#include "internlmsgsender.h"
#include "internlmsg.h"

using namespace std;
//typedef message<int> MESSAGE_TYPE;
typedef internlmsg MESSAGE_TYPE;
//typedef void (netconnectionhandler<MESSAGE_TYPE>::workerconnhandler::*MAINLOOP_TYPE)();
//typedef void (worker<MESSAGE_TYPE>::*MAINLOOP_TYPE)();
//typedef void (netlistener<MESSAGE_TYPE>::*MAINLOOP_TYPE_PTR)();
typedef netlistener<MESSAGE_TYPE>& NETLISTENER_TYPE_REF;
typedef netlistener<MESSAGE_TYPE>* NETLISTENER_TYPE_PTR;

template <class D>
class ref_wrapper
{
    D &_d;
public:
    ref_wrapper(D &d) : _d(d)
    {
    }
    void operator ()()
    {
        _d();
    }
};

int main()
{
    WorkerDisplay<MESSAGE_TYPE> display;
    netconnectionhandler<MESSAGE_TYPE> wrk(&display);
    netlistener<MESSAGE_TYPE> netlisten(&display);
    netlisten.add_worker(&wrk);
//    MAINLOOP_TYPE_PTR main_loop = &netlistener<MESSAGE_TYPE>::MainLoop;
//    std::thread main_thrd;
//    std::function<void(NETLISTENER_TYPE_REF)> start_func = &netlistener<MESSAGE_TYPE>::MainLoop;
//    std::reference_wrapper(wrk);

    std::thread conn_thrd = std::thread(ref_wrapper<worker<MESSAGE_TYPE>>(wrk));
    std::thread main_thrd = std::thread(ref_wrapper<netlistener<MESSAGE_TYPE>>(netlisten));
    std::thread disp_thrd = std::thread(ref_wrapper<WorkerDisplay<MESSAGE_TYPE>>(display));

    conn_thrd.join();
    main_thrd.join();
    disp_thrd.join();
    return 0;
}
