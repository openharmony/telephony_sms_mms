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

#include "setgetsmsc_fuzzer.h"

#define private public

#include "addsmstoken_fuzzer.h"
#include "sms_service.h"

using namespace OHOS::Telephony;
namespace OHOS {
static bool g_isInited = false;
static int32_t SIM_COUNT = 2;

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

void SetSmscFuzz(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }

    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option(MessageOption::TF_SYNC);

    int32_t slotId = static_cast<int32_t>(size % SIM_COUNT);
    std::string smsc(reinterpret_cast<const char *>(data), size);
    auto smscU16 = Str8ToStr16(smsc);

    dataParcel.WriteInt32(slotId);
    dataParcel.WriteString16(smscU16);
    size_t dataSize = size - sizeof(uint32_t);
    dataParcel.WriteBuffer(data + sizeof(uint32_t), dataSize);
    dataParcel.RewindRead(0);
    DelayedSingleton<SmsService>::GetInstance()->OnSetSmscAddr(dataParcel, replyParcel, option);

    std::shared_ptr<SmsInterfaceManager> interfaceManager = std::make_shared<SmsInterfaceManager>(slotId);
    if (interfaceManager == nullptr) {
        TELEPHONY_LOGE("interfaceManager nullptr error");
        return;
    }
    interfaceManager->SetSmscAddr(smsc);
}

void GetSmscFuzz(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option(MessageOption::TF_SYNC);

    int32_t slotId = static_cast<int32_t>(size % SIM_COUNT);
    dataParcel.WriteInt32(slotId);
    size_t dataSize = size - sizeof(uint32_t);
    dataParcel.WriteBuffer(data + sizeof(uint32_t), dataSize);
    dataParcel.RewindRead(0);
    DelayedSingleton<SmsService>::GetInstance()->OnGetSmscAddr(dataParcel, replyParcel, option);

    std::shared_ptr<SmsInterfaceManager> interfaceManager = std::make_shared<SmsInterfaceManager>(slotId);
    if (interfaceManager == nullptr) {
        TELEPHONY_LOGE("interfaceManager nullptr error");
        return;
    }
    std::u16string smscAddress;
    interfaceManager->GetSmscAddr(smscAddress);
}

void DoSetGetSmscMyAPI(const uint8_t *data, size_t size)
{
    if (data == nullptr || size == 0) {
        return;
    }
    SetSmscFuzz(data, size);
    GetSmscFuzz(data, size);
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    /* Run your code on data */
    OHOS::AddSmsTokenFuzzer token;
    OHOS::DoSetGetSmscMyAPI(data, size);
    return 0;
}
