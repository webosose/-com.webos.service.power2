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

#include "ShutdownCategoryMethods.h"
#include "PmsLuna2Utils.h"
#include "PowerManagerService.h"
#include "PmsLogging.h"
#include "ShutdownStateHandler.h"
#include "NyxUtil.h"
#include "PMSCommon.h"
#include "ShutdownClientsMgrImpl.h"

ShutdownCategoryMethods::ShutdownCategoryMethods(LS::Handle &refLsHandle) :
    mRefLsHandle(refLsHandle)
{
    LS_CREATE_CATEGORY_BEGIN(ShutdownCategoryMethods, shutdownAPI)
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
        {"shutdownServices" },
        { },
    };

    try {
        mRefLsHandle.registerCategory("/shutdown",
                                      LS_CATEGORY_TABLE_NAME(shutdownAPI), shutdownSignals, nullptr);
        mRefLsHandle.setCategoryData("/shutdown", this);
        PMSLOG_INFO(MSGID_SHUTDOWN_DEBUG, 0,
                    "%s, %s, shutdown category registration is success**", __FILE__,
                    __FUNCTION__);
    } catch (LS::Error &lunaError) {
        PMSLOG_ERROR(MSGID_CATEGORY_REG_FAIL, 0, "could not register shutdown category: %s", lunaError.what());
    }
}

ShutdownCategoryMethods::~ShutdownCategoryMethods()
{
    delete mAppsMgr;
    delete mServicesMgr;
}

bool ShutdownCategoryMethods::init()
{
    mAppsMgr = new ShutdownClientsMgrImpl();
    mServicesMgr = new ShutdownClientsMgrImpl();
    return (mAppsMgr && mServicesMgr);
}

/**
@brief This api will reboot the device.

@par Parameters

  Name     | Required |   Type  | Description
--------|----------|---------|----------
reason  | YES      | String  | Reboot reason

@par Returns(Call)

  Name         | Required |   Type  | Description
------------|----------|---------|----------
returnValue | Yes      | Boolean | Returns true. If there is an error in json data parsing, returns false.
errorText   | No       | String  | error string that describes the error

@example
luna-send -n 1 luna://com.webos.service.power2/shutdown/machineReboot '{"reason":"laf"}'
{
    "returnValue": true
}
*/
bool ShutdownCategoryMethods::machineReboot(LSMessage &message)
{
    LS::Message request(&message);
    pbnjson::JValue responseObj = pbnjson::Object();
    pbnjson::JValue requestObj;
    std::string reason;
    const std::string schema = RELAXED_SCHEMA(PROPS_2(PROP(reason, string), PROP(params, string))REQUIRED_1(reason));
    int parseError = 0;

    if (!LSUtils::parsePayload(request.getPayload(), requestObj, schema, &parseError)) {
        PMSLOG_ERROR(MSGID_SCEMA_VAL_FAIL, 0, "machineReboot schema validation failed");
        LSUtils::respondWithError(request, errorParseFailed, parseError);
        return true;
    }

    mReboot = true;

    if (!mShutdownStarted) {
        LSMessageRef(&message);
        startShutdown(message);
    } else {
        responseObj.put("returnValue", true);
        LSUtils::postToClient(request, responseObj);
    }

    return true;
}

/**
@brief This API will poweroff the device.

@par Parameters

  Name  | Required |   Type  | Description
--------|----------|---------|----------
reason  | Yes      | String  | poweroff reason

@par Returns(Call)

  Name      | Required |   Type  | Description
------------|----------|---------|----------
returnValue | Yes      | Boolean | Returns true. If there is an error in json data parsing, returns false.
errorText   | No       | String  | error string that describes the error

@example
luna-send -n 1 luna://com.webos.service.power2/shutdown/machineOff '{"reason":"Shutdown request by User"}'
{
    "returnValue": true
}
*/
bool ShutdownCategoryMethods::machineOff(LSMessage &message)
{
    LS::Message request(&message);
    pbnjson::JValue responseObj = pbnjson::Object();
    pbnjson::JValue requestObj;

    int parseError = 0;
    const std::string schema = STRICT_SCHEMA(PROPS_1(PROP(reason, string))REQUIRED_1(reason));

    if (!LSUtils::parsePayload(request.getPayload(), requestObj, schema, &parseError)) {
        PMSLOG_ERROR(MSGID_SCEMA_VAL_FAIL, 0, "machineOff schema validation failed");
        LSUtils::respondWithError(request, errorParseFailed, parseError);
        return true;
    }

    mReboot = false;

    if (!mShutdownStarted) {
        LSMessageRef(&message);
        startShutdown(message);
    } else {
        responseObj.put("returnValue", true);
        LSUtils::postToClient(request, responseObj);
    }

    return true;
}

