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

/**
 * @file commands.c
 *
 * @brief This file contains APIs for registering new clients with powerd.
 */

#include <stdbool.h>
#include <stdint.h>
#include <pthread.h>
#include <string.h>
#include <assert.h>
#include <glib.h>
#include <cjson/json.h>
#include <luna-service2/lunaservice.h>

#include "powerd.h"
#include "init.h"

#define POWERD_IPC_NAME "com.palm.power"
#define POWERD_DEFAULT_CATEGORY "/com/palm/power/"
#define POWERD_SHUTDOWN_SERVICE "luna://com.palm.power/shutdown/"
#define POWERD_SHUTDOWN_CATEGORY "/shutdown"

typedef struct CallbackHelper
{
    void           *callback;
    LSMessageToken  token;

    enum CALLBACK_TYPE callbackType;
} CBH;

#define CBH_INIT(helper) \
CBH helper = {0, 0, CALLBACK_TYPE_PowerdCallback}

GMainLoop *gMainLoop = NULL;
bool gOwnMainLoop = false;
bool gOwnLunaService = false;

static LSHandle *gServiceHandle = NULL;

/**
* @brief Use your own LSHandle.  This MUST be called before PowerdCLientInit()
*
* @param  mainLoop
*/
void
_LSHandleAttach(LSHandle *sh)
{
    if (sh)
    {
        gServiceHandle = sh;
        gOwnLunaService = true;
    }
    else
    {
        gServiceHandle = NULL;
        gOwnLunaService = false;
    }
}

static void
SignalCancel(CBH *helper)
{
    bool retVal;
    LSError lserror;
    LSErrorInit(&lserror);

    retVal = LSCallCancel(gServiceHandle, helper->token, &lserror);
    if (!retVal)
    {
        LSErrorPrint(&lserror, stderr);
        LSErrorFree(&lserror);
    }

    helper->token = 0;
}

static void SignalRegisterWithCategory(const char *category, const char *signalName, LSFilterFunc callback, CBH *helper)
{
    bool retVal;
    LSError lserror;
    LSErrorInit(&lserror);

    char *payload = g_strdup_printf("{\"category\":\"%s\",\"method\":\"%s\"}", category, signalName);

    retVal = LSCall(gServiceHandle,
                    "luna://com.palm.lunabus/signal/addmatch", payload,
                    callback, helper, &helper->token, &lserror);
    if (!retVal)
    {
        LSErrorPrint(&lserror, stderr);
        LSErrorFree(&lserror);
    }

    g_free(payload);
}

static void
SignalRegister(const char *signalName, LSFilterFunc callback, CBH *helper)
{
    bool retVal;
    LSError lserror;
    LSErrorInit(&lserror);

    char *payload = g_strdup_printf(
            "{\"category\":\"/com/palm/power\",\"method\":\"%s\"}",
            signalName);

    retVal = LSCall(gServiceHandle,
        "luna://com.palm.lunabus/signal/addmatch", payload,
        callback, helper, &helper->token, &lserror);
    if (!retVal)
    {
        LSErrorPrint(&lserror, stderr);
        LSErrorFree(&lserror);
    }

    g_free(payload);
}

static void
SendMessage(LSFilterFunc callback, const char *uri, const char *payload, ...)
{
    bool retVal;
    char *buffer;

    LSError lserror;
    LSErrorInit(&lserror);

    va_list args;
    va_start(args, payload);
    buffer  = g_strdup_vprintf(payload, args);
    va_end(args);

    retVal = LSCall(gServiceHandle, uri, buffer,
                    callback, NULL, NULL, &lserror);
    if (!retVal)
    {
        LSErrorPrint(&lserror, stderr);
        LSErrorFree(&lserror);
    }

    g_free(buffer);
}

