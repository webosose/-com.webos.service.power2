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

    private:
        bool initPmSupportInterface();
        bool exit(LSMessage &message);
        bool setAlarm(const std::string &key, int timeout);
        bool clearAlarm(const std::string &key);
        void deregisterClient(const std::string &clientId);

    private:
        GMainLoop *mLoopdata = nullptr;
        pms_support_update_callbacks mSupportCallback;
        ShutdownCategoryMethods *mShutdownCategoryHandle = nullptr;
        WakelockClientsMgr *mWakelocksMgr = nullptr;
};

#endif //_PMS_MGR_SERVICE_H_

