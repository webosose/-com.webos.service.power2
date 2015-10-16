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

#include "ShutdownStateHandler.h"
#include "StateShutdownIdle.h"
#include "StateShutdownApps.h"
#include "StateShutdownAppsProcess.h"
#include "StateShutdownServices.h"
#include "StateShutdownServicesProcess.h"
#include "StateShutdownAction.h"
#include "PMSCommon.h"

ShutdownStateHandler::ShutdownStateHandler()
{
    mNextState = kPowerShutdownNone;
    mMovenext = false;
}

bool ShutdownStateHandler::init(LS::Handle &handle,
                                ShutdownClientsMgr &appClientsMgr, ShutdownClientsMgr &servClientsMgr)
{
    mStates[kPowerShutdownNone] = std::unique_ptr<State>(new StateShutdownIdle(this));
    mStates[kPowerShutdownApps] = std::unique_ptr<State>(new StateShutdownApps(this, handle));
    mStates[kPowerShutdownAppsProcess] = std::unique_ptr<State>(new StateShutdownAppsProcess(this, appClientsMgr));
    mStates[kPowerShutdownServices] = std::unique_ptr<State>(new StateShutdownServices(this, handle));
    mStates[kPowerShutdownServicesProcess] = std::unique_ptr<State>(new StateShutdownServicesProcess(this, servClientsMgr));
    mStates[kPowerShutdownAction] = std::unique_ptr<State>(new StateShutdownAction(this));

    for (const auto &state : mStates) {
        if (!state.second) {
            return false;
        }
    }

    return true;
}

ShutdownStateHandler::~ShutdownStateHandler()
{
    mStates.clear();
}

void ShutdownStateHandler::stateDispatch(ShutdownEvent event)
{
    setMoveNext(false);
    std::unique_ptr<State> &state = mStates[mNextState];
    state->handleEvent(event);
}

