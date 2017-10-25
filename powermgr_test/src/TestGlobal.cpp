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

#include "TestGlobal.h"
#include <unistd.h>

static void replaceAll(std::string *str, const std::string &from,
        const std::string &to) {
    if (from.empty()) {
        return;
    }

    size_t start_pos = 0;

    while ((start_pos = str->find(from, start_pos)) != std::string::npos) {
        str->replace(start_pos, from.length(), to);
        start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
    }
}

GlobalSettings::GlobalSettings() {
    // find test_data at the current executable's path.
    char exe_name[512] = { 0, };
    int len = readlink("/proc/self/exe", exe_name, sizeof(exe_name));

    if (len > 0) {
        m_data_path = exe_name;
        m_data_path = m_data_path.substr(0, m_data_path.find_last_of("/"));
        m_data_path += "/test_data";
    }

    value_list["{TEST_DATA}"] = m_data_path;
    value_list["{INST_APP}"] = "apps/applications";
    value_list["{INST_PKG}"] = "apps/packages";
    value_list["{USER_INST_APP}"] = "user_apps/apps/applications";
    value_list["{DEV_INST_APP}"] = "dev_apps/apps/applications";
    value_list["{DEV_INST_PKG}"] = "dev_apps/apps/packages";
}

std::string GlobalSettings::get_test_data_path() {
    return m_data_path;
}

void GlobalSettings::set_test_data_path(const std::string &path) {
    m_data_path = path;
}

std::string GlobalSettings::get_settings_list() {
    std::string result;
    std::map<std::string, std::string>::iterator pit = value_list.begin();

    for (; pit != value_list.end(); ++pit) {
        result = pit->first + "  = " + pit->second + "\n";
    }

    return result;
}

void GlobalSettings::replace_value_by_template(std::string *in_out) {
    std::map<std::string, std::string>::iterator pit = value_list.begin();

    for (; pit != value_list.end(); ++pit) {
        replaceAll(in_out, pit->second, pit->first);
    }
}

void GlobalSettings::replace_template_by_value(std::string *in_out) {
    std::map<std::string, std::string>::iterator pit = value_list.begin();

    for (; pit != value_list.end(); ++pit) {
        replaceAll(in_out, pit->first, pit->second);
    }
}
