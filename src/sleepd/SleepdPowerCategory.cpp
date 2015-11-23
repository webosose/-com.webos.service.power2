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

#include "PowerManagerService.h"
#include "PmsLuna2Utils.h"
#include "SleepdPowerCategory.h"
#include "WakelockClientsMgr.h"
#include "PmsLogging.h"

using namespace LSUtils;

const std::string powerdServiceUri = "com.palm.power";
const std::string batterydServiceUri = "com.webos.service.battery";
const std::string chargingStatusPath = "/sys/class/power_supply/battery/status";
const std::string otpEventPath = "/sys/module/qpnp_charger/parameters/otp_event";
#define STR_BUF_MAX 32

#define URI_BATTERY_SERVICE "luna://com.webos.service.battery"
#define URI_SIGNAL_ADDMATCH "palm://com.palm.lunabus/signal/addmatch"


#define JSON_BATTERY_STATUS_SIGNAL "{\"category\":\"/\",\"method\":\"batteryStatus\"}"
#define JSON_CHARGER_STATUS_SIGNAL "{\"category\":\"/\",\"method\":\"chargerStatus\"}"
#define JSON_BATTERY_GET_STATUS_SIGNAL "{\"category\":\"/\",\"method\":\"getBatteryStatus\"}"
#define JSON_CHARGER_GET_STATUS_SIGNAL "{\"category\":\"/\",\"method\":\"getChargerStatus\"}"

#define JSON_CHARGER_CONNECT_SIGNAL "{\"category\":\"/\",\"method\":\"chargerConnected\"}"

LSHandle *gPowerdHandle = nullptr;

SleepdPowerCategory::SleepdPowerCategory(PowerManagerService &refHandle, LS::Handle &sleepdLsHandle,
        LS::Handle &powerdLsHandle) :
    mRefPms(refHandle),
    mRefSleepdLsHandle(sleepdLsHandle),
    mRefPowerdLsHandle(powerdLsHandle)
{
}

bool SleepdPowerCategory::init(bool isPowerdUp)
{
    LS_CREATE_CATEGORY_BEGIN(SleepdPowerCategory, powerCategoryAPI)
    LS_CATEGORY_METHOD(identify)
    LS_CATEGORY_METHOD(wakeLockRegister)
    LS_CATEGORY_METHOD(setWakeLock)
    LS_CATEGORY_METHOD(clientCancelByName)
    LS_CATEGORY_METHOD(activityStart)
    LS_CATEGORY_METHOD(activityEnd)
    LS_CATEGORY_METHOD(somebodyWantsWakeup)
    LS_CATEGORY_METHOD(systemTimeChanged)
    LS_CREATE_CATEGORY_END

    static LSSignal powerCategorySignals[] = {
        { "suspended" },
        { "resume" },
        { },
    };

    try {
        mRefSleepdLsHandle.registerCategoryAppend("/com/palm/power",
                const_cast<LSMethod *>(LS_CATEGORY_TABLE_NAME(powerCategoryAPI)), powerCategorySignals);
        mRefSleepdLsHandle.setCategoryData("/com/palm/power", this);
        PMSLOG_INFO(MSGID_SHUTDOWN_DEBUG, 0, "%s, %s, sleepdsupport /com/palm/power category registration is success**",
                    __FILE__,
                    __FUNCTION__);
    } catch (LS::Error &lunaError) {
        PMSLOG_ERROR(MSGID_CATEGORY_REG_FAIL, 0, "could not register sleepdsupport /com/palm/power category");
        return false;
    }

    if (isPowerdUp) {
        registerPowerdMethods(mRefPowerdLsHandle);
    }

    return true;
}

void SleepdPowerCategory::registerPowerdMethods(LS::Handle &powerdLsHandle)
{
    static LSSignal powerdSignals[] = {
        { "batteryStatus" },
        { "batteryStatusQuery" },
        { "chargerStatus" },
        { "chargerStatusQuery" },
        { "chargerConnected" },
        { "USBDockStatus" },
        { "suspendRequest" },
        { "prepareSuspend" },
        { "suspended" },
        { "resume" },
        { },
    };

    LS_CREATE_CATEGORY_BEGIN(SleepdPowerCategory, powerdAPI)
    LS_CATEGORY_METHOD(battchargingStatusQuery)
    LS_CATEGORY_METHOD(batteryStatusQuery)
    LS_CATEGORY_METHOD(chargerStatusQuery)
    LS_CATEGORY_METHOD(getFakeBatteryMode)
    LS_CATEGORY_METHOD(setFakeBatteryMode)
    LS_CATEGORY_METHOD(suspendRequestRegister)
    LS_CATEGORY_METHOD(prepareSuspendRegister)
    LS_CATEGORY_METHOD(prepareSuspendAck)
    LS_CATEGORY_METHOD(suspendRequestAck)
    LS_CATEGORY_METHOD(batterySaverOnOff)
    LS_CATEGORY_METHOD(identify)
    LS_CATEGORY_METHOD(activityStart)
    LS_CATEGORY_METHOD(activityEnd)
    LS_CATEGORY_METHOD(systemTimeChanged)
    LS_CREATE_CATEGORY_END

    try {
        powerdLsHandle.registerCategoryAppend("/com/palm/power", const_cast<LSMethod *>(LS_CATEGORY_TABLE_NAME(powerdAPI)),
                                              powerdSignals);
        powerdLsHandle.setCategoryData("/com/palm/power", this);
        PMSLOG_INFO(MSGID_SHUTDOWN_DEBUG, 0, "%s, %s, powerd /com/palm/power category registration is success**", __FILE__,
                    __FUNCTION__);
        PMSLOG_DEBUG("powerd category registration is success**");
        gPowerdHandle = powerdLsHandle.get();
        checkBatterydStatus();
    } catch (LS::Error &lunaError) {
        PMSLOG_ERROR(MSGID_CATEGORY_REG_FAIL, 0, "could not register powerd /com/palm/power category");
    }
}


