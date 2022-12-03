/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "smsrecvitem_fuzzer.h"

#define private public
#define protected public

#include "addsmstoken_fuzzer.h"
#include "cdma_sms_message.h"
#include "delivery_short_message_callback_stub.h"
#include "send_short_message_callback_stub.h"
#include "sms_interface_manager.h"

using namespace OHOS::Telephony;
namespace OHOS {
const std::int32_t MAX_EVENT = 1024;
const std::int32_t MIDDLE_EVENT = 50;
const std::int32_t EVERY_STEP = 100;
constexpr int32_t SLOT_NUM = 2;

void DoRecvItemsTest(const uint8_t *data, size_t size, std::shared_ptr<SmsReceiveManager> smsReceiveManager)
{
    const std::shared_ptr<GsmSmsMessage> smsGsmMessage = std::make_shared<GsmSmsMessage>();
    const std::shared_ptr<CdmaSmsMessage> smsCdmaMessage = std::make_shared<CdmaSmsMessage>();
    smsReceiveManager->gsmSmsReceiveHandler_->HandleReceivedSms(smsGsmMessage);
    smsReceiveManager->cdmaSmsReceiveHandler_->HandleReceivedSms(smsCdmaMessage);
    smsReceiveManager->gsmSmsReceiveHandler_->HandleSmsByType(smsGsmMessage);
    smsReceiveManager->cdmaSmsReceiveHandler_->HandleSmsByType(smsCdmaMessage);

    std::int32_t result = static_cast<int32_t>(size);
    smsReceiveManager->gsmSmsReceiveHandler_->ReplySmsToSmsc(result, smsGsmMessage);
    smsReceiveManager->cdmaSmsReceiveHandler_->ReplySmsToSmsc(result, smsCdmaMessage);

    const std::shared_ptr<SmsReceiveIndexer> smsReceiveIndexer = std::make_shared<SmsReceiveIndexer>();
    smsReceiveManager->gsmSmsReceiveHandler_->CombineMessagePart(smsReceiveIndexer);
    smsReceiveManager->cdmaSmsReceiveHandler_->CombineMessagePart(smsReceiveIndexer);
    smsReceiveManager->gsmSmsReceiveHandler_->DeleteMessageFormDb(smsReceiveIndexer);
    smsReceiveManager->cdmaSmsReceiveHandler_->DeleteMessageFormDb(smsReceiveIndexer);
    smsReceiveManager->gsmSmsReceiveHandler_->IsRepeatedMessagePart(smsReceiveIndexer);
    smsReceiveManager->cdmaSmsReceiveHandler_->IsRepeatedMessagePart(smsReceiveIndexer);
}

void DoSomethingInterestingWithMyAPI(const uint8_t *data, size_t size)
{
    if (data == nullptr || size == 0) {
        return;
    }

    int32_t slotId = static_cast<int32_t>(size % SLOT_NUM);
    auto smsInterfaceManager = std::make_shared<SmsInterfaceManager>(slotId);
    if (smsInterfaceManager == nullptr) {
        return;
    }

    smsInterfaceManager->InitInterfaceManager();
    auto smsReceiveManager = std::make_shared<SmsReceiveManager>(slotId);
    if (smsReceiveManager == nullptr) {
        return;
    }
    smsReceiveManager->Init();
    if (smsReceiveManager->gsmSmsReceiveHandler_ == nullptr || smsReceiveManager->cdmaSmsReceiveHandler_ == nullptr) {
        return;
    }
    smsReceiveManager->gsmSmsReceiveHandler_->Init();
    smsReceiveManager->cdmaSmsReceiveHandler_->Init();

    std::int32_t eventId = static_cast<int32_t>(size);
    std::int64_t refId = static_cast<int64_t>(size);

    while (eventId < MIDDLE_EVENT) {
        AppExecFwk::InnerEvent::Pointer response = AppExecFwk::InnerEvent::Get(eventId, refId);
        smsReceiveManager->gsmSmsReceiveHandler_->ProcessEvent(response);
        smsReceiveManager->cdmaSmsReceiveHandler_->ProcessEvent(response);
        eventId++;
    }

    while (eventId < MAX_EVENT) {
        AppExecFwk::InnerEvent::Pointer response = AppExecFwk::InnerEvent::Get(eventId, refId);
        smsReceiveManager->gsmSmsReceiveHandler_->ProcessEvent(response);
        smsReceiveManager->cdmaSmsReceiveHandler_->ProcessEvent(response);
        eventId += EVERY_STEP;
    }

    DoRecvItemsTest(data, size, smsReceiveManager);
    return;
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    /* Run your code on data */
    OHOS::AddSmsTokenFuzzer token;
    OHOS::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}