static void
SendSignal(LSFilterFunc callback, const char *uri, const char *payload, ...)
{
    bool retVal;
    char *buffer;

    LSError lserror;
    LSErrorInit(&lserror);

    va_list args;
    va_start(args, payload);
    buffer  = g_strdup_vprintf(payload, args);
    va_end(args);

    retVal = LSSignalSend(gServiceHandle, uri, buffer, &lserror);
    if (!retVal)
    {
        LSErrorPrint(&lserror, stderr);
        LSErrorFree(&lserror);
    }

    g_free(buffer);
}



/**
* @brief Common callback used for incoming signals.
*
* @param  sh
* @param  message
* @param  ctx
*
* @retval
*/
static bool
ClientNoParamCallback(LSHandle *sh, LSMessage *message, void *ctx)
{
    CBH *helper = (CBH *)ctx;
    const char* ptrCategory = NULL;

    if (!helper || !helper->callback) goto end;

    ptrCategory = LSMessageGetCategory(message);

    if (ptrCategory != NULL) {
        if (strcmp(ptrCategory, "/com/palm/power") == 0)
        {
            if (helper->callbackType == CALLBACK_TYPE_PowerdCallback_String_Int32) {
                ((PowerdCallback_String_Int32)helper->callback)( LSMessageGetPayload(message) , 0 );
            }
            else {
                ((PowerdCallback)helper->callback)();
            }
        }
        else if (strcmp(ptrCategory, POWERD_SHUTDOWN_CATEGORY) == 0)
        {
            ((PowerdCallback_String_Int32)helper->callback)( LSMessageGetPayload(message) , 0 );
        }
    }

end:
    return true;
}

/**
* @brief User API to register a callback for 'suspendRequest'
*        notification. Client should respond with PowerdSuspendRequestAck().
*
* @param  callback_function
*/
void
PowerdSuspendRequestRegister(PowerdCallback callback_function)
{
    static CBH_INIT(helper);
    PowerdHandle *handle = PowerdGetHandle();

    helper.callback = (void*)callback_function;

    if (helper.token)
    {
        SignalCancel(&helper);
    }

    if (callback_function)
    {
        SignalRegister("suspendRequest", ClientNoParamCallback, &helper);

        handle->suspendRequestRegistered = true;
    }
    else
    {
        handle->suspendRequestRegistered = false;
    }

    PowerdClientLock(handle);

    char *message = g_strdup_printf(
            "{\"register\":%s,\"clientId\":\"%s\"}",
            handle->suspendRequestRegistered ? "true" : "false",
            handle->clientId ?: "(null)");

    PowerdClientUnlock(handle);

    SendMessage(NULL, "luna://" POWERD_IPC_NAME POWERD_DEFAULT_CATEGORY
            "suspendRequestRegister", message);

    g_free(message);
}

/**
* @brief User API to register a callback for 'prepareSuspend'
*        notification.
*
* @param  callback_function
*/
void
PowerdPrepareSuspendRegister(PowerdCallback callback_function)
{
    static CBH_INIT(helper);
    PowerdHandle *handle = PowerdGetHandle();

    helper.callback = (void*)callback_function;

    if (helper.token)
    {
        SignalCancel(&helper);
    }

    if (callback_function)
    {
        SignalRegister("prepareSuspend", ClientNoParamCallback, &helper);

        handle->prepareSuspendRegistered = true;
    }
    else
    {
        handle->prepareSuspendRegistered = false;
    }

    PowerdClientLock(handle);

    char *message = g_strdup_printf(
            "{\"register\":%s,\"clientId\":\"%s\"}",
            handle->prepareSuspendRegistered ? "true" : "false",
            handle->clientId ?: "(null)");

    PowerdClientUnlock(handle);

    SendMessage(NULL, "luna://" POWERD_IPC_NAME POWERD_DEFAULT_CATEGORY
            "prepareSuspendRegister", message);

    g_free(message);
}

/**
* @brief Register a callback for resume notification when
*        system wakes from sleep.
*
* @param  callback_function
*/
void
PowerdResumeRegister(PowerdCallback callback_function)
{
    static CBH_INIT(helper);

    helper.callback = (void*)callback_function;

    if (helper.token)
    {
        SignalCancel(&helper);
    }

    SignalRegister("resume", ClientNoParamCallback, &helper);
}

