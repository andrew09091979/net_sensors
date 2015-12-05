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
#include "Log.h"

using namespace std;
typedef InternalMsg MESSAGE_TYPE;
typedef NetListener<MESSAGE_TYPE>& NETLISTENER_TYPE_REF;
typedef NetListener<MESSAGE_TYPE>* NETLISTENER_TYPE_PTR;

typedef WorkerDisplay<MESSAGE_TYPE> WORKER_DISPLAY_T;
typedef DeviceManager<MESSAGE_TYPE> DEVICE_MANAGER_T;
typedef NetConnectionHandler<MESSAGE_TYPE> NETCONN_HANDLER_T;
typedef InternalMsgRouter<MESSAGE_TYPE> INTERNAL_MSG_ROUTER_T;
typedef NetListener<MESSAGE_TYPE> NETLISTENER_T;
typedef Log<MESSAGE_TYPE> LOG_T;
typedef std::reference_wrapper<DEVICE_MANAGER_T> DEVICE_MANAGER_REF;
typedef std::reference_wrapper<NETCONN_HANDLER_T> NETCONN_HANDLER_REF;
typedef std::reference_wrapper<NETLISTENER_T> NETLISTENER_REF;
typedef std::reference_wrapper<WORKER_DISPLAY_T> WORKER_DISPLAY_REF;
typedef std::reference_wrapper<INTERNAL_MSG_ROUTER_T> INTERNAL_MSG_ROUTER_REF;
typedef std::reference_wrapper<LOG_T> LOG_REF;

int main()
{
    INTERNAL_MSG_ROUTER_T internalMsgRouter;
    WORKER_DISPLAY_T display(&internalMsgRouter);
    DEVICE_MANAGER_T deviceManager(&internalMsgRouter);
    NETCONN_HANDLER_T netConnHandler(&internalMsgRouter);
    NETLISTENER_T netlistener(&internalMsgRouter);
    LOG_T log(&internalMsgRouter, "/home/andrew/log.txt");

    std::thread internalMsgRouterThread = std::thread(INTERNAL_MSG_ROUTER_REF(internalMsgRouter));
    std::thread deviceManagerThread = std::thread(DEVICE_MANAGER_REF(deviceManager));
    std::thread netConnHandlerThread = std::thread(NETCONN_HANDLER_REF(netConnHandler));
    std::thread netlistenerThread = std::thread(NETLISTENER_REF(netlistener));
    std::thread displayThread = std::thread(WORKER_DISPLAY_REF(display));
    std::thread logThread = std::thread(LOG_REF(log));

    netlistenerThread.join();
    deviceManagerThread.join();
    netConnHandlerThread.join();
    displayThread.join();
    logThread.join();
    internalMsgRouterThread.join();
    return 0;
}
