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

#ifndef _LUNASHUTDOWN_H_
#define _LUNASHUTDOWN_H_

#include <time.h>
#include <stdint.h>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <luna-service2/lunaservice.hpp>

class PowerManagerService;
class ShutdownClientListManager;
class ShutdownClientsMgr;

class ShutdownCategoryMethods
{
    public:
        ShutdownCategoryMethods(LS::Handle &refLsHandle);
        ~ShutdownCategoryMethods();
        bool init();

        // LUNA API's
        bool initiate(LSMessage &message);
        bool machineOff(LSMessage &message);
        bool machineReboot(LSMessage &message);
        bool shutdownApplicationsAck(LSMessage &message);
        bool shutdownApplicationsRegister(LSMessage &message);
        bool shutdownServicesRegister(LSMessage &message);
        bool shutdownServicesAck(LSMessage &message);

        void run(LSMessage &message);
        void deregisterAppsServicesClient(const std::string &clientId);

    private:
        void addSubscription(LSMessage &message, const std::string &key);
        void sendInitiateReply(LSMessage &message);

    private:
        LS::Handle &mRefLsHandle;
        time_t mStartTime;
        std::mutex mShutdownThreadMtx;
        std::condition_variable mShutdownThreadCV;
        ShutdownClientsMgr *mAppsMgr = nullptr;
        ShutdownClientsMgr *mServicesMgr = nullptr;
};

#endif /* _LUNASHUTDOWN_H_ */

