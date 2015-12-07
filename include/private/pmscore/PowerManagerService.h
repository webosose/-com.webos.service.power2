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

#ifndef _PMS_MGR_SERVICE_H_
#define _PMS_MGR_SERVICE_H_

#include <glib.h>
#include <luna-service2/lunaservice.hpp>
#include <pms-support.h>

class ShutdownCategoryMethods;
class WakelockClientsMgr;

const std::string serviceUri = "com.webos.service.power2";
const std::string displayUri = "com.palm.display";
const std::string sleepdUri = "com.palm.sleep";

class PowerManagerService: public LS::Handle
{
    public:
        PowerManagerService(GMainLoop *mainLoop);
        ~PowerManagerService();

        bool init();
        void shutDown();
        bool setKeepAwake(LSMessage &message);
        bool clearKeepAwake(LSMessage &message);
        bool registerClient(LSMessage &message);
        bool notifyAlarmExpiry(LSMessage &message);
        void addSubscription(LSMessage &message);
        void setSubscriptionCancelCallback();
        static bool clientSubscriptionCancel(LSHandle *sh, LSMessage *msg, void *ctx);
        static bool setSystemTimeCallback(LSHandle *sh, LSMessage *message, void *ctx);
        ShutdownCategoryMethods &getShutdownCategoryHandle();

#ifdef SLEEPD_BACKWARD_COMPATIBILITY
        void setActivityStart(bool state)
        {
            mActivityStart = state;
        }
        GMainLoop *getGMainLoop()
        {
            return mLoopdata;
        }

        bool clientCancelByName(LSMessage &message);
        LS::Handle &getSleepdLsHandle();
        LS::Handle &getPowerdLsHandle();
        LS::Handle &getDisplayLsHandle();
        bool isClientRegistered(const std::string &clientId);
        bool isSuspendedState();
        static bool cancelSubscription(LSHandle *sh, LSMessage *msg, void *ctx);
        static bool dimModeDisableCallback(LSHandle *sh, LSMessage *message, void *ctx);
        WakelockClientsMgr *getWakeLockManageRef()
        {
            return mWakelocksMgr;
        }
        bool getIsPowerdRegistered()
        {
            return mIsPowerdRegistered;
        }
#endif
        bool setAwake(int timeout, LS::Message &request, std::string clientId, std::string sender, bool isTimeout = false);
        bool clearAlarm(const std::string &key);

    private:
        bool initPmSupportInterface();
        bool configInit(void);
        bool checkSystemClock(void);
        bool exit(LSMessage &message);
        bool setAlarm(const std::string &key, int timeout);
        void deregisterClient(LSMessage *msg, const std::string &clientId);

    private:
        GMainLoop *mLoopdata = nullptr;
        pms_support_update_callbacks mSupportCallback;
        ShutdownCategoryMethods *mShutdownCategoryHandle = nullptr;
        WakelockClientsMgr *mWakelocksMgr = nullptr;
        long mDifferenceBetweenRtcAndSystemClock = 0;
#ifdef SLEEPD_BACKWARD_COMPATIBILITY
        bool mActivityStart = false;
        bool mIsPowerdRegistered = false;
        LS::Handle mSleepdHandle;
        LS::Handle mDisplayHandle;
        LS::Handle mPowerdHandle;
#endif
};

#endif //_PMS_MGR_SERVICE_H_

