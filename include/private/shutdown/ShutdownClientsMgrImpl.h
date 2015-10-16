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

#ifndef _SHUTDOWNCLIENTSMGRIMPL_H_
#define _SHUTDOWNCLIENTSMGRIMPL_H_

#include <map>

#include "ShutdownClientsMgr.h"

class ShutdownClientsMgrImpl: public ShutdownClientsMgr
{
    public:
        ShutdownClientsMgrImpl() = default;
        ~ShutdownClientsMgrImpl();

        void addClient(const std::string &clientId, const std::string &clientName) override;
        void removeClient(const std::string &clientId) override;
        bool isClientExist(const std::string &clientId) override;
        bool setAck(const std::string &clientId, bool ack, double timeElapsed) override;
        bool isAllClientsAcknowledged() override;

    private:
        class ShutdownClient
        {
            public:
                std::string mClientName;
                bool mAck = false;
                double mTimeElapsed = 0;
        };
        std::map<std::string, ShutdownClient> mClients;
};

#endif /* _SHUTDOWNCLIENTSMGRIMPL_H_ */