void
PowerdResumeRegister2(PowerdCallback_String_Int32 callback_function)
{
    static CBH_INIT(helper);

    helper.callback = (void*)callback_function;
    helper.callbackType = CALLBACK_TYPE_PowerdCallback_String_Int32;

    if (helper.token)
    {
        SignalCancel(&helper);
    }

    SignalRegister("resume", ClientNoParamCallback, &helper);
}

/**
* @brief Register a callback for suspended notification when
*        the device goes to sleep. There is no guarantee you will
*        receive this message before the system has gone alseep.
*
*        If you want a message before the system has gone alseep,
*        see PowerdSuspendRequestRegister() or late state
*        PowerdPrepareSuspendRegister().
*
* @param  callback_function
*/
void
PowerdSuspendedRegister(PowerdCallback callback_function)
{
    static CBH_INIT(helper);

    helper.callback = (void*)callback_function;

    if (helper.token)
    {
        SignalCancel(&helper);
    }

    SignalRegister("suspended", ClientNoParamCallback, &helper);
}

/**
* @brief Permit or deny suspend execution.  This should be called in response
*        to a 'suspendRequest' notification.
*
* @param allowSuspend
*   - if true, allows system to prepare to suspend
*   - if false, system stays awake
*/
void
PowerdSuspendRequestAck(bool allowSuspend)
{
    char *message;

    PowerdHandle *handle = PowerdGetHandle();

    PowerdClientLock(handle);

    message = g_strdup_printf("{\"ack\":%s,\"clientId\":\"%s\"}",
            allowSuspend ? "true" : "false",
            handle->clientId ?: "(null)");

    PowerdClientUnlock(handle);

    SendMessage(NULL, "luna://" POWERD_IPC_NAME POWERD_DEFAULT_CATEGORY
                "suspendRequestAck", message);

    g_free(message);
}

/**
* @brief Late stage permit or deny suspend. This should be called
*        in response to a 'prepareSuspend' notification.
*
* @param finishedSuspend
*   - if true, client is ready for suspend
*   - if false, client is not ready for suspend,
*      suspend aborted, and Resume is sent.
*      See PowerdResumeRegister.
*/
void
PowerdPrepareSuspendAck(bool finishedSuspend)
{
    char *message;

    PowerdHandle *handle = PowerdGetHandle();

    PowerdClientLock(handle);

    message = g_strdup_printf("{\"ack\":%s,\"clientId\":\"%s\"}",
            finishedSuspend ? "true" : "false",
            handle->clientId ?: "(null)");

    PowerdClientUnlock(handle);

    SendMessage(NULL, "luna://" POWERD_IPC_NAME POWERD_DEFAULT_CATEGORY
                "prepareSuspendAck", message);

    g_free(message);
}

/**
* @brief Turn on the backlight.
*
* @param  on
*
* @retval
*/
int
PowerdSetDisplayMode(bool on)
{
    const char *message = on ?
        "{\"state\":\"on\"}" :
        "{\"state\":\"off\"}";

   SendMessage(NULL, "luna://" POWERD_IPC_NAME "/backlight", message);
   return 0;
}

/**
* @brief Set the brightness of the backlight. Currently not implemented.
*
* @param  percentBrightness
*
* @retval
*/
int
PowerdSetBacklightBrightness(int32_t percentBrightness)
{
    g_warning("%s is not implemented", __FUNCTION__);
    return 0;
}

/**
* @brief Set the brightness of the keylight. Currently not implemented.
*
* @param  percentBrightness
*
* @retval
*/
int
PowerdSetKeylightBrightness(int32_t percentBrightness)
{
    g_warning("%s is not implemented", __FUNCTION__);
    return 0;
}

/**
* @brief Forces suspend.  Called from test code.
*/
void
PowerdForceSuspend(void)
{
    SendMessage(NULL, "luna://" POWERD_IPC_NAME POWERD_DEFAULT_CATEGORY
                "forceSuspend", "{}");
}

