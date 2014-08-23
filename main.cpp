#include <iostream>
#include <thread>
#include <functional>
#include "internlmsg.h"
#include "internlmsgsender.h"
#include "netlistener.h"
#include "netconnectionhandler.h"
#include "workerdisplay.h"
#include "modulemanager.h"
#include "devicemanager.h"

using namespace std;
typedef internlmsg MESSAGE_TYPE;
typedef netlistener<MESSAGE_TYPE>& NETLISTENER_TYPE_REF;
typedef netlistener<MESSAGE_TYPE>* NETLISTENER_TYPE_PTR;

typedef modulemanager<MESSAGE_TYPE> MODULE_MANAGER_T;
typedef WorkerDisplay<MESSAGE_TYPE> WORKER_DISPLAY_T;
typedef devicemanager<MESSAGE_TYPE> DEVICE_MANAGER_T;
typedef netconnectionhandler<MESSAGE_TYPE> NETCONN_HANDLER_T;
typedef netlistener<MESSAGE_TYPE> NETLISTENER_T;
typedef std::reference_wrapper<DEVICE_MANAGER_T> DEVICE_MANAGER_REF;
typedef std::reference_wrapper<NETCONN_HANDLER_T> NETCONN_HANDLER_REF;
typedef std::reference_wrapper<NETLISTENER_T> NETLISTENER_REF;
typedef std::reference_wrapper<WORKER_DISPLAY_T> WORKER_DISPLAY_REF;

int main()
{
    MODULE_MANAGER_T module_mgr;
    WORKER_DISPLAY_T display;
    module_mgr.register_receiver(&display);
    DEVICE_MANAGER_T devMgr(&module_mgr);
    module_mgr.register_receiver(&devMgr);
    NETCONN_HANDLER_T netConnHandler(&module_mgr);
    module_mgr.register_receiver(&netConnHandler);
    NETLISTENER_T netlisten(&module_mgr);

    std::thread devmgr_thrd = std::thread(DEVICE_MANAGER_REF(devMgr));
    std::thread conn_thrd = std::thread(NETCONN_HANDLER_REF(netConnHandler));
    std::thread main_thrd = std::thread(NETLISTENER_REF(netlisten));
    std::thread disp_thrd = std::thread(WORKER_DISPLAY_REF(display));

    devmgr_thrd.join();
    conn_thrd.join();
    main_thrd.join();
    disp_thrd.join();
    return 0;
}
