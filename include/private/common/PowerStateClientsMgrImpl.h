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

#ifndef POWERSTATECLIENTSMGRIMPL_H
#define POWERSTATECLIENTSMGRIMPL_H

#include <map>
#include <set>

#include "pmscore/log.h"
#include "PowerStateClientsMgr.h"

class PowerStateClientsMgrImpl: public PowerStateClientsMgr
{
    public:
        PowerStateClientsMgrImpl();
        ~PowerStateClientsMgrImpl();

        void addClient(const std::string &stateName, const std::string &clientName);
        bool removeClient(const std::string &statename, const std::string &clientName);
        bool isClientExist(const std::string &statename, const std::string  &clientName);
        bool updateClientRegistered(const std::string &clientId, bool isRegister);
        unsigned int getPowerStateCount(const std::string &clientId) const;

    private:
        std::map<std::string, std::set<std::string>> mClients;
        Logger* mpLog;
};

#endif /* POWERSTATECLIENTSMGRIMPL_H */
