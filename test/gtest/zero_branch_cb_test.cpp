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

#include "gsm_cb_codec.h"
#include "gsm_cb_gsm_codec.h"
#include "gsm_cb_umts_codec.h"
#include "gtest/gtest.h"
#include "radio_event.h"
#include "sms_service.h"

namespace OHOS {
namespace Telephony {
using namespace testing::ext;

namespace {
const int32_t INVALID_SLOTID = 2;
const uint16_t PWS_FIRST_ID = 0x1100;
const std::string ETWS_PDU = "000B1100011165FA7D4E9BD564";
const std::string CMAS_PDU = "000D11120111E376784E9BDD60";
const std::string CBS_PDU = "00031112011163F19C36BBC11A";
const std::string CMAS_JP_PDU =
    "01A41F51101102EA3030A830EA30A230E130FC30EB914D4FE130C630B930C8000D000A3053308C306F8A669A137528306E30E130C330BB30FC"
    "30B8306730593002000D000AFF080032003000310033002F00310031002F003252EA300037002000310035003A00340034FF09000D000AFF08"
    "30A830EA30A25E02FF090000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
    "0022";
} // namespace

class BranchCbTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};
void BranchCbTest::SetUpTestCase() {}

void BranchCbTest::TearDownTestCase() {}

void BranchCbTest::SetUp() {}

void BranchCbTest::TearDown() {}

/**
 * @tc.number   Telephony_SmsMmsGtest_GsmCbCodec_0001
 * @tc.name     Test GsmCbCodec
 * @tc.desc     Function test
 */
HWTEST_F(BranchCbTest, GsmCbCodec_0001, Function | MediumTest | Level1)
{
    std::vector<unsigned char> pdu;
    unsigned char data = 1;
    pdu.push_back(data);
    auto cbCodec = std::make_shared<GsmCbCodec>();
    cbCodec->PduAnalysis(pdu);
    int8_t format = 1;
    EXPECT_TRUE(cbCodec->GetFormat(format));
    EXPECT_TRUE(cbCodec->GetPriority(format));
    EXPECT_NE(cbCodec->GsmCbCodec::ToString(), "");
    uint8_t gs = 1;
    EXPECT_TRUE(cbCodec->GetGeoScope(gs));
    uint16_t serial = 1;
    EXPECT_TRUE(cbCodec->GetSerialNum(serial));
    bool isUserAlert = false;
    EXPECT_TRUE(cbCodec->IsEtwsEmergencyUserAlert(isUserAlert));
    EXPECT_TRUE(cbCodec->IsEtwsPopupAlert(isUserAlert));
    EXPECT_TRUE(cbCodec->GetServiceCategory(serial));
    uint8_t cmasType;
    EXPECT_TRUE(cbCodec->GetCmasResponseType(cmasType));
    uint8_t cmasClass;
    EXPECT_TRUE(cbCodec->GetCmasMessageClass(cmasClass));
    EXPECT_TRUE(cbCodec->GetWarningType(serial));
    EXPECT_TRUE(cbCodec->GetMsgType(gs));
    EXPECT_TRUE(cbCodec->GetLangType(gs));
    EXPECT_TRUE(cbCodec->GetDcs(gs));
    int64_t recvTime = 0;
    EXPECT_TRUE(cbCodec->GetReceiveTime(recvTime));
}

/**
 * @tc.number   Telephony_SmsMmsGtest_GsmCbCodec_0002
 * @tc.name     Test GsmCbCodec
 * @tc.desc     Function test
 */
HWTEST_F(BranchCbTest, GsmCbCodec_0002, Function | MediumTest | Level1)
{
    std::vector<unsigned char> pdu;
    unsigned char data = 1;
    pdu.push_back(data);
    auto cbCodec = std::make_shared<GsmCbCodec>();
    cbCodec->PduAnalysis(pdu);
    uint8_t cmasType;
    cbCodec->GetCmasCategory(cmasType);
    std::string raw = "qwe";
    std::string message = "asd";
    cbCodec->ConvertToUTF8(raw, message);
    cbCodec->cbHeader_ = std::make_shared<GsmCbCodec::GsmCbMessageHeader>();
    cbCodec->cbHeader_->dcs.codingScheme = DATA_CODING_7BIT;
    cbCodec->ConvertToUTF8(raw, message);
    cbCodec->cbHeader_->dcs.codingScheme = DATA_CODING_UCS2;
    cbCodec->ConvertToUTF8(raw, message);
    cbCodec->cbHeader_->dcs.codingScheme = DATA_CODING_AUTO;
    cbCodec->ConvertToUTF8(raw, message);
    cbCodec->cbHeader_->bEtwsMessage = true;
    cbCodec->cbHeader_->cbEtwsType = GsmCbCodec::ETWS_PRIMARY;
    cbCodec->ConvertToUTF8(raw, message);
    cbCodec->GetCbMessageRaw();
    int8_t format = 1;
    EXPECT_TRUE(cbCodec->GetPriority(format));
    cbCodec->cbHeader_->msgId = PWS_FIRST_ID;
    EXPECT_TRUE(cbCodec->GetPriority(format));
    EXPECT_FALSE(cbCodec->IsSinglePageMsg());
    std::string pdus = "123";
    EXPECT_TRUE(cbCodec->CreateCbMessage(pdu) == nullptr);
    EXPECT_FALSE(cbCodec->CreateCbMessage(pdus));
    EXPECT_TRUE(cbCodec->GetCbHeader() != nullptr);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_GsmCbCodec_0003
 * @tc.name     Test GsmCbCodec
 * @tc.desc     Function test
 */
HWTEST_F(BranchCbTest, GsmCbCodec_0003, Function | MediumTest | Level1)
{
    auto smsCbMessage = std::make_shared<GsmCbCodec>();
    uint8_t severity = 1;
    smsCbMessage->cbHeader_ = std::make_shared<GsmCbCodec::GsmCbMessageHeader>();
    smsCbMessage->cbHeader_->msgId = GsmCbCodec::CmasMsgType::EXTREME_OBSERVED;
    EXPECT_TRUE(smsCbMessage->GetCmasSeverity(severity));
    EXPECT_TRUE(smsCbMessage->GetCmasUrgency(severity));
    smsCbMessage->cbHeader_->msgId = GsmCbCodec::CmasMsgType::EXTREME_OBSERVED_SPANISH;
    EXPECT_TRUE(smsCbMessage->GetCmasSeverity(severity));
    EXPECT_TRUE(smsCbMessage->GetCmasUrgency(severity));
    smsCbMessage->cbHeader_->msgId = GsmCbCodec::CmasMsgType::EXTREME_LIKELY;
    EXPECT_TRUE(smsCbMessage->GetCmasSeverity(severity));
    EXPECT_TRUE(smsCbMessage->GetCmasUrgency(severity));
    smsCbMessage->cbHeader_->msgId = GsmCbCodec::CmasMsgType::EXTREME_LIKELY_SPANISH;
    EXPECT_TRUE(smsCbMessage->GetCmasSeverity(severity));
    EXPECT_TRUE(smsCbMessage->GetCmasUrgency(severity));
    smsCbMessage->cbHeader_->msgId = GsmCbCodec::CmasMsgType::EXTREME_OBSERVED;
    EXPECT_TRUE(smsCbMessage->GetCmasSeverity(severity));
    EXPECT_TRUE(smsCbMessage->GetCmasUrgency(severity));
    smsCbMessage->cbHeader_->msgId = GsmCbCodec::CmasMsgType::SEVERE_OBSERVED_SPANISH;
    EXPECT_TRUE(smsCbMessage->GetCmasSeverity(severity));
    EXPECT_TRUE(smsCbMessage->GetCmasUrgency(severity));
    smsCbMessage->cbHeader_->msgId = GsmCbCodec::CmasMsgType::SEVERE_LIKELY;
    EXPECT_TRUE(smsCbMessage->GetCmasSeverity(severity));
    EXPECT_TRUE(smsCbMessage->GetCmasUrgency(severity));
    smsCbMessage->cbHeader_->msgId = GsmCbCodec::CmasMsgType::SEVERE_LIKELY_SPANISH;
    EXPECT_TRUE(smsCbMessage->GetCmasSeverity(severity));
    EXPECT_TRUE(smsCbMessage->GetCmasUrgency(severity));
    smsCbMessage->cbHeader_->msgId = GsmCbCodec::CmasMsgType::ALERT_OBSERVED_DEFUALT;
    EXPECT_TRUE(smsCbMessage->GetCmasSeverity(severity));
    EXPECT_TRUE(smsCbMessage->GetCmasUrgency(severity));
    smsCbMessage->cbHeader_->msgId = GsmCbCodec::CmasMsgType::ALERT_OBSERVED_SPANISH;
    EXPECT_TRUE(smsCbMessage->GetCmasSeverity(severity));
    EXPECT_TRUE(smsCbMessage->GetCmasUrgency(severity));
    smsCbMessage->cbHeader_->msgId = GsmCbCodec::CmasMsgType::ALERT_LIKELY;
    EXPECT_TRUE(smsCbMessage->GetCmasSeverity(severity));
    EXPECT_TRUE(smsCbMessage->GetCmasUrgency(severity));
    smsCbMessage->cbHeader_->msgId = GsmCbCodec::CmasMsgType::ALERT_LIKELY_SPANISH;
    EXPECT_TRUE(smsCbMessage->GetCmasSeverity(severity));
    EXPECT_TRUE(smsCbMessage->GetCmasUrgency(severity));
    smsCbMessage->cbHeader_->msgId = GsmCbCodec::CmasMsgType::EXPECTED_OBSERVED;
    EXPECT_TRUE(smsCbMessage->GetCmasSeverity(severity));
    EXPECT_TRUE(smsCbMessage->GetCmasUrgency(severity));
    smsCbMessage->cbHeader_->msgId = GsmCbCodec::CmasMsgType::EXPECTED_OBSERVED_SPANISH;
    EXPECT_TRUE(smsCbMessage->GetCmasSeverity(severity));
    EXPECT_TRUE(smsCbMessage->GetCmasUrgency(severity));
    smsCbMessage->cbHeader_->msgId = GsmCbCodec::CmasMsgType::EXPECTED_LIKELY;
    EXPECT_TRUE(smsCbMessage->GetCmasSeverity(severity));
    EXPECT_TRUE(smsCbMessage->GetCmasUrgency(severity));
}

/**
 * @tc.number   Telephony_SmsMmsGtest_GsmCbCodec_0004
 * @tc.name     Test GsmCbCodec
 * @tc.desc     Function test
 */
HWTEST_F(BranchCbTest, GsmCbCodec_0004, Function | MediumTest | Level1)
{
    auto smsCbMessage = std::make_shared<GsmCbCodec>();
    uint8_t severity = 1;
    smsCbMessage->cbHeader_ = std::make_shared<GsmCbCodec::GsmCbMessageHeader>();
    smsCbMessage->cbHeader_->msgId = GsmCbCodec::CmasMsgType::EXPECTED_LIKELY_SPANISH;
    EXPECT_TRUE(smsCbMessage->GetCmasSeverity(severity));
    EXPECT_TRUE(smsCbMessage->GetCmasUrgency(severity));
    EXPECT_TRUE(smsCbMessage->GetCmasCertainty(severity));
    smsCbMessage->cbHeader_->msgId = GsmCbCodec::CmasMsgType::OPERATOR_ALERT_SPANISH;
    EXPECT_TRUE(smsCbMessage->GetCmasSeverity(severity));
    EXPECT_TRUE(smsCbMessage->GetCmasUrgency(severity));
    EXPECT_TRUE(smsCbMessage->GetCmasCertainty(severity));
    smsCbMessage->cbHeader_->msgId = GsmCbCodec::CmasMsgType::EXTREME_OBSERVED;
    EXPECT_TRUE(smsCbMessage->GetCmasCertainty(severity));
    smsCbMessage->cbHeader_->msgId = GsmCbCodec::CmasMsgType::EXTREME_OBSERVED_SPANISH;
    EXPECT_TRUE(smsCbMessage->GetCmasCertainty(severity));
    smsCbMessage->cbHeader_->msgId = GsmCbCodec::CmasMsgType::SEVERE_OBSERVED;
    EXPECT_TRUE(smsCbMessage->GetCmasCertainty(severity));
    smsCbMessage->cbHeader_->msgId = GsmCbCodec::CmasMsgType::SEVERE_OBSERVED_SPANISH;
    EXPECT_TRUE(smsCbMessage->GetCmasCertainty(severity));
    smsCbMessage->cbHeader_->msgId = GsmCbCodec::CmasMsgType::ALERT_OBSERVED_DEFUALT;
    EXPECT_TRUE(smsCbMessage->GetCmasCertainty(severity));
    smsCbMessage->cbHeader_->msgId = GsmCbCodec::CmasMsgType::ALERT_OBSERVED_SPANISH;
    EXPECT_TRUE(smsCbMessage->GetCmasCertainty(severity));
    smsCbMessage->cbHeader_->msgId = GsmCbCodec::CmasMsgType::EXPECTED_OBSERVED;
    EXPECT_TRUE(smsCbMessage->GetCmasCertainty(severity));
    smsCbMessage->cbHeader_->msgId = GsmCbCodec::CmasMsgType::EXPECTED_OBSERVED_SPANISH;
    EXPECT_TRUE(smsCbMessage->GetCmasCertainty(severity));
    smsCbMessage->cbHeader_->msgId = GsmCbCodec::CmasMsgType::EXTREME_LIKELY;
    EXPECT_TRUE(smsCbMessage->GetCmasCertainty(severity));
    smsCbMessage->cbHeader_->msgId = GsmCbCodec::CmasMsgType::EXTREME_LIKELY_SPANISH;
    EXPECT_TRUE(smsCbMessage->GetCmasCertainty(severity));
    smsCbMessage->cbHeader_->msgId = GsmCbCodec::CmasMsgType::SEVERE_LIKELY;
    EXPECT_TRUE(smsCbMessage->GetCmasCertainty(severity));
    smsCbMessage->cbHeader_->msgId = GsmCbCodec::CmasMsgType::SEVERE_LIKELY_SPANISH;
    EXPECT_TRUE(smsCbMessage->GetCmasCertainty(severity));
    smsCbMessage->cbHeader_->msgId = GsmCbCodec::CmasMsgType::ALERT_LIKELY;
    EXPECT_TRUE(smsCbMessage->GetCmasCertainty(severity));
    smsCbMessage->cbHeader_->msgId = GsmCbCodec::CmasMsgType::ALERT_LIKELY_SPANISH;
    EXPECT_TRUE(smsCbMessage->GetCmasCertainty(severity));
    smsCbMessage->cbHeader_->msgId = GsmCbCodec::CmasMsgType::EXPECTED_LIKELY;
    EXPECT_TRUE(smsCbMessage->GetCmasCertainty(severity));
}

/**
 * @tc.number   Telephony_SmsMmsGtest_GsmCbCodec_0005
 * @tc.name     Test GsmCbCodec
 * @tc.desc     Function test
 */
HWTEST_F(BranchCbTest, GsmCbCodec_0005, Function | MediumTest | Level1)
{
    auto cbMsg = std::make_shared<GsmCbCodec>();
    auto gsmMsg = std::make_shared<GsmCbGsmCodec>(cbMsg->cbHeader_, cbMsg->cbPduBuffer_, cbMsg);
    auto umtsMsg = std::make_shared<GsmCbUmtsCodec>(cbMsg->cbHeader_, cbMsg->cbPduBuffer_, cbMsg);
    if (cbMsg == nullptr || gsmMsg == nullptr || umtsMsg == nullptr) {
        EXPECT_TRUE(false);
        return;
    }
    EXPECT_FALSE(gsmMsg->Decode2gHeader());
    unsigned char data = 1;
    std::vector<unsigned char> pdu;
    pdu.push_back(data);
    cbMsg->PduAnalysis(pdu);
    gsmMsg->Decode2gCbMsg();
    umtsMsg->Decode3gCbMsg();
    cbMsg->cbHeader_ = std::make_shared<GsmCbCodec::GsmCbMessageHeader>();
    cbMsg->cbHeader_->dcs.codingScheme = DataCodingScheme::DATA_CODING_7BIT;
    gsmMsg->Decode2gCbMsg();
    umtsMsg->Decode3gCbMsg();
    cbMsg->cbHeader_->dcs.codingScheme = DataCodingScheme::DATA_CODING_8BIT;
    gsmMsg->Decode2gCbMsg();
    umtsMsg->Decode3gCbMsg();
    cbMsg->cbHeader_->dcs.codingScheme = DataCodingScheme::DATA_CODING_UCS2;
    gsmMsg->Decode2gCbMsg();
    umtsMsg->Decode3gCbMsg();
    cbMsg->cbHeader_->dcs.codingScheme = DataCodingScheme::DATA_CODING_ASCII7BIT;
    gsmMsg->Decode2gCbMsg();
    umtsMsg->Decode3gCbMsg();
    cbMsg->cbHeader_->totalPages = 1;
    umtsMsg->Decode3g7Bit();
    umtsMsg->Decode3gUCS2();
    EXPECT_EQ(cbMsg->CMASClass(GsmCbCodec::PRESIDENTIAL), GsmCbCodec::GSMCbMsgSubType::CMAS_PRESIDENTIAL);
    EXPECT_EQ(cbMsg->CMASClass(GsmCbCodec::PRESIDENTIAL_SPANISH), GsmCbCodec::GSMCbMsgSubType::CMAS_PRESIDENTIAL);
    EXPECT_EQ(cbMsg->CMASClass(GsmCbCodec::EXTREME_OBSERVED), GsmCbCodec::GSMCbMsgSubType::CMAS_EXTREME);
    EXPECT_EQ(cbMsg->CMASClass(GsmCbCodec::EXTREME_OBSERVED_SPANISH), GsmCbCodec::GSMCbMsgSubType::CMAS_EXTREME);
    EXPECT_EQ(cbMsg->CMASClass(GsmCbCodec::EXTREME_LIKELY), GsmCbCodec::GSMCbMsgSubType::CMAS_EXTREME);
    EXPECT_EQ(cbMsg->CMASClass(GsmCbCodec::EXTREME_LIKELY_SPANISH), GsmCbCodec::GSMCbMsgSubType::CMAS_EXTREME);
    EXPECT_EQ(cbMsg->CMASClass(GsmCbCodec::SEVERE_OBSERVED), GsmCbCodec::GSMCbMsgSubType::CMAS_SEVERE);
    EXPECT_EQ(cbMsg->CMASClass(GsmCbCodec::SEVERE_OBSERVED_SPANISH), GsmCbCodec::GSMCbMsgSubType::CMAS_SEVERE);
    EXPECT_EQ(cbMsg->CMASClass(GsmCbCodec::SEVERE_LIKELY), GsmCbCodec::GSMCbMsgSubType::CMAS_SEVERE);
    EXPECT_EQ(cbMsg->CMASClass(GsmCbCodec::SEVERE_LIKELY_SPANISH), GsmCbCodec::GSMCbMsgSubType::CMAS_SEVERE);
    EXPECT_EQ(cbMsg->CMASClass(GsmCbCodec::ALERT_OBSERVED_DEFUALT), GsmCbCodec::GSMCbMsgSubType::CMAS_SEVERE);
    EXPECT_EQ(cbMsg->CMASClass(GsmCbCodec::ALERT_OBSERVED_SPANISH), GsmCbCodec::GSMCbMsgSubType::CMAS_SEVERE);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_GsmCbCodec_0006
 * @tc.name     Test GsmCbCodec
 * @tc.desc     Function test
 */
HWTEST_F(BranchCbTest, GsmCbCodec_0006, Function | MediumTest | Level1)
{
    auto smsCbMessage = std::make_shared<GsmCbCodec>();
    EXPECT_EQ(smsCbMessage->CMASClass(GsmCbCodec::ALERT_LIKELY), GsmCbCodec::GSMCbMsgSubType::CMAS_SEVERE);
    EXPECT_EQ(smsCbMessage->CMASClass(GsmCbCodec::ALERT_LIKELY_SPANISH), GsmCbCodec::GSMCbMsgSubType::CMAS_SEVERE);
    EXPECT_EQ(smsCbMessage->CMASClass(GsmCbCodec::EXPECTED_OBSERVED), GsmCbCodec::GSMCbMsgSubType::CMAS_SEVERE);
    EXPECT_EQ(smsCbMessage->CMASClass(GsmCbCodec::EXPECTED_OBSERVED_SPANISH), GsmCbCodec::GSMCbMsgSubType::CMAS_SEVERE);
    EXPECT_EQ(smsCbMessage->CMASClass(GsmCbCodec::EXPECTED_LIKELY_SPANISH), GsmCbCodec::GSMCbMsgSubType::CMAS_SEVERE);
    EXPECT_EQ(smsCbMessage->CMASClass(GsmCbCodec::EXPECTED_LIKELY_SPANISH), GsmCbCodec::GSMCbMsgSubType::CMAS_SEVERE);
    EXPECT_EQ(smsCbMessage->CMASClass(GsmCbCodec::AMBER_ALERT), GsmCbCodec::GSMCbMsgSubType::CMAS_AMBER);
    EXPECT_EQ(smsCbMessage->CMASClass(GsmCbCodec::AMBER_ALERT_SPANISH), GsmCbCodec::GSMCbMsgSubType::CMAS_AMBER);
    EXPECT_EQ(smsCbMessage->CMASClass(GsmCbCodec::MONTHLY_ALERT), GsmCbCodec::GSMCbMsgSubType::CMAS_TEST);
    EXPECT_EQ(smsCbMessage->CMASClass(GsmCbCodec::MONTHLY_ALERT_SPANISH), GsmCbCodec::GSMCbMsgSubType::CMAS_TEST);
    EXPECT_EQ(smsCbMessage->CMASClass(GsmCbCodec::EXERCISE_ALERT), GsmCbCodec::GSMCbMsgSubType::CMAS_EXERCISE);
    EXPECT_EQ(smsCbMessage->CMASClass(GsmCbCodec::EXERCISE_ALERT_SPANISH), GsmCbCodec::GSMCbMsgSubType::CMAS_EXERCISE);
    EXPECT_EQ(smsCbMessage->CMASClass(GsmCbCodec::OPERATOR_ALERT), GsmCbCodec::GSMCbMsgSubType::CMAS_OPERATOR_DEFINED);
    EXPECT_EQ(smsCbMessage->CMASClass(GsmCbCodec::OPERATOR_ALERT_SPANISH),
        GsmCbCodec::GSMCbMsgSubType::CMAS_OPERATOR_DEFINED);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_GsmCbGsmCodec_0001
 * @tc.name     Test GsmCbGsmCodec
 * @tc.desc     Function test
 */
HWTEST_F(BranchCbTest, GsmCbGsmCodec_0001, Function | MediumTest | Level1)
{
    auto cbMsg = std::make_shared<GsmCbCodec>();
    if (cbMsg == nullptr) {
        TELEPHONY_LOGI("cbMsg nullptr");
        EXPECT_TRUE(false);
        return;
    }
    cbMsg->CreateCbMessage(ETWS_PDU);
    std::vector<unsigned char> pdu = StringUtils::HexToByteVector(ETWS_PDU);
    cbMsg->cbPduBuffer_ = std::make_shared<GsmCbPduDecodeBuffer>(pdu.size());
    cbMsg->cbHeader_ = std::make_shared<GsmCbCodec::GsmCbMessageHeader>();
    for (size_t index = 0; index < pdu.size() && index < cbMsg->cbPduBuffer_->GetSize(); index++) {
        cbMsg->cbPduBuffer_->pduBuffer_[index] = static_cast<char>(pdu[index]);
    }
    auto gsmMsg = std::make_shared<GsmCbGsmCodec>(cbMsg->cbHeader_, cbMsg->cbPduBuffer_, cbMsg);
    auto umtsMsg = std::make_shared<GsmCbUmtsCodec>(cbMsg->cbHeader_, cbMsg->cbPduBuffer_, cbMsg);
    gsmMsg->cbPduBuffer_ = cbMsg->cbPduBuffer_;
    umtsMsg->cbPduBuffer_ = cbMsg->cbPduBuffer_;
    gsmMsg->Decode2gHeader();
    gsmMsg->Decode2gHeaderEtws();
    EXPECT_TRUE(gsmMsg->Decode2gCbMsg());

    gsmMsg->Decode2gHeaderCommonCb();
    uint16_t dataLen = gsmMsg->cbPduBuffer_->GetSize() - gsmMsg->cbPduBuffer_->GetCurPosition();
    gsmMsg->Decode2gCbMsg7bit(dataLen);
    EXPECT_TRUE(gsmMsg->DecodeEtwsMsg());

    umtsMsg->Decode3gHeader();
    umtsMsg->Decode3gHeaderPartData(0);
    umtsMsg->Decode3gCbMsg();
    umtsMsg->Decode3g7Bit();
    EXPECT_FALSE(umtsMsg->Decode3gUCS2());
    cbMsg->cbHeader_ = std::make_shared<GsmCbCodec::GsmCbMessageHeader>();
    cbMsg->cbHeader_->dcs.codingScheme = DataCodingScheme::DATA_CODING_7BIT;
    gsmMsg->Decode2gCbMsg();
    EXPECT_FALSE(umtsMsg->Decode3gCbMsg());

    cbMsg->cbHeader_->dcs.codingScheme = DataCodingScheme::DATA_CODING_8BIT;
    gsmMsg->Decode2gCbMsg();
    EXPECT_FALSE(umtsMsg->Decode3gCbMsg());
    cbMsg->cbHeader_->dcs.codingScheme = DataCodingScheme::DATA_CODING_UCS2;
    gsmMsg->Decode2gCbMsg();
    EXPECT_FALSE(umtsMsg->Decode3gCbMsg());

    cbMsg->cbHeader_->dcs.codingScheme = DataCodingScheme::DATA_CODING_ASCII7BIT;
    gsmMsg->Decode2gCbMsg();
    EXPECT_FALSE(umtsMsg->Decode3gCbMsg());
    cbMsg->cbHeader_->totalPages = 1;
    umtsMsg->Decode3g7Bit();
    EXPECT_FALSE(umtsMsg->Decode3gUCS2());
}

/**
 * @tc.number   Telephony_SmsMmsGtest_GsmCbGsmCodec_0002
 * @tc.name     Test GsmCbGsmCodec
 * @tc.desc     Function test
 */
HWTEST_F(BranchCbTest, GsmCbGsmCodec_0002, Function | MediumTest | Level1)
{
    auto cbMsg = std::make_shared<GsmCbCodec>();
    if (cbMsg == nullptr) {
        TELEPHONY_LOGI("cbMsg nullptr");
        EXPECT_TRUE(false);
        return;
    }
    cbMsg->CreateCbMessage(CMAS_PDU);
    std::vector<unsigned char> pdu = StringUtils::HexToByteVector(CMAS_PDU);
    cbMsg->cbPduBuffer_ = std::make_shared<GsmCbPduDecodeBuffer>(pdu.size());
    cbMsg->cbHeader_ = std::make_shared<GsmCbCodec::GsmCbMessageHeader>();
    for (size_t index = 0; index < pdu.size() && index < cbMsg->cbPduBuffer_->GetSize(); index++) {
        cbMsg->cbPduBuffer_->pduBuffer_[index] = static_cast<char>(pdu[index]);
    }
    auto gsmMsg = std::make_shared<GsmCbGsmCodec>(cbMsg->cbHeader_, cbMsg->cbPduBuffer_, cbMsg);
    auto umtsMsg = std::make_shared<GsmCbUmtsCodec>(cbMsg->cbHeader_, cbMsg->cbPduBuffer_, cbMsg);
    gsmMsg->cbPduBuffer_ = cbMsg->cbPduBuffer_;
    umtsMsg->cbPduBuffer_ = cbMsg->cbPduBuffer_;
    gsmMsg->Decode2gHeader();
    gsmMsg->Decode2gHeaderEtws();
    EXPECT_TRUE(gsmMsg->Decode2gCbMsg());

    gsmMsg->Decode2gHeaderCommonCb();
    uint16_t dataLen = gsmMsg->cbPduBuffer_->GetSize() - gsmMsg->cbPduBuffer_->GetCurPosition();
    gsmMsg->Decode2gCbMsg7bit(dataLen);
    EXPECT_TRUE(gsmMsg->DecodeEtwsMsg());

    umtsMsg->Decode3gHeader();
    umtsMsg->Decode3gHeaderPartData(0);
    umtsMsg->Decode3gCbMsg();
    umtsMsg->Decode3g7Bit();
    EXPECT_FALSE(umtsMsg->Decode3gUCS2());
    cbMsg->cbHeader_ = std::make_shared<GsmCbCodec::GsmCbMessageHeader>();
    cbMsg->cbHeader_->dcs.codingScheme = DataCodingScheme::DATA_CODING_7BIT;
    gsmMsg->Decode2gCbMsg();
    EXPECT_FALSE(umtsMsg->Decode3gCbMsg());

    cbMsg->cbHeader_->dcs.codingScheme = DataCodingScheme::DATA_CODING_8BIT;
    gsmMsg->Decode2gCbMsg();
    EXPECT_FALSE(umtsMsg->Decode3gCbMsg());
    cbMsg->cbHeader_->dcs.codingScheme = DataCodingScheme::DATA_CODING_UCS2;
    gsmMsg->Decode2gCbMsg();
    EXPECT_FALSE(umtsMsg->Decode3gCbMsg());

    cbMsg->cbHeader_->dcs.codingScheme = DataCodingScheme::DATA_CODING_ASCII7BIT;
    gsmMsg->Decode2gCbMsg();
    EXPECT_FALSE(umtsMsg->Decode3gCbMsg());
    cbMsg->cbHeader_->totalPages = 1;
    umtsMsg->Decode3g7Bit();
    EXPECT_FALSE(umtsMsg->Decode3gUCS2());
}

/**
 * @tc.number   Telephony_SmsMmsGtest_GsmCbGsmCodec_0003
 * @tc.name     Test GsmCbGsmCodec
 * @tc.desc     Function test
 */
HWTEST_F(BranchCbTest, GsmCbGsmCodec_0003, Function | MediumTest | Level1)
{
    auto cbMsg = std::make_shared<GsmCbCodec>();
    if (cbMsg == nullptr) {
        TELEPHONY_LOGI("cbMsg nullptr");
        EXPECT_TRUE(false);
        return;
    }
    cbMsg->CreateCbMessage(CBS_PDU);
    std::vector<unsigned char> pdu = StringUtils::HexToByteVector(CBS_PDU);
    cbMsg->cbPduBuffer_ = std::make_shared<GsmCbPduDecodeBuffer>(pdu.size());
    cbMsg->cbHeader_ = std::make_shared<GsmCbCodec::GsmCbMessageHeader>();
    for (size_t index = 0; index < pdu.size() && index < cbMsg->cbPduBuffer_->GetSize(); index++) {
        cbMsg->cbPduBuffer_->pduBuffer_[index] = static_cast<char>(pdu[index]);
    }
    auto gsmMsg = std::make_shared<GsmCbGsmCodec>(cbMsg->cbHeader_, cbMsg->cbPduBuffer_, cbMsg);
    auto umtsMsg = std::make_shared<GsmCbUmtsCodec>(cbMsg->cbHeader_, cbMsg->cbPduBuffer_, cbMsg);
    gsmMsg->cbPduBuffer_ = cbMsg->cbPduBuffer_;
    umtsMsg->cbPduBuffer_ = cbMsg->cbPduBuffer_;
    gsmMsg->Decode2gHeader();
    gsmMsg->Decode2gHeaderEtws();
    EXPECT_TRUE(gsmMsg->Decode2gCbMsg());

    gsmMsg->Decode2gHeaderCommonCb();
    uint16_t dataLen = gsmMsg->cbPduBuffer_->GetSize() - gsmMsg->cbPduBuffer_->GetCurPosition();
    gsmMsg->Decode2gCbMsg7bit(dataLen);
    EXPECT_TRUE(gsmMsg->DecodeEtwsMsg());

    umtsMsg->Decode3gHeader();
    umtsMsg->Decode3gHeaderPartData(0);
    umtsMsg->Decode3gCbMsg();
    umtsMsg->Decode3g7Bit();
    EXPECT_FALSE(umtsMsg->Decode3gUCS2());
    cbMsg->cbHeader_ = std::make_shared<GsmCbCodec::GsmCbMessageHeader>();
    cbMsg->cbHeader_->dcs.codingScheme = DataCodingScheme::DATA_CODING_7BIT;
    gsmMsg->Decode2gCbMsg();
    EXPECT_FALSE(umtsMsg->Decode3gCbMsg());

    cbMsg->cbHeader_->dcs.codingScheme = DataCodingScheme::DATA_CODING_8BIT;
    gsmMsg->Decode2gCbMsg();
    EXPECT_FALSE(umtsMsg->Decode3gCbMsg());
    cbMsg->cbHeader_->dcs.codingScheme = DataCodingScheme::DATA_CODING_UCS2;
    gsmMsg->Decode2gCbMsg();
    EXPECT_FALSE(umtsMsg->Decode3gCbMsg());

    cbMsg->cbHeader_->dcs.codingScheme = DataCodingScheme::DATA_CODING_ASCII7BIT;
    gsmMsg->Decode2gCbMsg();
    EXPECT_FALSE(umtsMsg->Decode3gCbMsg());
    cbMsg->cbHeader_->totalPages = 1;
    umtsMsg->Decode3g7Bit();
    EXPECT_FALSE(umtsMsg->Decode3gUCS2());
}

/**
 * @tc.number   Telephony_SmsMmsGtest_GsmCbGsmCodec_0004
 * @tc.name     Test GsmCbGsmCodec
 * @tc.desc     Function test
 */
HWTEST_F(BranchCbTest, GsmCbGsmCodec_0004, Function | MediumTest | Level1)
{
    auto cbMsg = std::make_shared<GsmCbCodec>();
    if (cbMsg == nullptr) {
        TELEPHONY_LOGI("cbMsg nullptr");
        EXPECT_TRUE(false);
        return;
    }
    cbMsg->CreateCbMessage(CMAS_JP_PDU);
    std::vector<unsigned char> pdu = StringUtils::HexToByteVector(CMAS_JP_PDU);
    cbMsg->cbPduBuffer_ = std::make_shared<GsmCbPduDecodeBuffer>(pdu.size());
    cbMsg->cbHeader_ = std::make_shared<GsmCbCodec::GsmCbMessageHeader>();
    for (size_t index = 0; index < pdu.size() && index < cbMsg->cbPduBuffer_->GetSize(); index++) {
        cbMsg->cbPduBuffer_->pduBuffer_[index] = static_cast<char>(pdu[index]);
    }
    auto gsmMsg = std::make_shared<GsmCbGsmCodec>(cbMsg->cbHeader_, cbMsg->cbPduBuffer_, cbMsg);
    auto umtsMsg = std::make_shared<GsmCbUmtsCodec>(cbMsg->cbHeader_, cbMsg->cbPduBuffer_, cbMsg);
    gsmMsg->cbPduBuffer_ = cbMsg->cbPduBuffer_;
    umtsMsg->cbPduBuffer_ = cbMsg->cbPduBuffer_;
    gsmMsg->Decode2gHeader();
    gsmMsg->Decode2gHeaderEtws();
    EXPECT_TRUE(gsmMsg->Decode2gCbMsg());

    gsmMsg->Decode2gHeaderCommonCb();
    uint16_t dataLen = gsmMsg->cbPduBuffer_->GetSize() - gsmMsg->cbPduBuffer_->GetCurPosition();
    gsmMsg->Decode2gCbMsg7bit(dataLen);
    EXPECT_FALSE(gsmMsg->DecodeEtwsMsg());

    umtsMsg->Decode3gHeader();
    umtsMsg->Decode3gHeaderPartData(0);
    umtsMsg->Decode3gCbMsg();
    umtsMsg->Decode3g7Bit();
    EXPECT_TRUE(umtsMsg->Decode3gUCS2());
    cbMsg->cbHeader_ = std::make_shared<GsmCbCodec::GsmCbMessageHeader>();
    cbMsg->cbHeader_->dcs.codingScheme = DataCodingScheme::DATA_CODING_7BIT;
    gsmMsg->Decode2gCbMsg();
    EXPECT_TRUE(umtsMsg->Decode3gCbMsg());

    cbMsg->cbHeader_->dcs.codingScheme = DataCodingScheme::DATA_CODING_8BIT;
    gsmMsg->Decode2gCbMsg();
    EXPECT_TRUE(umtsMsg->Decode3gCbMsg());
    cbMsg->cbHeader_->dcs.codingScheme = DataCodingScheme::DATA_CODING_UCS2;
    gsmMsg->Decode2gCbMsg();
    EXPECT_TRUE(umtsMsg->Decode3gCbMsg());

    cbMsg->cbHeader_->dcs.codingScheme = DataCodingScheme::DATA_CODING_ASCII7BIT;
    gsmMsg->Decode2gCbMsg();
    EXPECT_TRUE(umtsMsg->Decode3gCbMsg());
    cbMsg->cbHeader_->totalPages = 1;
    umtsMsg->Decode3g7Bit();
    EXPECT_TRUE(umtsMsg->Decode3gUCS2());
}

/**
 * @tc.number   Telephony_SmsMmsGtest_GsmCbGsmCodec_0005
 * @tc.name     Test GsmCbGsmCodec
 * @tc.desc     Function test
 */
HWTEST_F(BranchCbTest, GsmCbGsmCodec_0005, Function | MediumTest | Level1)
{
    auto cbMsg = std::make_shared<GsmCbCodec>();
    unsigned char data = 1;
    std::vector<unsigned char> pdu;
    pdu.push_back(data);
    cbMsg->PduAnalysis(pdu);
    auto gsmMsg = std::make_shared<GsmCbGsmCodec>(cbMsg->cbHeader_, cbMsg->cbPduBuffer_, cbMsg);
    auto umtsMsg = std::make_shared<GsmCbUmtsCodec>(cbMsg->cbHeader_, cbMsg->cbPduBuffer_, cbMsg);
    if (cbMsg == nullptr || gsmMsg == nullptr || umtsMsg == nullptr) {
        EXPECT_TRUE(false);
        return;
    }

    gsmMsg->Decode2gCbMsg();
    EXPECT_FALSE(umtsMsg->Decode3gCbMsg());
    cbMsg->cbHeader_ = std::make_shared<GsmCbCodec::GsmCbMessageHeader>();
    cbMsg->cbHeader_->dcs.codingScheme = DataCodingScheme::DATA_CODING_7BIT;
    gsmMsg->Decode2gCbMsg();
    EXPECT_FALSE(umtsMsg->Decode3gCbMsg());
    cbMsg->cbHeader_->dcs.codingScheme = DataCodingScheme::DATA_CODING_8BIT;
    gsmMsg->Decode2gCbMsg();
    EXPECT_FALSE(umtsMsg->Decode3gCbMsg());
    cbMsg->cbHeader_->dcs.codingScheme = DataCodingScheme::DATA_CODING_UCS2;
    gsmMsg->Decode2gCbMsg();
    EXPECT_FALSE(umtsMsg->Decode3gCbMsg());
    cbMsg->cbHeader_->dcs.codingScheme = DataCodingScheme::DATA_CODING_ASCII7BIT;
    gsmMsg->Decode2gCbMsg();
    umtsMsg->Decode3gCbMsg();
    cbMsg->cbHeader_->totalPages = 1;
    umtsMsg->Decode3g7Bit();
    EXPECT_FALSE(umtsMsg->Decode3gUCS2());
}

/**
 * @tc.number   Telephony_SmsMmsGtest_GsmSmsCbHandler_0001
 * @tc.name     Test GsmSmsCbHandler
 * @tc.desc     Function test
 */
HWTEST_F(BranchCbTest, GsmSmsCbHandler_0001, Function | MediumTest | Level1)
{
    auto gsmSmsCbHandler = std::make_shared<GsmSmsCbHandler>(INVALID_SLOTID);
    auto cbMessage = std::make_shared<GsmCbCodec>();
    auto message = std::make_shared<CBConfigReportInfo>();
    AppExecFwk::InnerEvent::Pointer event = AppExecFwk::InnerEvent::Get(RadioEvent::RADIO_CELL_BROADCAST, 1);
    gsmSmsCbHandler->ProcessEvent(event);
    event = AppExecFwk::InnerEvent::Get(RadioEvent::RADIO_GET_STATUS, 1);
    gsmSmsCbHandler->ProcessEvent(event);
    gsmSmsCbHandler->HandleCbMessage(message);
    EXPECT_EQ(gsmSmsCbHandler->CheckCbMessage(cbMessage), 0);
    EXPECT_TRUE(gsmSmsCbHandler->FindCbMessage(cbMessage) == nullptr);
    EXPECT_FALSE(gsmSmsCbHandler->AddCbMessageToList(cbMessage));
    EXPECT_FALSE(gsmSmsCbHandler->SendCbMessageBroadcast(cbMessage));
    cbMessage->cbHeader_ = std::make_shared<GsmCbCodec::GsmCbMessageHeader>();
    EXPECT_EQ(gsmSmsCbHandler->CheckCbMessage(cbMessage), 1);
    cbMessage->cbHeader_->totalPages = 1;
    EXPECT_EQ(gsmSmsCbHandler->CheckCbMessage(cbMessage), 0x01);
    EXPECT_FALSE(gsmSmsCbHandler->AddCbMessageToList(cbMessage));
    CbInfo cbInfo;
    gsmSmsCbHandler->cbMsgList_.push_back(cbInfo);
    EXPECT_EQ(gsmSmsCbHandler->CheckCbMessage(cbMessage), 0x01);
    cbMessage = nullptr;
    message = nullptr;
    event = nullptr;
    gsmSmsCbHandler->ProcessEvent(event);
    SmsCbData::CbData sendData;
    gsmSmsCbHandler->GetCbData(cbMessage, sendData);
    EventFwk::Want want;
    gsmSmsCbHandler->PackageWantData(sendData, want);
    gsmSmsCbHandler->HandleCbMessage(message);
    EXPECT_FALSE(gsmSmsCbHandler->CheckCbActive(cbMessage));
    EXPECT_FALSE(gsmSmsCbHandler->AddCbMessageToList(cbMessage));
    EXPECT_FALSE(gsmSmsCbHandler->SendCbMessageBroadcast(cbMessage));
    EXPECT_TRUE(gsmSmsCbHandler->FindCbMessage(cbMessage) == nullptr);
    EXPECT_EQ(gsmSmsCbHandler->CheckCbMessage(cbMessage), 0);
}
} // namespace Telephony
} // namespace OHOS
