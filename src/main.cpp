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

#include <csignal>

#include "pmscore/log.h"
#include "pmscore/PMSCommon.h"
#include "pmscore/PowerManagerService.h"

static GMainLoop *mainLoop = nullptr;

void term_handler(int32_t signum)
{
    const char *str = nullptr;

    switch (signum) {
        case SIGTERM:
            str = "SIGTERM";
            break;

        case SIGABRT:
            str = "SIGABRT";
            break;

        default:
            str = "Unknown";
            break;
    }

    MSG_DEBUG2(LOG_CONTEXT_DEBUG, "signal received.. signal[%s]", str);
    g_main_loop_quit(mainLoop);
}

int main(int argc, char *argv[])
{
    signal(SIGTERM, term_handler);
    signal(SIGABRT, term_handler);

    mainLoop = g_main_loop_new(NULL, FALSE);

    //! Initiate logging
    PmsLog::log_context_init();

    if (!mainLoop) {
        MSG_DEBUG2(LOG_CONTEXT_DEBUG, "Unable to initiate g_main_loop");
        return EXIT_FAILURE;
    }

    std::unique_ptr<PowerManagerService> pmService(new PowerManagerService(mainLoop));

    if (!pmService || !pmService->init()) {
        MSG_DEBUG2(LOG_CONTEXT_DEBUG, "PMS service registration failed");
        g_main_loop_unref(mainLoop);
        return EXIT_FAILURE;
    }

    g_main_loop_run(mainLoop);
    g_main_loop_unref(mainLoop);

    return EXIT_SUCCESS;
}

