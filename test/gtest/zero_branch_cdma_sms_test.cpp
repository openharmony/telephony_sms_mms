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
#include "cdma_sms_receive_handler.h"
#include "cdma_sms_transport_message.h"
#include "gtest/gtest.h"

namespace OHOS {
namespace Telephony {
using namespace testing::ext;

namespace {
const uint16_t PDU_BUFFER_MAX_SIZE = 0xFF;
const size_t DATA_LENGTH = 162;
const int32_t HEADER_LENGTH = 7;
} // namespace

class BranchCdmaSmsTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};
void BranchCdmaSmsTest::SetUpTestCase() {}

void BranchCdmaSmsTest::TearDownTestCase() {}

void BranchCdmaSmsTest::SetUp() {}

void BranchCdmaSmsTest::TearDown() {}

/**
 * @tc.number   Telephony_BranchCdmaSmsTest_CdmaSmsMessage_0001
 * @tc.name     Test CdmaSmsMessage
 * @tc.desc     Function test
 */
HWTEST_F(BranchCdmaSmsTest, CdmaSmsMessage_0001, Function | MediumTest | Level1)
{
    std::shared_ptr<CdmaSmsMessage> cdmaSmsMessage = std::make_shared<CdmaSmsMessage>();
    string pduHex = "";
    DataCodingScheme codingType;
    CdmaP2PMsg p2pMsg;
    p2pMsg.telesvcMsg.type = TeleserviceMsgType::DELIVER;
    cdmaSmsMessage->AnalysisP2pMsg(p2pMsg);
    p2pMsg.telesvcMsg.type = TeleserviceMsgType::DELIVERY_ACK;
    cdmaSmsMessage->AnalysisP2pMsg(p2pMsg);
    p2pMsg.telesvcMsg.type = TeleserviceMsgType::USER_ACK;
    cdmaSmsMessage->AnalysisP2pMsg(p2pMsg);
    p2pMsg.telesvcMsg.type = TeleserviceMsgType::READ_ACK;
    cdmaSmsMessage->AnalysisP2pMsg(p2pMsg);
    p2pMsg.telesvcMsg.type = TeleserviceMsgType::SUBMIT_REPORT;
    cdmaSmsMessage->AnalysisP2pMsg(p2pMsg);
    p2pMsg.telesvcMsg.type = TeleserviceMsgType::SUBMIT;
    cdmaSmsMessage->AnalysisP2pMsg(p2pMsg);
    p2pMsg.telesvcMsg.type = TeleserviceMsgType::MAX_VALUE;
    cdmaSmsMessage->AnalysisP2pMsg(p2pMsg);
    p2pMsg.teleserviceId = static_cast<uint16_t>(SmsTransTelsvcId::RESERVED);
    cdmaSmsMessage->AnalysisP2pMsg(p2pMsg);
    EXPECT_FALSE(cdmaSmsMessage->IsWapPushMsg());
    EXPECT_EQ(cdmaSmsMessage->GetTransMsgType(), CdmaTransportMsgType::RESERVED);
    EXPECT_EQ(cdmaSmsMessage->GetTransTeleService(), static_cast<uint16_t>(SmsTransTelsvcId::RESERVED));
    EXPECT_FALSE(cdmaSmsMessage->PduAnalysis(pduHex));
    cdmaSmsMessage->transMsg_ = std::make_unique<struct CdmaTransportMsg>();
    cdmaSmsMessage->transMsg_->type = CdmaTransportMsgType::BROADCAST;
    EXPECT_EQ(cdmaSmsMessage->GetTransTeleService(), static_cast<uint16_t>(SmsTransTelsvcId::RESERVED));
    cdmaSmsMessage->transMsg_->type = CdmaTransportMsgType::P2P;
    cdmaSmsMessage->transMsg_->data.p2p.teleserviceId = static_cast<uint16_t>(SmsTransTelsvcId::WAP);
    EXPECT_TRUE(cdmaSmsMessage->IsWapPushMsg());
    codingType = DataCodingScheme::DATA_CODING_7BIT;
    EXPECT_EQ(cdmaSmsMessage->CovertEncodingType(codingType), SmsEncodingType::GSM7BIT);
    codingType = DataCodingScheme::DATA_CODING_ASCII7BIT;
    EXPECT_EQ(cdmaSmsMessage->CovertEncodingType(codingType), SmsEncodingType::ASCII_7BIT);
    codingType = DataCodingScheme::DATA_CODING_8BIT;
    EXPECT_EQ(cdmaSmsMessage->CovertEncodingType(codingType), SmsEncodingType::OCTET);
    codingType = DataCodingScheme::DATA_CODING_UCS2;
    EXPECT_EQ(cdmaSmsMessage->CovertEncodingType(codingType), SmsEncodingType::UNICODE);
    codingType = DataCodingScheme::DATA_CODING_EUCKR;
    EXPECT_EQ(cdmaSmsMessage->CovertEncodingType(codingType), SmsEncodingType::UNICODE);
    EXPECT_FALSE(cdmaSmsMessage->PduAnalysis(pduHex));
    pduHex = "12";
    EXPECT_FALSE(cdmaSmsMessage->PduAnalysis(pduHex));
}

/**
 * @tc.number   Telephony_BranchCdmaSmsTest_CdmaSmsMessage_0002
 * @tc.name     Test CdmaSmsMessage
 * @tc.desc     Function test
 */
HWTEST_F(BranchCdmaSmsTest, CdmaSmsMessage_0002, Function | MediumTest | Level1)
{
    std::shared_ptr<CdmaSmsMessage> cdmaSmsMessage = std::make_shared<CdmaSmsMessage>();
    CdmaP2PMsg p2pMsg;
    TeleserviceDeliver deliver;
    SmsTeleSvcUserData userData;
    CdmaBroadCastMsg cbMsg;
    SmsUDH header;
    p2pMsg.teleserviceId = static_cast<uint16_t>(SmsTransTelsvcId::VMN_95);
    p2pMsg.telesvcMsg.data.deliver.vmn.faxIncluded = true;
    p2pMsg.telesvcMsg.data.deliver.userData.userData.length = 0;
    deliver.displayMode = SmsDisplayMode::IMMEDIATE;
    cdmaSmsMessage->AnalsisDeliverMwi(p2pMsg);
    cdmaSmsMessage->AnalsisDeliverMsg(deliver);
    userData.encodeType = SmsEncodingType::GSM7BIT;
    cdmaSmsMessage->AnalsisUserData(userData);
    userData.encodeType = SmsEncodingType::KOREAN;
    cdmaSmsMessage->AnalsisUserData(userData);
    userData.encodeType = SmsEncodingType::EUCKR;
    cdmaSmsMessage->AnalsisUserData(userData);
    userData.encodeType = SmsEncodingType::IA5;
    cdmaSmsMessage->AnalsisUserData(userData);
    userData.encodeType = SmsEncodingType::ASCII_7BIT;
    cdmaSmsMessage->AnalsisUserData(userData);
    userData.encodeType = SmsEncodingType::LATIN_HEBREW;
    cdmaSmsMessage->AnalsisUserData(userData);
    userData.encodeType = SmsEncodingType::LATIN;
    cdmaSmsMessage->AnalsisUserData(userData);
    userData.encodeType = SmsEncodingType::OCTET;
    cdmaSmsMessage->AnalsisUserData(userData);
    userData.encodeType = SmsEncodingType::SHIFT_JIS;
    cdmaSmsMessage->AnalsisUserData(userData);
    userData.encodeType = SmsEncodingType::RESERVED;
    cdmaSmsMessage->AnalsisUserData(userData);
    deliver.cmasData.dataLen = DATA_LENGTH;
    cdmaSmsMessage->AnalsisCMASMsg(deliver);
    cbMsg.telesvcMsg.type = TeleserviceMsgType::RESERVED;
    cdmaSmsMessage->AnalysisCbMsg(cbMsg);
    cbMsg.telesvcMsg.type = TeleserviceMsgType::DELIVER;
    cdmaSmsMessage->AnalysisCbMsg(cbMsg);
    cbMsg.serviceCtg = static_cast<uint16_t>(SmsServiceCtg::CMAS_AMBER);
    cdmaSmsMessage->AnalysisCbMsg(cbMsg);
    userData.userData.headerCnt = HEADER_LENGTH;
    cdmaSmsMessage->bHeaderInd_ = true;
    cdmaSmsMessage->AnalsisHeader(userData);
    EXPECT_FALSE(cdmaSmsMessage->AddUserDataHeader(header));
}

/**
 * @tc.number   Telephony_SmsMmsGtest_SmsPduBuffer_0001
 * @tc.name     Test SmsPduBuffer
 * @tc.desc     Function test
 */
HWTEST_F(BranchCdmaSmsTest, SmsPduBuffer_0001, Function | MediumTest | Level1)
{
    auto buffer = std::make_shared<SmsPduBuffer>();
    EXPECT_TRUE(buffer->IsEmpty());
    EXPECT_EQ(buffer->GetIndex(), 0);
    EXPECT_FALSE(buffer->SetIndex(1));
    EXPECT_EQ(buffer->GetIndex(), 0);
    EXPECT_TRUE(buffer->SetIndex(0));
    EXPECT_EQ(buffer->GetIndex(), 0);
    EXPECT_EQ(buffer->MoveForward(), 0);
    EXPECT_EQ(buffer->MoveForward(1), 0);
    EXPECT_EQ(buffer->MoveBack(), 0);
    EXPECT_EQ(buffer->MoveBack(1), 0);
    EXPECT_EQ(buffer->MoveBack(2), 0);
    EXPECT_EQ(buffer->MoveForward(0), 0);
    EXPECT_EQ(buffer->SkipBits(), 0);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_SmsReadBuffer_0001
 * @tc.name     Test SmsReadBuffer
 * @tc.desc     Function test
 */
HWTEST_F(BranchCdmaSmsTest, SmsReadBuffer_0001, Function | MediumTest | Level1)
{
    std::string pduHex = "0000021002";
    std::string pdu = StringUtils::HexToString(pduHex);
    uint16_t pduLen = pdu.length();

    auto buffer = std::make_shared<SmsReadBuffer>(pdu);
    EXPECT_FALSE(buffer->IsEmpty());
    EXPECT_EQ(buffer->GetIndex(), 0);
    EXPECT_TRUE(buffer->SetIndex(1));
    EXPECT_EQ(buffer->GetIndex(), 1);
    EXPECT_TRUE(buffer->SetIndex(0));
    EXPECT_EQ(buffer->GetIndex(), 0);
    EXPECT_TRUE(buffer->SetIndex(pduLen));
    EXPECT_EQ(buffer->GetIndex(), pduLen);
    EXPECT_FALSE(buffer->SetIndex(pduLen + 1));
    EXPECT_EQ(buffer->GetIndex(), pduLen);

    EXPECT_TRUE(buffer->SetIndex(0));
    EXPECT_EQ(buffer->GetIndex(), 0);
    EXPECT_EQ(buffer->MoveForward(), 0);
    EXPECT_EQ(buffer->MoveForward(1), 1);
    EXPECT_EQ(buffer->MoveBack(1), 2);
    EXPECT_EQ(buffer->MoveForward(0), 1);
    EXPECT_EQ(buffer->MoveBack(2), 1);
    EXPECT_EQ(buffer->MoveBack(1), 1);
    EXPECT_EQ(buffer->GetIndex(), 0);

    EXPECT_TRUE(buffer->SetIndex(pduLen));
    EXPECT_EQ(buffer->GetIndex(), pduLen);
    EXPECT_EQ(buffer->MoveForward(), pduLen);
    EXPECT_EQ(buffer->MoveForward(), pduLen);
    EXPECT_EQ(buffer->MoveBack(1), pduLen);
    EXPECT_EQ(buffer->GetIndex(), pduLen - 1);

    uint16_t index = buffer->GetIndex();
    EXPECT_EQ(buffer->SkipBits(), index);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_SmsReadBuffer_0002
 * @tc.name     Test SmsReadBuffer
 * @tc.desc     Function test
 */
HWTEST_F(BranchCdmaSmsTest, SmsReadBuffer_0002, Function | MediumTest | Level1)
{
    std::string pduHex = "00000210020000021002";
    std::string pdu = StringUtils::HexToString(pduHex);
    uint16_t pduLen = pdu.length();

    auto buffer = std::make_shared<SmsReadBuffer>(pdu);
    EXPECT_FALSE(buffer->IsEmpty());

    buffer->SetIndex(0);
    uint8_t v = 0;
    for (uint16_t i = 0; i < pduLen; i++) {
        EXPECT_TRUE(buffer->ReadByte(v));
    }
    EXPECT_FALSE(buffer->ReadByte(v));

    buffer->SetIndex(0);
    uint16_t v2 = 0;
    uint16_t index = buffer->GetIndex();
    do {
        EXPECT_TRUE(buffer->ReadWord(v2));
        index += 2;
        EXPECT_EQ(buffer->GetIndex(), index);
    } while (index < pduLen);
    index = buffer->GetIndex();
    EXPECT_FALSE(buffer->ReadWord(v2));
    EXPECT_EQ(buffer->GetIndex(), index);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_SmsReadBuffer_0003
 * @tc.name     Test SmsReadBuffer
 * @tc.desc     Function test
 */
HWTEST_F(BranchCdmaSmsTest, SmsReadBuffer_0003, Function | MediumTest | Level1)
{
    std::string pduHex = "00000210020000021002";
    std::string pdu = StringUtils::HexToString(pduHex);
    uint16_t pduLen = pdu.length();

    auto buffer = std::make_shared<SmsReadBuffer>(pdu);
    EXPECT_FALSE(buffer->IsEmpty());

    buffer->SetIndex(0);
    uint32_t bitLen = pduLen * BIT8;
    uint8_t v = 0;
    for (uint32_t i = 0; i < bitLen; i++) {
        EXPECT_TRUE(buffer->ReadBits(v));
    }
    EXPECT_FALSE(buffer->ReadBits(v));
    uint16_t index = buffer->GetIndex();
    EXPECT_EQ(buffer->SkipBits(), index);

    buffer->SetIndex(0);
    uint8_t bits = BIT0;
    uint32_t bitIndex = 0;
    do {
        bits++;
        if (bits > BIT8) {
            EXPECT_FALSE(buffer->ReadBits(v, bits));
            if (bits == 10) {
                bits = BIT0;
            }
        } else {
            if (bitIndex + bits <= bitLen) {
                bitIndex += bits;
                EXPECT_TRUE(buffer->ReadBits(v, bits));
            } else {
                EXPECT_FALSE(buffer->ReadBits(v, bits));
            }
        }
    } while (bitIndex < bitLen);

    buffer->SetIndex(0);
    index = buffer->GetIndex();
    EXPECT_EQ(buffer->SkipBits(), index);
    EXPECT_TRUE(buffer->ReadBits(v));
    EXPECT_FALSE(buffer->SetIndex(0));
    EXPECT_EQ(buffer->SkipBits(), index + 1);
    EXPECT_TRUE(buffer->SetIndex(0));
}

/**
 * @tc.number   Telephony_SmsMmsGtest_SmsReadBuffer_0004
 * @tc.name     Test SmsReadBuffer
 * @tc.desc     Function test
 */
HWTEST_F(BranchCdmaSmsTest, SmsReadBuffer_0004, Function | MediumTest | Level1)
{
    auto buffer = std::make_shared<SmsReadBuffer>(StringUtils::HexToString("00"));
    EXPECT_TRUE(buffer->IsEmpty());
    buffer = std::make_shared<SmsReadBuffer>(StringUtils::HexToString("0000"));
    EXPECT_FALSE(buffer->IsEmpty());
    buffer = std::make_shared<SmsReadBuffer>(StringUtils::HexToString("000002"));
    EXPECT_FALSE(buffer->IsEmpty());
    buffer = std::make_shared<SmsReadBuffer>(StringUtils::HexToString("0000021002"));
    EXPECT_FALSE(buffer->IsEmpty());
    std::string s1(PDU_BUFFER_MAX_SIZE * 2, '0');
    buffer = std::make_shared<SmsReadBuffer>(StringUtils::HexToString(s1));
    EXPECT_FALSE(buffer->IsEmpty());
    std::string s2((PDU_BUFFER_MAX_SIZE + 1) * 2, '0');
    buffer = std::make_shared<SmsReadBuffer>(StringUtils::HexToString(s2));
    EXPECT_FALSE(buffer->IsEmpty());
    std::string s3((PDU_BUFFER_MAX_SIZE + 2) * 2, '0');
    buffer = std::make_shared<SmsReadBuffer>(StringUtils::HexToString(s3));
    EXPECT_TRUE(buffer->IsEmpty());
}

/**
 * @tc.number   Telephony_SmsMmsGtest_SmsWriteBuffer_0001
 * @tc.name     Test SmsWriteBuffer
 * @tc.desc     Function test
 */
HWTEST_F(BranchCdmaSmsTest, SmsWriteBuffer_0001, Function | MediumTest | Level1)
{
    uint16_t pduLen = PDU_BUFFER_MAX_SIZE + 1;
    auto buffer = std::make_shared<SmsWriteBuffer>();
    EXPECT_FALSE(buffer->IsEmpty());
    EXPECT_EQ(buffer->GetIndex(), 0);
    EXPECT_TRUE(buffer->SetIndex(1));
    EXPECT_EQ(buffer->GetIndex(), 1);
    EXPECT_TRUE(buffer->SetIndex(0));
    EXPECT_EQ(buffer->GetIndex(), 0);
    EXPECT_TRUE(buffer->SetIndex(pduLen));
    EXPECT_EQ(buffer->GetIndex(), pduLen);
    EXPECT_FALSE(buffer->SetIndex(pduLen + 1));
    EXPECT_EQ(buffer->GetIndex(), pduLen);

    EXPECT_TRUE(buffer->SetIndex(0));
    EXPECT_EQ(buffer->GetIndex(), 0);
    EXPECT_EQ(buffer->MoveForward(), 0);
    EXPECT_EQ(buffer->MoveForward(1), 1);
    EXPECT_EQ(buffer->MoveBack(1), 2);
    EXPECT_EQ(buffer->MoveForward(0), 1);
    EXPECT_EQ(buffer->MoveBack(2), 1);
    EXPECT_EQ(buffer->MoveBack(1), 1);
    EXPECT_EQ(buffer->GetIndex(), 0);

    EXPECT_TRUE(buffer->SetIndex(pduLen));
    EXPECT_EQ(buffer->GetIndex(), pduLen);
    EXPECT_EQ(buffer->MoveForward(), pduLen);
    EXPECT_EQ(buffer->MoveForward(), pduLen);
    EXPECT_EQ(buffer->MoveBack(1), pduLen);
    EXPECT_EQ(buffer->GetIndex(), pduLen - 1);

    uint16_t index = buffer->GetIndex();
    EXPECT_EQ(buffer->SkipBits(), index);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_SmsWriteBuffer_0002
 * @tc.name     Test SmsWriteBuffer
 * @tc.desc     Function test
 */
HWTEST_F(BranchCdmaSmsTest, SmsWriteBuffer_0002, Function | MediumTest | Level1)
{
    uint16_t pduLen = PDU_BUFFER_MAX_SIZE + 1;
    auto buffer = std::make_shared<SmsWriteBuffer>();
    EXPECT_FALSE(buffer->IsEmpty());

    EXPECT_EQ(buffer->GetPduBuffer(), nullptr);

    buffer->SetIndex(0);
    uint8_t v = 0;
    for (uint16_t i = 0; i < pduLen; i++) {
        v++;
        if (v == 0xff) {
            v = 0;
        }
        EXPECT_TRUE(buffer->InsertByte(v, i));
    }
    EXPECT_FALSE(buffer->InsertByte(v, pduLen));

    buffer->SetIndex(0);
    for (uint16_t i = 0; i < pduLen; i++) {
        v++;
        if (v == 0xff) {
            v = 0;
        }
        EXPECT_TRUE(buffer->WriteByte(v));
        EXPECT_EQ(buffer->GetIndex(), i + 1);
    }
    uint16_t index = buffer->GetIndex();
    EXPECT_FALSE(buffer->WriteByte(v));
    EXPECT_EQ(buffer->GetIndex(), index);

    buffer->SetIndex(1);
    std::unique_ptr<std::vector<uint8_t>> data = buffer->GetPduBuffer();
    EXPECT_NE(data, nullptr);
    EXPECT_EQ(data->size(), 1);

    buffer->SetIndex(0);
    uint16_t v2 = 0;
    index = buffer->GetIndex();
    do {
        v2++;
        if (v2 == 0xffff) {
            v = 0;
        }
        EXPECT_TRUE(buffer->WriteWord(v2));
        index += 2;
        EXPECT_EQ(buffer->GetIndex(), index);
    } while (index < pduLen);
    index = buffer->GetIndex();
    EXPECT_FALSE(buffer->WriteByte(v));
    EXPECT_EQ(buffer->GetIndex(), index);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_SmsWriteBuffer_0003
 * @tc.name     Test SmsWriteBuffer
 * @tc.desc     Function test
 */
HWTEST_F(BranchCdmaSmsTest, SmsWriteBuffer_0003, Function | MediumTest | Level1)
{
    uint16_t pduLen = PDU_BUFFER_MAX_SIZE + 1;
    auto buffer = std::make_shared<SmsWriteBuffer>();
    EXPECT_FALSE(buffer->IsEmpty());

    buffer->SetIndex(0);
    uint32_t bitLen = pduLen * BIT8;
    uint8_t v = 0;
    for (uint32_t i = 0; i < bitLen; i++) {
        EXPECT_TRUE(buffer->WriteBits(v));
    }
    EXPECT_FALSE(buffer->WriteBits(v));
    uint16_t index = buffer->GetIndex();
    EXPECT_EQ(buffer->SkipBits(), index);

    buffer->SetIndex(0);
    uint8_t bits = BIT0;
    uint32_t bitIndex = 0;
    do {
        bits++;
        if (bits > BIT8) {
            EXPECT_FALSE(buffer->WriteBits(v, bits));
            if (bits == 10) {
                bits = BIT0;
            }
        } else {
            if (bitIndex + bits <= bitLen) {
                bitIndex += bits;
                EXPECT_TRUE(buffer->WriteBits(v, bits));
            } else {
                EXPECT_FALSE(buffer->WriteBits(v, bits));
            }
        }
    } while (bitIndex < bitLen);

    buffer->SetIndex(0);
    index = buffer->GetIndex();
    EXPECT_EQ(buffer->SkipBits(), index);
    EXPECT_TRUE(buffer->WriteBits(v));
    EXPECT_FALSE(buffer->SetIndex(0));
    EXPECT_EQ(buffer->SkipBits(), index + 1);
    EXPECT_TRUE(buffer->SetIndex(0));

    buffer->SetIndex(0);
    EXPECT_EQ(buffer->GetPduBuffer(), nullptr);
    buffer->SetIndex(1);
    index = buffer->GetIndex();
    EXPECT_EQ(buffer->GetPduBuffer()->size(), index);
    EXPECT_TRUE(buffer->WriteBits(v));
    EXPECT_EQ(buffer->GetPduBuffer(), nullptr);
    EXPECT_EQ(buffer->SkipBits(), index + 1);
    EXPECT_EQ(buffer->GetPduBuffer()->size(), static_cast<unsigned long>(index + 1));
}

/**
 * @tc.number   Telephony_SmsMmsGtest_CdmaSmsTransportMessage_0001
 * @tc.name     Test CdmaSmsTransportMessage
 * @tc.desc     Function test
 */
HWTEST_F(BranchCdmaSmsTest, CdmaSmsTransportMessage_0001, Function | MediumTest | Level1)
{
    auto message = std::make_shared<CdmaSmsTransportMessage>();
    EXPECT_TRUE(message->IsEmpty());

    auto buffer1 = std::make_shared<SmsWriteBuffer>();
    EXPECT_FALSE(buffer1->IsEmpty());
    EXPECT_FALSE(message->Encode(*buffer1));

    std::stringstream ss;
    ss.clear();
    ss << static_cast<uint8_t>(CdmaTransportMsgType::P2P);
    auto buffer2 = std::make_shared<SmsReadBuffer>(ss.str());
    EXPECT_TRUE(buffer2->IsEmpty());
    EXPECT_FALSE(message->Decode(*buffer2));

    ss << CdmaSmsParameterRecord::TELESERVICE_ID;
    auto buffer3 = std::make_shared<SmsReadBuffer>(ss.str());
    EXPECT_FALSE(buffer3->IsEmpty());
    EXPECT_FALSE(message->Decode(*buffer3));
}

/**
 * @tc.number   Telephony_SmsMmsGtest_CdmaSmsTransportMessage_0002
 * @tc.name     Test CdmaSmsTransportMessage
 * @tc.desc     Function test
 */
HWTEST_F(BranchCdmaSmsTest, CdmaSmsTransportMessage_0002, Function | MediumTest | Level1)
{
    do {
        CdmaTransportMsg msg;
        msg.type = CdmaTransportMsgType::RESERVED;
        auto message = CdmaSmsTransportMessage::CreateTransportMessage(msg);
        EXPECT_EQ(message, nullptr);
    } while (0);

    uint8_t type = 0;
    for (type = 0; type < static_cast<uint8_t>(CdmaTransportMsgType::RESERVED); type++) {
        CdmaTransportMsg msg;
        msg.type = CdmaTransportMsgType(type);
        auto message = CdmaSmsTransportMessage::CreateTransportMessage(msg);
        EXPECT_NE(message, nullptr);
        auto buffer = std::make_shared<SmsWriteBuffer>();
        EXPECT_FALSE(buffer->IsEmpty());
        message->Encode(*buffer);
        SmsReadBuffer rBuffer(StringUtils::HexToString("000000"));
        EXPECT_FALSE(message->Decode(rBuffer));
    }

    CdmaTransportMsg msg;
    std::vector<std::string> pduHex;
    pduHex.clear();
    pduHex.push_back("00");
    pduHex.push_back("0300");
    for (uint8_t i = 0; i < pduHex.size(); i++) {
        SmsReadBuffer buffer(StringUtils::HexToString(pduHex[i]));
        auto message = CdmaSmsTransportMessage::CreateTransportMessage(msg, buffer);
        EXPECT_EQ(message, nullptr);
    }

    pduHex.clear();
    pduHex.push_back("0000");
    pduHex.push_back("0100");
    pduHex.push_back("0200");
    for (uint8_t i = 0; i < pduHex.size(); i++) {
        SmsReadBuffer buffer(StringUtils::HexToString(pduHex[i]));
        auto message = CdmaSmsTransportMessage::CreateTransportMessage(msg, buffer);
        EXPECT_NE(message, nullptr);
        EXPECT_FALSE(message->Decode(buffer));
    }

    pduHex.clear();
    pduHex.push_back("0000021002");
    pduHex.push_back("0101020000");
    pduHex.push_back("0207020000");
    for (uint8_t i = 0; i < pduHex.size(); i++) {
        SmsReadBuffer buffer(StringUtils::HexToString(pduHex[i]));
        auto message = CdmaSmsTransportMessage::CreateTransportMessage(msg, buffer);
        EXPECT_NE(message, nullptr);
        EXPECT_TRUE(message->Decode(buffer));
    }
}

/**
 * @tc.number   Telephony_SmsMmsGtest_CdmaSmsTransportMessage_0003
 * @tc.name     Test CdmaSmsTransportMessage
 * @tc.desc     Function test
 */
HWTEST_F(BranchCdmaSmsTest, CdmaSmsTransportMessage_0003, Function | MediumTest | Level1)
{
    auto message = std::make_shared<CdmaSmsTransportMessage>();
    EXPECT_TRUE(message->IsEmpty());

    auto buffer1 = std::make_shared<SmsWriteBuffer>();
    EXPECT_FALSE(buffer1->IsEmpty());
    EXPECT_FALSE(message->Encode(*buffer1));

    std::stringstream ss;
    ss.clear();
    ss << static_cast<uint8_t>(CdmaTransportMsgType::P2P);
    ss << CdmaSmsParameterRecord::TELESERVICE_ID;
    auto buffer2 = std::make_shared<SmsReadBuffer>(ss.str());
    EXPECT_FALSE(buffer2->IsEmpty());
    EXPECT_FALSE(message->Decode(*buffer2));

    CdmaTransportMsg msg;
    msg.type = CdmaTransportMsgType::P2P;
    auto message2 = CdmaSmsTransportMessage::CreateTransportMessage(msg);
    EXPECT_NE(message2, nullptr);
    auto message3 = CdmaSmsTransportMessage::CreateTransportMessage(msg, *buffer2);
    EXPECT_NE(message3, nullptr);
    auto message4 = std::make_shared<CdmaSmsP2pMessage>(msg.data.p2p);
    EXPECT_NE(message4, nullptr);
    auto message5 = std::make_shared<CdmaSmsP2pMessage>(msg.data.p2p, *buffer2);
    EXPECT_NE(message5, nullptr);
    auto message6 = std::make_shared<CdmaSmsBroadcastMessage>(msg.data.broadcast);
    EXPECT_NE(message6, nullptr);
    auto message7 = std::make_shared<CdmaSmsBroadcastMessage>(msg.data.broadcast, *buffer2);
    EXPECT_NE(message7, nullptr);
    auto message8 = std::make_shared<CdmaSmsAckMessage>(msg.data.ack);
    EXPECT_NE(message8, nullptr);
    auto message9 = std::make_shared<CdmaSmsAckMessage>(msg.data.ack, *buffer2);
    EXPECT_NE(message9, nullptr);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_CdmaSmsTeleserviceMessage_0001
 * @tc.name     Test CdmaSmsTeleserviceMessage
 * @tc.desc     Function test
 */
HWTEST_F(BranchCdmaSmsTest, CdmaSmsTeleserviceMessage_0001, Function | MediumTest | Level1)
{
    SmsWriteBuffer wBuffer;
    SmsReadBuffer rBuffer(StringUtils::HexToString("0003200640"));
    auto message = std::make_shared<CdmaSmsTeleserviceMessage>();
    EXPECT_NE(message, nullptr);
    wBuffer.SetIndex(PDU_BUFFER_MAX_SIZE);
    EXPECT_FALSE(message->Encode(wBuffer));
    EXPECT_FALSE(message->Decode(rBuffer));
    EXPECT_EQ(CdmaSmsTeleserviceMessage::GetMessageType(rBuffer), CdmaSmsTeleserviceMessage::SUBMIT);

    wBuffer.SetIndex(PDU_BUFFER_MAX_SIZE);
    CdmaTransportMsg msg;
    auto message1 = std::make_shared<CdmaSmsSubmitMessage>(msg.data.p2p.telesvcMsg.data.submit);
    EXPECT_NE(message1, nullptr);
    EXPECT_FALSE(message1->Encode(wBuffer));
    EXPECT_FALSE(message1->Decode(rBuffer));
    auto message2 = std::make_shared<CdmaSmsSubmitMessage>(msg.data.p2p.telesvcMsg.data.submit, rBuffer);
    EXPECT_NE(message2, nullptr);
    EXPECT_FALSE(message2->Encode(wBuffer));
    EXPECT_FALSE(message2->Decode(rBuffer));
    auto message3 = std::make_shared<CdmaSmsCancelMessage>(msg.data.p2p.telesvcMsg.data.cancel);
    EXPECT_NE(message3, nullptr);
    EXPECT_FALSE(message3->Encode(wBuffer));
    EXPECT_FALSE(message3->Decode(rBuffer));
    auto message4 = std::make_shared<CdmaSmsDeliverReport>(msg.data.p2p.telesvcMsg.data.report);
    EXPECT_NE(message4, nullptr);
    EXPECT_FALSE(message4->Encode(wBuffer));
    EXPECT_FALSE(message4->Decode(rBuffer));
    auto message5 = std::make_shared<CdmaSmsDeliverMessage>(msg.data.p2p.telesvcMsg.data.deliver, rBuffer);
    EXPECT_NE(message5, nullptr);
    EXPECT_FALSE(message5->Encode(wBuffer));
    EXPECT_FALSE(message5->Decode(rBuffer));
    auto message6 = std::make_shared<CdmaSmsDeliveryAck>(msg.data.p2p.telesvcMsg.data.deliveryAck, rBuffer);
    EXPECT_NE(message6, nullptr);
    EXPECT_FALSE(message6->Encode(wBuffer));
    EXPECT_FALSE(message6->Decode(rBuffer));
    auto message7 = std::make_shared<CdmaSmsUserAck>(msg.data.p2p.telesvcMsg.data.userAck, rBuffer);
    EXPECT_NE(message7, nullptr);
    EXPECT_FALSE(message7->Encode(wBuffer));
    EXPECT_FALSE(message7->Decode(rBuffer));
    auto message8 = std::make_shared<CdmaSmsReadAck>(msg.data.p2p.telesvcMsg.data.readAck, rBuffer);
    EXPECT_NE(message8, nullptr);
    EXPECT_FALSE(message8->Encode(wBuffer));
    EXPECT_FALSE(message8->Decode(rBuffer));
}

/**
 * @tc.number   Telephony_SmsMmsGtest_CdmaSmsParameterRecord_0001
 * @tc.name     Test CdmaSmsParameterRecord
 * @tc.desc     Function test
 */
HWTEST_F(BranchCdmaSmsTest, CdmaSmsParameterRecord_0001, Function | MediumTest | Level1)
{
    auto initValue = static_cast<uint16_t>(SmsTransTelsvcId::RESERVED);
    auto testValue = static_cast<uint16_t>(SmsTransTelsvcId::CMT_95);
    uint16_t v1 = testValue;
    uint16_t v2 = initValue;

    auto message1 = std::make_shared<CdmaSmsTeleserviceId>(v1);
    SmsWriteBuffer wBuffer;
    EXPECT_TRUE(message1->Encode(wBuffer));
    auto buffer = wBuffer.GetPduBuffer();
    EXPECT_GT(buffer->size(), static_cast<uint32_t>(0));
    std::stringstream ss;
    ss.clear();
    for (uint16_t i = 0; i < buffer->size(); i++) {
        ss << (*buffer)[i];
    }
    SmsReadBuffer rBuffer(ss.str());
    auto message2 = std::make_shared<CdmaSmsTeleserviceId>(v2);
    EXPECT_TRUE(message2->Decode(rBuffer));
    EXPECT_EQ(v2, testValue);

    wBuffer.SetIndex(PDU_BUFFER_MAX_SIZE);
    EXPECT_FALSE(message1->Encode(wBuffer));
    rBuffer.SetIndex(1);
    EXPECT_FALSE(message1->Decode(rBuffer));
}

/**
 * @tc.number   Telephony_SmsMmsGtest_CdmaSmsParameterRecord_0002
 * @tc.name     Test CdmaSmsParameterRecord
 * @tc.desc     Function test
 */
HWTEST_F(BranchCdmaSmsTest, CdmaSmsParameterRecord_0002, Function | MediumTest | Level1)
{
    auto initValue = static_cast<uint16_t>(SmsServiceCtg::RESERVED);
    auto testValue = static_cast<uint16_t>(SmsServiceCtg::CMAS_TEST);
    uint16_t v1 = testValue;
    uint16_t v2 = initValue;

    auto message1 = std::make_shared<CdmaSmsServiceCategory>(v1);
    SmsWriteBuffer wBuffer;
    EXPECT_TRUE(message1->Encode(wBuffer));
    auto buffer = wBuffer.GetPduBuffer();
    EXPECT_GT(buffer->size(), 0);
    std::stringstream ss;
    ss.clear();
    for (uint16_t i = 0; i < buffer->size(); i++) {
        ss << (*buffer)[i];
    }
    SmsReadBuffer rBuffer(ss.str());
    auto message2 = std::make_shared<CdmaSmsServiceCategory>(v2);
    EXPECT_TRUE(message2->Decode(rBuffer));
    EXPECT_EQ(v2, testValue);

    wBuffer.SetIndex(PDU_BUFFER_MAX_SIZE);
    EXPECT_FALSE(message1->Encode(wBuffer));
    rBuffer.SetIndex(1);
    EXPECT_FALSE(message1->Decode(rBuffer));
}

/**
 * @tc.number   Telephony_SmsMmsGtest_CdmaSmsParameterRecord_0003
 * @tc.name     Test CdmaSmsParameterRecord
 * @tc.desc     Function test
 */
HWTEST_F(BranchCdmaSmsTest, CdmaSmsParameterRecord_0003, Function | MediumTest | Level1)
{
    auto initValue = static_cast<uint32_t>(0);
    auto testValue = static_cast<uint32_t>(5);
    TransportAddr v1;
    memset_s(&v1, sizeof(TransportAddr), 0x00, sizeof(TransportAddr));
    v1.addrLen = testValue;
    TransportAddr v2;
    v2.addrLen = initValue;

    auto message1 = std::make_shared<CdmaSmsAddressParameter>(v1, CdmaSmsParameterRecord::DEST_ADDRESS);
    SmsWriteBuffer wBuffer;
    EXPECT_TRUE(message1->Encode(wBuffer));
    auto buffer = wBuffer.GetPduBuffer();
    EXPECT_GT(buffer->size(), 0);
    std::stringstream ss;
    ss.clear();
    for (uint16_t i = 0; i < buffer->size(); i++) {
        ss << (*buffer)[i];
    }
    SmsReadBuffer rBuffer(ss.str());
    auto message2 = std::make_shared<CdmaSmsAddressParameter>(v2, CdmaSmsParameterRecord::DEST_ADDRESS);
    EXPECT_TRUE(message2->Decode(rBuffer));
    EXPECT_EQ(v2.addrLen, testValue);

    wBuffer.SetIndex(PDU_BUFFER_MAX_SIZE);
    EXPECT_FALSE(message1->Encode(wBuffer));
    rBuffer.SetIndex(1);
    EXPECT_FALSE(message1->Decode(rBuffer));
}

/**
 * @tc.number   Telephony_SmsMmsGtest_CdmaSmsParameterRecord_0004
 * @tc.name     Test CdmaSmsParameterRecord
 * @tc.desc     Function test
 */
HWTEST_F(BranchCdmaSmsTest, CdmaSmsParameterRecord_0004, Function | MediumTest | Level1)
{
    auto initValue = static_cast<uint32_t>(0);
    auto testValue = static_cast<uint32_t>(5);
    TransportSubAddr v1;
    memset_s(&v1, sizeof(TransportSubAddr), 0x00, sizeof(TransportSubAddr));
    v1.addrLen = testValue;
    TransportSubAddr v2;
    v2.addrLen = initValue;

    auto message1 = std::make_shared<CdmaSmsSubaddress>(v1, CdmaSmsParameterRecord::DEST_SUB_ADDRESS);
    SmsWriteBuffer wBuffer;
    EXPECT_TRUE(message1->Encode(wBuffer));
    auto buffer = wBuffer.GetPduBuffer();
    EXPECT_GT(buffer->size(), 0);
    std::stringstream ss;
    ss.clear();
    for (uint16_t i = 0; i < buffer->size(); i++) {
        ss << (*buffer)[i];
    }
    SmsReadBuffer rBuffer(ss.str());
    auto message2 = std::make_shared<CdmaSmsSubaddress>(v2, CdmaSmsParameterRecord::DEST_SUB_ADDRESS);
    EXPECT_TRUE(message2->Decode(rBuffer));
    EXPECT_EQ(v2.addrLen, testValue);

    wBuffer.SetIndex(PDU_BUFFER_MAX_SIZE);
    EXPECT_FALSE(message1->Encode(wBuffer));
    rBuffer.SetIndex(1);
    EXPECT_FALSE(message1->Decode(rBuffer));
}

/**
 * @tc.number   Telephony_SmsMmsGtest_CdmaSmsParameterRecord_0005
 * @tc.name     Test CdmaSmsParameterRecord
 * @tc.desc     Function test
 */
HWTEST_F(BranchCdmaSmsTest, CdmaSmsParameterRecord_0005, Function | MediumTest | Level1)
{
    auto initValue = static_cast<uint8_t>(0);
    auto testValue = static_cast<uint8_t>(5);
    uint8_t v1 = testValue;
    uint8_t v2 = initValue;

    auto message1 = std::make_shared<CdmaSmsBearerReply>(v1);
    SmsWriteBuffer wBuffer;
    EXPECT_TRUE(message1->Encode(wBuffer));
    auto buffer = wBuffer.GetPduBuffer();
    EXPECT_GT(buffer->size(), 0);
    std::stringstream ss;
    ss.clear();
    for (uint16_t i = 0; i < buffer->size(); i++) {
        ss << (*buffer)[i];
    }
    SmsReadBuffer rBuffer(ss.str());
    auto message2 = std::make_shared<CdmaSmsBearerReply>(v2);
    EXPECT_TRUE(message2->Decode(rBuffer));
    EXPECT_EQ(v2, testValue);

    wBuffer.SetIndex(PDU_BUFFER_MAX_SIZE);
    EXPECT_FALSE(message1->Encode(wBuffer));
    rBuffer.SetIndex(1);
    EXPECT_FALSE(message1->Decode(rBuffer));
}

/**
 * @tc.number   Telephony_SmsMmsGtest_CdmaSmsParameterRecord_0006
 * @tc.name     Test CdmaSmsParameterRecord
 * @tc.desc     Function test
 */
HWTEST_F(BranchCdmaSmsTest, CdmaSmsParameterRecord_0006, Function | MediumTest | Level1)
{
    auto initValue = TeleserviceMsgType::RESERVED;
    auto testValue = TeleserviceMsgType::SUBMIT;
    CdmaTeleserviceMsg v1;
    memset_s(&v1, sizeof(CdmaTeleserviceMsg), 0x00, sizeof(CdmaTeleserviceMsg));
    v1.type = testValue;
    CdmaTeleserviceMsg v2;
    v2.type = initValue;

    auto message1 = std::make_shared<CdmaSmsBearerData>(v1);
    SmsWriteBuffer wBuffer;
    EXPECT_TRUE(message1->Encode(wBuffer));
    auto buffer = wBuffer.GetPduBuffer();
    EXPECT_GT(buffer->size(), 0);
    std::stringstream ss;
    ss.clear();
    for (uint16_t i = 0; i < buffer->size(); i++) {
        ss << (*buffer)[i];
    }
    SmsReadBuffer rBuffer(ss.str());
    rBuffer.SetIndex(2);
    auto message2 = std::make_shared<CdmaSmsBearerData>(v2, rBuffer);
    rBuffer.SetIndex(0);
    EXPECT_TRUE(message2->Decode(rBuffer));
    EXPECT_EQ(v2.type, testValue);

    wBuffer.SetIndex(PDU_BUFFER_MAX_SIZE);
    EXPECT_FALSE(message1->Encode(wBuffer));
    rBuffer.SetIndex(1);
    EXPECT_FALSE(message1->Decode(rBuffer));
}

/**
 * @tc.number   Telephony_SmsMmsGtest_CdmaSmsParameterRecord_0007
 * @tc.name     Test CdmaSmsParameterRecord
 * @tc.desc     Function test
 */
HWTEST_F(BranchCdmaSmsTest, CdmaSmsParameterRecord_0007, Function | MediumTest | Level1)
{
    auto initValue = TransportErrClass::NONE;
    auto testValue = TransportErrClass::TEMPORARY;
    TransportCauseCode v1;
    memset_s(&v1, sizeof(TransportCauseCode), 0x00, sizeof(TransportCauseCode));
    v1.errorClass = testValue;
    TransportCauseCode v2;
    v2.errorClass = initValue;

    auto message1 = std::make_shared<CdmaSmsCauseCodes>(v1);
    SmsWriteBuffer wBuffer;
    EXPECT_TRUE(message1->Encode(wBuffer));
    auto buffer = wBuffer.GetPduBuffer();
    EXPECT_GT(buffer->size(), 0);
    std::stringstream ss;
    ss.clear();
    for (uint16_t i = 0; i < buffer->size(); i++) {
        ss << (*buffer)[i];
    }
    SmsReadBuffer rBuffer(ss.str());
    auto message2 = std::make_shared<CdmaSmsCauseCodes>(v2);
    EXPECT_TRUE(message2->Decode(rBuffer));
    EXPECT_EQ(v2.errorClass, testValue);

    wBuffer.SetIndex(PDU_BUFFER_MAX_SIZE);
    EXPECT_FALSE(message1->Encode(wBuffer));
    rBuffer.SetIndex(1);
    EXPECT_FALSE(message1->Decode(rBuffer));
}

/**
 * @tc.number   Telephony_SmsMmsGtest_CdmaSmsSubParameter_0001
 * @tc.name     Test CdmaSmsSubParameter
 * @tc.desc     Function test
 */
HWTEST_F(BranchCdmaSmsTest, CdmaSmsSubParameter_0001, Function | MediumTest | Level1)
{
    auto initValue = static_cast<uint8_t>(0);
    auto testValue = static_cast<uint8_t>(5);
    uint8_t v1 = testValue;
    uint8_t v2 = initValue;

    auto message1 = std::make_shared<CdmaSmsBaseParameter>(CdmaSmsSubParameter::USER_RESPONSE_CODE, v1);
    SmsWriteBuffer wBuffer;
    EXPECT_TRUE(message1->Encode(wBuffer));
    auto buffer = wBuffer.GetPduBuffer();
    EXPECT_GT(buffer->size(), 0);
    std::stringstream ss;
    ss.clear();
    for (uint16_t i = 0; i < buffer->size(); i++) {
        ss << (*buffer)[i];
    }
    SmsReadBuffer rBuffer(ss.str());
    auto message2 = std::make_shared<CdmaSmsBaseParameter>(CdmaSmsSubParameter::USER_RESPONSE_CODE, v2);
    EXPECT_TRUE(message2->Decode(rBuffer));
    EXPECT_EQ(v2, testValue);

    wBuffer.SetIndex(PDU_BUFFER_MAX_SIZE);
    EXPECT_FALSE(message1->Encode(wBuffer));
    rBuffer.SetIndex(1);
    EXPECT_FALSE(message1->Decode(rBuffer));
}

/**
 * @tc.number   Telephony_SmsMmsGtest_CdmaSmsSubParameter_0002
 * @tc.name     Test CdmaSmsSubParameter
 * @tc.desc     Function test
 */
HWTEST_F(BranchCdmaSmsTest, CdmaSmsSubParameter_0002, Function | MediumTest | Level1)
{
    auto message1 = std::make_shared<CdmaSmsReservedParameter>(CdmaSmsSubParameter::USER_RESPONSE_CODE);
    SmsWriteBuffer wBuffer;
    EXPECT_TRUE(message1->Encode(wBuffer));
    std::stringstream ss;
    ss.clear();
    ss << static_cast<uint8_t>(CdmaSmsSubParameter::USER_RESPONSE_CODE);
    ss << static_cast<uint8_t>(0x01);
    ss << static_cast<uint8_t>(0x00);
    SmsReadBuffer rBuffer(ss.str());
    auto message2 = std::make_shared<CdmaSmsReservedParameter>(CdmaSmsSubParameter::USER_RESPONSE_CODE);
    EXPECT_TRUE(message2->Decode(rBuffer));

    wBuffer.SetIndex(PDU_BUFFER_MAX_SIZE);
    EXPECT_TRUE(message1->Encode(wBuffer));
    rBuffer.SetIndex(1);
    EXPECT_FALSE(message1->Decode(rBuffer));
}

/**
 * @tc.number   Telephony_SmsMmsGtest_CdmaSmsSubParameter_0003
 * @tc.name     Test CdmaSmsSubParameter
 * @tc.desc     Function test
 */
HWTEST_F(BranchCdmaSmsTest, CdmaSmsSubParameter_0003, Function | MediumTest | Level1)
{
    auto initValue = static_cast<uint16_t>(0x0);
    auto testValue = static_cast<uint16_t>(0x10);
    SmsTeleSvcMsgId v1;
    memset_s(&v1, sizeof(SmsTeleSvcMsgId), 0x00, sizeof(SmsTeleSvcMsgId));
    v1.msgId = testValue;
    SmsTeleSvcMsgId v2;
    v2.msgId = initValue;
    uint8_t type = static_cast<uint8_t>(TeleserviceMsgType::SUBMIT);

    auto message1 = std::make_shared<CdmaSmsMessageId>(v1, type);
    SmsWriteBuffer wBuffer;
    EXPECT_TRUE(message1->Encode(wBuffer));
    auto buffer = wBuffer.GetPduBuffer();
    EXPECT_GT(buffer->size(), 0);
    std::stringstream ss;
    ss.clear();
    for (uint16_t i = 0; i < buffer->size(); i++) {
        ss << (*buffer)[i];
    }
    SmsReadBuffer rBuffer(ss.str());
    auto message2 = std::make_shared<CdmaSmsMessageId>(v2, type);
    EXPECT_TRUE(message2->Decode(rBuffer));
    EXPECT_EQ(message2->GetMessageType(), type);
    EXPECT_EQ(v2.msgId, testValue);

    wBuffer.SetIndex(PDU_BUFFER_MAX_SIZE);
    EXPECT_FALSE(message1->Encode(wBuffer));
    rBuffer.SetIndex(1);
    EXPECT_FALSE(message1->Decode(rBuffer));
}

/**
 * @tc.number   Telephony_SmsMmsGtest_CdmaSmsSubParameter_0004
 * @tc.name     Test CdmaSmsSubParameter
 * @tc.desc     Function test
 */
HWTEST_F(BranchCdmaSmsTest, CdmaSmsSubParameter_0004, Function | MediumTest | Level1)
{
    auto initValue = SmsEncodingType::RESERVED;
    auto testValue = SmsEncodingType::ASCII_7BIT;
    SmsTeleSvcUserData v1;
    memset_s(&v1, sizeof(SmsTeleSvcUserData), 0x00, sizeof(SmsTeleSvcUserData));
    v1.encodeType = testValue;
    SmsTeleSvcUserData v2;
    v2.encodeType = initValue;
    bool headerInd = false;

    auto message1 = std::make_shared<CdmaSmsUserData>(v1, headerInd);
    SmsWriteBuffer wBuffer;
    EXPECT_TRUE(message1->Encode(wBuffer));
    auto buffer = wBuffer.GetPduBuffer();
    EXPECT_GT(buffer->size(), 0);
    std::stringstream ss;
    ss.clear();
    for (uint16_t i = 0; i < buffer->size(); i++) {
        ss << (*buffer)[i];
    }
    SmsReadBuffer rBuffer(ss.str());
    auto message2 = std::make_shared<CdmaSmsUserData>(v2, headerInd);
    EXPECT_TRUE(message2->Decode(rBuffer));

    EXPECT_EQ(v2.encodeType, testValue);

    wBuffer.SetIndex(0);
    EXPECT_TRUE(message1->EncodeHeader7Bit(wBuffer));
    EXPECT_TRUE(message1->EncodeAscii7Bit(wBuffer));
    EXPECT_TRUE(message1->EncodeGsm7Bit(wBuffer));
    EXPECT_TRUE(message1->EncodeUnicode(wBuffer));

    rBuffer.SetIndex(0);
    EXPECT_EQ(message2->DecodeHeader7Bit(rBuffer), 0);
    EXPECT_TRUE(message2->DecodeAscii7Bit(rBuffer, 0, 0));
    EXPECT_TRUE(message2->DecodeGsm7Bit(rBuffer, 0, 0));

    wBuffer.SetIndex(PDU_BUFFER_MAX_SIZE);
    EXPECT_FALSE(message1->Encode(wBuffer));
    rBuffer.SetIndex(1);
    EXPECT_FALSE(message1->Decode(rBuffer));
}

/**
 * @tc.number   Telephony_SmsMmsGtest_CdmaSmsSubParameter_0005
 * @tc.name     Test CdmaSmsSubParameter
 * @tc.desc     Function test
 */
HWTEST_F(BranchCdmaSmsTest, CdmaSmsSubParameter_0005, Function | MediumTest | Level1)
{
    SmsTeleSvcCmasData v1;
    memset_s(&v1, sizeof(SmsTeleSvcCmasData), 0x00, sizeof(SmsTeleSvcCmasData));
    SmsTeleSvcCmasData v2;

    auto message1 = std::make_shared<CdmaSmsCmasData>(v1);
    SmsWriteBuffer wBuffer;
    EXPECT_FALSE(message1->Encode(wBuffer));

    std::stringstream ss;
    ss.clear();
    ss << static_cast<uint8_t>(CdmaSmsSubParameter::USER_DATA);
    uint8_t len = 100;
    ss << static_cast<uint8_t>(len);
    for (uint8_t i = 0; i < len; i++) {
        ss << static_cast<uint8_t>(0x00);
    }
    SmsReadBuffer rBuffer(ss.str());
    auto message2 = std::make_shared<CdmaSmsCmasData>(v2);
    EXPECT_TRUE(message2->Decode(rBuffer));

    rBuffer.SetIndex(0);
    EXPECT_TRUE(message2->DecodeType0Data(rBuffer));
    EXPECT_TRUE(message2->DecodeType1Data(rBuffer));
    EXPECT_TRUE(message2->DecodeType2Data(rBuffer));
    EXPECT_TRUE(message2->DecodeAbsTime(rBuffer));

    wBuffer.SetIndex(PDU_BUFFER_MAX_SIZE);
    EXPECT_FALSE(message1->Encode(wBuffer));
    rBuffer.SetIndex(1);
    EXPECT_FALSE(message1->Decode(rBuffer));
}

/**
 * @tc.number   Telephony_SmsMmsGtest_CdmaSmsSubParameter_0006
 * @tc.name     Test CdmaSmsSubParameter
 * @tc.desc     Function test
 */
HWTEST_F(BranchCdmaSmsTest, CdmaSmsSubParameter_0006, Function | MediumTest | Level1)
{
    auto initValue = static_cast<unsigned char>(0);
    auto testValue = static_cast<unsigned char>(5);
    SmsTimeAbs v1;
    memset_s(&v1, sizeof(SmsTimeAbs), 0x00, sizeof(SmsTimeAbs));
    v1.month = testValue;
    SmsTimeAbs v2;
    v2.month = initValue;

    auto message1 = std::make_shared<CdmaSmsAbsoluteTime>(CdmaSmsSubParameter::VALIDITY_PERIOD_ABSOLUTE, v1);
    SmsWriteBuffer wBuffer;
    EXPECT_TRUE(message1->Encode(wBuffer));
    auto buffer = wBuffer.GetPduBuffer();
    EXPECT_GT(buffer->size(), 0);
    std::stringstream ss;
    ss.clear();
    for (uint16_t i = 0; i < buffer->size(); i++) {
        ss << (*buffer)[i];
    }
    SmsReadBuffer rBuffer(ss.str());
    auto message2 = std::make_shared<CdmaSmsAbsoluteTime>(CdmaSmsSubParameter::VALIDITY_PERIOD_ABSOLUTE, v2);
    EXPECT_TRUE(message2->Decode(rBuffer));
    EXPECT_EQ(v2.month, testValue);

    wBuffer.SetIndex(PDU_BUFFER_MAX_SIZE);
    EXPECT_FALSE(message1->Encode(wBuffer));
    rBuffer.SetIndex(1);
    EXPECT_FALSE(message1->Decode(rBuffer));
}

/**
 * @tc.number   Telephony_SmsMmsGtest_CdmaSmsSubParameter_0007
 * @tc.name     Test CdmaSmsSubParameter
 * @tc.desc     Function test
 */
HWTEST_F(BranchCdmaSmsTest, CdmaSmsSubParameter_0007, Function | MediumTest | Level1)
{
    auto initValue = SmsPriorityIndicator::RESERVED;
    auto testValue = SmsPriorityIndicator::NORMAL;
    SmsPriorityIndicator v1 = testValue;
    SmsPriorityIndicator v2 = initValue;

    auto message1 = std::make_shared<CdmaSmsPriorityInd>(v1);
    SmsWriteBuffer wBuffer;
    EXPECT_TRUE(message1->Encode(wBuffer));
    auto buffer = wBuffer.GetPduBuffer();
    EXPECT_GT(buffer->size(), 0);
    std::stringstream ss;
    ss.clear();
    for (uint16_t i = 0; i < buffer->size(); i++) {
        ss << (*buffer)[i];
    }
    SmsReadBuffer rBuffer(ss.str());
    auto message2 = std::make_shared<CdmaSmsPriorityInd>(v2);
    EXPECT_TRUE(message2->Decode(rBuffer));
    EXPECT_EQ(v2, testValue);

    wBuffer.SetIndex(PDU_BUFFER_MAX_SIZE);
    EXPECT_FALSE(message1->Encode(wBuffer));
    rBuffer.SetIndex(1);
    EXPECT_FALSE(message1->Decode(rBuffer));
}

/**
 * @tc.number   Telephony_SmsMmsGtest_CdmaSmsSubParameter_0008
 * @tc.name     Test CdmaSmsSubParameter
 * @tc.desc     Function test
 */
HWTEST_F(BranchCdmaSmsTest, CdmaSmsSubParameter_0008, Function | MediumTest | Level1)
{
    auto initValue = SmsPrivacyIndicator::NOT_RESTRICTED;
    auto testValue = SmsPrivacyIndicator::RESTRICTED;
    SmsPrivacyIndicator v1 = testValue;
    SmsPrivacyIndicator v2 = initValue;

    auto message1 = std::make_shared<CdmaSmsPrivacyInd>(v1);
    SmsWriteBuffer wBuffer;
    EXPECT_TRUE(message1->Encode(wBuffer));
    auto buffer = wBuffer.GetPduBuffer();
    EXPECT_GT(buffer->size(), 0);
    std::stringstream ss;
    ss.clear();
    for (uint16_t i = 0; i < buffer->size(); i++) {
        ss << (*buffer)[i];
    }
    SmsReadBuffer rBuffer(ss.str());
    auto message2 = std::make_shared<CdmaSmsPrivacyInd>(v2);
    EXPECT_TRUE(message2->Decode(rBuffer));
    EXPECT_EQ(v2, testValue);

    wBuffer.SetIndex(PDU_BUFFER_MAX_SIZE);
    EXPECT_FALSE(message1->Encode(wBuffer));
    rBuffer.SetIndex(1);
    EXPECT_FALSE(message1->Decode(rBuffer));
}

/**
 * @tc.number   Telephony_SmsMmsGtest_CdmaSmsSubParameter_0009
 * @tc.name     Test CdmaSmsSubParameter
 * @tc.desc     Function test
 */
HWTEST_F(BranchCdmaSmsTest, CdmaSmsSubParameter_0009, Function | MediumTest | Level1)
{
    auto initValue = false;
    auto testValue = true;
    SmsReplyOption v1;
    memset_s(&v1, sizeof(SmsReplyOption), 0x00, sizeof(SmsReplyOption));
    v1.dak = testValue;
    SmsReplyOption v2;
    v2.dak = initValue;

    auto message1 = std::make_shared<CdmaSmsReplyOption>(v1);
    SmsWriteBuffer wBuffer;
    EXPECT_TRUE(message1->Encode(wBuffer));
    auto buffer = wBuffer.GetPduBuffer();
    EXPECT_GT(buffer->size(), 0);
    std::stringstream ss;
    ss.clear();
    for (uint16_t i = 0; i < buffer->size(); i++) {
        ss << (*buffer)[i];
    }
    SmsReadBuffer rBuffer(ss.str());
    auto message2 = std::make_shared<CdmaSmsReplyOption>(v2);
    EXPECT_TRUE(message2->Decode(rBuffer));
    EXPECT_EQ(v2.dak, testValue);

    wBuffer.SetIndex(PDU_BUFFER_MAX_SIZE);
    EXPECT_FALSE(message1->Encode(wBuffer));
    rBuffer.SetIndex(1);
    EXPECT_FALSE(message1->Decode(rBuffer));
}

/**
 * @tc.number   Telephony_SmsMmsGtest_CdmaSmsSubParameter_0010
 * @tc.name     Test CdmaSmsSubParameter
 * @tc.desc     Function test
 */
HWTEST_F(BranchCdmaSmsTest, CdmaSmsSubParameter_0010, Function | MediumTest | Level1)
{
    auto initValue = SmsAlertPriority::DEFAULT;
    auto testValue = SmsAlertPriority::HIGH;
    SmsAlertPriority v1 = testValue;
    SmsAlertPriority v2 = initValue;

    auto message1 = std::make_shared<CdmaSmsAlertPriority>(v1);
    SmsWriteBuffer wBuffer;
    EXPECT_TRUE(message1->Encode(wBuffer));
    auto buffer = wBuffer.GetPduBuffer();
    EXPECT_GT(buffer->size(), 0);
    std::stringstream ss;
    ss.clear();
    for (uint16_t i = 0; i < buffer->size(); i++) {
        ss << (*buffer)[i];
    }
    SmsReadBuffer rBuffer(ss.str());
    auto message2 = std::make_shared<CdmaSmsAlertPriority>(v2);
    EXPECT_TRUE(message2->Decode(rBuffer));
    EXPECT_EQ(v2, testValue);

    wBuffer.SetIndex(PDU_BUFFER_MAX_SIZE);
    EXPECT_FALSE(message1->Encode(wBuffer));
    rBuffer.SetIndex(1);
    EXPECT_FALSE(message1->Decode(rBuffer));
}

/**
 * @tc.number   Telephony_SmsMmsGtest_CdmaSmsSubParameter_0011
 * @tc.name     Test CdmaSmsSubParameter
 * @tc.desc     Function test
 */
HWTEST_F(BranchCdmaSmsTest, CdmaSmsSubParameter_0011, Function | MediumTest | Level1)
{
    auto initValue = SmsLanguageType::UNKNOWN;
    auto testValue = SmsLanguageType::CHINESE;
    SmsLanguageType v1 = testValue;
    SmsLanguageType v2 = initValue;

    auto message1 = std::make_shared<CdmaSmsLanguageInd>(v1);
    SmsWriteBuffer wBuffer;
    EXPECT_TRUE(message1->Encode(wBuffer));
    auto buffer = wBuffer.GetPduBuffer();
    EXPECT_GT(buffer->size(), 0);
    std::stringstream ss;
    ss.clear();
    for (uint16_t i = 0; i < buffer->size(); i++) {
        ss << (*buffer)[i];
    }
    SmsReadBuffer rBuffer(ss.str());
    auto message2 = std::make_shared<CdmaSmsLanguageInd>(v2);
    EXPECT_TRUE(message2->Decode(rBuffer));
    EXPECT_EQ(v2, testValue);

    wBuffer.SetIndex(PDU_BUFFER_MAX_SIZE);
    EXPECT_FALSE(message1->Encode(wBuffer));
    rBuffer.SetIndex(1);
    EXPECT_FALSE(message1->Decode(rBuffer));
}

/**
 * @tc.number   Telephony_SmsMmsGtest_CdmaSmsSubParameter_0012
 * @tc.name     Test CdmaSmsSubParameter
 * @tc.desc     Function test
 */
HWTEST_F(BranchCdmaSmsTest, CdmaSmsSubParameter_0012, Function | MediumTest | Level1)
{
    auto initValue = static_cast<uint32_t>(0);
    auto testValue = static_cast<uint32_t>(5);
    SmsTeleSvcAddr v1;
    memset_s(&v1, sizeof(SmsTeleSvcAddr), 0x00, sizeof(SmsTeleSvcAddr));
    v1.addrLen = testValue;
    SmsTeleSvcAddr v2;
    v2.addrLen = initValue;

    auto message1 = std::make_shared<CdmaSmsCallbackNumber>(v1);
    SmsWriteBuffer wBuffer;
    EXPECT_TRUE(message1->Encode(wBuffer));
    auto buffer = wBuffer.GetPduBuffer();
    EXPECT_GT(buffer->size(), 0);
    std::stringstream ss;
    ss.clear();
    for (uint16_t i = 0; i < buffer->size(); i++) {
        ss << (*buffer)[i];
    }
    SmsReadBuffer rBuffer(ss.str());
    auto message2 = std::make_shared<CdmaSmsCallbackNumber>(v2);
    EXPECT_TRUE(message2->Decode(rBuffer));
    EXPECT_EQ(v2.addrLen, testValue);

    wBuffer.SetIndex(PDU_BUFFER_MAX_SIZE);
    EXPECT_FALSE(message1->Encode(wBuffer));
    rBuffer.SetIndex(1);
    EXPECT_FALSE(message1->Decode(rBuffer));
}

/**
 * @tc.number   Telephony_SmsMmsGtest_CdmaSmsSubParameter_0013
 * @tc.name     Test CdmaSmsSubParameter
 * @tc.desc     Function test
 */
HWTEST_F(BranchCdmaSmsTest, CdmaSmsSubParameter_0013, Function | MediumTest | Level1)
{
    uint16_t initValue = 0;
    uint16_t testValue = 5;
    uint16_t v1 = testValue;
    uint16_t v2 = initValue;

    auto message1 = std::make_shared<CdmaSmsDepositIndex>(v1);
    SmsWriteBuffer wBuffer;
    EXPECT_TRUE(message1->Encode(wBuffer));
    auto buffer = wBuffer.GetPduBuffer();
    EXPECT_GT(buffer->size(), 0);
    std::stringstream ss;
    ss.clear();
    for (uint16_t i = 0; i < buffer->size(); i++) {
        ss << (*buffer)[i];
    }
    SmsReadBuffer rBuffer(ss.str());
    auto message2 = std::make_shared<CdmaSmsDepositIndex>(v2);
    EXPECT_TRUE(message2->Decode(rBuffer));
    EXPECT_EQ(v2, testValue);

    wBuffer.SetIndex(PDU_BUFFER_MAX_SIZE);
    EXPECT_FALSE(message1->Encode(wBuffer));
    rBuffer.SetIndex(1);
    EXPECT_FALSE(message1->Decode(rBuffer));
}

/**
 * @tc.number   Telephony_SmsMmsGtest_CdmaSmsSubParameter_0014
 * @tc.name     Test CdmaSmsSubParameter
 * @tc.desc     Function test
 */
HWTEST_F(BranchCdmaSmsTest, CdmaSmsSubParameter_0014, Function | MediumTest | Level1)
{
    auto initValue = SmsDisplayMode::RESERVED;
    auto testValue = SmsDisplayMode::DEFAULT_SETTING;
    SmsDisplayMode v1 = testValue;
    SmsDisplayMode v2 = initValue;

    auto message1 = std::make_shared<CdmaSmsDisplayMode>(v1);
    SmsWriteBuffer wBuffer;
    EXPECT_TRUE(message1->Encode(wBuffer));
    auto buffer = wBuffer.GetPduBuffer();
    EXPECT_GT(buffer->size(), 0);
    std::stringstream ss;
    ss.clear();
    for (uint16_t i = 0; i < buffer->size(); i++) {
        ss << (*buffer)[i];
    }
    SmsReadBuffer rBuffer(ss.str());
    auto message2 = std::make_shared<CdmaSmsDisplayMode>(v2);
    EXPECT_TRUE(message2->Decode(rBuffer));
    EXPECT_EQ(v2, testValue);

    wBuffer.SetIndex(PDU_BUFFER_MAX_SIZE);
    EXPECT_FALSE(message1->Encode(wBuffer));
    rBuffer.SetIndex(1);
    EXPECT_FALSE(message1->Decode(rBuffer));
}

/**
 * @tc.number   Telephony_SmsMmsGtest_CdmaSmsSubParameter_0015
 * @tc.name     Test CdmaSmsSubParameter
 * @tc.desc     Function test
 */
HWTEST_F(BranchCdmaSmsTest, CdmaSmsSubParameter_0015, Function | MediumTest | Level1)
{
    SmsEnhancedVmn v1;
    SmsEnhancedVmn v2;

    auto message1 = std::make_shared<CdmaSmsEnhancedVmn>(v1);
    SmsWriteBuffer wBuffer;
    EXPECT_FALSE(message1->Encode(wBuffer));
    std::stringstream ss;
    ss.clear();
    ss << static_cast<uint8_t>(CdmaSmsSubParameter::ENHANCED_VMN);
    uint8_t len = 20;
    ss << static_cast<uint8_t>(len);
    for (uint8_t i = 0; i < len; i++) {
        ss << static_cast<uint8_t>(0x00);
    }
    SmsReadBuffer rBuffer(ss.str());
    auto message2 = std::make_shared<CdmaSmsEnhancedVmn>(v2);
    EXPECT_TRUE(message2->Decode(rBuffer));

    rBuffer.SetIndex(0);
    EXPECT_TRUE(message2->DecodeHeader(rBuffer));
    EXPECT_TRUE(message2->DecodeAccessNumber(rBuffer));
    EXPECT_TRUE(message2->DecodeCallingPartyNumber(rBuffer));

    wBuffer.SetIndex(PDU_BUFFER_MAX_SIZE);
    EXPECT_FALSE(message1->Encode(wBuffer));
    rBuffer.SetIndex(1);
    EXPECT_FALSE(message1->Decode(rBuffer));
}

/**
 * @tc.number   Telephony_SmsMmsGtest_CdmaSmsSubParameter_0016
 * @tc.name     Test CdmaSmsSubParameter
 * @tc.desc     Function test
 */
HWTEST_F(BranchCdmaSmsTest, CdmaSmsSubParameter_0016, Function | MediumTest | Level1)
{
    SmsEnhancedVmnAck v1;
    SmsEnhancedVmnAck v2;

    auto message1 = std::make_shared<CdmaSmsEnhancedVmnAck>(v1);
    SmsWriteBuffer wBuffer;
    EXPECT_FALSE(message1->Encode(wBuffer));
    std::stringstream ss;
    ss.clear();
    ss << static_cast<uint8_t>(CdmaSmsSubParameter::ENHANCED_VMN_ACK);
    ss << static_cast<uint8_t>(0x05);
    ss << static_cast<uint8_t>(0x00);
    ss << static_cast<uint8_t>(0x00);
    ss << static_cast<uint8_t>(0x00);
    ss << static_cast<uint8_t>(0x00);
    ss << static_cast<uint8_t>(0x00);
    SmsReadBuffer rBuffer(ss.str());
    auto message2 = std::make_shared<CdmaSmsEnhancedVmnAck>(v2);
    EXPECT_TRUE(message2->Decode(rBuffer));

    wBuffer.SetIndex(PDU_BUFFER_MAX_SIZE);
    EXPECT_FALSE(message1->Encode(wBuffer));
    rBuffer.SetIndex(1);
    EXPECT_FALSE(message1->Decode(rBuffer));
}

/**
 * @tc.number   Telephony_SmsMmsGtest_CdmaSmsSubParameter_0017
 * @tc.name     Test CdmaSmsSubParameter
 * @tc.desc     Function test
 */
HWTEST_F(BranchCdmaSmsTest, CdmaSmsSubParameter_0017, Function | MediumTest | Level1)
{
    SmsStatusCode v1;
    SmsStatusCode v2;

    auto message1 = std::make_shared<CdmaSmsMessageStatus>(v1);
    SmsWriteBuffer wBuffer;
    EXPECT_FALSE(message1->Encode(wBuffer));
    std::stringstream ss;
    ss.clear();
    ss << static_cast<uint8_t>(CdmaSmsSubParameter::MESSAGE_STATUS);
    ss << static_cast<uint8_t>(0x01);
    ss << static_cast<uint8_t>(0x00);
    SmsReadBuffer rBuffer(ss.str());
    auto message2 = std::make_shared<CdmaSmsMessageStatus>(v2);
    EXPECT_TRUE(message2->Decode(rBuffer));

    wBuffer.SetIndex(PDU_BUFFER_MAX_SIZE);
    EXPECT_FALSE(message1->Encode(wBuffer));
    rBuffer.SetIndex(1);
    EXPECT_FALSE(message1->Decode(rBuffer));
}

/**
 * @tc.number   Telephony_SmsMmsGtest_CdmaSmsSubParameter_0018
 * @tc.name     Test CdmaSmsSubParameter
 * @tc.desc     Function test
 */
HWTEST_F(BranchCdmaSmsTest, CdmaSmsSubParameter_0018, Function | MediumTest | Level1)
{
    uint32_t v1;
    uint32_t v2;

    auto message1 = std::make_shared<CdmaSmsNumberMessages>(v1);
    SmsWriteBuffer wBuffer;
    EXPECT_FALSE(message1->Encode(wBuffer));
    std::stringstream ss;
    ss.clear();
    ss << static_cast<uint8_t>(CdmaSmsSubParameter::NUMBER_OF_MESSAGES);
    ss << static_cast<uint8_t>(0x01);
    ss << static_cast<uint8_t>(0x00);
    SmsReadBuffer rBuffer(ss.str());
    auto message2 = std::make_shared<CdmaSmsNumberMessages>(v2);
    EXPECT_TRUE(message2->Decode(rBuffer));

    wBuffer.SetIndex(PDU_BUFFER_MAX_SIZE);
    EXPECT_FALSE(message1->Encode(wBuffer));
    rBuffer.SetIndex(1);
    EXPECT_FALSE(message1->Decode(rBuffer));
}
} // namespace Telephony
} // namespace OHOS
