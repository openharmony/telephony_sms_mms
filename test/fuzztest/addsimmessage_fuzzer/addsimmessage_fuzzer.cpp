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

#include "addsimmessage_fuzzer.h"

#define private public
#include "addsmstoken_fuzzer.h"
#include "i_sms_service_interface.h"
#include "napi_util.h"
#include "sms_interface_stub.h"
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

void OnRemoteRequest(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }

    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(SmsInterfaceStub::GetDescriptor())) {
        TELEPHONY_LOGE("OnRemoteRequest WriteInterfaceToken is false");
        return;
    }

    MessageParcel replyParcel;
    MessageOption option(MessageOption::TF_SYNC);

    size_t dataSize = size - sizeof(uint32_t);
    dataParcel.WriteBuffer(data + sizeof(uint32_t), dataSize);
    dataParcel.RewindRead(0);
    uint32_t code = static_cast<uint32_t>(size);

    DelayedSingleton<SmsService>::GetInstance()->OnRemoteRequest(code, dataParcel, replyParcel, option);
    return;
}

void AddSimMessage(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }

    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option(MessageOption::TF_SYNC);

    std::string smsc(reinterpret_cast<const char *>(data), size);
    std::string pdu(reinterpret_cast<const char *>(data), size);
    auto smscU16 = Str8ToStr16(smsc);
    auto pduU16 = Str8ToStr16(pdu);
    int32_t slotId = static_cast<int32_t>(size % SLOT_NUM);
    ISmsServiceInterface::SimMessageStatus status = static_cast<ISmsServiceInterface::SimMessageStatus>(size % 4);

    dataParcel.WriteInt32(slotId);
    dataParcel.WriteString16(smscU16);
    dataParcel.WriteString16(pduU16);
    dataParcel.WriteUint32(status);
    dataParcel.RewindRead(0);

    DelayedSingleton<SmsService>::GetInstance()->OnAddSimMessage(dataParcel, replyParcel, option);
    return;
}

void HasSmsCapability(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }

    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option(MessageOption::TF_SYNC);

    dataParcel.WriteBuffer(data, size);
    dataParcel.RewindRead(0);
    DelayedSingleton<SmsService>::GetInstance()->OnHasSmsCapability(dataParcel, replyParcel, option);
    return;
}

void DoSomethingInterestingWithMyAPI(const uint8_t *data, size_t size)
{
    if (data == nullptr || size <= 0) {
        return;
    }

    OnRemoteRequest(data, size);
    AddSimMessage(data, size);
    HasSmsCapability(data, size);
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
