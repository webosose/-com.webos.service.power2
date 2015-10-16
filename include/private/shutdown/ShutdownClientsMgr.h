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

#ifndef _SHUTDOWNCLIENTSMGR_H_
#define _SHUTDOWNCLIENTSMGR_H_

#include <string>

class ShutdownClientsMgr
{
    public:
        virtual ~ShutdownClientsMgr() = default;
        virtual void addClient(const std::string &clientId,
                               const std::string &clientName) = 0;
        virtual void removeClient(const std::string &clientId) = 0;
        virtual bool isClientExist(const std::string &clientId) = 0;
        virtual bool setAck(const std::string &clientId, bool ack,
                            double timeElapsed) = 0;
        virtual bool isAllClientsAcknowledged() = 0;
};

#endif /* _SHUTDOWNCLIENTSMGR_H_ */

