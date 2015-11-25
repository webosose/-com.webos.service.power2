/* @@@LICENSE
*
*      Copyright (c) 2007-2013 LG Electronics, Inc.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
* LICENSE@@@ */

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
