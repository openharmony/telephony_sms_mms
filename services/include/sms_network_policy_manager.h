/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef SMS_NETWORK_POLICY_MANAGER_H
#define SMS_NETWORK_POLICY_MANAGER_H

#include <functional>
#include <map>
#include <optional>

#include "event_handler.h"
#include "event_runner.h"
#include "ims_reg_state_callback_stub.h"
#include "network_state.h"
#include "sms_common.h"

namespace OHOS {
namespace Telephony {
enum NotificationType {
    NOTIFICATION_TYPE_IMS = 600,
};

class SmsNetworkPolicyManager : public AppExecFwk::EventHandler {
public:
    SmsNetworkPolicyManager(const std::shared_ptr<AppExecFwk::EventRunner> &runner, int32_t slotId);
    virtual ~SmsNetworkPolicyManager() = default;
    virtual void ProcessEvent(const AppExecFwk::InnerEvent::Pointer &event) override;
    int32_t IsImsSmsSupported(int32_t slotId, bool &isSupported);
    int32_t GetImsShortMessageFormat(std::u16string &format);
    void Init();
    void UnRegisterHandler();
    NetWorkType GetNetWorkType();
    bool IsImsNetDomain() const;
    void GetRadioState();
    int32_t GetVoiceServiceState() const;
    std::optional<int32_t> NetworkRegister(
        const std::function<void(bool isImsNetDomain, int32_t serviceState)> &callback);
    std::optional<int32_t> NetworkRegister(
        const std::function<void(bool isImsNetDomain, int32_t serviceState)> &&callback);
    void NetworkUnregister(int32_t id);

protected:
    void HandlerRadioState(const AppExecFwk::InnerEvent::Pointer &event);
    void RegisterHandler();

private:
    int32_t GetId();
    void GetImsRegState();
    bool IsCtSimCard();

private:
    int32_t slotId_;
    enum NetWorkType netWorkType_ = NetWorkType::NET_TYPE_UNKNOWN;
    bool isImsNetDomain_ = false;
    int32_t voiceServiceState_ = static_cast<int32_t>(RegServiceState::REG_STATE_UNKNOWN);
    std::map<int32_t, const std::function<void(bool isImsNetDomain, int32_t serviceState)>> callbackMap_;
    int32_t id_ = 0;
    sptr<ImsRegInfoCallback> callback_ = nullptr;
};
} // namespace Telephony
} // namespace OHOS
#endif // SMS_NETWORK_POLICY_MANAGER_H
