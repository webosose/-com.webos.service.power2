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

#ifndef _DEBUG_H_
#define _DEBUG_H_


#include <stdio.h>
#include <assert.h>
#include <glib.h>
#include <string.h>
#include <stdbool.h>

/* define this to assert when BUG() macro is called */
#define ASSERT_ON_BUG

#undef G_LOG_DOMAIN
#define G_LOG_DOMAIN "Powerd"

// TODO use common error codes
#define FATAL_ERROR -1
#define ERROR       1
#define POWERD_ERROR_TIMEOUT 355

/* Set the current log level */
void SetLogLevel(GLogLevelFlags newLogLevel);

/* Set the destination of the log */
void SetUseSyslog(int useSyslog);

/* Return the current log level */
GLogLevelFlags GetLogLevel();

int DebugInit(void);

void _good_assert(const char * cond_str, bool cond);

#ifdef ASSERT_ON_BUG
#define BUG() {                     \
    *( (int*) NULL) = 0;            \
}
#else
#define BUG() {}
#endif

#define _assert(cond)               \
do {                                \
    _good_assert(#cond, cond);      \
} while (0)

#define g_critical_once(...)        \
do {                                \
    static int seen = 0;            \
    if (!seen)                      \
    {                               \
        seen = 1;                   \
        g_critical(__VA_ARGS__);    \
    }                               \
} while(0)

#define g_info(...)                 \
    g_log (G_LOG_DOMAIN,            \
           G_LOG_LEVEL_INFO,        \
           __VA_ARGS__)

#define g_perror(...)           \
do {                                   \
    char buf[512];                     \
    buf[0] = 0;                        \
    g_critical(__VA_ARGS__);           \
    strerror_r(errno, buf, 512);       \
    g_error(buf); \
} while(0)

#define MESSAGE(...)                   \
do {                                \
    g_message(__VA_ARGS__);         \
} while (0)

#define TRACE(...)                  \
do {                                \
    g_debug(__VA_ARGS__);         \
} while(0)

#define TRACE_ERROR(...)  \
do {                      \
    g_error(__VA_ARGS__);        \
} while(0)

#define TRACE_PERROR g_perror

#define iferr(cond, text, goto_label) \
do {                                    \
    if (cond)                           \
    {                                   \
        TRACE_ERROR(text);              \
        goto goto_label;                \
    }                                   \
} while (0);

void print_trace(void);

#endif //_DEBUG_H_
