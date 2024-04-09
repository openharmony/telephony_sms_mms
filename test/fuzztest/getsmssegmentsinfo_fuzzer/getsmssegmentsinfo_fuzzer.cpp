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
#include "core_manager_inner.h"
#include "ims_sms_callback_stub.h"
#include "sms_service.h"

using namespace OHOS::Telephony;
namespace OHOS {
static bool g_isInited = false;
constexpr int32_t SLOT_NUM = 2;
constexpr int32_t TYPE_NUM = 5;

bool IsServiceInited()
{
    if (!g_isInited) {
        CoreManagerInner::GetInstance().isInitAllObj_ = true;
        DelayedSingleton<SmsService>::GetInstance()->registerToService_ = true;
        DelayedSingleton<SmsService>::GetInstance()->WaitCoreServiceToInit();
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
    dataParcel.RewindRead(0);
    uint32_t code = static_cast<uint32_t>(size);
    DelayedSingleton<ImsSmsCallbackStub>::GetInstance()->OnRemoteRequest(code, dataParcel, replyParcel, option);
    return;
}

void ImsSendMessageResponseInner(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }

    MessageParcel dataParcel;
    MessageParcel replyParcel;

    int32_t slotId = static_cast<int32_t>(size % SLOT_NUM);
    dataParcel.WriteInt32(slotId);
    RadioResponseInfo responseInfo;
    responseInfo.flag = static_cast<uint32_t>(size);
    responseInfo.serial = static_cast<uint32_t>(size);
    responseInfo.error = static_cast<ErrType>(size);
    responseInfo.type = static_cast<ResponseTypes>(size % TYPE_NUM);
    dataParcel.WriteRawData((const void *)&responseInfo, sizeof(RadioResponseInfo));
    SendSmsResultInfo resultInfo;
    resultInfo.msgRef = static_cast<uint32_t>(size);
    std::string pdu(reinterpret_cast<const char *>(data), size);
    resultInfo.pdu = pdu;
    resultInfo.errCode = static_cast<uint32_t>(size);
    resultInfo.flag = static_cast<int64_t>(size);
    dataParcel.WriteRawData((const void *)&resultInfo, sizeof(SendSmsResultInfo));
    dataParcel.RewindRead(0);
    DelayedSingleton<ImsSmsCallbackStub>::GetInstance()->OnImsSendMessageResponseInner(dataParcel, replyParcel);
}

void ImsSetSmsConfigResponseInner(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }

    MessageParcel dataParcel;
    MessageParcel replyParcel;

    int32_t slotId = static_cast<int32_t>(size % SLOT_NUM);
    dataParcel.WriteInt32(slotId);
    RadioResponseInfo responseInfo;
    responseInfo.flag = static_cast<uint32_t>(size);
    responseInfo.serial = static_cast<uint32_t>(size);
    responseInfo.error = static_cast<ErrType>(size);
    responseInfo.type = static_cast<ResponseTypes>(size % TYPE_NUM);
    dataParcel.WriteRawData((const void *)&responseInfo, sizeof(RadioResponseInfo));
    dataParcel.RewindRead(0);
    DelayedSingleton<ImsSmsCallbackStub>::GetInstance()->OnImsSetSmsConfigResponseInner(dataParcel, replyParcel);
}

void CreateMessage(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }

    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option(MessageOption::TF_SYNC);

    std::string pdu(reinterpret_cast<const char *>(data), size);
    std::string reply(reinterpret_cast<const char *>(data), size);
    dataParcel.WriteString(pdu);
    dataParcel.WriteString(reply);
    dataParcel.RewindRead(0);
    DelayedSingleton<SmsService>::GetInstance()->OnCreateMessage(dataParcel, replyParcel, option);
}

void GetBase64Encode(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }

    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option(MessageOption::TF_SYNC);

    std::string message(reinterpret_cast<const char *>(data), size);
    auto messageU16 = Str8ToStr16(message);
    dataParcel.WriteString16(messageU16);
    dataParcel.RewindRead(0);
    DelayedSingleton<SmsService>::GetInstance()->OnGetBase64Encode(dataParcel, replyParcel, option);
}

