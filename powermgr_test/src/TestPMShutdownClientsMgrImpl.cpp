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

#include <memory>

#include "PMSCommon.h"
#include "PmsLuna2Utils.h"
#include "PowerManagerService.h"
#include "PmsLogging.h"
#include "ShutdownCategoryMethods.h"
#include "NyxUtil.h"
#include "TestUtil.h"
#include "ShutdownClientsMgrImpl.h"

class TestPMShutdownClientsMgrImpl: /*  public DBQuery , */public ::testing::Test {
public:
    TestPMShutdownClientsMgrImpl() //:DBQuery(NULL)
    {
        printf("TestPMShutdownClientsMgrImpl constructor entered\n");
        printf("TestPMShutdownClientsMgrImpl constructor exit\n");
    }

    ~TestPMShutdownClientsMgrImpl() {
        printf("====TestPMShutdownClientsMgrImpl destructor entered ===\n");
        printf("====TestPMShutdownClientsMgrImpl destructor exit ===\n");
    }
};

TEST_F(TestPMShutdownClientsMgrImpl, PMShutdownClientsMgrImpl_addClient)
{
    printf("------------------ BEGIN PMShutdownClientsMgrImpl_addClient ----------------------------\n");
    std::unique_ptr<ShutdownClientsMgr> lockclientmgr(new ShutdownClientsMgrImpl());
    std::string clientId("/var/run/ls2/d2DecF.4");
    std::string clientName("com.palm.dummy");
    lockclientmgr->addClient(clientId, clientName);
    printf("------------------ END PMShutdownClientsMgrImpl_addClient----------------------------\n");
}

TEST_F(TestPMShutdownClientsMgrImpl, PMShutdownClientsMgrImpl_removeClient)
{
    printf("------------------ BEGIN PMShutdownClientsMgrImpl_removeClient ----------------------------\n");
    std::string clientId;
    ShutdownClientsMgrImpl *lockclientmgr = new ShutdownClientsMgrImpl();
    lockclientmgr->removeClient(clientId);
    printf("------------------ END PMShutdownClientsMgrImpl_removeClient----------------------------\n");
}

TEST_F(TestPMShutdownClientsMgrImpl, PMShutdownClientsMgrImpl_isClientExist)
{
    printf("------------------ BEGIN PMShutdownClientsMgrImpl_isClientExist ----------------------------\n");
    ShutdownClientsMgrImpl *lockclientmgr= new ShutdownClientsMgrImpl();
    std::string clientId;
    lockclientmgr->isClientExist(clientId);
    printf("------------------ END PMShutdownClientsMgrImpl_isClientExist----------------------------\n");
}

TEST_F(TestPMShutdownClientsMgrImpl, PMShutdownClientsMgrImpl_setAck)
{
    printf("------------------ BEGIN PMShutdownClientsMgrImpl_getWakelockTimeout ----------------------------\n");
    ShutdownClientsMgrImpl *lockclientmgr= new ShutdownClientsMgrImpl();
    std::string clientId;
    lockclientmgr->setAck(clientId, true, 10.0);
    printf("------------------ END PMShutdownClientsMgrImpl_getWakelockTimeout----------------------------\n");
}

