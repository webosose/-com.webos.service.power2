// @@@LICENSE
//
//      Copyright (c) 2017 LG Electronics, Inc.
//
// Confidential computer software. Valid license from LG required for
// possession, use or copying. Consistent with FAR 12.211 and 12.212,
// Commercial Computer Software, Computer Software Documentation, and
// Technical Data for Commercial Items are licensed to the U.S. Government
// under vendor's standard commercial license.
//
// LICENSE@@@

#include <errno.h>
#include <fstream>
#include <sys/inotify.h>
#include <sys/time.h>
#include <signal.h>
#include <string>

#include "LunaPmsRoot.h"
#include "pmscore/LunaInterfaceCommon.h"
#include "pmscore/PowerStateClientsMgrImpl.h"

const char* const LunaPmsRoot::kPmsInterfaceName = "LunaInterfacePMSRoot";
const char* const LunaPmsRoot::kPmsLogContext = "LogContext";
const char* const LunaPmsRoot::kPmsTimeOut = "TimeOut";

const char* LunaPmsRoot::mpLogContext = "";

std::string LunaPmsRoot::mPowerOnReason = "Not Set";
bool LunaPmsRoot::mIsTransitionState = false;
int LunaPmsRoot::mpLogTimeOutSec = 0;

#define CALLBACK_BIND(function) (std::bind1st(std::mem_fun(function), this))
#define TIMER_CHECK_TIMEOUT      3
#define NOTIFY_WITH_ALARM        0

/**
 * Register with the factory object
 */
bool LunaPmsRoot::mIsObjRegistered = LunaPmsRoot::RegisterObject();
unsigned int LunaPmsRoot::subscribersCount = 0;

/**
 * The methods table, add more commands and their handlers.
 * Note these are registered with Cb methods.
 */
LSMethod LunaPmsRoot::mStateMethodsTable[] =
{
    { "getPowerOnReason",                LunaPmsRoot::getPowerOnReason            },
    { "getState",                        LunaPmsRoot::getState                    },
    { "shutdown",                        LunaPmsRoot::shutdown                    },
    { "reboot",                          LunaPmsRoot::reboot                      },
    { "registerStateTransition",         LunaPmsRoot::registerStateTransition     },
    { "respondStateTransitionAck",       LunaPmsRoot::respondStateTransitionAck   },
    { "setPowerOnReason",                LunaPmsRoot::setPowerOnReason            },
    { "setState",                        LunaPmsRoot::setState                    },
    { "notifyAlarmExpiry",               LunaPmsRoot::notifyAlarmExpiry           },
    { "acquireWakeLock",                 LunaPmsRoot::acquireWakeLock             },
    { "releaseWakeLock",                 LunaPmsRoot::releaseWakeLock             },
    { },
};

enum PowerManagerServiceConsts {
    MAX_TIME_FORMAT_SIZE = 9,
    MAX_PARAMS_SIZE = 100
};

LunaPmsRoot::LunaPmsRoot(PmsConfig* pConfig, LSHandle *pLsHandle) :
    LunaInterfaceBase(pConfig, pLsHandle)
{
    std::string logContext, timeContext;

    mpConfig = pConfig;

    PmsErrorCode_t err = kPmsSuccess;
    const char* logCtxt = LOG_CONTEXT_DEBUG; //default logging context

    err = mpConfig->GetString(kPmsInterfaceName,
    kPmsLogContext, &logContext);

    if(err == kPmsSuccess)
    logCtxt = logContext.c_str();

    mpLogContext = logCtxt;

    mpConfig->GetString(kPmsInterfaceName,
    kPmsTimeOut, &timeContext);

    char lastChar = timeContext.back();
    mpLogTimeOutSec = lastChar - '0';

    mpLog = new Logger(logCtxt);

    mLoopdata = g_main_loop_new(NULL, FALSE);

    // register the IPC methods and notification callbacks with Client
    RegisterIpc(pLsHandle);
}

void LunaPmsRoot::RegisterIpc(LSHandle *pLsHandle)
{
    PMSLunaCategoryContext *pCxt = PMSLunaCategoryContext::Instance();
    pCxt->AddLunaCategoryContext(kPmsMsgCategoryRoot, (unsigned int)this);

    LSError lserror;
    LSErrorInit(&lserror);

    if (!LSRegisterCategoryAppend(pLsHandle, "/", mStateMethodsTable, NULL, &lserror))
    {
        LSErrorPrint(&lserror, stderr);
        LSErrorFree(&lserror);
        return;
    }
    setDbKind();

    mPowerStatesMgr = new PowerStateClientsMgrImpl();
    stateReference::GetInstance().registerListener(this);
    return;
}

