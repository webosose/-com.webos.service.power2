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
#include <string>
#include <glib.h>

#include "TestGlobal.h"

//! Uses as temporary variable for test_data path changes
static char *data_path(NULL);

static GOptionEntry entries[] = { { "test_data", 'd', 0, G_OPTION_ARG_STRING,
        &data_path, "Set custom test data path", NULL }, { NULL } };

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);

    GError *error = NULL;
    GOptionContext *context;

    context = g_option_context_new("- test application");
    g_option_context_add_main_entries(context, entries, NULL);
    g_option_context_set_ignore_unknown_options(context, true);

    if (!g_option_context_parse(context, &argc, &argv, &error)) {
        g_warning("%s", error->message);
        g_option_context_free(context);
        return EXIT_FAILURE;
    }

    g_option_context_free(context);

    if (data_path) {
        GlobalSettingsImpl::instance().set_test_data_path(data_path);
    }

    return RUN_ALL_TESTS();
}

