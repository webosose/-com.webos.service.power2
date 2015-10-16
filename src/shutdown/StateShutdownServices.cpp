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

#include "StateShutdownServices.h"
#include "ShutdownStateHandler.h"
#include "PMSCommon.h"

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

    try {
        mLsHandle.sendSignal(signalURI, "{}");
        PMSLOG_DEBUG("shutdownServices signal is sent");
    } catch (LS::Error &error) {
        PMSLOG_ERROR(MSGID_SHUTDOWN_SRVC_SIG_FAIL, 0, "erro while sendingshutdownServices signal");
    }
}