bool LunaPmsRoot::setState(LSHandle *sh, LSMessage *message, void *data)
{
    PMSLunaCategoryContext *pCxt = PMSLunaCategoryContext::Instance();
    LunaPmsRoot *pThis = (LunaPmsRoot *)(pCxt->GetLunaCategoryContext(kPmsMsgCategoryRoot));

    return pThis->setStateCb(sh, message, data);
}

bool LunaPmsRoot::getState(LSHandle *sh, LSMessage *message, void *data)
{
    PMSLunaCategoryContext *pCxt = PMSLunaCategoryContext::Instance();
    LunaPmsRoot *pThis = (LunaPmsRoot *)(pCxt->GetLunaCategoryContext(kPmsMsgCategoryRoot));

    return pThis->getStateCb(sh, message, data);
}

bool LunaPmsRoot::respondStateTransitionAck(LSHandle *sh, LSMessage *message, void *data)
{
    PMSLunaCategoryContext *pCxt = PMSLunaCategoryContext::Instance();
    LunaPmsRoot *pThis = (LunaPmsRoot *)(pCxt->GetLunaCategoryContext(kPmsMsgCategoryRoot));

    return pThis->respondStateTransitionAckCb(sh, message, data);
}

bool LunaPmsRoot::reboot(LSHandle *sh, LSMessage *message, void *data)
{
    PMSLunaCategoryContext *pCxt = PMSLunaCategoryContext::Instance();
    LunaPmsRoot *pThis = (LunaPmsRoot *)(pCxt->GetLunaCategoryContext(kPmsMsgCategoryRoot));

    return pThis->rebootCb(sh, message, data);
}

bool LunaPmsRoot::registerStateTransition(LSHandle *sh, LSMessage *message, void *data)
{
    PMSLunaCategoryContext *pCxt = PMSLunaCategoryContext::Instance();
    LunaPmsRoot *pThis = (LunaPmsRoot *)(pCxt->GetLunaCategoryContext(kPmsMsgCategoryRoot));

    return pThis->registerStateTransitionCb(sh, message, data);
}

bool LunaPmsRoot::shutdown(LSHandle *sh, LSMessage *message, void *data)
{
    PMSLunaCategoryContext *pCxt = PMSLunaCategoryContext::Instance();
    LunaPmsRoot *pThis = (LunaPmsRoot *)(pCxt->GetLunaCategoryContext(kPmsMsgCategoryRoot));

    return pThis->shutdownCb(sh, message, data);
}

bool LunaPmsRoot::setPowerOnReason(LSHandle *sh, LSMessage *message, void *data)
{
    PMSLunaCategoryContext *pCxt = PMSLunaCategoryContext::Instance();
    LunaPmsRoot *pThis = (LunaPmsRoot *)(pCxt->GetLunaCategoryContext(kPmsMsgCategoryRoot));

    return pThis->setPowerOnReasonCb(sh, message, data);
}

bool LunaPmsRoot::getPowerOnReason(LSHandle *sh, LSMessage *message, void *data)
{
    PMSLunaCategoryContext *pCxt = PMSLunaCategoryContext::Instance();
    LunaPmsRoot *pThis = (LunaPmsRoot *)(pCxt->GetLunaCategoryContext(kPmsMsgCategoryRoot));

    return pThis->getPowerOnReasonCb(sh, message, data);
}

bool LunaPmsRoot::notifyAlarmExpiry(LSHandle *sh, LSMessage *message, void *data)
{
    PMSLunaCategoryContext *pCxt = PMSLunaCategoryContext::Instance();
    LunaPmsRoot *pThis = (LunaPmsRoot *)(pCxt->GetLunaCategoryContext(kPmsMsgCategoryRoot));

    return pThis->notifyAlarmExpiryCb(sh, message, data);
}

bool LunaPmsRoot::acquireWakeLock(LSHandle *sh, LSMessage *message, void *data)
{
    PMSLunaCategoryContext *pCxt = PMSLunaCategoryContext::Instance();
    LunaPmsRoot *pThis = (LunaPmsRoot *)(pCxt->GetLunaCategoryContext(kPmsMsgCategoryRoot));

    return pThis->acquireWakeLockCb(sh, message, data);
}

bool LunaPmsRoot::releaseWakeLock(LSHandle *sh, LSMessage *message, void *data)
{
    PMSLunaCategoryContext *pCxt = PMSLunaCategoryContext::Instance();
    LunaPmsRoot *pThis = (LunaPmsRoot *)(pCxt->GetLunaCategoryContext(kPmsMsgCategoryRoot));

    return pThis->releaseWakeLockCb(sh, message, data);
}

