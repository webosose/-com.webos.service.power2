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

#include "StateShutdownIdle.h"
#include "ShutdownStateHandler.h"

StateShutdownIdle::StateShutdownIdle(ShutdownStateHandler *shutdownHandle) :
    mShutdownStateHandler(shutdownHandle)
{
}

ShutdownState StateShutdownIdle::getState() const
{
    return kPowerShutdownNone;
}

void StateShutdownIdle::handleEvent(ShutdownEvent event)
{
    switch (event) {
        case ShutdownEventInit:
        case ShutdownEventAllAck:
        case ShutdownEventTimeout:
            mShutdownStateHandler->setState(kPowerShutdownApps);
            mShutdownStateHandler->setMoveNext(true);
            break;

        default:
            break;
    }
}

