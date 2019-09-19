// @@@LICENSE
//
//      Copyright (c) 2017-2019 LG Electronics, Inc.
//
// Confidential computer software. Valid license from LG required for
// possession, use or copying. Consistent with FAR 12.211 and 12.212,
// Commercial Computer Software, Computer Software Documentation, and
// Technical Data for Commercial Items are licensed to the U.S. Government
// under vendor's standard commercial license.
//
// LICENSE@@@

#include <cstdio>
#include <string>       // std::string

#include "pmscore/log.h"
#include "pmscore/PmsConfigLinux.h"
#include "pmscore/Singleton.h"
#include "pmscore/StateManager.h"

#include "PowerManagerService.h"

const char* const PowerManagerService::kPmsInterfaceName = "LunaInterfacePMSRoot";
const char* const PowerManagerService::kPmsLogContext = "LogContext";

PowerManagerService::PowerManagerService(GMainLoop *mainLoop):
    mLoopdata(mainLoop)
{
    std::string logContext;

    mpConfig = new PmsConfigLinux(PMS_CONF_FILE_PATH);

    PmsErrorCode_t err = kPmsSuccess;
    const char* logCtxt = LOG_CONTEXT_DEBUG; //default logging context

    // TODO: Change The Logging Context, make it based on class names
    err = mpConfig->GetString(kPmsInterfaceName, kPmsLogContext, &logContext);

    if(err == kPmsSuccess)
    logCtxt = logContext.c_str();
    mpLog = new Logger(logCtxt);

    MSG_DEBUG("Logger Instance created - PMS");
}

PowerManagerService::~PowerManagerService()
{
    delete mpConfig;
    mpConfig = nullptr;

    delete mpClientBlock;
    mpClientBlock = nullptr;

    delete mpLog;
    mpLog = nullptr;
}

bool PowerManagerService::init()
{
    PmsErrorCode_t err = kPmsSuccess;

    mpClientBlock = new ClientBlock(mpConfig, mLoopdata);
    if (mpClientBlock)
    {
        err = mpClientBlock->Initialize();
        if (err != kPmsSuccess)
            return false;
    }

    mpClientBlock->Start();

    if (!initializeStateManager())
    {
        MSG_INFO("Error StateMachine: Initialization Failed");
    }

    return true;
}

void PowerManagerService::deinit()
{
    MSG_DEBUG("[%s]", __PRETTY_FUNCTION__);
    PmsErrorCode_t err = kPmsSuccess;

    if (!deinitializeStateManager())
    {
        MSG_INFO("Error Deinitializing  StateMachine");
    }

    if (mpClientBlock) {
        mpClientBlock->Stop();

        err = mpClientBlock->Deinitialize();

        if (err != kPmsSuccess) {
            MSG_INFO("Error Deinitializing  ClientBlock");
        }
    }
}

bool PowerManagerService::initializeStateManager() {
    return stateReference::GetInstance().init();
}

bool PowerManagerService::deinitializeStateManager() {
    return stateReference::GetInstance().deinit();
}
