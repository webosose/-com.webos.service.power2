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

#ifndef _CLOCK_H_
#define _CLOCK_H_

#include <stdbool.h>
#include <time.h>
#include <glib.h>

void ClockGetTime(struct timespec *time);

bool ClockTimeIsGreater(struct timespec *a, struct timespec *b);

void ClockStr(GString *str, struct timespec *time);

void ClockPrintTime(struct timespec *time);
void ClockPrint(void);

void ClockDiff(struct timespec *diff, struct timespec *a, struct timespec *b);
void ClockAccum(struct timespec *sum, struct timespec *b);

void ClockAccumMs (struct timespec *sum, int duration_ms);

long ClockGetMs(struct timespec *ts);

void ClockClear(struct timespec *a);

#endif    // _CLOCK_H_
