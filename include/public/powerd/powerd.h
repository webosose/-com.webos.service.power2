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

#ifndef _LIBPOWERD_H_
#define _LIBPOWERD_H_

#include <stdbool.h>
#include <stdint.h>
#if defined(ENABLE_WAKELOCK_FOR_SLEEP_STATE)
#include <luna-service2/lunaservice.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

enum {
    ResumeTypeKernel = 0,
    ResumeTypeActivity,
    ResumeAbortSuspend,
    ResumeTypeKernelAmbientRtc
};

enum CALLBACK_TYPE {
    CALLBACK_TYPE_PowerdCallback = 0,
    CALLBACK_TYPE_PowerdCallback_Int32,
    CALLBACK_TYPE_PowerdCallback_Int32_4,
    CALLBACK_TYPE_PowerdCallback_String_Int32
};

/**
 * Interface to libpowerd.
 * See doxygen pages for more information.
 */

typedef void (*PowerdCallback)(void);
typedef void (*PowerdCallback_Int32)(int);
typedef void (*PowerdCallback_Int32_4)(int, int, int, int);
typedef void (*PowerdCallback_String_Int32)(const char *, int);

void PowerdClientInit(const char *clientName);

void PowerdClientDeinit(void);

void PowerdDeferIdle(void);

void PowerdForceSuspend(void);

void PowerdSuspendRequestRegister(PowerdCallback callback);

void PowerdSuspendRequestAck(bool allowSuspend);

void PowerdPrepareSuspendRegister(PowerdCallback callback);

void PowerdPrepareSuspendAck(bool finishedSuspend);

void PowerdResumeRegister(PowerdCallback callback);

void PowerdResumeRegister2(PowerdCallback_String_Int32 callback);

void PowerdSuspendedRegister(PowerdCallback callback);

void PowerdGetChargerStatusNotification(void);

void PowerdChargerStatusRegister(PowerdCallback_String_Int32 callback);

void PowerdGetBatteryStatusNotification(void);

void PowerdBatteryStatusRegister(PowerdCallback_Int32_4 callback);

int PowerdSetDisplayMode(bool on);
int PowerdSetBacklightBrightness(int32_t percentBrightness);
int PowerdSetKeylightBrightness(int32_t percentBrightness);

#if defined(ENABLE_POWEROFF_REBOOT_SIGNAL)
void PowerdShutdownServicesRegister(PowerdCallback_String_Int32 callback_function);
void PowerdShutdownServicesAck(PowerdCallback callback_function , bool isAck);
#endif

#if defined(ENABLE_WAKELOCK_FOR_SLEEP_STATE)
void PowerdWakeLockRegister(LSFilterFunc callback_function);
void PowerdSetWakeLock(LSFilterFunc callback_function , bool isWakeup);
#endif

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _LIBPOWERD_H_
