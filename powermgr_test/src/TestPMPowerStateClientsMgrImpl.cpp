// @@@LICENSE
//
//      Copyright (c) 2017 LG Electronics, Inc.
//
// Confidential computer software. Valid license from LG required for
// possession, use or copying. Consistent with FAR 12.211 and 12.212,
// Commercial Computer Software, Computer Software Documentation, and
// Technical Data for Commercial Items are licensed to the U.S. Government
// under vendor's standard commercial license.
//
// LICENSE@@@

#include <gtest/gtest.h>
#include <pbnjson.hpp>

#include "pmscore/PMSCommon.h"
#include "pmscore/PmsLuna2Utils.h"
#include "PowerManagerService.h"
#include "pmscore/PmsLogging.h"
#include "pmscore/NyxUtil.h"
#include "PowerStateClientsMgrImpl.h"
#include "TestUtil.h"

class TestPMPowerStateClientsMgrImpl: public ::testing::Test {
    public:
        TestPMPowerStateClientsMgrImpl()
        {
            printf("TestPMPowerStateClientsMgrImpl constructor entered\n");
            printf("TestPMPowerStateClientsMgrImpl constructor exit\n");
        }

        ~TestPMPowerStateClientsMgrImpl() {
            printf("====TestPMPowerStateClientsMgrImpl destructor entered ===\n");
            printf("====TestPMPowerStateClientsMgrImpl destructor exit ===\n");
        }
};

TEST_F(TestPMPowerStateClientsMgrImpl, PMPowerStateClientsMgrImpl_addClient)
{
    printf("------------------ BEGIN PMPowerStateClientsMgrImpl_addClient ----------------------------\n");

    PowerStateClientsMgrImpl *pwrstateclientmgr= new PowerStateClientsMgrImpl();
    //! Test Case 1
    std::string stateName = "ActiveState";
    std::string clientName = "com.webos.dummy";
    pwrstateclientmgr->addClient(stateName, clientName);

    //! Test Case2
    stateName = "ActiveStandByState";
    clientName = "com.webos.dummy";
    pwrstateclientmgr->addClient(stateName, clientName);

    //! Test Case 3
    stateName = "TransitionPowerOffState";
    clientName = "com.webos.testalarm";
    pwrstateclientmgr->addClient(stateName, clientName);

    printf("------------------ END PMPowerStateClientsMgrImpl_addClient----------------------------\n");
}

TEST_F(TestPMPowerStateClientsMgrImpl, PMPowerStateClientsMgrImpl_removeClient)
{
    printf("------------------ BEGIN PMPowerStateClientsMgrImpl_removeClient ----------------------------\n");
    PowerStateClientsMgrImpl *pwrstateclientmgr= new PowerStateClientsMgrImpl();
    //! Test Case 1
    std::string stateName = "ActiveState";
    std::string clientName = "com.webos.dummy";
    bool result = pwrstateclientmgr->removeClient(stateName, clientName);
    EXPECT_FALSE(result);

    //! Test Case 2
    stateName = "ActiveStandByState";
    clientName = "com.webos.dummy";
    pwrstateclientmgr->addClient(stateName, clientName);
    result = pwrstateclientmgr->removeClient(stateName, clientName);
    EXPECT_TRUE(result);

    //! Test Case 3
    stateName = "TransitionPowerOffState";
    clientName = "com.webos.dummy";
    pwrstateclientmgr->addClient(stateName, clientName);
    result = pwrstateclientmgr->removeClient(stateName, clientName);
    EXPECT_TRUE(result);

    printf("------------------ END PMPowerStateClientsMgrImpl_removeClient----------------------------\n");
}

TEST_F(TestPMPowerStateClientsMgrImpl, PMPowerStateClientsMgrImpl_updateClientRegistered)
{
    printf("------------------ BEGIN PMPowerStateClientsMgrImpl_updateClientRegistered ----------------------------\n");
    //! Test case 1
    std::string clientId;
    bool isRegister = true;
    PowerStateClientsMgrImpl *pwrstateclientmgr= new PowerStateClientsMgrImpl();
    bool result = pwrstateclientmgr->updateClientRegistered(clientId, isRegister);
    EXPECT_FALSE(result);

    //! Test case 2
    std::string stateName = "ActiveStandByState";
    std::string clientName = "com.webos.dummy";
    pwrstateclientmgr->addClient(stateName, clientName);
    result = pwrstateclientmgr->updateClientRegistered(stateName, isRegister);
    EXPECT_TRUE(result);

    printf("------------------ END PMPowerStateClientsMgrImpl_updateClientRegistered----------------------------\n");
}

TEST_F(TestPMPowerStateClientsMgrImpl, PMPowerStateClientsMgrImpl_isClientExist)
{
    printf("------------------ BEGIN PMPowerStateClientsMgrImpl_isClientExist ----------------------------\n");
    PowerStateClientsMgrImpl *pwrstateclientmgr= new PowerStateClientsMgrImpl();
    //! Test Case 1
    std::string stateName = "ActiveState";
    std::string clientName = "com.webos.testalarm";
    bool result = pwrstateclientmgr->isClientExist(stateName, clientName);
    EXPECT_FALSE(result);

    //! Test Case 2
    stateName = "ActiveStandByState";
    clientName = "com.webos.testalarm";
    pwrstateclientmgr->addClient(stateName, clientName);
    result = pwrstateclientmgr->isClientExist(stateName, clientName);
    EXPECT_TRUE(result);

    //! Test Case 3
    stateName = "TransitionPowerOffState";
    clientName = "com.webos.testalarm";
    pwrstateclientmgr->addClient(stateName, clientName);
    result = pwrstateclientmgr->isClientExist(stateName, clientName);
    EXPECT_TRUE(result);

    printf("------------------ END PMPowerStateClientsMgrImpl_isClientExist----------------------------\n");
}

TEST_F(TestPMPowerStateClientsMgrImpl, PMPowerStateClientsMgrImpl_getPowerStateCount)
{
    printf("------------------ BEGIN PMPowerStateClientsMgrImpl_getPowerStateCount ----------------------------\n");
    std::string clientId;
    PowerStateClientsMgrImpl *pwrstateclientmgr= new PowerStateClientsMgrImpl();
    pwrstateclientmgr->getPowerStateCount(clientId);
    printf("------------------ END PMPowerStateClientsMgrImpl_getPowerStateCount----------------------------\n");
}
