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
