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

#include "SleepdShutdownCategory.h"
#include "ShutdownCategoryMethods.h"
#include "PmsLogging.h"
#include "PmsLuna2Utils.h"
#include "NyxUtil.h"
#include "ShutdownClientsMgr.h"

SleepdShutdownCategory::SleepdShutdownCategory(ShutdownCategoryMethods &handle, LS::Handle &refLsHandle,
        LS::Handle &refPowerdLsHandle) :
    mRefShutdownCategoryMethods(handle),
    mRefLsHandle(refLsHandle),
    mRefPowerdLsHandle(refPowerdLsHandle)
{
}

bool SleepdShutdownCategory::init(bool isPowerdUp)
{

    LS_CREATE_CATEGORY_BEGIN(SleepdShutdownCategory, shutdownAPI)
    LS_CATEGORY_METHOD(initiate)
    LS_CATEGORY_METHOD(machineReboot)
    LS_CATEGORY_METHOD(machineOff)
    LS_CATEGORY_METHOD(shutdownApplicationsRegister)
    LS_CATEGORY_METHOD(shutdownApplicationsAck)
    LS_CATEGORY_METHOD(shutdownServicesRegister)
    LS_CATEGORY_METHOD(shutdownServicesAck)
    LS_CREATE_CATEGORY_END

    static const LSSignal shutdownSignals[] = {
        { "shutdownApplications" },
        { "shutdownServices" },
        { },
    };

    try {
        mRefLsHandle.registerCategory("/shutdown",
                                      LS_CATEGORY_TABLE_NAME(shutdownAPI), shutdownSignals, nullptr);
        mRefLsHandle.setCategoryData("/shutdown", this);
        PMSLOG_INFO(MSGID_SHUTDOWN_DEBUG, 0, "%s, %s, sleepdsupport shutdown category registration is success**", __FILE__,
                    __FUNCTION__);
    } catch (LS::Error &lunaError) {
        PMSLOG_ERROR(MSGID_CATEGORY_REG_FAIL, 0, "could not register sleepdsupport shutdown category");
        return false;
    }

    if (isPowerdUp) {
        try {
            mRefPowerdLsHandle.registerCategory("/shutdown",
                                                LS_CATEGORY_TABLE_NAME(shutdownAPI), shutdownSignals, nullptr);
            mRefPowerdLsHandle.setCategoryData("/shutdown", this);
            PMSLOG_INFO(MSGID_SHUTDOWN_DEBUG, 0, "%s, %s, powerd shutdown category registration is success**", __FILE__,
                        __FUNCTION__);
        } catch (LS::Error &lunaError) {
            PMSLOG_ERROR(MSGID_CATEGORY_REG_FAIL, 0, "could not register powerd shutdown category");
        }
    }

    return true;
}

bool SleepdShutdownCategory::initiate(LSMessage &message)
{
    LS::Message request(&message);
    pbnjson::JValue responseObj = pbnjson::Object();
    responseObj.put("returnValue", true);
    LSUtils::postToClient(request, responseObj);
    return true;
}

bool SleepdShutdownCategory::shutdownApplicationsAck(LSMessage &message)
{
    return mRefShutdownCategoryMethods.shutdownApplicationsAck(message);
}

bool SleepdShutdownCategory::shutdownApplicationsRegister(LSMessage &message)
{
    return mRefShutdownCategoryMethods.shutdownApplicationsRegister(message);
}

bool SleepdShutdownCategory::shutdownServicesRegister(LSMessage &message)
{
    return mRefShutdownCategoryMethods.shutdownServicesRegister(message);
}

bool SleepdShutdownCategory::shutdownServicesAck(LSMessage &message)
{

    LS::Message request(&message);
    pbnjson::JValue requestObj;
    int parseError = 0;
    pbnjson::JValue responseObj = pbnjson::Object();

    const std::string schema = STRICT_SCHEMA(PROPS_2(PROP(clientId, string), PROP(isAck, boolean))REQUIRED_2(clientId,
                               isAck));

    if (!LSUtils::parsePayload(request.getPayload(), requestObj, schema, &parseError)) {
        PMSLOG_ERROR(MSGID_SCEMA_VAL_FAIL, 0, "shutdownServicesAck schema validation failed");
        LSUtils::respondWithError(request, errorParseFailed, 0);
        return true;
    }

    // get the clientId
    std::string clientId = requestObj["clientId"].asString();
    bool isAck = requestObj["isAck"].asBool();
    time_t diff = NyxUtil::getInstance().getRTCTime() - mRefShutdownCategoryMethods.getStartTime();
    mRefShutdownCategoryMethods.getShutDownClientMgr()->setAck(clientId, isAck, static_cast<double>(diff));

    if (mRefShutdownCategoryMethods.getShutDownClientMgr()->isAllClientsAcknowledged()) {
        mRefShutdownCategoryMethods.notifyAll();
    }

    responseObj.put("returnValue", true);
    responseObj.put("clientId", clientId);
    LSUtils::postToClient(request, responseObj);

    return true;
}

bool SleepdShutdownCategory::machineOff(LSMessage &message)
{

    return mRefShutdownCategoryMethods.machineOff(message);
}

bool SleepdShutdownCategory::machineReboot(LSMessage &message)
{
    return mRefShutdownCategoryMethods.machineReboot(message);
}
