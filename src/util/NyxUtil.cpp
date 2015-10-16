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

#include <cstdlib>
#include "NyxUtil.h"
#include "PmsLogging.h"

NyxUtil::NyxUtil()
{
    init();
}

void NyxUtil::init()
{
    int ret = NYX_ERROR_NONE;
    ret = nyx_device_open(NYX_DEVICE_SYSTEM, "Main", &mNyxDeviceHandle);

    if (ret != NYX_ERROR_NONE) {
        mNyxDeviceHandle = nullptr;
        PMSLOG_DEBUG("Error while opening nyx device system. Error[%d]", ret);
        std::abort();
    }
}

time_t NyxUtil::getRTCTime()
{
    time_t now = 0;
    nyx_system_query_rtc_time(mNyxDeviceHandle, &now);
    return now;
}

NyxUtil &NyxUtil::getInstance()
{
    static NyxUtil handle;
    return handle;
}

void NyxUtil::shutdown(const std::string &reason)
{
    PMSLOG_DEBUG("[%s] shutting down the system. reason[%s]", __FUNCTION__,
                 reason.c_str());

    //TODO: fasthalt info has to be read from the configfile
    nyx_system_shutdown(mNyxDeviceHandle, NYX_SYSTEM_NORMAL_SHUTDOWN, reason.c_str());
}

void NyxUtil::reboot(const std::string &reason)
{
    PMSLOG_INFO(MSGID_FRC_SHUTDOWN, 1, PMLOGKS("Reason", reason.c_str()),
                "Pwrevents shutting down system");
    //TODO: fasthalt info has to be read from the configfile
    nyx_system_reboot(mNyxDeviceHandle, NYX_SYSTEM_NORMAL_SHUTDOWN, reason.c_str());
}

void NyxUtil::setRtcAlarm()
{
    nyx_system_set_alarm(mNyxDeviceHandle, 0, nullptr, nullptr);
}

