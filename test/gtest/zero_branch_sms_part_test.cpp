/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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

#define private public
#define protected public

#include "delivery_short_message_callback_stub.h"
#include "gtest/gtest.h"
#include "send_short_message_callback_stub.h"
#include "sms_misc_manager.h"
#include "sms_mms_gtest.h"
#include "sms_service.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace Telephony {
using namespace testing::ext;

namespace {
const std::string TEST_SOURCE_DATA = "srcData";
const std::string SMS_ADDR_ONE = "";
const std::string SMS_ADDR_TWO = " ";
const std::string SMS_ADDR_THREE = " 123 ";
const std::string SMS_ADDR_FOUR = "1 23";
const std::string SMS_ADDR_FIVE = " a 123 ";
const std::string SEND_MMS_FILE_URL = "1,2,";
const std::string VNET_MMSC = "http://mmsc.vnet.mobi";
} // namespace

class BranchSmsPartTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};
void BranchSmsPartTest::SetUpTestCase() {}

void BranchSmsPartTest::TearDownTestCase() {}

void BranchSmsPartTest::SetUp() {}

void BranchSmsPartTest::TearDown() {}

/**
 * @tc.number   Telephony_SmsMmsGtest_SmsInterfaceStub_0001
 * @tc.name     Test SmsInterfaceStub
 * @tc.desc     Function test
 */
