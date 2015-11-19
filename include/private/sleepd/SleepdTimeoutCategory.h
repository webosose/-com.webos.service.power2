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

#ifndef GIT_SRC_SLEEPD_SLEEPDTIMEOUTCATEGORY_H_
#define GIT_SRC_SLEEPD_SLEEPDTIMEOUTCATEGORY_H_

#include <luna-service2/lunaservice.hpp>

class SleepdTimeoutCategory
{
    public:
        SleepdTimeoutCategory(LS::Handle &refSleepdLsHandle);
        ~SleepdTimeoutCategory() = default;
        bool init();

        bool set(LSMessage &message);
        bool clear(LSMessage &message);

        static bool alarmsTimeoutCb(LSHandle *sh, LSMessage *message, void *ctx);

    private:
        LS::Handle &mRefSleepdLsHandle;
};

#endif /* GIT_SRC_SLEEPD_SLEEPDTIMEOUTCATEGORY_H_ */