bool LunaPmsRoot::setStateCb(LSHandle *sh, LSMessage *message, void *data)
{
    LS::Message request(message);
    pbnjson::JValue requestObj;
    int parseError = 0;
    bool ret = false;

    const std::string schema = STRICT_SCHEMA(PROPS_2(PROP(state, string), PROP(reason, integer))REQUIRED_2(state, reason));

    if (!LSUtils::parsePayload(request.getPayload(), requestObj, schema, &parseError)) {
        LSUtils::respondWithError(request, errorInvalidJsonFormat, INVALID_JSON_FORMAT);
        return true;
    }

    pbnjson::JValue responseObj = pbnjson::Object();
    std::string stateName = requestObj["state"].asString();
    int inputReason = requestObj["reason"].asNumber<int>();

    if (inputReason >= REASON_WAKE_ON_LAN && inputReason < MAX_REASONS)
    {
        ret = forceStateChange(stateName);
    }

    MSG_DEBUG("State Set: %s", stateName.c_str());

    responseObj.put("returnValue", ret ? true : false);

    if(ret){
        MSG_DEBUG("State is set to %s", stateName.c_str());
    }
    else
    {
        LSUtils::respondWithError(request, errorUnknown, UNKNOWN_ERROR);
        MSG_DEBUG("Could not set the State  %s", stateName.c_str());
    }

    LSUtils::postToClient(request, responseObj);

    return true;
}

bool LunaPmsRoot::getStateCb(LSHandle *sh, LSMessage *message, void *data)
{
    LS::Message request(message);
    pbnjson::JValue requestObj;
    int parseError = 0;

    const std::string schema = RELAXED_SCHEMA(PROPS_1(PROP(subscribe, boolean)));

    if(!LSUtils::parsePayload(request.getPayload(), requestObj, schema, &parseError))
    {
        LSUtils::respondWithError(request, errorInvalidJsonFormat, INVALID_JSON_FORMAT);
        return true;
    }

    pbnjson::JValue responseObj = pbnjson::Object();

    MSG_DEBUG("Current State: %s", mCurrentState.c_str());

    if (LSMessageIsSubscription(message)) {
        addSubscription(sh, message, "powerState");
        responseObj.put("subscribed", true);
    } else {
        responseObj.put("subscribed", false);
    }

    responseObj.put("returnValue", true);
    responseObj.put("state", mCurrentState);


    LSUtils::postToClient(request, responseObj);

    return true;
}

bool LunaPmsRoot::registerStateTransitionCb(LSHandle *sh, LSMessage *message, void *data)
{
    LS::Message request(message);
    pbnjson::JValue requestObj;
    int parseError = 0;
    const std::string schema = STRICT_SCHEMA(PROPS_1(PROP(state, string))REQUIRED_1(state));
    if (!LSUtils::parsePayload(request.getPayload(), requestObj, schema, &parseError))
    {
        LSUtils::respondWithError(request, errorInvalidJsonFormat, INVALID_JSON_FORMAT);
        return true;
    }

    std::string clientName = LSMessageGetSenderServiceName(message);
    std::string stateName = requestObj["state"].asString();

    pbnjson::JValue responseObj = pbnjson::Object();

    if ((stateName == "ActiveState") || (stateName == "ActiveStandbyState")) {
        if (mPowerStatesMgr->isClientExist(mCurrentState, clientName))
        {
            LSUtils::respondWithError(request, errorCleintAlreadyRegistered, CLIENT_ALREADY_REGISTERED);
            return false;
        }
        if (!mIsTransitionState)
        {
            mPowerStatesMgr->addClient(stateName, clientName);
            mPowerStatesMgr->updateClientRegistered(stateName, true);
        }
        else
        {
            LSUtils::respondWithError(request, errorPower2Busy, POWER2_BUSY);
            return true;
        }
        addSubscription(sh, message, stateName);
    }

    RegisterCancelSubscriptionCallback(message, CALLBACK_BIND(&LunaPmsRoot::CancelSubscriptionStateChange));
    responseObj.put("returnValue", true);
    responseObj.put("subscribed", true);
    responseObj.put("clientId", clientName);
    responseObj.put("state", stateName);
    LSUtils::postToClient(request, responseObj);
    return true;
}

