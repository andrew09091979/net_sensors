#include <iostream>
#include <thread>
#include <functional>
#include "InternalMsg.h"
#include "InternalMsgSender.h"
#include "InternalMsgRouter.h"
#include "NetListener.h"
#include "NetConnectionHandler.h"
#include "WorkerDisplay.h"
#include "DeviceManager.h"

using namespace std;
typedef InternalMsg MESSAGE_TYPE;
typedef NetListener<MESSAGE_TYPE>& NETLISTENER_TYPE_REF;
typedef NetListener<MESSAGE_TYPE>* NETLISTENER_TYPE_PTR;

typedef WorkerDisplay<MESSAGE_TYPE> WORKER_DISPLAY_T;
typedef DeviceManager<MESSAGE_TYPE> DEVICE_MANAGER_T;
typedef NetConnectionHandler<MESSAGE_TYPE> NETCONN_HANDLER_T;
typedef InternalMsgRouter<MESSAGE_TYPE> INTERNAL_MSG_ROUTER_T;
typedef NetListener<MESSAGE_TYPE> NETLISTENER_T;
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
    NETLISTENER_T NetListener(&internal_msg_router);

    std::thread inl_msg_router_thrd = std::thread(INTERNAL_MSG_ROUTER_REF(internal_msg_router));
    std::thread devmgr_thrd = std::thread(Device_MANAGER_REF(devMgr));
    std::thread conn_thrd = std::thread(NETCONN_HANDLER_REF(netConnHandler));
    std::thread netlisten_thrd = std::thread(Netlistener_REF(NetListener));
    std::thread disp_thrd = std::thread(WORKER_DISPLAY_REF(display));

    netlisten_thrd.join();
    devmgr_thrd.join();
    conn_thrd.join();
    disp_thrd.join();
    inl_msg_router_thrd.join();
    return 0;
}
