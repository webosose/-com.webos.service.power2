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
        SleepdCategoryMethods(PowerManagerService &pmsRef, LS::Handle &sleepdLsHandle, LS::Handle &powerdLsHandle);
        ~SleepdCategoryMethods();
        bool init();

        SleepdCategoryMethods(const SleepdCategoryMethods &) = delete;
        SleepdCategoryMethods &operator=(const SleepdCategoryMethods &) = delete;

    private:
        PowerManagerService    &mRefPowerManagerService;
        LS::Handle             &mRefSleepdLsHandle;
        LS::Handle             &mRefPowerdLsHandle;
        SleepdPowerCategory    *mPtrSleepdPowerCategory = nullptr;
        SleepdShutdownCategory *mPtrSleepdShutdownCategory = nullptr;
        SleepdTimeCategory     *mPtrSleepdTimeCategory = nullptr;
        SleepdTimeoutCategory  *mPtrSleepdTimeoutCategory = nullptr;
};

#endif /* _SLEEPDCATEGORYMETHODS_H_ */
