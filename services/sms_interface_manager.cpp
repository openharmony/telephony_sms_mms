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

void SmsInterfaceManager::TextBasedSmsDelivery(const string &desAddr, const string &scAddr, const string &text,
    const sptr<ISendShortMessageCallback> &sendCallback,
    const sptr<IDeliveryShortMessageCallback> &deliveryCallback)
{
    if (desAddr.empty() || text.empty() || (smsSendManager_ == nullptr)) {
        SmsSender::SendResultCallBack(sendCallback, ISendShortMessageCallback::SEND_SMS_FAILURE_UNKNOWN);
        SmsHiSysEvent::WriteSmsSendFaultEvent(slotId_, SmsMmsMessageType::SMS_SHORT_MESSAGE,
            SmsMmsErrorCode::SMS_ERROR_EMPTY_INPUT_PARAMETER, "text sms arges is empty is nullptr");
        TELEPHONY_LOGE("TextBasedSmsDelivery failed to send.");
        return;
    }
    smsSendManager_->TextBasedSmsDelivery(desAddr, scAddr, text, sendCallback, deliveryCallback);
}

void SmsInterfaceManager::DataBasedSmsDelivery(const string &desAddr, const string &scAddr, const uint16_t port,
    const uint8_t *data, uint16_t dataLen, const sptr<ISendShortMessageCallback> &sendCallback,
    const sptr<IDeliveryShortMessageCallback> &deliveryCallback)
{
    if (desAddr.empty() || (smsSendManager_ == nullptr) || (data == nullptr)) {
        SmsSender::SendResultCallBack(sendCallback, ISendShortMessageCallback::SEND_SMS_FAILURE_UNKNOWN);
        SmsHiSysEvent::WriteSmsSendFaultEvent(slotId_, SmsMmsMessageType::SMS_SHORT_MESSAGE,
            SmsMmsErrorCode::SMS_ERROR_EMPTY_INPUT_PARAMETER, "data sms arges is empty is nullptr");
        TELEPHONY_LOGE("DataBasedSmsDelivery failed to send.");
        return;
    }
    smsSendManager_->DataBasedSmsDelivery(desAddr, scAddr, port, data, dataLen, sendCallback, deliveryCallback);
}

bool SmsInterfaceManager::AddSimMessage(
    const std::string &smsc, const std::string &pdu, ISmsServiceInterface::SimMessageStatus status)
{
    if (smsMiscManager_ == nullptr) {
        TELEPHONY_LOGE("smsMiscManager nullptr error.");
        return false;
    }
    return smsMiscManager_->AddSimMessage(smsc, pdu, status);
}

bool SmsInterfaceManager::DelSimMessage(uint32_t msgIndex)
{
    if (smsMiscManager_ == nullptr) {
        TELEPHONY_LOGE("smsMiscManager nullptr error.");
        return false;
    }
    return smsMiscManager_->DelSimMessage(msgIndex);
}

bool SmsInterfaceManager::UpdateSimMessage(uint32_t msgIndex, ISmsServiceInterface::SimMessageStatus newStatus,
    const std::string &pdu, const std::string &smsc)
{
    if (smsMiscManager_ == nullptr) {
        TELEPHONY_LOGE("smsMiscManager nullptr error.");
        return false;
    }
    return smsMiscManager_->UpdateSimMessage(msgIndex, newStatus, pdu, smsc);
}

vector<ShortMessage> SmsInterfaceManager::GetAllSimMessages()
{
    std::vector<ShortMessage> ret;
    if (smsMiscManager_ == nullptr) {
        TELEPHONY_LOGE("smsMiscManager nullptr error.");
        return ret;
    }
    return smsMiscManager_->GetAllSimMessages();
}

bool SmsInterfaceManager::SetSmscAddr(const std::string &scAddr)
{
    if (smsMiscManager_ == nullptr) {
        TELEPHONY_LOGE("smsMiscManager nullptr error.");
        return false;
    }
    return smsMiscManager_->SetSmscAddr(scAddr);
}

std::string SmsInterfaceManager::GetSmscAddr()
{
    if (smsMiscManager_ == nullptr) {
        TELEPHONY_LOGE("smsMiscManager nullptr error.");
        return "";
    }
    return smsMiscManager_->GetSmscAddr();
}

bool SmsInterfaceManager::SetCBConfig(bool enable, uint32_t fromMsgId, uint32_t toMsgId, uint8_t netType)
{
    if (smsMiscManager_ == nullptr) {
        TELEPHONY_LOGE("smsMiscManager nullptr error.");
        return false;
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

bool SmsInterfaceManager::SetDefaultSmsSlotId(int32_t slotId)
{
    if (smsMiscManager_ == nullptr) {
        TELEPHONY_LOGE("smsMiscManager nullptr error.");
        return false;
    }
    return smsMiscManager_->SetDefaultSmsSlotId(slotId);
}

int32_t SmsInterfaceManager::GetDefaultSmsSlotId()
{
    int32_t result = -1;
    if (smsMiscManager_ == nullptr) {
        TELEPHONY_LOGE("smsMiscManager nullptr error.");
        return result;
    }
    return smsMiscManager_->GetDefaultSmsSlotId();
}

std::vector<std::string> SmsInterfaceManager::SplitMessage(const std::string &message)
{
    std::vector<std::string> result;
    if (smsSendManager_ == nullptr) {
        TELEPHONY_LOGE("smsSendManager nullptr error.");
        return result;
    }
    return smsSendManager_->SplitMessage(message);
}

bool SmsInterfaceManager::GetSmsSegmentsInfo(const std::string &message, bool force7BitCode, LengthInfo &outInfo)
{
    if (smsSendManager_ == nullptr) {
        TELEPHONY_LOGE("smsSendManager nullptr error.");
        return false;
    }
    return smsSendManager_->GetSmsSegmentsInfo(message, force7BitCode, outInfo);
}

bool SmsInterfaceManager::IsImsSmsSupported(int32_t slotId)
{
    bool result = false;
    if (smsSendManager_ == nullptr) {
        TELEPHONY_LOGE("smsSendManager is nullptr error.");
        return result;
    }
    return smsSendManager_->IsImsSmsSupported(slotId);
}

std::string SmsInterfaceManager::GetImsShortMessageFormat()
{
    std::string result;
    if (smsSendManager_ == nullptr) {
        TELEPHONY_LOGE("smsSendManager is nullptr error.");
        return result;
    }
    return smsSendManager_->GetImsShortMessageFormat();
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