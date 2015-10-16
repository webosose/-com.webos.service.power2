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
#include "WakelockClientsMgrImpl.h"

class TestPMWakelockClientsMgrImpl: public ::testing::Test {
    public:
        TestPMWakelockClientsMgrImpl()
        {
            printf("TestPMWakelockClientsMgrImpl constructor entered\n");
            printf("TestPMWakelockClientsMgrImpl constructor exit\n");
        }

        ~TestPMWakelockClientsMgrImpl() {
            printf("====TestPMWakelockClientsMgrImpl destructor entered ===\n");
            printf("====TestPMWakelockClientsMgrImpl destructor exit ===\n");
        }
};

TEST_F(TestPMWakelockClientsMgrImpl, PMWakelockClientsMgrImpl_addClient)
{
    printf("------------------ BEGIN PMWakelockClientsMgrImpl_addClient ----------------------------\n");
    WakelockClientsMgrImpl *lockclientmgr= new WakelockClientsMgrImpl();
    std::string clientId;
    std::string clientName;
    lockclientmgr->addClient(clientId,clientName);
    printf("------------------ END PMWakelockClientsMgrImpl_addClient----------------------------\n");
}

TEST_F(TestPMWakelockClientsMgrImpl, PMWakelockClientsMgrImpl_removeClient)
{
    printf("------------------ BEGIN PMWakelockClientsMgrImpl_removeClient ----------------------------\n");
    std::string clientId;
    WakelockClientsMgrImpl *lockclientmgr= new WakelockClientsMgrImpl();
    lockclientmgr->removeClient(clientId);
    printf("------------------ END PMWakelockClientsMgrImpl_removeClient----------------------------\n");
}

TEST_F(TestPMWakelockClientsMgrImpl, PMWakelockClientsMgrImpl_setWakelock)
{
    printf("------------------ BEGIN PMWakelockClientsMgrImpl_setWakelock ----------------------------\n");
    WakelockClientsMgrImpl *lockclientmgr= new WakelockClientsMgrImpl();
    std::string clientId;
    int timeout = 0;
    lockclientmgr->setWakelock(clientId, timeout);
    printf("------------------ END PMWakelockClientsMgrImpl_setWakelock----------------------------\n");
}

TEST_F(TestPMWakelockClientsMgrImpl, PMWakelockClientsMgrImpl_clearWakelock)
{
    printf("------------------ BEGIN PMWakelockClientsMgrImpl_clearWakelock ----------------------------\n");
    WakelockClientsMgrImpl *lockclientmgr= new WakelockClientsMgrImpl();
    std::string clientId;
    lockclientmgr->clearWakelock(clientId);
    printf("------------------ END PMWakelockClientsMgrImpl_clearWakelock----------------------------\n");
}

TEST_F(TestPMWakelockClientsMgrImpl, PMWakelockClientsMgrImpl_isClientExist)
{
    printf("------------------ BEGIN PMWakelockClientsMgrImpl_isClientExist ----------------------------\n");
    WakelockClientsMgrImpl *lockclientmgr= new WakelockClientsMgrImpl();
    std::string clientId;
    lockclientmgr->isClientExist(clientId);
    printf("------------------ END PMWakelockClientsMgrImpl_isClientExist----------------------------\n");
}

TEST_F(TestPMWakelockClientsMgrImpl, PMWakelockClientsMgrImpl_isWakelockSet)
{
    printf("------------------ BEGIN PMWakelockClientsMgrImpl_isWakelockSet ----------------------------\n");
    WakelockClientsMgrImpl *lockclientmgr= new WakelockClientsMgrImpl();
    std::string clientId;
    lockclientmgr->isWakelockSet(clientId);
    printf("------------------ END PMWakelockClientsMgrImpl_isWakelockSet----------------------------\n");
}

TEST_F(TestPMWakelockClientsMgrImpl, PMWakelockClientsMgrImpl_getWakelockTimeout)
{
    printf("------------------ BEGIN PMWakelockClientsMgrImpl_getWakelockTimeout ----------------------------\n");
    WakelockClientsMgrImpl *lockclientmgr= new WakelockClientsMgrImpl();
    std::string clientId;
    lockclientmgr->getWakelockTimeout(clientId);
    printf("------------------ END PMWakelockClientsMgrImpl_getWakelockTimeout----------------------------\n");
}

TEST_F(TestPMWakelockClientsMgrImpl, PMWakelockClientsMgrImpl_getWakelockCount)
{
    printf("------------------ BEGIN PMWakelockClientsMgrImpl_getWakelockCount ----------------------------\n");
    WakelockClientsMgrImpl *lockclientmgr= new WakelockClientsMgrImpl();
    lockclientmgr->getWakelockCount();
    printf("------------------ END PMWakelockClientsMgrImpl_getWakelockCount----------------------------\n");
}
