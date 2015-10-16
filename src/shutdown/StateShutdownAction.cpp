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

#include <string>
#include <luna-service2/lunaservice.hpp>
#include "StateShutdownAction.h"
#include "ShutdownStateHandler.h"
#include "NyxUtil.h"

StateShutdownAction::StateShutdownAction(ShutdownStateHandler *shutdownHandle) :
    mShutdownStateHandler(shutdownHandle)
{
}

ShutdownState StateShutdownAction::getState() const
{
    return kPowerShutdownAction;
}

void StateShutdownAction::handleEvent(ShutdownEvent event)
{
    NyxUtil::getInstance().setRtcAlarm();
    PMSLOG_DEBUG("entered shutdown action state");
}

