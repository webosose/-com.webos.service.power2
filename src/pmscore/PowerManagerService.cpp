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

#include <string>       // std::string
#include <sstream>      // std::stringstream
#include <cstdio>

#include "PmsLuna2Utils.h"
#include "PmsLogging.h"
#include "PowerManagerService.h"
#include "ShutdownCategoryMethods.h"
#include "PMSCommon.h"
#include "WakelockClientsMgrImpl.h"

void sendSuspendSignal(int type);
void sendResumeSignal(int resumeType);
LSHandle *gHandle = nullptr;


#ifdef SLEEPD_BACKWARD_COMPATIBILITY
LSHandle *gSleepdLsHandle = nullptr;
LSHandle *gDisplayLsHandle = nullptr;
LSHandle *gPowerdLsHandle = nullptr;

bool gSuspendedState = false;
const std::string powerdServiceUri = "com.palm.power";

#endif

static const std::string uriAlarmClear("palm://com.webos.service.alarm/clear");
static const std::string uriNotifyAlarmExpiry("palm://com.webos.service.power2/notifyAlarmExpiry");
static const std::string uriAlarmSet("palm://com.webos.service.alarm/set");

enum PowerManagerServiceConsts {
    MAX_TIME_FORMAT_SIZE = 9,
    MAX_PARAMS_SIZE = 100
};

PowerManagerService::PowerManagerService(GMainLoop *mainLoop) : LS::Handle(LS::registerService(serviceUri.c_str())),
    mLoopdata(mainLoop)
#ifdef SLEEPD_BACKWARD_COMPATIBILITY
    , mSleepdHandle(LS::registerService(sleepdUri.c_str())),
    mDisplayHandle(LS::registerService(displayUri.c_str()))
#endif
{
    LS_CREATE_CATEGORY_BEGIN(PowerManagerService, rootAPI)
    LS_CATEGORY_METHOD(registerClient)
    LS_CATEGORY_METHOD(setKeepAwake)
    LS_CATEGORY_METHOD(clearKeepAwake)
    LS_CATEGORY_METHOD(notifyAlarmExpiry)
    LS_CATEGORY_METHOD(clientCancelByName)
    LS_CREATE_CATEGORY_END

    static const LSSignal pmsSignals[] = {
        { "suspend" },
        { "resume" },
        { },
    };

    try {
        this->registerCategory("/", LS_CATEGORY_TABLE_NAME(rootAPI), pmsSignals, nullptr);
        this->setCategoryData("/", this);
        PMSLOG_INFO(MSGID_SHUTDOWN_DEBUG, 0, "%s, %s, root category registration is success**", __FILE__, __FUNCTION__);
    } catch (LS::Error &lunaError) {
        PMSLOG_ERROR(MSGID_CATEGORY_REG_FAIL, 0, "could not register root category");
    }

    gHandle = this->get();
#ifdef SLEEPD_BACKWARD_COMPATIBILITY
    gSleepdLsHandle = mSleepdHandle.get();
    gDisplayLsHandle = mDisplayHandle.get();
#endif
}


PowerManagerService::~PowerManagerService()
{
    pms_support_release();
    delete mShutdownCategoryHandle;
    delete mWakelocksMgr;
}

void PowerManagerService::shutDown()
{
    g_main_loop_quit(mLoopdata);
}

bool PowerManagerService::init()
{
    mShutdownCategoryHandle = new ShutdownCategoryMethods(*this);
    mWakelocksMgr = new WakelockClientsMgrImpl();

    try {
        mPowerdHandle = LS::registerService(powerdServiceUri.c_str());
        gPowerdLsHandle = mPowerdHandle.get();
        mIsPowerdRegistered = true;
        PMSLOG_INFO(MSGID_SHUTDOWN_DEBUG, 0, "%s, %s, powerd registration is success**", __FILE__, __FUNCTION__);
    } catch (LS::Error &lunaError) {
        mIsPowerdRegistered = false;
        PMSLOG_ERROR(MSGID_CATEGORY_REG_FAIL, 0, "could not register powerd ");
    }

    if (!mShutdownCategoryHandle || !mShutdownCategoryHandle->init() || !mWakelocksMgr) {
        PMSLOG_ERROR(MSGID_MEM_ALLOC_FAIL, 0, "memory allocation error");
        return false;
    }

    setSubscriptionCancelCallback();

    if (!initPmSupportInterface()) {
        return false;
    }

    return true;
}

