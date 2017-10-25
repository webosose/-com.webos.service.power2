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

#include "pmscore/PowerStateClientsMgrImpl.h"

PowerStateClientsMgrImpl::PowerStateClientsMgrImpl()
{
    mpLog = new Logger(LOG_CONTEXT_DEBUG);
}

PowerStateClientsMgrImpl::~PowerStateClientsMgrImpl()
{
    mClients.clear();

    delete mpLog;
    mpLog = nullptr;
}

void PowerStateClientsMgrImpl::addClient(const std::string &stateName,const std::string  &clientName)
{
    mClients[stateName].insert(clientName);
    MSG_DEBUG("Client Added: %s", clientName.c_str());
}

bool PowerStateClientsMgrImpl::removeClient(const std::string &statename, const std::string &clientName)
{
    const auto &it = mClients.find(statename);

    if (mClients.end() == it) {
        return false;
    }

    const auto &iter = it->second.find(clientName);

    if (iter != it->second.end()) {
        it->second.erase(iter);
        MSG_DEBUG("Client Removed: %s", clientName);
        return true;
    }
    return false;
}

bool PowerStateClientsMgrImpl::updateClientRegistered(const std::string &clientId, bool isRegister)
{
    const auto &it = mClients.find(clientId);

    if (mClients.end() == it) {
        return false;
    }

    return isRegister;
}

bool PowerStateClientsMgrImpl::isClientExist(const std::string &statename, const std::string  &clientName)
{
    const auto &it = mClients.find(statename);

    if (mClients.end() == it) {
        return false;
    }

    return (it->second.find(clientName) != it->second.end()) ? true : false;
}

unsigned int PowerStateClientsMgrImpl::getPowerStateCount(const std::string &clientId) const
{
    const auto &it = mClients.find(clientId);

    if (mClients.end() == it) {
        return 0;
    }
    return it->second.size();
}