/**
* @brief Request a battery status notification.
*/
void
PowerdGetBatteryStatusNotification(void)
{
    SendSignal(NULL, "luna://" POWERD_IPC_NAME POWERD_DEFAULT_CATEGORY
                "batteryStatusQuery", "{}");
}

/**
* @brief Helper that translates from jsonized message into form for use
*        with callback.
*
* @param  sh
* @param  message
* @param  ctx
*
* @retval
*/
static bool
_ClientBatteryStatusCallback(LSHandle *sh, LSMessage *message, void *ctx)
{
    CBH *helper = (CBH *)ctx;
    if (!helper || !helper->callback) return true;

    const char *payload = LSMessageGetPayload(message);
    struct json_object *object = json_tokener_parse(payload);
    if (is_error(object)) {
         goto end;
    }

    int percent;
    int temp_C;
    int current_mA;
    int voltage_mV;

    percent = json_object_get_int(
            json_object_object_get(object, "percent"));

    temp_C = json_object_get_int(
            json_object_object_get(object, "temperature_C"));

    current_mA = json_object_get_int(
            json_object_object_get(object, "current_mA"));

    voltage_mV = json_object_get_int(
            json_object_object_get(object, "voltage_mV"));

    ((PowerdCallback_Int32_4)helper->callback)
                (percent, temp_C, current_mA, voltage_mV);

end:
    if (!is_error(object)) json_object_put(object);
    return true;
}

/**
* @brief Register a callback for battery status notifications.
*
* @param  callback_function
* Callback is called with the following parameters:
* callback(percent, temperature, current, voltage);
*/
void
PowerdBatteryStatusRegister(PowerdCallback_Int32_4 callback_function)
{
    static CBH_INIT(helper);

    if (helper.token)
    {
        SignalCancel(&helper);
    }

    helper.callback = (void*)callback_function;

    SignalRegister("batteryStatus", _ClientBatteryStatusCallback, &helper);
}

/**
* @brief Unimplemented.
*
* @param  callback_function
*/
void
PowerdGetChargerStatusNotification(void)
{
    g_warning("%s: This function is not implemented", __FUNCTION__);
}

/**
* @brief Unimplemented.
* @param callback - Function to be called when charger state changes.
*
* @param  callback_function
* Callback is called with the following parameters:
* callback(source, current_mA).
*/
void
PowerdChargerStatusRegister(PowerdCallback_String_Int32 callback_function)
{
    g_warning("%s: This function is not implemented", __FUNCTION__);
}

static void *
_PowerdIPCThread(void *ctx)
{
    g_main_loop_run(gMainLoop);
    return NULL;
}

static bool
_identify_callback(LSHandle *sh, LSMessage *msg, void *ctx)
{
    struct json_object * object =
                json_tokener_parse(LSMessageGetPayload(msg));
    if (is_error(object)) goto error;

    bool subscribed = json_object_get_boolean(
            json_object_object_get(object, "subscribed"));
    const char *clientId = json_object_get_string(
            json_object_object_get(object, "clientId"));

    if (!subscribed || !clientId)
    {
        g_critical("%s: Could not subscribe to powerd %s.", __FUNCTION__,
                   LSMessageGetPayload(msg));
        goto end;
    }

    PowerdHandle *handle = PowerdGetHandle();
    PowerdSetClientId(handle, clientId);

    char *message = g_strdup_printf(
            "{\"register\":true,\"clientId\":\"%s\"}", clientId);

    if (handle->suspendRequestRegistered)
    {
        SendMessage(NULL, "luna://" POWERD_IPC_NAME POWERD_DEFAULT_CATEGORY
                "suspendRequestRegister", message);
    }

    if (handle->prepareSuspendRegistered)
    {
        SendMessage(NULL, "luna://" POWERD_IPC_NAME POWERD_DEFAULT_CATEGORY
                "prepareSuspendRegister", message);
    }

#if defined(ENABLE_WAKELOCK_FOR_SLEEP_STATE)
    if (handle->wakeLockRegistered) {
        SendMessage(handle->wakeLockRegisterCallback, "luna://com.palm.sleep/com/palm/power/wakeLockRegister", message);
    }
#endif

#if defined(ENABLE_POWEROFF_REBOOT_SIGNAL)
    if (handle->shutdownServicesRegistered)
    {
        char *shutdownMessage = NULL;

        shutdownMessage = g_strdup_printf("{\"clientId\":\"%s\",\"clientName\":\"%s\"}", handle->clientId ?: "(null)", handle->clientName ?: "(null)");
        SendMessage(NULL, POWERD_SHUTDOWN_SERVICE "shutdownServicesRegister", shutdownMessage);
        g_free(shutdownMessage);
    }
#endif

    g_free(message);

error:
end:
    if (!is_error(object)) json_object_put(object);
    return true;
}

