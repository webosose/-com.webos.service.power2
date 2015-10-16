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

#include <luna-service2/lunaservice.hpp>

#include "PMSCommon.h"
#include "PmsLuna2Utils.h"
#include "PowerManagerService.h"
#include "PmsLogging.h"
#include "ShutdownCategoryMethods.h"
#include "NyxUtil.h"
#include "StateShutdownAction.h"
#include "ShutdownStateHandler.h"
#include "TestUtil.h"

class TestPMStateShutdownAction: public ::testing::Test {
public:
    TestPMStateShutdownAction()
    {
        printf("TestPMStateShutdownAction constructor entered\n");
        printf("TestPMStateShutdownAction constructor exit\n");
    }

    ~TestPMStateShutdownAction() {
        printf("====TestPMStateShutdownAction destructor entered ===\n");
        printf("====TestPMStateShutdownAction destructor exit ===\n");
    }
};

TEST_F(TestPMStateShutdownAction, PMStateShutdownAction_handleEvent)
{
    printf("------------------ BEGIN PMStateShutdownAction_handleEvent ----------------------------\n");
//    StateShutdownAction *lockclientmgr= new StateShutdownAction();
//    ShutdownEvent event=ShutdownEventNone;
//    lockclientmgr->handleEvent(event);
    printf("------------------ END PMStateShutdownAction_handleEvent----------------------------\n");
}

TEST_F(TestPMStateShutdownAction, PMStateShutdownAction_getState)
{
    printf("------------------ BEGIN PMStateShutdownAction_getState ----------------------------\n");
//    StateShutdownAction *lockclientmgr= new StateShutdownAction();
//    ShutdownState ret=lockclientmgr->getState();
    printf("------------------ END PMStateShutdownAction_getState----------------------------\n");
}

TEST_F(TestPMStateShutdownAction, PMStateShutdownAction_forceShutdownReboot)
{
    printf("------------------ BEGIN PMStateShutdownAction_forceShutdownReboot ----------------------------\n");
//    StateShutdownAction *lockclientmgr= new StateShutdownAction();
//    lockclientmgr->forceShutdownReboot();
    printf("------------------ END PMStateShutdownAction_forceShutdownReboot----------------------------\n");
}
