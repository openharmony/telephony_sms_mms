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

#include "setgetcbconfig_fuzzer.h"

#define private public

#include "addsmstoken_fuzzer.h"
#include "sms_service.h"
#include "string_utils.h"

using namespace OHOS::Telephony;
namespace OHOS {
static bool g_isInited = false;
static int32_t SIM_COUNT = 2;
static int32_t CB_CHANNEL_DIVISOR = 2;
static int32_t NET_COUNT = 3;

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

void SetCBConfigFuzz(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }

    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option(MessageOption::TF_SYNC);

    int32_t slotId = static_cast<int32_t>(size % SIM_COUNT);
    bool enable = slotId == 1 ? true : false;
    uint32_t fromMsgId = static_cast<uint32_t>(size / CB_CHANNEL_DIVISOR);
    uint32_t toMsgId = static_cast<uint32_t>(size);
    int32_t netType = static_cast<int32_t>(size % NET_COUNT);

    dataParcel.WriteInt32(slotId);
    dataParcel.WriteBool(enable);
    dataParcel.WriteUint32(fromMsgId);
    dataParcel.WriteUint32(toMsgId);
    dataParcel.WriteUint8(netType);

    size_t dataSize = size - sizeof(uint32_t);
    dataParcel.WriteBuffer(data + sizeof(uint32_t), dataSize);
    dataParcel.RewindRead(0);
    DelayedSingleton<SmsService>::GetInstance()->OnSetCBConfig(dataParcel, replyParcel, option);

    std::shared_ptr<SmsInterfaceManager> interfaceManager = std::make_shared<SmsInterfaceManager>(slotId);
    if (interfaceManager == nullptr) {
        TELEPHONY_LOGE("interfaceManager nullptr error");
        return;
    }
    interfaceManager->SetCBConfig(enable, fromMsgId, toMsgId, netType);

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
    smsMiscManager->SetCBConfig(enable, fromMsgId, toMsgId, netType);
}

void SetImsSmsConfigFuzz(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }

    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option(MessageOption::TF_SYNC);

    int32_t slotId = static_cast<int32_t>(size % SIM_COUNT);
    int32_t enable = slotId == 1 ? true : false;
    dataParcel.WriteInt32(slotId);
    dataParcel.WriteInt32(enable);
    size_t dataSize = size - sizeof(uint32_t);

    dataParcel.WriteBuffer(data + sizeof(uint32_t), dataSize);
    dataParcel.RewindRead(0);
    DelayedSingleton<SmsService>::GetInstance()->OnSetImsSmsConfig(dataParcel, replyParcel, option);

    std::shared_ptr<SmsInterfaceManager> interfaceManager = std::make_shared<SmsInterfaceManager>(slotId);
    if (interfaceManager == nullptr) {
        TELEPHONY_LOGE("interfaceManager nullptr error");
        return;
    }
    interfaceManager->SetImsSmsConfig(slotId, enable);

    auto smsSendManager = std::make_shared<SmsSendManager>(slotId);
    if (smsSendManager == nullptr) {
        return;
    }
    smsSendManager->SetImsSmsConfig(slotId, enable);
}

void UpdataCBMessage(const uint8_t *data, size_t size)
{
    std::string pdu(reinterpret_cast<const char *>(data), size);
    auto cbMessage = SmsCbMessage::CreateCbMessage(pdu);
    if (cbMessage == nullptr) {
        return;
    }
    cbMessage->GetCbHeader();
    auto cbMessageByVectorInit = SmsCbMessage::CreateCbMessage(StringUtils::HexToByteVector(pdu));
    if (cbMessageByVectorInit == nullptr) {
        return;
    }
    cbMessageByVectorInit->GetCbMessageRaw();
    cbMessageByVectorInit->IsSinglePageMsg();

    cbMessage->PduAnalysis(StringUtils::HexToByteVector(pdu));
    cbMessage->Decode2gHeader(StringUtils::HexToByteVector(pdu));
    cbMessage->Decode3gHeader(StringUtils::HexToByteVector(pdu));

    cbMessage->Decode2gCbMsg(StringUtils::HexToByteVector(pdu));
    cbMessage->Decode3gCbMsg(StringUtils::HexToByteVector(pdu));
    cbMessage->Decode3g7Bit(StringUtils::HexToByteVector(pdu));
    cbMessage->Decode3gUCS2(StringUtils::HexToByteVector(pdu));
    cbMessage->DecodeEtwsMsg(StringUtils::HexToByteVector(pdu));

    std::string raw(reinterpret_cast<const char *>(data), size);
    std::string message(reinterpret_cast<const char *>(data), size);
    cbMessage->ConvertToUTF8(raw, message);
}

void DoCBConfigWithMyAPI(const uint8_t *data, size_t size)
{
    if (data == nullptr || size == 0) {
        return;
    }
    SetCBConfigFuzz(data, size);
    SetImsSmsConfigFuzz(data, size);
    UpdataCBMessage(data, size);
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    /* Run your code on data */
    OHOS::AddSmsTokenFuzzer token;
    OHOS::DoCBConfigWithMyAPI(data, size);
    return 0;
}