bool SleepdPowerCategory::checkBatterydStatus()
{
    LSError lserror;
    LSErrorInit(&lserror);
    bool result = false;

    result =  LSRegisterServerStatusEx(mRefSleepdLsHandle.get(),
                                       batterydServiceUri.c_str(),
                                       SleepdPowerCategory::batterydServiceStatusCb,
                                       this,
                                       &mBatterydCookie,
                                       &lserror);

    if (!result) {
        PMSLOG_ERROR(MSGID_SERVER_STATUS_REGISTRY_FAIL, 0, "RegisterServerStatus failed for Battery_Service");
        LSErrorFree(&lserror);
    }

    return result;
}

bool SleepdPowerCategory::_batterydServiceStatusCb(LSHandle *sh, const char *serviceName, bool connected)
{
    PMSLOG_DEBUG("%s  %d", __FUNCTION__, connected);

    if (connected) {
        LSError lserror;
        LSErrorInit(&lserror);

        bool result = false;
        result = LSCall(gPowerdHandle, URI_SIGNAL_ADDMATCH, JSON_BATTERY_STATUS_SIGNAL,
                        SleepdPowerCategory::batteryStatusQuerySignal, this, NULL, &lserror);

        if (!result) {
            LSErrorPrint(&lserror, stderr);
            LSErrorFree(&lserror);
        }

        result = LSCall(gPowerdHandle, URI_SIGNAL_ADDMATCH, JSON_BATTERY_GET_STATUS_SIGNAL,
                        SleepdPowerCategory::batteryStatusQuerySignal, this, NULL, &lserror);

        if (!result) {
            LSErrorPrint(&lserror, stderr);
            LSErrorFree(&lserror);
        }

        result = LSCall(gPowerdHandle, URI_SIGNAL_ADDMATCH, JSON_CHARGER_GET_STATUS_SIGNAL,
                        SleepdPowerCategory::chargerStatusQuerySignal, this, NULL, &lserror);

        if (!result) {
            LSErrorPrint(&lserror, stderr);
            LSErrorFree(&lserror);
        }

        result = LSCall(gPowerdHandle, URI_SIGNAL_ADDMATCH, JSON_CHARGER_STATUS_SIGNAL,
                        SleepdPowerCategory::chargerStatusQuerySignal, this, NULL, &lserror);

        if (!result) {
            LSErrorPrint(&lserror, stderr);
            LSErrorFree(&lserror);
        }

        result = LSCall(gPowerdHandle, URI_SIGNAL_ADDMATCH, JSON_CHARGER_CONNECT_SIGNAL, SleepdPowerCategory::chargerConnected,
                        this, NULL, &lserror);

        if (!result) {
            LSErrorPrint(&lserror, stderr);
            LSErrorFree(&lserror);
        }
    }

    return true;
}



bool SleepdPowerCategory::batteryStatusQuerySignal(LSHandle *sh, LSMessage *message, void *user_data)
{
    LS::Message request(message);
    pbnjson::JValue responseObj = pbnjson::Object();
    pbnjson::JValue requestObj;
    int parseError = 0;
    PMSLOG_DEBUG("Signal recieved from batteryd %s", __FUNCTION__);
    bool retVal = LSUtils::parsePayload(request.getPayload(), requestObj, SCHEMA_ANY, &parseError);

    if (retVal) {
        responseObj.put("percent", (requestObj["percent"].asNumber<int32_t>()));
        responseObj.put("percent_ui", (requestObj["percent_ui"].asNumber<int32_t>()));
        responseObj.put("temperature_C", (requestObj["temperature_C"].asNumber<int32_t>()));
        responseObj.put("current_mA", (requestObj["current_mA"].asNumber<int32_t>()));
        responseObj.put("voltage_mV", (requestObj["voltage_mV"].asNumber<int32_t>()));
        responseObj.put("capacity_mAh", (requestObj["capacity_mAh"].asNumber<int32_t>()));
        responseObj.put("present", (requestObj["present"].asBool()));
        responseObj.put("health", (requestObj["health"].asNumber<int32_t>()));
        responseObj.put("charging", (requestObj["charging"].asBool()));


        std::string payload;
        generatePayload(responseObj, payload);

        LSError lserror;
        retVal = LSSignalSend(gPowerdHandle, "luna://com.palm.power/com/palm/power/batteryStatus", payload.c_str(), &lserror);

        if (!retVal) {
            LSErrorFree(&lserror);
        }

        retVal = LSSignalSend(gPowerdHandle, "luna://com.palm.power/com/palm/power/batteryStatusQuery", payload.c_str(),
                              &lserror);

        if (!retVal) {
            LSErrorFree(&lserror);
        }
    }

    return true;
}

