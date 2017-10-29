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

#include <string>
#include <map>
#include "Singleton.hpp"

//! List of global settings like default path or something like this
class GlobalSettings {
public:
    //! return path to test data
    std::string get_test_data_path();

    //! Set path to test data. Can be used if you would like to work with custom data.
    void set_test_data_path(const std::string &path);

    /*! replace values internall of the string by template.
     * for example /home/usr/src/testdata/my_app by {TST_DATA_PATH}/my_app
     */
    void replace_value_by_template(std::string *in_out);

    /*! replace specific templates with the value
     * for example {TST_DATA_PATH}/my_app into /home/usr/src/testdata/my_app
     */
    void replace_template_by_value(std::string *in_out);

    //! Return user readable key-value description of internal settings (path etc)
    std::string get_settings_list();
protected:
    friend class Singleton<GlobalSettings> ;
    GlobalSettings();
private:
    std::string m_data_path;
    std::map<std::string, std::string> value_list;
};

typedef Singleton<GlobalSettings> GlobalSettingsImpl;
