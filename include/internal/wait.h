/* @@@LICENSE
*
*      Copyright (c) 2007-2012 Hewlett-Packard Development Company, L.P.
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

#ifndef _WAITOBJ_H_
#define _WAITOBJ_H_

#define _GNU_SOURCE
#include <pthread.h>
#include <stdbool.h>

typedef struct
{
    pthread_condattr_t condattr;
    pthread_cond_t     cond;
    pthread_mutex_t    mutex;

    bool               locked;
} WaitObj;

void WaitObjectInit(WaitObj *obj);

void WaitObjectLock(WaitObj *obj);
void WaitObjectUnlock(WaitObj *obj);

int WaitObjectWait(WaitObj *obj, int ms);
int WaitObjectWaitTimeSpec(WaitObj *obj, struct timespec *delta);
int WaitObjectWaitAbsTime(WaitObj *obj, struct timespec *abstime);

void WaitObjectSignal(WaitObj *obj);
void WaitObjectSignalUnlocked(WaitObj *obj);


void WaitObjectBroadcast(WaitObj *obj);
void WaitObjectBroadcastUnlocked(WaitObj *obj);

bool WaitObjectIsLocked(WaitObj *obj);

#endif