bool SleepdPowerCategory::chargerConnected(LSHandle *sh, LSMessage *message, void *user_data)
{
    LSError lserror;
    LS::Message request(message);
    pbnjson::JValue responseObj = pbnjson::Object();
    pbnjson::JValue requestObj;
    bool retVal;
    int parseError = 0;
    PMSLOG_DEBUG("Signal recieved from batteryd %s", __FUNCTION__);

    SleepdPowerCategory *ctx = reinterpret_cast<SleepdPowerCategory *>(user_data);

    retVal = LSUtils::parsePayload(request.getPayload(), requestObj, SCHEMA_ANY, &parseError);

    if (retVal) {
        responseObj.put("connected", (requestObj["connected"].asBool()));
        ctx->mIsChargerPresent = (requestObj["connected"].asBool());
        std::string payload;
        generatePayload(responseObj, payload);

        retVal = LSSignalSend(gPowerdHandle, "luna://com.palm.power/com/palm/power/chargerConnected", payload.c_str(),
                              &lserror);

        if (!retVal) {
            LSErrorFree(&lserror);
        }
    }

    return true;
}

bool SleepdPowerCategory::chargerStatusQuerySignal(LSHandle *sh, LSMessage *message, void *user_data)
{
    LSError lserror;
    LS::Message request(message);
    pbnjson::JValue responseObj = pbnjson::Object();
    pbnjson::JValue responseObjDock = pbnjson::Object();
    pbnjson::JValue responseObjCharger = pbnjson::Object();

    pbnjson::JValue requestObj;
    int parseError = 0;
    PMSLOG_DEBUG("Signal recieved from batteryd %s", __FUNCTION__);

    bool retVal = LSUtils::parsePayload(request.getPayload(), requestObj, SCHEMA_ANY, &parseError);

    if (retVal) {
        responseObj.put("type", (requestObj["type"].asString()));
        responseObj.put("name", (requestObj["name"].asString()));
        responseObj.put("connected", (requestObj["connected"].asBool()));
        responseObj.put("current_mA", (requestObj["current_mA"].asNumber<int32_t>()));
        responseObj.put("message_source", "powerd");

        std::string payload;
        generatePayload(responseObj, payload);

        responseObjDock.put("DockConnected", "false");
        responseObjDock.put("DockPower", "-1");
        responseObjDock.put("DockSerialNo", "-1");

        if (requestObj["type"].asString() == "usb") {
            responseObjDock.put("USBConnected", "true");
        } else {
            responseObjDock.put("USBConnected", "false");
        }

        responseObjDock.put("ACConnected", "false");
        responseObjDock.put("USBName", (requestObj["name"].asString()));
        responseObjDock.put("Charging", (requestObj["connected"].asBool() ? "true" : "false"));

        std::string payloadDock;
        generatePayload(responseObjDock, payloadDock);

        responseObjCharger.put("connected", (requestObj["connected"].asBool() ? "true" : "false"));
        std::string payloadCharger;
        generatePayload(responseObjCharger, payloadCharger);

        retVal = LSSignalSend(gPowerdHandle, "luna://com.palm.power/com/palm/power/USBDockStatus", payloadDock.c_str(),
                              &lserror);

        if (!retVal) {
            LSErrorFree(&lserror);
        }

        retVal = LSSignalSend(gPowerdHandle, "luna://com.palm.power/com/palm/power/chargerStatus", payload.c_str(), &lserror);

        if (!retVal) {
            LSErrorFree(&lserror);
        }

        retVal = LSSignalSend(gPowerdHandle, "luna://com.palm.power/com/palm/power/chargerConnected", payloadCharger.c_str(),
                              &lserror);

        if (!retVal) {
            LSErrorFree(&lserror);
        }

    }

    return true;
}

void SleepdPowerCategory::addSubscription(LSMessage &message)
{
    LSError lserror;
    LSErrorInit(&lserror);

    bool retVal = LSSubscriptionAdd(mRefSleepdLsHandle.get(), "sleepd", &message, &lserror);

    if (!retVal) {
        PMSLOG_ERROR(MSGID_LSSUBSCRI_ADD_FAIL, 1, PMLOGKS(ERRTEXT, lserror.message), "LSSubscriptionAdd failed");
        LSErrorFree(&lserror);
    }
}

bool SleepdPowerCategory::battchargingStatusQuery(LSMessage &message)
{
    char str_buf[STR_BUF_MAX];
    char str_buf2[STR_BUF_MAX];
    char *payload;
    int rv;

    if ((rv = readLineFromFile(chargingStatusPath.c_str(), str_buf, STR_BUF_MAX)) > 0) {
        str_buf[rv - 1] = '\0';
    } else {
        strcpy(str_buf, "ReadFail");
    }

    if ((rv = readLineFromFile(otpEventPath.c_str(), str_buf2, STR_BUF_MAX)) > 0) {
        str_buf2[rv - 1] = '\0';
    } else {
        strcpy(str_buf2, "ReadFail");
    }

    payload = g_strdup_printf("{\"ChargingStatus\":\"%s\",\"OTPevent\":%s}",
                              str_buf, str_buf2);

    if (!payload) {
        return true;
    }

    LSError lserror;
    LSErrorInit(&lserror);
    bool retVal = LSMessageReply(mRefPowerdLsHandle.get(), &message, payload, &lserror);

    if (!retVal) {
        LSErrorPrint(&lserror, stderr);
        LSErrorFree(&lserror);
    }

    g_free(payload);

    return retVal;
}

