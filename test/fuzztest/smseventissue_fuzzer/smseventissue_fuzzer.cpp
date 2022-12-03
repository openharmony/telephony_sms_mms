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

#include "smseventissue_fuzzer.h"

#define private public
#define protected public

#include "addsmstoken_fuzzer.h"
#include "delivery_short_message_callback_stub.h"
#include "send_short_message_callback_stub.h"
#include "sms_interface_manager.h"

using namespace OHOS::Telephony;
namespace OHOS {
const std::int32_t MAX_EVENT = 1024;
const std::int32_t MIDDLE_EVENT = 50;
const std::int32_t EVERY_STEP = 100;
const std::int32_t SLOT_NUM = 2;

void DoSentIssueTest(const uint8_t *data, size_t size, std::shared_ptr<SmsSendManager> smsSendManager)
{
    std::string desAddr(reinterpret_cast<const char *>(data), size);
    std::string scAddr(reinterpret_cast<const char *>(data), size);
    std::string text(reinterpret_cast<const char *>(data), size);
    const sptr<ISendShortMessageCallback> sendCallback =
        iface_cast<ISendShortMessageCallback>(new SendShortMessageCallbackStub());
    if (sendCallback == nullptr) {
        return;
    }
    const sptr<IDeliveryShortMessageCallback> deliveryCallback =
        iface_cast<IDeliveryShortMessageCallback>(new DeliveryShortMessageCallbackStub());
    if (deliveryCallback == nullptr) {
        return;
    }
    const std::shared_ptr<SmsSendIndexer> smsSendIndexer =
        std::make_shared<SmsSendIndexer>(desAddr, scAddr, text, sendCallback, deliveryCallback);
    smsSendManager->gsmSmsSender_->SendMessageSucceed(smsSendIndexer);
    smsSendManager->cdmaSmsSender_->SendMessageSucceed(smsSendIndexer);
    smsSendManager->gsmSmsSender_->SendMessageFailed(smsSendIndexer);
    smsSendManager->cdmaSmsSender_->SendMessageFailed(smsSendIndexer);
    smsSendManager->gsmSmsSender_->HandleResend(smsSendIndexer);
    smsSendManager->cdmaSmsSender_->HandleResend(smsSendIndexer);

    std::int64_t erase = static_cast<int64_t>(size);
    smsSendManager->gsmSmsSender_->SendCacheMapEraseItem(erase);
    smsSendManager->cdmaSmsSender_->SendCacheMapEraseItem(erase);

    std::int32_t eventId = static_cast<int32_t>(size);
    std::int64_t refId = static_cast<int64_t>(size);
    AppExecFwk::InnerEvent::Pointer response = AppExecFwk::InnerEvent::Get(eventId, refId);
    smsSendManager->gsmSmsSender_->FindCacheMapAndTransform(response);
    smsSendManager->cdmaSmsSender_->FindCacheMapAndTransform(response);

    std::int32_t slotId = static_cast<int32_t>(size % SLOT_NUM);
    std::int32_t enable = static_cast<int32_t>(size % SLOT_NUM);
    smsSendManager->gsmSmsSender_->SetImsSmsConfig(slotId, enable);
    smsSendManager->cdmaSmsSender_->SetImsSmsConfig(slotId, enable);

    bool isImsNetDomain = slotId == 1 ? true : false;
    std::int32_t voiceServiceState = static_cast<int32_t>(size);
    smsSendManager->gsmSmsSender_->SetNetworkState(isImsNetDomain, voiceServiceState);
    smsSendManager->cdmaSmsSender_->SetNetworkState(isImsNetDomain, voiceServiceState);

    smsSendManager->gsmSmsSender_->SendCacheMapTimeoutCheck();
    smsSendManager->cdmaSmsSender_->SendCacheMapTimeoutCheck();
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
    auto smsSendManager = std::make_shared<SmsSendManager>(slotId);
    if (smsSendManager == nullptr) {
        return;
    }
    smsSendManager->Init();
    if (smsSendManager->gsmSmsSender_ == nullptr || smsSendManager->cdmaSmsSender_ == nullptr) {
        return;
    }

    std::int32_t eventId = static_cast<int32_t>(size);
    std::int64_t refId = static_cast<int64_t>(size);

    while (eventId < MIDDLE_EVENT) {
        AppExecFwk::InnerEvent::Pointer response = AppExecFwk::InnerEvent::Get(eventId, refId);
        smsSendManager->gsmSmsSender_->ProcessEvent(response);
        smsSendManager->cdmaSmsSender_->ProcessEvent(response);
        eventId++;
    }

    while (eventId < MAX_EVENT) {
        AppExecFwk::InnerEvent::Pointer response = AppExecFwk::InnerEvent::Get(eventId, refId);
        smsSendManager->gsmSmsSender_->ProcessEvent(response);
        smsSendManager->cdmaSmsSender_->ProcessEvent(response);
        eventId += EVERY_STEP;
    }

    DoSentIssueTest(data, size, smsSendManager);
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
