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

#ifndef PMS_MGR_SERVICE_H
#define PMS_MGR_SERVICE_H

#include <glib.h>

#include "pmscore/ClientBlock.h"
#include "pmscore/log.h"

class PowerManagerService
{
    public:
        PowerManagerService(GMainLoop *mainLoop);
        ~PowerManagerService();
        bool init();
        void deinit();

    private:
        bool initializeStateManager();
        bool deinitializeStateManager();
        GMainLoop *mLoopdata;
        ClientBlock* mpClientBlock;
        PmsConfig*  mpConfig;
        Logger* mpLog;

        static const char* const kPmsInterfaceName;
        static const char* const kPmsLogContext;
};

#endif //PMS_MGR_SERVICE_H

