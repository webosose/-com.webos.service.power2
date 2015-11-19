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

#include "SleepdCategoryMethods.h"
#include "SleepdTimeoutCategory.h"
#include "SleepdTimeCategory.h"
#include "SleepdShutdownCategory.h"
#include "SleepdPowerCategory.h"
#include "PowerManagerService.h"
#include "ShutdownCategoryMethods.h"
#include "PmsLogging.h"

SleepdCategoryMethods::SleepdCategoryMethods(PowerManagerService &pmsRef, LS::Handle &sleepdLsHandle) :
    mRefPowerManagerService(pmsRef),
    mRefSleepdLsHandle(sleepdLsHandle)
{
}

SleepdCategoryMethods::~SleepdCategoryMethods()
{
    delete mPtrSleepdTimeoutCategory;
    delete mPtrSleepdTimeCategory;
    delete mPtrSleepdShutdownCategory;
    delete mPtrSleepdPowerCategory;
}

bool SleepdCategoryMethods::init()
{
    mPtrSleepdTimeoutCategory = new SleepdTimeoutCategory(mRefSleepdLsHandle);
    mPtrSleepdTimeCategory = new SleepdTimeCategory();
    mPtrSleepdShutdownCategory = new SleepdShutdownCategory(mRefPowerManagerService.getShutdownCategoryHandle(),
            mRefSleepdLsHandle);
    mPtrSleepdPowerCategory = new SleepdPowerCategory(mRefPowerManagerService, mRefSleepdLsHandle);

    if (!(mPtrSleepdTimeCategory && mPtrSleepdTimeoutCategory && mPtrSleepdShutdownCategory && mPtrSleepdPowerCategory)) {
        return false;
    }

    if (!(mPtrSleepdShutdownCategory->init() && mPtrSleepdPowerCategory->init() && mPtrSleepdTimeoutCategory->init())) {
        return false;
    }

    return true;
}

