/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#include "sms_service_manager_client.h"

#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "sms_service_interface_death_recipient.h"
#include "system_ability_definition.h"
#include "telephony_errors.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
SmsServiceManagerClient::SmsServiceManagerClient() {}

SmsServiceManagerClient::~SmsServiceManagerClient() {}

bool SmsServiceManagerClient::InitSmsServiceProxy()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (smsServiceInterface_ == nullptr) {
        sptr<ISystemAbilityManager> systemAbilityManager =
            SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        if (!systemAbilityManager) {
            TELEPHONY_LOGE(" Get system ability mgr failed.");
            return false;
        }
        sptr<IRemoteObject> remoteObject = systemAbilityManager->GetSystemAbility(TELEPHONY_SMS_MMS_SYS_ABILITY_ID);
        if (!remoteObject) {
            TELEPHONY_LOGE("Get SMS Service Failed.");
            return false;
        }
        smsServiceInterface_ = iface_cast<ISmsServiceInterface>(remoteObject);
        if ((!smsServiceInterface_) || (!smsServiceInterface_->AsObject())) {
            TELEPHONY_LOGE("Get SMS Service Proxy Failed.");
            return false;
        }
        recipient_ = new SmsServiceInterfaceDeathRecipient(*this);
        if (!recipient_) {
            TELEPHONY_LOGE("Failed to create death Recipient ptr SmsServiceInterfaceDeathRecipient!");
            return false;
        }
        smsServiceInterface_->AsObject()->AddDeathRecipient(recipient_);
    }
    return true;
}

void SmsServiceManagerClient::ResetSmsServiceProxy()
{
    std::lock_guard<std::mutex> lock(mutex_);
    std::lock_guard<std::mutex> mmsLock(mmsMutex_);
    if ((smsServiceInterface_ != nullptr) && (smsServiceInterface_->AsObject() != nullptr)) {
        smsServiceInterface_->AsObject()->RemoveDeathRecipient(recipient_);
    }
    smsServiceInterface_ = nullptr;
}

int32_t SmsServiceManagerClient::SetDefaultSmsSlotId(int32_t slotId)
{
    if (InitSmsServiceProxy()) {
        std::lock_guard<std::mutex> lock(mutex_);
        return smsServiceInterface_->SetDefaultSmsSlotId(slotId);
    }
    return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
}

int32_t SmsServiceManagerClient::GetDefaultSmsSimId(int32_t &simId)
{
    if (InitSmsServiceProxy()) {
        std::lock_guard<std::mutex> lock(mutex_);
        return smsServiceInterface_->GetDefaultSmsSimId(simId);
    }
    return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
}

int32_t SmsServiceManagerClient::GetDefaultSmsSlotId()
{
    if (InitSmsServiceProxy()) {
        std::lock_guard<std::mutex> lock(mutex_);
        return smsServiceInterface_->GetDefaultSmsSlotId();
    }
    return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
}

int32_t SmsServiceManagerClient::SendMessage(int32_t slotId, const std::u16string desAddr, const std::u16string scAddr,
    const std::u16string text, const sptr<ISendShortMessageCallback> &callback,
    const sptr<IDeliveryShortMessageCallback> &deliveryCallback)
{
    if (InitSmsServiceProxy()) {
        std::lock_guard<std::mutex> lock(mutex_);
        return smsServiceInterface_->SendMessage(slotId, desAddr, scAddr, text, callback, deliveryCallback);
    }
    return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
}

int32_t SmsServiceManagerClient::SendMessage(int32_t slotId, const std::u16string desAddr, const std::u16string scAddr,
    uint16_t port, const uint8_t *data, uint16_t dataLen, const sptr<ISendShortMessageCallback> &callback,
    const sptr<IDeliveryShortMessageCallback> &deliveryCallback)
{
    if (InitSmsServiceProxy()) {
        std::lock_guard<std::mutex> lock(mutex_);
        return smsServiceInterface_->SendMessage(
            slotId, desAddr, scAddr, port, data, dataLen, callback, deliveryCallback);
    }
    return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
}