int SleepdPowerCategory::readLineFromFile(const char *path, char *buf, size_t count)
{
    char *fgets_ret;
    FILE *fd;
    int rv;

    fd = fopen(path, "r");

    if (fd == NULL) {
        return -1;
    }

    fgets_ret = fgets(buf, count, fd);

    if (NULL != fgets_ret) {
        rv = strlen(buf);
    } else {
        rv = ferror(fd);
    }

    fclose(fd);

    return rv;
}

bool SleepdPowerCategory::batteryStatusQuery(LSMessage &message)
{
    LS::Message request(&message);
    pbnjson::JValue responseObj = pbnjson::Object();
    pbnjson::JValue requestObj;
    int parseError = 0;
    LSError lserror;

    if (!LSUtils::parsePayload(request.getPayload(), requestObj, SCHEMA_ANY, &parseError)) {
        PMSLOG_ERROR(MSGID_SCEMA_VAL_FAIL, 0, "identify schema validation failed");
        LSUtils::respondWithError(request, errorParseFailed, 0);
        return true;
    }

    LSMessageRef(&message);

    bool success = LSCallOneReply(gPowerdHandle, "palm://com.webos.service.battery/getBatteryStatus", "{}",
                                  SleepdPowerCategory::batteryStatusCallback
                                  , (void *)(&message), NULL, &lserror);

    if (!success) {
        PMSLOG_DEBUG(MSGID_LSSUBSCRI_ADD_FAIL, 1, "LSCallOneReply failed");
        LSErrorFree(&lserror);
        responseObj.put("returnValue", false);
        LSUtils::postToClient(request, responseObj);
        return true;
    }

    return true;

}

bool SleepdPowerCategory::chargerStatusQuery(LSMessage &message)
{
    LS::Message request(&message);
    pbnjson::JValue responseObj = pbnjson::Object();
    pbnjson::JValue requestObj;
    int parseError = 0;
    LSError lserror;

    if (!LSUtils::parsePayload(request.getPayload(), requestObj, SCHEMA_ANY, &parseError)) {
        PMSLOG_ERROR(MSGID_SCEMA_VAL_FAIL, 0, "identify schema validation failed");
        LSUtils::respondWithError(request, errorParseFailed, 0);
        return true;
    }

    LSMessageRef(&message);

    bool success = LSCallOneReply(gPowerdHandle, "palm://com.webos.service.battery/getChargerStatus", "{}",
                                  SleepdPowerCategory::chargerStatusCallback
                                  , (void *)(&message), NULL, &lserror);

    if (!success) {
        PMSLOG_DEBUG(MSGID_LSSUBSCRI_ADD_FAIL, 1, "LSCallOneReply failed");
        LSErrorFree(&lserror);
        responseObj.put("returnValue", false);
        LSUtils::postToClient(request, responseObj);
        return true;
    }

    return true;
}

bool SleepdPowerCategory::getFakeBatteryMode(LSMessage &message)
{
    LS::Message request(&message);
    pbnjson::JValue responseObj = pbnjson::Object();
    pbnjson::JValue requestObj;
    int parseError = 0;
    LSError lserror;

    if (!LSUtils::parsePayload(request.getPayload(), requestObj, SCHEMA_ANY, &parseError)) {
        PMSLOG_ERROR(MSGID_SCEMA_VAL_FAIL, 0, "identify schema validation failed");
        LSUtils::respondWithError(request, errorParseFailed, 0);
        return true;
    }

    LSMessageRef(&message);

    bool success = LSCallOneReply(gPowerdHandle, "palm://com.webos.service.battery/getFakeBatteryMode", "{}",
                                  SleepdPowerCategory::getFakeBatteryModeCallback
                                  , (void *)(&message), NULL, &lserror);

    if (!success) {
        PMSLOG_DEBUG(MSGID_LSSUBSCRI_ADD_FAIL, 1, "LSCallOneReply failed");
        LSErrorFree(&lserror);
        responseObj.put("returnValue", false);
        LSUtils::postToClient(request, responseObj);
        return true;
    }

    return true;
}

bool SleepdPowerCategory::setFakeBatteryMode(LSMessage &message)
{
    LS::Message request(&message);
    pbnjson::JValue responseObj = pbnjson::Object();
    pbnjson::JValue requestObj;
    int parseError = 0;
    LSError lserror;

    if (!LSUtils::parsePayload(request.getPayload(), requestObj, SCHEMA_ANY, &parseError)) {
        PMSLOG_ERROR(MSGID_SCEMA_VAL_FAIL, 0, "identify schema validation failed");
        LSUtils::respondWithError(request, errorParseFailed, 0);
        return true;
    }

    LSMessageRef(&message);
    bool success = LSCallOneReply(gPowerdHandle, "palm://com.webos.service.battery/setFakeBatteryMode",
                                  LSMessageGetPayload(&message), SleepdPowerCategory::setFakeBatteryModeCallback
                                  , (void *)(&message), NULL, &lserror);

    if (!success) {
        PMSLOG_DEBUG(MSGID_LSSUBSCRI_ADD_FAIL, 1, "LSCallOneReply failed");
        LSErrorFree(&lserror);
        responseObj.put("returnValue", false);
        LSUtils::postToClient(request, responseObj);
        return true;
    }

    return true;
}

