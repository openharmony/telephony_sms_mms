/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

#include "sms_network_policy_manager.h"

#include "core_manager_inner.h"
#include "radio_event.h"
#include "telephony_log_wrapper.h"
#include "ims_reg_types.h"

namespace OHOS {
namespace Telephony {
SmsNetworkPolicyManager::SmsNetworkPolicyManager(
    const std::shared_ptr<AppExecFwk::EventRunner> &runner, int32_t slotId)
    : AppExecFwk::EventHandler(runner), slotId_(slotId)
{}

SmsNetworkPolicyManager::~SmsNetworkPolicyManager()
{
    UnRegisterHandler();
}

void SmsNetworkPolicyManager::Init()
{
    RegisterHandler();
}

void SmsNetworkPolicyManager::RegisterHandler()
{
    TELEPHONY_LOGI("SmsNetworkPolicyManager::RegisterHandler Ok.");
    CoreManagerInner::GetInstance().RegisterCoreNotify(
        slotId_, shared_from_this(), RadioEvent::RADIO_PS_CONNECTION_ATTACHED, nullptr);
    CoreManagerInner::GetInstance().RegisterCoreNotify(
        slotId_, shared_from_this(), RadioEvent::RADIO_PS_CONNECTION_DETACHED, nullptr);
    CoreManagerInner::GetInstance().RegisterCoreNotify(
        slotId_, shared_from_this(), RadioEvent::RADIO_ON, nullptr);
    CoreManagerInner::GetInstance().RegisterCoreNotify(
        slotId_, shared_from_this(), RadioEvent::RADIO_OFF, nullptr);
    CoreManagerInner::GetInstance().RegisterCoreNotify(
        slotId_, shared_from_this(), RadioEvent::RADIO_STATE_CHANGED, nullptr);
    CoreManagerInner::GetInstance().RegisterCoreNotify(
        slotId_, shared_from_this(), RadioEvent::RADIO_PS_RAT_CHANGED, nullptr);
    CoreManagerInner::GetInstance().RegisterCoreNotify(
        slotId_, shared_from_this(), RadioEvent::RADIO_NETWORK_STATE, nullptr);
    CoreManagerInner::GetInstance().RegisterCoreNotify(
        slotId_, shared_from_this(), RadioEvent::RADIO_IMS_NETWORK_STATE_CHANGED, nullptr);
    GetRadioState();
}

void SmsNetworkPolicyManager::UnRegisterHandler()
{
    CoreManagerInner::GetInstance().UnRegisterCoreNotify(
        slotId_, shared_from_this(), RadioEvent::RADIO_PS_CONNECTION_ATTACHED);
    CoreManagerInner::GetInstance().UnRegisterCoreNotify(
        slotId_, shared_from_this(), RadioEvent::RADIO_PS_CONNECTION_DETACHED);
    CoreManagerInner::GetInstance().UnRegisterCoreNotify(slotId_, shared_from_this(), RadioEvent::RADIO_ON);
    CoreManagerInner::GetInstance().UnRegisterCoreNotify(slotId_, shared_from_this(), RadioEvent::RADIO_OFF);
    CoreManagerInner::GetInstance().UnRegisterCoreNotify(
        slotId_, shared_from_this(), RadioEvent::RADIO_STATE_CHANGED);
    CoreManagerInner::GetInstance().UnRegisterCoreNotify(
        slotId_, shared_from_this(), RadioEvent::RADIO_PS_RAT_CHANGED);
    CoreManagerInner::GetInstance().UnRegisterCoreNotify(
        slotId_, shared_from_this(), RadioEvent::RADIO_NETWORK_STATE);
    CoreManagerInner::GetInstance().UnRegisterCoreNotify(
        slotId_, shared_from_this(), RadioEvent::RADIO_IMS_NETWORK_STATE_CHANGED);
    callbackMap_.clear();
}

void SmsNetworkPolicyManager::ProcessEvent(const AppExecFwk::InnerEvent::Pointer &event)
{
    if (event == nullptr) {
        TELEPHONY_LOGE("SmsNetworkPolicyManager::ProcessEvent event == nullptr");
        return;
    }

    uint32_t eventId = 0;
    eventId = event->GetInnerEventId();
    TELEPHONY_LOGI("SmsNetworkPolicyManager::ProcessEvent Handler Rec%{public}d", eventId);
    switch (eventId) {
        case RadioEvent::RADIO_ON:
        case RadioEvent::RADIO_OFF:
        case RadioEvent::RADIO_STATE_CHANGED:
        case RadioEvent::RADIO_PS_RAT_CHANGED:
        case RadioEvent::RADIO_NETWORK_STATE:
        case RadioEvent::RADIO_IMS_NETWORK_STATE_CHANGED:
        case RadioEvent::RADIO_PS_CONNECTION_DETACHED:
        case RadioEvent::RADIO_PS_CONNECTION_ATTACHED:
            GetRadioState();
            break;
        default:
            break;
    }
}

NetWorkType SmsNetworkPolicyManager::GetNetWorkType()
{
    return netWorkType_;
}

void SmsNetworkPolicyManager::GetRadioState()
{
    netWorkType_ = static_cast<NetWorkType>(CoreManagerInner::GetInstance().GetPhoneType(slotId_));
    ImsRegInfo info;
    CoreManagerInner::GetInstance().GetImsRegStatus(slotId_, ImsServiceType::TYPE_SMS, info);
    isImsNetDomain_ = info.imsRegState == ImsRegState::IMS_REGISTERED;
    voiceServiceState_ = CoreManagerInner::GetInstance().GetCsRegState(slotId_);
    TELEPHONY_LOGI("netWorkType_ = %{public}d isImsNetDomain_ = %{public}s GetCsRegStatus = %{public}d",
        netWorkType_, isImsNetDomain_ ? "true" : "false", voiceServiceState_);
    for (const auto &item : callbackMap_) {
        if (item.second == nullptr) {
            TELEPHONY_LOGE("callbackList's item is nullptr");
            continue;
        }
        TELEPHONY_LOGI("update network info.");
        item.second(isImsNetDomain_, voiceServiceState_);
    }
}

bool SmsNetworkPolicyManager::IsImsNetDomain() const
{
    return isImsNetDomain_;
}

int32_t SmsNetworkPolicyManager::GetVoiceServiceState() const
{
    return voiceServiceState_;
}

std::optional<int32_t> SmsNetworkPolicyManager::NetworkRegister(
    const std::function<void(bool isImsNetDomain, int32_t voiceServiceState)> &callback)
{
    if (callback == nullptr) {
        TELEPHONY_LOGE("NetworkRegister is failed");
        return std::nullopt;
    }
    callback(isImsNetDomain_, voiceServiceState_);
    callbackMap_.emplace(GetId(), callback);
    return id_;
}

std::optional<int32_t>  SmsNetworkPolicyManager::NetworkRegister(
    const std::function<void(bool isImsNetDomain, int32_t voiceServiceState)> &&callback)
{
    if (callback == nullptr) {
        TELEPHONY_LOGE("NetworkRegister is failed");
        return std::nullopt;
    }
    callback(isImsNetDomain_, voiceServiceState_);
    callbackMap_.emplace(GetId(), std::move(callback));
    return id_;
}

int32_t SmsNetworkPolicyManager::GetId()
{
    return ++id_;
}

void SmsNetworkPolicyManager::NetworkUnregister(int32_t id)
{
    auto iter = callbackMap_.find(id);
    if (iter != callbackMap_.end()) {
        callbackMap_.erase(iter);
    } else {
        TELEPHONY_LOGE("NetworkUnregister id[%{public}d] is failed", id);
    }
}
} // namespace Telephony
} // namespace OHOS