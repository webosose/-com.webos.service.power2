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

#ifndef _WAKELOCKCLIENTSMGRIMPL_H_
#define _WAKELOCKCLIENTSMGRIMPL_H_

#include <map>

#include "WakelockClientsMgr.h"

class WakelockClientsMgrImpl: public WakelockClientsMgr
{
    public:
        WakelockClientsMgrImpl() = default;
        ~WakelockClientsMgrImpl();

        void addClient(const std::string &clientId, const std::string &clientName);
        void removeClient(const std::string &clientId);
        void removeClientByName(const std::string &clientName);
        void setWakelock(const std::string &clienId, int timeout);
        void clearWakelock(const std::string &clientId);
        bool isClientExist(const std::string &clienId);
        bool isWakelockSet(const std::string &clientId);
        int getWakelockTimeout(const std::string &clientId) const;
        int getWakelockCount() const;
        void updateClientRegistered(const std::string &clientId, bool isRegister);

    private:
        class WakelockClient
        {
            public:
                std::string mName;
                bool mWakelockSet = false;
                bool mRegWakeLock = false;
                int mTimeout = 0;
        };
        std::map<std::string, WakelockClient> mClients;
        int32_t mWakelocksCount  = 0;
};

#endif /* _WAKELOCKCLIENTSMGRIMPL_H_ */

