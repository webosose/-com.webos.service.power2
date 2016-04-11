// @@@LICENSE
//
//      Copyright (c) 2015-2016 LG Electronics, Inc.
//
// Confidential computer software. Valid license from LG required for
// possession, use or copying. Consistent with FAR 12.211 and 12.212,
// Commercial Computer Software, Computer Software Documentation, and
// Technical Data for Commercial Items are licensed to the U.S. Government
// under vendor's standard commercial license.
//
// LICENSE@@@

#include <luna-service2/lunaservice.hpp>
#include <pbnjson.hpp>

#include "StateShutdownServices.h"
#include "ShutdownStateHandler.h"
#include "PMSCommon.h"

#ifdef SLEEPD_BACKWARD_COMPATIBILITY
extern LSHandle *gSleepdLsHandle;
#endif

StateShutdownServices::StateShutdownServices(
    ShutdownStateHandler *shutdownHandle, LS::Handle &handle) :
    mShutdownStateHandler(shutdownHandle), mLsHandle(handle)
{
}

ShutdownState StateShutdownServices::getState() const
{
    return kPowerShutdownServices;
}

void StateShutdownServices::handleEvent(ShutdownEvent event)
{
    mShutdownStateHandler->setMoveNext(true);

    //Send shutdownServices signal
    sendSignalShutdownServices();

    mShutdownStateHandler->setState(kPowerShutdownServicesProcess);
}

void StateShutdownServices::sendSignalShutdownServices()
{
    const char *signalURI = PMS_SHUDOWN_URI SIGNAL_SHUTDOWN_SERVICES;

#ifdef SLEEPD_BACKWARD_COMPATIBILITY

    if (gSleepdLsHandle) {
        LSError lserror;
        LSErrorInit(&lserror);

        PMSLOG_INFO(MSGID_SHUTDOWN_DEBUG, 0, "sending sleepd shutdownServices signal");

        bool retVal = LSSignalSend(gSleepdLsHandle,
                                   "luna://com.palm.sleep/shutdown/shutdownServices",
                                   "{}", &lserror);

        if (!retVal) {
            PMSLOG_ERROR(MSGID_SHUTDOWN_APPS_SIG_FAIL, 0,
                         "Could not send sleepd shutdownServices signal");
            LSErrorPrint(&lserror, stderr);
            LSErrorFree(&lserror);
        }
    }

#endif

    try {
        mLsHandle.sendSignal(signalURI, "{}");
        PMSLOG_DEBUG("shutdownServices signal is sent");
    } catch (LS::Error &error) {
        PMSLOG_ERROR(MSGID_SHUTDOWN_SRVC_SIG_FAIL, 0, "erro while sendingshutdownServices signal: %s", error.what());
    }
}

