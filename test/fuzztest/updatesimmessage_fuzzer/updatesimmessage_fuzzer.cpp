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

#include "updatesimmessage_fuzzer.h"

#define private public
#include "addsmstoken_fuzzer.h"
#include "i_sms_service_interface.h"
#include "napi_util.h"
#include "sms_service.h"

using namespace OHOS::Telephony;
namespace OHOS {
static bool g_isInited = false;
constexpr int32_t SLOT_NUM = 2;
constexpr int32_t SIM_MESSAGE_STATUE = 4;

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

void UpdateSimMessage(const uint8_t *data, size_t size)
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
    ISmsServiceInterface::SimMessageStatus status =
        static_cast<ISmsServiceInterface::SimMessageStatus>(size % SIM_MESSAGE_STATUE);

    dataParcel.WriteInt32(slotId);
    dataParcel.WriteUint32(size);
    dataParcel.WriteUint32(status);
    dataParcel.WriteString16(smscU16);
    dataParcel.WriteString16(pduU16);
    dataParcel.RewindRead(0);

    DelayedSingleton<SmsService>::GetInstance()->OnUpdateSimMessage(dataParcel, replyParcel, option);

    std::shared_ptr<SmsInterfaceManager> interfaceManager = std::make_shared<SmsInterfaceManager>(slotId);
    if (interfaceManager == nullptr) {
        TELEPHONY_LOGE("interfaceManager nullptr error");
        return;
    }
    interfaceManager->UpdateSimMessage(size, status, pdu, smsc);

    auto smsMiscRunner = AppExecFwk::EventRunner::Create("SmsMiscRunner");
    if (smsMiscRunner == nullptr) {
        TELEPHONY_LOGE("failed to create SmsMiscRunner");
        return;
    }
    std::shared_ptr<SmsMiscManager> smsMiscManager = std::make_shared<SmsMiscManager>(smsMiscRunner, slotId);
    if (smsMiscManager == nullptr) {
        TELEPHONY_LOGE("smsMiscManager nullptr error");
        return;
    }
    smsMiscManager->UpdateSimMessage(size, status, pdu, smsc);
}

void DoSomethingInterestingWithMyAPI(const uint8_t *data, size_t size)
{
    if (data == nullptr || size == 0) {
        return;
    }

    UpdateSimMessage(data, size);
}
}  // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::AddSmsTokenFuzzer token;
    OHOS::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}