bool LunaPmsRoot::respondStateTransitionAckCb(LSHandle *sh, LSMessage *message, void *data)
{
    LS::Message request(message);
    pbnjson::JValue requestObj;
    int parseError = 0;

    PMSLunaCategoryContext *pCxt = PMSLunaCategoryContext::Instance();
    LunaPmsRoot *pThis = (LunaPmsRoot *)(pCxt->GetLunaCategoryContext(kPmsMsgCategoryRoot));

    const std::string schema = STRICT_SCHEMA(PROPS_3(PROP(ack, boolean), PROP(state, string), PROP(token, string))REQUIRED_3(ack, state,token));

    if (!LSUtils::parsePayload(request.getPayload(), requestObj, schema, &parseError)) {
        LSUtils::respondWithError(request, errorInvalidJsonFormat, INVALID_JSON_FORMAT);
        return true;
    }

    std::string clientName = LSMessageGetSenderServiceName(message);
    std::string stateName = requestObj["state"].asString();
    std::string sessionToken = requestObj["token"].asString();

    pbnjson::JValue responseObj = pbnjson::Object();

    if (stateName != mCurrentState || !mIsTransitionState) {
        responseObj.put("returnValue", false);
        responseObj.put("errorText", "Response in incorrect state");
        LSUtils::postToClient(request, responseObj);
        return true;
    }

    if(!(mPowerStatesMgr->isClientExist(stateName, clientName))) {
        LSUtils::respondWithError(request, errorClientNotRegistered, CLIENT_NOT_REGISTERED);
        responseObj.put("returnValue", false);
        LSUtils::postToClient(request, responseObj);
        return true;
    }

    if(sessionToken != mSessionToken)
    {
        MSG_DEBUG("Response not for current session!");
        LSUtils::respondWithError(request, errorResponseTooLate, RESPONSE_TOO_LATE);
        responseObj.put("returnValue", false);
        LSUtils::postToClient(request, responseObj);
        return true;
    }

    auto iter = capturedReplies.insert(clientName);

    if (false == iter.second) {
        responseObj.put("returnValue", false);
        responseObj.put("errorText", "Duplicate Response");
        LSUtils::postToClient(request, responseObj);
        return true;
    }

    bool ack = requestObj["ack"].asBool();

    if (ack)
    {
        LunaPmsRoot::subscribersCount++;
    }
    else
    {
    clearAlarm("notifyAlarmExpiry");
        capturedReplies.clear();
        stateReference::GetInstance().processEvent("NACK");
    responseObj.put("returnValue", false);
    LSUtils::postToClient(request, responseObj);

    /* send the pending response for shutDown method call */
        LS::Message pendingResponse(pThis->pendingMsg);
    pbnjson::JValue penResponseObj = pbnjson::Object();
    penResponseObj.put("returnValue", false);
    penResponseObj.put("errorText", "Cannot move to requested state");
    LSUtils::postToClient(pendingResponse, penResponseObj);
    LSMessageUnref(message);
        return true;
    }

    // TODO:
    // If Nack has come, notify statemanager to cancel timeouts if any and donot pass event to statemachine

    if (LunaPmsRoot::subscribersCount == mPowerStatesMgr->getPowerStateCount(stateName))
    {
        clearAlarm("notifyAlarmExpiry");
         //PMSLOG_DEBUG("All subscribers response received !!!subscriber_count = %d", subscriber_count);
         MSG_DEBUG("All subscribers response received !!! subscribersCount = %d", subscribersCount);
         stateReference::GetInstance().processEvent("ACK");

        /* send the pending response for shutDown method call */
        pbnjson::JValue penResponseObj = pbnjson::Object();
        LS::Message pendingResponse(pThis->pendingMsg);
        penResponseObj.put("returnValue", true);
        LSUtils::postToClient(pendingResponse, penResponseObj);
        LSMessageUnref(message);
    }

    mSessionToken.clear();
    responseObj.put("returnValue", true);
    LSUtils::postToClient(request, responseObj);

    return true;
}

bool LunaPmsRoot::rebootCb(LSHandle *sh, LSMessage *message, void *data)
{
    LS::Message request(message);
    pbnjson::JValue requestObj;
    pbnjson::JValue responseObj = pbnjson::Object();

    PMSLunaCategoryContext *pCxt = PMSLunaCategoryContext::Instance();
    LunaPmsRoot *pThis = (LunaPmsRoot *)(pCxt->GetLunaCategoryContext(kPmsMsgCategoryRoot));

    int parseError = 0;
    const std::string schema = STRICT_SCHEMA(PROPS_1(PROP(reason, string))REQUIRED_1(reason));

    if (!LSUtils::parsePayload(request.getPayload(), requestObj, schema, &parseError)) {
        LSUtils::respondWithError(request, errorInvalidJsonFormat, INVALID_JSON_FORMAT);
        return true;
    }

    std::string reason = requestObj["reason"].asString();

    if ((reason == "reset") ||(reason == "ota") || (reason == "SwDownload"))
    {
       std::string event  = "reboot";
       if (!handleEvent(event)) {
       responseObj.put("returnValue", false);
       LSUtils::respondWithError(request, errorPower2Busy, POWER2_BUSY);
       } else {
                  responseObj.put("returnValue", true);
              /* Dont send reply until Voting is done */
              LSMessageRef(message);
              pThis->pendingMsg = message;
              }
    } else {
               LSUtils::respondWithError(request, errorUnknown, UNKNOWN_ERROR);
           return false;
    }

    LSUtils::postToClient(request, responseObj);

    return true;
}