bool SleepdPowerCategory::suspendRequestRegister(LSMessage &message)
{
    LS::Message request(&message);
    pbnjson::JValue responseObj = pbnjson::Object();
    pbnjson::JValue requestObj;
    int parseError = 0;


    if (!LSUtils::parsePayload(request.getPayload(), requestObj, SCHEMA_ANY, &parseError)) {
        PMSLOG_ERROR(MSGID_SCEMA_VAL_FAIL, 0, "suspendRequestRegister schema validation failed");
        LSUtils::respondWithError(request, errorParseFailed, 0);
        return true;
    }

    responseObj.put("returnValue", true);
    LSUtils::postToClient(request, responseObj);
    return true;
}

bool SleepdPowerCategory::prepareSuspendRegister(LSMessage &message)
{
    LS::Message request(&message);
    pbnjson::JValue responseObj = pbnjson::Object();
    pbnjson::JValue requestObj;
    int parseError = 0;


    if (!LSUtils::parsePayload(request.getPayload(), requestObj, SCHEMA_ANY, &parseError)) {
        PMSLOG_ERROR(MSGID_SCEMA_VAL_FAIL, 0, "prepareSuspendRegister schema validation failed");
        LSUtils::respondWithError(request, errorParseFailed, 0);
        return true;
    }

    responseObj.put("returnValue", true);
    LSUtils::postToClient(request, responseObj);
    return true;
}

bool SleepdPowerCategory::suspendRequestAck(LSMessage &message)
{
    LS::Message request(&message);
    pbnjson::JValue responseObj = pbnjson::Object();
    pbnjson::JValue requestObj;
    int parseError = 0;


    if (!LSUtils::parsePayload(request.getPayload(), requestObj, SCHEMA_ANY, &parseError)) {
        PMSLOG_ERROR(MSGID_SCEMA_VAL_FAIL, 0, "suspendRequestAck schema validation failed");
        LSUtils::respondWithError(request, errorParseFailed, 0);
        return true;
    }

    responseObj.put("returnValue", true);
    LSUtils::postToClient(request, responseObj);
    return true;
}

bool SleepdPowerCategory::prepareSuspendAck(LSMessage &message)
{
    LS::Message request(&message);
    pbnjson::JValue responseObj = pbnjson::Object();
    pbnjson::JValue requestObj;
    int parseError = 0;


    if (!LSUtils::parsePayload(request.getPayload(), requestObj, SCHEMA_ANY, &parseError)) {
        PMSLOG_ERROR(MSGID_SCEMA_VAL_FAIL, 0, "prepareSuspendAck schema validation failed");
        LSUtils::respondWithError(request, errorParseFailed, 0);
        return true;
    }

    responseObj.put("returnValue", true);
    LSUtils::postToClient(request, responseObj);
    return true;
}

bool SleepdPowerCategory::batterySaverOnOff(LSMessage &message)
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

bool SleepdPowerCategory::identify(LSMessage &message)
{
    LS::Message request(&message);
    pbnjson::JValue responseObj = pbnjson::Object();
    pbnjson::JValue requestObj;
    int parseError = 0;

    const std::string schema = STRICT_SCHEMA(PROPS_2(PROP(clientName, string), PROP(subscribe,
                               boolean))REQUIRED_2(clientName,
                                       subscribe));

    if (!LSUtils::parsePayload(request.getPayload(), requestObj, schema, &parseError)) {
        PMSLOG_ERROR(MSGID_SCEMA_VAL_FAIL, 0, "identify schema validation failed");
        LSUtils::respondWithError(request, errorParseFailed, 0);
        return true;
    }

    if (!requestObj["subscribe"].asBool()) {
        LSUtils::respondWithError(request, errorInvalidParam, 0);
        return true;
    }

    //get the clientId
    std::string clientName ;


    clientName = requestObj["clientName"].asString();

    std::string clientId = request.getUniqueToken();

    if (clientName.empty()) {
        LSUtils::respondWithError(request, errorInvalidParam, 0);
        return true;
    }

    addSubscription(message);
    // add the client to client list
    mRefPms.getWakeLockManageRef()->addClient(clientId, clientName);

    PMSLOG_DEBUG("[%s] client registered. clientName[%s] clientId[%s]",
                 __FUNCTION__, clientName.c_str(), clientId.c_str());


    //return response
    responseObj.put("returnValue", true);
    responseObj.put("subscribed", true);
    responseObj.put("clientId", clientId);
    LSUtils::postToClient(request, responseObj);

    return true;
}