int32_t SmsServiceManagerClient::SendMessageWithoutSave(int32_t slotId, const std::u16string desAddr,
    const std::u16string scAddr, const std::u16string text, const sptr<ISendShortMessageCallback> &callback,
    const sptr<IDeliveryShortMessageCallback> &deliveryCallback)
{
    if (InitSmsServiceProxy()) {
        std::lock_guard<std::mutex> lock(mutex_);
        return smsServiceInterface_->SendMessageWithoutSave(slotId, desAddr, scAddr, text, callback, deliveryCallback);
    }
    return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
}

int32_t SmsServiceManagerClient::SetScAddress(int32_t slotId, const std::u16string &scAddr)
{
    if (InitSmsServiceProxy()) {
        std::lock_guard<std::mutex> lock(mutex_);
        return smsServiceInterface_->SetSmscAddr(slotId, scAddr);
    }
    return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
}

int32_t SmsServiceManagerClient::GetScAddress(int32_t slotId, std::u16string &smscAddress)
{
    if (InitSmsServiceProxy()) {
        std::lock_guard<std::mutex> lock(mutex_);
        return smsServiceInterface_->GetSmscAddr(slotId, smscAddress);
    }
    return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
}

int32_t SmsServiceManagerClient::AddSimMessage(int32_t slotId, const std::u16string &smsc, const std::u16string &pdu,
    ISmsServiceInterface::SimMessageStatus status)
{
    if (InitSmsServiceProxy()) {
        std::lock_guard<std::mutex> lock(mutex_);
        return smsServiceInterface_->AddSimMessage(slotId, smsc, pdu, status);
    }
    return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
}

int32_t SmsServiceManagerClient::DelSimMessage(int32_t slotId, uint32_t msgIndex)
{
    if (InitSmsServiceProxy()) {
        std::lock_guard<std::mutex> lock(mutex_);
        return smsServiceInterface_->DelSimMessage(slotId, msgIndex);
    }
    return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
}

int32_t SmsServiceManagerClient::UpdateSimMessage(int32_t slotId, uint32_t msgIndex,
    ISmsServiceInterface::SimMessageStatus newStatus, const std::u16string &pdu, const std::u16string &smsc)
{
    if (InitSmsServiceProxy()) {
        std::lock_guard<std::mutex> lock(mutex_);
        return smsServiceInterface_->UpdateSimMessage(slotId, msgIndex, newStatus, pdu, smsc);
    }
    return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
}

int32_t SmsServiceManagerClient::GetAllSimMessages(int32_t slotId, std::vector<ShortMessage> &message)
{
    if (InitSmsServiceProxy()) {
        std::lock_guard<std::mutex> lock(mutex_);
        return smsServiceInterface_->GetAllSimMessages(slotId, message);
    }
    return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
}

int32_t SmsServiceManagerClient::SetCBConfig(
    int32_t slotId, bool enable, uint32_t startMessageId, uint32_t endMessageId, uint8_t ranType)
{
    if (InitSmsServiceProxy()) {
        std::lock_guard<std::mutex> lock(mutex_);
        return smsServiceInterface_->SetCBConfig(slotId, enable, startMessageId, endMessageId, ranType);
    }
    return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
}

int32_t SmsServiceManagerClient::SetCBConfigList(
    int32_t slotId, const std::vector<int32_t>& messageIds, int32_t ranType)
{
    if (InitSmsServiceProxy()) {
        std::lock_guard<std::mutex> lock(mutex_);
        return smsServiceInterface_->SetCBConfigList(slotId, messageIds, ranType);
    }
    return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
}

bool SmsServiceManagerClient::SetImsSmsConfig(int32_t slotId, int32_t enable)
{
    if (InitSmsServiceProxy()) {
        std::lock_guard<std::mutex> lock(mutex_);
        return smsServiceInterface_->SetImsSmsConfig(slotId, enable);
    }
    return false;
}