bool PowerManagerService::initPmSupportInterface()
{
    mSupportCallback.resume_cb = ::sendResumeSignal;
    mSupportCallback.suspend_cb = ::sendSuspendSignal;

#ifdef SLEEPD_BACKWARD_COMPATIBILITY

    if (!pms_support_init2(this->get(), mDisplayHandle.get(), mSleepdHandle.get(), &mSupportCallback, this)) {
        return false;
    }

#else

    if (!pms_support_init(this->get(), &mSupportCallback, this)) {
        return false;
    }

#endif

    return true;
}

void sendSuspendSignal(int type)
{
    bool retVal = false;
    LSError lserror;

#ifdef SLEEPD_BACKWARD_COMPATIBILITY
    LSErrorInit(&lserror);
    gSuspendedState = true;
    retVal = LSSignalSend(gSleepdLsHandle,
                          "luna://com.palm.sleep/com/palm/power/suspended",
                          "{}", &lserror);

    if (!retVal) {
        PMSLOG_ERROR(MSGID_SUSPEND_SIG_FAIL, 1, PMLOGKS(ERRTEXT, lserror.message), "could not send suspend signal(sleepd)");
    }

    LSErrorFree(&lserror);
#endif
    LSErrorInit(&lserror);
    retVal = LSSignalSend(gHandle,
                          "luna://com.webos.service.power2/suspend",
                          "{}", &lserror);

    if (!retVal) {
        PMSLOG_ERROR(MSGID_SUSPEND_SIG_FAIL, 1, PMLOGKS(ERRTEXT, lserror.message), "could not send suspend signal");
    }

    LSErrorFree(&lserror);
    PMSLOG_DEBUG("sendSuspendSignal");
}

void sendResumeSignal(int resumeType)
{
    bool retVal = false;
    PMSLOG_DEBUG("sendResumeSignal--> resumeType = %d", resumeType);
    LSError lserror;
    char *payload = g_strdup_printf(
                        "{\"resumetype\":%d}", resumeType);

    if (!payload) {
        PMSLOG_ERROR(MSGID_MEM_ALLOC_FAIL, 0, "memory allocation error");
        return;
    }

#ifdef SLEEPD_BACKWARD_COMPATIBILITY
    LSErrorInit(&lserror);
    gSuspendedState = false;
    retVal = LSSignalSend(gSleepdLsHandle,
                          "luna://com.palm.sleep/com/palm/power/resume",
                          payload, &lserror);

    if (!retVal) {
        PMSLOG_ERROR(MSGID_RESUME_SIG_FAIL, 1, PMLOGKS(ERRTEXT, lserror.message), "could not send resume signal(sleepd)");
    }

    LSErrorFree(&lserror);
#endif
    retVal = LSSignalSend(gHandle,
                          "luna://com.webos.service.power2/resume",
                          payload, &lserror);

    LSErrorInit(&lserror);

    if (!retVal) {
        PMSLOG_ERROR(MSGID_RESUME_SIG_FAIL, 1, PMLOGKS(ERRTEXT, lserror.message), "could not send resume signal");
    }

    LSErrorFree(&lserror);
    g_free(payload);
}

#ifdef SLEEPD_BACKWARD_COMPATIBILITY
bool PowerManagerService::isSuspendedState()
{
    return gSuspendedState;
}
#endif

