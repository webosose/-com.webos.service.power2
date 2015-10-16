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

#include "StateShutdownServicesProcess.h"
#include "ShutdownStateHandler.h"
#include "ShutdownClientsMgr.h"

StateShutdownServicesProcess::StateShutdownServicesProcess(
    ShutdownStateHandler *shutdownHandle, ShutdownClientsMgr &clientMgr) :
    mShutdownSateHanlder(shutdownHandle), mServiceClientsMgr(clientMgr)
{
}

ShutdownState StateShutdownServicesProcess::getState() const
{
    return kPowerShutdownServicesProcess;
}

void StateShutdownServicesProcess::handleEvent(ShutdownEvent event)
{
    bool canMoveNext = (event == ShutdownEventTimeout);
    canMoveNext =
        (canMoveNext || mServiceClientsMgr.isAllClientsAcknowledged());

    if (canMoveNext) {
        mShutdownSateHanlder->setState(kPowerShutdownAction);
        mShutdownSateHanlder->setMoveNext(true);
    }
}

