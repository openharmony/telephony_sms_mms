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

#ifndef SMS_INTERFACE_MANAGER_H
#define SMS_INTERFACE_MANAGER_H

#include <list>
#include <memory>
#include <string>
#include <vector>

#include "i_sms_service_interface.h"
#include "mms_receive_manager.h"
#include "mms_send_manager.h"
#include "sms_misc_manager.h"
#include "sms_persist_helper.h"
#include "sms_receive_manager.h"
#include "sms_send_manager.h"

namespace OHOS {
namespace Telephony {
class SmsInterfaceManager {
public:
    explicit SmsInterfaceManager(int32_t slotId);
    virtual ~SmsInterfaceManager();
    void InitInterfaceManager();
    int32_t TextBasedSmsDelivery(const std::string &desAddr, const std::string &scAddr, const std::string &text,
        const sptr<ISendShortMessageCallback> &sendCallback,
        const sptr<IDeliveryShortMessageCallback> &deliveryCallback, int32_t id, bool isMmsApp = true);
    int32_t DataBasedSmsDelivery(const std::string &desAddr, const std::string &scAddr, const uint16_t port,
        const uint8_t *data, uint16_t dataLen, const sptr<ISendShortMessageCallback> &sendCallback,
        const sptr<IDeliveryShortMessageCallback> &deliveryCallback);
    int32_t AddSimMessage(
        const std::string &smsc, const std::string &pdu, ISmsServiceInterface::SimMessageStatus status);
    int32_t DelSimMessage(uint32_t msgIndex);
    int32_t UpdateSimMessage(uint32_t msgIndex, ISmsServiceInterface::SimMessageStatus newStatus,
        const std::string &pdu, const std::string &smsc);
    int32_t GetAllSimMessages(std::vector<ShortMessage> &message);
    int32_t SetSmscAddr(const std::string &scAddr);
    int32_t GetSmscAddr(std::u16string &smscAddress);
    int32_t SetCBConfig(bool enable, uint32_t fromMsgId, uint32_t toMsgId, uint8_t netType);
    int32_t SetCBConfigList(const std::vector<int32_t>& messageIds, int32_t ranType);
    bool SetImsSmsConfig(int32_t slotId, int32_t enable);
    int32_t SetDefaultSmsSlotId(int32_t slotId);
    int32_t GetDefaultSmsSlotId();
    int32_t GetDefaultSmsSimId(int32_t &simId);
    int32_t SplitMessage(const std::string &message, std::vector<std::u16string> &splitMessage);
    int32_t GetSmsSegmentsInfo(const std::string &message, bool force7BitCode, LengthInfo &outInfo);
    int32_t IsImsSmsSupported(int32_t slotId, bool &isSupported);
    int32_t GetImsShortMessageFormat(std::u16string &format);
    bool HasSmsCapability();
    int32_t SendMms(
        const std::u16string &mmsc, const std::u16string &data, const std::u16string &ua, const std::u16string &uaprof);
    int32_t DownloadMms(
        const std::u16string &mmsc, std::u16string &data, const std::u16string &ua, const std::u16string &uaprof);
    int32_t OnRilAdapterHostDied();

private:
    int32_t slotId_;
    std::unique_ptr<SmsSendManager> smsSendManager_;
    std::unique_ptr<SmsReceiveManager> smsReceiveManager_;
    std::shared_ptr<SmsMiscManager> smsMiscManager_;
    std::unique_ptr<MmsSendManager> mmsSendManager_;
    std::unique_ptr<MmsReceiveManager> mmsReceiverManager_;
};
} // namespace Telephony
} // namespace OHOS
#endif