/**
@brief keep the device awake for specified number of seconds.
Precondition: client should be registered with registerClient luna api.

@par Parameters
Name        | Required |   Type  | Description
------------|----------|---------|----------
clientId    | Yes      | String  | clientId returned by registerClient API
timeout     | Yes      | Integer | no. of seconds that the device should be kept awake

@par Returns(Call)

Name        | Required |   Type  | Description
------------|----------|---------|----------
returnValue | Yes      | Boolean | Returns true. Return false if there is json parse error
errorText   | No       | String  | error string that describes the error

@example
luna-send -n 1 luna://com.webos.service.power2/setKeepAwake '{"clientId" : "/var/run/ls2/6xrG0z.4", "timeout" : 60}'
{
    "returnValue": true
}
*/
bool PowerManagerService::setKeepAwake(LSMessage &message)
{
    LS::Message request(&message);
    pbnjson::JValue responseObj = pbnjson::Object();
    pbnjson::JValue requestObj;
    int parseError = 0;

    const std::string schema = STRICT_SCHEMA(PROPS_2(PROP(clientId, string), PROP(timeout, integer))REQUIRED_2(clientId,
                               timeout));

    if (!LSUtils::parsePayload(request.getPayload(), requestObj, schema, &parseError)) {
        PMSLOG_ERROR(MSGID_SCEMA_VAL_FAIL, 0, "identify schema validation failed");
        LSUtils::respondWithError(request, errorParseFailed, 0);
        return true;
    }

    std::string sender = LSMessageGetSenderServiceName(&message);

    if (!LSUtils::parsePayload(request.getPayload(), requestObj, schema, &parseError)) {
        PMSLOG_ERROR(MSGID_SCEMA_VAL_FAIL, 0, "setKeepAwake schema validation failed");
        LSUtils::respondWithError(request, errorParseFailed, 0);
        return true;
    }

    int timeout = requestObj["timeout"].asNumber<int32_t>();
    std::string clientId = requestObj["clientId"].asString();

    char kernelWakelock[512] = {0,};
    snprintf(kernelWakelock , 512 , "echo %s 1000000000 > /sys/power/wake_lock" , sender.c_str());
    system(kernelWakelock);

    if (setAwake(timeout, request, clientId, sender)) {
        responseObj.put("returnValue", true);
        LSUtils::postToClient(request, responseObj);
    }

    return true;
}

bool PowerManagerService::setAwake(int timeout, LS::Message &request, std::string clientId, std::string sender,
                                   bool isTimeout)
{
    const int maxSeconds = (24 * 60 * 60) - 1; // max duration: 23:59:59

    // if clientId is invalid or wakelock is set already or timeout is invalid
    // return error
    if (isTimeout) {
        if ((timeout < 1 || timeout > maxSeconds) || (!mWakelocksMgr->isClientExist(clientId))) {
            PMSLOG_WARNING(MSGID_SET_KEEP_AWAKE, 0, "invalid timeout/clientId");
            LSUtils::respondWithError(request, errorInvalidParam, 0);
            return false;
        }
    } else if (!mWakelocksMgr->isClientExist(clientId)) {
        PMSLOG_WARNING(MSGID_SET_KEEP_AWAKE, 0, "invalid clientId");
        LSUtils::respondWithError(request, errorInvalidParam, 0);
        return false;
    }

    if (mWakelocksMgr->isWakelockSet(clientId)) {
        PMSLOG_WARNING(MSGID_SET_KEEP_AWAKE, 0, "wakelock is already set");
        LSUtils::respondWithError(request, errorKeepAwakeSet, 0);
        return false;
    }

    //set the alarm. clientId will be used as key
    if (setAlarm(clientId, timeout)) {
        mWakelocksMgr->setWakelock(clientId, timeout);
        //notify to library
        pms_support_notify_wakeup(mWakelocksMgr->getWakelockCount());
        PMSLOG_DEBUG("wakelock is set");
    } else {
        LSUtils::respondWithError(request, errorInternalError, 0);
        return false;
    }

    std::stringstream kernelWakelock;
    kernelWakelock << "echo " << sender << " 1000000000 > /sys/power/wake_lock";
    system(kernelWakelock.str().c_str());

    return true;
}

bool PowerManagerService::setAlarm(const std::string &key, int timeout)
{
    char time[MAX_TIME_FORMAT_SIZE] = {'\0'};
    char params[MAX_PARAMS_SIZE] = {'\0'};
    int HH = 0;
    int MM = 0;
    int SS = 0;

    SS = timeout;
    MM = SS / 60;
    SS = SS % 60;
    HH = MM / 60;
    MM = MM % 60;

    std::sprintf(time, "%02d:%02d:%02d", HH, MM, SS);
    PMSLOG_DEBUG("timeout : %s", time);

    pbnjson::JValue jsonObject = pbnjson::Object();
    jsonObject.put("in", time);
    jsonObject.put("key", key);
    jsonObject.put("uri", uriNotifyAlarmExpiry);
    jsonObject.put("wakeup", true);

    std::sprintf(params, "{\"clientId\":\"%s\"}", key.c_str());
    jsonObject.put("params", params);

    std::string alarmSetPayload;

    if (!LSUtils::generatePayload(jsonObject, alarmSetPayload)) {
        PMSLOG_DEBUG("Error while generating the payload");
        return false;
    }

    PMSLOG_DEBUG("setAlarm payload : %s", alarmSetPayload.c_str());
    this->callOneReply(uriAlarmSet.c_str(), alarmSetPayload.c_str());
    return true;
}

