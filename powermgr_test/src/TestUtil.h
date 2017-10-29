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

#ifndef TESTUTIL_H
#define TESTUTIL_H

#include <gtest/gtest.h>
#include <luna-service2/lunaservice.h>
#include <string>
#include <pbnjson.hpp>

//! List of useful for testing utulites and functions
class TestUtil {
public:
    //! Create or return main loop
    static GMainLoop *get_main_loop();

    //! Release main loop
    static void release_mainloop();

    //! Create json value from file, and replace templates with the values
    static pbnjson::JValue get_json_from_file(const std::string &path);

    /*! Jenerate plaiin text file from the json value and replace known patterns with the templates
     * for example:
     * TestUtil::put_json_to_file(INSTANCE(GlobalSettings).get_test_data_path()+"/my_file.txt", actual_value);
     */
    static void put_json_to_file(const std::string &path,
            const pbnjson::JValue &value);

    //! Return plain text file content
    static std::string get_file_content(const std::string &path);

    //! return pointer to the function from methid list by name of this pointer
    static LSMethodFunction get_cb_pointer(const char *name, LSMethod *methods);

    //! Create message and fill internal fields
    static LSMessage *create_message(LSHandle *sh, const char *category,
            const char *method, const char *payload, LSMessageToken token = 0);

    static const char *message_get_category(LSMessage *message);
    static const char *message_get_method(LSMessage *message);
    static const char *message_get_payload(LSMessage *message);

protected:
private:
    static GMainLoop *s_main_loop;
};
#endif // TESTUTIL_H
