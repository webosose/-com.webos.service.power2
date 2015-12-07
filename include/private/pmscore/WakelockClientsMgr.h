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

#ifndef _WAKELOCKCLIENTSMGR_H_
#define _WAKELOCKCLIENTSMGR_H_

#include <string>

class WakelockClientsMgr
{
    public:
        virtual ~WakelockClientsMgr() = default;
        virtual void addClient(const std::string &clientId,
                               const std::string &clientName) = 0;
        virtual void removeClient(const std::string &clientId) = 0;
        virtual void removeClientByName(const std::string &clientName) = 0;
        virtual void setWakelock(const std::string &clienId, int timeout) = 0;
        virtual void clearWakelock(const std::string &clientId) = 0;
        virtual std::string getClientIdByName(const std::string &clientName) = 0;
        virtual bool isClientExist(const std::string &clienId) = 0;
        virtual bool isWakelockSet(const std::string &clientId) = 0;
        virtual int getWakelockTimeout(const std::string &clientId) const = 0;
        virtual int getWakelockCount() const = 0;
        virtual void updateClientRegistered(const std::string &clientId, bool isRegister) = 0;
};
#endif /* _WAKELOCKCLIENTSMGR_H_ */