bool LunaPmsRoot::shutdownCb(LSHandle *sh, LSMessage *message, void *data)
{
    LS::Message request(message);
    pbnjson::JValue requestObj;

    PMSLunaCategoryContext *pCxt = PMSLunaCategoryContext::Instance();
    LunaPmsRoot *pThis = (LunaPmsRoot *)(pCxt->GetLunaCategoryContext(kPmsMsgCategoryRoot));

    int parseError = 0;
    const std::string schema = STRICT_SCHEMA(PROPS_1(PROP(reason, string))REQUIRED_1(reason));

    if (!LSUtils::parsePayload(request.getPayload(), requestObj, schema, &parseError)) {
        LSUtils::respondWithError(request, errorInvalidJsonFormat, INVALID_JSON_FORMAT);
        return true;
    }

    pbnjson::JValue responseObj = pbnjson::Object();
    std::string reason = requestObj["reason"].asString();

    if((reason == "remoteKey") || (reason == "offTimer") || (reason == "deviceKey") || (reason == "localKey"))
    {
        if (!handleEvent("poweroff")) {
            responseObj.put("returnValue", false);
        LSUtils::respondWithError(request, errorPower2Busy, POWER2_BUSY);
        } else {
                   responseObj.put("returnValue", true);
               /* Dont send reply until Voting is done */
               LSMessageRef(message);
               pThis->pendingMsg = message;
           }
    } else {
           LSUtils::respondWithError(request, errorUnknown, UNKNOWN_ERROR);
           return false;
    }

    LSUtils::postToClient(request, responseObj);
    return true;
}

bool LunaPmsRoot::getPowerOnReasonCb(LSHandle *sh, LSMessage *message, void *data)
{
    LS::Message request(message);
    pbnjson::JValue requestObj;
    int parseError = 0;

    const std::string schema = RELAXED_SCHEMA(PROPS_1(PROP(subscribe, boolean)));

    if (!LSUtils::parsePayload(request.getPayload(), requestObj, schema, &parseError)) {
        LSUtils::respondWithError(request, errorInvalidJsonFormat, INVALID_JSON_FORMAT);
        return true;
    }

    pbnjson::JValue responseObj = pbnjson::Object();
    std::string powerOnReason = mPowerOnReason;

    MSG_DEBUG("reason: %s", powerOnReason.c_str());

    if (LSMessageIsSubscription(message)) {
        addSubscription(sh, message, "powerOnReason");
        responseObj.put("subscribed", true);
    }

    responseObj.put("returnValue", true);
    responseObj.put("reason", powerOnReason);

    LSUtils::postToClient(request, responseObj);
    return true;
}

bool LunaPmsRoot::setPowerOnReasonCb(LSHandle *sh, LSMessage *message, void *data)
{
    LS::Message request(message);
    pbnjson::JValue requestObj;
    int parseError = 0;
    LSError lserror;
    LSErrorInit(&lserror);

    const std::string schema = STRICT_SCHEMA(PROPS_1(PROP(reason, string))REQUIRED_1(reason));

    if (!LSUtils::parsePayload(request.getPayload(), requestObj, schema, &parseError)) {
        LSUtils::respondWithError(request, errorInvalidJsonFormat, INVALID_JSON_FORMAT);
        return true;
    }

    pbnjson::JValue responseObj = pbnjson::Object();
    std::string reason = requestObj["reason"].asString();

    if((reason == "remoteKey") || (reason == "onTimer") || (reason == "SwDownload")|| (reason == "ota"))
    {
        mPowerOnReason = requestObj["reason"].asString();
    } else {
           LSUtils::respondWithError(request, errorInvalidJsonFormat, INVALID_JSON_FORMAT);
           return false;
    }

    responseObj.put("reason", mPowerOnReason);

    std::string payload;
    LSUtils::generatePayload(responseObj, payload);

    if (!LSSubscriptionReply(mpLsHandle, "powerOnReason", payload.c_str(), &lserror)) {
        LSErrorPrint(&lserror, stderr);
        LSErrorFree(&lserror);
    return false;
    }
    putInDb();

    responseObj.put("returnValue", true);
    LSUtils::postToClient(request, responseObj);
    return true;
}