bool SleepdPowerCategory::activityStart(LSMessage &message)
{
    LS::Message request(&message);
    pbnjson::JValue responseObj = pbnjson::Object();
    pbnjson::JValue requestObj;
    int parseError = 0;

    const std::string schema = STRICT_SCHEMA(PROPS_2(PROP(id, string), PROP(duration_ms, integer))REQUIRED_2(id,
                               duration_ms));

    if (!LSUtils::parsePayload(request.getPayload(), requestObj, schema, &parseError)) {
        PMSLOG_ERROR(MSGID_SCEMA_VAL_FAIL, 0, "identify schema validation failed");
        LSUtils::respondWithError(request, errorParseFailed, 0);
        return true;
    }


    std::string clientName = requestObj["id"].asString();

    std::string clientId = request.getUniqueToken();
    int timeout = (requestObj["duration_ms"].asNumber<int32_t>()) / 1000;

    if (clientName.empty() || timeout <= 0) {
        LSUtils::respondWithError(request, errorInvalidParam, 0);
        return true;
    }

    addSubscription(message);
    // add the client to client list
    mRefPms.getWakeLockManageRef()->addClient(clientId, clientName);
    pms_support_notify_wakeup(mRefPms.getWakeLockManageRef()->getWakelockCount());

    PMSLOG_DEBUG("[%s] client registered. clientName[%s] clientId[%s]",
                 __FUNCTION__, clientName.c_str(), clientId.c_str());
    std::string sender = LSMessageGetSenderServiceName(&message);

    if (mRefPms.setAwake(timeout, request, clientId, sender)) {
        responseObj.put("returnValue", true);
        responseObj.put("clientId", clientId);
        LSUtils::postToClient(request, responseObj);
    }

    return true;
}

bool SleepdPowerCategory::activityEnd(LSMessage &message)
{

    LS::Message request(&message);
    pbnjson::JValue responseObj = pbnjson::Object();
    pbnjson::JValue requestObj;
    int parseError = 0;
    const std::string schema = STRICT_SCHEMA(PROPS_1(PROP(id, string))REQUIRED_1(id));

    if (!LSUtils::parsePayload(request.getPayload(), requestObj, schema, &parseError)) {
        PMSLOG_ERROR(MSGID_SCEMA_VAL_FAIL, 0, "clearKeepAwake schema validation failed");
        LSUtils::respondWithError(request, errorParseFailed, 0);
        return true;
    }

    /********** clientId ***********/
    std::string clientName = requestObj["id"].asString();
    std::string clientId = request.getUniqueToken();

    // if client doesn't exist return error
    if (!mRefPms.getWakeLockManageRef()->isClientExist(clientId)) {
        LSUtils::respondWithError(request, errorInvalidClient, 0);
        return true;
    }

    // if wakelock is not set return error
    if (!mRefPms.getWakeLockManageRef()->isWakelockSet(clientId)) {
        std::string errorNoWakelock("wakelock is not set");
        LSUtils::respondWithError(request, errorNoWakelock, 0);
        return true;
    }

    // clear the alarm and clear the wakelock
    mRefPms.clearAlarm(clientId);
    mRefPms.getWakeLockManageRef()->clearWakelock(clientId);
    pms_support_notify_wakeup(mRefPms.getWakeLockManageRef()->getWakelockCount());

    responseObj.put("returnValue", true);
    LSUtils::postToClient(request, responseObj);
    return true;
}

bool SleepdPowerCategory::somebodyWantsWakeup(LSMessage &message)
{

    LS::Message request(&message);
    pbnjson::JValue requestObj;
    pbnjson::JValue responseObj = pbnjson::Object();
    int parseError = 0;
    std::string schema = STRICT_SCHEMA(PROPS_3(PROP(clientId, string), PROP(isWakeup, boolean), PROP(wakeupReason,
                                       string))REQUIRED_3(clientId, isWakeup, wakeupReason));

    if (!LSUtils::parsePayload(request.getPayload(), requestObj, schema, &parseError)) {
        PMSLOG_ERROR(MSGID_SCEMA_VAL_FAIL, 0, "schema validation failed");
        LSUtils::respondWithError(request, errorParseFailed, 0);
        return true;
    }


    if (mRefPms.isSuspendedState() && requestObj["isWakeup"].asBool()) {
        // Set kernel wakelock for 1 second.
        system("echo somebodyWantsWakeup 1000000000 > /sys/power/wake_lock");

    }

    responseObj.put("returnValue", true);
    LSUtils::postToClient(request, responseObj);
    return true;
}

bool SleepdPowerCategory::systemTimeChanged(LSMessage &message)
{
    LS::Message request(&message);
    pbnjson::JValue requestObj;
    LSError lserror;
    int parseError = 0;
    PMSLOG_DEBUG("In SystemTimeChanged--> called via %s", LSMessageGetApplicationID(&message));

    if (!LSUtils::parsePayload(request.getPayload(), requestObj, SCHEMA_ANY, &parseError)) {
        PMSLOG_ERROR(MSGID_SCEMA_VAL_FAIL, 0, "systemTimeChanged schema validation failed");
        LSUtils::respondWithError(request, errorParseFailed, 0);
        return true;
    }

    pbnjson::JValue responseObj = pbnjson::Object();

    PMSLOG_DEBUG("In SystemTimeChanged of PMS ");
    bool success = LSCallOneReply(mRefSleepdLsHandle.get(), "palm://com.webos.service.power2/wearable/systemTimeChanged",
                                  "{}", SleepdPowerCategory::systemTimeChangedCallback,
                                  NULL, NULL, &lserror);

    if (!success) {
        PMSLOG_DEBUG(MSGID_LSSUBSCRI_ADD_FAIL, 1, "LSCallOneReply failed");
        LSErrorFree(&lserror);
        responseObj.put("returnValue", false);
        LSUtils::postToClient(request, responseObj);
        return true;
    }

    responseObj.put("returnValue", true);
    LSUtils::postToClient(request, responseObj);
    return true;
}

