/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef SATELLITE_SMS_CLIENT_H
#define SATELLITE_SMS_CLIENT_H

#include <cstdint>

#include "gsm_sms_receive_handler.h"
#include "gsm_sms_sender.h"
#include "i_satellite_service.h"
#include "iremote_object.h"
#include "satellite/i_satellite_sms_service.h"
#include "singleton.h"

namespace OHOS {
namespace Telephony {
class SatelliteSmsClient : public DelayedRefSingleton<SatelliteSmsClient> {
    DECLARE_DELAYED_REF_SINGLETON(SatelliteSmsClient);

public:
    sptr<ISatelliteSmsService> GetProxy();
    void OnRemoteDied(const wptr<IRemoteObject> &remote);

    bool GetSatelliteSupported();
    bool IsSatelliteEnabled();
    int32_t GetSatelliteCapability();
    int32_t RegisterSmsNotify(int32_t slotId, int32_t what, const sptr<ISatelliteSmsCallback> &callback);
    int32_t UnRegisterSmsNotify(int32_t slotId, int32_t what);
    int32_t AddSendHandler(int32_t slotId, const std::shared_ptr<TelEventHandler> sender);
    int32_t AddReceiveHandler(int32_t slotId, const std::shared_ptr<TelEventHandler> receiver);

    int32_t SendSms(int32_t slotId, int32_t eventId, SatelliteMessage &message);
    int32_t SendSmsMoreMode(int32_t slotId, int32_t eventId, SatelliteMessage &message);
    int32_t SendSmsAck(int32_t slotId, int32_t eventId, bool success, int32_t cause);

private:
    sptr<ISatelliteService> GetServiceProxy();
    void ServiceOn();
    void ServiceOff();

    void RemoveDeathRecipient(const wptr<IRemoteObject> &remote, bool isRemoteDied);
    class SatelliteServiceDeathRecipient : public IRemoteObject::DeathRecipient {
    public:
        explicit SatelliteServiceDeathRecipient(SatelliteSmsClient &client) : client_(client) {}
        ~SatelliteServiceDeathRecipient() override = default;
        void OnRemoteDied(const wptr<IRemoteObject> &remote) override
        {
            client_.OnRemoteDied(remote);
        }

    private:
        SatelliteSmsClient &client_;
    };

    class SystemAbilityListener : public SystemAbilityStatusChangeStub {
    public:
        SystemAbilityListener() {}
        ~SystemAbilityListener() {}

    public:
        void OnAddSystemAbility(int32_t systemAbilityId, const std::string &deviceId) override;
        void OnRemoveSystemAbility(int32_t systemAbilityId, const std::string &deviceId) override;
    };

private:
    std::mutex mutexProxy_;
    sptr<ISystemAbilityStatusChange> statusChangeListener_ { nullptr };
    sptr<ISatelliteService> satelliteServiceProxy_ { nullptr };
    sptr<ISatelliteSmsService> proxy_ { nullptr };
    sptr<IRemoteObject::DeathRecipient> deathRecipient_ { nullptr };
    std::map<int32_t, std::shared_ptr<TelEventHandler>> senderMap_;
    std::map<int32_t, std::shared_ptr<TelEventHandler>> receiverMap_;
};
} // namespace Telephony
} // namespace OHOS
#endif // SATELLITE_SMS_CLIENT_H