/**
* @brief Called to re-register with powerd if powerd crashes.
*
* @param  sh
* @param  msg
* @param  ctx
*
* @retval
*/
static bool
_powerd_server_up(LSHandle *sh, LSMessage *msg, void *ctx)
{
    bool connected;

    struct json_object *object = json_tokener_parse(LSMessageGetPayload(msg));
    if (is_error(object)) goto error;

    connected = json_object_get_boolean(
                json_object_object_get(object, "connected"));

    if (connected)
    {
        g_debug("%s connected was true (powerd is already running)", __FUNCTION__);
        PowerdHandle *handle = PowerdGetHandle();

        /* Send our name to powerd. */
        SendMessage(_identify_callback,
                "luna://com.palm.power/com/palm/power/identify",
                "{\"subscribe\":true,\"clientName\":\"%s\"}",
                handle->clientName);
    }

end:
    if (!is_error(object)) json_object_put(object);
    return true;

error:
    g_critical("%s: Error registering with com.palm.power", __FUNCTION__);
    goto end;
}

void
_PowerdClientIPCRun(void)
{
    bool retVal;
    LSError lserror;

    PowerdHandle *handle = PowerdGetHandle();

    LSErrorInit(&lserror);

    if (!gServiceHandle)
    {
        retVal = LSRegister(NULL, &gServiceHandle, &lserror);

        // If luna service connection is failed, establish new connection using handle.clientName
        #if defined(ENABLE_POWEROFF_REBOOT_SIGNAL)
            if (!retVal) {
                if ( handle != NULL ) {
                    retVal = LSRegister(handle->clientName, &gServiceHandle, &lserror);
                }
            }
        #endif

        if (!retVal) goto error;

        if (!gMainLoop)
        {
            int ret;

            GMainContext *context = g_main_context_new();
            if (!context) goto error;

            gMainLoop = g_main_loop_new(context, FALSE);
            g_main_context_unref(context);
            if (!gMainLoop) goto error;

            pthread_attr_t attr;
            pthread_attr_init(&attr);

            ret = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
            assert(ret == 0);

            pthread_t tid;
            pthread_create(&tid, &attr, _PowerdIPCThread, NULL);
        }

        retVal = LSGmainAttach(gServiceHandle, gMainLoop, &lserror);
        if (!retVal) goto error;
    }

    retVal = LSCall(gServiceHandle,
        "luna://com.palm.lunabus/signal/registerServerStatus",
        "{\"serviceName\":\"com.palm.power\"}", _powerd_server_up,
        NULL, NULL, &lserror);
    if (!retVal) goto error;

    return;
error:
    LSErrorPrint(&lserror, stderr);
    LSErrorFree(&lserror);
    return;
}


/**
* @brief Cleanup all IPC.
*/
void
_PowerdClientIPCStop(void)
{
    if (gMainLoop)
    {
        if (!gOwnMainLoop)
            g_main_loop_quit(gMainLoop);

        g_main_loop_unref(gMainLoop);

        gMainLoop = NULL;
        gOwnMainLoop = false;
    }

    if (gServiceHandle && !gOwnLunaService)
    {
        bool retVal;
        LSError lserror;
        LSErrorInit(&lserror);

        retVal = LSUnregister(gServiceHandle, &lserror);
        if (!retVal)
        {
            LSErrorPrint(&lserror, stderr);
            LSErrorFree(&lserror);
        }
    }
    gServiceHandle = NULL;
    gOwnLunaService = false;
}

