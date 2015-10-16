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
#include "StateShutdownServicesProcess.h"
#include "ShutdownStateHandler.h"

#define MSM_WAIT_SECONDS 5

class TestPMStateShutdownServicesProcess: public ::testing::Test {
public:
    TestPMStateShutdownServicesProcess()
    {
        printf("TestPMStateShutdownServicesProcess constructor entered\n");
        printf("TestPMStateShutdownServicesProcess constructor exit\n");
    }

    ~TestPMStateShutdownServicesProcess() {
        printf(
                "====TestPMStateShutdownServicesProcess destructor entered ===\n");
        printf("====TestPMStateShutdownServicesProcess destructor exit ===\n");
    }
};

TEST_F(TestPMStateShutdownServicesProcess, PMStateShutdownServicesProcess_handleEvent)
{
    printf("------------------ BEGIN PMStateShutdownServicesProcess_handleEvent ----------------------------\n");
//    StateShutdownServicesProcess *lockclientmgr= new StateShutdownServicesProcess();
//    ShutdownEvent event=ShutdownEventNone;
//    lockclientmgr->handleEvent(event);
    printf("------------------ END PMStateShutdownServicesProcess_handleEvent----------------------------\n");
}

TEST_F(TestPMStateShutdownServicesProcess, PMStateShutdownServicesProcess_getState)
{
    printf("------------------ BEGIN PMStateShutdownServicesProcess_getState ----------------------------\n");
//    StateShutdownServicesProcess *lockclientmgr= new StateShutdownServicesProcess();
//    ShutdownState ret=lockclientmgr->getState();
    printf("------------------ END PMStateShutdownServicesProcess_getState----------------------------\n");
}
