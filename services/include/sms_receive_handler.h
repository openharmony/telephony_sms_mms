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

#include <memory>
#include <unordered_map>
#include <vector>

#include "event_handler.h"
#include "event_runner.h"

#include "hril_sms_parcel.h"
#include "sms_base_message.h"
#include "sms_common.h"
#include "sms_receive_indexer.h"
#include "sms_persist_helper.h"
#include "sms_wap_push_handler.h"

namespace OHOS {
namespace Telephony {
class SmsReceiveHandler : public AppExecFwk::EventHandler {
public:
    SmsReceiveHandler(const std::shared_ptr<AppExecFwk::EventRunner> &runner, int32_t slotId);
    virtual ~SmsReceiveHandler();
    virtual void ProcessEvent(const AppExecFwk::InnerEvent::Pointer &event) override;

protected:
    virtual int32_t HandleSmsByType(const std::shared_ptr<SmsBaseMessage> &smsBaseMessage) = 0;
    virtual void ReplySmsToSmsc(int result, const std::shared_ptr<SmsBaseMessage> &response) = 0;
    virtual std::shared_ptr<SmsBaseMessage> TransformMessageInfo(const std::shared_ptr<SmsMessageInfo> &info) = 0;
    void CombineMessagePart(const std::shared_ptr<SmsReceiveIndexer> &smsIndexer);
    void DeleteMessageFormDb(const std::shared_ptr<SmsReceiveIndexer> &smsIndexer);
    bool IsRepeatedMessagePart(const std::shared_ptr<SmsReceiveIndexer> &smsIndexer);
    bool AddMsgToDB(const std::shared_ptr<SmsReceiveIndexer> &indexer);
    bool CheckBlockPhone(const std::shared_ptr<SmsReceiveIndexer> &indexer);
    bool CheckSmsCapable();
    int32_t slotId_ = -1;

private:
    constexpr static uint16_t PDU_POS_OFFSET = 1;
    constexpr static int32_t TEXT_MSG_RECEIVE_CODE = 0;
    constexpr static int32_t DATA_MSG_RECEIVE_CODE = 1;

    void SendBroadcast(
        const std::shared_ptr<SmsReceiveIndexer> &indexer, const std::shared_ptr<std::vector<std::string>> &pdus);
    void HandleReceivedSms(const std::shared_ptr<SmsBaseMessage> &smsBaseMessage);
    SmsReceiveHandler(const SmsReceiveHandler &) = delete;
    SmsReceiveHandler(const SmsReceiveHandler &&) = delete;
    SmsReceiveHandler &operator=(const SmsReceiveHandler &) = delete;
    SmsReceiveHandler &operator=(const SmsReceiveHandler &&) = delete;

    std::unique_ptr<SmsWapPushHandler> smsWapPushHandler_;
};
} // namespace Telephony
} // namespace OHOS
#endif