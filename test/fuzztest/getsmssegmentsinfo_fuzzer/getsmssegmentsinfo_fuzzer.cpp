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

#include "getsmssegmentsinfo_fuzzer.h"

#define private public
#include "addsmstoken_fuzzer.h"
#include "cdma_sms_message.h"
#include "napi_util.h"
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

void GetSmsSegmentsInfo(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }

    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option(MessageOption::TF_SYNC);

    int32_t slotId = static_cast<int32_t>(size % SLOT_NUM);
    std::string message(reinterpret_cast<const char *>(data), size);
    auto messageU16 = Str8ToStr16(message);
    bool force7BitCode = slotId == 1 ? true : false;

    dataParcel.WriteInt32(slotId);
    dataParcel.WriteString16(messageU16);
    dataParcel.WriteBool(force7BitCode);
    dataParcel.RewindRead(0);

    DelayedSingleton<SmsService>::GetInstance()->OnGetSmsSegmentsInfo(dataParcel, replyParcel, option);

    std::shared_ptr<SmsInterfaceManager> interfaceManager = std::make_shared<SmsInterfaceManager>(slotId);
    if (interfaceManager == nullptr) {
        TELEPHONY_LOGE("interfaceManager nullptr error");
        return;
    }
    LengthInfo lenInfo;
    interfaceManager->GetSmsSegmentsInfo(message, force7BitCode, lenInfo);

    auto smsSendManager = std::make_unique<SmsSendManager>(slotId);
    if (smsSendManager == nullptr) {
        TELEPHONY_LOGE("failed to create SmsSendManager");
        return;
    }
    smsSendManager->GetSmsSegmentsInfo(message, force7BitCode, lenInfo);
    CdmaSmsMessage cdmaSmsMessage;
    cdmaSmsMessage.GetSmsSegmentsInfo(message, force7BitCode, lenInfo);
    GsmSmsMessage gsmSmsMessage;
    gsmSmsMessage.GetSmsSegmentsInfo(message, force7BitCode, lenInfo);
}

void IsImsSmsSupported(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }

    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option(MessageOption::TF_SYNC);

    dataParcel.WriteBuffer(data, size);
    dataParcel.RewindRead(0);
    DelayedSingleton<SmsService>::GetInstance()->OnIsImsSmsSupported(dataParcel, replyParcel, option);

    int32_t slotId = static_cast<int32_t>(size % SLOT_NUM);
    std::shared_ptr<SmsInterfaceManager> interfaceManager = std::make_shared<SmsInterfaceManager>(slotId);
    if (interfaceManager == nullptr) {
        TELEPHONY_LOGE("interfaceManager nullptr error");
        return;
    }
    bool isSupported = false;
    interfaceManager->IsImsSmsSupported(slotId, isSupported);
    auto smsSendManager = std::make_unique<SmsSendManager>(slotId);
    if (smsSendManager == nullptr) {
        TELEPHONY_LOGE("failed to create SmsSendManager");
        return;
    }
    smsSendManager->IsImsSmsSupported(slotId, isSupported);
}

void DoSomethingInterestingWithMyAPI(const uint8_t *data, size_t size)
{
    if (data == nullptr || size == 0) {
        return;
    }

    GetSmsSegmentsInfo(data, size);
    IsImsSmsSupported(data, size);
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
