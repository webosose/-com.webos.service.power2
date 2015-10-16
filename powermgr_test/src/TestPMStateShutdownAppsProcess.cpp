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
#include "StateShutdownAppsProcess.h"
#include "ShutdownStateHandler.h"
#include "TestUtil.h"


class TestPMStateShutdownAppsProcess: public ::testing::Test {
    public:
        TestPMStateShutdownAppsProcess()
        {
            printf("TestPMStateShutdownAppsProcess constructor entered\n");
            printf("TestPMStateShutdownAppsProcess constructor exit\n");
        }

        ~TestPMStateShutdownAppsProcess() {
            printf("====TestPMStateShutdownAppsProcess destructor entered ===\n");
            printf("====TestPMStateShutdownAppsProcess destructor exit ===\n");
        }
    };

TEST_F(TestPMStateShutdownAppsProcess, PMStateShutdownAppsProcess_handleEvent)
{
    printf("------------------ BEGIN PMStateShutdownAppsProcess_handleEvent ----------------------------\n");
    //TODO:
//    StateShutdownAppsProcess *lockclientmgr= new StateShutdownAppsProcess();
//    ShutdownEvent event=ShutdownEventNone;
//    lockclientmgr->handleEvent(event);
    printf("------------------ END PMStateShutdownAppsProcess_handleEvent----------------------------\n");
}

TEST_F(TestPMStateShutdownAppsProcess, PMStateShutdownAppsProcess_getState)
{
    printf("------------------ BEGIN PMStateShutdownAppsProcess_getState ----------------------------\n");
    //TODO:
//    StateShutdownAppsProcess *lockclientmgr= new StateShutdownAppsProcess();
//    ShutdownState ret=lockclientmgr->getState();
    printf("------------------ END PMStateShutdownAppsProcess_getState----------------------------\n");
}
