// @@@LICENSE
//
//      Copyright (c) 2017-2018 LG Electronics, Inc.
//
// Confidential computer software. Valid license from LG required for
// possession, use or copying. Consistent with FAR 12.211 and 12.212,
// Commercial Computer Software, Computer Software Documentation, and
// Technical Data for Commercial Items are licensed to the U.S. Government
// under vendor's standard commercial license.
//
// LICENSE@@@

#ifndef POWERSTATECLIENTSMGR_H
#define POWERSTATECLIENTSMGR_H

#include <string>

class PowerStateClientsMgr
{
    public:
        virtual ~PowerStateClientsMgr() = default;
        virtual void addClient(const std::string &stateName,
                               const std::string &clientName) = 0;
        virtual bool removeClient(const std::string &statename, const std::string &clientName) = 0;
        virtual bool isClientExist(const std::string &statename, const std::string  &clientName) = 0;
        virtual unsigned int getPowerStateCount(const std::string &clientId) const = 0;
        virtual bool updateClientRegistered(const std::string &clientId, bool isRegister) = 0;
};
#endif /* POWERSTATECLIENTSMGR_H */