void ShutdownCategoryMethods::startShutdown(LSMessage &message)
{
    struct timespec tv;
    system(SHUTDOWN_WAKE_LOCK);

    if (mReboot) {
        system("initctl emit --no-wait machineReboot");
    } else {
        system("initctl emit --no-wait machineOff");
    }

    /* Wait for 200ms. */
    tv.tv_sec = 0;
    tv.tv_nsec = 200 * 1000000;
    nanosleep(&tv, NULL);
    std::thread(&ShutdownCategoryMethods::run, this, std::ref(message)).detach();
}

void ShutdownCategoryMethods::run(LSMessage &message)
{
    std::unique_ptr<ShutdownStateHandler> shutdownstateHandle(new ShutdownStateHandler());
    shutdownstateHandle->setRebootMode(mReboot);

    if (!shutdownstateHandle || !shutdownstateHandle->init(mRefLsHandle, std::ref(*mAppsMgr), std::ref(*mServicesMgr))) {
        PMSLOG_INFO(MSGID_SHUTDOWN_DEBUG, 0, "memory allocation error");
        LS::Message request(&message);
        LSUtils::respondWithError(request, errorInternalError, 0);
        LSMessageUnref(&message);
        return;
    }

    ShutdownEvent event = ShutdownEventInit;
    std::unique_lock<std::mutex> lock(mShutdownThreadMtx);

    while (true) {
        //run the state machine
        do {
            PMSLOG_INFO(MSGID_SHUTDOWN_DEBUG, 0, "Entering the state : %d", shutdownstateHandle->getState());
            shutdownstateHandle->stateDispatch(event);
        } while (shutdownstateHandle->moveToNextState());

        // stop the thread after statemachine enters kPowerShutdownAction state
        if (kPowerShutdownAction == shutdownstateHandle->getState()) {
            PMSLOG_INFO(MSGID_SHUTDOWN_DEBUG, 0, "stopping thread");
            mStartTime = 0;
            sendInitiateReply(message);
            break;
        }

        if (std::cv_status::timeout == mShutdownThreadCV.wait_for(lock, std::chrono::seconds(MIN_WAIT_TIME_FOR_SHUTDOWN))) {
            event = ShutdownEventTimeout;
            PMSLOG_INFO(MSGID_SHUTDOWN_DEBUG, 0, "Thread  timeout happened");
        } else {
            event = ShutdownEventAllAck;
            PMSLOG_INFO(MSGID_SHUTDOWN_DEBUG, 0, "Thread  notification happened");
        }
    }
}

void ShutdownCategoryMethods::sendInitiateReply(LSMessage &message)
{
    LS::Message request(&message);
    pbnjson::JValue requestObj;
    pbnjson::JValue responseObj = pbnjson::Object();
    int parseError = 0;

    if (!LSUtils::parsePayload(request.getPayload(), requestObj, SCHEMA_ANY, &parseError)) {
        responseObj.put("reason", "Unknown");
    } else {
        responseObj.put("reason", requestObj["reason"].asString());
        responseObj.put("params", requestObj["params"].asString());
    }

    NyxUtil::getInstance().setRtcAlarm();

    responseObj.put("returnValue", true);
    LSUtils::postToClient(request, responseObj);
    LSMessageUnref(&message);

    if (mReboot) {
        NyxUtil::getInstance().reboot(requestObj["reason"].asString(), requestObj["params"].asString());
    } else {
        NyxUtil::getInstance().shutdown(requestObj["reason"].asString());
    }
}

/**
@brief The initiate method starts the shutdown process - sending the
shutdownApplications and shutdownServices signals to all the registered apps/services.
Once the shutdown process completes and the system decides it can
shutdown, this call returns.

@par Parameters none

@par Returns(Call)

Name        | Required |   Type  | Description
------------|----------|---------|------------
returnValue | Yes      | Boolean | Returns true/false

@example
luna-send -n 1 luna://com.webos.service.power2/initiate '{}'
{
    "returnValue": true
}
*/
bool ShutdownCategoryMethods::initiate(LSMessage &message)
{
    mStartTime = NyxUtil::getInstance().getRTCTime();
    LSMessageRef(&message);
    std::thread(&ShutdownCategoryMethods::run, this, std::ref(message)).detach();
    return true;
}

