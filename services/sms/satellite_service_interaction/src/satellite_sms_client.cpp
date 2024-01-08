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

#include "satellite_sms_client.h"

#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "satellite_sms_proxy.h"
#include "telephony_errors.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
SatelliteSmsClient::SatelliteSmsClient()
{
    statusChangeListener_ = new (std::nothrow) SystemAbilityListener();
    if (statusChangeListener_ == nullptr) {
        TELEPHONY_LOGE("Init, failed to create statusChangeListener.");
        return;
    }
    auto managerPtr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (managerPtr == nullptr) {
        TELEPHONY_LOGE("Init, get system ability manager error.");
        return;
    }
    int32_t ret = managerPtr->SubscribeSystemAbility(TELEPHONY_SATELLITE_SERVICE_ABILITY_ID, statusChangeListener_);
    if (ret) {
        TELEPHONY_LOGE("Init, failed to subscribe sa:%{public}d", TELEPHONY_SATELLITE_SERVICE_ABILITY_ID);
    }
}

SatelliteSmsClient::~SatelliteSmsClient()
{
    RemoveDeathRecipient(nullptr, false);
}

sptr<ISatelliteService> SatelliteSmsClient::GetServiceProxy()
{
    std::lock_guard<std::mutex> lock(mutexProxy_);
    if (satelliteServiceProxy_ != nullptr) {
        return satelliteServiceProxy_;
    }

    sptr<ISystemAbilityManager> sam = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (sam == nullptr) {
        TELEPHONY_LOGE("Failed to get system ability manager");
        return nullptr;
    }
    sptr<IRemoteObject> obj = sam->CheckSystemAbility(TELEPHONY_SATELLITE_SERVICE_ABILITY_ID);
    if (obj == nullptr) {
        TELEPHONY_LOGE("Failed to get satellite service");
        return nullptr;
    }
    std::unique_ptr<SatelliteServiceDeathRecipient> recipient = std::make_unique<SatelliteServiceDeathRecipient>(*this);
    if (recipient == nullptr) {
        TELEPHONY_LOGE("recipient is null");
        return nullptr;
    }
    sptr<IRemoteObject::DeathRecipient> dr(recipient.release());
    if ((obj->IsProxyObject()) && (!obj->AddDeathRecipient(dr))) {
        TELEPHONY_LOGE("Failed to add death recipient");
        return nullptr;
    }
    satelliteServiceProxy_ = iface_cast<ISatelliteService>(obj);

    deathRecipient_ = dr;
    TELEPHONY_LOGD("Succeed to connect satellite service %{public}d", satelliteServiceProxy_ == nullptr);
    return satelliteServiceProxy_;
}

sptr<ISatelliteSmsService> SatelliteSmsClient::GetProxy()
{
    auto satelliteServiceProxy = GetServiceProxy();

    std::lock_guard<std::mutex> lock(mutexProxy_);
    if (proxy_ != nullptr) {
        return proxy_;
    }

    if (satelliteServiceProxy == nullptr) {
        TELEPHONY_LOGE("GetProxy: satellite service is null or destroyed");
        return nullptr;
    }

    sptr<IRemoteObject> smsObj = satelliteServiceProxy->GetProxyObjectPtr(PROXY_SATELLITE_SMS);
    if (smsObj == nullptr) {
        TELEPHONY_LOGE("satellite sms service is null");
        return nullptr;
    }
    proxy_ = iface_cast<ISatelliteSmsService>(smsObj);

    TELEPHONY_LOGD("Succeed to get satellite sms service %{public}d", proxy_ == nullptr);
    return proxy_;
}

void SatelliteSmsClient::OnRemoteDied(const wptr<IRemoteObject> &remote)
{
    RemoveDeathRecipient(remote, true);
}

void SatelliteSmsClient::RemoveDeathRecipient(const wptr<IRemoteObject> &remote, bool isRemoteDied)
{
    if (isRemoteDied && remote == nullptr) {
        TELEPHONY_LOGE("Remote died, remote is nullptr");
        return;
    }
    std::lock_guard<std::mutex> lock(mutexProxy_);
    if (satelliteServiceProxy_ == nullptr) {
        TELEPHONY_LOGE("satelliteServiceProxy_ is nullptr");
        return;
    }
    auto serviceRemote = satelliteServiceProxy_->AsObject();
    if (serviceRemote == nullptr) {
        TELEPHONY_LOGE("serviceRemote is nullptr");
        return;
    }
    if (isRemoteDied && serviceRemote != remote.promote()) {
        TELEPHONY_LOGE("Remote died serviceRemote is not same");
        return;
    }
    serviceRemote->RemoveDeathRecipient(deathRecipient_);
    satelliteServiceProxy_ = nullptr;
    proxy_ = nullptr;
    TELEPHONY_LOGI("SatelliteSmsClient:RemoveDeathRecipient success");
}

void SatelliteSmsClient::SystemAbilityListener::OnAddSystemAbility(int32_t systemAbilityId, const std::string &deviceId)
{
    TELEPHONY_LOGI("SA:%{public}d is added!", systemAbilityId);
    if (!CheckInputSysAbilityId(systemAbilityId)) {
        TELEPHONY_LOGE("add SA:%{public}d is invalid!", systemAbilityId);
        return;
    }

    auto &satelliteSmsClient = SatelliteSmsClient::GetInstance();
    satelliteSmsClient.ServiceOn();
    TELEPHONY_LOGI("SA:%{public}d reconnect service successfully!", systemAbilityId);
}

