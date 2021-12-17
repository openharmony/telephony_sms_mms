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

#ifndef GSM_SMS_SENDER_H
#define GSM_SMS_SENDER_H

#include <functional>
#include <memory>
#include <string>
#include <mutex>

#include "event_runner.h"

#include "gsm_sms_message.h"
#include "i_delivery_short_message_callback.h"
#include "i_send_short_message_callback.h"
#include "sms_send_indexer.h"
#include "sms_sender.h"

namespace OHOS {
namespace Telephony {
class GsmSmsSender : public SmsSender {
public:
    GsmSmsSender(const std::shared_ptr<AppExecFwk::EventRunner> &runner, int32_t slotId,
        const std::function<void(std::shared_ptr<SmsSendIndexer>)> sendRetryFun);
    ~GsmSmsSender() override;
    void Init() override;
    void TextBasedSmsDelivery(const std::string &desAddr, const std::string &scAddr, const std::string &text,
        const sptr<ISendShortMessageCallback> &sendCallback,
        const sptr<IDeliveryShortMessageCallback> &deliveryCallback) override;
    void DataBasedSmsDelivery(const std::string &desAddr, const std::string &scAddr, int32_t port,
        const uint8_t *data, uint32_t dataLen, const sptr<ISendShortMessageCallback> &sendCallback,
        const sptr<IDeliveryShortMessageCallback> &deliveryCallback) override;
    void SendSmsToRil(const std::shared_ptr<SmsSendIndexer> &smsIndexer) override;
    void ResendTextDelivery(const std::shared_ptr<SmsSendIndexer> &smsIndexer) override;
    void ResendDataDelivery(const std::shared_ptr<SmsSendIndexer> &smsIndexer) override;
    std::vector<std::string> SplitMessage(const std::string &message) override;
    std::vector<int32_t> CalculateLength(const std::string &message, bool force7BitCode) override;

protected:
    void StatusReportAnalysis(const AppExecFwk::InnerEvent::Pointer &event) override;

private:
    void SetSendIndexerInfo(const std::shared_ptr<SmsSendIndexer> &indexer,
        const std::shared_ptr<struct EncodeInfo> &encodeInfo, unsigned char msgRef8bit);
    bool RegisterHandler();
    bool SetPduInfo(const std::shared_ptr<SmsSendIndexer> &smsIndexer, GsmSmsMessage &gsmSmsMessage, bool &isMore);
    int SetConcatData(unsigned char msgRef8bit, int cellsInfosSize,
        std::shared_ptr<SmsSendIndexer> &indexer, GsmSmsMessage &gsmSmsMessage, int i);
    bool SetUserData(std::shared_ptr<struct SmsTpdu> &tpdu,
        const std::vector<struct SplitInfo> &cellsInfos, int i);
    bool SetStatusReport(int cellsInfosSize, int i, std::shared_ptr<struct SmsTpdu> &tpdu,
        bool isStatusReport);
    void SetSendStatusIndexerInfo(const std::shared_ptr<uint8_t> &unSentCellCount,
        const std::shared_ptr<bool> &hasCellFailed, const std::vector<struct SplitInfo> &cellsInfos,
        std::shared_ptr<SmsSendIndexer> &indexer, int i);

    std::mutex mutex_;
};
} // namespace Telephony
} // namespace OHOS
#endif