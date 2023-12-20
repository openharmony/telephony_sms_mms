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
#include "cdma_sms_transport_message.h"
#include "core_service_client.h"
#include "gtest/gtest.h"
#include "i_sms_service_interface.h"
#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "radio_event.h"
#include "sms_mms_gtest.h"
#include "sms_mms_test_helper.h"
#include "sms_service.h"
#include "sms_service_manager_client.h"
#include "string_utils.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
namespace {
sptr<ISmsServiceInterface> g_telephonyService = nullptr;
} // namespace
using namespace testing::ext;

class CdmaSmsGtest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    static sptr<ISmsServiceInterface> GetProxy();
    static bool HasSimCard(int32_t slotId)
    {
        bool hasSimCard = false;
        if (CoreServiceClient::GetInstance().GetProxy() == nullptr) {
            return hasSimCard;
        }
        CoreServiceClient::GetInstance().HasSimCard(slotId, hasSimCard);
        return hasSimCard;
    }
};

void CdmaSmsGtest::TearDownTestCase() {}

void CdmaSmsGtest::SetUp() {}

void CdmaSmsGtest::TearDown() {}

const int32_t DEFAULT_SIM_SLOT_ID_1 = 1;
const uint8_t HEX_CHAR_LEN = 2;
const uint8_t UNICODE_CHAR_LEN = 2;
const uint8_t VALUE_INDEX = 2;
const char *CDMA_PDU = "01010101";
const char *OTHER_CDMA_PDU = "111111";

void CdmaSmsGtest::SetUpTestCase()
{
    TELEPHONY_LOGI("SetUpTestCase slotId%{public}d", DEFAULT_SIM_SLOT_ID_1);
    g_telephonyService = GetProxy();
    if (g_telephonyService == nullptr) {
        return;
    }
    DelayedSingleton<SmsServiceManagerClient>::GetInstance()->ResetSmsServiceProxy();
    DelayedSingleton<SmsServiceManagerClient>::GetInstance()->InitSmsServiceProxy();
}

sptr<ISmsServiceInterface> CdmaSmsGtest::GetProxy()
{
    auto systemAbilityMgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (systemAbilityMgr == nullptr) {
        return nullptr;
    }
    sptr<IRemoteObject> remote = systemAbilityMgr->CheckSystemAbility(TELEPHONY_SMS_MMS_SYS_ABILITY_ID);
    if (remote) {
        sptr<ISmsServiceInterface> smsService = iface_cast<ISmsServiceInterface>(remote);
        return smsService;
    }
    return nullptr;
}

#ifndef TEL_TEST_UNSUPPORT
/**
 * @tc.number   Telephony_SmsMmsGtest_CdmaSmsMessage_0001
 * @tc.name     Test CdmaSmsMessage
 * @tc.desc     Function test
 */
