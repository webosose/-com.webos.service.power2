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

#ifndef _SHUTDOWN_SHUTDOWNEVENTS_H_
#define _SHUTDOWN_SHUTDOWNEVENTS_H_

typedef enum ShutdownEvents {
    ShutdownEventNone,
    ShutdownEventInit,
    ShutdownEventAbort,
    ShutdownEventAllAck,
    ShutdownEventTimeout
} ShutdownEvent;

#endif /* _SHUTDOWN_SHUTDOWNEVENTS_H_ */

