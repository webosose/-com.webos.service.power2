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

#include "SleepdCategoryMethods.h"
#include "SleepdTimeoutCategory.h"
#include "SleepdTimeCategory.h"
#include "SleepdShutdownCategory.h"
#include "SleepdPowerCategory.h"
#include "PowerManagerService.h"
#include "ShutdownCategoryMethods.h"
#include "PmsLogging.h"

const std::string sleepdServiceUri = "com.palm.sleep";
const std::string powerdServiceUri = "com.palm.power";

SleepdCategoryMethods::SleepdCategoryMethods(PowerManagerService &pmsRef, GMainLoop *mainLoop) :
    mRefPowerManagerService(pmsRef)
    , mLoopdata(mainLoop)
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
    try {
        mRefPowerdLsHandle = LS::registerService(powerdServiceUri.c_str());
        PMSLOG_INFO(MSGID_SHUTDOWN_DEBUG, 0, "%s, %s, powerd registration is success**", __FILE__, __FUNCTION__);
    } catch (LS::Error &lunaError) {
        PMSLOG_ERROR(MSGID_CATEGORY_REG_FAIL, 0, "could not register powerd : %s", lunaError.what());
        return false;
    }

    try {
        mRefSleepdLsHandle = LS::registerService(sleepdServiceUri.c_str());
        PMSLOG_INFO(MSGID_SHUTDOWN_DEBUG, 0, "%s, %s, sleepd registration is success**", __FILE__, __FUNCTION__);
    } catch (LS::Error &lunaError) {
        PMSLOG_ERROR(MSGID_CATEGORY_REG_FAIL, 0, "could not register powerd : %s", lunaError.what());
        return false;
    }

    mPtrSleepdTimeoutCategory = new SleepdTimeoutCategory(mRefSleepdLsHandle, mRefPowerdLsHandle);
    mPtrSleepdTimeCategory = new SleepdTimeCategory(mRefSleepdLsHandle, mRefPowerdLsHandle);
    mPtrSleepdShutdownCategory = new SleepdShutdownCategory(mRefPowerManagerService.getShutdownCategoryHandle(),
            mRefSleepdLsHandle, mRefPowerdLsHandle);
    mPtrSleepdPowerCategory = new SleepdPowerCategory(mRefPowerManagerService, mRefSleepdLsHandle, mRefPowerdLsHandle);

    if (!(mPtrSleepdTimeCategory && mPtrSleepdTimeoutCategory && mPtrSleepdShutdownCategory && mPtrSleepdPowerCategory)) {
        return false;
    }

    if (!(mPtrSleepdShutdownCategory->init() &&
          mPtrSleepdPowerCategory->init()
          && mPtrSleepdTimeoutCategory->init() &&
          mPtrSleepdTimeCategory->init())) {
        return false;
    }

    mRefSleepdLsHandle.attachToLoop(mLoopdata);
    mRefPowerdLsHandle.attachToLoop(mLoopdata);
    registerSubscriptionCancelCallback();
    mRefPowerManagerService.mSleepdHandle = mRefSleepdLsHandle.get();

    return true;
}

void SleepdCategoryMethods::registerSubscriptionCancelCallback()
{
    LSError lserror;
    LSErrorInit(&lserror);

    if (!LSSubscriptionSetCancelFunction(mRefSleepdLsHandle.get(), clientSubscriptionCancel, this, &lserror)) {
        PMSLOG_ERROR(MSGID_LS_SUBSCRIB_SETFUN_FAIL, 1, PMLOGKS(ERRTEXT, lserror.message),
                     "Error in setting subscription cancel function");
        LSErrorFree(&lserror);
    }
}

bool SleepdCategoryMethods::clientSubscriptionCancel(LSHandle *sh, LSMessage *msg, void *ctx)
{
    SleepdCategoryMethods *ptrHandle = static_cast<SleepdCategoryMethods *>(ctx);
    std::string clientId = LSMessageGetUniqueToken(msg);

    PMSLOG_DEBUG("subscription cancel deregisterClient[%s]",LSMessageGetMethod(msg));
    ptrHandle->getPmsHandle().deregisterClient(msg,clientId);

    return true;
}

