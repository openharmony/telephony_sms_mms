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

#ifndef SMS_SEND_MANAGER_H
#define SMS_SEND_MANAGER_H

#include <list>
#include <memory>
#include <string>

#include "cdma_sms_sender.h"
#include "gsm_sms_sender.h"
#include "sms_send_indexer.h"
#include "sms_network_policy_manager.h"

namespace OHOS {
namespace Telephony {
class SmsSendManager {
public:
    explicit SmsSendManager(int32_t slotId);
    virtual ~SmsSendManager();
    void TextBasedSmsDelivery(const std::string &desAddr, const std::string &scAddr, const std::string &text,
        const sptr<ISendShortMessageCallback> &sendCallback,
        const sptr<IDeliveryShortMessageCallback> &deliveryCallback);
    void DataBasedSmsDelivery(const std::string &desAddr, const std::string &scAddr, const uint16_t port,
        const uint8_t *data, uint16_t dataLen, const sptr<ISendShortMessageCallback> &sendCallback,
        const sptr<IDeliveryShortMessageCallback> &deliveryCallback);
    void RetriedSmsDelivery(const std::shared_ptr<SmsSendIndexer> smsIndexer);
    void Init();
    void InitNetworkHandle();
    std::shared_ptr<SmsSender> GetCdmaSmsSender() const;
    int32_t SplitMessage(const std::string &message, std::vector<std::u16string> &splitMessage);
    int32_t GetSmsSegmentsInfo(const std::string &message, bool force7BitCode, LengthInfo &lenInfo);
    int32_t IsImsSmsSupported(int32_t slotId, bool &isSupported);
    bool SetImsSmsConfig(int32_t slotId, int32_t enable);
    int32_t GetImsShortMessageFormat(std::u16string &format);

private:
    SmsSendManager &operator=(const SmsSendManager &) = delete;
    SmsSendManager &operator=(const SmsSendManager &&) = delete;
    SmsSendManager(const SmsSendManager &) = delete;
    SmsSendManager(const SmsSendManager &&) = delete;

    int32_t slotId_;
    std::shared_ptr<SmsSender> gsmSmsSender_;
    std::shared_ptr<SmsSender> cdmaSmsSender_;
    std::shared_ptr<AppExecFwk::EventRunner> gsmSmsSendRunner_;
    std::shared_ptr<AppExecFwk::EventRunner> cdmaSmsSendRunner_;
    std::shared_ptr<SmsNetworkPolicyManager> networkManager_;
    std::shared_ptr<AppExecFwk::EventRunner> networkRunner_;
};
} // namespace Telephony
} // namespace OHOS
#endif