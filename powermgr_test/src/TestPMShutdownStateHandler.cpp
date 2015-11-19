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
#include "ShutdownStateHandler.h"

class TestPMShutdownStateHandler:public ::testing::Test {
    public:
        TestPMShutdownStateHandler()
        {
            printf("TestPMShutdownStateHandler constructor entered\n");
            printf("TestPMShutdownStateHandler constructor exit\n");
        }

        ~TestPMShutdownStateHandler() {
            printf("====TestPMShutdownStateHandler destructor entered ===\n");
            printf("====TestPMShutdownStateHandler destructor exit ===\n");
        }
};

TEST_F(TestPMShutdownStateHandler, PMShutdownStateHandler_setState)
{
    printf("------------------ BEGIN PMShutdownStateHandler_setState ----------------------------\n");
    ShutdownStateHandler *lockclientmgr= new ShutdownStateHandler();
    ShutdownState state=kPowerShutdownNone;
    lockclientmgr->setState(state);
    printf("------------------ END PMShutdownStateHandler_setState----------------------------\n");
}

TEST_F(TestPMShutdownStateHandler, PMShutdownStateHandler_getState)
{
    printf("------------------ BEGIN PMShutdownStateHandler_getState ----------------------------\n");
    ShutdownStateHandler *lockclientmgr= new ShutdownStateHandler();
    lockclientmgr->getState();
    printf("------------------ END PMShutdownStateHandler_getState----------------------------\n");
}
/*
TEST_F(TestPMShutdownStateHandler, PMShutdownStateHandler_stateDispatch)
{
    printf("------------------ BEGIN PMShutdownStateHandler_stateDispatch ----------------------------\n");
    ShutdownStateHandler *lockclientmgr= new ShutdownStateHandler();
    ShutdownEvent event = ShutdownEventNone;
    lockclientmgr->stateDispatch(event);
    printf("------------------ END PMShutdownStateHandler_stateDispatch----------------------------\n");
}
*/
TEST_F(TestPMShutdownStateHandler, PMShutdownStateHandler_setMoveNext)
{
    printf("------------------ BEGIN PMShutdownStateHandler_setMoveNext ----------------------------\n");
    ShutdownStateHandler *lockclientmgr= new ShutdownStateHandler();
    bool next = true;
    lockclientmgr->setMoveNext(next);
    printf("------------------ END PMShutdownStateHandler_setMoveNext----------------------------\n");
}

TEST_F(TestPMShutdownStateHandler, PMShutdownStateHandler_moveToNextState)
{
    printf("------------------ BEGIN PMShutdownStateHandler_moveToNextState ----------------------------\n");
    ShutdownStateHandler *lockclientmgr= new ShutdownStateHandler();
    lockclientmgr->moveToNextState();
    printf("------------------ END PMShutdownStateHandler_moveToNextState----------------------------\n");
}
