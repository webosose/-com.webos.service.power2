// @@@LICENSE
//
//      Copyright (c) 2015-2016 LG Electronics, Inc.
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

class PowerManagerService: public LS::Handle
{
    public:
        PowerManagerService(GMainLoop *mainLoop);
        ~PowerManagerService();

        bool init();
        bool initPmSupportInterface();
        bool setKeepAwake(LSMessage &message);
        bool clearKeepAwake(LSMessage &message);
        bool registerClient(LSMessage &message);
        bool notifyAlarmExpiry(LSMessage &message);
        bool clientCancelByName(LSMessage &message);
        bool isClientRegistered(const std::string &clientId);
        bool isSuspendedState();
        bool checkSystemClock(void);
        bool setAwake(int timeout, LS::Message &request, std::string clientId, std::string sender, bool isTimeout = false);
        bool clearAlarm(const std::string &key);
        void addSubscription(LSMessage &message);
        void registerSubscriptionCancelCallback();
        void deregisterClient(LSMessage *msg, const std::string &clientId);
        void shutDown();
        static bool clientSubscriptionCancel(LSHandle *sh, LSMessage *msg, void *ctx);
        static bool setSystemTimeCallback(LSHandle *sh, LSMessage *message, void *ctx);
        ShutdownCategoryMethods &getShutdownCategoryHandle();
        WakelockClientsMgr *getWakeLockManageRef()
        {
            return mWakelocksMgr;
        }

    private:
        bool configInit(void);
        bool exit(LSMessage &message);
        bool setAlarm(const std::string &key, int timeout);

    public:
        LSHandle *mSleepdHandle;

    private:
        GMainLoop *mLoopdata = nullptr;
        pms_support_update_callbacks mSupportCallback;
        ShutdownCategoryMethods *mShutdownCategoryHandle = nullptr;
        WakelockClientsMgr *mWakelocksMgr = nullptr;
        long mDifferenceBetweenRtcAndSystemClock = 0;
};

#endif //_PMS_MGR_SERVICE_H_