int32_t SmsServiceManagerClient::SendMms(int32_t slotId, const std::u16string &mmsc, const std::u16string &data,
    const std::u16string &ua, const std::u16string &uaprof, int64_t &time)
{
    if (InitSmsServiceProxy()) {
        std::lock_guard<std::mutex> lock(mmsMutex_);
        return smsServiceInterface_->SendMms(slotId, mmsc, data, ua, uaprof, time);
    }
    return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
}

int32_t SmsServiceManagerClient::DownloadMms(int32_t slotId, const std::u16string &mmsc, std::u16string &data,
    const std::u16string &ua, const std::u16string &uaprof)
{
    if (InitSmsServiceProxy()) {
        std::lock_guard<std::mutex> lock(mmsMutex_);
        return smsServiceInterface_->DownloadMms(slotId, mmsc, data, ua, uaprof);
    }
    return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
}

int32_t SmsServiceManagerClient::SplitMessage(const std::u16string &message, std::vector<std::u16string> &splitMessage)
{
    if (InitSmsServiceProxy()) {
        std::lock_guard<std::mutex> lock(mutex_);
        return smsServiceInterface_->SplitMessage(message, splitMessage);
    }
    return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
}

int32_t SmsServiceManagerClient::GetSmsSegmentsInfo(
    int32_t slotId, const std::u16string &message, bool force7BitCode, ISmsServiceInterface::SmsSegmentsInfo &segInfo)
{
    if (InitSmsServiceProxy()) {
        std::lock_guard<std::mutex> lock(mutex_);
        return smsServiceInterface_->GetSmsSegmentsInfo(slotId, message, force7BitCode, segInfo);
    }
    return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
}

int32_t SmsServiceManagerClient::IsImsSmsSupported(int32_t slotId, bool &isSupported)
{
    if (InitSmsServiceProxy()) {
        std::lock_guard<std::mutex> lock(mutex_);
        return smsServiceInterface_->IsImsSmsSupported(slotId, isSupported);
    }
    return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
}

int32_t SmsServiceManagerClient::GetImsShortMessageFormat(std::u16string &format)
{
    if (InitSmsServiceProxy()) {
        std::lock_guard<std::mutex> lock(mutex_);
        return smsServiceInterface_->GetImsShortMessageFormat(format);
    }
    return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
}

bool SmsServiceManagerClient::HasSmsCapability()
{
    if (InitSmsServiceProxy()) {
        std::lock_guard<std::mutex> lock(mutex_);
        return smsServiceInterface_->HasSmsCapability();
    }
    return false;
}

int32_t SmsServiceManagerClient::CreateMessage(std::string pdu, std::string specification, ShortMessage &message)
{
    if (InitSmsServiceProxy()) {
        std::lock_guard<std::mutex> lock(mutex_);
        return smsServiceInterface_->CreateMessage(pdu, specification, message);
    }
    return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
}

bool SmsServiceManagerClient::GetBase64Encode(std::string src, std::string &dest)
{
    if (InitSmsServiceProxy()) {
        std::lock_guard<std::mutex> lock(mutex_);
        return smsServiceInterface_->GetBase64Encode(src, dest);
    }
    return false;
}

bool SmsServiceManagerClient::GetBase64Decode(std::string src, std::string &dest)
{
    if (InitSmsServiceProxy()) {
        std::lock_guard<std::mutex> lock(mutex_);
        return smsServiceInterface_->GetBase64Decode(src, dest);
    }
    return false;
}

bool SmsServiceManagerClient::GetEncodeStringFunc(
    std::string &encodeString, uint32_t charset, uint32_t valLength, std::string strEncodeString)
{
    if (InitSmsServiceProxy()) {
        std::lock_guard<std::mutex> lock(mutex_);
        return smsServiceInterface_->GetEncodeStringFunc(encodeString, charset, valLength, strEncodeString);
    }
    return false;
}
} // namespace Telephony
} // namespace OHOS