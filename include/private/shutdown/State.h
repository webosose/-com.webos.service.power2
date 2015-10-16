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

#ifndef _STATE_H_
#define _STATE_H_

#include "ShutdownEvents.h"
#include "PmsLogging.h"

typedef enum ShutdownStates {
    kPowerShutdownNone,
    kPowerShutdownApps,
    kPowerShutdownAppsProcess,
    kPowerShutdownServices,
    kPowerShutdownServicesProcess,
    kPowerShutdownAction,
    kPowerShutdownLast
} ShutdownState;

class State
{
    public:
        State() = default;
        virtual ShutdownState getState() const = 0;
        virtual void handleEvent(ShutdownEvent event) = 0;
        virtual ~State() = default;
};

#endif /* _STATE_H_ */

