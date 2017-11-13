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

#ifndef LUNA_PMS_ROOT_H
#define LUNA_PMS_ROOT_H

#include <unordered_set>
#include <glib.h>

#include "pmscore/log.h"
#include "pmscore/LunaInterfaceBase.h"
#include "pmscore/PmsLuna2Utils.h"
#include "pmscore/StateEventListners.h"
#include "pmscore/StateManager.h"

#include "PowerStateClientsMgr.h"
#include "timersource.h"

class LunaPmsRoot : public LunaInterfaceBase, public StateEventListners
{
public:

    /**
     * The object name string. This is initialized in .cpp file
     */
    static const char* const kPmsInterfaceName;
    static const char* const kPmsLogContext;
    static const char* const kPmsTimeOut;
    /*
     * Object Creator of Luna Service Handler
     */
    static LunaPmsRoot* CreateObject(PmsConfig* pConfig, LSHandle* pLsHandle)
    {
        return new LunaPmsRoot(pConfig, pLsHandle);
    }

    virtual ~LunaPmsRoot( )
    {
        Stop();
        UnregisterIpc();

        delete mPowerStatesMgr;
        mPowerStatesMgr = nullptr;
    }

    GTimerSource *sTimerCheck = nullptr;

    PmsErrorCode_t Stop();
    //To Handle Fixed State and Transition State
    bool handleStateChange(const std::string& statename);
    bool handleTransitionState(const std::string& processing);
    bool handlePowerOnReason(const std::string& mPowerOnReason);

    static bool addSubscription(LSHandle *sh, LSMessage* message, const std::string key);
    static bool addSubscription(LSHandle *sh, LSMessage* message,  std::string statename, int priority);
    bool setAlarm(int timeout);
    bool setAwake(int timeout, LS::Message &request, std::string clientName, LSHandle *sh);
    bool setDbKind();
    bool putInDb();
    bool clearAlarm(const std::string &key);
    static gboolean timerFired(gpointer);
    std::string generateRandomString( size_t length );

    //LSMethods Start
    static bool setState(LSHandle *sh, LSMessage *message, void *data);
    static bool getState(LSHandle *sh, LSMessage *message, void *data);
    static bool registerStateTransition(LSHandle *sh, LSMessage *message, void *data);
    static bool respondStateTransitionAck(LSHandle *sh, LSMessage *message, void *data);
    static bool reboot(LSHandle *sh, LSMessage *message, void *data);
    static bool shutdown(LSHandle *sh, LSMessage *message, void *data);
    static bool notifyStateTransition(LSHandle *sh, LSMessage *message, void *data);
    static bool getPowerOnReason(LSHandle *sh, LSMessage *message, void *data);
    static bool setPowerOnReason(LSHandle *sh, LSMessage *message, void *data);
    static bool notifyAlarmExpiry(LSHandle *sh, LSMessage *message, void *data);
    static bool acquireWakeLock(LSHandle *sh, LSMessage *message, void *data);
    static bool releaseWakeLock(LSHandle *sh, LSMessage *message, void *data);
    //LSMethods End

    bool setStateCb(LSHandle *sh, LSMessage *message, void *data);
    bool getStateCb(LSHandle *sh, LSMessage *message, void *data);
    bool registerStateTransitionCb(LSHandle *sh, LSMessage *message, void *data);
    bool respondStateTransitionAckCb(LSHandle *sh, LSMessage *message, void *data);
    bool rebootCb(LSHandle *sh, LSMessage *message, void *data);
    bool shutdownCb(LSHandle *sh, LSMessage *message, void *data);
    bool notifyStateTransitionCb(LSHandle *sh, LSMessage *message, void *data);
    bool getPowerOnReasonCb(LSHandle *sh, LSMessage *message, void *data);
    bool setPowerOnReasonCb(LSHandle *sh, LSMessage *message, void *data);
    bool notifyAlarmExpiryCb(LSHandle *sh, LSMessage *message, void *data);
    bool acquireWakeLockCb(LSHandle *sh, LSMessage *message, void *data);
    bool releaseWakeLockCb(LSHandle *sh, LSMessage *message, void *data);

    PmsErrorCode_t CancelSubscriptionStateChange (LSMessage* message);

    static unsigned int subscribersCount;

private:
    typedef enum {
        REASON_WAKE_ON_LAN,
        REASON_WAKE_ON_WIFI,
        REASON_REMOTE_KEY,
        REASON_on_Timer,
        MAX_REASONS
        }ERR_CODE_T;

    std::string reasonString[5] = {
        "wakeOnLan",
        "wakeOnWiFi",
        "remoteKey",
        "onTimer"
        };
    /**
     * Constructor of LUNA State handler
     */
    LunaPmsRoot(PmsConfig* pConfig, LSHandle *pLsHandle);
    bool handleEvent(const std::string& event);
    bool forceStateChange(const std::string& statename);

    GMainLoop *mLoopdata = nullptr;

    /**
    * @brief Register the CreateObject() method of the LunaInterfaceCall class with the Interface
    * Object factory. This registration happens automatically when the Pms library is loaded
    */
    static bool RegisterObject()
    {
      return (sLunaInterfaceFactory::GetInstance().Register(LunaPmsRoot::kPmsInterfaceName, &LunaPmsRoot::CreateObject));
    }

    /**
     * Register with the client methods and also with the LUNA BUS for the LUNA commands
     */
    void RegisterIpc( LSHandle *pLsHandle );

    std::string mMyServiceName;
    std::string mCurrentState;

    PowerStateClientsMgr *mPowerStatesMgr;

    static std::string mPowerOnReason;

    /**
     * This is the methods table, used during the registartion with
     * the LUNA bus.
     */
    static LSMethod mStateMethodsTable[];

    PmsConfig* mpConfig;
    Logger* mpLog;

    LSMessage *pendingMsg;
    std::vector<LSMessage *> pendingReply;

    /**
     * @var Used to Check if the registration operation to the object factory has
     * succeeded. This variable is assigned to the method Register()
     */
    static bool mIsObjRegistered;
    static bool mIsTransitionState;

    static const char* mpLogContext;
    static int mpLogTimeOutSec;

    std::string mSessionToken;
    std::unordered_set<std::string> capturedReplies; // Two ways to check No registered client reply twice.
};

#endif //LUNA_INTERFACE_STATE_H