#if defined(ENABLE_POWEROFF_REBOOT_SIGNAL)
void PowerdShutdownServicesRegister(PowerdCallback_String_Int32 callback_function)
{
    static CBH_INIT(helper);
    PowerdHandle *handle = PowerdGetHandle();

    helper.callback = (void*)callback_function;
    helper.callbackType = CALLBACK_TYPE_PowerdCallback_String_Int32;

    if (helper.token)
    {
        SignalCancel(&helper);
    }

    if (callback_function)
    {
        SignalRegisterWithCategory(POWERD_SHUTDOWN_CATEGORY, "shutdownServices", ClientNoParamCallback, &helper);
        handle->shutdownServicesRegistered = true;
    }
    else
    {
        handle->shutdownServicesRegistered = false;
    }

    if ( handle->clientId ) {
        PowerdClientLock(handle);

        char *message = g_strdup_printf(
                                        "{\"clientId\":\"%s\",\"clientName\":\"%s\"}",
                                        handle->clientId ?: "(null)",
                                        handle->clientName ?: "(null)");

        PowerdClientUnlock(handle);

        SendMessage(NULL, POWERD_SHUTDOWN_SERVICE "shutdownServicesRegister", message);

        g_free(message);
    }
}

void PowerdShutdownServicesAck(PowerdCallback callback_function , bool isAck)
{
    static CBH_INIT(helper);
    PowerdHandle *handle = PowerdGetHandle();

    helper.callback = (void*)callback_function;

    if (helper.token)
    {
        SignalCancel(&helper);
    }

    if ( handle && handle->clientId ) {
        PowerdClientLock(handle);

        char *message = g_strdup_printf("{\"clientId\":\"%s\",\"isAck\":%s}", handle->clientId ?: "(null)", isAck ? "true" : "false");

        PowerdClientUnlock(handle);

        SendMessage(NULL, POWERD_SHUTDOWN_SERVICE "shutdownServicesAck", message);

        g_free(message);
    }
}
#endif

#if defined(ENABLE_WAKELOCK_FOR_SLEEP_STATE)
void PowerdWakeLockRegister(LSFilterFunc callback_function)
{
    char *message = NULL;
    PowerdHandle *handle = PowerdGetHandle();

    if ( handle ) {
        if (callback_function)
        {
            handle->wakeLockRegistered = true;
            handle->wakeLockRegisterCallback = callback_function;
        }
        else
        {
            handle->wakeLockRegistered = false;
            handle->wakeLockRegisterCallback = NULL;
        }

        if ( handle->clientId ) {
            if (callback_function)
            {
                handle->wakeLockRegistered = true;
                handle->wakeLockRegisterCallback = callback_function;
            }
            else
            {
                handle->wakeLockRegistered = false;
                handle->wakeLockRegisterCallback = NULL;
            }

            PowerdClientLock(handle);

            message = g_strdup_printf("{\"clientId\":\"%s\",\"register\":\"true\"}", handle->clientId ?: "(null)");

            PowerdClientUnlock(handle);

            SendMessage(callback_function, "luna://com.palm.sleep/com/palm/power/wakeLockRegister", message);

            g_free(message);
        }
    }

}

void PowerdSetWakeLock(LSFilterFunc callback_function , bool isWakeup)
{
    char *message = NULL;
    PowerdHandle *handle = PowerdGetHandle();

    if ( handle && handle->clientId ) {
        PowerdClientLock(handle);

        message = g_strdup_printf("{\"clientId\":\"%s\",\"isWakeup\":%s}", handle->clientId ?: "(null)", isWakeup ? "true" : "false");

        PowerdClientUnlock(handle);

        SendMessage(callback_function, "luna://com.palm.sleep/com/palm/power/setWakeLock", message);

        g_free(message);
    }
}
#endif
