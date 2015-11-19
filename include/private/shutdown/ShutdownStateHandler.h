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

#ifndef _SHUTDOWNSTATEHANDLER_H_
#define _SHUTDOWNSTATEHANDLER_H_

#include <map>
#include <memory>
#include <luna-service2/lunaservice.hpp>

#include "State.h"
#include "ShutdownEvents.h"

class ShutdownClientsMgr;

class ShutdownStateHandler
{
    public:
        ShutdownStateHandler();
        ~ShutdownStateHandler();
        bool init(LS::Handle &handle, ShutdownClientsMgr &appClientsMgr, ShutdownClientsMgr &servClientsMgr);

        inline void setState(ShutdownState state)
        {
            mNextState = state;
        }

        inline ShutdownState getState()
        {
            return mNextState;
        }

        inline void setMoveNext(bool next)
        {
            mMovenext = next;
        }

        inline bool moveToNextState()
        {
            return mMovenext;
        }

        void stateDispatch(ShutdownEvent event);
        void setRebootMode(bool state)
        {
            mRebootState = state;
        }
        bool getRebootState()
        {
            return mRebootState;
        }

    private:
        bool mMovenext = false;
        bool mRebootState = false;
        ShutdownState mNextState = kPowerShutdownNone;
        std::map<ShutdownState, std::unique_ptr<State>> mStates;
};

#endif /* _SHUTDOWNSTATEHANDLER_H_ */