/**
@brief clear the wakelock.

@par Parameters
Name        | Required |   Type  | Description
------------|----------|---------|----------
clientId    | Yes      | String  | clientId returned by registerClient API

@par Returns(Call)

Name        | Required |   Type  | Description
------------|----------|---------|----------
returnValue | Yes      | Boolean | Returns true. Return false if there is json parse error
errorText   | No       | String  | error string that describes the error

@example
luna-send -n 1 luna://com.webos.service.power2/clearKeepAwake '{"clientId" : "/var/run/ls2/6xrG0z.4"}'
{
    "returnValue": true
}
*/
bool PowerManagerService::clearKeepAwake(LSMessage &message)
{
    LS::Message request(&message);
    pbnjson::JValue responseObj = pbnjson::Object();
    pbnjson::JValue requestObj;
    int parseError = 0;

    const std::string schema = STRICT_SCHEMA(PROPS_1(PROP(clientId, string))REQUIRED_1(clientId));

    if (!LSUtils::parsePayload(request.getPayload(), requestObj, schema, &parseError)) {
        PMSLOG_ERROR(MSGID_SCEMA_VAL_FAIL, 0, "clearKeepAwake schema validation failed");
        LSUtils::respondWithError(request, errorParseFailed, 0);
        return true;
    }

    std::string clientId;

    /********** clientId ***********/
    clientId = requestObj["clientId"].asString();

    // if client doesn't exist return error
    if (!mWakelocksMgr->isClientExist(clientId)) {
        LSUtils::respondWithError(request, errorInvalidClient, 0);
        return true;
    }

    // if wakelock is not set return error
    if (!mWakelocksMgr->isWakelockSet(clientId)) {
        std::string errorNoWakelock("wakelock is not set");
        LSUtils::respondWithError(request, errorNoWakelock, 0);
        return true;
    }

    // clear the alarm and clear the wakelock
    clearAlarm(clientId);
    mWakelocksMgr->clearWakelock(clientId);
    pms_support_notify_wakeup(mWakelocksMgr->getWakelockCount());
    responseObj.put("returnValue", true);
    LSUtils::postToClient(request, responseObj);

    return true;
}

bool PowerManagerService::clearAlarm(const std::string &key)
{
    pbnjson::JValue jsonObject = pbnjson::Object();
    jsonObject.put("key", key);
    jsonObject.put("appId", "appclear");

    std::string alarmClearPayload;

    if (!LSUtils::generatePayload(jsonObject, alarmClearPayload)) {
        PMSLOG_ERROR(MSGID_GENERAL, 0, "Error while generating the payload");
        return false;
    }

    PMSLOG_DEBUG("alarmclear payload : %s", alarmClearPayload.c_str());
    this->callOneReply(uriAlarmClear.c_str(), alarmClearPayload.c_str());
    return true;
}

/**
@brief Register a client, so that it can set the wakelock.

@par Parameters
Name        | Required |   Type  | Description
------------|----------|---------|----------
clientName  | Yes      | String  | name of the client

@par Returns(Call)

Name        | Required |   Type  | Description
------------|----------|---------|----------
clientId    | Yes      | String  | unique clientId identifying the registered client
returnValue | Yes      | Boolean | Returns true. Return false if there is json parse error
errorText   | No       | String  | error string that describes the error

@example
luna-send -i -f luna://com.webos.service.power2/registerClient '{"clientName" : "com.palm.dummy"}'
{
    "clientId": "/var/run/ls2/WIFvNE.4",
    "returnValue": true
}
*/
bool PowerManagerService::registerClient(LSMessage &message)
{
    LS::Message request(&message);
    pbnjson::JValue responseObj = pbnjson::Object();
    pbnjson::JValue requestObj;
    int parseError = 0;
    const std::string schema = STRICT_SCHEMA(PROPS_1(PROP(clientName, string))REQUIRED_1(clientName));

    if (!LSUtils::parsePayload(request.getPayload(), requestObj, schema, &parseError)) {
        PMSLOG_ERROR(MSGID_SCEMA_VAL_FAIL, 0, "registerClient schema validation failed");
        LSUtils::respondWithError(request, errorParseFailed, 0);
        return true;
    }

    //get the clientId
    std::string clientName = requestObj["clientName"].asString();

    std::string clientId = request.getUniqueToken();

    if (clientName.empty()) {
        LSUtils::respondWithError(request, errorInvalidParam, 0);
        return true;
    }

    addSubscription(message);
    // add the client to client list
    mWakelocksMgr->addClient(clientId, clientName);
    mWakelocksMgr->updateClientRegistered(clientId, true);

    PMSLOG_DEBUG("[%s] client registered. clientName[%s] clientId[%s]",
                 __FUNCTION__, clientName.c_str(), clientId.c_str());


    //return response
    responseObj.put("returnValue", true);
    responseObj.put("clientId", clientId);
    LSUtils::postToClient(request, responseObj);

    return true;
}