HWTEST_F(CdmaSmsGtest, CdmaSmsMessage_0001, Function | MediumTest | Level1)
{
    TELEPHONY_LOGI("TelSMSMMSTest::CdmaSmsMessage_0001 -->");
    CdmaSmsMessage cdmaSmsMessage;
    std::string dest = "dest";
    std::string sc = "sc";
    std::string text = "text";
    int32_t port = 10;
    uint8_t *data;
    uint32_t dataLen = 10;
    std::string pdu = "01000B818176251308F4000007E8B0BCFD76E701";
    bool bStatusReport = false;
    DataCodingScheme codingScheme = DATA_CODING_7BIT;
    cdmaSmsMessage.CreateSubmitTransMsg(dest, sc, text, bStatusReport, codingScheme);
    cdmaSmsMessage.CreateSubmitTransMsg(dest, sc, port, data, dataLen, bStatusReport);
    cdmaSmsMessage.GreateTransMsg();
    cdmaSmsMessage.CovertEncodingType(codingScheme);
    cdmaSmsMessage.CreateMessage(pdu);
    cdmaSmsMessage.PduAnalysis(pdu);
    CdmaP2PMsg p2pMsg;
    cdmaSmsMessage.AnalysisP2pMsg(p2pMsg);
    cdmaSmsMessage.AnalsisDeliverMwi(p2pMsg);
    bool ret = cdmaSmsMessage.PduAnalysis("");
    EXPECT_EQ(false, ret);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_CdmaSmsReceiveHandler_0001
 * @tc.name     Test CdmaSmsReceiveHandler
 * @tc.desc     Function test
 */
HWTEST_F(CdmaSmsGtest, CdmaSmsReceiveHandler_0001, Function | MediumTest | Level1)
{
    TELEPHONY_LOGI("TelSMSMMSTest::CdmaSmsReceiveHandler_0001 -->");
    CdmaSmsReceiveHandler cdmaSmsReceiveHandler(DEFAULT_SIM_SLOT_ID);
    int32_t retInt = cdmaSmsReceiveHandler.HandleSmsByType(nullptr);
    EXPECT_EQ(AckIncomeCause::SMS_ACK_UNKNOWN_ERROR, retInt);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_CdmaSmsReceiveHandler_0002
 * @tc.name     Test CdmaSmsReceiveHandler
 * @tc.desc     Function test
 */
HWTEST_F(CdmaSmsGtest, CdmaSmsReceiveHandler_0002, Function | MediumTest | Level1)
{
    TELEPHONY_LOGI("TelSMSMMSTest::CdmaSmsReceiveHandler_0002 -->");
    auto cdmaSmsReceiveHandler = std::make_shared<CdmaSmsReceiveHandler>(DEFAULT_SIM_SLOT_ID);
    cdmaSmsReceiveHandler->Init();
    auto retMsg = std::make_shared<CdmaSmsMessage>();
    retMsg = CdmaSmsMessage::CreateMessage(CDMA_PDU);
    EXPECT_EQ(retMsg, nullptr);
    auto cdmaMsg = std::make_shared<CdmaSmsMessage>();
    cdmaMsg->transMsg_ = std::make_unique<struct CdmaTransportMsg>();
    cdmaMsg->transMsg_->type = CdmaTransportMsgType::BROADCAST;
    int32_t retInt = cdmaSmsReceiveHandler->HandleSmsByType(cdmaMsg);
    EXPECT_EQ(AckIncomeCause::SMS_ACK_RESULT_OK, retInt);
    cdmaMsg->transMsg_->type = CdmaTransportMsgType::P2P;
    retInt = cdmaSmsReceiveHandler->HandleSmsByType(cdmaMsg);
    EXPECT_EQ(AckIncomeCause::SMS_ACK_RESULT_OK, retInt);
    cdmaMsg->transMsg_->type = CdmaTransportMsgType::ACK;
    retInt = cdmaSmsReceiveHandler->HandleSmsByType(cdmaMsg);
    EXPECT_EQ(AckIncomeCause::SMS_ACK_RESULT_OK, retInt);
    cdmaMsg->transMsg_->type = CdmaTransportMsgType::RESERVED;
    retInt = cdmaSmsReceiveHandler->HandleSmsByType(cdmaMsg);
    EXPECT_EQ(AckIncomeCause::SMS_ACK_RESULT_OK, retInt);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_CdmaSmsReceiveHandler_0003
 * @tc.name     Test CdmaSmsReceiveHandler
 * @tc.desc     Function test
 */
HWTEST_F(CdmaSmsGtest, CdmaSmsReceiveHandler_0003, Function | MediumTest | Level1)
{
    TELEPHONY_LOGI("TelSMSMMSTest::CdmaSmsReceiveHandler_0003 -->");
    auto cdmaSmsReceiveHandler = std::make_shared<CdmaSmsReceiveHandler>(DEFAULT_SIM_SLOT_ID);
    int32_t retInt = cdmaSmsReceiveHandler->HandleSmsOtherSvcid(nullptr);
    EXPECT_NE(AckIncomeCause::SMS_ACK_RESULT_OK, retInt);
    auto retMsg = CdmaSmsMessage::CreateMessage(OTHER_CDMA_PDU);
    EXPECT_EQ(retMsg, nullptr);

    auto cdmaMsg = std::make_shared<CdmaSmsMessage>();
    cdmaSmsReceiveHandler->ReplySmsToSmsc(1, cdmaMsg);
    retInt = cdmaSmsReceiveHandler->HandleSmsOtherSvcid(cdmaMsg);
    EXPECT_NE(AckIncomeCause::SMS_ACK_RESULT_OK, retInt);
    cdmaMsg->smsConcat_ = std::make_shared<SmsConcat>();
    cdmaMsg->smsConcat_->is8Bits = true;
    cdmaMsg->smsConcat_->msgRef = 1;
    cdmaMsg->smsConcat_->seqNum = 1;
    cdmaMsg->smsConcat_->totalSeg = 1;
    retInt = cdmaSmsReceiveHandler->HandleSmsOtherSvcid(cdmaMsg);
    EXPECT_NE(AckIncomeCause::SMS_ACK_RESULT_OK, retInt);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_CdmaSmsReceiveHandler_0004
 * @tc.name     Test CdmaSmsReceiveHandler
 * @tc.desc     Function test
 */
HWTEST_F(CdmaSmsGtest, CdmaSmsReceiveHandler_0004, Function | MediumTest | Level1)
{
    TELEPHONY_LOGI("TelSMSMMSTest::CdmaSmsReceiveHandler_0004 -->");
    CdmaSmsReceiveHandler cdmaSmsReceiveHandler(DEFAULT_SIM_SLOT_ID);
    std::function<void(std::shared_ptr<SmsSendIndexer>)> fun = nullptr;
    auto smsSender = std::make_shared<CdmaSmsSender>(DEFAULT_SIM_SLOT_ID, fun);
    cdmaSmsReceiveHandler.SetCdmaSender(smsSender);
    auto info = std::make_shared<SmsMessageInfo>();
    auto ret = cdmaSmsReceiveHandler.TransformMessageInfo(info);
    EXPECT_EQ(ret, nullptr);
    cdmaSmsReceiveHandler.SendCBBroadcast(nullptr);
    auto cdmaMsg = std::make_shared<CdmaSmsMessage>();
    EXPECT_EQ(cdmaSmsReceiveHandler.SendCBBroadcast(cdmaMsg), true);
}

/**
 * @tc.number   Telephony_CdmaSmsGtest_SmsPduBuffer_0001
 * @tc.name     Test SmsPduBuffer
 * @tc.desc     Function test
 */
HWTEST_F(CdmaSmsGtest, SmsPduBuffer_0001, Function | MediumTest | Level1)
{
    auto buffer = std::make_shared<SmsPduBuffer>();
    EXPECT_TRUE(buffer->IsEmpty());
    EXPECT_EQ(buffer->GetIndex(), 0);
    EXPECT_FALSE(buffer->SetIndex(1));
    EXPECT_EQ(buffer->MoveForward(), 0);
    EXPECT_EQ(buffer->SkipBits(), 0);

    std::string pduHex = "00000210020000021002";
    std::string pdu = StringUtils::HexToString(pduHex);
    auto rBuffer = std::make_shared<SmsReadBuffer>(pdu);
    EXPECT_FALSE(rBuffer->IsEmpty());
    rBuffer->SetIndex(0);
    uint8_t v = 0;
    uint16_t v2 = 0;
    EXPECT_TRUE(rBuffer->ReadByte(v));
    EXPECT_TRUE(rBuffer->ReadWord(v2));
    EXPECT_TRUE(rBuffer->ReadBits(v));

    auto wBuffer = std::make_shared<SmsWriteBuffer>();
    EXPECT_FALSE(wBuffer->IsEmpty());
    wBuffer->SetIndex(0);
    v = 1;
    v2 = 0x1234;
    EXPECT_TRUE(wBuffer->WriteByte(v));
    EXPECT_TRUE(wBuffer->WriteWord(v2));
    EXPECT_TRUE(wBuffer->InsertByte(v, 1));
    EXPECT_TRUE(wBuffer->WriteBits(v));
}

/**
 * @tc.number   Telephony_CdmaSmsGtest_CdmaSmsTransportMessage_0001
 * @tc.name     Test CdmaSmsTransportMessage
 * @tc.desc     Function test
 */
HWTEST_F(CdmaSmsGtest, CdmaSmsTransportMessage_0001, Function | MediumTest | Level1)
{
    TELEPHONY_LOGI("TelSMSMMSTest::CdmaSmsTransportMessage_0001 -->");
    CdmaSmsMessage cdmaSmsMessage;
    std::string dest = "dest";
    std::string sc = "sc";
    std::string text = "text";
    bool bStatusReport = false;
    DataCodingScheme codingScheme = DATA_CODING_7BIT;
    std::unique_ptr<CdmaTransportMsg> transMsg =
        cdmaSmsMessage.CreateSubmitTransMsg(dest, sc, text, bStatusReport, codingScheme);

    std::unique_ptr<CdmaSmsTransportMessage> transportMessage =
        CdmaSmsTransportMessage::CreateTransportMessage(*transMsg.get());
    SmsWriteBuffer pduBuffer;
    EXPECT_NE(transportMessage, nullptr);
    EXPECT_NE(transportMessage->IsEmpty(), true);
    EXPECT_EQ(transportMessage->Encode(pduBuffer), true);
    std::unique_ptr<std::vector<uint8_t>> pdu = pduBuffer.GetPduBuffer();
    EXPECT_NE(pdu, nullptr);
    EXPECT_GT(pdu->size(), static_cast<uint32_t>(0));
}

/**
 * @tc.number   Telephony_CdmaSmsGtest_CdmaSmsTransportMessage_0002
 * @tc.name     Test CdmaSmsTransportMessage
 * @tc.desc     Function test
 */
HWTEST_F(CdmaSmsGtest, CdmaSmsTransportMessage_0002, Function | MediumTest | Level1)
{
    TELEPHONY_LOGI("TelSMSMMSTest::CdmaSmsTransportMessage_0002 -->");
    std::string pduHex = "0000021002040702C48D159E268406010408260003200640011910D61C58F265CD9F469D5AF66DDDBF871E5CFA75E"
                         "DDFC79F400801000A0140";
    std::string pdu = StringUtils::HexToString(pduHex);
    SmsReadBuffer pduBuffer(pdu);
    CdmaTransportMsg msg;
    memset_s(&msg, sizeof(msg), 0, sizeof(msg));
    std::unique_ptr<CdmaSmsTransportMessage> transportMessage =
        CdmaSmsTransportMessage::CreateTransportMessage(msg, pduBuffer);
    EXPECT_NE(transportMessage, nullptr);
    EXPECT_NE(transportMessage->IsEmpty(), true);
    EXPECT_EQ(transportMessage->Decode(pduBuffer), true);
}

/**
 * @tc.number   Telephony_CdmaSmsGtest_CdmaSmsTransportMessage_0003
 * @tc.name     Test CdmaSmsTransportMessage
 * @tc.desc     Function test
 */
HWTEST_F(CdmaSmsGtest, CdmaSmsTransportMessage_0003, Function | MediumTest | Level1)
{
    // BROADCAST DELIVER CMASDATA
    uint8_t CMAS_TEST_BEARER_DATA[] = { 0x00, 0x03, 0x1C, 0x78, 0x00, 0x01, 0x59, 0x02, 0xB8, 0x00, 0x02, 0x10, 0xAA,
        0x68, 0xD3, 0xCD, 0x06, 0x9E, 0x68, 0x30, 0xA0, 0xE9, 0x97, 0x9F, 0x44, 0x1B, 0xF3, 0x20, 0xE9, 0xA3, 0x2A,
        0x08, 0x7B, 0xF6, 0xED, 0xCB, 0xCB, 0x1E, 0x9C, 0x3B, 0x10, 0x4D, 0xDF, 0x8B, 0x4E, 0xCC, 0xA8, 0x20, 0xEC,
        0xCB, 0xCB, 0xA2, 0x0A, 0x7E, 0x79, 0xF4, 0xCB, 0xB5, 0x72, 0x0A, 0x9A, 0x34, 0xF3, 0x41, 0xA7, 0x9A, 0x0D,
        0xFB, 0xB6, 0x79, 0x41, 0x85, 0x07, 0x4C, 0xBC, 0xFA, 0x2E, 0x00, 0x08, 0x20, 0x58, 0x38, 0x88, 0x80, 0x10,
        0x54, 0x06, 0x38, 0x20, 0x60, 0x30, 0xA8, 0x81, 0x90, 0x20, 0x08 };

    std::stringstream ss;
    ss.clear();
    ss << static_cast<uint8_t>(CdmaTransportMsgType::BROADCAST);
    ss << static_cast<uint8_t>(CdmaSmsParameterRecord::SERVICE_CATEGORY);
    ss << static_cast<uint8_t>(0x02);
    ss << static_cast<uint8_t>(static_cast<uint16_t>(SmsServiceCtg::CMAS_TEST) >> 8);
    ss << static_cast<uint8_t>(static_cast<uint16_t>(SmsServiceCtg::CMAS_TEST) & 0xff);
    ss << static_cast<uint8_t>(CdmaSmsParameterRecord::BEARER_DATA);
    ss << static_cast<uint8_t>(sizeof(CMAS_TEST_BEARER_DATA));
    for (uint8_t i = 0; i < sizeof(CMAS_TEST_BEARER_DATA); i++) {
        ss << CMAS_TEST_BEARER_DATA[i];
    }
    SmsReadBuffer rBuffer(ss.str());
    CdmaTransportMsg msg;
    memset_s(&msg, sizeof(msg), 0, sizeof(msg));
    std::unique_ptr<CdmaSmsTransportMessage> message = CdmaSmsTransportMessage::CreateTransportMessage(msg, rBuffer);
    EXPECT_TRUE(message->Decode(rBuffer));
    EXPECT_EQ(msg.data.broadcast.telesvcMsg.data.deliver.msgId.msgId, 0xc780);
    EXPECT_EQ(msg.data.broadcast.serviceCtg, static_cast<uint16_t>(SmsServiceCtg::CMAS_TEST));
    EXPECT_EQ(msg.data.broadcast.telesvcMsg.data.deliver.cmasData.dataLen, 74);
    EXPECT_EQ(msg.data.broadcast.telesvcMsg.data.deliver.cmasData.urgency, SmsCmaeUrgency::EXPECTED);
}

/**
 * @tc.number   Telephony_CdmaSmsGtest_CdmaSmsTransportMessage_0004
 * @tc.name     Test CdmaSmsTransportMessage
 * @tc.desc     Function test
 */
HWTEST_F(CdmaSmsGtest, CdmaSmsTransportMessage_0004, Function | MediumTest | Level1)
{
    // BROADCAST DELIVER USERDATA(BCCBB)
    std::string pduHex = "0101020004081300031008d00106102c2870e1420801c00c01c0";
    std::string pdu = StringUtils::HexToString(pduHex);
    SmsReadBuffer pduBuffer(pdu);
    CdmaTransportMsg msg;
    memset_s(&msg, sizeof(msg), 0, sizeof(msg));
    std::unique_ptr<CdmaSmsTransportMessage> transportMessage =
        CdmaSmsTransportMessage::CreateTransportMessage(msg, pduBuffer);
    EXPECT_NE(transportMessage, nullptr);
    EXPECT_FALSE(transportMessage->IsEmpty());
    EXPECT_TRUE(transportMessage->Decode(pduBuffer));
}

string GetUserDataString(SmsTeleSvcUserData userData)
{
    std::stringstream ssUserData;
    if (userData.encodeType == SmsEncodingType::UNICODE) {
        char unicodeChar[UNICODE_CHAR_LEN + 1];
        unicodeChar[UNICODE_CHAR_LEN] = '\0';
        for (uint8_t i = 0; i < userData.userData.length; i += UNICODE_CHAR_LEN) {
            ssUserData << "\\u";
            snprintf_s(unicodeChar, sizeof(unicodeChar), sizeof(unicodeChar) - 1, "%02x",
                static_cast<uint8_t>(userData.userData.data[i]));
            ssUserData << unicodeChar;
            snprintf_s(unicodeChar, sizeof(unicodeChar), sizeof(unicodeChar) - 1, "%02x",
                static_cast<uint8_t>(userData.userData.data[i + 1]));
            ssUserData << unicodeChar;
        }
    } else {
        for (uint8_t i = 0; i < userData.userData.length; i++) {
            ssUserData << static_cast<uint8_t>(userData.userData.data[i]);
        }
    }
    return ssUserData.str();
}

void CheckDeliverUserData(string pdu, string userData)
{
    std::stringstream ss;
    ss << static_cast<uint8_t>(CdmaSmsParameterRecord::BEARER_DATA);
    ss << static_cast<uint8_t>(pdu.size() / HEX_CHAR_LEN);
    ss << StringUtils::HexToString(pdu);

    CdmaTeleserviceMsg v;
    memset_s(&v, sizeof(CdmaTeleserviceMsg), 0x00, sizeof(CdmaTeleserviceMsg));
    SmsReadBuffer rBuffer(ss.str());
    rBuffer.SetIndex(VALUE_INDEX);
    auto message = std::make_shared<CdmaSmsBearerData>(v, rBuffer);
    rBuffer.SetIndex(0);
    EXPECT_TRUE(message->Decode(rBuffer));
    EXPECT_STREQ(GetUserDataString(v.data.deliver.userData).c_str(), userData.c_str());
}

void CheckSubmitUserData(string pdu, string userData)
{
    std::stringstream ss;
    ss << static_cast<uint8_t>(CdmaSmsParameterRecord::BEARER_DATA);
    ss << static_cast<uint8_t>(pdu.size() / HEX_CHAR_LEN);
    ss << StringUtils::HexToString(pdu);

    CdmaTeleserviceMsg v;
    memset_s(&v, sizeof(CdmaTeleserviceMsg), 0x00, sizeof(CdmaTeleserviceMsg));
    SmsReadBuffer rBuffer(ss.str());
    rBuffer.SetIndex(VALUE_INDEX);
    auto message = std::make_shared<CdmaSmsBearerData>(v, rBuffer);
    rBuffer.SetIndex(0);
    EXPECT_TRUE(message->Decode(rBuffer));
    EXPECT_STREQ(GetUserDataString(v.data.submit.userData).c_str(), userData.c_str());

    auto message1 = std::make_shared<CdmaSmsBearerData>(v);
    SmsWriteBuffer wBuffer;
    EXPECT_TRUE(message1->Encode(wBuffer));
    auto buffer = wBuffer.GetPduBuffer();
    EXPECT_GT(buffer->size(), 0);
    std::stringstream ssEncode;
    for (uint16_t i = 0; i < buffer->size(); i++) {
        ssEncode << (*buffer)[i];
    }

    CdmaTeleserviceMsg v2;
    memset_s(&v2, sizeof(CdmaTeleserviceMsg), 0x00, sizeof(CdmaTeleserviceMsg));
    SmsReadBuffer rBuffer2(ssEncode.str());
    rBuffer2.SetIndex(VALUE_INDEX);
    auto message2 = std::make_shared<CdmaSmsBearerData>(v2, rBuffer2);
    rBuffer2.SetIndex(0);
    EXPECT_TRUE(message2->Decode(rBuffer2));
    EXPECT_STREQ(GetUserDataString(v2.data.submit.userData).c_str(), userData.c_str());
}

/**
 * @tc.number   Telephony_CdmaSmsGtest_CdmaSmsBearerData_0001
 * @tc.name     Test CdmaSmsBearerData
 * @tc.desc     Function test
 */
HWTEST_F(CdmaSmsGtest, CdmaSmsBearerData_0001, Function | MediumTest | Level1)
{
    // gsm 7bit
    CheckDeliverUserData("00031040900112488ea794e074d69e1b7392c270326cde9e98", "Test standard SMS");
    // ascii 7bit
    CheckDeliverUserData("0003100160010610262d5ab500", "bjjj");
    // ia5
    CheckDeliverUserData("00031002100109184539b4d052ebb3d0", "SMS Rulz");
    // unicode
    CheckDeliverUserData("000310021001062012716B2C380801000A0140", "\\u4e2d\\u6587");

    // gsm 7bit
    CheckSubmitUserData("00032006400112488ea794e074d69e1b7392c270326cde9e98", "Test standard SMS");
    // ascii 7bit
    CheckSubmitUserData("0003200640010610262d5ab500", "bjjj");
    // ia5
    CheckSubmitUserData("00032006400109184539b4d052ebb3d0", "SMS Rulz");
    // unicode
    CheckSubmitUserData("000320064001062012716B2C380801000A0140", "\\u4e2d\\u6587");
}

/**
 * @tc.number   Telephony_CdmaSmsGtest_CdmaSmsBearerData_0002
 * @tc.name     Test CdmaSmsBearerData
 * @tc.desc     Function test
 */
HWTEST_F(CdmaSmsGtest, CdmaSmsBearerData_0002, Function | MediumTest | Level1)
{
    std::string dataStr = "0003200010010410168d20020105030608120111015905019206069706180000000801c00901800a01e00b"
                          "01030c01c00d01070e05039acc13880f018011020566";
    std::stringstream ss;
    ss.clear();
    ss << static_cast<uint8_t>(CdmaSmsParameterRecord::BEARER_DATA);
    ss << static_cast<uint8_t>(dataStr.size() / HEX_CHAR_LEN);
    ss << StringUtils::HexToString(dataStr);

    CdmaTeleserviceMsg v;
    memset_s(&v, sizeof(CdmaTeleserviceMsg), 0x00, sizeof(CdmaTeleserviceMsg));
    SmsReadBuffer rBuffer(ss.str());
    rBuffer.SetIndex(VALUE_INDEX);
    auto message = std::make_shared<CdmaSmsBearerData>(v, rBuffer);
    rBuffer.SetIndex(0);
    EXPECT_TRUE(message->Decode(rBuffer));
    EXPECT_EQ(v.type, TeleserviceMsgType::SUBMIT);
    EXPECT_EQ(v.data.submit.msgId.msgId, 1);
    EXPECT_EQ(v.data.submit.priority, SmsPriorityIndicator::EMERGENCY);
    EXPECT_EQ(v.data.submit.privacy, SmsPrivacyIndicator::CONFIDENTIAL);
    EXPECT_EQ(v.data.submit.callbackNumber.addrLen, 7);
    char number[] = "3598271";
    for (uint8_t i = 0; i < sizeof(number); i++) {
        EXPECT_EQ(v.data.submit.callbackNumber.szData[i], number[i]);
    }
    EXPECT_EQ(v.data.submit.depositId, 1382);
    EXPECT_EQ(v.data.submit.language, SmsLanguageType::HEBREW);
    EXPECT_EQ(v.data.submit.alertPriority, SmsAlertPriority::HIGH);
    EXPECT_EQ(v.data.submit.deferValPeriod.time.absTime.year, 97);
    EXPECT_EQ(v.data.submit.deferValPeriod.time.absTime.month, 6);
    EXPECT_EQ(v.data.submit.deferValPeriod.time.absTime.day, 18);
}
#endif // TEL_TEST_UNSUPPORT
} // namespace Telephony
} // namespace OHOS