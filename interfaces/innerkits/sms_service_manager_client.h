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

#ifndef SMS_SERVICE_MANAGER_CLIENT_H
#define SMS_SERVICE_MANAGER_CLIENT_H

#include <string>
#include <vector>
#include <mutex>

#include "singleton.h"

#include "i_sms_service_interface.h"
#include "refbase.h"

namespace OHOS {
namespace Telephony {
class SmsServiceManagerClient : public std::enable_shared_from_this<SmsServiceManagerClient>  {
    DECLARE_DELAYED_SINGLETON(SmsServiceManagerClient)
public:
    bool InitSmsServiceProxy();
    void ResetSmsServiceProxy();

    bool SetDefaultSmsSlotId(int32_t slotId);
    int32_t GetDefaultSmsSlotId();
    int32_t SendMessage(int32_t slotId, const std::u16string desAddr, const std::u16string scAddr,
        const std::u16string text, const sptr<ISendShortMessageCallback> &callback,
        const sptr<IDeliveryShortMessageCallback> &deliveryCallback);
    int32_t SendMessage(int32_t slotId, const std::u16string desAddr, const std::u16string scAddr,
        uint16_t port, const uint8_t *data, uint16_t dataLen, const sptr<ISendShortMessageCallback> &callback,
        const sptr<IDeliveryShortMessageCallback> &deliveryCallback);
    bool SetScAddress(int32_t slotId, const std::u16string &scAddr);
    std::u16string GetScAddress(int32_t slotId);
    bool AddSimMessage(int32_t slotId, const std::u16string &smsc, const std::u16string &pdu,
        ISmsServiceInterface::SimMessageStatus status);
    bool DelSimMessage(int32_t slotId, uint32_t msgIndex);
    bool UpdateSimMessage(int32_t slotId, uint32_t msgIndex,
        ISmsServiceInterface::SimMessageStatus newStatus, const std::u16string &pdu, const std::u16string &smsc);
    std::vector<ShortMessage> GetAllSimMessages(int32_t slotId);
    bool SetCBConfig(
        int32_t slotId, bool enable, uint32_t startMessageId, uint32_t endMessageId, uint8_t ranType);
    bool SetImsSmsConfig(int32_t slotId, int32_t enable);
    std::vector<std::u16string> SplitMessage(const std::u16string &message);
    bool GetSmsSegmentsInfo(int32_t slotId, const std::u16string &message, bool force7BitCode,
        ISmsServiceInterface::SmsSegmentsInfo &segInfo);
    bool IsImsSmsSupported(int32_t slotId);
    std::u16string GetImsShortMessageFormat();
    bool HasSmsCapability();
private:
    std::mutex mutex_;
    sptr<ISmsServiceInterface> smsServiceInterface_;
    sptr<IRemoteObject::DeathRecipient> recipient_;
};

enum class SmsSendResult {
    /**
     *  Indicates that the SMS message is successfully sent.
     */
    SEND_SMS_SUCCESS = 0,
    /**
     * Indicates that sending the SMS message fails due to an unknown reason.
     */
    SEND_SMS_FAILURE_UNKNOWN = 1,
    /**
     * Indicates that sending the SMS fails because the modem is powered off.
     */
    SEND_SMS_FAILURE_RADIO_OFF = 2,
    /**
     * Indicates that sending the SMS message fails because the network is unavailable
     * or does not support sending or reception of SMS messages.
     */
    SEND_SMS_FAILURE_SERVICE_UNAVAILABLE = 3
};
} // namespace Telephony
} // namespace OHOS
#endif // SMS_SERVICE_MANAGER_CLIENT_H