void SatelliteSmsClient::SystemAbilityListener::OnRemoveSystemAbility(
    int32_t systemAbilityId, const std::string &deviceId)
{
    TELEPHONY_LOGI("SA:%{public}d is removed!", systemAbilityId);
    auto &satelliteSmsClient = SatelliteSmsClient::GetInstance();
    satelliteSmsClient.ServiceOff();
}

int32_t SatelliteSmsClient::AddSendHandler(int32_t slotId, const std::shared_ptr<TelEventHandler> sender)
{
    if (sender == nullptr) {
        TELEPHONY_LOGE("AddSendHandler return, sender is null.");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }

    senderMap_.insert(std::make_pair(slotId, sender));
    TELEPHONY_LOGI("AddSendHandler success: %{public}d", slotId);
    return TELEPHONY_SUCCESS;
}

int32_t SatelliteSmsClient::AddReceiveHandler(int32_t slotId, const std::shared_ptr<TelEventHandler> receiver)
{
    if (receiver == nullptr) {
        TELEPHONY_LOGE("AddReceiveHandler return, receiver is null.");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }

    receiverMap_.insert(std::make_pair(slotId, receiver));
    TELEPHONY_LOGI("AddReceiveHandler success: %{public}d", slotId);
    return TELEPHONY_SUCCESS;
}

void SatelliteSmsClient::ServiceOn()
{
    for (auto pair : senderMap_) {
        auto handler = static_cast<GsmSmsSender *>(pair.second.get());
        if (handler == nullptr) {
            TELEPHONY_LOGE("SenderHandler is null: %{public}d", pair.first);
            continue;
        }
        handler->RegisterSatelliteCallback();
    }
    for (auto pair : receiverMap_) {
        auto handler = static_cast<GsmSmsReceiveHandler *>(pair.second.get());
        if (handler == nullptr) {
            TELEPHONY_LOGE("ReceiveHandler is null: %{public}d", pair.first);
            continue;
        }
        handler->RegisterSatelliteCallback();
    }
}

void SatelliteSmsClient::ServiceOff()
{
    std::lock_guard<std::mutex> lock(mutexProxy_);
    satelliteServiceProxy_ = nullptr;
    proxy_ = nullptr;

    for (auto pair : senderMap_) {
        auto handler = static_cast<GsmSmsSender *>(pair.second.get());
        if (handler == nullptr) {
            TELEPHONY_LOGE("SenderHandler is null: %{public}d", pair.first);
            continue;
        }
        handler->UnregisterSatelliteCallback();
    }
    for (auto pair : receiverMap_) {
        auto handler = static_cast<GsmSmsReceiveHandler *>(pair.second.get());
        if (handler == nullptr) {
            TELEPHONY_LOGE("ReceiveHandler is null: %{public}d", pair.first);
            continue;
        }
        handler->UnregisterSatelliteCallback();
    }
}

bool SatelliteSmsClient::GetSatelliteSupported()
{
    char satelliteSupported[SYSPARA_SIZE] = { 0 };
    GetParameter(TEL_SATELLITE_SUPPORTED, SATELLITE_DEFAULT_VALUE, satelliteSupported, SYSPARA_SIZE);
    TELEPHONY_LOGI("SatelliteSms satelliteSupported is %{public}s", satelliteSupported);
    return std::atoi(satelliteSupported);
}

bool SatelliteSmsClient::IsSatelliteEnabled()
{
    auto proxy = GetServiceProxy();
    if (proxy == nullptr) {
        TELEPHONY_LOGE("service proxy is null!");
        return false;
    }
    return proxy->IsSatelliteEnabled();
}

int32_t SatelliteSmsClient::GetSatelliteCapability()
{
    auto proxy = GetServiceProxy();
    if (proxy == nullptr) {
        TELEPHONY_LOGE("service proxy is null!");
        return static_cast<int32_t>(SatelliteCapability::NONE);
    }
    return proxy->GetSatelliteCapability();
}

int32_t SatelliteSmsClient::RegisterSmsNotify(int32_t slotId, int32_t what, const sptr<ISatelliteSmsCallback> &callback)
{
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        TELEPHONY_LOGE("proxy is null!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return proxy->RegisterSmsNotify(slotId, what, callback);
}

int32_t SatelliteSmsClient::UnRegisterSmsNotify(int32_t slotId, int32_t what)
{
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        TELEPHONY_LOGE("proxy is null!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return proxy->UnRegisterSmsNotify(slotId, what);
}

int32_t SatelliteSmsClient::SendSms(int32_t slotId, int32_t eventId, SatelliteMessage &message)
{
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        TELEPHONY_LOGE("proxy is null!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    TELEPHONY_LOGI("SatelliteSms SendSms");
    return proxy->SendSms(slotId, eventId, message);
}

int32_t SatelliteSmsClient::SendSmsMoreMode(int32_t slotId, int32_t eventId, SatelliteMessage &message)
{
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        TELEPHONY_LOGE("proxy is null!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    TELEPHONY_LOGI("SatelliteSms SendSmsMore");
    return proxy->SendSmsMoreMode(slotId, eventId, message);
}

int32_t SatelliteSmsClient::SendSmsAck(int32_t slotId, int32_t eventId, bool success, int32_t cause)
{
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        TELEPHONY_LOGE("proxy is null!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return proxy->SendSmsAck(slotId, eventId, success, cause);
}
} // namespace Telephony
} // namespace OHOS