bool PowerManagerService::notifyAlarmExpiry(LSMessage &message)
{
    LS::Message request(&message);
    pbnjson::JValue responseObj = pbnjson::Object();
    pbnjson::JValue requestObj;
    int parseError = 0;
    const std::string schema = STRICT_SCHEMA(PROPS_1(PROP(clientId, string))REQUIRED_1(clientId));

    if (!LSUtils::parsePayload(request.getPayload(), requestObj, schema, &parseError)) {
        PMSLOG_ERROR(MSGID_SCEMA_VAL_FAIL, 0, "notifyAlarmExpiry schema validation failed");
        LSUtils::respondWithError(request, errorParseFailed, 0);
        return true;
    }

    //get the clientId
    std::string clientId = requestObj["clientId"].asString();

    PMSLOG_DEBUG("clientId[%s]", clientId.c_str());

    //if no wakelock is set, return error
    if (!mWakelocksMgr->isWakelockSet(clientId)) {
        LSUtils::respondWithError(request, errorInvalidClient, 0);
        return true;
    }

    mWakelocksMgr->clearWakelock(clientId);

    //notify to library
    pms_support_notify_wakeup(mWakelocksMgr->getWakelockCount());

    PMSLOG_DEBUG("[%s] . clientId[%s]", __FUNCTION__, clientId.c_str());

    //return response
    responseObj.put("returnValue", true);
    responseObj.put("clientId", clientId);
    LSUtils::postToClient(request, responseObj);
    return true;
}

void PowerManagerService::addSubscription(LSMessage &message)
{
    bool retVal = false;
    LSError lserror;
    LSErrorInit(&lserror);

    retVal = LSSubscriptionAdd(this->get(), "PwrEventsClients",
                               &message, &lserror);

    if (!retVal) {
        PMSLOG_ERROR(MSGID_LSSUBSCRI_ADD_FAIL, 1, PMLOGKS(ERRTEXT, lserror.message), "LSSubscriptionAdd failed");
        LSErrorFree(&lserror);
    }
}

void PowerManagerService::setSubscriptionCancelCallback()
{
    bool retVal = false;
    LSError lserror;
    LSErrorInit(&lserror);

    retVal = LSSubscriptionSetCancelFunction(this->get(),
             clientSubscriptionCancel, this, &lserror);

    if (!retVal) {
        PMSLOG_ERROR(MSGID_LS_SUBSCRIB_SETFUN_FAIL, 1, PMLOGKS(ERRTEXT, lserror.message),
                     "Error in setting subscription cancel function");
    }

    LSErrorFree(&lserror);

#ifdef SLEEPD_BACKWARD_COMPATIBILITY
    LSErrorInit(&lserror);

    retVal = LSSubscriptionSetCancelFunction(mSleepdHandle.get(),
             clientSubscriptionCancel, this, &lserror);

    if (!retVal) {
        PMSLOG_ERROR(MSGID_LS_SUBSCRIB_SETFUN_FAIL, 1, PMLOGKS(ERRTEXT, lserror.message),
                     "Error in setting subscription cancel function");
    }

    retVal = LSSubscriptionSetCancelFunction(mDisplayHandle.get(),
             cancelSubscription, this, &lserror);

    if (!retVal) {
        PMSLOG_ERROR(MSGID_LS_SUBSCRIB_SETFUN_FAIL, 1, PMLOGKS(ERRTEXT, lserror.message),
                     "Error in setting Display subscription cancel function");
        LSErrorFree(&lserror);
    }

    LSErrorFree(&lserror);
#endif

}