bool LunaPmsRoot::setDbKind()
{
    PMSLunaCategoryContext *pCxt = PMSLunaCategoryContext::Instance();
    LunaPmsRoot *pThis = (LunaPmsRoot *)(pCxt->GetLunaCategoryContext(kPmsMsgCategoryRoot));
    LSError lserror;

    pbnjson::JValue jsonObject  = pbnjson::Object();
    jsonObject.put("id", "com.webos.service.power2:1");
    jsonObject.put("owner", "com.webos.service.power2");

    pbnjson::JValue subObject  = pbnjson::Object();
    subObject.put("name",mPowerOnReason);
    std::string subParamStr = "{\"name\":\"mPowerOnReason\"}";
    subObject.put("props",subParamStr);
    std::string paramStr;
    LSUtils::generatePayload(subObject, paramStr);

    jsonObject.put("indexes", paramStr);
    std::string putkindpayload;

    if (!LSUtils::generatePayload(jsonObject, putkindpayload))
    {
        MSG_DEBUG("Error while generating the payload");
        return false;
    }

    std::string uridbputkind = "luna://com.palm.db/putKind";
    bool success = LSCallOneReply(LunaInterfaceBase::mpLsHandle, uridbputkind.c_str(), putkindpayload.c_str(),NULL, pThis, NULL,&lserror);
    if (!success) {
        LSErrorPrint(&lserror, stderr);
        LSErrorFree(&lserror);
        return true;
    }
}

bool LunaPmsRoot::putInDb()
{
    PMSLunaCategoryContext *pCxt = PMSLunaCategoryContext::Instance();
    LunaPmsRoot *pThis = (LunaPmsRoot *)(pCxt->GetLunaCategoryContext(kPmsMsgCategoryRoot));
    LSError lserror;

    pbnjson::JValue subObject  = pbnjson::Object();
    subObject.put("_kind","com.webos.service.power2:1");
    subObject.put("reason",mPowerOnReason);
    std::string objparam;
    LSUtils::generatePayload(subObject, objparam);

    pbnjson::JValue jsonObject  = pbnjson::Object();
    jsonObject.put("objects",objparam);

    std::string putpayload;

    if (!LSUtils::generatePayload(jsonObject, putpayload))
    {
        MSG_DEBUG("Error while generating the payload");
        return false;
    }

    std::string uridbput = "luna://com.palm.db/put";
    bool success = LSCallOneReply(LunaInterfaceBase::mpLsHandle, uridbput.c_str(), putpayload.c_str(),NULL, pThis, NULL,&lserror);
    if (!success) {
        LSErrorPrint(&lserror, stderr);
        LSErrorFree(&lserror);
        return true;
    }

}

bool LunaPmsRoot::notifyAlarmExpiryCb(LSHandle *sh, LSMessage *message, void *data)
{
    MSG_DEBUG("Timer Expired");

    LS::Message request(message);
    pbnjson::JValue responseObj = pbnjson::Object();
    pbnjson::JValue requestObj;
    int parseError = 0;

    const std::string schema = RELAXED_SCHEMA(PROPS_1(PROP(state, string)));
    if (!LSUtils::parsePayload(request.getPayload(), requestObj, schema, &parseError)) {
        LSUtils::respondWithError(request, errorInvalidJsonFormat, INVALID_JSON_FORMAT);
        return true;
    }

    std::string stateName = requestObj["state"].asString();

    stateReference::GetInstance().processEvent("ACK");

    responseObj.put("returnValue", true);
    LSUtils::postToClient(request, responseObj);
    return true;
}

bool LunaPmsRoot::acquireWakeLockCb(LSHandle *sh, LSMessage *message, void *data)
{
    LS::Message request(message);
    pbnjson::JValue responseObj = pbnjson::Object();
    pbnjson::JValue requestObj;
    int parseError = 0;

    const std::string schema = STRICT_SCHEMA(PROPS_2(PROP(clientId, string), PROP(timeout, integer))REQUIRED_2(clientId, timeout));

    if (!LSUtils::parsePayload(request.getPayload(), requestObj, schema, &parseError)) {
        LSUtils::respondWithError(request, errorInvalidJsonFormat, INVALID_JSON_FORMAT);
        return true;
    }

    int timeout = requestObj["timeout"].asNumber<int>();
    std::string clientName = requestObj["clientId"].asString();

    if (setAwake(timeout, request, clientName,sh)) {
        responseObj.put("returnValue", true);
        LSUtils::postToClient(request, responseObj);
    }
    return true;
}


bool LunaPmsRoot::releaseWakeLockCb(LSHandle *sh, LSMessage *message, void *data)
{
    LS::Message request(message);
    pbnjson::JValue responseObj = pbnjson::Object();
    pbnjson::JValue requestObj;
    pbnjson::JValue jsonObject  = pbnjson::Object();
    int parseError = 0;
    PMSLunaCategoryContext *pCxt = PMSLunaCategoryContext::Instance();
    LunaPmsRoot *pThis = (LunaPmsRoot *)(pCxt->GetLunaCategoryContext(kPmsMsgCategoryRoot));

    const std::string schema = STRICT_SCHEMA(PROPS_1(PROP(clientId, string))REQUIRED_1(clientId));

    if (!LSUtils::parsePayload(request.getPayload(), requestObj, schema, &parseError)) {
        LSUtils::respondWithError(request, errorInvalidJsonFormat, INVALID_JSON_FORMAT);
        return true;
    }

    std::string clientName = requestObj["clientId"].asString();

    // if client doesn't exist return error
    if (!mPowerStatesMgr->isClientExist(mCurrentState, clientName))
    {
        LSUtils::respondWithError(request, errorClientNotRegistered, CLIENT_NOT_REGISTERED);
        return true;
    }

    clearAlarm("notifyAlarmExpiry");
    responseObj.put("returnValue", true);
    LSUtils::postToClient(request, responseObj);
    return true;
}

