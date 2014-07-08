#include <iostream>
#include <thread>
#include <functional>
#include "internlmsg.h"
#include "internlmsgsender.h"
#include "netlistener.h"
#include "netconnectionhandler.h"
#include "workerdisplay.h"
#include "modulemanager.h"

using namespace std;
typedef internlmsg MESSAGE_TYPE;
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
    modulemanager<MESSAGE_TYPE> module_mgr;
    WorkerDisplay<MESSAGE_TYPE> display;
    module_mgr.register_receiver(&display);
    netconnectionhandler<MESSAGE_TYPE> wrk(&module_mgr);
    module_mgr.register_receiver(&wrk);
    netlistener<MESSAGE_TYPE> netlisten(&module_mgr);
//    MAINLOOP_TYPE_PTR main_loop = &netlistener<MESSAGE_TYPE>::MainLoop;
//    std::thread main_thrd;
//    std::function<void(NETLISTENER_TYPE_REF)> start_func = &netlistener<MESSAGE_TYPE>::MainLoop;
//    std::reference_wrapper(wrk);

    std::thread conn_thrd = std::thread(ref_wrapper<internlmsgreceiver<MESSAGE_TYPE>>(wrk));
    std::thread main_thrd = std::thread(ref_wrapper<netlistener<MESSAGE_TYPE>>(netlisten));
    std::thread disp_thrd = std::thread(ref_wrapper<WorkerDisplay<MESSAGE_TYPE>>(display));

    conn_thrd.join();
    main_thrd.join();
    disp_thrd.join();
    return 0;
}
