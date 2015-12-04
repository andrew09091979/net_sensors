#include <iostream>
#include <thread>
#include <functional>
#include "Internalmsg.h"
#include "Internalmsgsender.h"
#include "Internalmsgrouter.h"
#include "Netlistener.h"
#include "Netconnectionhandler.h"
#include "workerdisplay.h"
#include "Devicemanager.h"

using namespace std;
typedef Internalmsg MESSAGE_TYPE;
typedef Netlistener<MESSAGE_TYPE>& NETLISTENER_TYPE_REF;
typedef Netlistener<MESSAGE_TYPE>* NETLISTENER_TYPE_PTR;

typedef WorkerDisplay<MESSAGE_TYPE> WORKER_DISPLAY_T;
typedef Devicemanager<MESSAGE_TYPE> DEVICE_MANAGER_T;
typedef Netconnectionhandler<MESSAGE_TYPE> NETCONN_HANDLER_T;
typedef Internalmsgrouter<MESSAGE_TYPE> INTERNAL_MSG_ROUTER_T;
typedef Netlistener<MESSAGE_TYPE> NETLISTENER_T;
typedef std::reference_wrapper<DEVICE_MANAGER_T> Device_MANAGER_REF;
typedef std::reference_wrapper<NETCONN_HANDLER_T> NETCONN_HANDLER_REF;
typedef std::reference_wrapper<NETLISTENER_T> Netlistener_REF;
typedef std::reference_wrapper<WORKER_DISPLAY_T> WORKER_DISPLAY_REF;
typedef std::reference_wrapper<INTERNAL_MSG_ROUTER_T> INTERNAL_MSG_ROUTER_REF;

int main()
{
    INTERNAL_MSG_ROUTER_T internal_msg_router;
    WORKER_DISPLAY_T display(&internal_msg_router);
    DEVICE_MANAGER_T devMgr(&internal_msg_router);
    NETCONN_HANDLER_T netConnHandler(&internal_msg_router);
    NETLISTENER_T netlisten(&internal_msg_router);

    std::thread inl_msg_router_thrd = std::thread(INTERNAL_MSG_ROUTER_REF(internal_msg_router));
    std::thread devmgr_thrd = std::thread(Device_MANAGER_REF(devMgr));
    std::thread conn_thrd = std::thread(NETCONN_HANDLER_REF(netConnHandler));
    std::thread netlisten_thrd = std::thread(Netlistener_REF(netlisten));
    std::thread disp_thrd = std::thread(WORKER_DISPLAY_REF(display));

    netlisten_thrd.join();
    devmgr_thrd.join();
    conn_thrd.join();
    disp_thrd.join();
    inl_msg_router_thrd.join();
    return 0;
}