HWTEST_F(BranchSmsPartTest, SmsInterfaceStub_0001, Function | MediumTest | Level1)
{
    AccessMmsToken token;
    int32_t slotId = 0;
    MessageParcel dataParcel;
    dataParcel.WriteInt32(slotId);
    dataParcel.WriteBuffer(TEST_SOURCE_DATA.c_str(), TEST_SOURCE_DATA.size());
    dataParcel.RewindRead(0);
    MessageParcel replyParcel;
    uint32_t code = 0;
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

    MessageParcel sendTextParcel;
    sendTextParcel.WriteInt32(0);
    sendTextParcel.WriteString16(Str8ToStr16(TEST_SOURCE_DATA));
    sendTextParcel.WriteString16(Str8ToStr16(TEST_SOURCE_DATA));
    sendTextParcel.WriteString16(Str8ToStr16(TEST_SOURCE_DATA));
    sendTextParcel.WriteString(TEST_SOURCE_DATA);
    DelayedSingleton<SmsService>::GetInstance()->OnSendSmsTextRequest(sendTextParcel, replyParcel, option);
    sendTextParcel.WriteInt32(-1);
    DelayedSingleton<SmsService>::GetInstance()->OnSendSmsTextRequest(sendTextParcel, replyParcel, option);
    int32_t result = replyParcel.ReadInt32();
    EXPECT_GE(result, 0);

    MessageParcel sendDataParcel;
    dataParcel.WriteInt32(0);
    dataParcel.WriteString16(Str8ToStr16(TEST_SOURCE_DATA));
    dataParcel.WriteString16(Str8ToStr16(TEST_SOURCE_DATA));
    dataParcel.WriteInt16(0);
    dataParcel.WriteInt16(TEST_SOURCE_DATA.size());
    dataParcel.WriteRawData(TEST_SOURCE_DATA.c_str(), TEST_SOURCE_DATA.size());
    DelayedSingleton<SmsService>::GetInstance()->OnSendSmsDataRequest(dataParcel, replyParcel, option);
    sendTextParcel.WriteInt32(-1);
    DelayedSingleton<SmsService>::GetInstance()->OnSendSmsDataRequest(dataParcel, replyParcel, option);
    result = replyParcel.ReadInt32();
    EXPECT_GE(result, 0);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_SmsInterfaceStub_0002
 * @tc.name     Test SmsInterfaceStub
 * @tc.desc     Function test
 */
HWTEST_F(BranchSmsPartTest, SmsInterfaceStub_0002, Function | MediumTest | Level1)
{
    std::u16string desAddr = Str8ToStr16(SMS_ADDR_ONE);
    DelayedSingleton<SmsService>::GetInstance()->RemoveSpacesInDesAddr(desAddr);
    EXPECT_GE(desAddr.size(), 0);
    desAddr = Str8ToStr16(SMS_ADDR_TWO);
    DelayedSingleton<SmsService>::GetInstance()->RemoveSpacesInDesAddr(desAddr);
    EXPECT_GE(desAddr.size(), 0);
    desAddr = Str8ToStr16(SMS_ADDR_THREE);
    DelayedSingleton<SmsService>::GetInstance()->RemoveSpacesInDesAddr(desAddr);
    EXPECT_GE(desAddr.size(), 0);
    desAddr = Str8ToStr16(SMS_ADDR_FOUR);
    DelayedSingleton<SmsService>::GetInstance()->RemoveSpacesInDesAddr(desAddr);
    EXPECT_GE(desAddr.size(), 0);
    desAddr = Str8ToStr16(SMS_ADDR_FIVE);
    DelayedSingleton<SmsService>::GetInstance()->RemoveSpacesInDesAddr(desAddr);
    EXPECT_GE(desAddr.size(), 0);

    MessageParcel setSmscParcel;
    MessageParcel replyParcel;
    MessageOption option(MessageOption::TF_SYNC);
    setSmscParcel.WriteInt32(0);
    setSmscParcel.WriteString16(Str8ToStr16(TEST_SOURCE_DATA));
    DelayedSingleton<SmsService>::GetInstance()->OnSetSmscAddr(setSmscParcel, replyParcel, option);
    MessageParcel getSmscParcel;
    getSmscParcel.WriteInt32(0);
    DelayedSingleton<SmsService>::GetInstance()->OnGetSmscAddr(getSmscParcel, replyParcel, option);
    DelayedSingleton<SmsService>::GetInstance()->OnGetAllSimMessages(getSmscParcel, replyParcel, option);
    DelayedSingleton<SmsService>::GetInstance()->OnSetDefaultSmsSlotId(getSmscParcel, replyParcel, option);
    DelayedSingleton<SmsService>::GetInstance()->OnGetDefaultSmsSlotId(getSmscParcel, replyParcel, option);
    int32_t result = replyParcel.ReadInt32();
    EXPECT_GE(result, 0);

    MessageParcel addSimMsgParcel;
    addSimMsgParcel.WriteInt32(0);
    addSimMsgParcel.WriteString16(Str8ToStr16(TEST_SOURCE_DATA));
    addSimMsgParcel.WriteString16(Str8ToStr16(TEST_SOURCE_DATA));
    addSimMsgParcel.WriteUint32(0);
    DelayedSingleton<SmsService>::GetInstance()->OnAddSimMessage(addSimMsgParcel, replyParcel, option);
    result = replyParcel.ReadInt32();
    EXPECT_GE(result, 0);

    MessageParcel delSimMsgParcel;
    delSimMsgParcel.WriteInt32(0);
    delSimMsgParcel.WriteUint32(0);
    DelayedSingleton<SmsService>::GetInstance()->OnDelSimMessage(delSimMsgParcel, replyParcel, option);
    result = replyParcel.ReadInt32();
    EXPECT_GE(result, 0);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_SmsInterfaceStub_0003
 * @tc.name     Test SmsInterfaceStub
 * @tc.desc     Function test
 */
HWTEST_F(BranchSmsPartTest, SmsInterfaceStub_0003, Function | MediumTest | Level1)
{
    MessageParcel simMsgParcel;
    MessageParcel replyParcel;
    MessageOption option(MessageOption::TF_SYNC);
    simMsgParcel.WriteInt32(0);
    simMsgParcel.WriteUint32(0);
    simMsgParcel.WriteUint32(0);
    simMsgParcel.WriteString16(Str8ToStr16(TEST_SOURCE_DATA));
    simMsgParcel.WriteString16(Str8ToStr16(TEST_SOURCE_DATA));
    DelayedSingleton<SmsService>::GetInstance()->OnUpdateSimMessage(simMsgParcel, replyParcel, option);
    int32_t result = replyParcel.ReadInt32();
    EXPECT_GE(result, 0);

    MessageParcel SetImsSmsParcel;
    SetImsSmsParcel.WriteInt32(0);
    SetImsSmsParcel.WriteInt32(false);
    DelayedSingleton<SmsService>::GetInstance()->OnSetImsSmsConfig(SetImsSmsParcel, replyParcel, option);
    result = replyParcel.ReadInt32();
    EXPECT_GE(result, 0);

    MessageParcel splitMsgParcel;
    splitMsgParcel.WriteString16(Str8ToStr16(TEST_SOURCE_DATA));
    DelayedSingleton<SmsService>::GetInstance()->OnSplitMessage(splitMsgParcel, replyParcel, option);
    DelayedSingleton<SmsService>::GetInstance()->OnGetBase64Encode(splitMsgParcel, replyParcel, option);
    DelayedSingleton<SmsService>::GetInstance()->OnGetBase64Decode(splitMsgParcel, replyParcel, option);
    result = replyParcel.ReadInt32();
    EXPECT_GE(result, 0);

    MessageParcel createMsgParcel;
    createMsgParcel.WriteString(TEST_SOURCE_DATA);
    createMsgParcel.WriteString(TEST_SOURCE_DATA);
    DelayedSingleton<SmsService>::GetInstance()->OnCreateMessage(createMsgParcel, replyParcel, option);
    result = replyParcel.ReadInt32();
    EXPECT_GE(result, 0);

    MessageParcel segmentParcel;
    segmentParcel.WriteInt32(0);
    segmentParcel.WriteString16(Str8ToStr16(TEST_SOURCE_DATA));
    segmentParcel.WriteBool(false);
    DelayedSingleton<SmsService>::GetInstance()->OnGetSmsSegmentsInfo(segmentParcel, replyParcel, option);
    result = replyParcel.ReadInt32();
    EXPECT_GE(result, 0);

    MessageParcel formatParcel;
    DelayedSingleton<SmsService>::GetInstance()->OnGetImsShortMessageFormat(formatParcel, replyParcel, option);
    result = replyParcel.ReadInt32();
    DelayedSingleton<SmsService>::GetInstance()->OnGetDefaultSmsSimId(formatParcel, replyParcel, option);
    result = replyParcel.ReadInt32();
    EXPECT_GE(result, 0);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_SmsInterfaceManager_0001
 * @tc.name     Test SmsInterfaceManager
 * @tc.desc     Function test
 */
HWTEST_F(BranchSmsPartTest, SmsInterfaceManager_0001, Function | MediumTest | Level1)
{
    auto interfaceManager = std::make_shared<SmsInterfaceManager>(0);
    if (interfaceManager == nullptr) {
        return;
    }
    interfaceManager->HasSmsCapability();
    std::string dataStr = TEST_SOURCE_DATA;
    auto callStub = iface_cast<ISendShortMessageCallback>(new SendShortMessageCallbackStub());
    auto deliveryStub = iface_cast<IDeliveryShortMessageCallback>(new DeliveryShortMessageCallbackStub());
    std::string emptyStr = "";

    interfaceManager->TextBasedSmsDelivery(dataStr, dataStr, dataStr, callStub, deliveryStub);
    interfaceManager->TextBasedSmsDelivery(dataStr, emptyStr, dataStr, callStub, deliveryStub);
    interfaceManager->TextBasedSmsDelivery(dataStr, dataStr, emptyStr, callStub, deliveryStub);
    int32_t result = interfaceManager->TextBasedSmsDelivery(dataStr, emptyStr, emptyStr, callStub, deliveryStub);
    EXPECT_GE(result, 0);

    const uint8_t *data = reinterpret_cast<const uint8_t *>(dataStr.c_str());
    interfaceManager->DataBasedSmsDelivery(dataStr, dataStr, 0, data, dataStr.size(), callStub, deliveryStub);
    interfaceManager->DataBasedSmsDelivery(dataStr, emptyStr, 0, data, dataStr.size(), callStub, deliveryStub);
    interfaceManager->DataBasedSmsDelivery(dataStr, dataStr, 0, nullptr, 0, callStub, deliveryStub);
    interfaceManager->smsSendManager_ = nullptr;
    interfaceManager->TextBasedSmsDelivery(dataStr, dataStr, dataStr, callStub, deliveryStub);
    result = interfaceManager->DataBasedSmsDelivery(dataStr, dataStr, 0, data, dataStr.size(), callStub, deliveryStub);
    EXPECT_GE(result, 0);

    interfaceManager->AddSimMessage(dataStr, dataStr, ISmsServiceInterface::SIM_MESSAGE_STATUS_UNREAD);
    interfaceManager->UpdateSimMessage(0, ISmsServiceInterface::SIM_MESSAGE_STATUS_UNREAD, dataStr, dataStr);
    std::vector<ShortMessage> message;
    interfaceManager->GetAllSimMessages(message);
    result = interfaceManager->SetSmscAddr(dataStr);
    EXPECT_GE(result, 0);

    std::u16string smsc;
    interfaceManager->GetSmscAddr(smsc);
    interfaceManager->SetCBConfig(false, 0, 1, 0);
    result = interfaceManager->SetImsSmsConfig(0, false);
    EXPECT_GE(result, 0);

    interfaceManager->SetDefaultSmsSlotId(0);
    interfaceManager->GetDefaultSmsSlotId();
    int32_t simId;
    interfaceManager->GetDefaultSmsSimId(simId);
    std::vector<std::u16string> splitMessage;
    result = interfaceManager->SplitMessage(dataStr, splitMessage);
    EXPECT_GE(result, 0);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_SmsInterfaceManager_0002
 * @tc.name     Test SmsInterfaceManager
 * @tc.desc     Function test
 */
HWTEST_F(BranchSmsPartTest, SmsInterfaceManager_0002, Function | MediumTest | Level1)
{
    auto interfaceManager = std::make_shared<SmsInterfaceManager>(0);
    if (interfaceManager == nullptr) {
        return;
    }
    LengthInfo outInfo;
    std::string dataStr = TEST_SOURCE_DATA;
    interfaceManager->GetSmsSegmentsInfo(dataStr, false, outInfo);
    bool isSupported = false;
    interfaceManager->IsImsSmsSupported(0, isSupported);
    std::u16string format;
    interfaceManager->GetImsShortMessageFormat(format);
    int32_t result = interfaceManager->HasSmsCapability();
    EXPECT_GE(result, 0);

    std::u16string mmsc = u"";
    std::u16string data = u"";
    std::u16string ua = u"";
    std::u16string uaprof = u"";
    interfaceManager->SendMms(mmsc, data, ua, uaprof);
    result = interfaceManager->DownloadMms(mmsc, data, ua, uaprof);
    EXPECT_GE(result, 0);

    interfaceManager->smsMiscManager_ = nullptr;
    interfaceManager->AddSimMessage(dataStr, dataStr, ISmsServiceInterface::SIM_MESSAGE_STATUS_UNREAD);
    interfaceManager->UpdateSimMessage(0, ISmsServiceInterface::SIM_MESSAGE_STATUS_UNREAD, dataStr, dataStr);
    std::vector<ShortMessage> message;
    interfaceManager->GetAllSimMessages(message);
    interfaceManager->SetSmscAddr(dataStr);
    std::u16string smsc;
    result = interfaceManager->GetSmscAddr(mmsc);
    EXPECT_GE(result, 0);

    interfaceManager->SetCBConfig(false, 0, 1, 0);
    interfaceManager->SetImsSmsConfig(0, false);
    interfaceManager->SetDefaultSmsSlotId(0);
    interfaceManager->GetDefaultSmsSlotId();
    int32_t simId;
    result = interfaceManager->GetDefaultSmsSimId(simId);
    EXPECT_GE(result, 0);

    std::vector<std::u16string> splitMessage;
    interfaceManager->SplitMessage(dataStr, splitMessage);
    interfaceManager->GetSmsSegmentsInfo(dataStr, false, outInfo);
    result = interfaceManager->IsImsSmsSupported(0, isSupported);
    EXPECT_GE(result, 0);

    interfaceManager->GetImsShortMessageFormat(format);
    interfaceManager->mmsSendManager_ = nullptr;
    interfaceManager->SendMms(mmsc, data, ua, uaprof);
    interfaceManager->mmsReceiverManager_ = nullptr;
    result = interfaceManager->DownloadMms(mmsc, data, ua, uaprof);
    EXPECT_GE(result, 0);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_SmsInterfaceManager_0003
 * @tc.name     Test SmsInterfaceManager
 * @tc.desc     Function test
 */
HWTEST_F(BranchSmsPartTest, SmsInterfaceManager_0003, Function | MediumTest | Level1)
{
    int32_t slotId = 0;
    std::shared_ptr<SmsInterfaceManager> interfaceManager = std::make_shared<SmsInterfaceManager>(slotId);
    interfaceManager->mmsSendManager_ = nullptr;
    std::u16string mmsc(StringUtils::ToUtf16(VNET_MMSC));
    std::u16string data(StringUtils::ToUtf16(SEND_MMS_FILE_URL));
    std::u16string ua = u"112233";
    std::u16string uaprof = u"112233";
    int32_t sendMmsRet = interfaceManager->SendMms(mmsc, data, ua, uaprof);
    interfaceManager->mmsReceiverManager_ = nullptr;
    int32_t downloadMmsRet = interfaceManager->DownloadMms(mmsc, data, ua, uaprof);
    EXPECT_GE(sendMmsRet, 0);
    EXPECT_GE(downloadMmsRet, 0);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_SmsStateObserver_0001
 * @tc.name     Test SmsStateObserver_0001
 * @tc.desc     Function test
 */
HWTEST_F(BranchSmsPartTest, SmsStateObserver_0001, Function | MediumTest | Level1)
{
    std::shared_ptr<SmsStateObserver> smsStateObserver = std::make_shared<SmsStateObserver>();
    smsStateObserver->smsSubscriber_ = nullptr;
    smsStateObserver->StopEventSubscriber();
    std::shared_ptr<CommonEventSubscribeInfo> subscribeInfo = std::make_shared<CommonEventSubscribeInfo>();
    std::shared_ptr<SmsStateEventSubscriber> smsStateEventSubscriber =
        std::make_shared<SmsStateEventSubscriber>(*subscribeInfo);
    EventFwk::CommonEventData eventData = EventFwk::CommonEventData();
    AAFwk::Want want = AAFwk::Want();
    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_SMS_EMERGENCY_CB_RECEIVE_COMPLETED);
    eventData.SetWant(want);
    smsStateEventSubscriber->OnReceiveEvent(eventData);
    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_SMS_RECEIVE_COMPLETED);
    eventData.SetWant(want);
    smsStateEventSubscriber->OnReceiveEvent(eventData);
    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_SMS_WAPPUSH_RECEIVE_COMPLETED);
    eventData.SetWant(want);
    smsStateEventSubscriber->OnReceiveEvent(eventData);
    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_BATTERY_LOW);
    eventData.SetWant(want);
    smsStateEventSubscriber->OnReceiveEvent(eventData);
    EXPECT_TRUE(smsStateObserver != nullptr);
    EXPECT_TRUE(subscribeInfo != nullptr);
    EXPECT_TRUE(smsStateEventSubscriber != nullptr);
}
} // namespace Telephony
} // namespace OHOS
