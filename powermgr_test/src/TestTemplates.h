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

#ifndef TESTTEMPLATES_H
#define TESTTEMPLATES_H

#include <gtest/gtest.h>
#include <luna-service2/lunaservice.h>
#include <string>
#include <pbnjson.hpp>
#include "TestUtil.h"

//#include "FakeLunaServiceProvider.h"

/* Used for initialization and deinitialization of test environment
 * * Initialize local path
 * * Initialize FakeLunaServiceProvider
 * * Hide g_log messages
 * * Initialize ApplicationManager
 */
class TestEnvGuard {
public:
    TestEnvGuard();
    ~TestEnvGuard();

    void init();
private:
    //! Uses for hiding error messages from standart output
    static void log_function(const gchar */*log_domain*/,
            GLogLevelFlags /*log_level*/, const gchar */*message*/,
            gpointer /*user_data*/);
    GLogFunc m_func;
};

#endif // TESTTEMPLATES_H
