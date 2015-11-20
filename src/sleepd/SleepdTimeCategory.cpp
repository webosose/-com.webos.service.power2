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

#include "SleepdTimeCategory.h"
#include "PmsLuna2Utils.h"
#include "PmsLogging.h"

SleepdTimeCategory::SleepdTimeCategory(LS::Handle &refPowerdLsHandle) :
    mRefPowerdLsHandle(refPowerdLsHandle)

{
}
bool SleepdTimeCategory::init(bool isPowerdUp)
{
    if (isPowerdUp) {
        LS_CREATE_CATEGORY_BEGIN(SleepdTimeCategory, timeAPI)
        LS_CATEGORY_METHOD(alarmAddCalendar)
        LS_CATEGORY_METHOD(alarmAdd)
        LS_CATEGORY_METHOD(alarmQuery)
        LS_CATEGORY_METHOD(alarmRemove)
        LS_CREATE_CATEGORY_END


        try {
            mRefPowerdLsHandle.registerCategoryAppend("/time", const_cast<LSMethod *>(LS_CATEGORY_TABLE_NAME(timeAPI)), nullptr);
            mRefPowerdLsHandle.setCategoryData("/time", this);
            PMSLOG_INFO(MSGID_GENERAL, 0, "%s, %s, powerd /time category registration is success**", __FILE__,
                        __FUNCTION__);
        } catch (LS::Error &lunaError) {
            PMSLOG_ERROR(MSGID_CATEGORY_REG_FAIL, 0, "could not register powerd /time category");
            return false;
        }
    }

    return true;
}

bool SleepdTimeCategory::alarmAddCalendar(LSMessage &message)
{
    LS::Message request(&message);
    pbnjson::JValue responseObj = pbnjson::Object();
    pbnjson::JValue requestObj;
    int parseError = 0;

    if (!LSUtils::parsePayload(request.getPayload(), requestObj, SCHEMA_ANY, &parseError)) {
        PMSLOG_ERROR(MSGID_SCEMA_VAL_FAIL, 0, "identify schema validation failed");
        LSUtils::respondWithError(request, errorParseFailed, 0);
        return true;
    }

    responseObj.put("returnValue", true);
    LSUtils::postToClient(request, responseObj);
    return true;

}

bool SleepdTimeCategory::alarmAdd(LSMessage &message)
{
    LS::Message request(&message);
    pbnjson::JValue responseObj = pbnjson::Object();
    pbnjson::JValue requestObj;
    int parseError = 0;

    if (!LSUtils::parsePayload(request.getPayload(), requestObj, SCHEMA_ANY, &parseError)) {
        PMSLOG_ERROR(MSGID_SCEMA_VAL_FAIL, 0, "identify schema validation failed");
        LSUtils::respondWithError(request, errorParseFailed, 0);
        return true;
    }

    responseObj.put("returnValue", true);
    LSUtils::postToClient(request, responseObj);
    return true;

}

bool SleepdTimeCategory::alarmQuery(LSMessage &message)
{
    LS::Message request(&message);
    pbnjson::JValue responseObj = pbnjson::Object();
    pbnjson::JValue requestObj;
    int parseError = 0;

    if (!LSUtils::parsePayload(request.getPayload(), requestObj, SCHEMA_ANY, &parseError)) {
        PMSLOG_ERROR(MSGID_SCEMA_VAL_FAIL, 0, "identify schema validation failed");
        LSUtils::respondWithError(request, errorParseFailed, 0);
        return true;
    }

    responseObj.put("returnValue", true);
    LSUtils::postToClient(request, responseObj);
    return true;

}

bool SleepdTimeCategory::alarmRemove(LSMessage &message)
{
    LS::Message request(&message);
    pbnjson::JValue responseObj = pbnjson::Object();
    pbnjson::JValue requestObj;
    int parseError = 0;

    if (!LSUtils::parsePayload(request.getPayload(), requestObj, SCHEMA_ANY, &parseError)) {
        PMSLOG_ERROR(MSGID_SCEMA_VAL_FAIL, 0, "identify schema validation failed");
        LSUtils::respondWithError(request, errorParseFailed, 0);
        return true;
    }

    responseObj.put("returnValue", true);
    LSUtils::postToClient(request, responseObj);
    return true;

}

