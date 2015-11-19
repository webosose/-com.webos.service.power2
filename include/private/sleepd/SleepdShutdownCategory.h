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

#ifndef GIT_SRC_SLEEPD_SLEEPDSHUTDOWNCATEGORY_H_
#define GIT_SRC_SLEEPD_SLEEPDSHUTDOWNCATEGORY_H_

#include <luna-service2/lunaservice.hpp>

class ShutdownCategoryMethods;

class SleepdShutdownCategory
{
    public:
        SleepdShutdownCategory(ShutdownCategoryMethods &pmsRef, LS::Handle &refLsHandle);
        ~SleepdShutdownCategory() = default;
        bool init();

        SleepdShutdownCategory(const SleepdShutdownCategory &) = delete;
        SleepdShutdownCategory &operator=(const SleepdShutdownCategory &) = delete;

        bool initiate(LSMessage &message);
        bool shutdownApplicationsAck(LSMessage &message);
        bool shutdownApplicationsRegister(LSMessage &message);
        bool shutdownServicesRegister(LSMessage &message);
        bool shutdownServicesAck(LSMessage &message);
        bool machineOff(LSMessage &message);
        bool machineReboot(LSMessage &message);

    private:
        ShutdownCategoryMethods &mRefShutdownCategoryMethods;
        LS::Handle          &mRefLsHandle;
};

#endif /* GIT_SRC_SLEEPD_SLEEPDSHUTDOWNCATEGORY_H_ */
