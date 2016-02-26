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

#include "SleepdTimeoutCategory.h"
#include "PmsLuna2Utils.h"
#include "PmsLogging.h"

LSHandle *gPowerdLsHandle = nullptr;

SleepdTimeoutCategory::SleepdTimeoutCategory(LS::Handle &refSleepdLsHandle, LS::Handle &refPowerdLsHandle) :
    mRefSleepdLsHandle(refSleepdLsHandle),
    mRefPowerdLsHandle(refPowerdLsHandle)
{}

bool SleepdTimeoutCategory::init()
{
    LS_CREATE_CATEGORY_BEGIN(SleepdTimeoutCategory, timeoutAPI)
    LS_CATEGORY_METHOD(set)
    LS_CATEGORY_METHOD(clear)
    LS_CREATE_CATEGORY_END

    static LSMethod powerdMethods[] = {
        { "set", SleepdTimeoutCategory::set },
        { "clear", SleepdTimeoutCategory::clear },
        { 0, 0 }
    };


    try {
        mRefSleepdLsHandle.registerCategoryAppend("/timeout", const_cast<LSMethod *>(LS_CATEGORY_TABLE_NAME(timeoutAPI)),
                nullptr);
        mRefSleepdLsHandle.setCategoryData("/timeout", this);
        PMSLOG_INFO(MSGID_GENERAL, 0, "%s, %s, sleepdsupport /timeout category registration is success**", __FILE__,
                    __FUNCTION__);
    } catch (LS::Error &lunaError) {
        PMSLOG_ERROR(MSGID_CATEGORY_REG_FAIL, 0, "could not register sleepdsupport /timeout category: %s", lunaError.what());
        return false;
    }

    bool bRetVal;
    LSError lSError;
    LSErrorInit(&lSError);
    bRetVal = LSRegisterCategoryAppend(mRefPowerdLsHandle.get(), "/timeout", powerdMethods, NULL, &lSError);
    gPowerdLsHandle = mRefPowerdLsHandle.get();

    if (!bRetVal || !LSCategorySetData(mRefPowerdLsHandle.get(), "/timeout", this, &lSError)) {
        PMSLOG_ERROR(MSGID_CATEGORY_REG_FAIL, 0, "powerd timeout registration failed");
        LSErrorPrint(&lSError, stderr);
        LSErrorFree(&lSError);
    }

    return true;
}

bool SleepdTimeoutCategory::set(LSMessage &message)
{
    LSMessageRef(&message);
    LSCallOneReply(mRefSleepdLsHandle.get(), "palm://com.webos.service.alarm/set",
                   LSMessageGetPayload(&message), alarmsTimeoutCb, static_cast<void *>(&message), nullptr, nullptr);
    return true;
}

bool SleepdTimeoutCategory::set(LSHandle *sh, LSMessage *message, void *ctx)
{
    LS::Message request(message);
    pbnjson::JValue requestObj;
    int parseError = 0;

    const std::string schema = RELAXED_SCHEMA(PROPS_7(PROP(key, string), PROP(at, string), \
                               PROP(in, string), PROP(uri, string), PROP(params, object), \
                               PROP(wakeup, boolean), PROP(keep_existing, boolean))REQUIRED_4(key, uri, params, wakeup));

    if (!LSUtils::parsePayload(request.getPayload(), requestObj, schema, &parseError)) {
        PMSLOG_ERROR(MSGID_SCEMA_VAL_FAIL, 0, "set schema validation failed");
        LSUtils::respondWithError(request, errorParseFailed, 0);
        return true;
    }

    LSMessageRef(message);
    LSCallOneReply(gPowerdLsHandle, "palm://com.webos.service.alarm/set",
                   LSMessageGetPayload(message), alarmsTimeoutCb, static_cast<void *>(message), nullptr, nullptr);
    return true;
}

bool SleepdTimeoutCategory::clear(LSMessage &message)
{
    LS::Message request(&message);
    pbnjson::JValue requestObj;
    int parseError = 0;

    const std::string schema = STRICT_SCHEMA(PROPS_1(PROP(key, string))REQUIRED_1(key));

    if (!LSUtils::parsePayload(request.getPayload(), requestObj, schema, &parseError)) {
        PMSLOG_ERROR(MSGID_SCEMA_VAL_FAIL, 0, "clear schema validation failed");
        LSUtils::respondWithError(request, errorParseFailed, 0);
        return true;
    }

    LSMessageRef(&message);
    LSCallOneReply(mRefSleepdLsHandle.get(), "palm://com.webos.service.alarm/clear",
                   LSMessageGetPayload(&message), alarmsTimeoutCb, static_cast<void *>(&message), nullptr, nullptr);
    return true;
}

bool SleepdTimeoutCategory::clear(LSHandle *sh, LSMessage *message, void *ctx)
{
    LS::Message request(message);
    pbnjson::JValue requestObj;
    int parseError = 0;

    const std::string schema = STRICT_SCHEMA(PROPS_1(PROP(key, string))REQUIRED_1(key));

    if (!LSUtils::parsePayload(request.getPayload(), requestObj, schema, &parseError)) {
        PMSLOG_ERROR(MSGID_SCEMA_VAL_FAIL, 0, "clear schema validation failed");
        LSUtils::respondWithError(request, errorParseFailed, 0);
        return true;
    }

    LSMessageRef(message);
    LSCallOneReply(gPowerdLsHandle, "palm://com.webos.service.alarm/clear",
                   LSMessageGetPayload(message), alarmsTimeoutCb, static_cast<void *>(message), nullptr, nullptr);
    return true;
}


bool SleepdTimeoutCategory::alarmsTimeoutCb(LSHandle *sh, LSMessage *message, void *ctx)
{
    LSMessage *replyMessage = (LSMessage *)ctx;

    PMSLOG_INFO(MSGID_GENERAL, 0, "payload : %s", LSMessageGetPayload(message));

    if (replyMessage && LSMessageGetConnection(replyMessage)) {
        bool retVal = LSMessageReply(LSMessageGetConnection(replyMessage), replyMessage,
                                     LSMessageGetPayload(message), nullptr);

        if (!retVal) {
            PMSLOG_WARNING(MSGID_GENERAL, 0, "alarmsTimeoutCb could not send reply");
        }

        LSMessageUnref(replyMessage);
    } else {
        PMSLOG_ERROR(MSGID_GENERAL, 0, "alarmsTimeoutCb replyMessage is NULL");
    }

    return true;
}
