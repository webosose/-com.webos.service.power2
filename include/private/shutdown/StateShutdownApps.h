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

#ifndef _SHUTDOWN_STATESHUTDOWNAPPS_H_
#define _SHUTDOWN_STATESHUTDOWNAPPS_H_

#include "State.h"

namespace LS {
class Handle;
}

class ShutdownStateHandler;

class StateShutdownApps: public State
{
    public:
        StateShutdownApps(ShutdownStateHandler *ptrSSHandle, LS::Handle &handle);
        ~StateShutdownApps() = default;
        ShutdownState getState() const;
        void handleEvent(ShutdownEvent event);

    private:
        void sendSignalShutdownApplications();

    private:
        ShutdownStateHandler *mShutdownStateHandler = nullptr;
        LS::Handle &mLsHandle;
};

#endif /* _SHUTDOWN_STATESHUTDOWNAPPS_H_ */

