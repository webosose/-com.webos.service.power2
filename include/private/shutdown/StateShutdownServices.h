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

#ifndef _SHUTDOWN_STATESHUTDOWNSERVICES_H_
#define _SHUTDOWN_STATESHUTDOWNSERVICES_H_

#include "ShutdownEvents.h"
#include "State.h"

namespace LS {
class Handle;
} /* namespace LS */

class ShutdownStateHandler;

class StateShutdownServices: public State
{
    public:
        StateShutdownServices(ShutdownStateHandler *ptrSSHandle,
                              LS::Handle &handle);
        ~StateShutdownServices() = default;
        ShutdownState getState() const;
        void handleEvent(ShutdownEvent event);

    private:
        void sendSignalShutdownServices();

    private:
        ShutdownStateHandler *mShutdownStateHandler = nullptr;
        LS::Handle &mLsHandle;
};

#endif /* _SHUTDOWN_STATESHUTDOWNSERVICES_H_ */

