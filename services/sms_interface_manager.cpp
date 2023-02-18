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

#include "sms_interface_manager.h"

#include "sms_hisysevent.h"
#include "sms_misc_manager.h"
#include "string_utils.h"
#include "telephony_errors.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
using namespace std;
SmsInterfaceManager::SmsInterfaceManager(int32_t slotId) : slotId_(slotId) {}

SmsInterfaceManager::~SmsInterfaceManager() {}

void SmsInterfaceManager::InitInterfaceManager()
{
    smsSendManager_ = make_unique<SmsSendManager>(slotId_);
    if (smsSendManager_ == nullptr) {
        TELEPHONY_LOGE("failed to create SmsSendManager");
        return;
    }
    smsSendManager_->Init();

    smsReceiveManager_ = make_unique<SmsReceiveManager>(slotId_);
    if (smsReceiveManager_ == nullptr) {
        TELEPHONY_LOGE("failed to create SmsReceiveManager");
        return;
    }
    smsReceiveManager_->Init();
    auto smsMiscRunner = AppExecFwk::EventRunner::Create("SmsMiscRunner" + to_string(slotId_));
    if (smsMiscRunner == nullptr) {
        TELEPHONY_LOGE("failed to create SmsCbRunner");
        return;
    }
    smsReceiveManager_->SetCdmaSender(smsSendManager_->GetCdmaSmsSender());
    smsMiscManager_ = make_shared<SmsMiscManager>(smsMiscRunner, slotId_);
    smsMiscRunner->Run();
    TELEPHONY_LOGI("SmsInterfaceManager::InitInterfaceManager success, %{public}d", slotId_);
}