#ifdef SLEEPD_BACKWARD_COMPATIBILITY
bool PowerManagerService::cancelSubscription(LSHandle *sh, LSMessage *msg, void *ctx)
{
    LSError lserror;
    LS::Message request(msg);
    pbnjson::JValue requestObj;
    int parseError = 0;
    bool success = false;
    LSErrorInit(&lserror);

    if (!LSUtils::parsePayload(request.getPayload(), requestObj, SCHEMA_ANY, &parseError)) {
        PMSLOG_DEBUG("cancel subscription schema failed");
        return true;
    }

    PMSLOG_DEBUG("pms-display subscription cancel function is called");

    if (!strcmp(LSMessageGetMethod(msg), "dimModeEnable") &&
        (!strcmp(LSMessageGetCategory(msg), "/control") || !strcmp(LSMessageGetCategory(msg), "/"))) {
        std::string clientName = requestObj["client"].asString();
        char *payload = g_strdup_printf("{\"client\":\"%s\"}", clientName.c_str());
        PMSLOG_DEBUG("pms-display subscription cancel function is called %d", gDisplayLsHandle);
        success = LSCallOneReply(gDisplayLsHandle, "palm://com.webos.service.power2/wearable/dimModeDisable", payload,
                                 PowerManagerService::dimModeDisableCallback,
                                 NULL, NULL, &lserror);

        if (!success) {
            PMSLOG_DEBUG("cancelSubscription failed");
        }

        if (payload) {
            g_free(payload);
        }
    }

    LSErrorFree(&lserror);
    return true;
}

bool PowerManagerService::dimModeDisableCallback(LSHandle *sh, LSMessage *message, void *ctx)
{
    PMSLOG_DEBUG("%s", __FUNCTION__);
    return true;
}
#endif

bool PowerManagerService::clientSubscriptionCancel(LSHandle *sh, LSMessage *msg, void *ctx)
{
    PMSLOG_DEBUG("subscription cancel function is called");
    PowerManagerService *ptrHandle = static_cast<PowerManagerService *>(ctx);
    std::string clientId = LSMessageGetUniqueToken(msg);
    ptrHandle->deregisterClient(clientId);
    cancelSubscription(sh, msg, ctx);
    return true;
}

void PowerManagerService::deregisterClient(const std::string &clientId)
{
    mShutdownCategoryHandle->deregisterAppsServicesClient(clientId);
    mWakelocksMgr->removeClient(clientId);
}

ShutdownCategoryMethods &PowerManagerService::getShutdownCategoryHandle()
{
    return *mShutdownCategoryHandle;
}

#ifdef SLEEPD_BACKWARD_COMPATIBILITY
LS::Handle &PowerManagerService::getSleepdLsHandle()
{
    return mSleepdHandle;
}

LS::Handle &PowerManagerService::getPowerdLsHandle()
{
    return mPowerdHandle;
}

LS::Handle &PowerManagerService::getDisplayLsHandle()
{
    return mDisplayHandle;
}



bool PowerManagerService::clientCancelByName(LSMessage &message)
{
    LS::Message request(&message);
    pbnjson::JValue responseObj = pbnjson::Object();
    pbnjson::JValue requestObj;
    int parseError = 0;
    const std::string schema = RELAXED_SCHEMA(PROPS_1(PROP(clientName, string)));

    if (!LSUtils::parsePayload(request.getPayload(), requestObj, schema, &parseError)) {
        PMSLOG_ERROR(MSGID_SCEMA_VAL_FAIL, 0, "clientCancelByName schema validation failed");
        LSUtils::respondWithError(request, errorParseFailed, 0);
        return true;
    }

    /********** clientName ***********/
    std::string clientName = requestObj["clientName"].asString();
    PMSLOG_DEBUG("clientCancelByName called --> client = %s", clientName.c_str());
    mWakelocksMgr->removeClientByName(clientName);

    responseObj.put("returnValue", true);
    LSUtils::postToClient(request, responseObj);
    return true;
}

bool PowerManagerService::isClientRegistered(const std::string &clientId)
{
    return mWakelocksMgr->isClientExist(clientId);
}
#endif

