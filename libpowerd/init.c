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

#include <assert.h>

#include <stdbool.h>
#include <stdlib.h>
#include <pthread.h>

#include <luna-service2/lunaservice.h>

#include "init.h"
#include "powerd-debug.h"

void _PowerdClientIPCRun(void);
void _PowerdClientIPCStop(void);

void _LSHandleAttach(LSHandle *sh);

extern GMainLoop *gMainLoop;
extern bool gOwnMainLoop;

static PowerdHandle sHandle =
{
    .clientName = "",
    .clientId = NULL,
    .suspendRequestRegistered = false,
    .prepareSuspendRegistered = false,

#if defined(ENABLE_POWEROFF_REBOOT_SIGNAL)
    .shutdownServicesRegistered = false,
#endif

#if defined(ENABLE_WAKELOCK_FOR_SLEEP_STATE)
    .wakeLockRegistered = false,
    .wakeLockRegisterCallback = NULL,
#endif

    .lock     = PTHREAD_MUTEX_INITIALIZER,
};

static void
PowerdHandleInit(PowerdHandle *handle)
{
    if (handle->clientId)
        g_free(handle->clientId);

    handle->clientName = "";
    handle->clientId = NULL;
    handle->suspendRequestRegistered = false;
    handle->prepareSuspendRegistered = false;

#if defined(ENABLE_POWEROFF_REBOOT_SIGNAL)
    handle->shutdownServicesRegistered = false;
#endif

#if defined(ENABLE_WAKELOCK_FOR_SLEEP_STATE)
    handle->wakeLockRegistered = false;
    handle->wakeLockRegisterCallback = NULL;
#endif
}

PowerdHandle*
PowerdGetHandle(void)
{
    return &sHandle;
}

/**
* @brief Register as a power-aware client with name 'clientName'.
*
* @param  clientName
*/
void
PowerdClientInit(const char *clientName)
{
    PowerdHandleInit(&sHandle);

    sHandle.clientName = clientName;

    _PowerdClientIPCRun();
}

/**
* @brief Register as a power-aware client using the existing
*        LunaService handle.
*
* @param  clientName
* @param  sh
*/
void
PowerdClientInitLunaService(const char *clientName, LSHandle *sh)
{
    _LSHandleAttach(sh);
    PowerdClientInit(clientName);
}

/**
* @brief Use this mainloop instead of creating a powerd IPC
*        thread automatically.  This MUST be called before PowerdClientInit()
*        if you wish to use your own mainloop.
*
* @param  mainLoop
*/
void
PowerdGmainAttach(GMainLoop *mainLoop)
{
    if (mainLoop)
    {
        gMainLoop = g_main_loop_ref(mainLoop);
        gOwnMainLoop = true;
    }
    else
    {
        gMainLoop = NULL;
        gOwnMainLoop = false;
    }
}

void
PowerdClientLock(PowerdHandle *handle)
{
    int ret = pthread_mutex_lock(&sHandle.lock);
    assert(ret == 0);
}

void
PowerdClientUnlock(PowerdHandle *handle)
{
    int ret = pthread_mutex_unlock(&sHandle.lock);
    assert(ret == 0);
}

void
PowerdSetClientId(PowerdHandle *handle, const char *clientId)
{
    PowerdClientLock(handle);

    if (handle->clientId) g_free(handle->clientId);
    handle->clientId = g_strdup(clientId);

    PowerdClientUnlock(handle);
}

/**
* @brief Stop being a powerd client.
*        Implicit in this is we disconnect from any communications
*        from powerd.
*/
void
PowerdClientDeinit(void)
{
    PowerdHandleInit(&sHandle);
    _PowerdClientIPCStop();

}
