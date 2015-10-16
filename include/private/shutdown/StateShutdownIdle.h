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

#ifndef _SHUTDOWN_STATESHUTDOWNIDLE_H_
#define _SHUTDOWN_STATESHUTDOWNIDLE_H_

#include "State.h"

class ShutdownStateHandler;

class StateShutdownIdle: public State
{
    public:
        StateShutdownIdle(ShutdownStateHandler *ptrSSHandle);
        ~StateShutdownIdle() = default;
        ShutdownState getState() const;
        void handleEvent(ShutdownEvent event);

    private:
        ShutdownStateHandler *mShutdownStateHandler = nullptr;
};

#endif /* _SHUTDOWN_STATESHUTDOWNIDLE_H_ */

