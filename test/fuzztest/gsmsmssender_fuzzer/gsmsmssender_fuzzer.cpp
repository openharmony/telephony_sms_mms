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

#include "gsmsmssender_fuzzer.h"

#define private public
#define protected public

#include "addsmstoken_fuzzer.h"
#include "core_manager_inner.h"
#include "delivery_short_message_callback_stub.h"
#include "i_sms_service_interface.h"
#include "send_short_message_callback_stub.h"
#include "sms_service.h"

using namespace OHOS::Telephony;
namespace OHOS {
static bool g_isInited = false;
constexpr int32_t SLOT_NUM = 2;
static int32_t STATUS_COUNT = 4;
constexpr int32_t TYPE_NUM = 6;
constexpr int32_t SLEEP_TIME_SECONDS = 2;

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

    dataParcel.WriteBuffer(data, size);
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
    auto status = static_cast<ISmsServiceInterface::SimMessageStatus>(size % STATUS_COUNT);

    dataParcel.WriteInt32(slotId);
    dataParcel.WriteString16(smscU16);
    dataParcel.WriteString16(pduU16);
    dataParcel.WriteUint32(status);
    dataParcel.RewindRead(0);
    DelayedSingleton<SmsService>::GetInstance()->OnAddSimMessage(dataParcel, replyParcel, option);

    std::shared_ptr<SmsInterfaceManager> interfaceManager = std::make_shared<SmsInterfaceManager>(slotId);
    if (interfaceManager == nullptr) {
        TELEPHONY_LOGE("interfaceManager nullptr");
        return;
    }
    interfaceManager->AddSimMessage(smsc, pdu, status);

    std::shared_ptr<SmsMiscManager> smsMiscManager = std::make_shared<SmsMiscManager>(slotId);
    if (smsMiscManager == nullptr) {
        TELEPHONY_LOGE("smsMiscManager nullptr");
        return;
    }
    smsMiscManager->AddSimMessage(smsc, pdu, status);
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

    int32_t slotId = static_cast<int32_t>(size % SLOT_NUM);
    std::shared_ptr<SmsInterfaceManager> interfaceManager = std::make_shared<SmsInterfaceManager>(slotId);
    if (interfaceManager == nullptr) {
        TELEPHONY_LOGE("interfaceManager nullptr error");
        return;
    }
    interfaceManager->HasSmsCapability();
}

void SendSmsTest(const uint8_t *data, size_t size)
{
    std::function<void(std::shared_ptr<SmsSendIndexer>)> fun = nullptr;
    int32_t slotId = static_cast<int32_t>(size % SLOT_NUM);
    auto sender = std::make_shared<GsmSmsSender>(slotId, fun);
    sender->Init();

    std::string desAddr(reinterpret_cast<const char *>(data), size);
    std::string scAddr(reinterpret_cast<const char *>(data), size);
    std::string text(reinterpret_cast<const char *>(data), size);
    const sptr<ISendShortMessageCallback> sendCallback =
        iface_cast<ISendShortMessageCallback>(new SendShortMessageCallbackStub());
    const sptr<IDeliveryShortMessageCallback> deliveryCallback =
        iface_cast<IDeliveryShortMessageCallback>(new DeliveryShortMessageCallbackStub());
    sender->TextBasedSmsDelivery(desAddr, scAddr, text, sendCallback, deliveryCallback);
    sender->DataBasedSmsDelivery(desAddr, scAddr, size, data, size, sendCallback, deliveryCallback);

    std::vector<struct SplitInfo> cellsInfos;
    struct SplitInfo cellInfo;
    cellInfo.langId = static_cast<MSG_LANGUAGE_ID_T>(data[0]);
    cellInfo.encodeType = static_cast<DataCodingScheme>(data[0] % TYPE_NUM);
    cellInfo.encodeData.push_back(data[0]);
    cellsInfos.push_back(cellInfo);
    DataCodingScheme codingType = static_cast<DataCodingScheme>(data[0] % TYPE_NUM);
    bool isStatusReport = (size % SLOT_NUM == 1);
    GsmSmsMessage msg;
    std::shared_ptr<struct SmsTpdu> tpdu =
        msg.CreateDefaultSubmitSmsTpdu(desAddr, scAddr, text, isStatusReport, codingType);
    sender->TextBasedSmsSplitDelivery(
        text, text, cellsInfos, codingType, isStatusReport, tpdu, msg, sendCallback, deliveryCallback);
    sender->SendCallbackExceptionCase(sendCallback, text);

    std::shared_ptr<SmsSendIndexer> smsIndexer =
        std::make_shared<SmsSendIndexer>(desAddr, scAddr, text, sendCallback, deliveryCallback);
    sender->SendSmsToRil(smsIndexer);
    sender->ResendTextDelivery(smsIndexer);
    sender->ResendDataDelivery(smsIndexer);
    bool isMore = (size % SLOT_NUM == 0);
    auto encodeInfo = msg.GetSubmitEncodeInfo(text, isMore);
    sender->SetSendIndexerInfo(smsIndexer, encodeInfo, 1);
    sender->ResendTextDelivery(smsIndexer);
    sender->voiceServiceState_ = static_cast<int32_t>(size);
    sender->imsSmsCfg_ = static_cast<int32_t>(size);
    sender->SendSmsToRil(smsIndexer);
    sender->SetPduInfo(smsIndexer, msg, isMore);
}

