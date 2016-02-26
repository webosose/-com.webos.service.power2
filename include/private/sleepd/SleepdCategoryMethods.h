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

#ifndef _SLEEPDCATEGORYMETHODS_H_
#define _SLEEPDCATEGORYMETHODS_H_

#include <glib.h>
#include <luna-service2/lunaservice.hpp>

class PowerManagerService;
class SleepdPowerCategory;
class SleepdShutdownCategory;
class SleepdTimeCategory;
class SleepdTimeoutCategory;

class SleepdCategoryMethods
{
    public:
        SleepdCategoryMethods(PowerManagerService &pmsRef, GMainLoop *mainLoop);
        ~SleepdCategoryMethods();
        bool init();
        PowerManagerService& getPmsHandle()
        {
            return mRefPowerManagerService;
        }

        SleepdCategoryMethods(const SleepdCategoryMethods &) = delete;
        SleepdCategoryMethods &operator=(const SleepdCategoryMethods &) = delete;

        static bool clientSubscriptionCancel(LSHandle *sh, LSMessage *msg, void *ctx);
        void registerSubscriptionCancelCallback();

    private:
        PowerManagerService    &mRefPowerManagerService;
        GMainLoop              *mLoopdata = nullptr;
        LS::Handle             mRefSleepdLsHandle;
        LS::Handle             mRefPowerdLsHandle;
        SleepdPowerCategory    *mPtrSleepdPowerCategory = nullptr;
        SleepdShutdownCategory *mPtrSleepdShutdownCategory = nullptr;
        SleepdTimeCategory     *mPtrSleepdTimeCategory = nullptr;
        SleepdTimeoutCategory  *mPtrSleepdTimeoutCategory = nullptr;
};

#endif /* _SLEEPDCATEGORYMETHODS_H_ */