void GetBase64Decode(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }

    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option(MessageOption::TF_SYNC);

    std::string messageSrc(reinterpret_cast<const char *>(data), size);
    auto stringU16 = Str8ToStr16(messageSrc);
    dataParcel.WriteString16(stringU16);
    dataParcel.RewindRead(0);
    DelayedSingleton<SmsService>::GetInstance()->OnGetBase64Decode(dataParcel, replyParcel, option);
}

void GetEncodeStringFunc(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }

    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option(MessageOption::TF_SYNC);

    int32_t charset = static_cast<int32_t>(size);
    int32_t valLength = static_cast<int32_t>(size);
    std::string strEncode(reinterpret_cast<const char *>(data), size);
    auto strEncodeU16 = Str8ToStr16(strEncode);
    dataParcel.WriteInt32(charset);
    dataParcel.WriteInt32(valLength);
    dataParcel.WriteString16(strEncodeU16);
    dataParcel.RewindRead(0);
    DelayedSingleton<SmsService>::GetInstance()->OnGetEncodeStringFunc(dataParcel, replyParcel, option);
}

void HighRiskInterface(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }

    int32_t slotId = static_cast<int32_t>(size % SLOT_NUM);
    MessageParcel dataParcel;
    dataParcel.WriteInt32(slotId);
    dataParcel.WriteBuffer(data, size);
    dataParcel.RewindRead(0);

    MessageParcel replyParcel;
    uint32_t code = static_cast<uint32_t>(size);
    MessageOption option(MessageOption::TF_SYNC);

    DelayedSingleton<SmsService>::GetInstance()->OnRemoteRequest(code, dataParcel, replyParcel, option);
    DelayedSingleton<SmsService>::GetInstance()->OnSendSmsTextRequest(dataParcel, replyParcel, option);
    DelayedSingleton<SmsService>::GetInstance()->OnSendSmsDataRequest(dataParcel, replyParcel, option);
    DelayedSingleton<SmsService>::GetInstance()->OnGetSmscAddr(dataParcel, replyParcel, option);
    DelayedSingleton<SmsService>::GetInstance()->OnAddSimMessage(dataParcel, replyParcel, option);
    DelayedSingleton<SmsService>::GetInstance()->OnUpdateSimMessage(dataParcel, replyParcel, option);
    DelayedSingleton<SmsService>::GetInstance()->OnGetAllSimMessages(dataParcel, replyParcel, option);
    DelayedSingleton<SmsService>::GetInstance()->OnSplitMessage(dataParcel, replyParcel, option);
    DelayedSingleton<SmsService>::GetInstance()->OnCreateMessage(dataParcel, replyParcel, option);
    DelayedSingleton<SmsService>::GetInstance()->OnGetBase64Encode(dataParcel, replyParcel, option);
    DelayedSingleton<SmsService>::GetInstance()->OnGetBase64Decode(dataParcel, replyParcel, option);
    DelayedSingleton<SmsService>::GetInstance()->OnGetEncodeStringFunc(dataParcel, replyParcel, option);
    DelayedSingleton<SmsService>::GetInstance()->OnGetSmsSegmentsInfo(dataParcel, replyParcel, option);
}

void DoSomethingInterestingWithMyAPI(const uint8_t *data, size_t size)
{
    if (data == nullptr || size == 0) {
        return;
    }

    GetSmsSegmentsInfo(data, size);
    IsImsSmsSupported(data, size);
    OnRemoteRequest(data, size);
    ImsSendMessageResponseInner(data, size);
    ImsSetSmsConfigResponseInner(data, size);
    CreateMessage(data, size);
    GetBase64Encode(data, size);
    GetBase64Decode(data, size);
    GetEncodeStringFunc(data, size);
    HighRiskInterface(data, size);
    DelayedSingleton<SmsService>::DestroyInstance();
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
