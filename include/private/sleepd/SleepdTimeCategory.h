// @@@LICENSE
//
//      Copyright (c) 2015-2016 LG Electronics, Inc.
//
// Confidential computer software. Valid license from LG required for
// possession, use or copying. Consistent with FAR 12.211 and 12.212,
// Commercial Computer Software, Computer Software Documentation, and
// Technical Data for Commercial Items are licensed to the U.S. Government
// under vendor's standard commercial license.
//
// LICENSE@@@

#ifndef GIT_SRC_SLEEPD_SLEEPDTIMECATEGORY_H_
#define GIT_SRC_SLEEPD_SLEEPDTIMECATEGORY_H_

#include <luna-service2/lunaservice.hpp>

class SleepdTimeCategory
{
    public:
        SleepdTimeCategory(LS::Handle &refSleepdLsHandle, LS::Handle &refPowerdLsHandle);
        ~SleepdTimeCategory() = default;

        bool init();
        bool alarmAddCalendar(LSMessage &message);
        bool alarmAdd(LSMessage &message);
        bool alarmQuery(LSMessage &message);
        bool alarmRemove(LSMessage &message);
        bool internalAlarmFired(LSMessage &message);

    private:
        LS::Handle &mRefSleepdLsHandle;
        LS::Handle &mRefPowerdLsHandle;
};

#endif /* GIT_SRC_SLEEPD_SLEEPDTIMECATEGORY_H_ */