bool LunaPmsRoot::clearAlarm(const std::string &key)
{
    PMSLunaCategoryContext *pCxt = PMSLunaCategoryContext::Instance();
    LunaPmsRoot *pThis = (LunaPmsRoot *)(pCxt->GetLunaCategoryContext(kPmsMsgCategoryRoot));
    pbnjson::JValue jsonObject  = pbnjson::Object();
    jsonObject.put("key", key);
    std::string alarmClearPayload;
    LSError lserror;

    if (!LSUtils::generatePayload(jsonObject, alarmClearPayload))
    {
       MSG_DEBUG("Error while generating the payload");
    }

    std::string uriAlarmClear = "luna://com.webos.service.alarm/clear";
    bool success = LSCallOneReply(LunaInterfaceBase::mpLsHandle, uriAlarmClear.c_str(), alarmClearPayload.c_str(),NULL, pThis,NULL,&lserror);
    if (!success) {
        LSErrorPrint(&lserror, stderr);
        LSErrorFree(&lserror);
        return true;
    }
}

bool LunaPmsRoot::setAwake(int timeout, LS::Message &request, std::string clientName, LSHandle *sh)
{
    const int maxSeconds = (24 * 60 * 60) - 1; // max duration: 23:59:59

    // if clientName is invalid
    // return error
    if(!mPowerStatesMgr->isClientExist(mCurrentState, clientName))
    {
        LSUtils::respondWithError(request, errorClientNotRegistered, CLIENT_NOT_REGISTERED);
        return false;
    }
    // timeout is invalid
    // return error
    if(timeout < 1 || timeout > maxSeconds)
    {
        LSUtils::respondWithError(request, errorInvalidJsonFormat, INVALID_JSON_FORMAT);
    return false;
    }
    if (timeout > 0 )
    {
        if (setAlarm(timeout))
        {
            MSG_DEBUG("Alarm is set ");
        }
        else
        {
             LSUtils::respondWithError(request, errorInternalError, 0);
             return false;
        }
    }
    return true;
}

gboolean LunaPmsRoot::timerFired(gpointer data)
{
    stateReference::GetInstance().processEvent("ACK");
}

bool LunaPmsRoot::setAlarm(int timeout)
{
#if NOTIFY_WITH_ALARM
    PMSLunaCategoryContext *pCxt = PMSLunaCategoryContext::Instance();
    LunaPmsRoot *pThis = (LunaPmsRoot *)(pCxt->GetLunaCategoryContext(kPmsMsgCategoryRoot));

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
    std::string uriAlarmSet = "luna://com.webos.service.alarm/set";

    pbnjson::JValue jsonObject = pbnjson::Object();

    std::sprintf(time, "%02d:%02d:%02d", HH, MM, SS);

    jsonObject.put("key", "notifyAlarmExpiry");
    jsonObject.put("uri", "luna://com.webos.service.power2/notifyAlarmExpiry");
    jsonObject.put("in", time);
    jsonObject.put("wakeup", true);
    jsonObject.put("params", "{}");

    std::string alarmSetPayload;
    LSError lserror;

    if (!LSUtils::generatePayload(jsonObject, alarmSetPayload))
    {
        MSG_DEBUG("Error while generating the payload");
    LSUtils::respondWithError(request, errorInvalidJsonFormat, INVALID_JSON_FORMAT);
        return false;
    }

    bool success = LSCallOneReply(LunaInterfaceBase::mpLsHandle, uriAlarmSet.c_str(), alarmSetPayload.c_str(),NULL, pThis, NULL,&lserror);
    if (!success) {
        LSErrorPrint(&lserror, stderr);
        LSErrorFree(&lserror);
        return true;
    }
#else
    /*gtimer for timeout alarms*/
    sTimerCheck = g_timer_source_new_seconds(TIMER_CHECK_TIMEOUT);
    g_source_set_callback((GSource *)sTimerCheck,(GSourceFunc)timerFired, NULL, NULL);
    g_source_attach((GSource *)sTimerCheck, g_main_loop_get_context(mLoopdata));
#endif

    return true;
}

