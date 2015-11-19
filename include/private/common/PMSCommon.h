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

#ifndef _PMSCOMMON_H_
#define _PMSCOMMON_H_

#include <cstddef>
#include <new>

// ensure that we always use the nothrow version of new internally
inline void *operator new(std::size_t size)
{
    return operator new(size, std::nothrow);
}

inline void *operator new[](std::size_t size)
{
    return operator new[](size, std::nothrow);
}

#define PMS_SHUDOWN_URI "luna://com.webos.service.power2/shutdown/"
#define SIGNAL_SHUTDOWNISINPROGRESS "shutdownIsInProgress"
#define SIGNAL_SHUTDOWN_SERVICES "shutdownServices"
#define SIGNAL_SHUTDOWN_APPLICATIONS "shutdownApplications"
#define MIN_WAIT_TIME_FOR_SHUTDOWN 15
#define MAX_WAIT_TIME_FOR_SHUTDOWN 60
#define SHUTDOWN_WAKE_LOCK "echo shutdown 65000000000 > /sys/power/wake_lock"

#endif /* _PMSCOMMON_H_ */

