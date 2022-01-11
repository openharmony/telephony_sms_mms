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

#include "short_message_manager.h"

#include "sms_service_proxy_holder.h"
#include "singleton.h"

namespace OHOS {
namespace Telephony {
bool ShortMessageManager::SetDefaultSmsSlotId(int32_t slotId)
{
    return DelayedSingleton<SmsServiceProxyHolder>::GetInstance()->SetDefaultSmsSlotId(slotId);
}

int32_t ShortMessageManager::GetDefaultSmsSlotId()
{
    return DelayedSingleton<SmsServiceProxyHolder>::GetInstance()->GetDefaultSmsSlotId();
}

int32_t ShortMessageManager::SendMessage(int32_t slotId, const std::u16string desAddr, const std::u16string scAddr,
    const std::u16string text, const sptr<ISendShortMessageCallback> &callback,
    const sptr<IDeliveryShortMessageCallback> &deliveryCallback)
{
    return DelayedSingleton<SmsServiceProxyHolder>::GetInstance()->SendMessage(
        slotId, desAddr, scAddr, text, callback, deliveryCallback);
}

int32_t ShortMessageManager::SendMessage(int32_t slotId, const std::u16string desAddr, const std::u16string scAddr,
    uint16_t port, const uint8_t *data, uint16_t dataLen, const sptr<ISendShortMessageCallback> &callback,
    const sptr<IDeliveryShortMessageCallback> &deliveryCallback)
{
    return DelayedSingleton<SmsServiceProxyHolder>::GetInstance()->SendMessage(
        slotId, desAddr, scAddr, port, data, dataLen, callback, deliveryCallback);
}

bool ShortMessageManager::SetScAddress(int32_t slotId, const std::u16string &scAddr)
{
    return DelayedSingleton<SmsServiceProxyHolder>::GetInstance()->SetScAddress(slotId, scAddr);
}

std::u16string ShortMessageManager::GetScAddress(int32_t slotId)
{
    return DelayedSingleton<SmsServiceProxyHolder>::GetInstance()->GetScAddress(slotId);
}

bool ShortMessageManager::AddSimMessage(int32_t slotId, const std::u16string &smsc, const std::u16string &pdu,
    ISmsServiceInterface::SimMessageStatus status)
{
    return DelayedSingleton<SmsServiceProxyHolder>::GetInstance()->AddSimMessage(slotId, smsc, pdu, status);
}

bool ShortMessageManager::DelSimMessage(int32_t slotId, uint32_t msgIndex)
{
    return DelayedSingleton<SmsServiceProxyHolder>::GetInstance()->DelSimMessage(slotId, msgIndex);
}

bool ShortMessageManager::UpdateSimMessage(int32_t slotId, uint32_t msgIndex,
    ISmsServiceInterface::SimMessageStatus newStatus, const std::u16string &pdu, const std::u16string &smsc)
{
    return DelayedSingleton<SmsServiceProxyHolder>::GetInstance()->UpdateSimMessage(
        slotId, msgIndex, newStatus, pdu, smsc);
}

std::vector<ShortMessage> ShortMessageManager::GetAllSimMessages(int32_t slotId)
{
    return DelayedSingleton<SmsServiceProxyHolder>::GetInstance()->GetAllSimMessages(slotId);
}

bool ShortMessageManager::SetCBConfig(
    int32_t slotId, bool enable, uint32_t startMessageId, uint32_t endMessageId, uint8_t ranType)
{
    return DelayedSingleton<SmsServiceProxyHolder>::GetInstance()->SetCBConfig(
        slotId, enable, startMessageId, endMessageId, ranType);
}

std::vector<std::u16string> ShortMessageManager::SplitMessage(const std::u16string &message)
{
    return DelayedSingleton<SmsServiceProxyHolder>::GetInstance()->SplitMessage(message);
}

bool ShortMessageManager::GetSmsSegmentsInfo(int32_t slotId, const std::u16string &message, bool force7BitCode,
    ISmsServiceInterface::SmsSegmentsInfo &segInfo)
{
    auto proxyHolder = DelayedSingleton<SmsServiceProxyHolder>::GetInstance();
    if (proxyHolder == nullptr) {
        return false;
    }
    return proxyHolder->GetSmsSegmentsInfo(slotId, message, force7BitCode, segInfo);
}

bool ShortMessageManager::IsImsSmsSupported()
{
    return DelayedSingleton<SmsServiceProxyHolder>::GetInstance()->IsImsSmsSupported();
}

std::u16string ShortMessageManager::GetImsShortMessageFormat()
{
    return DelayedSingleton<SmsServiceProxyHolder>::GetInstance()->GetImsShortMessageFormat();
}

bool ShortMessageManager::HasSmsCapability()
{
    return DelayedSingleton<SmsServiceProxyHolder>::GetInstance()->HasSmsCapability();
}
} // namespace Telephony
} // namespace OHOS