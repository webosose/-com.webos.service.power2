// @@@LICENSE
//
//      Copyright (c) 2015-2016 LG Electronics, Inc.
//
// Confidential computer software. Valid license from LG required for
// possession, use or copying. Consistent with FAR 12.211 and 12.212,
// Commercial Computer Software, Computer Software Documentation, and
// Technical Data for Commercial Items are licensed to the U.S. Government
// under vendor's standard commercial license.
//
// LICENSE@@@

#include <csignal>

#include "PowerManagerService.h"
#include "PmsLogging.h"
#include "PMSCommon.h"
#include "SleepdCategoryMethods.h"

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

    PMSLOG_DEBUG("signal received.. signal[%s]", str);
    g_main_loop_quit(mainLoop);
}

int main(int argc, char *argv[])
{
    signal(SIGTERM, term_handler);
    signal(SIGABRT, term_handler);

    mainLoop = g_main_loop_new(NULL, FALSE);

    if (!mainLoop) {
        PMSLOG_ERROR(MSGID_MAINLOOP_FAIL, 0, "Unable to initiate g_main_loop");
        return EXIT_FAILURE;
    }

    std::unique_ptr<PowerManagerService> pmService(new PowerManagerService(mainLoop));

    if (!pmService || !pmService->init()) {
        PMSLOG_ERROR(MSGID_SRVC_REGISTER_FAIL, 0, "PMS service registration failed");
        g_main_loop_unref(mainLoop);
        return EXIT_FAILURE;
    }

#ifdef SLEEPD_BACKWARD_COMPATIBILITY
    std::unique_ptr<SleepdCategoryMethods> sleepdMethods(new SleepdCategoryMethods(*pmService, mainLoop));

    if (!(sleepdMethods && sleepdMethods->init())) {
        PMSLOG_ERROR(MSGID_SRVC_REGISTER_FAIL, 0, "Sleepd service registration failed");
        g_main_loop_unref(mainLoop);
        return EXIT_FAILURE;
    }
#endif

    if (!pmService->initPmSupportInterface())
    {
        PMSLOG_ERROR(MSGID_SRVC_REGISTER_FAIL, 0, "Library Init failed");
        g_main_loop_unref(mainLoop);
        return EXIT_FAILURE;
    }

    pmService->attachToLoop(mainLoop);

    g_main_loop_run(mainLoop);
    g_main_loop_unref(mainLoop);

    return EXIT_SUCCESS;
}

