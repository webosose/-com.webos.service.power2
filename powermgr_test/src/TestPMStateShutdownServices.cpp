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
#include "TestUtil.h"
#include "StateShutdownServices.h"
#include "ShutdownStateHandler.h"

#define MSM_WAIT_SECONDS 5

class TestPMStateShutdownServices: public ::testing::Test {
public:
    TestPMStateShutdownServices() //:DBQuery(NULL)
    {
        printf("TestPMStateShutdownServices constructor entered\n");
        printf("TestPMStateShutdownServices constructor exit\n");
    }

    ~TestPMStateShutdownServices() {
        printf("====TestPMStateShutdownServices destructor entered ===\n");
        printf("====TestPMStateShutdownServices destructor exit ===\n");
    }
};

TEST_F(TestPMStateShutdownServices, PMStateShutdownServices_handleEvent)
{
    printf("------------------ BEGIN PMStateShutdownServices_handleEvent ----------------------------\n");
//    StateShutdownServices *lockclientmgr= new StateShutdownServices();
//    ShutdownEvent event=ShutdownEventNone;
//    lockclientmgr->handleEvent(event);
    printf("------------------ END PMStateShutdownServices_handleEvent----------------------------\n");
}

TEST_F(TestPMStateShutdownServices, PMStateShutdownServices_getState)
{
    printf("------------------ BEGIN PMStateShutdownServices_getState ----------------------------\n");
//    StateShutdownServices *lockclientmgr= new StateShutdownServices();
//    ShutdownState ret=lockclientmgr->getState();
    printf("------------------ END PMStateShutdownServices_getState----------------------------\n");
}

TEST_F(TestPMStateShutdownServices, PMStateShutdownServices_sendSignalShutdownServices)
{
    printf("------------------ BEGIN PMStateShutdownServices_sendSignalShutdownServices ----------------------------\n");
//    StateShutdownServices *lockclientmgr= new StateShutdownServices();
//    lockclientmgr->sendSignalShutdownServices();
    printf("------------------ END PMStateShutdownServices_sendSignalShutdownServices----------------------------\n");
}
