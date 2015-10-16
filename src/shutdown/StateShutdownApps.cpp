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

    try {
        mLsHandle.sendSignal(signalURI, "{}");
        PMSLOG_DEBUG("shutdownApplications signal is sent");
    } catch (LS::Error &error) {
        PMSLOG_ERROR(MSGID_SHUTDOWN_APPS_SIG_FAIL, 0, "erro while sendingshutdownApplications signal");
    }
}

