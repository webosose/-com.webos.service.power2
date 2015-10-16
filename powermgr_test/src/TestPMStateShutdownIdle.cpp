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

#include "PMSCommon.h"
#include "PmsLuna2Utils.h"
#include "PowerManagerService.h"
#include "PmsLogging.h"
#include "ShutdownCategoryMethods.h"
#include "NyxUtil.h"
#include "TestUtil.h"
#include "StateShutdownIdle.h"
#include "ShutdownStateHandler.h"

class TestPMStateShutdownIdle : public ::testing::Test {
    public:
        TestPMStateShutdownIdle() //:DBQuery(NULL)
        {
            printf("TestPMStateShutdownIdle constructor entered\n");
            printf("TestPMStateShutdownIdle constructor exit\n");
        }

        ~TestPMStateShutdownIdle() {
            printf("====TestPMStateShutdownIdle destructor entered ===\n");
            printf("====TestPMStateShutdownIdle destructor exit ===\n");
        }
};

TEST_F(TestPMStateShutdownIdle, PMStateShutdownIdle_handleEvent)
{
    printf("------------------ BEGIN PMStateShutdownIdle_handleEvent ----------------------------\n");
//    StateShutdownIdle *lockclientmgr= new StateShutdownIdle();
//    ShutdownEvent event = ShutdownEventNone;
//    lockclientmgr->handleEvent(event);
    printf("------------------ END PMStateShutdownIdle_handleEvent----------------------------\n");
}

TEST_F(TestPMStateShutdownIdle, PMStateShutdownIdle_getState)
{
    printf("------------------ BEGIN PMStateShutdownIdle_getState ----------------------------\n");
//    StateShutdownIdle *lockclientmgr= new StateShutdownIdle();
//    ShutdownState ret = lockclientmgr->getState();
    printf("------------------ END PMStateShutdownIdle_getState----------------------------\n");
}
