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

#ifndef _POWERD_INIT_H_
#define _POWERD_INIT_H_

#if defined(ENABLE_WAKELOCK_FOR_SLEEP_STATE)
#include <luna-service2/lunaservice.h>
#endif

typedef struct
{
    const char *clientName;
    char       *clientId;
    bool        suspendRequestRegistered;
    bool        prepareSuspendRegistered;

#if defined(ENABLE_POWEROFF_REBOOT_SIGNAL)
    bool shutdownServicesRegistered;
#endif

#if defined(ENABLE_WAKELOCK_FOR_SLEEP_STATE)
    bool wakeLockRegistered;
    LSFilterFunc wakeLockRegisterCallback;
#endif

    pthread_mutex_t lock;
} PowerdHandle;

PowerdHandle* PowerdGetHandle(void);

void PowerdSetClientId(PowerdHandle *handle, const char *clientId);

void PowerdClientLock(PowerdHandle *handle);
void PowerdClientUnlock(PowerdHandle *handle);

#endif // _POWERD_INIT_H_