void SendSmsTest2(const uint8_t *data, size_t size)
{
    std::function<void(std::shared_ptr<SmsSendIndexer>)> fun = nullptr;
    int32_t slotId = static_cast<int32_t>(size % SLOT_NUM);
    auto sender = std::make_shared<GsmSmsSender>(slotId, fun);
    sender->Init();
    sender->RegisterSatelliteCallback();
    sender->UnregisterSatelliteCallback();
    std::string desAddr(reinterpret_cast<const char *>(data), size);
    std::string scAddr(reinterpret_cast<const char *>(data), size);
    std::string text(reinterpret_cast<const char *>(data), size);
    const sptr<ISendShortMessageCallback> sendCallback =
        iface_cast<ISendShortMessageCallback>(new SendShortMessageCallbackStub());
    const sptr<IDeliveryShortMessageCallback> deliveryCallback =
        iface_cast<IDeliveryShortMessageCallback>(new DeliveryShortMessageCallbackStub());
    GsmSimMessageParam smsData;
    std::string pdu(reinterpret_cast<const char *>(data), size);
    smsData.refId = static_cast<int64_t>(size);
    smsData.smscPdu = pdu;
    smsData.pdu = pdu;
    std::shared_ptr<SmsSendIndexer> smsIndexer =
        std::make_shared<SmsSendIndexer>(desAddr, scAddr, text, sendCallback, deliveryCallback);
    sender->SendCsSms(smsIndexer, smsData);
    sender->SendSatelliteSms(smsIndexer, smsData);
    sender->SendImsSms(smsIndexer, smsData);
    bool isSupported;
    sender->IsImsSmsSupported(slotId, isSupported);
    InnerEvent::Pointer event = InnerEvent::Get(static_cast<int32_t>(size));
    sender->StatusReportAnalysis(event);
    event = InnerEvent::Get(static_cast<int32_t>(size));
    sender->StatusReportGetImsSms(event);
}

void DoSomethingInterestingWithMyAPI(const uint8_t *data, size_t size)
{
    if (data == nullptr || size == 0) {
        return;
    }

    OnRemoteRequest(data, size);
    AddSimMessage(data, size);
    HasSmsCapability(data, size);
    SendSmsTest(data, size);
    SendSmsTest2(data, size);
    DelayedSingleton<ImsSmsClient>::GetInstance()->UnInit();
    DelayedSingleton<ImsSmsClient>::DestroyInstance();
    sleep(SLEEP_TIME_SECONDS);
    DelayedSingleton<SmsService>::DestroyInstance();
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
