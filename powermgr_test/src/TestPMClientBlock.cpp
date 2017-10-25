/* @@@LICENSE
 *
 * Copyright (c) 2017 LG Electronics, Inc.
 *
 * Confidential computer software. Valid license from LG required for
 * possession, use or copying. Consistent with FAR 12.211 and 12.212,
 * Commercial Computer Software, Computer Software Documentation, and
 * Technical Data for Commercial Items are licensed to the U.S. Government
 * under vendor's standard commercial license.
 *
 * LICENSE@@@
 */

#include <gtest/gtest.h>
#include <pbnjson.hpp>

#include "PMSCommon.h"
#include "NyxUtil.h"
#include "TestUtil.h"
#include "ClientBlock.h"
#include "PmsConfigLinux.h"

static GMainLoop *mainLoop = nullptr;

class TestPMClientBlock: public ::testing::Test {
    public:
        TestPMClientBlock()
        {
            printf("TestPMClientBlock constructor entered\n");
            printf("TestPMClientBlock constructor exit\n");
        }

        ~TestPMClientBlock() {
            printf("====TestPMClientBlock destructor entered ===\n");
            printf("====TestPMClientBlock destructor exit ===\n");
        }
};

TEST_F(TestPMClientBlock, ClientBlock_Initialize)
{
    printf("------------------ BEGIN ClientBlock_Initialize ----------------------------\n");
    PmsConfig* pConfig = new PmsConfigLinux(PMS_CONF_FILE_PATH);
    mainLoop = g_main_loop_new(NULL, FALSE);
    ClientBlock *ptrClientBlk= new ClientBlock(pConfig, mainLoop);
    ptrClientBlk->Initialize();
    printf("------------------ END ClientBlock_Initialize----------------------------\n");
}

TEST_F(TestPMClientBlock, ClientBlock_Start)
{
    printf("------------------ BEGIN ClientBlock_Start ----------------------------\n");
    PmsConfig* pConfig = new PmsConfigLinux(PMS_CONF_FILE_PATH);
    mainLoop = g_main_loop_new(NULL, FALSE);
    ClientBlock *ptrClientBlk= new ClientBlock(pConfig, mainLoop);
    ptrClientBlk->Start();
    printf("------------------ END ClientBlock_Start----------------------------\n");
}

TEST_F(TestPMClientBlock, ClientBlock_Deinitialize)
{
    printf("------------------ BEGIN ClientBlock_Deinitialize ----------------------------\n");
    PmsConfig* pConfig = new PmsConfigLinux(PMS_CONF_FILE_PATH);
    mainLoop = g_main_loop_new(NULL, FALSE);
    ClientBlock *ptrClientBlk= new ClientBlock(pConfig, mainLoop);
    ptrClientBlk->Deinitialize();
    printf("------------------ END ClientBlock_Deinitialize----------------------------\n");
}

TEST_F(TestPMClientBlock, ClientBlock_InitializeIpc)
{
    printf("------------------ BEGIN ClientBlock_InitializeIpc ----------------------------\n");
    /* PmsConfig* pConfig = new PmsConfigLinux(PMS_CONF_FILE_PATH);
    mainLoop = g_main_loop_new(NULL, FALSE);
    ClientBlock *ptrClientBlk= new ClientBlock(pConfig, mainLoop);
    ptrClientBlk->InitializeIpc(); */
    printf("------------------ END ClientBlock_InitializeIpc----------------------------\n");
}

TEST_F(TestPMClientBlock, ClientBlock_DeinitializeIpc)
{
    printf("------------------ BEGIN ClientBlock_DeinitializeIpc ----------------------------\n");
    /* PmsConfig* pConfig = new PmsConfigLinux(PMS_CONF_FILE_PATH);
    mainLoop = g_main_loop_new(NULL, FALSE);
    ClientBlock *ptrClientBlk= new ClientBlock(pConfig, mainLoop);
    ptrClientBlk->DeinitializeIpc(); */
    printf("------------------ END ClientBlock_DeinitializeIpc----------------------------\n");
}