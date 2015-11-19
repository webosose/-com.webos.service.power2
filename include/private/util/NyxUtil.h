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

#ifndef _UTIL_NYXUTIL_H_
#define _UTIL_NYXUTIL_H_

#include <time.h>
#include <string>
#include <nyx/nyx_client.h>

class NyxUtil
{
    public:
        ~NyxUtil() = default;
        time_t getRTCTime();
        static NyxUtil &getInstance();

        void shutdown(const std::string &reason);
        void reboot(const std::string &reason, const std::string &params);
        void setRtcAlarm();

    private:
        void init();
        NyxUtil();

    private:
        nyx_device_handle_t mNyxDeviceHandle = nullptr;
};

#endif /* _UTIL_NYXUTIL_H_ */

