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

#ifndef SMS_RECEIVE_HANDLER_H
#define SMS_RECEIVE_HANDLER_H

#include <vector>

#include "tel_ril_sms_parcel.h"
#include "sms_base_message.h"
#include "sms_receive_indexer.h"
#include "sms_receive_reliability_handler.h"
#include "sms_wap_push_handler.h"
#include "tel_event_handler.h"

namespace OHOS {
namespace Telephony {
class SmsReceiveHandler : public TelEventHandler {
public:
    explicit SmsReceiveHandler(int32_t slotId);
    virtual ~SmsReceiveHandler();
    virtual void ProcessEvent(const AppExecFwk::InnerEvent::Pointer &event) override;

protected:
    virtual int32_t HandleSmsByType(const std::shared_ptr<SmsBaseMessage> smsBaseMessage) = 0;
    virtual void ReplySmsToSmsc(int result, const std::shared_ptr<SmsBaseMessage> response) = 0;
    virtual std::shared_ptr<SmsBaseMessage> TransformMessageInfo(const std::shared_ptr<SmsMessageInfo> info) = 0;
    void CombineMessagePart(const std::shared_ptr<SmsReceiveIndexer> &smsIndexer);
    bool IsRepeatedMessagePart(const std::shared_ptr<SmsReceiveIndexer> &smsIndexer);
    bool AddMsgToDB(const std::shared_ptr<SmsReceiveIndexer> indexer);

private:
    SmsReceiveHandler(const SmsReceiveHandler &) = delete;
    SmsReceiveHandler(const SmsReceiveHandler &&) = delete;
    SmsReceiveHandler &operator=(const SmsReceiveHandler &) = delete;
    SmsReceiveHandler &operator=(const SmsReceiveHandler &&) = delete;
    void HandleReceivedSms(const std::shared_ptr<SmsBaseMessage> smsBaseMessage);
    bool CombineMultiPageMessage(const std::shared_ptr<SmsReceiveIndexer> &indexer,
        std::shared_ptr<std::vector<std::string>> pdus,
        std::shared_ptr<SmsReceiveReliabilityHandler> reliabilityHandler);
    void UpdateMultiPageMessage(
        const std::shared_ptr<SmsReceiveIndexer> &indexer, std::shared_ptr<std::vector<std::string>> pdus);

protected:
    int32_t slotId_ = -1;

private:
    std::unique_ptr<SmsWapPushHandler> smsWapPushHandler_;
};
} // namespace Telephony
} // namespace OHOS
#endif