#include <iostream>
#include <thread>
#include <functional>
#include "netlistener.h"
#include "netconnectionhandler.h"
#include "message.h"

using namespace std;
typedef message<int> MESSAGE_TYPE;
//typedef void (netconnectionhandler<MESSAGE_TYPE>::workerconnhandler::*MAINLOOP_TYPE)();
//typedef void (worker<MESSAGE_TYPE>::*MAINLOOP_TYPE)();
typedef void (netlistener<MESSAGE_TYPE>::*MAINLOOP_TYPE_PTR)();
typedef netlistener<MESSAGE_TYPE>& NETLISTENER_TYPE_REF;

void starter(NETLISTENER_TYPE_REF cls, MAINLOOP_TYPE_PTR mth)
{
    (cls.*mth)();
}

int main()
{
    netconnectionhandler<MESSAGE_TYPE> wrk;
    netlistener<MESSAGE_TYPE> netlisten(wrk.GetWorker());
//    MAINLOOP_TYPE_PTR main_loop = &netlistener<MESSAGE_TYPE>::MainLoop;
    std::function<void(NETLISTENER_TYPE_REF)> start_func = &netlistener<MESSAGE_TYPE>::MainLoop;
    std::thread main_thrd(start_func, netlisten);
    main_thrd.join();
    return 0;
}

