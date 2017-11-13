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
#include "pmscore/NyxUtil.h"
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
    bool result = pmService->init();
    EXPECT_FALSE(result);
    printf("------------------ END PowerManagerService_init----------------------------\n");
}
