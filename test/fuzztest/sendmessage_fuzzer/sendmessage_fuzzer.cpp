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

#include "sendmessage_fuzzer.h"

#define private public

#include "addsmstoken_fuzzer.h"
#include "delivery_send_call_back_stub.h"
#include "napi_util.h"
#include "send_call_back_stub.h"
#include "sms_service.h"

using namespace OHOS::Telephony;
namespace OHOS {
static bool g_isInited = false;
constexpr int32_t SLOT_NUM = 2;

bool IsServiceInited()
{
    if (!g_isInited) {
        DelayedSingleton<SmsService>::GetInstance()->OnStart();
        if (DelayedSingleton<SmsService>::GetInstance()->GetServiceRunningState() ==
            static_cast<int32_t>(Telephony::ServiceRunningState::STATE_RUNNING)) {
            g_isInited = true;
        }
    }
    return g_isInited;
}

void SendSmsTextRequest(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }

    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option(MessageOption::TF_SYNC);

    int32_t slotId = static_cast<int32_t>(size % SLOT_NUM);
    std::string desAddr(reinterpret_cast<const char *>(data), size);
    std::string scAddr(reinterpret_cast<const char *>(data), size);
    std::string text(reinterpret_cast<const char *>(data), size);
    auto desAddrU16 = Str8ToStr16(desAddr);
    auto scAddrU16 = Str8ToStr16(scAddr);
    auto textU16 = Str8ToStr16(text);

    std::unique_ptr<SendCallbackStub> sendCallback = std::make_unique<SendCallbackStub>();
    std::unique_ptr<DeliverySendCallbackStub> deliveryCallback = std::make_unique<DeliverySendCallbackStub>();

    dataParcel.WriteInt32(slotId);
    dataParcel.WriteString16(desAddrU16);
    dataParcel.WriteString16(scAddrU16);
    dataParcel.WriteString16(textU16);
    if (sendCallback != nullptr) {
        dataParcel.WriteRemoteObject(sendCallback.release()->AsObject().GetRefPtr());
    }
    if (deliveryCallback != nullptr) {
        dataParcel.WriteRemoteObject(deliveryCallback.release()->AsObject().GetRefPtr());
    }
    dataParcel.RewindRead(0);

    DelayedSingleton<SmsService>::GetInstance()->OnSendSmsTextRequest(dataParcel, replyParcel, option);
}

void GetDefaultSmsSlotId(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }

    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option(MessageOption::TF_SYNC);

    dataParcel.WriteBuffer(data, size);
    dataParcel.RewindRead(0);
    DelayedSingleton<SmsService>::GetInstance()->OnGetDefaultSmsSlotId(dataParcel, replyParcel, option);
    return;
}

void DoSomethingInterestingWithMyAPI(const uint8_t *data, size_t size)
{
    if (data == nullptr || size == 0) {
        return;
    }

    SendSmsTextRequest(data, size);
    GetDefaultSmsSlotId(data, size);
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::AddSmsTokenFuzzer token;
    OHOS::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}
