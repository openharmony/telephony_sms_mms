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
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
const std::vector<std::string> CT_ICCID_ARRAY = { "898603", "898606", "898611", "8985302", "8985307" };
const std::vector<std::string> CT_CPLMNS = { "46003", "46005", "46011", "46012", "47008", "45002", "45007" };
constexpr int32_t ICCID_LEN_MINIMUM = 7;
constexpr int32_t PREFIX_LOCAL_ICCID_LEN = 4;
constexpr const char *PREFIX_LOCAL_ICCID = "8986";
constexpr int32_t ICCID_LEN_SIX = 6;

SmsNetworkPolicyManager::SmsNetworkPolicyManager(
    const std::shared_ptr<AppExecFwk::EventRunner> &runner, int32_t slotId)
    : AppExecFwk::EventHandler(runner), slotId_(slotId)
{}

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
    GetImsRegState();
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
        case NotificationType::NOTIFICATION_TYPE_IMS:
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

bool SmsNetworkPolicyManager::IsCtSimCard()
{
    CardType cardType = CardType::UNKNOWN_CARD;
    CoreManagerInner::GetInstance().GetCardType(slotId_, cardType);
    bool isCTCardType = false;
    bool result = false;
    TELEPHONY_LOGI("[slot%{public}d] cardType = %{public}d", slotId_, cardType);
    switch (cardType) {
        case CardType::SINGLE_MODE_USIM_CARD:
        case CardType::CT_NATIONAL_ROAMING_CARD:
        case CardType::DUAL_MODE_TELECOM_LTE_CARD:
            isCTCardType = true;
            break;
        default:
            isCTCardType = false;
            break;
    }
    if (isCTCardType) {
        std::u16string tempIccId;
        CoreManagerInner::GetInstance().GetSimIccId(slotId_, tempIccId);
        std::string iccid = Str16ToStr8(tempIccId);
        if (!iccid.empty() && iccid.length() >= ICCID_LEN_MINIMUM) {
            std::string subIccId = iccid.substr(0, ICCID_LEN_MINIMUM);
            if (!subIccId.empty() && (subIccId.compare(0, PREFIX_LOCAL_ICCID_LEN, PREFIX_LOCAL_ICCID) == 0) &&
                subIccId.length() >= ICCID_LEN_MINIMUM) {
                subIccId = subIccId.substr(0, ICCID_LEN_SIX);
            }
            auto iccIdRet = find(CT_ICCID_ARRAY.begin(), CT_ICCID_ARRAY.end(), subIccId);
            result = iccIdRet != CT_ICCID_ARRAY.end();
        } else {
            result = false;
        }
    } else {
        result = isCTCardType;
    }
    if (!result) {
        std::u16string operatorNumeric;
        CoreManagerInner::GetInstance().GetSimOperatorNumeric(slotId_, operatorNumeric);
        std::string cplmn = Str16ToStr8(operatorNumeric);
        if (!cplmn.empty()) {
            auto cplmnRet = find(CT_CPLMNS.begin(), CT_CPLMNS.end(), cplmn);
            result = cplmnRet != CT_CPLMNS.end();
        }
    }
    TELEPHONY_LOGI("[slot%{public}d] result = %{public}d", slotId_, result);
    return result;
}

void SmsNetworkPolicyManager::GetRadioState()
{
    if (IsCtSimCard()) {
        netWorkType_ = NET_TYPE_CDMA;
    } else {
        netWorkType_ = NET_TYPE_GSM;
    }

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

void SmsNetworkPolicyManager::GetImsRegState()
{
    ImsServiceType imsSrvType = TYPE_VOICE;
    callback_ = (std::make_unique<ImsRegStateCallbackStub>(shared_from_this())).release();
    int32_t ret = CoreServiceClient::GetInstance().RegisterImsRegInfoCallback(slotId_, imsSrvType, callback_);
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
} // namespace Telephony
} // namespace OHOS