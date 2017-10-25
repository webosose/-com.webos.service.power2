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

#include "PMSCommon.h"
#include "PmsLuna2Utils.h"
#include "PowerManagerService.h"
#include "NyxUtil.h"
#include "TestUtil.h"

static GMainLoop *mainLoop = nullptr;

class TestPMPowerManagerService: public ::testing::Test {
    public:
        TestPMPowerManagerService()
        {
            printf("TestPMPowerManagerService constructor entered\n");
            printf("TestPMPowerManagerService constructor exit\n");
        }

        ~TestPMPowerManagerService() {
            printf("====TestPMPowerManagerService destructor entered ===\n");
            printf("====TestPMPowerManagerService destructor exit ===\n");
        }
};

TEST_F(TestPMPowerManagerService, PowerManagerService_init)
{
    printf("------------------ BEGIN PowerManagerService_init ----------------------------\n");
    mainLoop = g_main_loop_new(NULL, FALSE);
    std::unique_ptr<PowerManagerService> pmService(new PowerManagerService(mainLoop));
    pmService->init();
    printf("------------------ END PowerManagerService_init----------------------------\n");
}

TEST_F(TestPMPowerManagerService, PowerManagerService_initSM)
{
    printf("------------------ BEGIN PowerManagerService_initSM ----------------------------\n");
    mainLoop = g_main_loop_new(NULL, FALSE);
    std::unique_ptr<PowerManagerService> pmService(new PowerManagerService(mainLoop));
    //pmService->initSM();
    printf("------------------ END PowerManagerService_initSM----------------------------\n");
}

TEST_F(TestPMPowerManagerService, PowerManagerService_cbSMAPI1)
{
    printf("------------------ BEGIN PowerManagerService_cbSMAPI1 ----------------------------\n");
    mainLoop = g_main_loop_new(NULL, FALSE);
    std::unique_ptr<PowerManagerService> pmService(new PowerManagerService(mainLoop));
    //pmService->cbSMAPI1();
    printf("------------------ END PowerManagerService_cbSMAPI1----------------------------\n");
}

TEST_F(TestPMPowerManagerService, PowerManagerService_cbSMAPI2)
{
    printf("------------------ BEGIN PowerManagerService_cbSMAPI2 ----------------------------\n");
    mainLoop = g_main_loop_new(NULL, FALSE);
    std::unique_ptr<PowerManagerService> pmService(new PowerManagerService(mainLoop));
    //pmService->cbSMAPI2();
    printf("------------------ END PowerManagerService_cbSMAPI2----------------------------\n");
}