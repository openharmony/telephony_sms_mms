/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "createsmsmessage_fuzzer.h"

#define private public

#include "addsmstoken_fuzzer.h"
#include "core_manager_inner.h"
#include "sms_service.h"

using namespace OHOS::Telephony;
namespace OHOS {
static bool g_isInited = false;
constexpr int32_t SLOT_NUM = 2;

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

void OnGetDefaultSmsSimId(const uint8_t *data, size_t size)
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

void SetDefaultSmsSlotId(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }

    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option(MessageOption::TF_SYNC);
    int32_t slotId = static_cast<int32_t>(size % SLOT_NUM);
    dataParcel.WriteInt32(slotId);
    dataParcel.WriteBuffer(data, size);
    dataParcel.RewindRead(0);
    DelayedSingleton<SmsService>::GetInstance()->OnSetDefaultSmsSlotId(dataParcel, replyParcel, option);
    return;
}

void CreateSmsMessage(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option(MessageOption::TF_SYNC);
    std::string pdu(reinterpret_cast<const char *>(data), size);
    dataParcel.WriteString(pdu);
    dataParcel.WriteString("3gpp");
    DelayedSingleton<SmsService>::GetInstance()->OnCreateMessage(dataParcel, replyParcel, option);
}

void CreateCbMessage(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }
    std::string strPdu(reinterpret_cast<const char *>(data), size);
    GsmCbCodec::CreateCbMessage(strPdu);
}

void SendMms(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option(MessageOption::TF_SYNC);
    int32_t slotId = static_cast<int32_t>(size % SLOT_NUM);
    std::string mmsc(reinterpret_cast<const char *>(data), size);
    auto mmscU16 = Str8ToStr16(mmsc);
    std::string mmsData(reinterpret_cast<const char *>(data), size);
    auto mmsDataU16 = Str8ToStr16(mmsData);
    std::string ua(reinterpret_cast<const char *>(data), size);
    auto uaU16 = Str8ToStr16(ua);
    std::string uaprof(reinterpret_cast<const char *>(data), size);
    auto uaprofU16 = Str8ToStr16(uaprof);
    dataParcel.WriteInt32(slotId);
    dataParcel.WriteString16(mmscU16);
    dataParcel.WriteString16(mmsDataU16);
    dataParcel.WriteString16(uaU16);
    dataParcel.WriteString16(uaprofU16);
    dataParcel.RewindRead(0);
    DelayedSingleton<SmsService>::GetInstance()->OnSendMms(dataParcel, replyParcel, option);
}

void DownloadMms(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option(MessageOption::TF_SYNC);
    int32_t slotId = static_cast<int32_t>(size % SLOT_NUM);
    std::string mmsc(reinterpret_cast<const char *>(data), size);
    auto mmscU16 = Str8ToStr16(mmsc);
    std::string mmsData(reinterpret_cast<const char *>(data), size);
    auto mmsDataU16 = Str8ToStr16(mmsData);
    std::string ua(reinterpret_cast<const char *>(data), size);
    auto uaU16 = Str8ToStr16(ua);
    std::string uaprof(reinterpret_cast<const char *>(data), size);
    auto uaprofU16 = Str8ToStr16(uaprof);
    dataParcel.WriteInt32(slotId);
    dataParcel.WriteString16(mmscU16);
    dataParcel.WriteString16(mmsDataU16);
    dataParcel.WriteString16(uaU16);
    dataParcel.WriteString16(uaprofU16);
    dataParcel.RewindRead(0);
    DelayedSingleton<SmsService>::GetInstance()->OnDownloadMms(dataParcel, replyParcel, option);
}

void DoCreateMessageMyAPI(const uint8_t *data, size_t size)
{
    if (data == nullptr || size == 0) {
        return;
    }
    GetDefaultSmsSlotId(data, size);
    OnGetDefaultSmsSimId(data, size);
    SetDefaultSmsSlotId(data, size);
    CreateSmsMessage(data, size);
    CreateCbMessage(data, size);
    SendMms(data, size);
    DownloadMms(data, size);
    DelayedSingleton<SmsService>::DestroyInstance();
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    /* Run your code on data */
    OHOS::AddSmsTokenFuzzer token;
    OHOS::DoCreateMessageMyAPI(data, size);
    return 0;
}
