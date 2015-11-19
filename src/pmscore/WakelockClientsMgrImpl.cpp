// @@@LICENSE
//
//      Copyright (c) 2015 LG Electronics, Inc.
//
// Confidential computer software. Valid license from LG required for
// possession, use or copying. Consistent with FAR 12.211 and 12.212,
// Commercial Computer Software, Computer Software Documentation, and
// Technical Data for Commercial Items are licensed to the U.S. Government
// under vendor's standard commercial license.
//
// LICENSE@@@

#include "WakelockClientsMgrImpl.h"

WakelockClientsMgrImpl::~WakelockClientsMgrImpl()
{
    mClients.clear();
}

void WakelockClientsMgrImpl::addClient(const std::string &clientId,
                                       const std::string &clientName)
{
    if (isClientExist(clientId)) {
        return;
    }

    WakelockClient client;
    client.mName = clientName;

    mClients[clientId] = client;
}

void WakelockClientsMgrImpl::removeClient(const std::string &clientId)
{
    if (isClientExist(clientId)) {
        mClients.erase(clientId);
    }
}

void WakelockClientsMgrImpl::removeClientByName(const std::string &clientName)
{
    std::string clientId;

    //get clientId if client exists
    for (const auto &client : mClients) {
        if (client.second.mName == clientName) {
            clientId = client.first;
            break;
        }
    }

    //remove the clientInfo if it exist
    if (!clientId.empty()) {
        mClients.erase(clientId);
    }
}

void WakelockClientsMgrImpl::setWakelock(const std::string &clientId, int timeout)
{
    const auto &it = mClients.find(clientId);

    if (mClients.end() == it) {
        return;
    }

    WakelockClient &client = it->second;

    if (client.mRegWakeLock && client.mWakelockSet) {
        return;
    }

    client.mWakelockSet = true;
    client.mTimeout = timeout;
    mWakelocksCount++;
}

void WakelockClientsMgrImpl::clearWakelock(const std::string &clientId)
{
    const auto &it = mClients.find(clientId);

    if (mClients.end() == it) {
        return;
    }

    WakelockClient &client = it->second;
    client.mWakelockSet = false;
    client.mTimeout = 0;

    if (mWakelocksCount > 0) {
        mWakelocksCount--;
    }
}

void  WakelockClientsMgrImpl::updateClientRegistered(const std::string &clientId, bool isRegister)
{
    const auto &it = mClients.find(clientId);

    if (mClients.end() == it) {
        return;
    }

    WakelockClient &client = it->second;
    client.mRegWakeLock = isRegister;
}

bool WakelockClientsMgrImpl::isClientExist(const std::string &clientId)
{
    return mClients.find(clientId) != mClients.end();
}

bool WakelockClientsMgrImpl::isWakelockSet(const std::string &clientId)
{
    const auto &it = mClients.find(clientId);

    if (mClients.end() == it) {
        return 0;
    }

    return it->second.mWakelockSet;
}

int WakelockClientsMgrImpl::getWakelockTimeout(const std::string &clientId) const
{
    const auto &it = mClients.find(clientId);

    if (mClients.end() == it) {
        return 0;
    }

    return it->second.mTimeout;
}

int WakelockClientsMgrImpl::getWakelockCount() const
{
    return mWakelocksCount;
}