bool LunaPmsRoot::addSubscription(LSHandle* sh, LSMessage* message, const std::string key)
{
    bool retVal = false;
    LSError lserror;
    LSErrorInit(&lserror);

    retVal = LSSubscriptionAdd(sh, key.c_str(),
                               message, &lserror);
    if (!retVal) {
        LSErrorPrint(&lserror, stderr);
        LSErrorFree(&lserror);
    }
        return retVal;
}

// TOCHECK: decide for single addSubscription method. API should decide the key
bool LunaPmsRoot::addSubscription(LSHandle* sh, LSMessage* message, std::string statename, int priority)
{
    bool retVal = false;
    LSError lserror;
    LSErrorInit(&lserror);

    std::string subscriptionKey = statename + std::to_string(priority);

    retVal = LSSubscriptionAdd(sh, subscriptionKey.c_str(),
                               message, &lserror);
    if (!retVal) {
        LSErrorPrint(&lserror, stderr);
        LSErrorFree(&lserror);
    }
    return retVal;
}

PmsErrorCode_t LunaPmsRoot::CancelSubscriptionStateChange (LSMessage* message) {
    LS::Message request(message);
    PmsErrorCode_t err = kPmsSuccess;
    pbnjson::JValue requestObj;
    int parseError;

    const std::string schema = STRICT_SCHEMA(PROPS_2(PROP(state, string), PROP(subscribe, boolean))REQUIRED_2(state, subscribe));

    if (!LSUtils::parsePayload(request.getPayload(), requestObj, schema, &parseError))
    {
        MSG_INFO("Parsing failed!!");
        err = kPmsErrInvalidJSONFormat;
        return err;
    }

    std::string statename = requestObj["state"].asString();
    std::string clientName = LSMessageGetSenderServiceName(message);

    mPowerStatesMgr->removeClient(statename, clientName);

    if (mCurrentState == statename && mIsTransitionState) {
        if (LunaPmsRoot::subscribersCount == mPowerStatesMgr->getPowerStateCount(statename)) {
            stateReference::GetInstance().processEvent("ACK");
        }
    }

    return err;
}

bool LunaPmsRoot::handleTransitionState(const std::string& nextState)
{
    MSG_DEBUG("Transition State Notification Reached Successfully!!");
    mIsTransitionState = true;

    if (0 == mPowerStatesMgr->getPowerStateCount(mCurrentState)) {
        stateReference::GetInstance().processEvent("ACK");
        return true;
    }

    LSError lserror;
    LSErrorInit(&lserror);
    pbnjson::JValue responseObj = pbnjson::Object();
    mSessionToken = generateRandomString(10);
    responseObj.put("state", mCurrentState);
    responseObj.put("nextState", nextState);
    responseObj.put("token", mSessionToken);

    std::string payload;
    LSUtils::generatePayload(responseObj, payload);

    if (setAlarm(mpLogTimeOutSec))
    {
        MSG_DEBUG("Alarm is set ");
    }
    else
    {
         return false;
    }

    if (!LSSubscriptionReply(mpLsHandle, mCurrentState.c_str(), payload.c_str(), &lserror)) {
        LSErrorPrint(&lserror, stderr);
        LSErrorFree(&lserror);
        return false;
    }
    return true;
}

bool LunaPmsRoot::handleStateChange(const std::string& statename)
{
    MSG_DEBUG("State Change Notification Reached Successfully!! State: %s", statename.c_str());
    LunaPmsRoot::subscribersCount = 0;
    mIsTransitionState = false;
    capturedReplies.clear();

    if (mCurrentState == statename) { // No Need to broadcast to Subscribers
        return true;
    }

    mCurrentState = statename;

    LSError lserror;
    LSErrorInit(&lserror);
    pbnjson::JValue responseObj = pbnjson::Object();

    responseObj.put("state", mCurrentState);

    std::string payload;
    LSUtils::generatePayload(responseObj, payload);

    clearAlarm("notifyAlarmExpiry");

    if (!LSSubscriptionReply(mpLsHandle, "powerState", payload.c_str(), &lserror)) {
        LSErrorPrint(&lserror, stderr);
        LSErrorFree(&lserror);
        return false;
    }
    return true;
}

bool LunaPmsRoot::forceStateChange(const std::string& statename)
{
    if(!mIsTransitionState)
    {
        return stateReference::GetInstance().requestToSetState(statename);
    }

    return false;
}

bool LunaPmsRoot::handleEvent(const std::string& event)
{
    if(!mIsTransitionState)
    {
        return stateReference::GetInstance().processEvent(event);
    }

    return false;
}

std::string LunaPmsRoot::generateRandomString( size_t length )
{
    auto randchar = []() -> char
    {
        const char charset[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
        const size_t max_index = (sizeof(charset) - 1);
        return charset[ rand() % max_index ];
    };
    std::string str(length,0);
    std::generate_n( str.begin(), length, randchar );
    return str;
}

