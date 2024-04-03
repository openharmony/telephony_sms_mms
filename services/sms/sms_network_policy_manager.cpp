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
#include "core_service_client.h"
#include "ims_reg_info_callback_stub.h"
#include "radio_event.h"
#include "sms_service.h"
#include "sms_persist_helper.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
SmsNetworkPolicyManager::SmsNetworkPolicyManager(int32_t slotId)
    : TelEventHandler("SmsNetworkPolicyManager"), slotId_(slotId)
{}

void SmsNetworkPolicyManager::Init()
{
    RegisterHandler();
}

void SmsNetworkPolicyManager::RegisterHandler()
{
    TELEPHONY_LOGI("SmsNetworkPolicyManager::RegisterHandler Ok.");
    CoreManagerInner &coreInner = CoreManagerInner::GetInstance();
    coreInner.RegisterCoreNotify(slotId_, shared_from_this(), RadioEvent::RADIO_PS_CONNECTION_ATTACHED, nullptr);
    coreInner.RegisterCoreNotify(slotId_, shared_from_this(), RadioEvent::RADIO_PS_CONNECTION_DETACHED, nullptr);
    coreInner.RegisterCoreNotify(slotId_, shared_from_this(), RadioEvent::RADIO_ON, nullptr);
    coreInner.RegisterCoreNotify(slotId_, shared_from_this(), RadioEvent::RADIO_OFF, nullptr);
    coreInner.RegisterCoreNotify(slotId_, shared_from_this(), RadioEvent::RADIO_STATE_CHANGED, nullptr);
    coreInner.RegisterCoreNotify(slotId_, shared_from_this(), RadioEvent::RADIO_PS_RAT_CHANGED, nullptr);
    coreInner.RegisterCoreNotify(slotId_, shared_from_this(), RadioEvent::RADIO_NETWORK_STATE, nullptr);
    coreInner.RegisterCoreNotify(slotId_, shared_from_this(), RadioEvent::RADIO_IMS_NETWORK_STATE_CHANGED, nullptr);
    coreInner.RegisterCoreNotify(slotId_, shared_from_this(), RadioEvent::RADIO_RIL_ADAPTER_HOST_DIED, nullptr);
    coreInner.RegisterCoreNotify(slotId_, shared_from_this(), RadioEvent::RADIO_FACTORY_RESET, nullptr);
    GetRadioState();
    GetImsRegState();
}

void SmsNetworkPolicyManager::UnRegisterHandler()
{
    CoreManagerInner &coreInner = CoreManagerInner::GetInstance();
    coreInner.UnRegisterCoreNotify(slotId_, shared_from_this(), RadioEvent::RADIO_PS_CONNECTION_ATTACHED);
    coreInner.UnRegisterCoreNotify(slotId_, shared_from_this(), RadioEvent::RADIO_PS_CONNECTION_DETACHED);
    coreInner.UnRegisterCoreNotify(slotId_, shared_from_this(), RadioEvent::RADIO_ON);
    coreInner.UnRegisterCoreNotify(slotId_, shared_from_this(), RadioEvent::RADIO_OFF);
    coreInner.UnRegisterCoreNotify(slotId_, shared_from_this(), RadioEvent::RADIO_STATE_CHANGED);
    coreInner.UnRegisterCoreNotify(slotId_, shared_from_this(), RadioEvent::RADIO_PS_RAT_CHANGED);
    coreInner.UnRegisterCoreNotify(slotId_, shared_from_this(), RadioEvent::RADIO_NETWORK_STATE);
    coreInner.UnRegisterCoreNotify(slotId_, shared_from_this(), RadioEvent::RADIO_IMS_NETWORK_STATE_CHANGED);
    coreInner.UnRegisterCoreNotify(slotId_, shared_from_this(), RadioEvent::RADIO_RIL_ADAPTER_HOST_DIED);
    coreInner.UnRegisterCoreNotify(slotId_, shared_from_this(), RadioEvent::RADIO_FACTORY_RESET);
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
    TELEPHONY_LOGD("SmsNetworkPolicyManager::ProcessEvent Handler Rec%{public}d", eventId);
    switch (eventId) {
        case RadioEvent::RADIO_ON:
        case RadioEvent::RADIO_OFF:
        case RadioEvent::RADIO_STATE_CHANGED:
        case RadioEvent::RADIO_PS_RAT_CHANGED:
        case RadioEvent::RADIO_NETWORK_STATE:
        case RadioEvent::RADIO_IMS_NETWORK_STATE_CHANGED:
        case RadioEvent::RADIO_PS_CONNECTION_DETACHED:
        case RadioEvent::RADIO_PS_CONNECTION_ATTACHED:
        case NotificationType::NOTIFICATION_TYPE_IMS:
            GetRadioState();
            break;
        case RadioEvent::RADIO_RIL_ADAPTER_HOST_DIED:
            DelayedSingleton<SmsService>::GetInstance()->OnRilAdapterHostDied(slotId_);
            break;
        case RadioEvent::RADIO_FACTORY_RESET:
            HandleFactoryReset();
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
    bool isCTSimCard = false;
    CoreManagerInner::GetInstance().IsCTSimCard(slotId_, isCTSimCard);
    if (isCTSimCard) {
        netWorkType_ = NET_TYPE_CDMA;
    } else {
        netWorkType_ = NET_TYPE_GSM;
    }

    ImsRegInfo info;
    CoreManagerInner::GetInstance().GetImsRegStatus(slotId_, ImsServiceType::TYPE_SMS, info);
    isImsNetDomain_ = info.imsRegState == ImsRegState::IMS_REGISTERED;
    voiceServiceState_ = CoreManagerInner::GetInstance().GetCsRegState(slotId_);
    TELEPHONY_LOGD("netWorkType_ = %{public}d isImsNetDomain_ = %{public}s GetCsRegStatus = %{public}d", netWorkType_,
        isImsNetDomain_ ? "true" : "false", voiceServiceState_);
    for (const auto &item : callbackMap_) {
        if (item.second == nullptr) {
            TELEPHONY_LOGE("callbackList's item is nullptr");
            continue;
        }
        TELEPHONY_LOGD("update network info.");
        item.second(isImsNetDomain_, voiceServiceState_);
    }
}

void SmsNetworkPolicyManager::GetImsRegState()
{
    ImsServiceType imsSrvType = TYPE_VOICE;
    sptr<ImsRegInfoCallback> callback = new ImsRegStateCallbackStub(shared_from_this());
    int32_t ret = CoreServiceClient::GetInstance().RegisterImsRegInfoCallback(slotId_, imsSrvType, callback);
    TELEPHONY_LOGI("SmsNetworkPolicyManager::GetImsRegState ret:%{public}d", ret);
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

void SmsNetworkPolicyManager::HandleFactoryReset()
{
    DataShare::DataSharePredicates predicates;
    predicates.EqualTo(SmsSubsection::SLOT_ID, std::to_string(slotId_));
    bool ret = DelayedSingleton<SmsPersistHelper>::GetInstance()->Delete(predicates);
    TELEPHONY_LOGI("sms factory reset ret:%{public}d", ret);
}
} // namespace Telephony
} // namespace OHOS