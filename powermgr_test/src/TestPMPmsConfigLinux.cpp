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

#include "PMSCommon.h"
#include "PmsLuna2Utils.h"
#include "PowerManagerService.h"
#include "PmsLogging.h"
#include "NyxUtil.h"
#include "TestUtil.h"
#include "PmsConfigLinux.h"

class TestPMPmsConfigLinux: public ::testing::Test {
    public:
        TestPMPmsConfigLinux()
        {
            printf("TestPMPmsConfigLinux constructor entered\n");
            printf("TestPMPmsConfigLinux constructor exit\n");
        }

        ~TestPMPmsConfigLinux() {
            printf("====TestPMPmsConfigLinux destructor entered ===\n");
            printf("====TestPMPmsConfigLinux destructor exit ===\n");
        }
};

TEST_F(TestPMPmsConfigLinux, PmsConfigLinux_GetString)
{
    printf("------------------ BEGIN PmsConfigLinux_GetString ----------------------------\n");
    //! Test Case 1
    const std::string kPmsBlockName = "PMS";
    static const std::string kPmsIpcName = "Ipc";
    static const std::string kPmsIpcType = "Type";
    std::string ipcConfig;

    PmsConfigLinux *ptrPmsCnfLinux= new PmsConfigLinux(PMS_CONF_FILE_PATH);
    ptrPmsCnfLinux->GetString(kPmsBlockName, kPmsIpcName, &ipcConfig);
    printf("------------------ END PmsConfigLinux_GetString----------------------------\n");
}

TEST_F(TestPMPmsConfigLinux, PmsConfigLinux_GetStringList)
{
    printf("------------------ BEGIN PmsConfigLinux_GetStringList ----------------------------\n");
    //! Test Case 1
    const std::string kPmsBlockName = "PMS";
    static const std::string kPmsIpcName = "Ipc";
    static const std::string kPmsIpcType = "Type";
    std::vector<std::string> ipcConfig;

    PmsConfigLinux *ptrPmsCnfLinux= new PmsConfigLinux(PMS_CONF_FILE_PATH);
    ptrPmsCnfLinux->GetStringList(kPmsBlockName, kPmsIpcName, &ipcConfig);
    printf("------------------ END PmsConfigLinux_GetStringList----------------------------\n");
}