bool SleepdPowerCategory::systemTimeChangedCallback(LSHandle *sh, LSMessage *message, void *ctx)
{
    PMSLOG_DEBUG("%s", __FUNCTION__);
    return true;
}

bool SleepdPowerCategory::wakeLockRegister(LSMessage &message)
{
    LS::Message request(&message);
    pbnjson::JValue requestObj;
    int parseError = 0;

    const std::string schema = STRICT_SCHEMA(PROPS_2(PROP(clientId, string), PROP(register, boolean))REQUIRED_2(clientId,
                               register));

    if (!LSUtils::parsePayload(request.getPayload(), requestObj, schema, &parseError)) {
        PMSLOG_ERROR(MSGID_SCEMA_VAL_FAIL, 0, "wakeLockRegister schema validation failed");
        LSUtils::respondWithError(request, errorParseFailed, 0);
        return true;
    }

    std::string clientName = requestObj["clientId"].asString();
    std::string clientId = request.getUniqueToken();

    if (clientName.empty()) {
        LSUtils::respondWithError(request, errorInvalidParam, 0);
        return true;
    }

    if (mRefPms.isClientRegistered(clientId)) {
        LSUtils::respondWithError(request, errorInvalidParam, 0);
        return true;
    }

    // add the client to client list
    // mRefPms.getWakeLockManageRef()->addClient(clientId, clientName);

    bool isRegister = requestObj["register"].asBool();

    mRefPms.getWakeLockManageRef()->updateClientRegistered(clientId, isRegister);
    pbnjson::JValue responseObj = pbnjson::Object();
    responseObj.put("returnValue", true);
    LSUtils::postToClient(request, responseObj);
    return true;
}

bool SleepdPowerCategory::setWakeLock(LSMessage &message)
{
    LS::Message request(&message);
    pbnjson::JValue requestObj;
    int parseError = 0;
    pbnjson::JValue responseObj = pbnjson::Object();


    const std::string schema = RELAXED_SCHEMA(PROPS_3(PROP(clientId, string), PROP(timeout, integer), PROP(isWakeup,
                               boolean))REQUIRED_2(clientId,
                                       isWakeup));

    if (!LSUtils::parsePayload(request.getPayload(), requestObj, schema, &parseError)) {
        PMSLOG_ERROR(MSGID_SCEMA_VAL_FAIL, 0, "setWakeLock schema validation failed");
        LSUtils::respondWithError(request, errorParseFailed, 0);
        return true;
    }

    std::string clientId = requestObj["clientId"].asString();
    std::string sender = LSMessageGetSenderServiceName(&message);

    if (!mRefPms.getWakeLockManageRef()->isClientExist(clientId)) {
        LSUtils::respondWithError(request, errorInvalidClient, 0);
        return true;
    }

    bool wakeup = requestObj["isWakeup"].asBool();
    int32_t timeout = 0;
    bool isTimeout = false;

    if (requestObj.hasKey("timeout")) {
        timeout = (requestObj["timeout"].asNumber<int32_t>()) / 1000;
        isTimeout = true;
    }

    if (wakeup) {
        char kernelWakelock[512] = {0,};
        PMSLOG_DEBUG("setWakelock true");
        snprintf(kernelWakelock , 512 , "echo %s 1000000000 > /sys/power/wake_lock" , sender.c_str());
        system(kernelWakelock);

        if (mRefPms.setAwake(timeout, request, clientId, sender, isTimeout)) {
            responseObj.put("returnValue", true);
            LSUtils::postToClient(request, responseObj);
        }
    } else {
        PMSLOG_DEBUG("setWakelock false");
        mRefPms.getWakeLockManageRef()->clearWakelock(clientId);
        pms_support_notify_wakeup(mRefPms.getWakeLockManageRef()->getWakelockCount());
        responseObj.put("returnValue", true);
        LSUtils::postToClient(request, responseObj);
    }

    return true;
}

bool SleepdPowerCategory::clientCancelByName(LSMessage &message)
{
    return mRefPms.clientCancelByName(message);
}

bool SleepdPowerCategory::getFakeBatteryModeCallback(LSHandle *sh, LSMessage *message, void *ctx)
{
    bool retVal;
    LS::Message request(message);
    pbnjson::JValue requestObj;
    int parseError = 0;

    LSMessage *replyMessage = (LSMessage *)ctx;

    if (replyMessage && LSMessageGetConnection(replyMessage)) {
        bool ret = LSUtils::parsePayload(request.getPayload(), requestObj, SCHEMA_ANY, &parseError);

        if (ret) {
            bool FakeBatteryModeVal = requestObj["FakeBatteryMode"].asBool();
            char *payload = g_strdup_printf("{\"FakeBatteryMode\":%s}", FakeBatteryModeVal ? "true" : "false");

            retVal = LSMessageReply(LSMessageGetConnection(replyMessage), replyMessage, payload, NULL);

            if (!retVal) {
                PMSLOG_DEBUG("Could not send reply");

            }

            LSMessageUnref(replyMessage);
            g_free(payload);
        }
    } else {
        PMSLOG_DEBUG("reply message is null");
    }

    return true;
}

