// @@@LICENSE
//
//      Copyright (c) 2015 LG Electronics, Inc.
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

#include "StateShutdownApps.h"
#include "ShutdownStateHandler.h"
#include "PMSCommon.h"

#ifdef SLEEPD_BACKWARD_COMPATIBILITY
extern LSHandle *gSleepdLsHandle;
#endif

StateShutdownApps::StateShutdownApps(ShutdownStateHandler *ptrSSHandle,
                                     LS::Handle &handle) :
    mShutdownStateHandler(ptrSSHandle), mLsHandle(handle)
{
}

ShutdownState StateShutdownApps::getState() const
{
    return kPowerShutdownApps;
}

void StateShutdownApps::handleEvent(ShutdownEvent event)
{
    mShutdownStateHandler->setMoveNext(true);

    //send shutdownApplications signal
    sendSignalShutdownApplications();

    //move to next state
    mShutdownStateHandler->setState(kPowerShutdownAppsProcess);
}

void StateShutdownApps::sendSignalShutdownApplications()
{
    pbnjson::JValue signalObj = pbnjson::Object();
    const char *signalURI = PMS_SHUDOWN_URI SIGNAL_SHUTDOWN_APPLICATIONS;

#ifdef SLEEPD_BACKWARD_COMPATIBILITY

    if (gSleepdLsHandle) {
        LSError lserror;
        LSErrorInit(&lserror);

        PMSLOG_INFO(MSGID_SHUTDOWN_DEBUG, 0, "sending sleepd shutdownApplications signal");

        bool retVal = LSSignalSend(gSleepdLsHandle,
                                   "luna://com.palm.sleep/shutdown/shutdownApplications",
                                   "{}", &lserror);

        if (!retVal) {
            PMSLOG_ERROR(MSGID_SHUTDOWN_APPS_SIG_FAIL, 0,
                         "Could not send sleepd shutdownApplications signal");
            LSErrorPrint(&lserror, stderr);
            LSErrorFree(&lserror);
        }
    }

#endif

    try {
        mLsHandle.sendSignal(signalURI, "{}");
        PMSLOG_DEBUG("shutdownApplications signal is sent");
    } catch (LS::Error &error) {
        PMSLOG_ERROR(MSGID_SHUTDOWN_APPS_SIG_FAIL, 0, "error while sendingshutdownApplications signal: %s", error.what());
    }
}

