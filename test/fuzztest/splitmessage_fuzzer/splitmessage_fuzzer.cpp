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

#include "splitmessage_fuzzer.h"

#define private public
#include "addsmstoken_fuzzer.h"
#include "cdma_sms_message.h"
#include "napi_util.h"
#include "sms_service.h"

using namespace OHOS::Telephony;
namespace OHOS {
static bool g_isInited = false;
static int32_t SLOT_NUM = 2;

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

void SplitMessage(const uint8_t *data, size_t size)
{
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option(MessageOption::TF_SYNC);

    std::string message(reinterpret_cast<const char *>(data), size);
    auto messageU16 = Str8ToStr16(message);
    dataParcel.WriteString16(messageU16);
    dataParcel.RewindRead(0);

    DelayedSingleton<SmsService>::GetInstance()->OnSplitMessage(dataParcel, replyParcel, option);

    int32_t slotId = static_cast<int32_t>(size % SLOT_NUM);
    std::shared_ptr<SmsInterfaceManager> interfaceManager = std::make_shared<SmsInterfaceManager>(slotId);
    if (interfaceManager == nullptr) {
        TELEPHONY_LOGE("interfaceManager nullptr error");
        return;
    }

    std::string messageData(reinterpret_cast<const char *>(data), size);
    std::vector<std::u16string> splitMessage;
    interfaceManager->SplitMessage(messageData, splitMessage);
    auto smsSendManager = std::make_unique<SmsSendManager>(slotId);
    if (smsSendManager == nullptr) {
        TELEPHONY_LOGE("failed to create SmsSendManager");
        return;
    }
    smsSendManager->SplitMessage(messageData, splitMessage);

    SmsCodingScheme codingType;
    std::vector<struct SplitInfo> cellsInfos;
    GsmSmsMessage gsmSmsMessage;
    gsmSmsMessage.SplitMessage(cellsInfos, messageData, false, codingType, false);
    CdmaSmsMessage cdmaSmsMessage;
    cdmaSmsMessage.SplitMessage(cellsInfos, messageData, false, codingType, false);
}

void GetImsShortMessageFormat(const uint8_t *data, size_t size)
{
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option(MessageOption::TF_SYNC);

    dataParcel.WriteBuffer(data, size);
    dataParcel.RewindRead(0);
    DelayedSingleton<SmsService>::GetInstance()->OnGetImsShortMessageFormat(dataParcel, replyParcel, option);

    int32_t slotId = static_cast<int32_t>(size % SLOT_NUM);
    std::shared_ptr<SmsInterfaceManager> interfaceManager = std::make_shared<SmsInterfaceManager>(slotId);
    if (interfaceManager == nullptr) {
        TELEPHONY_LOGE("interfaceManager nullptr error");
        return;
    }
    std::u16string format;
    interfaceManager->GetImsShortMessageFormat(format);

    auto smsSendManager = std::make_unique<SmsSendManager>(slotId);
    if (smsSendManager == nullptr) {
        TELEPHONY_LOGE("failed to create SmsSendManager");
        return;
    }
    smsSendManager->GetImsShortMessageFormat(format);
}

void DoSomethingInterestingWithMyAPI(const uint8_t *data, size_t size)
{
    if (data == nullptr || size == 0) {
        return;
    }

    if (!IsServiceInited()) {
        return;
    }

    SplitMessage(data, size);
    GetImsShortMessageFormat(data, size);
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
