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

#ifndef JUTIL_H
#define JUTIL_H

#include <pbnjson.hpp>
#include <string>
#include <map>

#include "Singleton.hpp"

// List of utilites for pbnjson
class JUtil: public Singleton<JUtil> {
public:
    //! Error class used in JUtil
    class Error {
    public:
        typedef enum {
            None = 0, File_Io, Schema, Parse,
        } ErrorCode;

        //! Constructor
        Error();

        //! Return ErrorCode
        ErrorCode code();

        //! Return Error Detail string
        std::string detail();

    private:
        friend class JUtil;

        /*! Set Error code and detail string.
         * If detail value is NULL, detail value set as default error message.
         */
        void set(ErrorCode code, const char *detail = NULL);

    private:
        ErrorCode m_code;
        std::string m_detail;
    };

    /*! Parse given json data using schema.
     * If schemaName is empty, use JSchemaFragment("{}")
     */
    static pbnjson::JValue parse(const char *rawData,
            const std::string &schemaName, Error *error = NULL,
            pbnjson::JResolver *schemaResolver = NULL);

    /*! Parse given json file path using schema.
     * If schemaName is empty, use JSchemaFragment("{}")
     */
    static pbnjson::JValue parseFile(const std::string &path,
            const std::string &schemaName, Error *error = NULL,
            pbnjson::JResolver *schemaResolver = NULL);

    /*! Load schema from file.
     * If schemaName is empty, return JSchemaFragment("{}")
     * Schema path is set to "/etc/palm/schemas/sam/" + schemaName + ".schema"
     * If cache set, find cache first and if not exist in cache load schema and store it.
     */
    pbnjson::JSchema loadSchema(const std::string &schemaName, bool cache);

    //! Convert json object to std::string
    static std::string jsonToString(pbnjson::JValue json);

protected:
    friend class Singleton<JUtil> ;

    //! Constructor
    JUtil();

    //! Destructor
    ~JUtil();

private:
    std::map<std::string, pbnjson::JSchema> m_mapSchema;
};
#endif /* JUTIL_H */