int32_t SmsInterfaceManager::TextBasedSmsDelivery(const string &desAddr, const string &scAddr, const string &text,
    const sptr<ISendShortMessageCallback> &sendCallback, const sptr<IDeliveryShortMessageCallback> &deliveryCallback)
{
    if (desAddr.empty() || text.empty()) {
        SmsSender::SendResultCallBack(sendCallback, ISendShortMessageCallback::SEND_SMS_FAILURE_UNKNOWN);
        SmsHiSysEvent::WriteSmsSendFaultEvent(slotId_, SmsMmsMessageType::SMS_SHORT_MESSAGE,
            SmsMmsErrorCode::SMS_ERROR_EMPTY_INPUT_PARAMETER, "text sms arges is empty");
        TELEPHONY_LOGE("TextBasedSmsDelivery failed to send.");
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }
    if (smsSendManager_ == nullptr) {
        TELEPHONY_LOGE("TextBasedSmsDelivery failed to send.");
        SmsSender::SendResultCallBack(sendCallback, ISendShortMessageCallback::SEND_SMS_FAILURE_UNKNOWN);
        SmsHiSysEvent::WriteSmsSendFaultEvent(slotId_, SmsMmsMessageType::SMS_SHORT_MESSAGE,
            SmsMmsErrorCode::SMS_ERROR_NULL_POINTER, "text sms smsSendManager_ is nullptr");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    smsSendManager_->TextBasedSmsDelivery(desAddr, scAddr, text, sendCallback, deliveryCallback);
    return TELEPHONY_ERR_SUCCESS;
}

int32_t SmsInterfaceManager::DataBasedSmsDelivery(const string &desAddr, const string &scAddr, const uint16_t port,
    const uint8_t *data, uint16_t dataLen, const sptr<ISendShortMessageCallback> &sendCallback,
    const sptr<IDeliveryShortMessageCallback> &deliveryCallback)
{
    if (desAddr.empty() || (data == nullptr)) {
        SmsSender::SendResultCallBack(sendCallback, ISendShortMessageCallback::SEND_SMS_FAILURE_UNKNOWN);
        SmsHiSysEvent::WriteSmsSendFaultEvent(slotId_, SmsMmsMessageType::SMS_SHORT_MESSAGE,
            SmsMmsErrorCode::SMS_ERROR_EMPTY_INPUT_PARAMETER, "data sms arges is empty");
        TELEPHONY_LOGE("DataBasedSmsDelivery failed to send.");
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }
    if (smsSendManager_ == nullptr) {
        TELEPHONY_LOGE("TextBasedSmsDelivery failed to send.");
        SmsSender::SendResultCallBack(sendCallback, ISendShortMessageCallback::SEND_SMS_FAILURE_UNKNOWN);
        SmsHiSysEvent::WriteSmsSendFaultEvent(slotId_, SmsMmsMessageType::SMS_SHORT_MESSAGE,
            SmsMmsErrorCode::SMS_ERROR_NULL_POINTER, "text sms smsSendManager_ is nullptr");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    smsSendManager_->DataBasedSmsDelivery(desAddr, scAddr, port, data, dataLen, sendCallback, deliveryCallback);
    return TELEPHONY_ERR_SUCCESS;
}

int32_t SmsInterfaceManager::AddSimMessage(
    const std::string &smsc, const std::string &pdu, ISmsServiceInterface::SimMessageStatus status)
{
    if (smsMiscManager_ == nullptr) {
        TELEPHONY_LOGE("smsMiscManager nullptr error.");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return smsMiscManager_->AddSimMessage(smsc, pdu, status);
}

int32_t SmsInterfaceManager::DelSimMessage(uint32_t msgIndex)
{
    if (smsMiscManager_ == nullptr) {
        TELEPHONY_LOGE("smsMiscManager nullptr error.");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return smsMiscManager_->DelSimMessage(msgIndex);
}

int32_t SmsInterfaceManager::UpdateSimMessage(uint32_t msgIndex, ISmsServiceInterface::SimMessageStatus newStatus,
    const std::string &pdu, const std::string &smsc)
{
    if (smsMiscManager_ == nullptr) {
        TELEPHONY_LOGE("smsMiscManager nullptr error.");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return smsMiscManager_->UpdateSimMessage(msgIndex, newStatus, pdu, smsc);
}

int32_t SmsInterfaceManager::GetAllSimMessages(std::vector<ShortMessage> &message)
{
    if (smsMiscManager_ == nullptr) {
        TELEPHONY_LOGE("smsMiscManager nullptr error.");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return smsMiscManager_->GetAllSimMessages(message);
}

int32_t SmsInterfaceManager::SetSmscAddr(const std::string &scAddr)
{
    if (smsMiscManager_ == nullptr) {
        TELEPHONY_LOGE("smsMiscManager nullptr error.");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return smsMiscManager_->SetSmscAddr(scAddr);
}

int32_t SmsInterfaceManager::GetSmscAddr(std::u16string &smscAddress)
{
    if (smsMiscManager_ == nullptr) {
        TELEPHONY_LOGE("smsMiscManager nullptr error.");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return smsMiscManager_->GetSmscAddr(smscAddress);
}

int32_t SmsInterfaceManager::SetCBConfig(bool enable, uint32_t fromMsgId, uint32_t toMsgId, uint8_t netType)
{
    if (smsMiscManager_ == nullptr) {
        TELEPHONY_LOGE("smsMiscManager nullptr error.");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return smsMiscManager_->SetCBConfig(enable, fromMsgId, toMsgId, netType);
}

bool SmsInterfaceManager::SetImsSmsConfig(int32_t slotId, int32_t enable)
{
    if (smsMiscManager_ == nullptr) {
        TELEPHONY_LOGE("smsMiscManager nullptr error.");
        return false;
    }
    return smsSendManager_->SetImsSmsConfig(slotId, enable);
}

int32_t SmsInterfaceManager::SetDefaultSmsSlotId(int32_t slotId)
{
    if (smsMiscManager_ == nullptr) {
        TELEPHONY_LOGE("smsMiscManager nullptr error.");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return smsMiscManager_->SetDefaultSmsSlotId(slotId);
}

int32_t SmsInterfaceManager::GetDefaultSmsSlotId()
{
    if (smsMiscManager_ == nullptr) {
        TELEPHONY_LOGE("smsMiscManager nullptr error.");
        return TELEPHONY_ERROR;
    }
    return smsMiscManager_->GetDefaultSmsSlotId();
}

int32_t SmsInterfaceManager::SplitMessage(const std::string &message, std::vector<std::u16string> &splitMessage)
{
    if (smsSendManager_ == nullptr) {
        TELEPHONY_LOGE("smsSendManager nullptr error.");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return smsSendManager_->SplitMessage(message, splitMessage);
}

int32_t SmsInterfaceManager::GetSmsSegmentsInfo(const std::string &message, bool force7BitCode, LengthInfo &outInfo)
{
    if (smsSendManager_ == nullptr) {
        TELEPHONY_LOGE("smsSendManager nullptr error.");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return smsSendManager_->GetSmsSegmentsInfo(message, force7BitCode, outInfo);
}

int32_t SmsInterfaceManager::IsImsSmsSupported(int32_t slotId, bool &isSupported)
{
    if (smsSendManager_ == nullptr) {
        TELEPHONY_LOGE("smsSendManager is nullptr error.");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return smsSendManager_->IsImsSmsSupported(slotId, isSupported);
}

int32_t SmsInterfaceManager::GetImsShortMessageFormat(std::u16string &format)
{
    if (smsSendManager_ == nullptr) {
        TELEPHONY_LOGE("smsSendManager is nullptr error.");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return smsSendManager_->GetImsShortMessageFormat(format);
}

bool SmsInterfaceManager::HasSmsCapability()
{
    auto helperPtr = DelayedSingleton<SmsPersistHelper>::GetInstance();
    if (helperPtr == nullptr) {
        TELEPHONY_LOGE("Get SmsPersistHelper Singleton nullptr error.");
        return true;
    }
    return helperPtr->QueryParamBoolean(SmsPersistHelper::SMS_CAPABLE_PARAM_KEY, true);
}
} // namespace Telephony
} // namespace OHOS