/**
@brief This API will register a client, so that PMS will wait for client acknowledgment during initiate.
If client is already registered, it'll be registered again

@par Parameters

Name        | Required |   Type  | Description
------------|----------|---------|----------
clientName  | Yes      | String  | client name

@par Returns(Call)

Name        | Required |   Type  | Description
------------|----------|---------|----------
clientId    | Yes      | String  | unique clientId identifying the client
returnValue | Yes      | Boolean | Returns true. Return false if there is json parse error
errorText   | No       | String  | error string that describes the error

@example
luna-send -n 1 luna://com.webos.service.power2/shutdown/shutdownApplicationsRegister '{"clientName": "com.palm.power"}'
{
    "clientId": "/var/run/ls2/M2DFcF.4",
    "returnValue": true
}
*/
bool ShutdownCategoryMethods::shutdownApplicationsRegister(LSMessage &message)
{
    LS::Message request(&message);
    pbnjson::JValue responseObj = pbnjson::Object();
    pbnjson::JValue requestObj;
    LSHandle *messageHandle = LSMessageGetConnection(&message);
    std::string schema;

    if (messageHandle == mRefLsHandle.get()) {
        schema = STRICT_SCHEMA(PROPS_1(PROP(clientName, string))REQUIRED_1(clientName));
    } else {
        schema = RELAXED_SCHEMA(PROPS_1(PROP(clientName, string))REQUIRED_1(clientName));
    }

    int parseError = 0;

    if (!LSUtils::parsePayload(request.getPayload(), requestObj, schema, &parseError)) {
        PMSLOG_ERROR(MSGID_SCEMA_VAL_FAIL, 0, "shutdownApplicationsRegister schema validation failed");
        LSUtils::respondWithError(request, errorParseFailed, parseError);
        return true;
    }

    // get the client name and id
    std::string clientName = requestObj["clientName"].asString();
    std::string clientId = request.getUniqueToken();

    //add client to the clienlist
    mAppsMgr->addClient(clientId, clientName);

    std::string subscripyionKey = "ShutdownAppsReg";
    addSubscription(message, subscripyionKey);

    PMSLOG_INFO(MSGID_SHUTDOWN_DEBUG, 0, "clientName : %s, clientId : %s", clientName.c_str(), clientId.c_str());

    responseObj.put("returnValue", true);
    responseObj.put("clientId", clientId);
    LSUtils::postToClient(request, responseObj);
    return true;
}

/**
@brief client will use this API for sending acknowledgment after receiving shutdownApplications Signal.

Precondition:
- client should be registered using "shutdownApplicationsRegister" API.
If it's not registered using "shutdownApplicationsRegister",client
acknowledgment will be ignored.

@par Parameters

Name        | Required |   Type  | Description
------------|----------|---------|----------
clientId    | Yes      | String  | clientId returned by shutdownApplicationsRegister API.

@par Returns(Call)

Name        | Required |   Type  | Description
------------|----------|---------|----------
returnValue | Yes      | Boolean | Returns true. Return false if there is json parse error
errorText   | No       | String  | error string that describes the error

@example
luna-send -n 1 luna://com.webos.service.power2/shutdown/shutdownApplicationsAck '{"clientId": "/var/run/ls2/9lgPKJ.4"}'
{
    "returnValue": true
}
*/
bool ShutdownCategoryMethods::shutdownApplicationsAck(LSMessage &message)
{
    LS::Message request(&message);
    pbnjson::JValue responseObj = pbnjson::Object();
    pbnjson::JValue requestObj;
    auto parseError = 0;
    const std::string schema = STRICT_SCHEMA(PROPS_1(PROP(clientId, string))REQUIRED_1(clientId));

    if (!LSUtils::parsePayload(request.getPayload(), requestObj, schema, &parseError)) {
        PMSLOG_ERROR(MSGID_SCEMA_VAL_FAIL, 0, "shutdownApplicationsAck schema validation failed");
        LSUtils::respondWithError(request, errorParseFailed, parseError);
        return true;
    }

    // get the client id
    std::string clientId = requestObj["clientId"].asString();

    time_t diff = NyxUtil::getInstance().getRTCTime() - mStartTime;
    mAppsMgr->setAck(clientId, true, static_cast<double>(diff));

    if (mAppsMgr->isAllClientsAcknowledged()) {
        //wake up the shutdown thread when ack is received from all registered clients
        // for shutdownApplications signal
        mShutdownThreadCV.notify_all();
    }

    responseObj.put("returnValue", true);
    LSUtils::postToClient(request, responseObj);
    return true;
}

