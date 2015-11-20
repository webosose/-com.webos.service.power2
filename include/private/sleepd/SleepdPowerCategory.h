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

#ifndef _SLEEPDPOWERCATEGORY_H_
#define _SLEEPDPOWERCATEGORY_H_

#include <luna-service2/lunaservice.hpp>

class PowerManagerService;

class SleepdPowerCategory
{
    public:
        SleepdPowerCategory(PowerManagerService &refHandle, LS::Handle &sleepdLsHandle, LS::Handle &powerdLsHandle);
        virtual ~SleepdPowerCategory() = default;
        bool init(bool isPowerdUp);

        SleepdPowerCategory(const SleepdPowerCategory &) = delete;
        SleepdPowerCategory &operator=(const SleepdPowerCategory &) = delete;

        static bool systemTimeChangedCallback(LSHandle *sh, LSMessage *message, void *ctx);
        bool systemTimeChanged(LSMessage &message);
        bool identify(LSMessage &message);
        bool wakeLockRegister(LSMessage &message);
        bool setWakeLock(LSMessage &message);
        bool clientCancelByName(LSMessage &message);
        bool somebodyWantsWakeup(LSMessage &message);
        bool activityStart(LSMessage &message);
        bool activityEnd(LSMessage &message);
        void addSubscription(LSMessage &message);
        void registerPowerdMethods(LS::Handle &powerdLsHandle);

        //Powerd Apis
        bool battchargingStatusQuery(LSMessage &message);
        bool batteryStatusQuery(LSMessage &message);
        bool chargerStatusQuery(LSMessage &message);
        bool getFakeBatteryMode(LSMessage &message);
        bool setFakeBatteryMode(LSMessage &message);
        bool suspendRequestRegister(LSMessage &message);
        bool prepareSuspendRegister(LSMessage &message);
        bool suspendRequestAck(LSMessage &message);
        bool batterySaverOnOff(LSMessage &message);

    private:
        bool _batterydServiceStatusCb(LSHandle *sh, const char *serviceName, bool connected);
        bool checkBatterydStatus();
        int readLineFromFile(const char *path, char *buf, size_t count);
        static bool batteryStatusQuerySignal(LSHandle *sh, LSMessage *message, void *user_data);
        static bool chargerStatusQuerySignal(LSHandle *sh, LSMessage *message, void *user_data);
        static bool getFakeBatteryModeCallback(LSHandle *sh, LSMessage *message, void *ctx);
        static bool setFakeBatteryModeCallback(LSHandle *sh, LSMessage *message, void *ctx);
        static bool chargerStatusCallback(LSHandle *sh, LSMessage *message, void *ctx);
        static bool batteryStatusCallback(LSHandle *sh, LSMessage *message, void *ctx);
        static bool batterydServiceStatusCb(LSHandle *sh,
                                            const char *serviceName,
                                            bool connected,
                                            void *ctx)
        {
            return ((SleepdPowerCategory *) ctx)->_batterydServiceStatusCb(sh, serviceName, connected);
        }

        static bool chargerConnected(LSHandle *sh, LSMessage *message, void *user_data);

    private:
        PowerManagerService &mRefPms;
        LS::Handle &mRefSleepdLsHandle;
        LS::Handle &mRefPowerdLsHandle;
        LSHandle     *mPowerdHandle = nullptr;
        bool          mIsPowerdUp = false;
        bool          mIsRedirectionAvailable = false;
        bool          mIsChargerPresent = false;
        void          *mPowerdCookie;
        void          *mBatterydCookie;
};

#endif /* _SLEEPDPOWERCATEGORY_H_ */
