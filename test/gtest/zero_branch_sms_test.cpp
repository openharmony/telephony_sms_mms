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

#include "cdma_sms_message.h"
#include "delivery_short_message_callback_stub.h"
#include "gsm_sms_message.h"
#include "gsm_sms_param_decode.h"
#include "gsm_sms_receive_handler.h"
#include "gsm_sms_tpdu_decode.h"
#include "gsm_sms_tpdu_encode.h"
#include "gsm_user_data_decode.h"
#include "gsm_user_data_encode.h"
#include "gtest/gtest.h"
#include "radio_event.h"
#include "satellite_sms_client.h"
#include "send_short_message_callback_stub.h"
#include "short_message.h"
#include "sms_common_utils.h"
#include "sms_misc_manager.h"
#include "sms_mms_gtest.h"
#include "sms_pdu_buffer.h"
#include "sms_receive_handler.h"
#include "sms_receive_reliability_handler.h"
#include "sms_send_manager.h"
#include "sms_sender.h"
#include "sms_service.h"
#include "sms_service_manager_client.h"
#include "sms_state_handler.h"
#include "telephony_errors.h"

namespace OHOS {
namespace Telephony {
using namespace testing::ext;

namespace {
const std::string TEXT_SMS_CONTENT = "hello world";
const std::u16string INTERFACE_TOKEN = u"ohos.telephony.ISatelliteSmsCallback";
const std::string BLOCK_NUMBER = "123";
const int8_t TEXT_PORT_NUM = -1;
const int16_t WAP_PUSH_PORT = 2948;
const uint16_t BUF_SIZE = 2401;
const uint8_t BUFFER_SIZE = 255;
const int DIGIT_LEN = 3;
const int START_BIT = 4;
const int32_t INVALID_SLOTID = 2;
const int32_t VALUE_LENGTH = 2;
const int32_t HEADER_LENGTH = 7;
const uint32_t CODE_BUFFER_MAX_SIZE = 300 * 1024;
const unsigned int SMS_REF_ID = 10;
static constexpr uint16_t MAX_TPDU_DATA_LEN = 255;
const std::string CB_RANGE_MIDS = "0-1,2-3";
const std::string CB_RANGE_DCSS = "0-255";
const std::string CB_RANGE_DELI = ",";
const std::string CB_RANGE_MID = "0-1";
const std::string CB_RANGE_DELIM = "-";
static constexpr const char *SLOT_ID = "slot_id";
const std::string TABLE_URL = "datashare:///com.ohos.smsmmsability/sms_mms/sms_subsection";
const std::string SMS_READ_PDU = "123456";
const size_t DECODE_SIZE = 10;
const int EIGHT_BIT = 8;
const int FOUR_BIT = 4;
const int FIVE_BIT = 5;
const int SIX_BIT = 6;
const int TWO_BIT = 2;
const std::string CBN_NUM = "+86192********";
const int NUM_LENGTH = 11;
const std::string PREFIX = "+86";
const std::vector<uint8_t> PDU = { 0 };
} // namespace

class BranchSmsTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};
void BranchSmsTest::SetUpTestCase() {}

void BranchSmsTest::TearDownTestCase() {}

void BranchSmsTest::SetUp() {}

void BranchSmsTest::TearDown() {}

/**
 * @tc.number   Telephony_SmsMmsGtest_SmsReceiveHandler_0001
 * @tc.name     Test SmsReceiveHandler
 * @tc.desc     Function test
 */
HWTEST_F(BranchSmsTest, SmsReceiveHandler_0001, Function | MediumTest | Level1)
{
    AppExecFwk::InnerEvent::Pointer event = AppExecFwk::InnerEvent::Get(RadioEvent::RADIO_GSM_SMS, 1);
    std::shared_ptr<SmsReceiveHandler> smsReceiveHandler = std::make_shared<CdmaSmsReceiveHandler>(INVALID_SLOTID);
    smsReceiveHandler->ProcessEvent(event);
    event = AppExecFwk::InnerEvent::Get(RadioEvent::RADIO_CDMA_SMS, 1);
    smsReceiveHandler->ProcessEvent(event);
    event = AppExecFwk::InnerEvent::Get(RadioEvent::RADIO_CONNECTED, 1);
    smsReceiveHandler->ProcessEvent(event);
    event = nullptr;
    smsReceiveHandler->ProcessEvent(event);
    std::shared_ptr<SmsReceiveIndexer> indexer = nullptr;
    std::shared_ptr<SmsBaseMessage> smsBaseMessage = nullptr;
    std::shared_ptr<vector<string>> pdus = nullptr;
    auto reliabilityHandler = std::make_shared<SmsReceiveReliabilityHandler>(INVALID_SLOTID);
    reliabilityHandler->DeleteMessageFormDb(SMS_REF_ID);
    smsReceiveHandler->CombineMessagePart(indexer);

    reliabilityHandler->CheckBlockedPhoneNumber(BLOCK_NUMBER);
    reliabilityHandler->DeleteAutoSmsFromDB(reliabilityHandler, 0, 0);
    reliabilityHandler->SendBroadcast(indexer, pdus);
    smsReceiveHandler->HandleReceivedSms(smsBaseMessage);
    indexer = std::make_shared<SmsReceiveIndexer>();
    smsReceiveHandler->CombineMessagePart(indexer);
    indexer->msgCount_ = 1;
    indexer->destPort_ = WAP_PUSH_PORT;
    smsReceiveHandler->CombineMessagePart(indexer);
    reliabilityHandler->SendBroadcast(indexer, pdus);
    pdus = std::make_shared<vector<string>>();
    string pud = "qwe";
    pdus->push_back(pud);
    reliabilityHandler->SendBroadcast(indexer, pdus);
    smsReceiveHandler->CombineMultiPageMessage(indexer, pdus, reliabilityHandler);
    smsReceiveHandler->UpdateMultiPageMessage(indexer, pdus);
    indexer->destPort_ = TEXT_PORT_NUM;
    reliabilityHandler->SendBroadcast(indexer, pdus);
    smsReceiveHandler->AddMsgToDB(indexer);
    smsReceiveHandler->IsRepeatedMessagePart(indexer);
    indexer = nullptr;
    EXPECT_FALSE(smsReceiveHandler->AddMsgToDB(indexer));
}

/**
 * @tc.number   Telephony_SmsMmsGtest_SmsReceiveReliabilityHandler_0001
 * @tc.name     Test SmsReceiveReliabilityHandler
 * @tc.desc     Function test
 */
HWTEST_F(BranchSmsTest, SmsReceiveReliabilityHandler_0001, Function | MediumTest | Level1)
{
    auto reliabilityHandler = std::make_shared<SmsReceiveReliabilityHandler>(INVALID_SLOTID);
    reliabilityHandler->DeleteMessageFormDb(SMS_REF_ID);

    std::vector<SmsReceiveIndexer> dbIndexers;
    std::string strData = "qwe";
    auto indexer = SmsReceiveIndexer(StringUtils::HexToByteVector(strData), strData.size(), strData.size(), false,
        strData, strData, strData.size(), strData.size(), strData.size(), false, strData);

    dbIndexers.push_back(indexer);
    indexer = SmsReceiveIndexer(
        StringUtils::HexToByteVector(strData), strData.size(), strData.size(), false, false, strData, strData, strData);
    dbIndexers.push_back(indexer);
    reliabilityHandler->CheckUnReceiveWapPush(dbIndexers);

    std::shared_ptr<std::vector<std::string>> userDataRaws = std::make_shared<std::vector<std::string>>();
    userDataRaws->push_back(strData);

    int32_t pages = 0;
    reliabilityHandler->GetWapPushUserDataSinglePage(indexer, userDataRaws);
    int32_t count;
    reliabilityHandler->GetWapPushUserDataMultipage(count, dbIndexers, 0, userDataRaws);
    reliabilityHandler->GetWapPushUserDataMultipage(count, dbIndexers, VALUE_LENGTH, userDataRaws);
    reliabilityHandler->GetWapPushUserDataMultipage(count, dbIndexers, VALUE_LENGTH + 1, userDataRaws);
    reliabilityHandler->ReadyDecodeWapPushUserData(indexer, userDataRaws);
    reliabilityHandler->GetSmsUserDataMultipage(pages, dbIndexers, 0, userDataRaws);
    reliabilityHandler->ReadySendSmsBroadcast(indexer, userDataRaws);
    reliabilityHandler->DeleteMessageFormDb(strData.size(), strData.size());
    reliabilityHandler->RemoveBlockedSms(dbIndexers);

    std::shared_ptr<SmsReceiveIndexer> indexerPtr =
        std::make_shared<SmsReceiveIndexer>(StringUtils::HexToByteVector(strData), strData.size(), strData.size(),
            false, strData, strData, strData.size(), strData.size(), strData.size(), false, strData);
    if (indexerPtr == nullptr) {
        return;
    }
    reliabilityHandler->SendBroadcast(indexerPtr, userDataRaws);
    EXPECT_TRUE(reliabilityHandler->CheckSmsCapable());
    EXPECT_FALSE(reliabilityHandler->CheckBlockedPhoneNumber(BLOCK_NUMBER));
}

/**
 * @tc.number   Telephony_SmsMmsGtest_SmsStateHandler_0001
 * @tc.name     Test SmsStateHandler
 * @tc.desc     Function test
 */
HWTEST_F(BranchSmsTest, SmsStateHandler_0001, Function | MediumTest | Level1)
{
    SmsStateHandler handler;
    handler.Init();
    handler.UnInit();
    EXPECT_TRUE(handler.RegisterHandler());
    EXPECT_TRUE(handler.UnRegisterHandler());
}

/**
 * @tc.number   Telephony_SmsMmsGtest_CdmaSmsSender_0001
 * @tc.name     Test CdmaSmsSender
 * @tc.desc     Function test
 */
HWTEST_F(BranchSmsTest, CdmaSmsSender_0001, Function | MediumTest | Level1)
{
    std::function<void(std::shared_ptr<SmsSendIndexer>)> fun = nullptr;
    auto cdmaSmsSender = std::make_shared<CdmaSmsSender>(INVALID_SLOTID, fun);
    cdmaSmsSender->isImsCdmaHandlerRegistered = true;
    cdmaSmsSender->RegisterImsHandler();
    const sptr<ISendShortMessageCallback> sendCallback =
        iface_cast<ISendShortMessageCallback>(new SendShortMessageCallbackStub());
    if (sendCallback == nullptr) {
        return;
    }
    const sptr<IDeliveryShortMessageCallback> deliveryCallback =
        iface_cast<IDeliveryShortMessageCallback>(new DeliveryShortMessageCallbackStub());
    if (deliveryCallback == nullptr) {
        return;
    }
    const std::string text = "123";
    const std::string desAddr = "qwe";
    const std::string scAddr = "123";
    cdmaSmsSender->isImsNetDomain_ = true;
    cdmaSmsSender->TextBasedSmsDelivery(desAddr, scAddr, text, sendCallback, deliveryCallback);
    std::vector<struct SplitInfo> splits;
    std::unique_ptr<CdmaTransportMsg> transMsg;
    uint8_t msgRef8bit = 0;
    uint16_t msgId = 0;
    long timeStamp = 0;
    cdmaSmsSender->TextBasedSmsSplitDelivery(
        desAddr, scAddr, splits, std::move(transMsg), msgRef8bit, msgId, timeStamp, sendCallback, deliveryCallback);
    cdmaSmsSender->TextBasedSmsDeliveryViaIms(desAddr, scAddr, text, sendCallback, deliveryCallback);
    std::shared_ptr<SmsSendIndexer> smsIndexer = nullptr;
    cdmaSmsSender->SendSmsToRil(smsIndexer);
    cdmaSmsSender->ResendTextDelivery(smsIndexer);
    cdmaSmsSender->ResendDataDelivery(smsIndexer);
    smsIndexer = std::make_shared<SmsSendIndexer>(desAddr, scAddr, text, sendCallback, deliveryCallback);
    cdmaSmsSender->SendSmsToRil(smsIndexer);
    cdmaSmsSender->ResendTextDelivery(smsIndexer);
    cdmaSmsSender->ResendDataDelivery(smsIndexer);
    AppExecFwk::InnerEvent::Pointer event = AppExecFwk::InnerEvent::Get(0, 1);
    cdmaSmsSender->StatusReportGetImsSms(event);
    cdmaSmsSender->StatusReportAnalysis(event);
    event = nullptr;
    cdmaSmsSender->StatusReportSetImsSms(event);
    cdmaSmsSender->StatusReportGetImsSms(event);
    cdmaSmsSender->StatusReportAnalysis(event);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_ShortMessage_0001
 * @tc.name     Test ShortMessage
 * @tc.desc     Function test
 */
HWTEST_F(BranchSmsTest, ShortMessage_0001, Function | MediumTest | Level1)
{
    auto shortMessage = std::make_shared<ShortMessage>();
    std::vector<unsigned char> pdu;
    std::string str = "3gpp";
    Parcel parcel;
    std::u16string specification = u" ";
    ShortMessage ShortMessageObj;
    EXPECT_TRUE(shortMessage->CreateMessage(pdu, specification, ShortMessageObj) != TELEPHONY_ERR_SUCCESS);
    pdu.push_back(ShortMessage::SmsSimMessageStatus::SMS_SIM_MESSAGE_STATUS_READ);
    EXPECT_GE(shortMessage->CreateIccMessage(pdu, str, 1).indexOnSim_, 0);
    pdu.push_back(ShortMessage::SmsSimMessageStatus::SMS_SIM_MESSAGE_STATUS_FREE);
    EXPECT_GE(shortMessage->CreateIccMessage(pdu, str, 1).indexOnSim_, 0);
    pdu.clear();
    pdu.push_back(ShortMessage::SmsSimMessageStatus::SMS_SIM_MESSAGE_STATUS_UNREAD);
    pdu.push_back(ShortMessage::SmsSimMessageStatus::SMS_SIM_MESSAGE_STATUS_FREE);
    EXPECT_GE(shortMessage->CreateIccMessage(pdu, str, 1).indexOnSim_, 0);
    pdu.clear();
    pdu.push_back(ShortMessage::SmsSimMessageStatus::SMS_SIM_MESSAGE_STATUS_SENT);
    pdu.push_back(ShortMessage::SmsSimMessageStatus::SMS_SIM_MESSAGE_STATUS_FREE);
    EXPECT_GE(shortMessage->CreateIccMessage(pdu, str, 1).indexOnSim_, 0);
    pdu.clear();
    pdu.push_back(ShortMessage::SmsSimMessageStatus::SMS_SIM_MESSAGE_STATUS_UNSENT);
    pdu.push_back(ShortMessage::SmsSimMessageStatus::SMS_SIM_MESSAGE_STATUS_FREE);
    EXPECT_GE(shortMessage->CreateIccMessage(pdu, str, 1).indexOnSim_, 0);
    pdu.clear();
    pdu.push_back(ShortMessage::SmsSimMessageStatus::SMS_SIM_MESSAGE_STATUS_FREE);
    pdu.push_back(ShortMessage::SmsSimMessageStatus::SMS_SIM_MESSAGE_STATUS_UNSENT);
    EXPECT_EQ(shortMessage->CreateIccMessage(pdu, str, 1).simMessageStatus_,
        ShortMessage::SmsSimMessageStatus::SMS_SIM_MESSAGE_STATUS_FREE);
    EXPECT_FALSE(shortMessage->ReadFromParcel(parcel));
}

/**
 * @tc.number   Telephony_SmsMmsGtest_ShortMessage_0002
 * @tc.name     Test ShortMessage
 * @tc.desc     Function test
 */
HWTEST_F(BranchSmsTest, ShortMessage_0002, Function | MediumTest | Level1)
{
    auto shortMessage = DelayedSingleton<ShortMessage>::GetInstance();
    MessageParcel reply;
    int32_t result = 0;
    reply.WriteInt32(result);
    shortMessage->UnMarshalling(reply);
    shortMessage->GetVisibleMessageBody();
    shortMessage->GetVisibleRawAddress();
    shortMessage->GetMessageClass();
    std::u16string smscAddress = u"13677884499";
    shortMessage->GetScAddress(smscAddress);
    shortMessage->GetScTimestamp();
    shortMessage->IsReplaceMessage();
    shortMessage->GetStatus();
    shortMessage->IsSmsStatusReportMessage();
    shortMessage->HasReplyPath();
    shortMessage->GetIccMessageStatus();
    shortMessage->GetProtocolId();
    shortMessage->GetPdu();
    std::vector<unsigned char> pdus;
    unsigned char data = 255;
    pdus.push_back(data);
    std::u16string specification = u"";
    ShortMessage messageObj;
    shortMessage->CreateMessage(pdus, specification, messageObj);
    shortMessage->GetIndexOnSim();
    EXPECT_TRUE(shortMessage != nullptr);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_GsmSmsMessage_0001
 * @tc.name     Test GsmSmsMessage
 * @tc.desc     Function test
 */
HWTEST_F(BranchSmsTest, GsmSmsMessage_0001, Function | MediumTest | Level1)
{
    auto gsmSmsMessage = std::make_shared<GsmSmsMessage>();
    std::string dest = "";
    std::string str = "";
    std::string text = "";
    std::string msgText = "";
    unsigned char langId = 0;
    std::string desAddr = "";
    SmsTimeStamp times;
    SmsConcat concat;
    std::shared_ptr<struct SmsTpdu> tPdu = std::make_shared<struct SmsTpdu>();
    gsmSmsMessage->ConvertMsgTimeStamp(times);
    times.format = SmsTimeFormat::SMS_TIME_ABSOLUTE;
    gsmSmsMessage->ConvertMsgTimeStamp(times);
    EXPECT_EQ(gsmSmsMessage->SetHeaderLang(1, DataCodingScheme::DATA_CODING_UCS2, langId), 0);
    EXPECT_EQ(gsmSmsMessage->SetHeaderConcat(1, concat), 0);
    EXPECT_EQ(gsmSmsMessage->SetHeaderReply(1), 0);
    EXPECT_TRUE(gsmSmsMessage->CreateDefaultSubmitSmsTpdu(dest, str, text, true, DataCodingScheme::DATA_CODING_7BIT) !=
                nullptr);
    EXPECT_NE(gsmSmsMessage->GetDestPort(), -1);
    msgText = "123";
    desAddr = "+SetSmsTpduDestAddress";
    EXPECT_EQ(gsmSmsMessage->SetSmsTpduDestAddress(tPdu, desAddr), 22);
    desAddr = "SetSmsTpduDestAddress";
    EXPECT_EQ(gsmSmsMessage->SetSmsTpduDestAddress(tPdu, desAddr), 21);
    tPdu = nullptr;
    EXPECT_EQ(gsmSmsMessage->SetSmsTpduDestAddress(tPdu, desAddr), 0);
    langId = 1;
    EXPECT_EQ(gsmSmsMessage->SetHeaderLang(1, DataCodingScheme::DATA_CODING_7BIT, langId), 1);
    EXPECT_EQ(gsmSmsMessage->SetHeaderConcat(1, concat), 1);
    concat.is8Bits = true;
    EXPECT_EQ(gsmSmsMessage->SetHeaderConcat(1, concat), 1);
    gsmSmsMessage->replyAddress_ = "++SetSmsTpduDestAddress";
    EXPECT_EQ(gsmSmsMessage->SetHeaderReply(1), 0);
    EXPECT_TRUE(gsmSmsMessage->GetSubmitEncodeInfo(msgText, true) != nullptr);
    msgText = "++";
    EXPECT_TRUE(gsmSmsMessage->GetSubmitEncodeInfo(msgText, true) != nullptr);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_GsmSmsMessage_0002
 * @tc.name     Test GsmSmsMessage
 * @tc.desc     Function test
 */
HWTEST_F(BranchSmsTest, GsmSmsMessage_0002, Function | MediumTest | Level1)
{
    auto gsmSmsMessage = std::make_shared<GsmSmsMessage>();
    string pdu = "";
    gsmSmsMessage->ConvertMessageDcs();
    gsmSmsMessage->ConvertUserData();
    EXPECT_TRUE(gsmSmsMessage->CreateMessage(pdu) == nullptr);
    EXPECT_FALSE(gsmSmsMessage->PduAnalysis(pdu));
    EXPECT_FALSE(gsmSmsMessage->PduAnalysis(pdu));
    pdu = "123";
    EXPECT_FALSE(gsmSmsMessage->PduAnalysis(pdu));
    pdu = "123456";
    EXPECT_FALSE(gsmSmsMessage->PduAnalysis(pdu));
    EXPECT_TRUE(gsmSmsMessage->CreateDeliverReportSmsTpdu() != nullptr);
    EXPECT_FALSE(gsmSmsMessage->PduAnalysis(pdu));
    gsmSmsMessage->smsTpdu_ = nullptr;
    gsmSmsMessage->smsTpdu_ = std::make_shared<struct SmsTpdu>();
    gsmSmsMessage->ConvertMessageDcs();
    gsmSmsMessage->smsTpdu_->tpduType = SmsTpduType::SMS_TPDU_DELIVER;
    gsmSmsMessage->ConvertUserData();
    gsmSmsMessage->ConvertMessageDcs();
    gsmSmsMessage->smsTpdu_->tpduType = SmsTpduType::SMS_TPDU_SUBMIT;
    gsmSmsMessage->ConvertUserData();
    gsmSmsMessage->ConvertMessageDcs();
    gsmSmsMessage->smsTpdu_->tpduType = SmsTpduType::SMS_TPDU_STATUS_REP;
    gsmSmsMessage->ConvertUserData();
    gsmSmsMessage->ConvertMessageDcs();
    gsmSmsMessage->smsTpdu_->tpduType = SmsTpduType::SMS_TPDU_DELIVER_REP;
    gsmSmsMessage->ConvertMessageDcs();
    gsmSmsMessage->ConvertUserData();
    EXPECT_TRUE(gsmSmsMessage->CreateDeliverSmsTpdu() != nullptr);
    EXPECT_FALSE(gsmSmsMessage->PduAnalysis(pdu));
}

/**
 * @tc.number   Telephony_SmsMmsGtest_SmsSender_0001
 * @tc.name     Test SmsSender
 * @tc.desc     Function test
 */
HWTEST_F(BranchSmsTest, SmsSender_0001, Function | MediumTest | Level1)
{
    std::function<void(std::shared_ptr<SmsSendIndexer>)> fun = nullptr;
    AppExecFwk::InnerEvent::Pointer event = AppExecFwk::InnerEvent::Get(RadioEvent::RADIO_SEND_SMS, 1);
    std::shared_ptr<SmsSender> smsSender = std::make_shared<CdmaSmsSender>(INVALID_SLOTID, fun);
    const sptr<ISendShortMessageCallback> sendCallback =
        iface_cast<ISendShortMessageCallback>(new SendShortMessageCallbackStub());
    const sptr<IDeliveryShortMessageCallback> deliveryCallback =
        iface_cast<IDeliveryShortMessageCallback>(new DeliveryShortMessageCallbackStub());
    const std::string desAddr = "qwe";
    const std::string scAddr = "123";
    const std::string text = "123";
    auto smsIndexer = std::make_shared<SmsSendIndexer>(desAddr, scAddr, text, sendCallback, deliveryCallback);
    smsSender->HandleMessageResponse(smsIndexer);
    smsIndexer->isFailure_ = true;
    smsSender->HandleMessageResponse(smsIndexer);
    smsSender->SyncSwitchISmsResponse();
    smsSender->ProcessEvent(event);
    event = AppExecFwk::InnerEvent::Get(RadioEvent::RADIO_SEND_CDMA_SMS, 1);
    smsSender->ProcessEvent(event);
    event = AppExecFwk::InnerEvent::Get(RadioEvent::RADIO_SEND_IMS_GSM_SMS, 1);
    smsSender->ProcessEvent(event);
    event = AppExecFwk::InnerEvent::Get(RadioEvent::RADIO_SEND_SMS_EXPECT_MORE, 1);
    smsSender->ProcessEvent(event);
    event = AppExecFwk::InnerEvent::Get(MSG_SMS_RETRY_DELIVERY, 1);
    smsSender->ProcessEvent(event);
    event = AppExecFwk::InnerEvent::Get(RadioEvent::RADIO_SMS_STATUS, 1);
    smsSender->ProcessEvent(event);
    event = AppExecFwk::InnerEvent::Get(RadioEvent::RADIO_GET_IMS_SMS, 1);
    smsSender->ProcessEvent(event);
    event = AppExecFwk::InnerEvent::Get(RadioEvent::RADIO_DIAL, 1);
    smsSender->ProcessEvent(event);
    event->GetSharedObject<RadioResponseInfo>() = std::make_shared<RadioResponseInfo>();
    smsSender->HandleResend(smsIndexer);
    smsIndexer->errorCode_ = static_cast<int32_t>(ErrType::ERR_CMD_SEND_FAILURE);
    smsSender->HandleResend(smsIndexer);
    smsIndexer->errorCode_ = static_cast<int32_t>(ErrType::ERR_GENERIC_FAILURE);
    smsSender->HandleResend(smsIndexer);
    smsSender->lastSmsDomain_ = 1;
    smsSender->HandleResend(smsIndexer);
    EXPECT_FALSE(smsSender->SendCacheMapAddItem(1, smsIndexer));
    event = nullptr;
    smsIndexer = nullptr;
    smsSender->HandleResend(smsIndexer);
    smsSender->ProcessEvent(event);
    smsSender->HandleMessageResponse(smsIndexer);
    smsSender->SetNetworkState(true, 1);
    EXPECT_TRUE(smsSender->FindCacheMapAndTransform(event) == nullptr);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_SmsSender_0002
 * @tc.name     Test SmsSender
 * @tc.desc     Function test
 */
HWTEST_F(BranchSmsTest, SmsSender_0002, Function | MediumTest | Level1)
{
    std::function<void(std::shared_ptr<SmsSendIndexer>)> fun = nullptr;
    std::shared_ptr<SmsSender> smsSender = std::make_shared<CdmaSmsSender>(INVALID_SLOTID, fun);
    const sptr<ISendShortMessageCallback> sendCallback =
        iface_cast<ISendShortMessageCallback>(new SendShortMessageCallbackStub());
    const sptr<IDeliveryShortMessageCallback> deliveryCallback =
        iface_cast<IDeliveryShortMessageCallback>(new DeliveryShortMessageCallbackStub());
    const std::string desAddr = "qwe";
    const std::string scAddr = "123";
    const std::string text = "123";
    auto smsIndexer = std::make_shared<SmsSendIndexer>(desAddr, scAddr, text, sendCallback, deliveryCallback);
    smsIndexer->unSentCellCount_ = 1;
    smsSender->SendMessageSucceed(smsIndexer);
    smsIndexer->unSentCellCount_ = 1;
    smsIndexer->hasCellFailed_ = std::make_shared<bool>(true);
    smsSender->SendMessageSucceed(smsIndexer);
    smsIndexer->unSentCellCount_ = 1;
    smsSender->SendMessageFailed(smsIndexer);
    smsIndexer->unSentCellCount_ = 0;
    smsSender->SendMessageFailed(smsIndexer);
    smsSender->SendResultCallBack(smsIndexer, ISendShortMessageCallback::SEND_SMS_FAILURE_UNKNOWN);
    smsSender->SendResultCallBack(sendCallback, ISendShortMessageCallback::SEND_SMS_FAILURE_UNKNOWN);
    smsIndexer = nullptr;
    smsSender->SendMessageSucceed(smsIndexer);
    smsSender->SendMessageFailed(smsIndexer);
    EXPECT_FALSE(smsSender->SendCacheMapAddItem(1, smsIndexer));
}

/**
 * @tc.number   Telephony_SmsMmsGtest_SmsSender_0003
 * @tc.name     Test SmsSender
 * @tc.desc     Function test
 */
HWTEST_F(BranchSmsTest, SmsSender_0003, Function | MediumTest | Level1)
{
    std::function<void(std::shared_ptr<SmsSendIndexer>)> fun = nullptr;
    std::shared_ptr<SmsSender> smsSender = std::make_shared<CdmaSmsSender>(INVALID_SLOTID, fun);
    const sptr<ISendShortMessageCallback> sendCallback =
        iface_cast<ISendShortMessageCallback>(new SendShortMessageCallbackStub());
    const sptr<IDeliveryShortMessageCallback> deliveryCallback =
        iface_cast<IDeliveryShortMessageCallback>(new DeliveryShortMessageCallbackStub());
    const std::string desAddr = "qwe";
    const std::string scAddr = "123";
    const std::string text = "123";
    auto smsIndexer = std::make_shared<SmsSendIndexer>(desAddr, scAddr, text, sendCallback, deliveryCallback);
    EXPECT_FALSE(smsSender->SendCacheMapAddItem(1, smsIndexer));
    smsIndexer->isFailure_ = true;
    smsSender->OnRilAdapterHostDied();
    smsIndexer->isFailure_ = false;
    smsIndexer->msgRefId64Bit_ = 0;
    smsSender->OnRilAdapterHostDied();
    EXPECT_GE(smsIndexer->msgRefId64Bit_, 0);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_SmsSendManager_0001
 * @tc.name     Test SmsSendManager
 * @tc.desc     Function test
 */
HWTEST_F(BranchSmsTest, SmsSendManager_0001, Function | MediumTest | Level1)
{
    auto smsSendManager = std::make_shared<SmsSendManager>(INVALID_SLOTID);
    auto smsNetworkPolicyManager = std::make_shared<SmsNetworkPolicyManager>(INVALID_SLOTID);
    std::function<void(std::shared_ptr<SmsSendIndexer>)> fun = nullptr;
    auto cdmaSmsSender = std::make_shared<CdmaSmsSender>(INVALID_SLOTID, fun);
    auto gsmSmsSender = std::make_shared<GsmSmsSender>(INVALID_SLOTID, fun);
    std::string desAddr = "";
    std::string scAddr = "123";
    std::string text = "";
    std::u16string format = u"";
    uint8_t *data = nullptr;
    const sptr<ISendShortMessageCallback> sendCallback =
        iface_cast<ISendShortMessageCallback>(new SendShortMessageCallbackStub());
    const sptr<IDeliveryShortMessageCallback> deliveryCallback =
        iface_cast<IDeliveryShortMessageCallback>(new DeliveryShortMessageCallbackStub());
    auto smsIndexer = std::make_shared<SmsSendIndexer>(desAddr, scAddr, text, sendCallback, deliveryCallback);
    smsSendManager->TextBasedSmsDelivery(desAddr, scAddr, text, sendCallback, deliveryCallback);
    smsSendManager->DataBasedSmsDelivery(desAddr, scAddr, 1, data, 1, sendCallback, deliveryCallback);
    desAddr = "qwe";
    smsSendManager->DataBasedSmsDelivery(desAddr, scAddr, 1, data, 1, sendCallback, deliveryCallback);
    smsSendManager->TextBasedSmsDelivery(desAddr, scAddr, text, sendCallback, deliveryCallback);
    text = "123";
    data = new uint8_t(1);
    smsSendManager->DataBasedSmsDelivery(desAddr, scAddr, 1, data, 1, sendCallback, deliveryCallback);
    smsSendManager->TextBasedSmsDelivery(desAddr, scAddr, text, sendCallback, deliveryCallback);
    smsSendManager->networkManager_ = smsNetworkPolicyManager;
    smsSendManager->DataBasedSmsDelivery(desAddr, scAddr, 1, data, 1, sendCallback, deliveryCallback);
    smsSendManager->TextBasedSmsDelivery(desAddr, scAddr, text, sendCallback, deliveryCallback);
    smsSendManager->RetriedSmsDelivery(smsIndexer);
    smsSendManager->gsmSmsSender_ = gsmSmsSender;
    smsSendManager->DataBasedSmsDelivery(desAddr, scAddr, 1, data, 1, sendCallback, deliveryCallback);
    smsSendManager->TextBasedSmsDelivery(desAddr, scAddr, text, sendCallback, deliveryCallback);
    smsSendManager->RetriedSmsDelivery(smsIndexer);
    smsSendManager->cdmaSmsSender_ = cdmaSmsSender;
    smsSendManager->networkManager_->netWorkType_ = NetWorkType::NET_TYPE_GSM;
    smsSendManager->DataBasedSmsDelivery(desAddr, scAddr, 1, data, 1, sendCallback, deliveryCallback);
    smsSendManager->TextBasedSmsDelivery(desAddr, scAddr, text, sendCallback, deliveryCallback);
    EXPECT_EQ(smsSendManager->GetImsShortMessageFormat(format), TELEPHONY_ERR_SUCCESS);
    smsSendManager->networkManager_->netWorkType_ = NetWorkType::NET_TYPE_CDMA;
    smsSendManager->DataBasedSmsDelivery(desAddr, scAddr, 1, data, 1, sendCallback, deliveryCallback);
    smsSendManager->TextBasedSmsDelivery(desAddr, scAddr, text, sendCallback, deliveryCallback);
    EXPECT_EQ(smsSendManager->GetImsShortMessageFormat(format), TELEPHONY_ERR_SUCCESS);
    smsSendManager->networkManager_->netWorkType_ = NetWorkType::NET_TYPE_UNKNOWN;
    smsSendManager->DataBasedSmsDelivery(desAddr, scAddr, 1, data, 1, sendCallback, deliveryCallback);
    smsSendManager->TextBasedSmsDelivery(desAddr, scAddr, text, sendCallback, deliveryCallback);
    smsIndexer = nullptr;
    smsSendManager->RetriedSmsDelivery(smsIndexer);
    EXPECT_EQ(smsSendManager->GetImsShortMessageFormat(format), TELEPHONY_ERR_SUCCESS);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_SmsSendManager_0002
 * @tc.name     Test SmsSendManager
 * @tc.desc     Function test
 */
HWTEST_F(BranchSmsTest, SmsSendManager_0002, Function | MediumTest | Level1)
{
    auto smsSendManager = std::make_shared<SmsSendManager>(INVALID_SLOTID);
    std::function<void(std::shared_ptr<SmsSendIndexer>)> fun = nullptr;
    std::string scAddr = "123";
    bool isSupported = true;
    std::vector<std::u16string> splitMessage;
    LengthInfo lenInfo;
    const sptr<ISendShortMessageCallback> sendCallback =
        iface_cast<ISendShortMessageCallback>(new SendShortMessageCallbackStub());
    const sptr<IDeliveryShortMessageCallback> deliveryCallback =
        iface_cast<IDeliveryShortMessageCallback>(new DeliveryShortMessageCallbackStub());
    auto smsIndexer = std::make_shared<SmsSendIndexer>("", scAddr, "", sendCallback, deliveryCallback);
    EXPECT_GT(smsSendManager->SplitMessage(scAddr, splitMessage), TELEPHONY_ERR_SUCCESS);
    EXPECT_GT(smsSendManager->GetSmsSegmentsInfo(scAddr, true, lenInfo), TELEPHONY_ERR_SUCCESS);
    EXPECT_GT(smsSendManager->IsImsSmsSupported(INVALID_SLOTID, isSupported), TELEPHONY_ERR_SUCCESS);
    EXPECT_FALSE(smsSendManager->SetImsSmsConfig(INVALID_SLOTID, 1));
    smsSendManager->networkManager_ = std::make_shared<SmsNetworkPolicyManager>(INVALID_SLOTID);
    EXPECT_FALSE(smsSendManager->SetImsSmsConfig(INVALID_SLOTID, 1));
    EXPECT_GT(smsSendManager->IsImsSmsSupported(INVALID_SLOTID, isSupported), TELEPHONY_ERR_SUCCESS);
    smsSendManager->gsmSmsSender_ = std::make_shared<CdmaSmsSender>(INVALID_SLOTID, fun);
    EXPECT_FALSE(smsSendManager->SetImsSmsConfig(INVALID_SLOTID, 1));
    EXPECT_GT(smsSendManager->IsImsSmsSupported(INVALID_SLOTID, isSupported), TELEPHONY_ERR_SUCCESS);
    smsSendManager->cdmaSmsSender_ = std::make_shared<GsmSmsSender>(INVALID_SLOTID, fun);
    smsSendManager->RetriedSmsDelivery(smsIndexer);
    smsIndexer->netWorkType_ = NetWorkType::NET_TYPE_CDMA;
    smsSendManager->networkManager_->netWorkType_ = NetWorkType::NET_TYPE_UNKNOWN;
    smsSendManager->RetriedSmsDelivery(smsIndexer);
    smsIndexer->netWorkType_ = NetWorkType::NET_TYPE_UNKNOWN;
    EXPECT_FALSE(smsSendManager->SetImsSmsConfig(INVALID_SLOTID, 1));
    EXPECT_GT(smsSendManager->IsImsSmsSupported(INVALID_SLOTID, isSupported), TELEPHONY_ERR_SUCCESS);
    EXPECT_GT(smsSendManager->GetSmsSegmentsInfo(scAddr, true, lenInfo), TELEPHONY_ERR_SUCCESS);
    EXPECT_EQ(smsSendManager->SplitMessage(scAddr, splitMessage), TELEPHONY_ERR_SUCCESS);
    smsSendManager->RetriedSmsDelivery(smsIndexer);
    smsSendManager->networkManager_->netWorkType_ = NetWorkType::NET_TYPE_GSM;
    smsIndexer->netWorkType_ = NetWorkType::NET_TYPE_GSM;
    EXPECT_TRUE(smsSendManager->SetImsSmsConfig(INVALID_SLOTID, 1));
    EXPECT_EQ(smsSendManager->IsImsSmsSupported(INVALID_SLOTID, isSupported), TELEPHONY_ERR_SUCCESS);
    EXPECT_EQ(smsSendManager->SplitMessage(scAddr, splitMessage), TELEPHONY_ERR_SUCCESS);
    EXPECT_EQ(smsSendManager->GetSmsSegmentsInfo(scAddr, true, lenInfo), TELEPHONY_ERR_SUCCESS);
    smsSendManager->RetriedSmsDelivery(smsIndexer);
    smsSendManager->networkManager_->netWorkType_ = NetWorkType::NET_TYPE_CDMA;
    smsIndexer->netWorkType_ = NetWorkType::NET_TYPE_CDMA;
    smsSendManager->RetriedSmsDelivery(smsIndexer);
    EXPECT_TRUE(smsSendManager->SetImsSmsConfig(INVALID_SLOTID, 1));
    EXPECT_EQ(smsSendManager->IsImsSmsSupported(INVALID_SLOTID, isSupported), TELEPHONY_ERR_SUCCESS);
    EXPECT_EQ(smsSendManager->SplitMessage(scAddr, splitMessage), TELEPHONY_ERR_SUCCESS);
    EXPECT_EQ(smsSendManager->GetSmsSegmentsInfo(scAddr, true, lenInfo), TELEPHONY_ERR_SUCCESS);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_SmsInterfaceManager_0001
 * @tc.name     Test SmsInterfaceManager
 * @tc.desc     Function test
 */
HWTEST_F(BranchSmsTest, SmsInterfaceManager_0001, Function | MediumTest | Level1)
{
    auto smsSendManager = std::make_shared<SmsSendManager>(INVALID_SLOTID);
    std::function<void(std::shared_ptr<SmsSendIndexer>)> fun = nullptr;
    std::string scAddr = "123";
    const sptr<ISendShortMessageCallback> sendCallback =
        iface_cast<ISendShortMessageCallback>(new SendShortMessageCallbackStub());
    const sptr<IDeliveryShortMessageCallback> deliveryCallback =
        iface_cast<IDeliveryShortMessageCallback>(new DeliveryShortMessageCallbackStub());
    auto smsIndexer = std::make_shared<SmsSendIndexer>("", scAddr, "", sendCallback, deliveryCallback);

    auto smsInterfaceManager = std::make_shared<SmsInterfaceManager>(INVALID_SLOTID);
    EXPECT_EQ(smsInterfaceManager->OnRilAdapterHostDied(), TELEPHONY_ERR_LOCAL_PTR_NULL);

    smsSendManager->gsmSmsSender_ = std::make_shared<GsmSmsSender>(INVALID_SLOTID, fun);
    smsSendManager->cdmaSmsSender_ = std::make_shared<CdmaSmsSender>(INVALID_SLOTID, fun);
    smsInterfaceManager->InitInterfaceManager();
    EXPECT_EQ(smsInterfaceManager->OnRilAdapterHostDied(), TELEPHONY_ERR_SUCCESS);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_GsmSmsParamCodec_0001
 * @tc.name     Test GsmSmsParamCodec
 * @tc.desc     Function test
 */
HWTEST_F(BranchSmsTest, GsmSmsParamCodec_0001, Function | MediumTest | Level1)
{
    auto gsmSmsParamCodec = std::make_shared<GsmSmsParamCodec>();
    AddressNumber *pAddress = new AddressNumber();
    SmsTimeStamp *pTimeStamp = new SmsTimeStamp();
    pTimeStamp->format = SmsTimeFormat::SMS_TIME_ABSOLUTE;
    pTimeStamp->time.absolute.timeZone = -1;
    std::string paramStr;
    EXPECT_GT(gsmSmsParamCodec->EncodeAddressPdu(pAddress, paramStr), 0);
    pAddress->address[0] = '+';
    paramStr.clear();
    EXPECT_GT(gsmSmsParamCodec->EncodeAddressPdu(pAddress, paramStr), 0);
    paramStr.clear();
    EXPECT_EQ(gsmSmsParamCodec->EncodeAddressPdu(nullptr, paramStr), 0);

    paramStr.clear();
    gsmSmsParamCodec->EncodeTimePdu(nullptr, paramStr);
    uint8_t value = 0;
    EXPECT_GE(paramStr.size(), value);
    paramStr.clear();
    gsmSmsParamCodec->EncodeTimePdu(pTimeStamp, paramStr);
    EXPECT_GT(paramStr.size(), value);
    pTimeStamp->format = SmsTimeFormat::SMS_TIME_RELATIVE;
    paramStr.clear();
    uint8_t result = 1;
    gsmSmsParamCodec->EncodeTimePdu(pTimeStamp, paramStr);
    EXPECT_GE(paramStr.size(), result);

    SmsDcs *pDCS = new SmsDcs();
    paramStr.clear();
    gsmSmsParamCodec->EncodeDCS(nullptr, paramStr);
    EXPECT_EQ(paramStr.size(), value);
    pDCS->codingGroup = PduSchemeGroup::CODING_DELETION_GROUP;
    paramStr.clear();
    gsmSmsParamCodec->EncodeDCS(pDCS, paramStr);
    EXPECT_GE(paramStr.size(), value);

    pDCS->codingGroup = PduSchemeGroup::CODING_DISCARD_GROUP;
    paramStr.clear();
    gsmSmsParamCodec->EncodeDCS(pDCS, paramStr);
    EXPECT_GE(paramStr.size(), value);

    pDCS->codingGroup = PduSchemeGroup::CODING_STORE_GROUP;
    paramStr.clear();
    gsmSmsParamCodec->EncodeDCS(pDCS, paramStr);
    EXPECT_GE(paramStr.size(), value);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_GsmSmsParamCodec_0002
 * @tc.name     Test GsmSmsParamCodec
 * @tc.desc     Function test
 */
HWTEST_F(BranchSmsTest, GsmSmsParamCodec_0002, Function | MediumTest | Level1)
{
    auto gsmSmsParamCodec = std::make_shared<GsmSmsParamCodec>();
    std::string paramStr;
    SmsDcs *pDCS = new SmsDcs();
    pDCS->codingGroup = PduSchemeGroup::CODING_GENERAL_GROUP;
    pDCS->msgClass = SmsMessageClass::SMS_FORWARD_MESSAGE;
    pDCS->bCompressed = true;
    gsmSmsParamCodec->EncodeDCS(pDCS, paramStr);
    uint8_t result = 0;
    EXPECT_GE(paramStr.size(), result);

    pDCS->codingGroup = PduSchemeGroup::SMS_CLASS_GROUP;
    paramStr.clear();
    gsmSmsParamCodec->EncodeDCS(pDCS, paramStr);
    EXPECT_GE(paramStr.size(), result);

    pDCS->codingScheme = DataCodingScheme::DATA_CODING_7BIT;
    paramStr.clear();
    gsmSmsParamCodec->EncodeDCS(pDCS, paramStr);
    EXPECT_GE(paramStr.size(), result);

    pDCS->codingScheme = DataCodingScheme::DATA_CODING_UCS2;
    paramStr.clear();
    gsmSmsParamCodec->EncodeDCS(pDCS, paramStr);
    EXPECT_GE(paramStr.size(), result);

    pDCS->codingScheme = DataCodingScheme::DATA_CODING_8BIT;
    paramStr.clear();
    gsmSmsParamCodec->EncodeDCS(pDCS, paramStr);
    EXPECT_GE(paramStr.size(), result);

    pDCS->codingScheme = DataCodingScheme::DATA_CODING_EUCKR;
    paramStr.clear();
    gsmSmsParamCodec->EncodeDCS(pDCS, paramStr);

    EXPECT_GE(paramStr.size(), result);
    paramStr.clear();
    pDCS->codingGroup = PduSchemeGroup::CODING_UNKNOWN_GROUP;
    gsmSmsParamCodec->EncodeDCS(pDCS, paramStr);
    uint8_t value = 0;
    EXPECT_EQ(paramStr.size(), value);

    unsigned char encodeData[BUF_SIZE];
    char addressData[BUF_SIZE];
    char *address = addressData;
    unsigned char *pEncodeAddr = encodeData;
    EXPECT_EQ(gsmSmsParamCodec->EncodeSmscPdu(nullptr, nullptr), 0);
    EXPECT_EQ(gsmSmsParamCodec->EncodeSmscPdu(address, nullptr), 0);
    EXPECT_EQ(gsmSmsParamCodec->EncodeSmscPdu(address, pEncodeAddr), 0);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_GsmSmsParamCodec_0003
 * @tc.name     Test GsmSmsParamCodec
 * @tc.desc     Function test
 */
HWTEST_F(BranchSmsTest, GsmSmsParamCodec_0003, Function | MediumTest | Level1)
{
    auto gsmSmsParamCodec = std::make_shared<GsmSmsParamCodec>();
    AddressNumber *pAddress = new AddressNumber();
    SmsDcs *pDCS = new SmsDcs();
    unsigned char encodeData[BUF_SIZE] = { 0 };
    unsigned char *pSMSC = encodeData;
    std::string decodeAddr;
    gsmSmsParamCodec->DecodeSmscPdu(pSMSC, 0, TypeOfNum::TYPE_UNKNOWN, decodeAddr);
    gsmSmsParamCodec->DecodeSmscPdu(nullptr, 1, TypeOfNum::TYPE_UNKNOWN, decodeAddr);
    gsmSmsParamCodec->DecodeSmscPdu(pSMSC, 1, TypeOfNum::TYPE_UNKNOWN, decodeAddr);
    gsmSmsParamCodec->DecodeSmscPdu(pSMSC, 1, TypeOfNum::TYPE_INTERNATIONAL, decodeAddr);
    gsmSmsParamCodec->DecodeSmscPdu(pSMSC, 1, TypeOfNum::TYPE_INTERNATIONAL, decodeAddr);

    auto decodeBuffer = std::make_shared<SmsReadBuffer>("00");
    EXPECT_EQ(gsmSmsParamCodec->EncodeSmscPdu(nullptr, pSMSC, 0), 0);
    EXPECT_EQ(gsmSmsParamCodec->EncodeSmscPdu(pAddress, nullptr, 0), 0);
    EXPECT_EQ(gsmSmsParamCodec->EncodeSmscPdu(pAddress, pSMSC, 0), 0);
    EXPECT_EQ(gsmSmsParamCodec->DecodeAddressPdu(*decodeBuffer, nullptr), 0);
    EXPECT_EQ(gsmSmsParamCodec->DecodeAddressPdu(*decodeBuffer, pAddress), 0);
    EXPECT_EQ(gsmSmsParamCodec->DecodeTimePdu(*decodeBuffer, nullptr), 0);
    EXPECT_EQ(gsmSmsParamCodec->DecodeDcsPdu(*decodeBuffer, pDCS), 1);
    EXPECT_EQ(gsmSmsParamCodec->DecodeDcsPdu(*decodeBuffer, nullptr), 0);
    AddressNumber smsAddress;
    EXPECT_EQ(gsmSmsParamCodec->DecodeSmscPdu(nullptr, 1, smsAddress), 0);
    EXPECT_GE(gsmSmsParamCodec->DecodeSmscPdu(pSMSC, 1, smsAddress), 0);
    EXPECT_EQ(gsmSmsParamCodec->CheckVoicemail(*decodeBuffer, nullptr, nullptr), 0);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_GsmSmsUDataCodec_0001
 * @tc.name     Test GsmUserDataPdu
 * @tc.desc     Function test
 */
HWTEST_F(BranchSmsTest, GsmSmsUDataCodec_0001, Function | MediumTest | Level1)
{
    auto udPdu = std::make_shared<GsmUserDataPdu>();
    SmsUDPackage smsUserData;
    SmsTpud *pTPUD = new SmsTpud();
    const struct SmsUDPackage *pUserData = &(smsUserData);
    auto encodeBuffer = std::make_shared<SmsWriteBuffer>();
    auto decodeBuffer = std::make_shared<SmsReadBuffer>("00");
    std::string str;
    EXPECT_GT(udPdu->EncodeUserDataPdu(*encodeBuffer, pUserData, DataCodingScheme::DATA_CODING_7BIT, str), 0);
    EXPECT_GT(udPdu->EncodeUserDataPdu(*encodeBuffer, pUserData, DataCodingScheme::DATA_CODING_8BIT, str), 0);
    EXPECT_GT(udPdu->EncodeUserDataPdu(*encodeBuffer, pUserData, DataCodingScheme::DATA_CODING_UCS2, str), 0);
    EXPECT_EQ(udPdu->EncodeUserDataPdu(*encodeBuffer, pUserData, DataCodingScheme::DATA_CODING_ASCII7BIT, str), 0);
    SmsUDPackage *userData = new SmsUDPackage();
    EXPECT_GE(udPdu->DecodeUserDataPdu(*decodeBuffer, true, DataCodingScheme::DATA_CODING_7BIT, userData, pTPUD), 0);
    EXPECT_GE(udPdu->DecodeUserDataPdu(*decodeBuffer, true, DataCodingScheme::DATA_CODING_8BIT, userData, pTPUD), 0);
    EXPECT_GE(udPdu->DecodeUserDataPdu(*decodeBuffer, true, DataCodingScheme::DATA_CODING_UCS2, userData, pTPUD), 0);
    EXPECT_EQ(
        udPdu->DecodeUserDataPdu(*decodeBuffer, true, DataCodingScheme::DATA_CODING_ASCII7BIT, userData, pTPUD), 0);
    EXPECT_GE(udPdu->DecodeUserDataPdu(*decodeBuffer, true, DataCodingScheme::DATA_CODING_7BIT, userData), 0);
    EXPECT_GE(udPdu->DecodeUserDataPdu(*decodeBuffer, true, DataCodingScheme::DATA_CODING_8BIT, userData), 0);
    EXPECT_GE(udPdu->DecodeUserDataPdu(*decodeBuffer, true, DataCodingScheme::DATA_CODING_UCS2, userData), 0);
    EXPECT_EQ(udPdu->DecodeUserDataPdu(*decodeBuffer, true, DataCodingScheme::DATA_CODING_ASCII7BIT, userData), 0);

    auto encode = std::make_shared<GsmUserDataEncode>(udPdu);
    auto decode = std::make_shared<GsmUserDataDecode>(udPdu);
    EXPECT_TRUE(encode->Encode8bitPdu(*encodeBuffer, userData, str));
    EXPECT_TRUE(encode->EncodeUcs2Pdu(*encodeBuffer, userData));
    userData->headerCnt = 1;
    userData->length = BUFFER_SIZE;
    EXPECT_EQ(encode->Encode8bitPdu(*encodeBuffer, userData, str), 0);
    EXPECT_EQ(encode->EncodeUcs2Pdu(*encodeBuffer, userData), 0);
    EXPECT_EQ(decode->DecodeGsmPdu(*decodeBuffer, true, userData, pTPUD), 0);
    EXPECT_FALSE(decode->DecodeGsmPdu(*decodeBuffer, false, userData, pTPUD));
    EXPECT_FALSE(decode->Decode8bitPdu(*decodeBuffer, true, userData, pTPUD));
    EXPECT_FALSE(decode->DecodeGsmPdu(*decodeBuffer, false, userData, pTPUD));
    EXPECT_FALSE(decode->DecodeUcs2Pdu(*decodeBuffer, true, userData, pTPUD));
    EXPECT_FALSE(decode->DecodeGsmPdu(*decodeBuffer, false, userData, pTPUD));
    decode->userData_ = nullptr;
    EXPECT_FALSE(decode->Decode8bitPduPartData(*decodeBuffer, false, userData, pTPUD, 0, 0));
    EXPECT_FALSE(decode->Decode8bitPduPartData(*decodeBuffer, false, userData, pTPUD, 1, 1));
}

/**
 * @tc.number   Telephony_SmsMmsGtest_GsmSmsUDataCodec_0002
 * @tc.name     Test GsmUserDataPdu
 * @tc.desc     Function test
 */
HWTEST_F(BranchSmsTest, GsmSmsUDataCodec_0002, Function | MediumTest | Level1)
{
    auto gsmUserDataPdu = std::make_shared<GsmUserDataPdu>();
    SmsUDH header;
    header.udhType = UDH_CONCAT_8BIT;
    auto encodeBuffer = std::make_shared<SmsWriteBuffer>();
    encodeBuffer->data_[0] = 1;
    gsmUserDataPdu->EncodeHeaderConcat(*encodeBuffer, header);
    gsmUserDataPdu->EncodeHeader(*encodeBuffer, header);
    header.udhType = UDH_CONCAT_16BIT;
    gsmUserDataPdu->EncodeHeaderConcat(*encodeBuffer, header);
    gsmUserDataPdu->EncodeHeader(*encodeBuffer, header);
    header.udhType = UDH_APP_PORT_8BIT;
    gsmUserDataPdu->EncodeHeader(*encodeBuffer, header);
    header.udhType = UDH_APP_PORT_16BIT;
    gsmUserDataPdu->EncodeHeader(*encodeBuffer, header);
    header.udhType = UDH_ALTERNATE_REPLY_ADDRESS;
    gsmUserDataPdu->EncodeHeader(*encodeBuffer, header);
    header.udhType = UDH_SINGLE_SHIFT;
    gsmUserDataPdu->EncodeHeader(*encodeBuffer, header);
    header.udhType = UDH_LOCKING_SHIFT;
    gsmUserDataPdu->EncodeHeader(*encodeBuffer, header);
    header.udhType = UDH_NONE;
    gsmUserDataPdu->EncodeHeaderConcat(*encodeBuffer, header);
    gsmUserDataPdu->EncodeHeader(*encodeBuffer, header);
    header.udhType = UDH_EMS_LAST;
    gsmUserDataPdu->EncodeHeaderConcat(*encodeBuffer, header);
    gsmUserDataPdu->EncodeHeader(*encodeBuffer, header);
    SmsUDH *pHeader = new SmsUDH();

    pHeader->udhType = UserDataHeadType::UDH_CONCAT_8BIT;
    uint16_t headerLen = 0;
    auto decodeBuffer = std::make_shared<SmsReadBuffer>("00");
    decodeBuffer->data_[0] = 1;
    EXPECT_EQ(gsmUserDataPdu->DecodeHeader(*decodeBuffer, *pHeader, headerLen), 0);
    pHeader->udhType = UserDataHeadType::UDH_CONCAT_16BIT;
    EXPECT_EQ(gsmUserDataPdu->DecodeHeader(*decodeBuffer, *pHeader, headerLen), 0);
    pHeader->udhType = UserDataHeadType::UDH_APP_PORT_8BIT;
    EXPECT_EQ(gsmUserDataPdu->DecodeHeader(*decodeBuffer, *pHeader, headerLen), 0);
    pHeader->udhType = UserDataHeadType::UDH_APP_PORT_16BIT;
    EXPECT_EQ(gsmUserDataPdu->DecodeHeader(*decodeBuffer, *pHeader, headerLen), 0);
    pHeader->udhType = UserDataHeadType::UDH_SPECIAL_SMS;
    EXPECT_EQ(gsmUserDataPdu->DecodeHeader(*decodeBuffer, *pHeader, headerLen), 0);
    pHeader->udhType = UserDataHeadType::UDH_ALTERNATE_REPLY_ADDRESS;
    EXPECT_EQ(gsmUserDataPdu->DecodeHeader(*decodeBuffer, *pHeader, headerLen), 0);
    pHeader->udhType = UserDataHeadType::UDH_SINGLE_SHIFT;
    EXPECT_EQ(gsmUserDataPdu->DecodeHeader(*decodeBuffer, *pHeader, headerLen), 0);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_GsmSmsUDataCodec_0003
 * @tc.name     Test GsmUserDataPdu
 * @tc.desc     Function test
 */
HWTEST_F(BranchSmsTest, GsmSmsUDataCodec_0003, Function | MediumTest | Level1)
{
    auto gsmUserDataPdu = std::make_shared<GsmUserDataPdu>();
    SmsUDH *pHeader = new SmsUDH();
    pHeader->udhType = UDH_CONCAT_8BIT;
    gsmUserDataPdu->DebugDecodeHeader(*pHeader);
    pHeader->udhType = UDH_CONCAT_16BIT;
    gsmUserDataPdu->DebugDecodeHeader(*pHeader);
    pHeader->udhType = UDH_APP_PORT_8BIT;
    gsmUserDataPdu->DebugDecodeHeader(*pHeader);
    pHeader->udhType = UDH_APP_PORT_16BIT;
    gsmUserDataPdu->DebugDecodeHeader(*pHeader);
    pHeader->udhType = UDH_SPECIAL_SMS;
    gsmUserDataPdu->DebugDecodeHeader(*pHeader);
    pHeader->udhType = UDH_ALTERNATE_REPLY_ADDRESS;
    gsmUserDataPdu->DebugDecodeHeader(*pHeader);
    pHeader->udhType = UDH_SINGLE_SHIFT;
    gsmUserDataPdu->DebugDecodeHeader(*pHeader);
    pHeader->udhType = UDH_LOCKING_SHIFT;
    gsmUserDataPdu->DebugDecodeHeader(*pHeader);
    pHeader->udhType = UDH_NONE;
    gsmUserDataPdu->DebugDecodeHeader(*pHeader);
    pHeader->udhType = UserDataHeadType::UDH_LOCKING_SHIFT;

    auto decodeBuffer = std::make_shared<SmsReadBuffer>("00");
    decodeBuffer->data_[0] = 1;
    uint16_t headerLen;
    EXPECT_EQ(gsmUserDataPdu->DecodeHeader(*decodeBuffer, *pHeader, headerLen), 0);
    pHeader->udhType = UserDataHeadType::UDH_NONE;
    EXPECT_EQ(gsmUserDataPdu->DecodeHeader(*decodeBuffer, *pHeader, headerLen), 0);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_GsmSmsTpduCodec_0001
 * @tc.name     Test GsmSmsTpduCodec
 * @tc.desc     Function test
 */
HWTEST_F(BranchSmsTest, GsmSmsTpduCodec_0001, Function | MediumTest | Level1)
{
    unsigned char encodeData[BUF_SIZE];
    unsigned char *pSMSC = encodeData;
    char addressData[BUF_SIZE];
    char *pDecodeAddr = addressData;
    auto smsTpdu = std::make_shared<SmsTpdu>();
    smsTpdu->tpduType = SmsTpduType::SMS_TPDU_SUBMIT;

    uint16_t bufLen;
    auto tpduCodec = std::make_shared<GsmSmsTpduCodec>();
    EXPECT_GE(tpduCodec->EncodeSmsPdu(smsTpdu, pDecodeAddr, 1, bufLen), 0);
    smsTpdu->tpduType = SmsTpduType::SMS_TPDU_DELIVER;
    EXPECT_GE(tpduCodec->EncodeSmsPdu(smsTpdu, pDecodeAddr, 1, bufLen), 0);
    smsTpdu->tpduType = SmsTpduType::SMS_TPDU_DELIVER_REP;
    EXPECT_GE(tpduCodec->EncodeSmsPdu(smsTpdu, pDecodeAddr, 1, bufLen), 0);
    smsTpdu->tpduType = SmsTpduType::SMS_TPDU_STATUS_REP;
    EXPECT_GE(tpduCodec->EncodeSmsPdu(smsTpdu, pDecodeAddr, 1, bufLen), 0);

    SmsTpdu *pSmsTpdu = new SmsTpdu();
    EXPECT_EQ(tpduCodec->DecodeSmsPdu(nullptr, 1, pSmsTpdu), 0);
    EXPECT_EQ(tpduCodec->DecodeSmsPdu(pSMSC, 1, nullptr), 0);
    EXPECT_GE(tpduCodec->DecodeSmsPdu(pSMSC, 1, pSmsTpdu), 0);
    pSMSC[0] = 1;
    EXPECT_GE(tpduCodec->DecodeSmsPdu(pSMSC, 1, pSmsTpdu), 0);
    pSMSC[0] = VALUE_LENGTH;
    EXPECT_GE(tpduCodec->DecodeSmsPdu(pSMSC, 1, pSmsTpdu), 0);
    pSMSC[0] = HEADER_LENGTH;
    EXPECT_EQ(tpduCodec->DecodeSmsPdu(pSMSC, 1, pSmsTpdu), 0);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_GsmSmsTpduCodec_0002
 * @tc.name     Test GsmSmsTpduCodec
 * @tc.desc     Function test
 */
HWTEST_F(BranchSmsTest, GsmSmsTpduCodec_0002, Function | MediumTest | Level1)
{
    auto tpduCodec = std::make_shared<GsmSmsTpduCodec>();
    auto encode = std::make_shared<GsmSmsTpduEncode>(tpduCodec->uDataCodec_, tpduCodec->paramCodec_, tpduCodec);
    auto encodeBuffer = std::make_shared<SmsWriteBuffer>();
    encodeBuffer->data_[0] = 1;
    SmsSubmit *pSubmit = new SmsSubmit();
    SmsDeliver *pDeliver = new SmsDeliver();
    SmsDeliverReport *pDeliverRep = new SmsDeliverReport();
    EXPECT_EQ(encode->EncodeSubmitPdu(*encodeBuffer, nullptr), 0);
    EXPECT_EQ(encode->EncodeSubmitPdu(*encodeBuffer, pSubmit), 1);
    EXPECT_EQ(encode->EncodeDeliverPdu(*encodeBuffer, nullptr), 0);
    EXPECT_EQ(encode->EncodeDeliverPdu(*encodeBuffer, pDeliver), 0);
    pDeliver->bMoreMsg = false;
    pDeliver->bStatusReport = true;
    pDeliver->bHeaderInd = true;
    pDeliver->bReplyPath = true;
    EXPECT_GE(encode->EncodeDeliverPdu(*encodeBuffer, pDeliver), 0);
    EXPECT_EQ(encode->EncodeDeliverReportPdu(*encodeBuffer, nullptr), 0);
    EXPECT_EQ(encode->EncodeDeliverReportPdu(*encodeBuffer, pDeliverRep), 0);
    pDeliverRep->bHeaderInd = true;
    pDeliverRep->reportType = _SMS_REPORT_TYPE_E::SMS_REPORT_NEGATIVE;
    pDeliverRep->paramInd = 1;
    EXPECT_GE(encode->EncodeDeliverReportPdu(*encodeBuffer, pDeliverRep), 0);
    pDeliverRep->paramInd = VALUE_LENGTH;
    EXPECT_GE(encode->EncodeDeliverReportPdu(*encodeBuffer, pDeliverRep), 0);
    pDeliverRep->paramInd = START_BIT;
    EXPECT_GE(encode->EncodeDeliverReportPdu(*encodeBuffer, pDeliverRep), 0);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_GsmSmsTpduCodec_0003
 * @tc.name     Test GsmSmsTpduCodec
 * @tc.desc     Function test
 */
HWTEST_F(BranchSmsTest, GsmSmsTpduCodec_0003, Function | MediumTest | Level1)
{
    auto tpduCodec = std::make_shared<GsmSmsTpduCodec>();
    SmsStatusReport *pStatusRep = new SmsStatusReport();
    SmsSubmit *pSmsSub = new SmsSubmit();

    auto encode = std::make_shared<GsmSmsTpduEncode>(tpduCodec->uDataCodec_, tpduCodec->paramCodec_, tpduCodec);
    auto encodeBuffer = std::make_shared<SmsWriteBuffer>();
    encodeBuffer->data_[0] = 1;
    EXPECT_EQ(encode->EncodeStatusReportPdu(*encodeBuffer, nullptr), 0);
    EXPECT_EQ(encode->EncodeStatusReportPdu(*encodeBuffer, pStatusRep), 0);
    pStatusRep->bMoreMsg = true;
    pStatusRep->bStatusReport = true;
    pStatusRep->bHeaderInd = true;
    pStatusRep->paramInd = 1;
    EXPECT_GE(encode->EncodeStatusReportPdu(*encodeBuffer, pStatusRep), 0);
    pStatusRep->paramInd = VALUE_LENGTH;
    EXPECT_GE(encode->EncodeStatusReportPdu(*encodeBuffer, pStatusRep), 0);
    pStatusRep->paramInd = START_BIT;
    EXPECT_GE(encode->EncodeStatusReportPdu(*encodeBuffer, pStatusRep), 0);

    auto decode = std::make_shared<GsmSmsTpduDecode>(tpduCodec->uDataCodec_, tpduCodec->paramCodec_, tpduCodec);
    auto decodeBuffer = std::make_shared<SmsReadBuffer>("00");
    decodeBuffer->data_[0] = 1;
    EXPECT_EQ(decode->DecodeSubmit(*decodeBuffer, pSmsSub), 0);
    EXPECT_EQ(decode->DecodeSubmit(*decodeBuffer, nullptr), 0);
    SmsDeliver *pDeliver = new SmsDeliver();
    EXPECT_EQ(decode->DecodeDeliver(*decodeBuffer, pDeliver), 0);
    EXPECT_EQ(decode->DecodeDeliver(*decodeBuffer, nullptr), 0);
    EXPECT_EQ(decode->DecodeStatusReport(*decodeBuffer, pStatusRep), 0);
    EXPECT_EQ(decode->DecodeStatusReport(*decodeBuffer, nullptr), 0);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_GsmSmsTpduCodec_0004
 * @tc.name     Test GsmSmsTpduCodec
 * @tc.desc     Function test
 */
HWTEST_F(BranchSmsTest, GsmSmsTpduCodec_0004, Function | MediumTest | Level1)
{
    auto tpduCodec = std::make_shared<GsmSmsTpduCodec>();
    auto smsTpdu_ = std::make_shared<SmsTpdu>();
    char tpduBuf[MAX_TPDU_DATA_LEN] = { 0 };
    uint16_t bufLen;
    bool ret = tpduCodec->EncodeSmsPdu(smsTpdu_, tpduBuf, sizeof(tpduBuf), bufLen);
    SmsSubmit pSubmit;
    pSubmit.bRejectDup = true;
    pSubmit.bStatusReport = true;
    pSubmit.bHeaderInd = true;
    pSubmit.bReplyPath = true;
    auto decodeBuffer = std::make_shared<SmsReadBuffer>("00");
    decodeBuffer->data_[0] = 1;
    tpduCodec->DebugTpdu(*decodeBuffer, SmsParseType::PARSE_SUBMIT_TYPE);

    auto encodeBuffer = std::make_shared<SmsWriteBuffer>();
    encodeBuffer->data_[0] = 1;
    auto encode = std::make_shared<GsmSmsTpduEncode>(tpduCodec->uDataCodec_, tpduCodec->paramCodec_, tpduCodec);
    encode->EncodeSubmitTpduType(*encodeBuffer, pSubmit);
    pSubmit.vpf = SmsVpf::SMS_VPF_NOT_PRESENT;
    encode->EncodeSubmitTpduType(*encodeBuffer, pSubmit);
    pSubmit.vpf = SmsVpf::SMS_VPF_ENHANCED;
    encode->EncodeSubmitTpduType(*encodeBuffer, pSubmit);
    pSubmit.vpf = SmsVpf::SMS_VPF_RELATIVE;
    encode->EncodeSubmitTpduType(*encodeBuffer, pSubmit);
    pSubmit.vpf = SmsVpf::SMS_VPF_ABSOLUTE;
    encode->EncodeSubmitTpduType(*encodeBuffer, pSubmit);
    EXPECT_TRUE(ret);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_SmsBaseMessage_0001
 * @tc.name     Test SmsBaseMessage
 * @tc.desc     Function test
 */
HWTEST_F(BranchSmsTest, SmsBaseMessage_0001, Function | MediumTest | Level1)
{
    auto smsBaseMessage = std::make_shared<GsmSmsMessage>();
    smsBaseMessage->ConvertMessageClass(SmsMessageClass::SMS_SIM_MESSAGE);
    smsBaseMessage->ConvertMessageClass(SmsMessageClass::SMS_INSTANT_MESSAGE);
    smsBaseMessage->ConvertMessageClass(SmsMessageClass::SMS_OPTIONAL_MESSAGE);
    smsBaseMessage->ConvertMessageClass(SmsMessageClass::SMS_FORWARD_MESSAGE);
    smsBaseMessage->ConvertMessageClass(SmsMessageClass::SMS_CLASS_UNKNOWN);
    SplitInfo split;
    DataCodingScheme codingType = DataCodingScheme::DATA_CODING_7BIT;
    smsBaseMessage->ConvertSpiltToUtf8(split, codingType);
    split.encodeData.push_back(1);
    codingType = DataCodingScheme::DATA_CODING_UCS2;
    smsBaseMessage->ConvertSpiltToUtf8(split, codingType);
    codingType = DataCodingScheme::DATA_CODING_AUTO;
    smsBaseMessage->ConvertSpiltToUtf8(split, codingType);
    smsBaseMessage->smsUserData_.headerCnt = 1;
    smsBaseMessage->smsUserData_.header[0].udhType = UserDataHeadType::UDH_CONCAT_8BIT;
    EXPECT_FALSE(smsBaseMessage->GetConcatMsg() == nullptr);
    smsBaseMessage->smsUserData_.header[0].udhType = UserDataHeadType::UDH_CONCAT_16BIT;
    EXPECT_FALSE(smsBaseMessage->GetConcatMsg() == nullptr);
    smsBaseMessage->smsUserData_.header[0].udhType = UserDataHeadType::UDH_APP_PORT_8BIT;
    EXPECT_FALSE(smsBaseMessage->GetPortAddress() == nullptr);
    smsBaseMessage->smsUserData_.header[0].udhType = UserDataHeadType::UDH_APP_PORT_16BIT;
    EXPECT_FALSE(smsBaseMessage->GetPortAddress() == nullptr);
    smsBaseMessage->smsUserData_.header[0].udhType = UserDataHeadType::UDH_SPECIAL_SMS;
    EXPECT_FALSE(smsBaseMessage->GetSpecialSmsInd() == nullptr);
    smsBaseMessage->smsUserData_.header[0].udhType = UserDataHeadType::UDH_APP_PORT_8BIT;
    EXPECT_FALSE(smsBaseMessage->IsWapPushMsg());
    MSG_LANGUAGE_ID_T langId = 1;
    codingType = DataCodingScheme::DATA_CODING_7BIT;
    EXPECT_GT(smsBaseMessage->GetMaxSegmentSize(codingType, 1, true, langId, 1), 0);
    EXPECT_GT(smsBaseMessage->GetSegmentSize(codingType, 1, true, langId, 1), 0);
    codingType = DataCodingScheme::DATA_CODING_ASCII7BIT;
    EXPECT_GT(smsBaseMessage->GetMaxSegmentSize(codingType, 1, true, langId, 1), 0);
    EXPECT_GT(smsBaseMessage->GetSegmentSize(codingType, 1, true, langId, 1), 0);
    codingType = DataCodingScheme::DATA_CODING_8BIT;
    EXPECT_GT(smsBaseMessage->GetMaxSegmentSize(codingType, 1, true, langId, 1), 0);
    EXPECT_GT(smsBaseMessage->GetSegmentSize(codingType, 1, true, langId, 1), 0);
    codingType = DataCodingScheme::DATA_CODING_UCS2;
    EXPECT_GT(smsBaseMessage->GetMaxSegmentSize(codingType, 1, true, langId, 1), 0);
    EXPECT_GT(smsBaseMessage->GetSegmentSize(codingType, 1, true, langId, 1), 0);
    std::string message = "";
    LengthInfo lenInfo;
    EXPECT_GE(smsBaseMessage->GetSmsSegmentsInfo(message, true, lenInfo), 0);
    message = "123";
    EXPECT_EQ(smsBaseMessage->GetSmsSegmentsInfo(message, true, lenInfo), TELEPHONY_ERR_SUCCESS);
    EXPECT_EQ(smsBaseMessage->GetSmsSegmentsInfo(message, false, lenInfo), TELEPHONY_ERR_SUCCESS);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_GsmSmsReceiveHandler_0001
 * @tc.name     Test GsmSmsReceiveHandler
 * @tc.desc     Function test
 */
HWTEST_F(BranchSmsTest, GsmSmsReceiveHandler_0001, Function | MediumTest | Level1)
{
    auto gsmSmsReceiveHandler = std::make_shared<GsmSmsReceiveHandler>(1);
    gsmSmsReceiveHandler->UnRegisterHandler();
    EXPECT_NE(gsmSmsReceiveHandler->HandleSmsByType(nullptr), TELEPHONY_ERR_SUCCESS);
    EXPECT_EQ(gsmSmsReceiveHandler->TransformMessageInfo(nullptr), nullptr);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_GsmSmsSender_0001
 * @tc.name     Test GsmSmsSender
 * @tc.desc     Function test
 */
HWTEST_F(BranchSmsTest, GsmSmsSender_0001, Function | MediumTest | Level1)
{
    std::function<void(std::shared_ptr<SmsSendIndexer>)> sendRetryFun = nullptr;
    auto gsmSmsSender = std::make_shared<GsmSmsSender>(INVALID_SLOTID, sendRetryFun);
    uint8_t *data = (uint8_t *)malloc(VALUE_LENGTH * sizeof(uint8_t));
    std::string dataStr = "123";
    gsmSmsSender->CharArrayToString(data, 1, dataStr);
    std::shared_ptr<SmsSendIndexer> smsIndexer = nullptr;
    const sptr<ISendShortMessageCallback> sendCallback =
        iface_cast<ISendShortMessageCallback>(new SendShortMessageCallbackStub());
    const sptr<IDeliveryShortMessageCallback> deliveryCallback =
        iface_cast<IDeliveryShortMessageCallback>(new DeliveryShortMessageCallbackStub());
    const std::string desAddr = "qwe";
    const std::string scAddr = "123";
    const std::string text = "123";
    std::shared_ptr<struct EncodeInfo> encodeInfo = nullptr;
    gsmSmsSender->SendSmsToRil(smsIndexer);
    gsmSmsSender->ResendTextDelivery(smsIndexer);
    gsmSmsSender->ResendDataDelivery(smsIndexer);
    gsmSmsSender->SetSendIndexerInfo(smsIndexer, encodeInfo, 1);
    smsIndexer = std::make_shared<SmsSendIndexer>(desAddr, scAddr, text, sendCallback, deliveryCallback);
    gsmSmsSender->ResendTextDelivery(smsIndexer);
    gsmSmsSender->voiceServiceState_ = static_cast<int32_t>(RegServiceState::REG_STATE_IN_SERVICE);
    gsmSmsSender->imsSmsCfg_ = 0;
    gsmSmsSender->SendSmsToRil(smsIndexer);
    smsIndexer->psResendCount_ = DIGIT_LEN;
    gsmSmsSender->SendSmsToRil(smsIndexer);
    AppExecFwk::InnerEvent::Pointer event = AppExecFwk::InnerEvent::Get(0, 1);
    gsmSmsSender->StatusReportAnalysis(event);
    gsmSmsSender->StatusReportGetImsSms(event);
    event = nullptr;
    gsmSmsSender->StatusReportSetImsSms(event);
    gsmSmsSender->StatusReportAnalysis(event);
    gsmSmsSender->StatusReportGetImsSms(event);
    gsmSmsSender->isImsGsmHandlerRegistered = true;
    gsmSmsSender->RegisterImsHandler();
    gsmSmsSender->SetSendIndexerInfo(smsIndexer, encodeInfo, 1);
    GsmSimMessageParam smsData;
    smsIndexer->hasMore_ = true;
    gsmSmsSender->SendCsSms(smsIndexer, smsData);
    smsIndexer->csResendCount_ = 1;
    gsmSmsSender->SendCsSms(smsIndexer, smsData);
    GsmSmsMessage gsmSmsMessage;
    bool isMore = true;
    smsIndexer->isConcat_ = true;
    EXPECT_FALSE(gsmSmsSender->SetPduInfo(nullptr, gsmSmsMessage, isMore));
    EXPECT_TRUE(gsmSmsSender->SetPduInfo(smsIndexer, gsmSmsMessage, isMore));
    smsIndexer->smsConcat_.totalSeg = VALUE_LENGTH;
    EXPECT_TRUE(gsmSmsSender->SetPduInfo(smsIndexer, gsmSmsMessage, isMore));
}

/**
 * @tc.number   Telephony_SmsMmsGtest_SatelliteSmsCallback_0001
 * @tc.name     Test SatelliteSms Callback
 * @tc.desc     Function test
 */
HWTEST_F(BranchSmsTest, SatelliteSmsCallback_0001, Function | MediumTest | Level1)
{
    std::function<void(std::shared_ptr<SmsSendIndexer>)> fun = nullptr;
    auto gsmSmsSender = std::make_shared<GsmSmsSender>(INVALID_SLOTID, fun);
    SatelliteSmsCallback callback(gsmSmsSender);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(u"");
    data.WriteInt32(0);
    EXPECT_EQ(callback.OnRemoteRequest(0, data, reply, option), TELEPHONY_ERR_DESCRIPTOR_MISMATCH);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_SatelliteSmsCallback_0002
 * @tc.name     Test SatelliteSms Callback
 * @tc.desc     Function test
 */
HWTEST_F(BranchSmsTest, SatelliteSmsCallback_0002, Function | MediumTest | Level1)
{
    std::function<void(std::shared_ptr<SmsSendIndexer>)> fun = nullptr;
    auto gsmSmsSender = std::make_shared<GsmSmsSender>(INVALID_SLOTID, fun);
    SatelliteSmsCallback callback(gsmSmsSender);
    MessageParcel reply;
    MessageOption option;

    MessageParcel hrilData;
    hrilData.WriteInterfaceToken(INTERFACE_TOKEN);
    hrilData.WriteInt32(0);
    hrilData.WriteInt32(static_cast<int32_t>(SatelliteSmsResultType::HRIL_RADIO_RESPONSE));
    hrilData.WriteInt32(0);
    hrilData.WriteInt32(0);
    hrilData.WriteInt32(0);
    hrilData.WriteInt32(0);
    EXPECT_EQ(callback.OnRemoteRequest(0, hrilData, reply, option), TELEPHONY_SUCCESS);

    MessageParcel data;
    data.WriteInterfaceToken(INTERFACE_TOKEN);
    data.WriteInt32(0);
    data.WriteInt32(static_cast<int32_t>(SatelliteSmsResultType::SEND_SMS_RESULT));
    data.WriteInt32(0);
    data.WriteString("");
    data.WriteInt32(0);
    data.WriteInt64(0);
    EXPECT_EQ(callback.OnRemoteRequest(0, data, reply, option), TELEPHONY_SUCCESS);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_SatelliteSmsCallback_0003
 * @tc.name     Test SatelliteSms Callback
 * @tc.desc     Function test
 */
HWTEST_F(BranchSmsTest, SatelliteSmsCallback_0003, Function | MediumTest | Level1)
{
    std::function<void(std::shared_ptr<SmsSendIndexer>)> fun = nullptr;
    auto gsmSmsSender = std::make_shared<GsmSmsSender>(INVALID_SLOTID, fun);
    SatelliteSmsCallback callback(gsmSmsSender);
    MessageParcel reply;
    MessageOption option;

    MessageParcel errData;
    errData.WriteInterfaceToken(INTERFACE_TOKEN);
    errData.WriteInt32(0);
    EXPECT_EQ(callback.OnRemoteRequest(1, errData, reply, option), TELEPHONY_ERR_READ_DATA_FAIL);

    MessageParcel data;
    data.WriteInterfaceToken(INTERFACE_TOKEN);
    data.WriteInt32(0);
    data.WriteInt32(0);
    data.WriteInt32(0);
    std::vector<uint8_t> pdu {};
    data.WriteUInt8Vector(pdu);
    EXPECT_EQ(callback.OnRemoteRequest(1, data, reply, option), TELEPHONY_SUCCESS);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_SatelliteSmsCallback_0004
 * @tc.name     Test SatelliteSms Callback
 * @tc.desc     Function test
 */
HWTEST_F(BranchSmsTest, SatelliteSmsCallback_0004, Function | MediumTest | Level1)
{
    std::function<void(std::shared_ptr<SmsSendIndexer>)> fun = nullptr;
    auto gsmSmsSender = std::make_shared<GsmSmsSender>(INVALID_SLOTID, fun);
    SatelliteSmsCallback callback(gsmSmsSender);
    MessageParcel reply;
    MessageOption option;

    MessageParcel errData;
    errData.WriteInterfaceToken(INTERFACE_TOKEN);
    errData.WriteInt32(0);
    EXPECT_EQ(callback.OnRemoteRequest(2, errData, reply, option), TELEPHONY_ERR_READ_DATA_FAIL);

    MessageParcel data;
    data.WriteInterfaceToken(INTERFACE_TOKEN);
    data.WriteInt32(0);
    data.WriteInt32(0);
    data.WriteInt32(0);
    std::vector<uint8_t> pdu {};
    data.WriteUInt8Vector(pdu);
    EXPECT_EQ(callback.OnRemoteRequest(2, data, reply, option), TELEPHONY_SUCCESS);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_SatelliteSmsClient_0001
 * @tc.name     Test Satellite Service Client
 * @tc.desc     Function test
 */
HWTEST_F(BranchSmsTest, SatelliteSmsClient_0001, Function | MediumTest | Level1)
{
    TELEPHONY_LOGI("SatelliteSmsClient_0001==========");
    std::function<void(std::shared_ptr<SmsSendIndexer>)> fun = nullptr;
    auto gsmSmsSender = std::make_shared<GsmSmsSender>(INVALID_SLOTID, fun);
    auto receiveHandler = std::make_shared<GsmSmsReceiveHandler>(INVALID_SLOTID);
    auto &satelliteSmsClient = SatelliteSmsClient::GetInstance();
    satelliteSmsClient.AddSendHandler(INVALID_SLOTID, std::static_pointer_cast<TelEventHandler>(gsmSmsSender));
    satelliteSmsClient.AddReceiveHandler(INVALID_SLOTID, std::static_pointer_cast<TelEventHandler>(receiveHandler));
    satelliteSmsClient.statusChangeListener_->OnAddSystemAbility(INVALID_SLOTID, "");
    satelliteSmsClient.statusChangeListener_->OnRemoveSystemAbility(INVALID_SLOTID, "");
    EXPECT_GE(satelliteSmsClient.GetSatelliteSupported(), 0);
    EXPECT_GE(satelliteSmsClient.IsSatelliteEnabled(), 0);
    EXPECT_GE(satelliteSmsClient.GetSatelliteCapability(), 0);
    SatelliteMessage message;
    EXPECT_EQ(satelliteSmsClient.SendSms(INVALID_SLOTID, RadioEvent::RADIO_SEND_SMS, message),
        TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL);
    EXPECT_EQ(satelliteSmsClient.SendSmsMoreMode(INVALID_SLOTID, RadioEvent::RADIO_SEND_SMS_EXPECT_MORE, message),
        TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL);
    EXPECT_EQ(
        satelliteSmsClient.SendSmsAck(INVALID_SLOTID, SMS_EVENT_NEW_SMS_REPLY, true, AckIncomeCause::SMS_ACK_RESULT_OK),
        TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_SmsMiscManager_0001
 * @tc.name     Test SmsMiscManager
 * @tc.desc     Function test
 */
HWTEST_F(BranchSmsTest, SmsMiscManager_0001, Function | MediumTest | Level1)
{
    auto smsMiscManager = std::make_shared<SmsMiscManager>(INVALID_SLOTID);
    AppExecFwk::InnerEvent::Pointer event = AppExecFwk::InnerEvent::Get(SmsMiscManager::SET_CB_CONFIG_FINISH, 1);
    smsMiscManager->ProcessEvent(event);
    event = AppExecFwk::InnerEvent::Get(SmsMiscManager::SET_SMSC_ADDR_FINISH, 1);
    smsMiscManager->ProcessEvent(event);
    event = AppExecFwk::InnerEvent::Get(TELEPHONY_ERR_STRCPY_FAIL, 1);
    smsMiscManager->ProcessEvent(event);
    event = nullptr;
    smsMiscManager->ProcessEvent(event);
    smsMiscManager->NotifyHasResponse();
    smsMiscManager->fairList_.push_back(1);
    smsMiscManager->NotifyHasResponse();
    EXPECT_EQ(smsMiscManager->SetCBConfig(true, CODE_BUFFER_MAX_SIZE, 1, 1), TELEPHONY_ERR_ARGUMENT_INVALID);
    EXPECT_EQ(smsMiscManager->SetCBConfig(true, 1, 0, 1), TELEPHONY_ERR_ARGUMENT_INVALID);
    EXPECT_EQ(smsMiscManager->SetCBConfig(true, 1, 1, 0), TELEPHONY_ERR_ARGUMENT_INVALID);
    EXPECT_EQ(smsMiscManager->SetCBConfig(true, 1, 0, 0), TELEPHONY_ERR_ARGUMENT_INVALID);
    EXPECT_EQ(smsMiscManager->SetCBConfig(false, 1, 1, 1), TELEPHONY_ERR_SUCCESS);
    EXPECT_GE(smsMiscManager->SetCBConfig(true, 1, 1, 1), TELEPHONY_ERR_SUCCESS);
    EXPECT_TRUE(smsMiscManager->OpenCBRange(1, 1));
    smsMiscManager->rangeList_.clear();
    smsMiscManager->rangeList_.emplace_back(VALUE_LENGTH, 1);
    EXPECT_EQ(smsMiscManager->SetCBConfig(true, 1, 1, 1), TELEPHONY_ERR_RIL_CMD_FAIL);
    SmsMiscManager::infoData data(1, 1);
    std::string smsc = "";
    std::string pdu = "";
    std::vector<ShortMessage> message;
    EXPECT_GE(
        smsMiscManager->AddSimMessage(smsc, pdu, ISmsServiceInterface::SimMessageStatus::SIM_MESSAGE_STATUS_UNREAD), 0);
    EXPECT_GE(smsMiscManager->UpdateSimMessage(
                  1, ISmsServiceInterface::SimMessageStatus::SIM_MESSAGE_STATUS_UNREAD, pdu, smsc),
        0);
    EXPECT_GT(smsMiscManager->GetAllSimMessages(message), TELEPHONY_ERR_SUCCESS);
    std::list<SmsMiscManager::gsmCBRangeInfo> rangeList;
    EXPECT_TRUE(smsMiscManager->SendDataToRil(true, rangeList));
    SmsMiscManager::gsmCBRangeInfo rangeInfo(1, 1);
    SmsMiscManager::gsmCBRangeInfo rangeInfoTwo(1, 0);
    rangeList.push_back(rangeInfo);
    rangeList.push_back(rangeInfoTwo);
    EXPECT_NE(smsMiscManager->RangeListToString(rangeList), "");
    EXPECT_FALSE(smsMiscManager->SendDataToRil(true, rangeList));
}

/**
 * @tc.number   Telephony_SmsMmsGtest_SmsMiscManager_0003
 * @tc.name     Test SmsMiscManager
 * @tc.desc     Function test
 */
HWTEST_F(BranchSmsTest, SmsMiscManager_0003, Function | MediumTest | Level1)
{
    auto smsMiscManager = std::make_shared<SmsMiscManager>(0);
    AppExecFwk::InnerEvent::Pointer event = AppExecFwk::InnerEvent::Get(SmsMiscManager::GET_CB_CONFIG_FINISH, 1);
    smsMiscManager->ProcessEvent(event);
    std::shared_ptr<CBConfigInfo> res = std::make_shared<CBConfigInfo>();
    res->mids = CB_RANGE_MIDS;
    res->dcss = CB_RANGE_DCSS;
    smsMiscManager->UpdateCbRangList(res);
    smsMiscManager->rangeList_.emplace_back(1, 1);
    smsMiscManager->rangeList_.emplace_back(0, 1);
    smsMiscManager->rangeList_.emplace_back(0, 0);
    smsMiscManager->CombineCBRange();
    EXPECT_FALSE(smsMiscManager->SendDataToRil(true, smsMiscManager->rangeList_));
    std::string src = CB_RANGE_MIDS;
    std::vector<std::string> dest;
    std::string delimiter = CB_RANGE_DELI;
    smsMiscManager->SplitMids(src, dest, delimiter);
    std::string value = CB_RANGE_MID;
    std::string start;
    std::string end;
    std::string dlm = CB_RANGE_DELIM;
    smsMiscManager->SplitMidValue(value, start, end, dlm);
    smsMiscManager->hasGotCbRange_ = true;
    smsMiscManager->GetModemCBRange();
    smsMiscManager->hasGotCbRange_ = false;
    smsMiscManager->GetModemCBRange();
    EXPECT_FALSE(smsMiscManager->SendDataToRil(true, smsMiscManager->rangeList_));
}

/**
 * @tc.number   Telephony_SmsMmsGtest_SmsService_0001
 * @tc.name     Test SmsService
 * @tc.desc     Function test
 */
HWTEST_F(BranchSmsTest, SmsService_0001, Function | MediumTest | Level1)
{
    auto smsService = DelayedSingleton<SmsService>::GetInstance();
    smsService->state_ = ServiceRunningState::STATE_RUNNING;
    smsService->OnStart();
    std::u16string desAddr = u"";
    uint8_t *data = nullptr;
    sptr<ISendShortMessageCallback> sendCallback = nullptr;
    sptr<IDeliveryShortMessageCallback> deliveryCallback = nullptr;
    EXPECT_GT(
        smsService->SendMessage(0, desAddr, desAddr, desAddr, sendCallback, deliveryCallback), TELEPHONY_ERR_SUCCESS);
    EXPECT_GT(smsService->SendMessage(0, desAddr, desAddr, 1, data, 1, sendCallback, deliveryCallback),
        TELEPHONY_ERR_SUCCESS);
    bool isSupported = true;
    std::string sca = "";
    smsService->TrimSmscAddr(sca);
    sca = " 123";
    smsService->TrimSmscAddr(sca);
    EXPECT_GT(smsService->IsImsSmsSupported(INVALID_SLOTID, isSupported), TELEPHONY_ERR_SUCCESS);
    EXPECT_GT(smsService->GetImsShortMessageFormat(desAddr), TELEPHONY_ERR_SUCCESS);
    EXPECT_GT(smsService->SetSmscAddr(INVALID_SLOTID, desAddr), TELEPHONY_ERR_SUCCESS);
    EXPECT_GT(smsService->GetSmscAddr(INVALID_SLOTID, desAddr), TELEPHONY_ERR_SUCCESS);
    EXPECT_GT(smsService->AddSimMessage(
                  INVALID_SLOTID, desAddr, desAddr, ISmsServiceInterface::SimMessageStatus::SIM_MESSAGE_STATUS_UNREAD),
        TELEPHONY_ERR_SUCCESS);
    EXPECT_GT(smsService->DelSimMessage(INVALID_SLOTID, 1), TELEPHONY_ERR_SUCCESS);
    EXPECT_GT(smsService->UpdateSimMessage(INVALID_SLOTID, 1,
                  ISmsServiceInterface::SimMessageStatus::SIM_MESSAGE_STATUS_UNREAD, desAddr, desAddr),
        TELEPHONY_ERR_SUCCESS);
    std::vector<ShortMessage> message;
    EXPECT_GT(smsService->SetCBConfig(INVALID_SLOTID, true, 1, 1, 1), TELEPHONY_ERR_SUCCESS);
    EXPECT_GE(smsService->SetImsSmsConfig(INVALID_SLOTID, 1), TELEPHONY_ERR_SUCCESS);
    EXPECT_GT(smsService->SetDefaultSmsSlotId(INVALID_SLOTID), TELEPHONY_ERR_SUCCESS);
    std::vector<std::u16string> splitMessage;
    EXPECT_GT(smsService->SplitMessage(desAddr, splitMessage), TELEPHONY_ERR_SUCCESS);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_SmsService_0002
 * @tc.name     Test SmsService
 * @tc.desc     Function test
 */
HWTEST_F(BranchSmsTest, SmsService_0002, Function | MediumTest | Level1)
{
    auto smsService = DelayedSingleton<SmsService>::GetInstance();
    std::u16string message = u"";
    ISmsServiceInterface::SmsSegmentsInfo info;
    EXPECT_GT(smsService->GetSmsSegmentsInfo(INVALID_SLOTID, message, true, info), TELEPHONY_ERR_SUCCESS);
    message = u"123";
    EXPECT_GT(smsService->GetSmsSegmentsInfo(INVALID_SLOTID, message, true, info), TELEPHONY_ERR_SUCCESS);
    std::string scAddr = "";
    std::string specification = "";
    ShortMessage messages;
    EXPECT_GT(smsService->CreateMessage(scAddr, specification, messages), TELEPHONY_ERR_SUCCESS);
    specification = "3gpp";
    EXPECT_GE(smsService->CreateMessage(scAddr, specification, messages), TELEPHONY_ERR_SUCCESS);
    specification = "3gpp2";
    EXPECT_GE(smsService->CreateMessage(scAddr, specification, messages), TELEPHONY_ERR_SUCCESS);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_SmsService_0003
 * @tc.name     Test SmsService
 * @tc.desc     Function test
 */
HWTEST_F(BranchSmsTest, SmsService_0003, Function | MediumTest | Level1)
{
    auto smsNwPolicyManager = std::make_shared<SmsNetworkPolicyManager>(INVALID_SLOTID);
    AppExecFwk::InnerEvent::Pointer event = AppExecFwk::InnerEvent::Get(RadioEvent::RADIO_ON);
    smsNwPolicyManager->ProcessEvent(event);
    event = AppExecFwk::InnerEvent::Get(RadioEvent::RADIO_RIL_ADAPTER_HOST_DIED);
    smsNwPolicyManager->ProcessEvent(event);
    smsNwPolicyManager->HandleFactoryReset();

    auto smsService = DelayedSingleton<SmsService>::GetInstance();
    EXPECT_EQ(smsService->OnRilAdapterHostDied(INVALID_SLOTID), TELEPHONY_ERR_LOCAL_PTR_NULL);
    smsService->slotSmsInterfaceManagerMap_[INVALID_SLOTID] = std::make_shared<SmsInterfaceManager>(INVALID_SLOTID);
    EXPECT_EQ(smsService->OnRilAdapterHostDied(INVALID_SLOTID), TELEPHONY_ERR_SUCCESS);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_SmsPersistHelper_0001
 * @tc.name     Test SmsService
 * @tc.desc     Function test
 */
HWTEST_F(BranchSmsTest, SmsPersistHelper_0001, Function | MediumTest | Level1)
{
    AccessMmsToken token;
    auto smsPersistHelper = DelayedSingleton<SmsPersistHelper>::GetInstance();
    DataShare::DataSharePredicates predicates;
    uint16_t maxGroupId = 0;
    smsPersistHelper->QueryMaxGroupId(predicates, maxGroupId);
    EXPECT_GE(maxGroupId, 0);

    std::string num = "";
    std::string countryCode = "123";
    i18n::phonenumbers::PhoneNumberUtil::PhoneNumberFormat formatInfo =
        i18n::phonenumbers::PhoneNumberUtil::PhoneNumberFormat::NATIONAL;
    std::string formatNum = "";
    smsPersistHelper->FormatSmsNumber(num, countryCode, formatInfo, formatNum);
    num = "123";
    smsPersistHelper->FormatSmsNumber(num, countryCode, formatInfo, formatNum);
    formatNum = "123";
    int32_t value = smsPersistHelper->FormatSmsNumber(num, countryCode, formatInfo, formatNum);
    EXPECT_GE(value, 0);

    DataShare::DataShareValuesBucket bucket;
    std::string id = "1";
    bucket.Put(SLOT_ID, id);
    uint16_t dataBaseId = 0;
    smsPersistHelper->Insert(bucket, dataBaseId);
    smsPersistHelper->Insert(TABLE_URL, bucket);
    uint16_t sessionId = 0;
    uint16_t messageCount = 0;
    smsPersistHelper->QuerySession(predicates, sessionId, messageCount);
    EXPECT_GE(sessionId, 0);

    smsPersistHelper->Update(predicates, bucket);
    std::vector<SmsReceiveIndexer> indexers;
    smsPersistHelper->Query(predicates, indexers);
    smsPersistHelper->Delete(predicates);
    std::string phoneNum = "";
    smsPersistHelper->QueryBlockPhoneNumber(phoneNum);
    smsPersistHelper->UpdateContact(phoneNum);
    EXPECT_GE(phoneNum.size(), 0);

    phoneNum = "13866666666";
    smsPersistHelper->QueryBlockPhoneNumber(phoneNum);
    smsPersistHelper->UpdateContact(phoneNum);
    int32_t rawCountId = 1;
    int32_t contactedCount = 1;
    smsPersistHelper->QueryContactedCount(phoneNum, rawCountId, contactedCount);
    EXPECT_TRUE(smsPersistHelper != nullptr);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_SmsServiceManagerClient_0001
 * @tc.name     Test SmsServiceManagerClient
 * @tc.desc     Function test
 */
HWTEST_F(BranchSmsTest, SmsServiceManagerClient_0001, Function | MediumTest | Level1)
{
    int32_t slotId = 0;
    std::u16string desAddr = u"";
    sptr<ISendShortMessageCallback> sendCallback;
    sptr<IDeliveryShortMessageCallback> deliveryCallback;
    auto smsServiceManagerClient = DelayedSingleton<SmsServiceManagerClient>::GetInstance();
    int32_t ret = 0;
    ret = smsServiceManagerClient->SetDefaultSmsSlotId(slotId);
    EXPECT_GE(ret, 0);
    smsServiceManagerClient->GetDefaultSmsSlotId();
    smsServiceManagerClient->GetDefaultSmsSimId(slotId);
    smsServiceManagerClient->SendMessage(slotId, desAddr, desAddr, desAddr, sendCallback, deliveryCallback);
    uint16_t port = 1;
    uint8_t *data = nullptr;
    smsServiceManagerClient->SendMessage(slotId, desAddr, desAddr, port, data, port, sendCallback, deliveryCallback);
    std::u16string scAddr = u"1234";
    smsServiceManagerClient->SetScAddress(slotId, scAddr);
    smsServiceManagerClient->GetScAddress(slotId, scAddr);
    std::u16string smsc = u"test";
    smsServiceManagerClient->AddSimMessage(
        slotId, smsc, smsc, ISmsServiceInterface::SimMessageStatus::SIM_MESSAGE_STATUS_UNREAD);
    uint32_t msgIndex = 1;
    smsServiceManagerClient->DelSimMessage(slotId, msgIndex);
    smsServiceManagerClient->UpdateSimMessage(
        slotId, msgIndex, ISmsServiceInterface::SimMessageStatus::SIM_MESSAGE_STATUS_UNREAD, smsc, smsc);
    std::vector<ShortMessage> messages;
    smsServiceManagerClient->GetAllSimMessages(slotId, messages);
    bool enable = true;
    uint8_t ranType = 1;
    smsServiceManagerClient->SetCBConfig(slotId, enable, msgIndex, msgIndex, ranType);
    smsServiceManagerClient->SetImsSmsConfig(slotId, enable);
    std::vector<std::u16string> splitMessage;
    smsServiceManagerClient->SplitMessage(desAddr, splitMessage);
    ISmsServiceInterface::SmsSegmentsInfo segInfo;
    smsServiceManagerClient->GetSmsSegmentsInfo(slotId, desAddr, enable, segInfo);
    smsServiceManagerClient->IsImsSmsSupported(slotId, enable);
    smsServiceManagerClient->GetImsShortMessageFormat(desAddr);
    smsServiceManagerClient->HasSmsCapability();
    std::string pdu = "";
    ShortMessage message;
    smsServiceManagerClient->CreateMessage(pdu, pdu, message);
    smsServiceManagerClient->GetBase64Encode(pdu, pdu);
    smsServiceManagerClient->GetBase64Decode(pdu, pdu);
    uint32_t charset = 1;
    smsServiceManagerClient->GetEncodeStringFunc(pdu, charset, charset, pdu);
    EXPECT_TRUE(smsServiceManagerClient != nullptr);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_GsmUserDataPdu_0001
 * @tc.name     Test GsmUserDataPdu
 * @tc.desc     Function test
 */
HWTEST_F(BranchSmsTest, GsmUserDataPdu_0001, Function | MediumTest | Level1)
{
    const struct SmsUDPackage *userData = nullptr;
    struct SmsUDPackage *pUserData = nullptr;
    auto encodeBuffer = std::make_shared<SmsWriteBuffer>();
    auto gsmUserDataPdu = DelayedSingleton<GsmUserDataPdu>::GetInstance();
    std::string str;
    gsmUserDataPdu->EncodeUserDataPdu(*encodeBuffer, userData, DataCodingScheme::DATA_CODING_7BIT, str);
    auto deBuffer = std::make_shared<SmsReadBuffer>(SMS_READ_PDU);
    gsmUserDataPdu->DecodeUserDataPdu(*deBuffer, true, DataCodingScheme::DATA_CODING_7BIT, pUserData);
    SmsTpud *pTPUD = new SmsTpud();
    gsmUserDataPdu->DecodeUserDataPdu(*deBuffer, true, DataCodingScheme::DATA_CODING_7BIT, pUserData, pTPUD);
    delete pTPUD;
    pTPUD = nullptr;
    pUserData = new SmsUDPackage();
    gsmUserDataPdu->ResetUserData(*pUserData);
    auto smsReadBuffer = std::make_shared<SmsReadBuffer>(SMS_READ_PDU);
    uint8_t udhl = 0;
    uint16_t i = 0;
    gsmUserDataPdu->GetHeaderCnt(*smsReadBuffer, pUserData, udhl, i);
    delete pUserData;
    pUserData = nullptr;
    SmsWriteBuffer buffer;
    uint8_t v = 1;
    buffer.WriteByte(v);
    SmsUDH header;
    header.udhType = UDH_CONCAT_16BIT;
    SmsUDH *pHeader = new SmsUDH();
    uint16_t headerLen = 0;
    auto decodeBuffer = std::make_shared<SmsReadBuffer>(SMS_READ_PDU);
    decodeBuffer->data_.reset(new uint8_t[DECODE_SIZE]());
    decodeBuffer->data_[0] = 0;
    decodeBuffer->data_[1] = EIGHT_BIT;
    decodeBuffer->data_[TWO_BIT] = FOUR_BIT;
    decodeBuffer->data_[DIGIT_LEN] = FIVE_BIT;
    decodeBuffer->data_[FOUR_BIT] = SIX_BIT;
    decodeBuffer->index_ = 0;
    gsmUserDataPdu->DecodeHeader(*decodeBuffer, *pHeader, headerLen);
    decodeBuffer->index_ = 1;
    gsmUserDataPdu->DecodeHeader(*decodeBuffer, *pHeader, headerLen);
    decodeBuffer->index_ = TWO_BIT;
    gsmUserDataPdu->DecodeHeader(*decodeBuffer, *pHeader, headerLen);
    decodeBuffer->index_ = DIGIT_LEN;
    gsmUserDataPdu->DecodeHeader(*decodeBuffer, *pHeader, headerLen);
    decodeBuffer->index_ = FOUR_BIT;
    gsmUserDataPdu->DecodeHeader(*decodeBuffer, *pHeader, headerLen);
    delete pHeader;
    pHeader = nullptr;
    EXPECT_TRUE(decodeBuffer != nullptr);
    EXPECT_TRUE(gsmUserDataPdu != nullptr);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_GsmUserDataPdu_0002
 * @tc.name     Test GsmUserDataPdu
 * @tc.desc     Function test
 */
HWTEST_F(BranchSmsTest, GsmUserDataPdu_0002, Function | MediumTest | Level1)
{
    SmsUDH *pHeader = new SmsUDH();
    auto decodeBuffer = std::make_shared<SmsReadBuffer>(SMS_READ_PDU);
    auto gsmUserDataPdu = DelayedSingleton<GsmUserDataPdu>::GetInstance();
    uint8_t oneByte = UDH_ALTERNATE_REPLY_ADDRESS;
    gsmUserDataPdu->DecodeHeaderPartData(*decodeBuffer, *pHeader, oneByte);
    oneByte = UDH_SINGLE_SHIFT;
    gsmUserDataPdu->DecodeHeaderPartData(*decodeBuffer, *pHeader, oneByte);
    oneByte = UDH_LOCKING_SHIFT;
    gsmUserDataPdu->DecodeHeaderPartData(*decodeBuffer, *pHeader, oneByte);
    gsmUserDataPdu->DecodeHeaderConcat8Bit(*decodeBuffer, *pHeader);
    decodeBuffer->data_[0] = 0;
    gsmUserDataPdu->DecodeHeaderConcat8Bit(*decodeBuffer, *pHeader);
    decodeBuffer->index_ = 0;
    gsmUserDataPdu->DecodeHeaderConcat8Bit(*decodeBuffer, *pHeader);
    decodeBuffer->data_[1] = 1;
    decodeBuffer->index_ = 1;
    gsmUserDataPdu->DecodeHeaderConcat8Bit(*decodeBuffer, *pHeader);
    auto sixteenDecodeBuffer = std::make_shared<SmsReadBuffer>(SMS_READ_PDU);
    gsmUserDataPdu->DecodeHeaderConcat16Bit(*sixteenDecodeBuffer, *pHeader);
    gsmUserDataPdu->DecodeHeaderAppPort8Bit(*sixteenDecodeBuffer, *pHeader);
    sixteenDecodeBuffer->data_[0] = 0;
    gsmUserDataPdu->DecodeHeaderAppPort8Bit(*sixteenDecodeBuffer, *pHeader);
    sixteenDecodeBuffer->index_ = 0;
    gsmUserDataPdu->DecodeHeaderAppPort8Bit(*sixteenDecodeBuffer, *pHeader);
    decodeBuffer->data_[1] = 1;
    decodeBuffer->index_ = 1;
    EXPECT_TRUE(decodeBuffer != nullptr);
    gsmUserDataPdu->DecodeHeaderAppPort8Bit(*sixteenDecodeBuffer, *pHeader);
    auto appPortDecodeBuffer = std::make_shared<SmsReadBuffer>(SMS_READ_PDU);
    gsmUserDataPdu->DecodeHeaderAppPort16Bit(*appPortDecodeBuffer, *pHeader);
    gsmUserDataPdu->DecodeHeaderReplyAddress(*appPortDecodeBuffer, *pHeader);
    appPortDecodeBuffer->data_[0] = 0;
    gsmUserDataPdu->DecodeHeaderReplyAddress(*appPortDecodeBuffer, *pHeader);
    appPortDecodeBuffer->index_ = 0;
    gsmUserDataPdu->DecodeHeaderReplyAddress(*appPortDecodeBuffer, *pHeader);
    appPortDecodeBuffer->data_[1] = 1;
    appPortDecodeBuffer->index_ = 1;
    EXPECT_TRUE(appPortDecodeBuffer != nullptr);
    gsmUserDataPdu->DecodeHeaderReplyAddress(*appPortDecodeBuffer, *pHeader);
    auto singleShiftDecodeBuffer = std::make_shared<SmsReadBuffer>(SMS_READ_PDU);
    gsmUserDataPdu->DecodeHeaderSingleShift(*singleShiftDecodeBuffer, *pHeader);
    gsmUserDataPdu->DecodeHeaderLockingShift(*singleShiftDecodeBuffer, *pHeader);
    gsmUserDataPdu->DecodeHeaderDefaultCase(*singleShiftDecodeBuffer, *pHeader);
    delete pHeader;
    pHeader = nullptr;
    EXPECT_TRUE(gsmUserDataPdu != nullptr);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_GsmSmsSender_0002
 * @tc.name     Test GsmSmsSender
 * @tc.desc     Function test
 */
HWTEST_F(BranchSmsTest, GsmSmsSender_0002, Function | MediumTest | Level1)
{
    std::function<void(std::shared_ptr<SmsSendIndexer>)> sendRetryFun = nullptr;
    auto gsmSmsSender = std::make_shared<GsmSmsSender>(INVALID_SLOTID, sendRetryFun);
    GsmSimMessageParam smsData;
    smsData.refId = 1;
    smsData.smscPdu = SMS_READ_PDU;
    smsData.pdu = SMS_READ_PDU;
    const std::shared_ptr<SmsSendIndexer> indexer = nullptr;
    gsmSmsSender->SendImsSms(indexer, smsData);
    EXPECT_TRUE(gsmSmsSender != nullptr);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_GsmSmsMessage_0003
 * @tc.name     Test GsmSmsMessage
 * @tc.desc     Function test
 */
HWTEST_F(BranchSmsTest, GsmSmsMessage_0003, Function | MediumTest | Level1)
{
    auto gsmSmsMessage = std::make_shared<GsmSmsMessage>();
    const std::string replyAddress = "";
    gsmSmsMessage->CalcReplyEncodeAddress(replyAddress);
    gsmSmsMessage->CalcReplyEncodeAddress(SMS_READ_PDU);
    gsmSmsMessage->CreateStatusReportSmsTpdu();
    gsmSmsMessage->IsSpecialMessage();
    gsmSmsMessage->GetIsSIMDataTypeDownload();
    gsmSmsMessage->GetIsTypeZeroInd();
    EXPECT_TRUE(gsmSmsMessage->GetGsm());
    gsmSmsMessage->GetIsSmsText();
    gsmSmsMessage->GetDestAddress();
    gsmSmsMessage->GetFullText();
    gsmSmsMessage->SetDestPort(INVALID_SLOTID);
    EXPECT_TRUE(gsmSmsMessage != nullptr);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_GsmSmsParamDecode_0001
 * @tc.name     Test GsmSmsParamDecode
 * @tc.desc     Function test
 */
HWTEST_F(BranchSmsTest, GsmSmsParamDecode_0001, Function | MediumTest | Level1)
{
    auto gsmSmsParamDecode = std::make_shared<GsmSmsParamDecode>();
    auto buffer = std::make_shared<SmsReadBuffer>(SMS_READ_PDU);
    AddressNumber *pAddress = new AddressNumber();
    uint8_t bcdLen = 1;
    uint8_t addrLen = 1;
    gsmSmsParamDecode->DecodeAddressAlphaNum(*buffer, pAddress, bcdLen, addrLen);
    SmsDcs *smsDcs = nullptr;
    gsmSmsParamDecode->DecodeDcsClassGroupPdu(bcdLen, smsDcs);
    gsmSmsParamDecode->DecodeDcsDiscardGroupPdu(bcdLen, smsDcs);
    gsmSmsParamDecode->DecodeDcsStoreGsmGroupPdu(bcdLen, smsDcs);
    gsmSmsParamDecode->DecodeDcsStoreUCS2GroupPdu(bcdLen, smsDcs);
    gsmSmsParamDecode->DecodeDcsUnknownGroupPdu(bcdLen, smsDcs);
    enum SmsIndicatorType ret = gsmSmsParamDecode->GetMsgIndicatorType(bcdLen);
    EXPECT_GE(ret, 0);
    SmsDcs *mwiTypeSmsDcs = new SmsDcs();
    gsmSmsParamDecode->GetMwiType(bcdLen, *mwiTypeSmsDcs);
    EXPECT_TRUE(gsmSmsParamDecode != nullptr);
    int32_t slotId = 0;
    auto gsmSmsCbHandler = std::make_shared<GsmSmsCbHandler>(slotId);
    std::shared_ptr<GsmCbCodec> cbMessage = nullptr;
    gsmSmsCbHandler->RemoveCbMessageFromList(cbMessage);
    delete pAddress;
    pAddress = nullptr;
    delete mwiTypeSmsDcs;
    mwiTypeSmsDcs = nullptr;
    EXPECT_TRUE(gsmSmsCbHandler != nullptr);
    auto gsmSmsReceiveHandler = std::make_shared<GsmSmsReceiveHandler>(slotId);
    gsmSmsReceiveHandler->CheckSmsSupport();
    std::shared_ptr<SmsBaseMessage> smsBaseMessage = nullptr;
    gsmSmsReceiveHandler->HandleNormalSmsByType(smsBaseMessage);
    EXPECT_TRUE(gsmSmsReceiveHandler != nullptr);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_SmsService_0004
 * @tc.name     Test SmsService
 * @tc.desc     Function test
 */
HWTEST_F(BranchSmsTest, SmsService_0004, Function | MediumTest | Level1)
{
    int32_t slotId = 0;
    std::u16string desAddr = u"";
    sptr<ISendShortMessageCallback> sendCallback;
    sptr<IDeliveryShortMessageCallback> deliveryCallback;
    auto smsService = DelayedSingleton<SmsService>::GetInstance();
    AccessMmsToken token;
    smsService->SendMessage(slotId, desAddr, desAddr, desAddr, sendCallback, deliveryCallback);
    uint16_t port = 1;
    uint8_t *data = nullptr;
    smsService->SendMessage(slotId, desAddr, desAddr, port, data, port, sendCallback, deliveryCallback);
    slotId = -1;
    desAddr = u"test";
    smsService->SendMessage(slotId, desAddr, desAddr, desAddr, sendCallback, deliveryCallback);
    smsService->SendMessage(slotId, desAddr, desAddr, port, data, port, sendCallback, deliveryCallback);
    slotId = 0;
    std::string telephone = "13888888888";
    smsService->InsertSessionAndDetail(slotId, telephone, telephone);
    bool isSupported = false;
    slotId = -1;
    smsService->IsImsSmsSupported(slotId, isSupported);
    std::u16string format = u"";
    smsService->GetImsShortMessageFormat(format);
    smsService->HasSmsCapability();
    int32_t setSmscRes = 0;
    setSmscRes = smsService->SetSmscAddr(slotId, desAddr);
    desAddr = u" test";
    string sca = StringUtils::ToUtf8(desAddr);
    smsService->TrimSmscAddr(sca);
    desAddr = u"test ";
    sca = StringUtils::ToUtf8(desAddr);
    smsService->TrimSmscAddr(sca);
    int32_t smscRes = 0;
    smscRes = smsService->GetSmscAddr(slotId, desAddr);
    EXPECT_GE(setSmscRes, TELEPHONY_ERR_SLOTID_INVALID);
    EXPECT_GE(smscRes, TELEPHONY_ERR_ARGUMENT_INVALID);
    EXPECT_TRUE(smsService != nullptr);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_SmsService_0005
 * @tc.name     Test SmsService
 * @tc.desc     Function test
 */
HWTEST_F(BranchSmsTest, SmsService_0005, Function | MediumTest | Level1)
{
    int32_t slotId = 0;
    std::u16string desAddr = u"";
    sptr<ISendShortMessageCallback> sendCallback;
    sptr<IDeliveryShortMessageCallback> deliveryCallback;
    auto smsService = DelayedSingleton<SmsService>::GetInstance();
    AccessMmsToken token;
    smsService->AddSimMessage(
        slotId, desAddr, desAddr, ISmsServiceInterface::SimMessageStatus::SIM_MESSAGE_STATUS_UNREAD);
    uint32_t msgIndex = 1;
    smsService->DelSimMessage(slotId, msgIndex);
    smsService->UpdateSimMessage(
        slotId, msgIndex, ISmsServiceInterface::SimMessageStatus::SIM_MESSAGE_STATUS_UNREAD, desAddr, desAddr);
    smsService->CheckSimMessageIndexValid(slotId, msgIndex);
    uint32_t fromMsgId = 1;
    uint32_t toMsgId = 1;
    uint8_t netType = 1;
    smsService->SetCBConfig(slotId, true, fromMsgId, toMsgId, netType);
    int32_t enable = 1;
    smsService->SetImsSmsConfig(slotId, enable);
    smsService->SetDefaultSmsSlotId(slotId);
    smsService->GetDefaultSmsSlotId();
    smsService->GetDefaultSmsSimId(slotId);
    std::u16string message = u"";
    ISmsServiceInterface::SmsSegmentsInfo info;
    std::vector<std::u16string> splitMessage;
    bool text = true;
    int32_t splitRes = 0;
    int32_t smsRes = 0;
    splitRes = smsService->SplitMessage(message, splitMessage);
    smsRes = smsService->GetSmsSegmentsInfo(slotId, message, text, info);
    message = u"text";
    splitRes = smsService->SplitMessage(message, splitMessage);
    smsRes = smsService->GetSmsSegmentsInfo(slotId, message, text, info);
    smsService->GetServiceRunningState();
    smsService->GetEndTime();
    smsService->GetSpendTime();
    EXPECT_GE(splitRes, TELEPHONY_ERR_ARGUMENT_INVALID);
    EXPECT_GE(smsRes, TELEPHONY_ERR_ARGUMENT_INVALID);
    EXPECT_TRUE(smsService != nullptr);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_SmsPersistHelper_0002
 * @tc.name     Test SmsPersistHelper
 * @tc.desc     Function test
 */
HWTEST_F(BranchSmsTest, SmsPersistHelper_0002, Function | MediumTest | Level1)
{
    std::string formatNum = "";
    auto smsPersistHelper = DelayedSingleton<SmsPersistHelper>::GetInstance();
    std::string num = "";
    std::string cbnNumTemp = "";
    cbnNumTemp.assign(CBN_NUM);
    num = cbnNumTemp.substr(DIGIT_LEN, NUM_LENGTH);
    smsPersistHelper->CbnFormat(num, i18n::phonenumbers::PhoneNumberUtil::PhoneNumberFormat::NATIONAL, formatNum);
    EXPECT_TRUE(formatNum == num);
    smsPersistHelper->CbnFormat(num, i18n::phonenumbers::PhoneNumberUtil::PhoneNumberFormat::INTERNATIONAL, formatNum);
    EXPECT_TRUE(formatNum == cbnNumTemp);
    smsPersistHelper->CbnFormat(
        cbnNumTemp, i18n::phonenumbers::PhoneNumberUtil::PhoneNumberFormat::INTERNATIONAL, formatNum);
    EXPECT_TRUE(formatNum == cbnNumTemp);
    smsPersistHelper->CbnFormat(
        cbnNumTemp, i18n::phonenumbers::PhoneNumberUtil::PhoneNumberFormat::NATIONAL, formatNum);
    EXPECT_TRUE(formatNum == num);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_SmsNetworkPolicyManager_0001
 * @tc.name     Test SmsNetworkPolicyManager
 * @tc.desc     Function test
 */
HWTEST_F(BranchSmsTest, SmsNetworkPolicyManager_0001, Function | MediumTest | Level1)
{
    auto smsNetworkPolicyManager = std::make_shared<SmsNetworkPolicyManager>(INVALID_SLOTID);
    smsNetworkPolicyManager->IsImsNetDomain();
    smsNetworkPolicyManager->GetVoiceServiceState();
    smsNetworkPolicyManager->NetworkRegister(nullptr);
    auto smsBaseMessage = std::make_shared<GsmSmsMessage>();
    smsBaseMessage->GetSmscAddr();
    smsBaseMessage->GetRawUserData();
    smsBaseMessage->GetRawWapPushUserData();
    smsBaseMessage->IsReplaceMessage();
    smsBaseMessage->IsCphsMwi();
    smsBaseMessage->IsMwiNotStore();
    smsBaseMessage->GetStatus();
    smsBaseMessage->IsSmsStatusReportMessage();
    smsBaseMessage->HasReplyPath();
    smsBaseMessage->GetMsgRef();
    smsBaseMessage->SetIndexOnSim(0);
    smsBaseMessage->GetIndexOnSim();
    auto smsInterfaceManager = std::make_shared<SmsInterfaceManager>(INVALID_SLOTID);
    smsInterfaceManager->DelSimMessage(0);
    auto smsWapPushBuffer = std::make_shared<SmsWapPushBuffer>();
    smsWapPushBuffer->DecodeExtensionMedia();
    smsWapPushBuffer->DecodeConstrainedEncoding();
    smsWapPushBuffer->MarkPosition();
    smsWapPushBuffer->UnMarkPosition();
    std::shared_ptr<CdmaSmsMessage> cdmaSmsMessage = std::make_shared<CdmaSmsMessage>();
    cdmaSmsMessage->GetCbInfo();
    cdmaSmsMessage->GetCMASResponseType();
    cdmaSmsMessage->GetSpecialSmsInd();
    cdmaSmsMessage->GetProtocolId();
    cdmaSmsMessage->IsReplaceMessage();
    cdmaSmsMessage->IsCphsMwi();
    EXPECT_TRUE(smsNetworkPolicyManager != nullptr);
    EXPECT_TRUE(smsBaseMessage != nullptr);
    EXPECT_TRUE(smsInterfaceManager != nullptr);
    EXPECT_TRUE(cdmaSmsMessage != nullptr);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_SmsNetworkPolicyManager_0004
 * @tc.name     Test SmsNetworkPolicyManager
 * @tc.desc     Function test
 */
HWTEST_F(BranchSmsTest, SmsNetworkPolicyManager_0004, Function | MediumTest | Level1)
{
    auto smsMiscManager = std::make_shared<SmsMiscManager>(INVALID_SLOTID);
    smsMiscManager->GetRangeInfo();
    smsMiscManager->IsEmpty();
    std::shared_ptr<SmsReceiveIndexer> indexer = std::make_shared<SmsReceiveIndexer>();
    indexer->SetVisibleAddress(CBN_NUM);
    indexer->SetMsgCount(0);
    indexer->SetMsgSeqId(0);
    indexer->SetOriginatingAddress(CBN_NUM);
    indexer->SetIsCdmaWapPdu(false);
    indexer->GetIsCdmaWapPdu();
    indexer->SetIsCdma(false);
    indexer->SetDestPort(0);
    indexer->SetTimestamp(0);
    indexer->SetPdu(PDU);
    indexer->SetRawWapPushUserData(CBN_NUM);
    const sptr<ISendShortMessageCallback> sendCallback =
        iface_cast<ISendShortMessageCallback>(new SendShortMessageCallbackStub());
    const sptr<IDeliveryShortMessageCallback> deliveryCallback =
        iface_cast<IDeliveryShortMessageCallback>(new DeliveryShortMessageCallbackStub());
    std::shared_ptr<SmsSendIndexer> smsIndexer =
        std::make_shared<SmsSendIndexer>(CBN_NUM, CBN_NUM, CBN_NUM, sendCallback, deliveryCallback);
    smsIndexer->SetEncodeSmca(PDU);
    smsIndexer->SetText(CBN_NUM);
    smsIndexer->SetDeliveryCallback(deliveryCallback);
    smsIndexer->SetSendCallback(sendCallback);
    smsIndexer->SetDestPort(0);
    smsIndexer->SetDestAddr(CBN_NUM);
    smsIndexer->SetSmcaAddr(CBN_NUM);
    smsIndexer->GetIsText();
    smsIndexer->GetErrorCode();
    smsIndexer->SetData(PDU);
    smsIndexer->SetAckPdu(PDU);
    smsIndexer->GetAckPdu();
    smsIndexer->SetMsgRefId64Bit(0);
    smsIndexer->SetIsConcat(true);
    EXPECT_TRUE(smsMiscManager != nullptr);
    EXPECT_TRUE(indexer != nullptr);
    EXPECT_TRUE(sendCallback != nullptr);
    EXPECT_TRUE(deliveryCallback != nullptr);
    EXPECT_TRUE(smsIndexer != nullptr);
}
} // namespace Telephony
} // namespace OHOS