/**
@brief This API will register a client, so that PMS will wait for client acknowledgment during initiate.
If client is already registered with this API, it'll replace the old one.

@par Parameters

Name        | Required |   Type  | Description
------------|----------|---------|----------
clientName  | Yes      | String  | client name

@par Returns(Call)

Name        | Required |   Type  | Description
------------|----------|---------|----------
clientId    | Yes      | String  | unique clientId identifying the client
returnValue | Yes      | Boolean | Returns true. Return false if there is json parse error or client is not registered with "registerClient" API
errorText   | No       | String  | error string that describes the error

@example
luna-send -n 1 luna://com.webos.service.power2/shutdown/shutdownServicesRegister '{"clientName": "com.palm.power"}'
{
    "clientId": "/var/run/ls2/M2DFcF.4",
    "returnValue": true
}
*/
bool ShutdownCategoryMethods::shutdownServicesRegister(LSMessage &message)
{
    LS::Message request(&message);
    pbnjson::JValue responseObj = pbnjson::Object();
    pbnjson::JValue requestObj;
    int parseError = 0;
    LSHandle *messageHandle = LSMessageGetConnection(&message);
    std::string schema;

    if (messageHandle == mRefLsHandle.get()) {
        schema = STRICT_SCHEMA(PROPS_1(PROP(clientName, string))REQUIRED_1(clientName));
    } else {
        schema = RELAXED_SCHEMA(PROPS_1(PROP(clientName, string))REQUIRED_1(clientName));
    }

    if (!LSUtils::parsePayload(request.getPayload(), requestObj, schema, &parseError)) {
        PMSLOG_ERROR(MSGID_SCEMA_VAL_FAIL, 0, "shutdownServicesRegister schema validation failed");
        LSUtils::respondWithError(request, errorParseFailed, 0);
        return true;
    }

    /********* clientId *************/
    std::string clientName = requestObj["clientName"].asString();
    std::string clientId = request.getUniqueToken();;

    mServicesMgr->addClient(clientId, clientName);

    std::string subscripyionKey = "ShutdownServicesReg";
    addSubscription(message, subscripyionKey);

    PMSLOG_INFO(MSGID_SHUTDOWN_DEBUG, 0, "clientName : %s, clientId : %s", clientName.c_str(), clientId.c_str());

    responseObj.put("returnValue", true);
    responseObj.put("clientId", clientId);
    LSUtils::postToClient(request, responseObj);
    return true;
}

/**
@brief client will use this API for sending acknowledgment after receiving shutdownServices Signal.

Precondition:
- client should be registered using "shutdownServicesRegister" API. If it's not registered using "shutdownServicesRegister",
client acknowledgment will be ignored.

@par Parameters

Name        | Required |   Type  | Description
------------|----------|---------|----------
clientId    | Yes      | String  | clientId returned by registerClient/shutdownServicesRegister API.

@par Returns(Call)

Name        | Required |   Type  | Description
------------|----------|---------|----------
returnValue | Yes      | Boolean | Returns true. Return false if there is json parse error or client is not registered with "registerClient" API
errorText   | No       | String  | error string that describes the error

@example
luna-send -n 1 luna://com.webos.service.power2/shutdown/shutdownServicesAck '{"clientId": "/var/run/ls2/9lgPKJ.4"}'
{
    "returnValue": true
}
*/
bool ShutdownCategoryMethods::shutdownServicesAck(LSMessage &message)
{
    LS::Message request(&message);
    pbnjson::JValue responseObj = pbnjson::Object();
    pbnjson::JValue requestObj;
    int parseError = 0;
    const std::string schema = STRICT_SCHEMA(PROPS_1(PROP(clientId, string)
                                                    )REQUIRED_1(clientId));

    if (!LSUtils::parsePayload(request.getPayload(), requestObj, schema, &parseError)) {
        PMSLOG_ERROR(MSGID_SCEMA_VAL_FAIL, 0, "shutdownServicesAck schema validation failed");
        LSUtils::respondWithError(request, errorParseFailed, 0);
        return true;
    }

    // get the clientId
    std::string clientId = requestObj["clientId"].asString();
    time_t diff = NyxUtil::getInstance().getRTCTime() - mStartTime;
    bool isAck = requestObj["isAck"].asBool();
    mServicesMgr->setAck(clientId, isAck, static_cast<double>(diff));

    if (mServicesMgr->isAllClientsAcknowledged()) {
        mShutdownThreadCV.notify_all();
    }

    responseObj.put("returnValue", true);
    responseObj.put("clientId", clientId);
    LSUtils::postToClient(request, responseObj);
    return true;
}

void ShutdownCategoryMethods::addSubscription(LSMessage &message, const std::string &key)
{
    bool retVal = false;
    LSError lserror;
    LSErrorInit(&lserror);

    retVal = LSSubscriptionAdd(mRefLsHandle.get(), key.c_str(),
                               &message, &lserror);

    if (!retVal) {
        PMSLOG_ERROR(MSGID_LSSUBSCRI_ADD_FAIL, 1, PMLOGKS(ERRTEXT, lserror.message), "LSSubscriptionAdd failed. key[%s]",
                     key.c_str());
        LSErrorFree(&lserror);
    }
}

void ShutdownCategoryMethods::deregisterAppsServicesClient(const std::string &clientId)
{
    PMSLOG_INFO(MSGID_SHUTDOWN_DEBUG, 0,
                "client removed clientId[%s]", clientId.c_str());
    mServicesMgr->removeClient(clientId);
    mAppsMgr->removeClient(clientId);
}