bool SleepdPowerCategory::chargerStatusCallback(LSHandle *sh, LSMessage *message, void *ctx)
{
    bool retVal;
    LS::Message request(message);
    pbnjson::JValue requestObj;
    int parseError = 0;
    pbnjson::JValue responseObj = pbnjson::Object();

    LSMessage *replyMessage = (LSMessage *)ctx;

    if (replyMessage && LSMessageGetConnection(replyMessage)) {
        bool ret = LSUtils::parsePayload(request.getPayload(), requestObj, SCHEMA_ANY, &parseError);

        if (ret) {

            std::string payload;
            responseObj.put("DockConnected", "false");
            responseObj.put("DockPower", "-1");
            responseObj.put("DockSerialNo", "-1");

            if (requestObj["type"].asString() == "usb") {
                responseObj.put("USBConnected", "true");
            } else {
                responseObj.put("USBConnected", "false");
            }

            responseObj.put("ACConnected", "false");
            responseObj.put("USBName", (requestObj["name"].asString()));
            responseObj.put("Charging", (requestObj["connected"].asBool() ? "true" : "false"));
            generatePayload(responseObj, payload);

            retVal = LSMessageReply(LSMessageGetConnection(replyMessage), replyMessage, payload.c_str(), NULL);

            if (!retVal) {
                PMSLOG_DEBUG("Could not send reply");

            }

            LSMessageUnref(replyMessage);
        }
    } else {
        PMSLOG_DEBUG("reply message is null");
    }

    return true;
}

bool SleepdPowerCategory::setFakeBatteryModeCallback(LSHandle *sh, LSMessage *message, void *ctx)
{
    bool retVal;
    LS::Message request(message);
    pbnjson::JValue requestObj;
    int parseError = 0;

    LSMessage *replyMessage = (LSMessage *)ctx;

    if (replyMessage && LSMessageGetConnection(replyMessage)) {
        bool ret = LSUtils::parsePayload(request.getPayload(), requestObj, SCHEMA_ANY, &parseError);

        if (ret) {
            bool FakeBatteryModeVal = requestObj["FakeBatteryMode"].asBool();
            char *payload = g_strdup_printf("{\"FakeBatteryMode\":%s}", FakeBatteryModeVal ? "true" : "false");

            retVal = LSMessageReply(LSMessageGetConnection(replyMessage), replyMessage, payload, NULL);

            if (!retVal) {
                PMSLOG_DEBUG("Could not send reply");

            }

            LSMessageUnref(replyMessage);
            g_free(payload);
        }
    } else {
        PMSLOG_DEBUG("reply message is null");
    }

    return true;
    /*
    bool retVal;
    LSMessage *replyMessage = (LSMessage *)ctx;

    if(replyMessage && LSMessageGetConnection(replyMessage))
    {
        retVal = LSMessageReply(LSMessageGetConnection(replyMessage), replyMessage, LSMessageGetPayload(message), NULL);
        if (!retVal)
        {
               PMSLOG_DEBUG("Could not send reply");
        }
        LSMessageUnref(replyMessage);
    }
    else

     PMSLOG_DEBUG("Could not send reply");

    return true;*/
}

bool SleepdPowerCategory::batteryStatusCallback(LSHandle *sh, LSMessage *message, void *userData)
{
    bool retVal;
    LS::Message request(message);
    LSMessage *replyMessage = (LSMessage *)userData;
    SleepdPowerCategory *ctx = reinterpret_cast<SleepdPowerCategory *>(userData);
    pbnjson::JValue requestObj;
    pbnjson::JValue responseObj = pbnjson::Object();
    int parseError = 0;

    bool ret = LSUtils::parsePayload(request.getPayload(), requestObj, SCHEMA_ANY, &parseError);

    if (ret) {
        responseObj.put("percent", (requestObj["percent"].asNumber<int32_t>()));
        responseObj.put("percent_ui", (requestObj["percent_ui"].asNumber<int32_t>()));
        responseObj.put("temperature_C", (requestObj["temperature_C"].asNumber<int32_t>()));
        responseObj.put("current_mA", (requestObj["current__mA"].asNumber<int32_t>()));
        responseObj.put("voltage_mV", (requestObj["voltage_mV"].asNumber<int32_t>()));

        responseObj.put("capacity_mAh", (requestObj["capacity_mAh"].asNumber<int32_t>()));
        responseObj.put("health", true);
        responseObj.put("charging", ctx->mIsChargerPresent ? "true" : "false");
        std::string payload;
        generatePayload(responseObj, payload);

        if (replyMessage && LSMessageGetConnection(replyMessage)) {
            retVal = LSMessageReply(LSMessageGetConnection(replyMessage), replyMessage, payload.c_str(), NULL);

            if (!retVal) {
                PMSLOG_DEBUG("Could not send reply");
            }

            LSMessageUnref(replyMessage);
        } else {
            PMSLOG_DEBUG("Could not send reply");
        }
    }

    return true;
}
