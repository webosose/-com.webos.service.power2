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

#include "ShutdownClientsMgrImpl.h"

ShutdownClientsMgrImpl::~ShutdownClientsMgrImpl()
{
    mClients.clear();
}

void ShutdownClientsMgrImpl::addClient(const std::string &clientId,
                                       const std::string &clientName)
{
    ShutdownClient client;
    client.mClientName = clientName;
    mClients[clientId] = client;
}

void ShutdownClientsMgrImpl::removeClient(const std::string &clientId)
{
    if (isClientExist(clientId)) {
        mClients.erase(clientId);
    }
}

bool ShutdownClientsMgrImpl::isClientExist(const std::string &clientId)
{
    const auto &it = mClients.find(clientId);

    if (mClients.end() == it) {
        return false;
    }

    return true;
}

bool ShutdownClientsMgrImpl::setAck(const std::string &clientId, bool ack,
                                    double timeElapsed)
{
    const auto &it = mClients.find(clientId);

    if (mClients.end() == it) {
        return false;
    }

    ShutdownClient &client = it->second;
    client.mTimeElapsed = timeElapsed;
    client.mAck = ack;
    return true;
}

bool ShutdownClientsMgrImpl::isAllClientsAcknowledged()
{
    for (const auto &client : mClients) {
        if (!client.second.mAck) {
            return false;
        }
    }

    return true;
}

