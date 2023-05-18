/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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
#include "cdma_sms_pdu_codec.h"
#include "cdma_sms_receive_handler.h"
#include "cdma_sms_sender.h"
#include "delivery_short_message_callback_stub.h"
#include "gsm_sms_cb_handler.h"
#include "gsm_sms_message.h"
#include "gsm_sms_receive_handler.h"
#include "gtest/gtest.h"
#include "mms_body.h"
#include "mms_body_part.h"
#include "mms_content_type.h"
#include "mms_encode_buffer.h"
#include "mms_header.h"
#include "mms_msg.h"
#include "msg_text_convert.h"
#include "radio_event.h"
#include "send_short_message_callback_stub.h"
#include "short_message.h"
#include "sms_cb_message.h"
#include "sms_common_utils.h"
#include "sms_misc_manager.h"
#include "sms_receive_handler.h"
#include "sms_receive_reliability_handler.h"
#include "sms_send_manager.h"
#include "sms_sender.h"
#include "sms_service.h"
#include "sms_wap_push_buffer.h"
#include "sms_wap_push_content_type.h"
#include "telephony_errors.h"
#include "telephony_hisysevent.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
using namespace testing::ext;

namespace {
const std::string TEXT_SMS_CONTENT = "hello world";
const std::string BLOCK_NUMBER = "123";
const int8_t TEXT_PORT_NUM = -1;
const uint32_t TRANSACTION_ID_LENGTH = 3;
const uint32_t PDU_BUFFE_LENGTH = 126;
const uint32_t PDU_BUFFE_LENGTH_ONE = 128;
const uint32_t PDU_BUFFE_LENGTH_TWO = 127;
const uint32_t PDU_BUFFE_LENGTH_THREE = 31;
const uint32_t PDU_BUFFE_LENGTH_FOUR = 129;
const uint32_t PDU_BUFFE_LENGTH_FIVE = 139;
const uint32_t PDU_BUFFE_LENGTH_SIX = 134;
const uint32_t PDU_BUFFE_LENGTH_SEVEN = 152;
const uint32_t PDU_BUFFE_LENGTH_EIGHT = 153;
const uint32_t PDU_BUFFE_LENGTH_NINE = 138;
const uint32_t PDU_BUFFE_LENGTH_TEN = 133;
const uint32_t PDU_BUFFE_LENGTH_ELEVEN = 151;
const uint32_t PDU_BUFFE_LENGTH_TWELVE = 154;
const uint32_t PDU_BUFFE_LENGTH_THIRTEEN = 131;
const uint32_t PDU_BUFFE_LENGTH_FOURTEEN = 137;
const int16_t WAP_PUSH_PORT = 2948;
const uint8_t MIN_TOKEN = 30;
const uint8_t MAX_TOKEN = 127;
const int BUF_SIZE = 2401;
const int TEXT_LENGTH = 2;
const int FILL_BITS = 2;
const int DIGIT_LEN = 3;
const int START_BIT = 4;
const int PID_LENGTH = 32;
const unsigned char SRC_TEXT = 2;
const unsigned char TP_FAIL_CAUSE = 128;
const uint8_t FIELD_ID = 2;
const int32_t INVALID_SLOTID = 2;
const int32_t VALUE_LENGTH = 2;
const uint16_t PWS_FIRST_ID = 0x1100;
const size_t DATA_LENGTH = 162;
const int32_t HEADER_LENGTH = 7;
static constexpr uint32_t MAX_MMS_MSG_PART_LEN = 300 * 1024;
const uint32_t CODE_BUFFER_MAX_SIZE = 300 * 1024;
unsigned int SHIFT_BIT = 10;
unsigned int SMS_REF_ID = 10;
} // namespace

class BranchTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};
void BranchTest::SetUpTestCase() {}

void BranchTest::TearDownTestCase() {}

void BranchTest::SetUp() {}

void BranchTest::TearDown() {}

/**
 * @tc.number   Telephony_SmsMmsGtest_MsgTextConvert_0001
 * @tc.name     Test MsgTextConvert
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, MsgTextConvert_0001, Function | MediumTest | Level1)
{
    SmsCodingScheme smsCodingScheme;
    MsgLangInfo msgLangInfo;
    unsigned char encodeData[BUF_SIZE];
    MSG_LANGUAGE_ID_T langId = 0;
    bool bAbnormal = false;
    unsigned short inText = 1;
    const unsigned char *pMsgText = (const unsigned char *)TEXT_SMS_CONTENT.c_str();
    unsigned char *pDestText = encodeData;
    MSG_LANGUAGE_ID_T *pLangId = &langId;
    bool *pIncludeAbnormalChar = &bAbnormal;
    std::tuple<unsigned char *, int, unsigned char *, int, MSG_LANGUAGE_ID_T *, bool *> paras(
        pDestText, BUF_SIZE, const_cast<unsigned char *>(pMsgText), 0, pLangId, pIncludeAbnormalChar);
    EXPECT_GE(MsgTextConvert::Instance().ConvertUTF8ToGSM7bit(paras), 0);
    EXPECT_GE(MsgTextConvert::Instance().ConvertCdmaUTF8ToAuto(pDestText, 1, pMsgText, 1, &smsCodingScheme), -1);
    EXPECT_GE(MsgTextConvert::Instance().ConvertGsmUTF8ToAuto(pDestText, 1, pMsgText, 1, &smsCodingScheme), -1);
    EXPECT_EQ(MsgTextConvert::Instance().ConvertUTF8ToUCS2(pDestText, 1, pMsgText, -1), -1);
    EXPECT_EQ(MsgTextConvert::Instance().ConvertUTF8ToUCS2(pDestText, 0, pMsgText, -1), -1);
    EXPECT_EQ(MsgTextConvert::Instance().ConvertCdmaUTF8ToAuto(pDestText, 1, pMsgText, 0, &smsCodingScheme), 0);
    EXPECT_EQ(MsgTextConvert::Instance().ConvertGsmUTF8ToAuto(pDestText, 1, pMsgText, 0, &smsCodingScheme), 0);
    EXPECT_EQ(MsgTextConvert::Instance().ConvertGSM7bitToUTF8(pDestText, 0, pMsgText, 0, &msgLangInfo), 0);
    EXPECT_GE(MsgTextConvert::Instance().ConvertSHIFTJISToUTF8(pDestText, 1, pMsgText, -1), 0);
    EXPECT_GE(MsgTextConvert::Instance().ConvertUCS2ToUTF8(pDestText, 1, pMsgText, -1), 0);
    EXPECT_GE(MsgTextConvert::Instance().ConvertEUCKRToUTF8(pDestText, 1, pMsgText, -1), 0);
    EXPECT_FALSE(MsgTextConvert::Instance().ConvertUCS2ToUTF8(pDestText, 0, pMsgText, 0));
    EXPECT_FALSE(MsgTextConvert::Instance().ConvertEUCKRToUTF8(pDestText, 0, pMsgText, 1));
    EXPECT_FALSE(MsgTextConvert::Instance().ConvertSHIFTJISToUTF8(pDestText, 0, pMsgText, 1));
    EXPECT_EQ(
        MsgTextConvert::Instance().ConvertUCS2ToGSM7bit(pDestText, 0, pMsgText, 0, pLangId, pIncludeAbnormalChar), -1);
    EXPECT_GT(MsgTextConvert::Instance().ConvertUCS2ToGSM7bit(
                  pDestText, 1, pMsgText, TEXT_LENGTH, pLangId, pIncludeAbnormalChar),
        0);
    EXPECT_EQ(MsgTextConvert::Instance().ConvertUCS2ToGSM7bitAuto(pDestText, 0, pMsgText, 0, pIncludeAbnormalChar), -1);
    EXPECT_GE(MsgTextConvert::Instance().ConvertUCS2ToGSM7bitAuto(pDestText, 1, pMsgText, 1, pIncludeAbnormalChar), 0);
    EXPECT_EQ(MsgTextConvert::Instance().ConvertUCS2ToASCII(pDestText, 0, pMsgText, 0, pIncludeAbnormalChar), -1);
    EXPECT_GE(MsgTextConvert::Instance().ConvertUCS2ToASCII(pDestText, 1, pMsgText, 1, pIncludeAbnormalChar), 0);
    EXPECT_EQ(MsgTextConvert::Instance().GetLangType(pMsgText, 0), MSG_DEFAULT_CHAR);
    EXPECT_GE(MsgTextConvert::Instance().GetLangType(pMsgText, 0), MSG_DEFAULT_CHAR);
    EXPECT_EQ(MsgTextConvert::Instance().FindUCS2toGSM7Ext(pDestText, 0, inText, bAbnormal), 0);
    EXPECT_GE(MsgTextConvert::Instance().FindUCS2toGSM7Ext(pDestText, 1, inText, bAbnormal), 0);
    EXPECT_EQ(MsgTextConvert::Instance().FindUCS2toTurkish(pDestText, 0, inText, bAbnormal), 0);
    EXPECT_GE(MsgTextConvert::Instance().FindUCS2toTurkish(pDestText, 1, inText, bAbnormal), 0);
    EXPECT_EQ(MsgTextConvert::Instance().FindUCS2toSpanish(pDestText, 0, inText, bAbnormal), 0);
    EXPECT_GE(MsgTextConvert::Instance().FindUCS2toSpanish(pDestText, 1, inText, bAbnormal), 0);
    EXPECT_EQ(MsgTextConvert::Instance().FindUCS2toPortu(pDestText, 0, inText, bAbnormal), 0);
    EXPECT_GE(MsgTextConvert::Instance().FindUCS2toPortu(pDestText, 1, inText, bAbnormal), 0);
    EXPECT_GE(MsgTextConvert::Instance().FindUCS2ReplaceChar(inText), MSG_DEFAULT_CHAR);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_MsgTextConvert_0002
 * @tc.name     Test MsgTextConvert
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, MsgTextConvert_0002, Function | MediumTest | Level1)
{
    MsgLangInfo pLangInfo;
    pLangInfo.bLockingShift = true;
    pLangInfo.bSingleShift = true;
    unsigned char encodeData[BUF_SIZE];
    unsigned short result = 1;
    const unsigned char *pText = nullptr;
    const unsigned char *pMsgText = (const unsigned char *)TEXT_SMS_CONTENT.c_str();
    unsigned char *pDestText = encodeData;
    MsgTextConvert::Instance().ConvertDumpTextToHex(pText, 1);
    MsgTextConvert::Instance().ConvertDumpTextToHex(pMsgText, 1);
    EXPECT_EQ(MsgTextConvert::Instance().ConvertGSM7bitToUCS2(pDestText, 0, pMsgText, 0, &pLangInfo), -1);
    pLangInfo.lockingLang = MSG_ID_TURKISH_LANG;
    EXPECT_GT(MsgTextConvert::Instance().ConvertGSM7bitToUCS2(pDestText, 1, pMsgText, 1, &pLangInfo), 0);
    EXPECT_GT(MsgTextConvert::Instance().EscapeToUCS2(SRC_TEXT, pLangInfo), 0);
    pLangInfo.lockingLang = MSG_ID_PORTUGUESE_LANG;
    EXPECT_GT(MsgTextConvert::Instance().ConvertGSM7bitToUCS2(pDestText, 1, pMsgText, 1, &pLangInfo), 0);
    EXPECT_GT(MsgTextConvert::Instance().EscapeToUCS2(SRC_TEXT, pLangInfo), 0);
    pLangInfo.bLockingShift = false;
    EXPECT_GT(MsgTextConvert::Instance().ConvertGSM7bitToUCS2(pDestText, 1, pMsgText, 1, &pLangInfo), 0);
    EXPECT_EQ(MsgTextConvert::Instance().EscapeTurkishLockingToUCS2(pMsgText, 0, pLangInfo, result), 0);
    EXPECT_EQ(MsgTextConvert::Instance().EscapeTurkishLockingToUCS2(pMsgText, 1, pLangInfo, result), 0);
    EXPECT_EQ(MsgTextConvert::Instance().EscapePortuLockingToUCS2(pMsgText, 0, pLangInfo, result), 0);
    EXPECT_EQ(MsgTextConvert::Instance().EscapePortuLockingToUCS2(pMsgText, 1, pLangInfo, result), 0);
    EXPECT_EQ(MsgTextConvert::Instance().EscapeGSM7BitToUCS2(pMsgText, 0, pLangInfo, result), 0);
    EXPECT_EQ(MsgTextConvert::Instance().EscapeGSM7BitToUCS2(pMsgText, 1, pLangInfo, result), 0);
    pLangInfo.singleLang = MSG_ID_SPANISH_LANG;
    EXPECT_GT(MsgTextConvert::Instance().EscapeToUCS2(SRC_TEXT, pLangInfo), 0);
    pLangInfo.singleLang = MSG_ID_RESERVED_LANG;
    EXPECT_GT(MsgTextConvert::Instance().EscapeToUCS2(SRC_TEXT, pLangInfo), 0);
    pLangInfo.bSingleShift = false;
    EXPECT_GT(MsgTextConvert::Instance().EscapeToUCS2(SRC_TEXT, pLangInfo), 0);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_MmsHeader_0001
 * @tc.name     Test MmsHeader
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, MmsHeader_0001, Function | MediumTest | Level1)
{
    auto mmsHeader = std::make_shared<MmsHeader>();
    MmsEncodeString encodeString;
    MmsDecodeBuffer decodeBuffer;
    MmsEncodeBuffer encodeBuffer;
    std::string encodeAddress = "123";
    uint8_t value = 0;
    MmsAddress address(encodeAddress);
    std::vector<MmsAddress> addressValue;
    addressValue.push_back(address);
    mmsHeader->octetValueMap_.insert(pair<uint8_t, uint8_t>(1, 1));
    mmsHeader->longValueMap_.insert(pair<uint8_t, int64_t>(1, 1));
    mmsHeader->textValueMap_.insert(pair<uint8_t, std::string>(1, "1"));
    mmsHeader->encodeStringsMap_.insert(pair<uint8_t, MmsEncodeString>(1, encodeString));
    mmsHeader->addressMap_.insert(pair<uint8_t, std::vector<MmsAddress>>(1, addressValue));
    mmsHeader->DumpMmsHeader();
    EXPECT_TRUE(mmsHeader->DecodeMmsHeader(decodeBuffer));
    EXPECT_FALSE(mmsHeader->EncodeMmsHeader(encodeBuffer));
    EXPECT_TRUE(mmsHeader->GetOctetValue(1, value));
    EXPECT_FALSE(mmsHeader->SetOctetValue(1, 0));
    mmsHeader->mmsHaderCateg_.mmsFieldDesMap_.insert(
        pair<uint8_t, MmsFieldValueType>(MMS_MESSAGE_TYPE, MmsFieldValueType::MMS_FIELD_OCTET_TYPE));
    mmsHeader->mmsHaderCateg_.mmsFieldDesMap_.insert(
        pair<uint8_t, MmsFieldValueType>(MMS_PRIORITY, MmsFieldValueType::MMS_FIELD_OCTET_TYPE));
    mmsHeader->mmsHaderCateg_.mmsFieldDesMap_.insert(
        pair<uint8_t, MmsFieldValueType>(MMS_RESPONSE_STATUS, MmsFieldValueType::MMS_FIELD_OCTET_TYPE));
    mmsHeader->mmsHaderCateg_.mmsFieldDesMap_.insert(
        pair<uint8_t, MmsFieldValueType>(MMS_SENDER_VISIBILITY, MmsFieldValueType::MMS_FIELD_OCTET_TYPE));
    mmsHeader->mmsHaderCateg_.mmsFieldDesMap_.insert(
        pair<uint8_t, MmsFieldValueType>(MMS_STATUS, MmsFieldValueType::MMS_FIELD_OCTET_TYPE));
    mmsHeader->mmsHaderCateg_.mmsFieldDesMap_.insert(
        pair<uint8_t, MmsFieldValueType>(MMS_RETRIEVE_STATUS, MmsFieldValueType::MMS_FIELD_OCTET_TYPE));
    mmsHeader->mmsHaderCateg_.mmsFieldDesMap_.insert(
        pair<uint8_t, MmsFieldValueType>(MMS_READ_STATUS, MmsFieldValueType::MMS_FIELD_OCTET_TYPE));
    mmsHeader->mmsHaderCateg_.mmsFieldDesMap_.insert(
        pair<uint8_t, MmsFieldValueType>(MMS_REPLY_CHARGING, MmsFieldValueType::MMS_FIELD_OCTET_TYPE));
    EXPECT_FALSE(mmsHeader->SetOctetValue(MMS_MESSAGE_TYPE, MMS_MESSAGE_TYPE));
    EXPECT_FALSE(mmsHeader->SetOctetValue(MMS_PRIORITY, MMS_PRIORITY));
    EXPECT_FALSE(mmsHeader->SetOctetValue(MMS_RESPONSE_STATUS, MMS_RESPONSE_STATUS));
    EXPECT_FALSE(mmsHeader->SetOctetValue(MMS_SENDER_VISIBILITY, MMS_SENDER_VISIBILITY));
    EXPECT_FALSE(mmsHeader->SetOctetValue(MMS_STATUS, MMS_STATUS));
    EXPECT_FALSE(mmsHeader->SetOctetValue(MMS_RETRIEVE_STATUS, MMS_RETRIEVE_STATUS));
    EXPECT_FALSE(mmsHeader->SetOctetValue(MMS_READ_STATUS, MMS_READ_STATUS));
    EXPECT_FALSE(mmsHeader->SetOctetValue(MMS_REPLY_CHARGING, MMS_REPLY_CHARGING));
    EXPECT_FALSE(mmsHeader->SetOctetValue(MMS_MM_STATE, MMS_MM_STATE));
    EXPECT_TRUE(mmsHeader->GetHeaderAllAddressValue(1, addressValue));
}

/**
 * @tc.number   Telephony_SmsMmsGtest_MmsHeader_0002
 * @tc.name     Test MmsHeader
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, MmsHeader_0002, Function | MediumTest | Level1)
{
    auto mmsHeader = std::make_shared<MmsHeader>();
    int64_t value = 0;
    std::string valueStr = "";
    std::string encodeAddress = "123";
    MmsAddress address(encodeAddress);
    std::vector<MmsAddress> addressValue;
    addressValue.push_back(address);
    MmsEncodeString encodeString;
    mmsHeader->longValueMap_.insert(pair<uint8_t, int64_t>(1, 1));
    mmsHeader->textValueMap_.insert(pair<uint8_t, std::string>(1, "1"));
    mmsHeader->encodeStringsMap_.insert(pair<uint8_t, MmsEncodeString>(1, encodeString));
    mmsHeader->mmsHaderCateg_.mmsFieldDesMap_.insert(
        pair<uint8_t, MmsFieldValueType>(MMS_STORE_STATUS, MmsFieldValueType::MMS_FIELD_OCTET_TYPE));
    mmsHeader->mmsHaderCateg_.mmsFieldDesMap_.insert(
        pair<uint8_t, MmsFieldValueType>(MMS_CONTENT_CLASS, MmsFieldValueType::MMS_FIELD_OCTET_TYPE));
    mmsHeader->mmsHaderCateg_.mmsFieldDesMap_.insert(
        pair<uint8_t, MmsFieldValueType>(MMS_MESSAGE_CLASS, MmsFieldValueType::MMS_FIELD_OCTET_TYPE));
    mmsHeader->mmsHaderCateg_.mmsFieldDesMap_.insert(
        pair<uint8_t, MmsFieldValueType>(MMS_CANCEL_STATUS, MmsFieldValueType::MMS_FIELD_OCTET_TYPE));
    mmsHeader->mmsHaderCateg_.mmsFieldDesMap_.insert(
        pair<uint8_t, MmsFieldValueType>(MMS_ADAPTATION_ALLOWED, MmsFieldValueType::MMS_FIELD_OCTET_TYPE));
    EXPECT_TRUE(mmsHeader->SetOctetValue(MMS_ADAPTATION_ALLOWED, MMS_BCC));
    EXPECT_TRUE(mmsHeader->GetLongValue(1, value));
    EXPECT_TRUE(mmsHeader->GetTextValue(1, valueStr));
    EXPECT_TRUE(mmsHeader->GetEncodeStringValue(1, encodeString));
    EXPECT_FALSE(mmsHeader->SetTextValue(MMS_CANCEL_STATUS, ""));
    EXPECT_FALSE(mmsHeader->SetTextValue(MMS_CANCEL_STATUS, "123"));
    EXPECT_FALSE(mmsHeader->SetOctetValue(MMS_MM_FLAGS, MMS_MM_FLAGS));
    EXPECT_FALSE(mmsHeader->SetOctetValue(MMS_CONTENT_CLASS, MMS_CONTENT_CLASS));
    EXPECT_FALSE(mmsHeader->SetOctetValue(MMS_MESSAGE_CLASS, MMS_MESSAGE_CLASS));
    EXPECT_FALSE(mmsHeader->SetOctetValue(MMS_CANCEL_STATUS, MMS_CANCEL_STATUS));
    EXPECT_FALSE(mmsHeader->SetLongValue(MMS_MESSAGE_CLASS, 0));
    EXPECT_FALSE(mmsHeader->SetEncodeStringValue(1, 1, valueStr));
    EXPECT_FALSE(mmsHeader->SetEncodeStringValue(MMS_STORE_STATUS, 1, encodeAddress));
    EXPECT_FALSE(mmsHeader->AddHeaderAddressValue(1, address));
    EXPECT_TRUE(mmsHeader->GetStringValue(1, encodeAddress));
    EXPECT_FALSE(mmsHeader->GetStringValue(FIELD_ID, encodeAddress));
    EXPECT_TRUE(mmsHeader->GetSmilFileName(valueStr));
    EXPECT_FALSE(mmsHeader->IsHaveBody());
}

/**
 * @tc.number   Telephony_SmsMmsGtest_MmsHeader_0003
 * @tc.name     Test MmsHeader
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, MmsHeader_0003, Function | MediumTest | Level1)
{
    auto mmsHeader = std::make_shared<MmsHeader>();
    MmsDecodeBuffer decodeBuffer;
    MmsEncodeBuffer buff;
    std::string encodeAddress = "123";
    MmsAddress address(encodeAddress);
    std::vector<MmsAddress> addressValue;
    addressValue.push_back(address);
    int32_t data = 0;
    mmsHeader->addressMap_.insert(pair<uint8_t, std::vector<MmsAddress>>(MMS_BCC, addressValue));
    mmsHeader->addressMap_.insert(pair<uint8_t, std::vector<MmsAddress>>(MMS_FROM, addressValue));
    EXPECT_NE(mmsHeader->MakeTransactionId(TRANSACTION_ID_LENGTH), "");
    EXPECT_FALSE(mmsHeader->DecodeMmsMsgType(1, decodeBuffer, data));
    EXPECT_FALSE(mmsHeader->DecodeMmsMsgType(MMS_BCC, decodeBuffer, data));
    EXPECT_FALSE(mmsHeader->DecodeFieldAddressModelValue(1, decodeBuffer, data));
    EXPECT_TRUE(mmsHeader->DecodeFieldAddressModelValue(MMS_BCC, decodeBuffer, data));
    EXPECT_FALSE(mmsHeader->DecodeFieldOctetValue(1, decodeBuffer, data));
    EXPECT_FALSE(mmsHeader->DecodeFieldLongValue(1, decodeBuffer, data));
    EXPECT_FALSE(mmsHeader->DecodeFieldIntegerValue(1, decodeBuffer, data));
    EXPECT_FALSE(mmsHeader->DecodeFieldTextStringValue(1, decodeBuffer, data));
    EXPECT_FALSE(mmsHeader->DecodeFieldTextStringValue(MMS_BCC, decodeBuffer, data));
    EXPECT_FALSE(mmsHeader->DecodeFieldEncodedStringValue(1, decodeBuffer, data));
    EXPECT_FALSE(mmsHeader->DecodeFieldEncodedStringValue(MMS_BCC, decodeBuffer, data));
    EXPECT_FALSE(mmsHeader->DecodeFromValue(1, decodeBuffer, data));
    EXPECT_FALSE(mmsHeader->DecodeMmsMsgUnKnownField(decodeBuffer));
    EXPECT_FALSE(mmsHeader->DecodeMmsContentType(1, decodeBuffer, data));
    EXPECT_FALSE(mmsHeader->DecodeFieldDate(1, decodeBuffer, data));
    EXPECT_FALSE(mmsHeader->DecodeFieldDate(MMS_BCC, decodeBuffer, data));
    EXPECT_FALSE(mmsHeader->DecodeFieldPreviouslySentDate(1, decodeBuffer, data));
    EXPECT_FALSE(mmsHeader->DecodeFieldPreviouslySentDate(MMS_BCC, decodeBuffer, data));
    EXPECT_FALSE(mmsHeader->DecodeFieldMBox(1, decodeBuffer, data));
    EXPECT_FALSE(mmsHeader->DecodeFieldMBox(MMS_BCC, decodeBuffer, data));
    EXPECT_FALSE(mmsHeader->DecodeFieldMMFlag(1, decodeBuffer, data));
    EXPECT_FALSE(mmsHeader->DecodeFieldMMFlag(MMS_BCC, decodeBuffer, data));
    EXPECT_TRUE(mmsHeader->EncodeFieldFromValue(buff, addressValue));
    mmsHeader->textValueMap_.insert(pair<uint8_t, std::string>(MMS_MESSAGE_CLASS, "personal"));
    EXPECT_TRUE(mmsHeader->EcondeFieldMessageClassValue(buff));
    mmsHeader->textValueMap_.insert(pair<uint8_t, std::string>(MMS_MESSAGE_CLASS, "advertisement"));
    EXPECT_TRUE(mmsHeader->EcondeFieldMessageClassValue(buff));
    mmsHeader->textValueMap_.insert(pair<uint8_t, std::string>(MMS_MESSAGE_CLASS, "informational"));
    EXPECT_TRUE(mmsHeader->EcondeFieldMessageClassValue(buff));
    mmsHeader->textValueMap_.insert(pair<uint8_t, std::string>(MMS_MESSAGE_CLASS, "auto"));
    EXPECT_TRUE(mmsHeader->EcondeFieldMessageClassValue(buff));
    mmsHeader->octetValueMap_.insert(pair<uint8_t, uint8_t>(MMS_MESSAGE_CLASS, 1));
    EXPECT_TRUE(mmsHeader->EcondeFieldMessageClassValue(buff));
}

/**
 * @tc.number   Telephony_SmsMmsGtest_MmsHeader_0004
 * @tc.name     Test MmsHeader
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, MmsHeader_0004, Function | MediumTest | Level1)
{
    auto mmsHeader = std::make_shared<MmsHeader>();
    MmsEncodeBuffer buff;
    std::string encodeAddress = "123";
    MmsAddress address(encodeAddress);
    std::vector<MmsAddress> addrs;
    mmsHeader->longValueMap_.insert(pair<uint8_t, int64_t>(MMS_EXPIRY, 1));
    mmsHeader->textValueMap_.insert(pair<uint8_t, std::string>(MMS_TRANSACTION_ID, "auto"));
    EXPECT_TRUE(mmsHeader->EncodeCommontFieldValue(buff));
    EXPECT_FALSE(mmsHeader->EncodeMultipleAddressValue(buff, 1, addrs));
    addrs.push_back(address);
    EXPECT_TRUE(mmsHeader->EncodeMultipleAddressValue(buff, 1, addrs));
    EXPECT_TRUE(mmsHeader->EncodeMmsSendReq(buff));
    EXPECT_FALSE(mmsHeader->EncodeMmsSendConf(buff));
    EXPECT_FALSE(mmsHeader->EncodeMmsNotificationInd(buff));
    mmsHeader->textValueMap_.insert(pair<uint8_t, std::string>(MMS_CONTENT_LOCATION, "auto"));
    EXPECT_FALSE(mmsHeader->EncodeMmsNotificationInd(buff));
    EXPECT_FALSE(mmsHeader->EnocdeMmsNotifyRespInd(buff));
    EXPECT_FALSE(mmsHeader->EnocdeMmsRetrieveConf(buff));
    mmsHeader->longValueMap_.insert(pair<uint8_t, int64_t>(MMS_DATE, 1));
    mmsHeader->addressMap_.insert(pair<uint8_t, std::vector<MmsAddress>>(MMS_CC, addrs));
    EXPECT_TRUE(mmsHeader->EnocdeMmsRetrieveConf(buff));
    EXPECT_FALSE(mmsHeader->EnocdeMmsDeliveryInd(buff));
    mmsHeader->addressMap_.insert(pair<uint8_t, std::vector<MmsAddress>>(MMS_TO, addrs));
    EXPECT_TRUE(mmsHeader->EnocdeMmsRetrieveConf(buff));
    mmsHeader->addressMap_.insert(pair<uint8_t, std::vector<MmsAddress>>(MMS_FROM, addrs));
    EXPECT_TRUE(mmsHeader->EnocdeMmsRetrieveConf(buff));
    EXPECT_FALSE(mmsHeader->EnocdeMmsDeliveryInd(buff));
    EXPECT_FALSE(mmsHeader->EncodeMmsReadRecInd(buff));
    mmsHeader->textValueMap_.insert(pair<uint8_t, std::string>(MMS_MESSAGE_ID, "auto"));
    EXPECT_FALSE(mmsHeader->EnocdeMmsDeliveryInd(buff));
    mmsHeader->longValueMap_.clear();
    EXPECT_FALSE(mmsHeader->EnocdeMmsDeliveryInd(buff));
    mmsHeader->addressMap_.clear();
    mmsHeader->textValueMap_.clear();
    EXPECT_FALSE(mmsHeader->EncodeMmsReadRecInd(buff));
    EXPECT_FALSE(mmsHeader->EncodeMmsReadOrigInd(buff));
    mmsHeader->textValueMap_.insert(pair<uint8_t, std::string>(MMS_MESSAGE_ID, "auto"));
    EXPECT_FALSE(mmsHeader->EncodeMmsReadOrigInd(buff));
    mmsHeader->addressMap_.insert(pair<uint8_t, std::vector<MmsAddress>>(MMS_TO, addrs));
    EXPECT_FALSE(mmsHeader->EncodeMmsReadOrigInd(buff));
    mmsHeader->octetValueMap_.insert(pair<uint8_t, uint8_t>(MMS_READ_STATUS, 1));
    EXPECT_FALSE(mmsHeader->EncodeMmsReadOrigInd(buff));
    EXPECT_FALSE(mmsHeader->CheckBooleanValue(MMS_DRM_CONTENT, 0));
    EXPECT_FALSE(mmsHeader->CheckBooleanValue(MMS_BCC, 1));
}

/**
 * @tc.number   Telephony_SmsMmsGtest_MmsHeader_0005
 * @tc.name     Test MmsHeader
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, MmsHeader_0005, Function | MediumTest | Level1)
{
    auto mmsHeader = std::make_shared<MmsHeader>();
    MmsEncodeBuffer buff;
    MmsEncodeString value;
    std::string str = "q";
    mmsHeader->TrimString(str);
    str = "qwer";
    mmsHeader->TrimString(str);
    str = "<wer";
    mmsHeader->TrimString(str);
    EXPECT_FALSE(mmsHeader->EncodeTextStringValue(buff, 1, ""));
    EXPECT_FALSE(mmsHeader->EncodeEncodeStringValue(buff, 1, value));
    EXPECT_TRUE(mmsHeader->IsHaveTransactionId(MMS_MSGTYPE_SEND_REQ));
    EXPECT_TRUE(mmsHeader->IsHaveTransactionId(MMS_MSGTYPE_SEND_CONF));
    EXPECT_TRUE(mmsHeader->IsHaveTransactionId(MMS_MSGTYPE_NOTIFICATION_IND));
    EXPECT_TRUE(mmsHeader->IsHaveTransactionId(MMS_MSGTYPE_NOTIFYRESP_IND));
    EXPECT_TRUE(mmsHeader->IsHaveTransactionId(MMS_MSGTYPE_RETRIEVE_CONF));
    EXPECT_TRUE(mmsHeader->IsHaveTransactionId(MMS_MSGTYPE_ACKNOWLEDGE_IND));
    EXPECT_TRUE(mmsHeader->CheckResponseStatus(static_cast<uint8_t>(MmsResponseStatus::MMS_OK)));
    EXPECT_TRUE(mmsHeader->CheckResponseStatus(static_cast<uint8_t>(MmsResponseStatus::MMS_ERROR_UNSUPPORTED_MESSAGE)));
    EXPECT_TRUE(mmsHeader->CheckResponseStatus(static_cast<uint8_t>(MmsResponseStatus::MMS_ERROR_PERMANENT_FAILURE)));
    EXPECT_TRUE(
        mmsHeader->CheckResponseStatus(static_cast<uint8_t>(MmsResponseStatus::MMS_ERROR_PERMANENT_LACK_OF_PREPAID)));
    EXPECT_TRUE(mmsHeader->CheckRetrieveStatus(static_cast<uint8_t>(MmsRetrieveStatus::MMS_RETRIEVE_STATUS_OK)));
    EXPECT_TRUE(mmsHeader->CheckRetrieveStatus(
        static_cast<uint8_t>(MmsRetrieveStatus::MMS_RETRIEVE_STATUS_ERROR_PERMANENT_FAILURE)));
    EXPECT_TRUE(mmsHeader->CheckRetrieveStatus(
        static_cast<uint8_t>(MmsRetrieveStatus::MMS_RETRIEVE_STATUS_ERROR_PERMANENT_CONTENT_UNSUPPORTED)));
    EXPECT_TRUE(mmsHeader->CheckRetrieveStatus(
        static_cast<uint8_t>(MmsRetrieveStatus::MMS_RETRIEVE_STATUS_ERROR_TRANSIENT_FAILURE)));
    EXPECT_TRUE(mmsHeader->CheckRetrieveStatus(
        static_cast<uint8_t>(MmsRetrieveStatus::MMS_RETRIEVE_STATUS_ERROR_TRANSIENT_NETWORK_PROBLEM)));
    EXPECT_TRUE(mmsHeader->CheckStoreStatus(static_cast<uint8_t>(MmsStoreStatus::MMS_STORE_STATUS_SUCCESS)));
    EXPECT_TRUE(
        mmsHeader->CheckStoreStatus(static_cast<uint8_t>(MmsStoreStatus::MMS_STORE_STATUS_ERROR_TRANSIENT_FAILURE)));
    EXPECT_TRUE(mmsHeader->CheckStoreStatus(
        static_cast<uint8_t>(MmsStoreStatus::MMS_STORE_STATUS_ERROR_TRANSIENT_NETWORK_PROBLEM)));
    EXPECT_TRUE(
        mmsHeader->CheckStoreStatus(static_cast<uint8_t>(MmsStoreStatus::MMS_STORE_STATUS_ERROR_PERMANENT_FAILURE)));
    EXPECT_TRUE(
        mmsHeader->CheckStoreStatus(static_cast<uint8_t>(MmsStoreStatus::MMS_STORE_STATUS_ERROR_PERMANENT_MMBOX_FULL)));
    EXPECT_TRUE(mmsHeader->CheckBooleanValue(MMS_DELIVERY_REPORT, static_cast<uint8_t>(MmsBoolType::MMS_YES)));
    EXPECT_FALSE(mmsHeader->CheckBooleanValue(MMS_READ_REPORT, 0));
    EXPECT_FALSE(mmsHeader->CheckBooleanValue(MMS_REPORT_ALLOWED, 0));
    EXPECT_FALSE(mmsHeader->CheckBooleanValue(MMS_STORE, 0));
    EXPECT_FALSE(mmsHeader->CheckBooleanValue(MMS_STORED, 0));
    EXPECT_FALSE(mmsHeader->CheckBooleanValue(MMS_TOTALS, 0));
    EXPECT_FALSE(mmsHeader->CheckBooleanValue(MMS_QUOTAS, 0));
    EXPECT_FALSE(mmsHeader->CheckBooleanValue(MMS_DISTRIBUTION_INDICATOR, 0));
}

/**
 * @tc.number   Telephony_SmsMmsGtest_SmsWapPushBuffer_0001
 * @tc.name     Test SmsWapPushBuffer
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, SmsWapPushBuffer_0001, Function | MediumTest | Level1)
{
    auto smsWapPushBuffer = std::make_shared<SmsWapPushBuffer>();
    uint8_t oneByte = 0;
    uint8_t sValueLength = 0;
    uint32_t valueLength = 0;
    uint32_t length = 1;
    std::string str = "qwe";
    EXPECT_FALSE(smsWapPushBuffer->GetOneByte(oneByte));
    EXPECT_FALSE(smsWapPushBuffer->IncreasePointer(1));
    EXPECT_FALSE(smsWapPushBuffer->DecreasePointer(1));
    EXPECT_FALSE(smsWapPushBuffer->DecodeShortLength(sValueLength));
    EXPECT_FALSE(smsWapPushBuffer->DecodeValueLengthReturnLen(valueLength, length));
    EXPECT_FALSE(smsWapPushBuffer->DecodeUintvar(valueLength, length));
    EXPECT_FALSE(smsWapPushBuffer->DecodeValueLength(valueLength));
    EXPECT_FALSE(smsWapPushBuffer->DecodeText(str, length));
    EXPECT_FALSE(smsWapPushBuffer->DecodeShortInteger(oneByte));
    smsWapPushBuffer->totolLength_ = 1;
    smsWapPushBuffer->curPosition_ = 1;
    EXPECT_FALSE(smsWapPushBuffer->IncreasePointer(1));
    smsWapPushBuffer->curPosition_ = 0;
    smsWapPushBuffer->pduBuffer_ = std::make_unique<char[]>(1);
    smsWapPushBuffer->pduBuffer_[0] = 0;
    EXPECT_TRUE(smsWapPushBuffer->DecodeShortLength(sValueLength));
    smsWapPushBuffer->curPosition_ = 0;
    smsWapPushBuffer->pduBuffer_[0] = 0;
    EXPECT_TRUE(smsWapPushBuffer->DecodeValueLengthReturnLen(valueLength, length));
    smsWapPushBuffer->curPosition_ = 0;
    smsWapPushBuffer->pduBuffer_[0] = PDU_BUFFE_LENGTH_TWO;
    EXPECT_TRUE(smsWapPushBuffer->DecodeIsString());
    smsWapPushBuffer->pduBuffer_[0] = PDU_BUFFE_LENGTH_ONE;
    EXPECT_FALSE(smsWapPushBuffer->DecodeUintvar(valueLength, length));
    smsWapPushBuffer->curPosition_ = 0;
    EXPECT_TRUE(smsWapPushBuffer->DecodeShortInteger(oneByte));
    smsWapPushBuffer->curPosition_ = 0;
    EXPECT_FALSE(smsWapPushBuffer->DecodeValueLength(valueLength));
    smsWapPushBuffer->curPosition_ = 0;
    smsWapPushBuffer->pduBuffer_[0] = PDU_BUFFE_LENGTH_THREE;
    EXPECT_TRUE(smsWapPushBuffer->DecodeIsValueLength());
    EXPECT_FALSE(smsWapPushBuffer->DecodeValueLength(valueLength));
    smsWapPushBuffer->curPosition_ = 0;
    EXPECT_FALSE(smsWapPushBuffer->DecodeTokenText(str, length));
    smsWapPushBuffer->pduBuffer_[0] = PDU_BUFFE_LENGTH_TWO;
    EXPECT_TRUE(smsWapPushBuffer->DecodeText(str, length));
    EXPECT_FALSE(smsWapPushBuffer->CharIsToken(MIN_TOKEN));
    EXPECT_FALSE(smsWapPushBuffer->CharIsToken(MAX_TOKEN));
    smsWapPushBuffer->curPosition_ = 0;
    EXPECT_FALSE(smsWapPushBuffer->DecodeTokenText(str, length));
    smsWapPushBuffer->curPosition_ = 0;
    smsWapPushBuffer->pduBuffer_[0] = PDU_BUFFE_LENGTH;
    EXPECT_TRUE(smsWapPushBuffer->DecodeTokenText(str, length));
}

/**
 * @tc.number   Telephony_SmsMmsGtest_SmsWapPushBuffer_0002
 * @tc.name     Test SmsWapPushBuffer
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, SmsWapPushBuffer_0002, Function | MediumTest | Level1)
{
    auto smsWapPushBuffer = std::make_shared<SmsWapPushBuffer>();
    uint64_t iInteger = 0;
    std::string str = "qwe";
    bool isNoValue = true;
    uint32_t len = 1;
    smsWapPushBuffer->pduBuffer_ = std::make_unique<char[]>(1);
    EXPECT_FALSE(smsWapPushBuffer->DecodeLongInteger(iInteger));
    EXPECT_FALSE(smsWapPushBuffer->DecodeTextValue(str, isNoValue));
    EXPECT_FALSE(smsWapPushBuffer->DecodeNoValue(isNoValue));
    EXPECT_TRUE(smsWapPushBuffer->ReadDataBuffer(1, 1) == nullptr);
    smsWapPushBuffer->totolLength_ = 1;
    smsWapPushBuffer->pduBuffer_[0] = 1;
    EXPECT_FALSE(smsWapPushBuffer->DecodeLongInteger(iInteger));
    smsWapPushBuffer->curPosition_ = 0;
    smsWapPushBuffer->pduBuffer_[0] = PDU_BUFFE_LENGTH_THREE;
    EXPECT_FALSE(smsWapPushBuffer->DecodeLongInteger(iInteger));
    smsWapPushBuffer->curPosition_ = 0;
    smsWapPushBuffer->pduBuffer_[0] = 1;
    EXPECT_FALSE(smsWapPushBuffer->DecodeTextValue(str, isNoValue));
    EXPECT_TRUE(smsWapPushBuffer->ReadDataBuffer(1, 1) == nullptr);
    EXPECT_FALSE(smsWapPushBuffer->WriteRawStringBuffer(str));
    EXPECT_FALSE(smsWapPushBuffer->WriteDataBuffer(nullptr, 1));
    smsWapPushBuffer->curPosition_ = 0;
    smsWapPushBuffer->pduBuffer_[0] = 0;
    EXPECT_FALSE(smsWapPushBuffer->DecodeQuotedText(str, len));
    smsWapPushBuffer->curPosition_ = 0;
    smsWapPushBuffer->pduBuffer_[0] = PDU_BUFFE_LENGTH_TWO;
    EXPECT_TRUE(smsWapPushBuffer->DecodeQuotedText(str, len));
    smsWapPushBuffer->curPosition_ = 0;
    smsWapPushBuffer->pduBuffer_[0] = PDU_BUFFE_LENGTH_ONE;
    EXPECT_TRUE(smsWapPushBuffer->DecodeInteger(iInteger));
    smsWapPushBuffer->curPosition_ = 0;
    smsWapPushBuffer->pduBuffer_[0] = PDU_BUFFE_LENGTH_TWO;
    EXPECT_FALSE(smsWapPushBuffer->DecodeInteger(iInteger));
}

/**
 * @tc.number   Telephony_SmsMmsGtest_SmsReceiveHandler_0001
 * @tc.name     Test SmsReceiveHandler
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, SmsReceiveHandler_0001, Function | MediumTest | Level1)
{
    std::shared_ptr<AppExecFwk::EventRunner> runner = AppExecFwk::EventRunner::Create("test");
    AppExecFwk::InnerEvent::Pointer event = AppExecFwk::InnerEvent::Get(RadioEvent::RADIO_GSM_SMS, 1);
    std::shared_ptr<SmsReceiveHandler> smsReceiveHandler =
        std::make_shared<CdmaSmsReceiveHandler>(runner, INVALID_SLOTID);
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
    indexer->destPort_ = TEXT_PORT_NUM;
    reliabilityHandler->SendBroadcast(indexer, pdus);
    smsReceiveHandler->AddMsgToDB(indexer);
    smsReceiveHandler->IsRepeatedMessagePart(indexer);
    indexer = nullptr;
    EXPECT_FALSE(smsReceiveHandler->AddMsgToDB(indexer));
}

/**
 * @tc.number   Telephony_SmsMmsGtest_SmsCommonUtils_0001
 * @tc.name     Test SmsCommonUtils
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, SmsCommonUtils_0001, Function | MediumTest | Level1)
{
    auto smsCommonUtils = std::make_shared<SmsCommonUtils>();
    const unsigned char *userData = (const unsigned char *)TEXT_SMS_CONTENT.c_str();
    const std::string str = "*#PPQQ";
    const char *digit = (const char *)str.c_str();
    char *bcd = (char *)str.c_str();
    unsigned char *packData = (unsigned char *)TEXT_SMS_CONTENT.c_str();
    EXPECT_EQ(smsCommonUtils->Pack7bitChar(nullptr, 0, 0, nullptr), 0);
    EXPECT_EQ(smsCommonUtils->Unpack7bitChar(nullptr, 1, 1, nullptr, 1), 0);
    EXPECT_EQ(smsCommonUtils->Pack7bitChar(userData, 1, 0, packData), 1);
    EXPECT_EQ(smsCommonUtils->Pack7bitChar(userData, 1, 1, packData), 1);
    EXPECT_EQ(smsCommonUtils->Pack7bitChar(userData, 1, FILL_BITS, packData), FILL_BITS);
    EXPECT_EQ(smsCommonUtils->Unpack7bitChar(userData, 1, 0, packData, 1), 1);
    EXPECT_EQ(smsCommonUtils->Unpack7bitChar(userData, 1, FILL_BITS, packData, 1), 1);
    EXPECT_EQ(smsCommonUtils->DigitToBcd(digit, 1, nullptr), 0);
    EXPECT_EQ(smsCommonUtils->DigitToBcd(nullptr, 1, packData), 0);
    EXPECT_EQ(smsCommonUtils->DigitToBcd(digit, DIGIT_LEN, packData), FILL_BITS);
    EXPECT_EQ(smsCommonUtils->BcdToDigit(userData, 1, nullptr), 0);
    EXPECT_EQ(smsCommonUtils->BcdToDigit(nullptr, 1, bcd), 0);
    EXPECT_EQ(smsCommonUtils->BcdToDigitCdma(userData, 1, bcd), FILL_BITS);
    EXPECT_EQ(smsCommonUtils->BcdToDigitCdma(nullptr, 1, bcd), 0);
    EXPECT_EQ(smsCommonUtils->ConvertDigitToDTMF(nullptr, 1, 1, packData), 0);
    EXPECT_EQ(smsCommonUtils->ConvertDigitToDTMF(digit, 1, 1, packData), 0);
    EXPECT_EQ(smsCommonUtils->ConvertDigitToDTMF(digit, 1, START_BIT, packData), 0);
    EXPECT_EQ(smsCommonUtils->ConvertDigitToDTMF(digit, FILL_BITS, 0, packData), 1);
    EXPECT_EQ(smsCommonUtils->ConvertDigitToDTMF(digit, FILL_BITS, 0, packData), 1);
    EXPECT_EQ(smsCommonUtils->ConvertDigitToDTMF(digit, FILL_BITS, FILL_BITS, packData), 1);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_CdmaSmsSender_0001
 * @tc.name     Test CdmaSmsSender
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, CdmaSmsSender_0001, Function | MediumTest | Level1)
{
    std::shared_ptr<AppExecFwk::EventRunner> runner = AppExecFwk::EventRunner::Create("test");
    std::function<void(std::shared_ptr<SmsSendIndexer>)> fun = nullptr;
    auto cdmaSmsSender = std::make_shared<CdmaSmsSender>(runner, INVALID_SLOTID, fun);
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
    const std::string desAddr = "qwe";
    const std::string scAddr = "123";
    const std::string text = "123";
    cdmaSmsSender->isImsNetDomain_ = true;
    cdmaSmsSender->TextBasedSmsDelivery(desAddr, scAddr, text, sendCallback, deliveryCallback);
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
 * @tc.number   Telephony_SmsMmsGtest_SmsCbMessage_0001
 * @tc.name     Test SmsCbMessage
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, SmsCbMessage_0001, Function | MediumTest | Level1)
{
    auto smsCbMessage = std::make_shared<SmsCbMessage>();
    int8_t format = 1;
    std::string pdu = "123";
    std::string raw = "qwe";
    std::string message = "asd";
    uint8_t gs = 1;
    uint16_t serial = 1;
    bool isUserAlert = false;
    int64_t recvTime = 1;
    std::vector<unsigned char> pdus;
    EXPECT_FALSE(smsCbMessage->GetFormat(format));
    EXPECT_FALSE(smsCbMessage->GetPriority(format));
    EXPECT_NE(smsCbMessage->SmsCbMessage::ToString(), "");
    EXPECT_FALSE(smsCbMessage->GetGeoScope(gs));
    EXPECT_FALSE(smsCbMessage->GetSerialNum(serial));
    EXPECT_FALSE(smsCbMessage->IsEtwsEmergencyUserAlert(isUserAlert));
    EXPECT_FALSE(smsCbMessage->IsEtwsPopupAlert(isUserAlert));
    EXPECT_FALSE(smsCbMessage->GetServiceCategory(serial));
    EXPECT_FALSE(smsCbMessage->GetCmasResponseType(format));
    EXPECT_FALSE(smsCbMessage->GetCmasMessageClass(format));
    EXPECT_FALSE(smsCbMessage->GetWarningType(serial));
    EXPECT_FALSE(smsCbMessage->GetMsgType(gs));
    EXPECT_FALSE(smsCbMessage->GetLangType(gs));
    EXPECT_FALSE(smsCbMessage->GetDcs(gs));
    EXPECT_FALSE(smsCbMessage->GetReceiveTime(recvTime));
    EXPECT_FALSE(smsCbMessage->PduAnalysis(pdus));
    smsCbMessage->GetCmasCategory(format);
    smsCbMessage->ConvertToUTF8(raw, message);
    smsCbMessage->cbHeader_ = std::make_shared<SmsCbMessage::SmsCbMessageHeader>();
    smsCbMessage->cbHeader_->dcs.codingScheme = SMS_CODING_7BIT;
    smsCbMessage->ConvertToUTF8(raw, message);
    smsCbMessage->cbHeader_->dcs.codingScheme = SMS_CODING_UCS2;
    smsCbMessage->ConvertToUTF8(raw, message);
    smsCbMessage->cbHeader_->dcs.codingScheme = SMS_CODING_AUTO;
    smsCbMessage->ConvertToUTF8(raw, message);
    smsCbMessage->cbHeader_->bEtwsMessage = true;
    smsCbMessage->cbHeader_->cbEtwsType = SmsCbMessage::SMS_NETTEXT_ETWS_PRIMARY;
    smsCbMessage->ConvertToUTF8(raw, message);
    smsCbMessage->GetCbMessageRaw();
    EXPECT_TRUE(smsCbMessage->GetPriority(format));
    smsCbMessage->cbHeader_->msgId = PWS_FIRST_ID;
    EXPECT_TRUE(smsCbMessage->GetPriority(format));
    EXPECT_FALSE(smsCbMessage->IsSinglePageMsg());
    EXPECT_TRUE(smsCbMessage->CreateCbMessage(pdu) == nullptr);
    EXPECT_FALSE(smsCbMessage->CreateCbMessage(pdus));
    EXPECT_TRUE(smsCbMessage->GetCbHeader() != nullptr);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_SmsCbMessage_0002
 * @tc.name     Test SmsCbMessage
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, SmsCbMessage_0002, Function | MediumTest | Level1)
{
    auto smsCbMessage = std::make_shared<SmsCbMessage>();
    int8_t severity = 1;
    smsCbMessage->cbHeader_ = std::make_shared<SmsCbMessage::SmsCbMessageHeader>();
    smsCbMessage->cbHeader_->msgId = SmsCbMessage::SmsCmasMessageType::CMAS_EXTREME_OBSERVED_DEFUALT;
    EXPECT_TRUE(smsCbMessage->GetCmasSeverity(severity));
    EXPECT_TRUE(smsCbMessage->GetCmasUrgency(severity));
    smsCbMessage->cbHeader_->msgId = SmsCbMessage::SmsCmasMessageType::CMAS_EXTREME_OBSERVED_SPANISH;
    EXPECT_TRUE(smsCbMessage->GetCmasSeverity(severity));
    EXPECT_TRUE(smsCbMessage->GetCmasUrgency(severity));
    smsCbMessage->cbHeader_->msgId = SmsCbMessage::SmsCmasMessageType::CMAS_EXTREME_LIKELY_DEFUALT;
    EXPECT_TRUE(smsCbMessage->GetCmasSeverity(severity));
    EXPECT_TRUE(smsCbMessage->GetCmasUrgency(severity));
    smsCbMessage->cbHeader_->msgId = SmsCbMessage::SmsCmasMessageType::CMAS_EXTREME_LIKELY_SPANISH;
    EXPECT_TRUE(smsCbMessage->GetCmasSeverity(severity));
    EXPECT_TRUE(smsCbMessage->GetCmasUrgency(severity));
    smsCbMessage->cbHeader_->msgId = SmsCbMessage::SmsCmasMessageType::CMAS_EXTREME_OBSERVED_DEFUALT;
    EXPECT_TRUE(smsCbMessage->GetCmasSeverity(severity));
    EXPECT_TRUE(smsCbMessage->GetCmasUrgency(severity));
    smsCbMessage->cbHeader_->msgId = SmsCbMessage::SmsCmasMessageType::CMAS_SERVER_EXTREME_OBSERVED_SPANISH;
    EXPECT_TRUE(smsCbMessage->GetCmasSeverity(severity));
    EXPECT_TRUE(smsCbMessage->GetCmasUrgency(severity));
    smsCbMessage->cbHeader_->msgId = SmsCbMessage::SmsCmasMessageType::CMAS_SERVER_EXTERME_LIKELY_DEFUALT;
    EXPECT_TRUE(smsCbMessage->GetCmasSeverity(severity));
    EXPECT_TRUE(smsCbMessage->GetCmasUrgency(severity));
    smsCbMessage->cbHeader_->msgId = SmsCbMessage::SmsCmasMessageType::CMAS_SERVER_EXTERME_LIKELY_SPANISH;
    EXPECT_TRUE(smsCbMessage->GetCmasSeverity(severity));
    EXPECT_TRUE(smsCbMessage->GetCmasUrgency(severity));
    smsCbMessage->cbHeader_->msgId = SmsCbMessage::SmsCmasMessageType::CMAS_SERVER_SERVER_OBSERVED_DEFUALT;
    EXPECT_TRUE(smsCbMessage->GetCmasSeverity(severity));
    EXPECT_TRUE(smsCbMessage->GetCmasUrgency(severity));
    smsCbMessage->cbHeader_->msgId = SmsCbMessage::SmsCmasMessageType::CMAS_SERVER_SERVER_OBSERVED_SPANISH;
    EXPECT_TRUE(smsCbMessage->GetCmasSeverity(severity));
    EXPECT_TRUE(smsCbMessage->GetCmasUrgency(severity));
    smsCbMessage->cbHeader_->msgId = SmsCbMessage::SmsCmasMessageType::CMAS_SERVER_SERVER_LIKELY_DEFUALT;
    EXPECT_TRUE(smsCbMessage->GetCmasSeverity(severity));
    EXPECT_TRUE(smsCbMessage->GetCmasUrgency(severity));
    smsCbMessage->cbHeader_->msgId = SmsCbMessage::SmsCmasMessageType::CMAS_SERVER_SERVER_LIKELY_SPANISH;
    EXPECT_TRUE(smsCbMessage->GetCmasSeverity(severity));
    EXPECT_TRUE(smsCbMessage->GetCmasUrgency(severity));
    smsCbMessage->cbHeader_->msgId = SmsCbMessage::SmsCmasMessageType::CMAS_SERVER_SERVER_EXPECTED_OBSERVED_DEFUALT;
    EXPECT_TRUE(smsCbMessage->GetCmasSeverity(severity));
    EXPECT_TRUE(smsCbMessage->GetCmasUrgency(severity));
    smsCbMessage->cbHeader_->msgId = SmsCbMessage::SmsCmasMessageType::CMAS_SERVER_SERVER_EXPECTED_OBSERVED_SPANISH;
    EXPECT_TRUE(smsCbMessage->GetCmasSeverity(severity));
    EXPECT_TRUE(smsCbMessage->GetCmasUrgency(severity));
    smsCbMessage->cbHeader_->msgId = SmsCbMessage::SmsCmasMessageType::CMAS_SERVER_SERVER_EXPECTED_LIKELY_DEFUALT;
    EXPECT_TRUE(smsCbMessage->GetCmasSeverity(severity));
    EXPECT_TRUE(smsCbMessage->GetCmasUrgency(severity));
}

/**
 * @tc.number   Telephony_SmsMmsGtest_SmsCbMessage_0003
 * @tc.name     Test SmsCbMessage
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, SmsCbMessage_0003, Function | MediumTest | Level1)
{
    auto smsCbMessage = std::make_shared<SmsCbMessage>();
    int8_t severity = 1;
    smsCbMessage->cbHeader_ = std::make_shared<SmsCbMessage::SmsCbMessageHeader>();
    smsCbMessage->cbHeader_->msgId = SmsCbMessage::SmsCmasMessageType::CMAS_SERVER_SERVER_EXPECTED_LIKELY_SPANISH;
    EXPECT_TRUE(smsCbMessage->GetCmasSeverity(severity));
    EXPECT_TRUE(smsCbMessage->GetCmasUrgency(severity));
    EXPECT_TRUE(smsCbMessage->GetCmasCertainty(severity));
    smsCbMessage->cbHeader_->msgId = SmsCbMessage::SmsCmasMessageType::CMAS_OPERATOR_ALERT_SPANISH;
    EXPECT_TRUE(smsCbMessage->GetCmasSeverity(severity));
    EXPECT_TRUE(smsCbMessage->GetCmasUrgency(severity));
    EXPECT_TRUE(smsCbMessage->GetCmasCertainty(severity));
    smsCbMessage->cbHeader_->msgId = SmsCbMessage::SmsCmasMessageType::CMAS_EXTREME_OBSERVED_DEFUALT;
    EXPECT_TRUE(smsCbMessage->GetCmasCertainty(severity));
    smsCbMessage->cbHeader_->msgId = SmsCbMessage::SmsCmasMessageType::CMAS_EXTREME_OBSERVED_SPANISH;
    EXPECT_TRUE(smsCbMessage->GetCmasCertainty(severity));
    smsCbMessage->cbHeader_->msgId = SmsCbMessage::SmsCmasMessageType::CMAS_SERVER_EXTERME_OBSERVED_DEFUALT;
    EXPECT_TRUE(smsCbMessage->GetCmasCertainty(severity));
    smsCbMessage->cbHeader_->msgId = SmsCbMessage::SmsCmasMessageType::CMAS_SERVER_EXTREME_OBSERVED_SPANISH;
    EXPECT_TRUE(smsCbMessage->GetCmasCertainty(severity));
    smsCbMessage->cbHeader_->msgId = SmsCbMessage::SmsCmasMessageType::CMAS_SERVER_SERVER_OBSERVED_DEFUALT;
    EXPECT_TRUE(smsCbMessage->GetCmasCertainty(severity));
    smsCbMessage->cbHeader_->msgId = SmsCbMessage::SmsCmasMessageType::CMAS_SERVER_SERVER_OBSERVED_SPANISH;
    EXPECT_TRUE(smsCbMessage->GetCmasCertainty(severity));
    smsCbMessage->cbHeader_->msgId = SmsCbMessage::SmsCmasMessageType::CMAS_SERVER_SERVER_EXPECTED_OBSERVED_DEFUALT;
    EXPECT_TRUE(smsCbMessage->GetCmasCertainty(severity));
    smsCbMessage->cbHeader_->msgId = SmsCbMessage::SmsCmasMessageType::CMAS_SERVER_SERVER_EXPECTED_OBSERVED_SPANISH;
    EXPECT_TRUE(smsCbMessage->GetCmasCertainty(severity));
    smsCbMessage->cbHeader_->msgId = SmsCbMessage::SmsCmasMessageType::CMAS_EXTREME_LIKELY_DEFUALT;
    EXPECT_TRUE(smsCbMessage->GetCmasCertainty(severity));
    smsCbMessage->cbHeader_->msgId = SmsCbMessage::SmsCmasMessageType::CMAS_EXTREME_LIKELY_SPANISH;
    EXPECT_TRUE(smsCbMessage->GetCmasCertainty(severity));
    smsCbMessage->cbHeader_->msgId = SmsCbMessage::SmsCmasMessageType::CMAS_SERVER_EXTERME_LIKELY_DEFUALT;
    EXPECT_TRUE(smsCbMessage->GetCmasCertainty(severity));
    smsCbMessage->cbHeader_->msgId = SmsCbMessage::SmsCmasMessageType::CMAS_SERVER_EXTERME_LIKELY_SPANISH;
    EXPECT_TRUE(smsCbMessage->GetCmasCertainty(severity));
    smsCbMessage->cbHeader_->msgId = SmsCbMessage::SmsCmasMessageType::CMAS_SERVER_SERVER_LIKELY_DEFUALT;
    EXPECT_TRUE(smsCbMessage->GetCmasCertainty(severity));
    smsCbMessage->cbHeader_->msgId = SmsCbMessage::SmsCmasMessageType::CMAS_SERVER_SERVER_LIKELY_SPANISH;
    EXPECT_TRUE(smsCbMessage->GetCmasCertainty(severity));
    smsCbMessage->cbHeader_->msgId = SmsCbMessage::SmsCmasMessageType::CMAS_SERVER_SERVER_EXPECTED_LIKELY_DEFUALT;
    EXPECT_TRUE(smsCbMessage->GetCmasCertainty(severity));
}

/**
 * @tc.number   Telephony_SmsMmsGtest_SmsCbMessage_0004
 * @tc.name     Test SmsCbMessage
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, SmsCbMessage_0004, Function | MediumTest | Level1)
{
    auto smsCbMessage = std::make_shared<SmsCbMessage>();
    std::vector<unsigned char> pdu;
    smsCbMessage->Decode2gCbMsg(pdu);
    smsCbMessage->Decode3gCbMsg(pdu);
    smsCbMessage->cbHeader_ = std::make_shared<SmsCbMessage::SmsCbMessageHeader>();
    smsCbMessage->cbHeader_->dcs.codingScheme = SmsCodingScheme::SMS_CODING_7BIT;
    smsCbMessage->Decode2gCbMsg(pdu);
    smsCbMessage->Decode3gCbMsg(pdu);
    smsCbMessage->cbHeader_->dcs.codingScheme = SmsCodingScheme::SMS_CODING_8BIT;
    smsCbMessage->Decode2gCbMsg(pdu);
    smsCbMessage->Decode3gCbMsg(pdu);
    smsCbMessage->cbHeader_->dcs.codingScheme = SmsCodingScheme::SMS_CODING_UCS2;
    smsCbMessage->Decode2gCbMsg(pdu);
    smsCbMessage->Decode3gCbMsg(pdu);
    smsCbMessage->cbHeader_->dcs.codingScheme = SmsCodingScheme::SMS_CODING_ASCII7BIT;
    smsCbMessage->Decode2gCbMsg(pdu);
    smsCbMessage->Decode3gCbMsg(pdu);
    unsigned char data = 1;
    pdu.push_back(data);
    smsCbMessage->cbHeader_->totalPages = 1;
    smsCbMessage->Decode3g7Bit(pdu);
    smsCbMessage->Decode3gUCS2(pdu);
    EXPECT_EQ(smsCbMessage->CMASClass(SmsCbMessage::CMAS_PRESIDENTIAL_DEFUALT),
        SmsCbMessage::SmsMessageSubType::MSG_CMAS_PRESIDENTIAL);
    EXPECT_EQ(smsCbMessage->CMASClass(SmsCbMessage::CMAS_PRESIDENTIAL_SPANISH),
        SmsCbMessage::SmsMessageSubType::MSG_CMAS_PRESIDENTIAL);
    EXPECT_EQ(smsCbMessage->CMASClass(SmsCbMessage::CMAS_EXTREME_OBSERVED_DEFUALT),
        SmsCbMessage::SmsMessageSubType::MSG_CMAS_EXTREME);
    EXPECT_EQ(smsCbMessage->CMASClass(SmsCbMessage::CMAS_EXTREME_OBSERVED_SPANISH),
        SmsCbMessage::SmsMessageSubType::MSG_CMAS_EXTREME);
    EXPECT_EQ(smsCbMessage->CMASClass(SmsCbMessage::CMAS_EXTREME_LIKELY_DEFUALT),
        SmsCbMessage::SmsMessageSubType::MSG_CMAS_EXTREME);
    EXPECT_EQ(smsCbMessage->CMASClass(SmsCbMessage::CMAS_EXTREME_LIKELY_SPANISH),
        SmsCbMessage::SmsMessageSubType::MSG_CMAS_EXTREME);
    EXPECT_EQ(smsCbMessage->CMASClass(SmsCbMessage::CMAS_SERVER_EXTERME_OBSERVED_DEFUALT),
        SmsCbMessage::SmsMessageSubType::MSG_CMAS_SEVERE);
    EXPECT_EQ(smsCbMessage->CMASClass(SmsCbMessage::CMAS_SERVER_EXTREME_OBSERVED_SPANISH),
        SmsCbMessage::SmsMessageSubType::MSG_CMAS_SEVERE);
    EXPECT_EQ(smsCbMessage->CMASClass(SmsCbMessage::CMAS_SERVER_EXTERME_LIKELY_DEFUALT),
        SmsCbMessage::SmsMessageSubType::MSG_CMAS_SEVERE);
    EXPECT_EQ(smsCbMessage->CMASClass(SmsCbMessage::CMAS_SERVER_EXTERME_LIKELY_SPANISH),
        SmsCbMessage::SmsMessageSubType::MSG_CMAS_SEVERE);
    EXPECT_EQ(smsCbMessage->CMASClass(SmsCbMessage::CMAS_SERVER_SERVER_OBSERVED_DEFUALT),
        SmsCbMessage::SmsMessageSubType::MSG_CMAS_SEVERE);
    EXPECT_EQ(smsCbMessage->CMASClass(SmsCbMessage::CMAS_SERVER_SERVER_OBSERVED_SPANISH),
        SmsCbMessage::SmsMessageSubType::MSG_CMAS_SEVERE);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_SmsCbMessage_0005
 * @tc.name     Test SmsCbMessage
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, SmsCbMessage_0005, Function | MediumTest | Level1)
{
    auto smsCbMessage = std::make_shared<SmsCbMessage>();
    EXPECT_EQ(smsCbMessage->CMASClass(SmsCbMessage::CMAS_SERVER_SERVER_LIKELY_DEFUALT),
        SmsCbMessage::SmsMessageSubType::MSG_CMAS_SEVERE);
    EXPECT_EQ(smsCbMessage->CMASClass(SmsCbMessage::CMAS_SERVER_SERVER_LIKELY_SPANISH),
        SmsCbMessage::SmsMessageSubType::MSG_CMAS_SEVERE);
    EXPECT_EQ(smsCbMessage->CMASClass(SmsCbMessage::CMAS_SERVER_SERVER_EXPECTED_OBSERVED_DEFUALT),
        SmsCbMessage::SmsMessageSubType::MSG_CMAS_SEVERE);
    EXPECT_EQ(smsCbMessage->CMASClass(SmsCbMessage::CMAS_SERVER_SERVER_EXPECTED_OBSERVED_SPANISH),
        SmsCbMessage::SmsMessageSubType::MSG_CMAS_SEVERE);
    EXPECT_EQ(smsCbMessage->CMASClass(SmsCbMessage::CMAS_SERVER_SERVER_EXPECTED_LIKELY_SPANISH),
        SmsCbMessage::SmsMessageSubType::MSG_CMAS_SEVERE);
    EXPECT_EQ(smsCbMessage->CMASClass(SmsCbMessage::CMAS_SERVER_SERVER_EXPECTED_LIKELY_SPANISH),
        SmsCbMessage::SmsMessageSubType::MSG_CMAS_SEVERE);
    EXPECT_EQ(smsCbMessage->CMASClass(SmsCbMessage::CMAS_AMBER_ALERT_DEFUALT),
        SmsCbMessage::SmsMessageSubType::MSG_CMAS_AMBER);
    EXPECT_EQ(smsCbMessage->CMASClass(SmsCbMessage::CMAS_AMBER_ALERT_SPANISH),
        SmsCbMessage::SmsMessageSubType::MSG_CMAS_AMBER);
    EXPECT_EQ(
        smsCbMessage->CMASClass(SmsCbMessage::CMAS_RMT_ALERT_DEFUALT), SmsCbMessage::SmsMessageSubType::MSG_CMAS_TEST);
    EXPECT_EQ(
        smsCbMessage->CMASClass(SmsCbMessage::CMAS_RMT_ALERT_SPANISH), SmsCbMessage::SmsMessageSubType::MSG_CMAS_TEST);
    EXPECT_EQ(smsCbMessage->CMASClass(SmsCbMessage::CMAS_EXERCISE_ALERT_DEFUALT),
        SmsCbMessage::SmsMessageSubType::MSG_CMAS_EXERCISE);
    EXPECT_EQ(smsCbMessage->CMASClass(SmsCbMessage::CMAS_EXERCISE_ALERT_SPANISH),
        SmsCbMessage::SmsMessageSubType::MSG_CMAS_EXERCISE);
    EXPECT_EQ(smsCbMessage->CMASClass(SmsCbMessage::CMAS_OPERATOR_ALERT_DEFUALT),
        SmsCbMessage::SmsMessageSubType::MSG_CMAS_OPERATOR_DEFINED);
    EXPECT_EQ(smsCbMessage->CMASClass(SmsCbMessage::CMAS_OPERATOR_ALERT_SPANISH),
        SmsCbMessage::SmsMessageSubType::MSG_CMAS_OPERATOR_DEFINED);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_GsmSmsCbHandler_0001
 * @tc.name     Test GsmSmsCbHandler
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, GsmSmsCbHandler_0001, Function | MediumTest | Level1)
{
    std::shared_ptr<AppExecFwk::EventRunner> runner = AppExecFwk::EventRunner::Create("test");
    auto gsmSmsCbHandler = std::make_shared<GsmSmsCbHandler>(runner, INVALID_SLOTID);
    auto cbMessage = std::make_shared<SmsCbMessage>();
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
    cbMessage->cbHeader_ = std::make_shared<SmsCbMessage::SmsCbMessageHeader>();
    EXPECT_EQ(gsmSmsCbHandler->CheckCbMessage(cbMessage), 0);
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
    gsmSmsCbHandler->HandleCbMessage(message);
    EXPECT_FALSE(gsmSmsCbHandler->CheckCbActive(cbMessage));
    EXPECT_FALSE(gsmSmsCbHandler->AddCbMessageToList(cbMessage));
    EXPECT_FALSE(gsmSmsCbHandler->SendCbMessageBroadcast(cbMessage));
    EXPECT_TRUE(gsmSmsCbHandler->FindCbMessage(cbMessage) == nullptr);
    EXPECT_EQ(gsmSmsCbHandler->CheckCbMessage(cbMessage), 0);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_SmsWapPushHandler_0001
 * @tc.name     Test SmsWapPushHandler
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, SmsWapPushHandler_0001, Function | MediumTest | Level1)
{
    auto smsWapPushHandler = std::make_shared<SmsWapPushHandler>(INVALID_SLOTID);
    SmsWapPushBuffer decodeBuffer;
    std::string wapPdu = "";
    EXPECT_FALSE(smsWapPushHandler->DecodeWapPushPduData(decodeBuffer, 1, 1));
    auto indexer = std::make_shared<SmsReceiveIndexer>();
    EXPECT_FALSE(smsWapPushHandler->DecodeWapPushPdu(indexer, wapPdu));
    EXPECT_FALSE(smsWapPushHandler->DeocdeCheckIsBlock(wapPdu));
    wapPdu = "qwer";
    EXPECT_FALSE(smsWapPushHandler->DecodeWapPushPdu(indexer, wapPdu));
    EXPECT_FALSE(smsWapPushHandler->DecodePushType(decodeBuffer));
    EXPECT_FALSE(smsWapPushHandler->DeocdeCheckIsBlock(wapPdu));
    EXPECT_FALSE(smsWapPushHandler->DecodeXWapApplication(decodeBuffer, 1));
    EXPECT_FALSE(smsWapPushHandler->DecodeXWapAbandonHeaderValue(decodeBuffer));
    decodeBuffer.totolLength_ = 1;
    EXPECT_FALSE(smsWapPushHandler->DecodeXWapApplication(decodeBuffer, 1));
    EXPECT_TRUE(smsWapPushHandler->SendWapPushMessageBroadcast(indexer));
}

/**
 * @tc.number   Telephony_SmsMmsGtest_ShortMessage_0001
 * @tc.name     Test ShortMessage
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, ShortMessage_0001, Function | MediumTest | Level1)
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
 * @tc.number   Telephony_SmsMmsGtest_GsmSmsMessage_0001
 * @tc.name     Test GsmSmsMessage
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, GsmSmsMessage_0001, Function | MediumTest | Level1)
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
    EXPECT_EQ(gsmSmsMessage->SetHeaderLang(1, SmsCodingScheme::SMS_CODING_UCS2, langId), 0);
    EXPECT_EQ(gsmSmsMessage->SetHeaderConcat(1, concat), 0);
    EXPECT_EQ(gsmSmsMessage->SetHeaderReply(1), 0);
    EXPECT_TRUE(
        gsmSmsMessage->CreateDefaultSubmitSmsTpdu(dest, str, text, true, SmsCodingScheme::SMS_CODING_7BIT) != nullptr);
    EXPECT_NE(gsmSmsMessage->GetDestPort(), -1);
    msgText = "123";
    desAddr = "+SetSmsTpduDestAddress";
    EXPECT_EQ(gsmSmsMessage->SetSmsTpduDestAddress(tPdu, desAddr), 22);
    desAddr = "SetSmsTpduDestAddress";
    EXPECT_EQ(gsmSmsMessage->SetSmsTpduDestAddress(tPdu, desAddr), 21);
    tPdu = nullptr;
    EXPECT_EQ(gsmSmsMessage->SetSmsTpduDestAddress(tPdu, desAddr), 0);
    langId = 1;
    EXPECT_EQ(gsmSmsMessage->SetHeaderLang(1, SmsCodingScheme::SMS_CODING_7BIT, langId), 1);
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
HWTEST_F(BranchTest, GsmSmsMessage_0002, Function | MediumTest | Level1)
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
    EXPECT_TRUE(gsmSmsMessage->PduAnalysis(pdu));
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
    EXPECT_TRUE(gsmSmsMessage->PduAnalysis(pdu));
}

/**
 * @tc.number   Telephony_SmsMmsGtest_CdmaSmsMessage_0001
 * @tc.name     Test CdmaSmsMessage
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, CdmaSmsMessage_0001, Function | MediumTest | Level1)
{
    std::shared_ptr<CdmaSmsMessage> cdmaSmsMessage = std::make_shared<CdmaSmsMessage>();
    string pduHex = "";
    SmsCodingScheme codingType;
    SmsTransP2PMsg p2pMsg;
    p2pMsg.telesvcMsg.type = SmsMessageType::SMS_TYPE_DELIVER;
    cdmaSmsMessage->AnalysisP2pMsg(p2pMsg);
    p2pMsg.telesvcMsg.type = SmsMessageType::SMS_TYPE_DELIVERY_ACK;
    cdmaSmsMessage->AnalysisP2pMsg(p2pMsg);
    p2pMsg.telesvcMsg.type = SmsMessageType::SMS_TYPE_USER_ACK;
    cdmaSmsMessage->AnalysisP2pMsg(p2pMsg);
    p2pMsg.telesvcMsg.type = SmsMessageType::SMS_TYPE_READ_ACK;
    cdmaSmsMessage->AnalysisP2pMsg(p2pMsg);
    p2pMsg.telesvcMsg.type = SmsMessageType::SMS_TYPE_SUBMIT_REPORT;
    cdmaSmsMessage->AnalysisP2pMsg(p2pMsg);
    p2pMsg.telesvcMsg.type = SmsMessageType::SMS_TYPE_SUBMIT;
    cdmaSmsMessage->AnalysisP2pMsg(p2pMsg);
    p2pMsg.telesvcMsg.type = SmsMessageType::SMS_TYPE_MAX_VALUE;
    cdmaSmsMessage->AnalysisP2pMsg(p2pMsg);
    p2pMsg.transTelesvcId = SmsTransTelsvcId::SMS_TRANS_TELESVC_RESERVED;
    cdmaSmsMessage->AnalysisP2pMsg(p2pMsg);
    EXPECT_FALSE(cdmaSmsMessage->IsWapPushMsg());
    EXPECT_EQ(cdmaSmsMessage->GetTransMsgType(), SmsTransMsgType::SMS_TRANS_TYPE_RESERVED);
    EXPECT_EQ(cdmaSmsMessage->GetTransTeleService(), SmsTransTelsvcId::SMS_TRANS_TELESVC_RESERVED);
    EXPECT_FALSE(cdmaSmsMessage->PduAnalysis(pduHex));
    cdmaSmsMessage->transMsg_ = std::make_unique<struct SmsTransMsg>();
    cdmaSmsMessage->transMsg_->type = SmsTransMsgType::SMS_TRANS_BROADCAST_MSG;
    EXPECT_EQ(cdmaSmsMessage->GetTransTeleService(), SmsTransTelsvcId::SMS_TRANS_TELESVC_RESERVED);
    cdmaSmsMessage->transMsg_->type = SmsTransMsgType::SMS_TRANS_P2P_MSG;
    cdmaSmsMessage->transMsg_->data.p2pMsg.transTelesvcId = SmsTransTelsvcId::SMS_TRANS_TELESVC_WAP;
    EXPECT_TRUE(cdmaSmsMessage->IsWapPushMsg());
    codingType = SmsCodingScheme::SMS_CODING_7BIT;
    EXPECT_EQ(cdmaSmsMessage->CovertEncodingType(codingType), SmsEncodingType::SMS_ENCODE_GSM7BIT);
    codingType = SmsCodingScheme::SMS_CODING_ASCII7BIT;
    EXPECT_EQ(cdmaSmsMessage->CovertEncodingType(codingType), SmsEncodingType::SMS_ENCODE_7BIT_ASCII);
    codingType = SmsCodingScheme::SMS_CODING_8BIT;
    EXPECT_EQ(cdmaSmsMessage->CovertEncodingType(codingType), SmsEncodingType::SMS_ENCODE_OCTET);
    codingType = SmsCodingScheme::SMS_CODING_UCS2;
    EXPECT_EQ(cdmaSmsMessage->CovertEncodingType(codingType), SmsEncodingType::SMS_ENCODE_UNICODE);
    codingType = SmsCodingScheme::SMS_CODING_EUCKR;
    EXPECT_EQ(cdmaSmsMessage->CovertEncodingType(codingType), SmsEncodingType::SMS_ENCODE_UNICODE);
    EXPECT_FALSE(cdmaSmsMessage->PduAnalysis(pduHex));
    pduHex = "12";
    EXPECT_FALSE(cdmaSmsMessage->PduAnalysis(pduHex));
}

/**
 * @tc.number   Telephony_SmsMmsGtest_CdmaSmsMessage_0002
 * @tc.name     Test CdmaSmsMessage
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, CdmaSmsMessage_0002, Function | MediumTest | Level1)
{
    std::shared_ptr<CdmaSmsMessage> cdmaSmsMessage = std::make_shared<CdmaSmsMessage>();
    SmsTransP2PMsg p2pMsg;
    SmsTeleSvcDeliver deliver;
    SmsTeleSvcUserData userData;
    SmsTransBroadCastMsg cbMsg;
    SmsUDH header;
    p2pMsg.transTelesvcId = SmsTransTelsvcId::SMS_TRANS_TELESVC_VMN_95;
    p2pMsg.telesvcMsg.data.deliver.enhancedVmn.faxIncluded = true;
    p2pMsg.telesvcMsg.data.deliver.userData.userData.length = 0;
    deliver.displayMode = SmsDisplayMode::SMS_DISPLAY_IMMEDIATE;
    cdmaSmsMessage->AnalsisDeliverMwi(p2pMsg);
    cdmaSmsMessage->AnalsisDeliverMsg(deliver);
    userData.encodeType = SmsEncodingType::SMS_ENCODE_GSM7BIT;
    cdmaSmsMessage->AnalsisUserData(userData);
    userData.encodeType = SmsEncodingType::SMS_ENCODE_KOREAN;
    cdmaSmsMessage->AnalsisUserData(userData);
    userData.encodeType = SmsEncodingType::SMS_ENCODE_EUCKR;
    cdmaSmsMessage->AnalsisUserData(userData);
    userData.encodeType = SmsEncodingType::SMS_ENCODE_IA5;
    cdmaSmsMessage->AnalsisUserData(userData);
    userData.encodeType = SmsEncodingType::SMS_ENCODE_7BIT_ASCII;
    cdmaSmsMessage->AnalsisUserData(userData);
    userData.encodeType = SmsEncodingType::SMS_ENCODE_LATIN_HEBREW;
    cdmaSmsMessage->AnalsisUserData(userData);
    userData.encodeType = SmsEncodingType::SMS_ENCODE_LATIN;
    cdmaSmsMessage->AnalsisUserData(userData);
    userData.encodeType = SmsEncodingType::SMS_ENCODE_OCTET;
    cdmaSmsMessage->AnalsisUserData(userData);
    userData.encodeType = SmsEncodingType::SMS_ENCODE_SHIFT_JIS;
    cdmaSmsMessage->AnalsisUserData(userData);
    userData.encodeType = SmsEncodingType::SMS_ENCODE_RESERVED;
    cdmaSmsMessage->AnalsisUserData(userData);
    deliver.cmasData.dataLen = DATA_LENGTH;
    cdmaSmsMessage->AnalsisCMASMsg(deliver);
    cbMsg.telesvcMsg.type = SmsMessageType::SMS_TYPE_RESERVED;
    cdmaSmsMessage->AnalysisCbMsg(cbMsg);
    cbMsg.telesvcMsg.type = SmsMessageType::SMS_TYPE_DELIVER;
    cdmaSmsMessage->AnalysisCbMsg(cbMsg);
    cbMsg.transSvcCtg = SmsTransSvcCtg::SMS_TRANS_SVC_CTG_CMAS_AMBER;
    cdmaSmsMessage->AnalysisCbMsg(cbMsg);
    userData.userData.headerCnt = HEADER_LENGTH;
    cdmaSmsMessage->bHeaderInd_ = true;
    cdmaSmsMessage->AnalsisHeader(userData);
    EXPECT_FALSE(cdmaSmsMessage->AddUserDataHeader(header));
}

/**
 * @tc.number   Telephony_SmsMmsGtest_SmsSender_0001
 * @tc.name     Test SmsSender
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, SmsSender_0001, Function | MediumTest | Level1)
{
    std::shared_ptr<AppExecFwk::EventRunner> runner = AppExecFwk::EventRunner::Create("test");
    std::function<void(std::shared_ptr<SmsSendIndexer>)> fun = nullptr;
    AppExecFwk::InnerEvent::Pointer event = AppExecFwk::InnerEvent::Get(RadioEvent::RADIO_SEND_SMS, 1);
    std::shared_ptr<SmsSender> smsSender = std::make_shared<CdmaSmsSender>(runner, INVALID_SLOTID, fun);
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
    event->GetSharedObject<HRilRadioResponseInfo>() = std::make_shared<HRilRadioResponseInfo>();
    smsSender->HandleResend(smsIndexer);
    smsIndexer->errorCode_ = HRIL_ERR_CMD_SEND_FAILURE;
    smsSender->HandleResend(smsIndexer);
    smsIndexer->errorCode_ = HRIL_ERR_GENERIC_FAILURE;
    smsSender->HandleResend(smsIndexer);
    smsSender->lastSmsDomain_ = 1;
    smsSender->HandleResend(smsIndexer);
    EXPECT_TRUE(smsSender->SendCacheMapAddItem(1, smsIndexer));
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
HWTEST_F(BranchTest, SmsSender_0002, Function | MediumTest | Level1)
{
    std::shared_ptr<AppExecFwk::EventRunner> runner = AppExecFwk::EventRunner::Create("test");
    std::function<void(std::shared_ptr<SmsSendIndexer>)> fun = nullptr;
    std::shared_ptr<SmsSender> smsSender = std::make_shared<CdmaSmsSender>(runner, INVALID_SLOTID, fun);
    const sptr<ISendShortMessageCallback> sendCallback =
        iface_cast<ISendShortMessageCallback>(new SendShortMessageCallbackStub());
    const sptr<IDeliveryShortMessageCallback> deliveryCallback =
        iface_cast<IDeliveryShortMessageCallback>(new DeliveryShortMessageCallbackStub());
    const std::string desAddr = "qwe";
    const std::string scAddr = "123";
    const std::string text = "123";
    auto smsIndexer = std::make_shared<SmsSendIndexer>(desAddr, scAddr, text, sendCallback, deliveryCallback);
    smsIndexer->unSentCellCount_ = std::make_shared<uint8_t>(1);
    smsSender->SendMessageSucceed(smsIndexer);
    smsIndexer->unSentCellCount_ = std::make_shared<uint8_t>(1);
    smsIndexer->hasCellFailed_ = std::make_shared<bool>(true);
    smsSender->SendMessageSucceed(smsIndexer);
    smsIndexer->unSentCellCount_ = std::make_shared<uint8_t>(1);
    smsSender->SendMessageFailed(smsIndexer);
    smsIndexer->unSentCellCount_ = nullptr;
    smsSender->SendMessageFailed(smsIndexer);
    smsSender->SendResultCallBack(smsIndexer, ISendShortMessageCallback::SEND_SMS_FAILURE_UNKNOWN);
    smsSender->SendResultCallBack(sendCallback, ISendShortMessageCallback::SEND_SMS_FAILURE_UNKNOWN);
    smsIndexer = nullptr;
    smsSender->SendMessageSucceed(smsIndexer);
    smsSender->SendMessageFailed(smsIndexer);
    EXPECT_FALSE(smsSender->SendCacheMapAddItem(1, smsIndexer));
}

/**
 * @tc.number   Telephony_SmsMmsGtest_SmsWapPushContentType_0001
 * @tc.name     Test SmsWapPushContentType
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, SmsWapPushContentType_0001, Function | MediumTest | Level1)
{
    auto smsWapPushContentType = std::make_shared<SmsWapPushContentType>();
    SmsWapPushBuffer decodeBuffer;
    int32_t contentLength = 1;
    decodeBuffer.totolLength_ = 1;
    decodeBuffer.pduBuffer_ = std::make_unique<char[]>(1);
    decodeBuffer.pduBuffer_[0] = PDU_BUFFE_LENGTH_ONE;
    EXPECT_FALSE(smsWapPushContentType->DecodeCTGeneralForm(decodeBuffer, contentLength));
    EXPECT_TRUE(smsWapPushContentType->DecodeContentType(decodeBuffer, contentLength));
    decodeBuffer.curPosition_ = 0;
    decodeBuffer.pduBuffer_[0] = PDU_BUFFE_LENGTH_TWO;
    EXPECT_TRUE(smsWapPushContentType->DecodeContentType(decodeBuffer, contentLength));
    decodeBuffer.curPosition_ = 0;
    decodeBuffer.pduBuffer_[0] = 0;
    EXPECT_FALSE(smsWapPushContentType->DecodeContentType(decodeBuffer, contentLength));
    EXPECT_FALSE(smsWapPushContentType->DecodeCTGeneralForm(decodeBuffer, contentLength));
    decodeBuffer.pduBuffer_[0] = PDU_BUFFE_LENGTH_THREE;
    decodeBuffer.curPosition_ = 1;
    EXPECT_FALSE(smsWapPushContentType->DecodeParameter(decodeBuffer, 1));
    EXPECT_FALSE(smsWapPushContentType->DecodeCTGeneralForm(decodeBuffer, contentLength));
    decodeBuffer.curPosition_ = 0;
    decodeBuffer.pduBuffer_[0] = PDU_BUFFE_LENGTH_THREE;
    EXPECT_FALSE(smsWapPushContentType->DecodeCTGeneralForm(decodeBuffer, contentLength));
    decodeBuffer.curPosition_ = 0;
    decodeBuffer.pduBuffer_[0] = PDU_BUFFE_LENGTH_FOUR;
    EXPECT_FALSE(smsWapPushContentType->DecodeParameter(decodeBuffer, 1));
    decodeBuffer.curPosition_ = 0;
    decodeBuffer.pduBuffer_[0] = PDU_BUFFE_LENGTH_FIVE;
    EXPECT_FALSE(smsWapPushContentType->DecodeParameter(decodeBuffer, 1));
    decodeBuffer.curPosition_ = 0;
    decodeBuffer.pduBuffer_[0] = PDU_BUFFE_LENGTH_SIX;
    EXPECT_FALSE(smsWapPushContentType->DecodeParameter(decodeBuffer, 1));
    decodeBuffer.curPosition_ = 0;
    decodeBuffer.pduBuffer_[0] = PDU_BUFFE_LENGTH_SEVEN;
    EXPECT_FALSE(smsWapPushContentType->DecodeParameter(decodeBuffer, 1));
    decodeBuffer.curPosition_ = 0;
    decodeBuffer.pduBuffer_[0] = PDU_BUFFE_LENGTH_EIGHT;
    EXPECT_FALSE(smsWapPushContentType->DecodeParameter(decodeBuffer, 1));
    decodeBuffer.curPosition_ = 0;
    decodeBuffer.pduBuffer_[0] = PDU_BUFFE_LENGTH_NINE;
    EXPECT_FALSE(smsWapPushContentType->DecodeParameter(decodeBuffer, 1));
    decodeBuffer.curPosition_ = 0;
    decodeBuffer.pduBuffer_[0] = PDU_BUFFE_LENGTH_TEN;
    EXPECT_FALSE(smsWapPushContentType->DecodeParameter(decodeBuffer, 1));
    decodeBuffer.curPosition_ = 0;
    decodeBuffer.pduBuffer_[0] = PDU_BUFFE_LENGTH_ELEVEN;
    EXPECT_FALSE(smsWapPushContentType->DecodeParameter(decodeBuffer, 1));
    decodeBuffer.curPosition_ = 0;
    decodeBuffer.pduBuffer_[0] = PDU_BUFFE_LENGTH_TWELVE;
    EXPECT_FALSE(smsWapPushContentType->DecodeParameter(decodeBuffer, 1));
}

/**
 * @tc.number   Telephony_SmsMmsGtest_SmsWapPushContentType_0002
 * @tc.name     Test SmsWapPushContentType
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, SmsWapPushContentType_0002, Function | MediumTest | Level1)
{
    auto smsWapPushContentType = std::make_shared<SmsWapPushContentType>();
    SmsWapPushBuffer decodeBuffer;
    int32_t valueLength = 1;
    string strCharSet = "US-ASCII";
    string result = "*/*";
    uint32_t charSet = 1;
    decodeBuffer.totolLength_ = 1;
    decodeBuffer.pduBuffer_ = std::make_unique<char[]>(1);
    decodeBuffer.pduBuffer_[0] = PDU_BUFFE_LENGTH_THIRTEEN;
    EXPECT_FALSE(smsWapPushContentType->DecodeParameter(decodeBuffer, 1));
    decodeBuffer.curPosition_ = 0;
    decodeBuffer.pduBuffer_[0] = PDU_BUFFE_LENGTH_FOURTEEN;
    EXPECT_FALSE(smsWapPushContentType->DecodeParameter(decodeBuffer, 1));
    decodeBuffer.curPosition_ = 0;
    decodeBuffer.pduBuffer_[0] = 1;
    EXPECT_FALSE(smsWapPushContentType->DecodeParameter(decodeBuffer, VALUE_LENGTH));
    decodeBuffer.curPosition_ = 0;
    decodeBuffer.pduBuffer_[0] = PDU_BUFFE_LENGTH;
    EXPECT_FALSE(smsWapPushContentType->DecodeCharsetField(decodeBuffer, valueLength));
    decodeBuffer.curPosition_ = 0;
    decodeBuffer.pduBuffer_[0] = 0;
    EXPECT_FALSE(smsWapPushContentType->DecodeCharsetField(decodeBuffer, valueLength));
    decodeBuffer.curPosition_ = 0;
    decodeBuffer.pduBuffer_[0] = PDU_BUFFE_LENGTH_TWO;
    EXPECT_FALSE(smsWapPushContentType->DecodeCharsetField(decodeBuffer, valueLength));
    decodeBuffer.curPosition_ = 0;
    decodeBuffer.pduBuffer_[0] = PDU_BUFFE_LENGTH_TWO;
    EXPECT_TRUE(smsWapPushContentType->DecodeTypeField(decodeBuffer, valueLength));
    decodeBuffer.curPosition_ = 0;
    decodeBuffer.pduBuffer_[0] = PDU_BUFFE_LENGTH_ONE;
    EXPECT_TRUE(smsWapPushContentType->DecodeTypeField(decodeBuffer, valueLength));
    EXPECT_TRUE(smsWapPushContentType->GetCharSetIntFromString(charSet, strCharSet));
    EXPECT_EQ(smsWapPushContentType->GetContentTypeFromInt(0), result);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_SmsSendManager_0001
 * @tc.name     Test SmsSendManager
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, SmsSendManager_0001, Function | MediumTest | Level1)
{
    auto smsSendManager = std::make_shared<SmsSendManager>(INVALID_SLOTID);
    std::shared_ptr<AppExecFwk::EventRunner> runner = AppExecFwk::EventRunner::Create("test");
    auto smsNetworkPolicyManager = std::make_shared<SmsNetworkPolicyManager>(runner, INVALID_SLOTID);
    std::function<void(std::shared_ptr<SmsSendIndexer>)> fun = nullptr;
    auto cdmaSmsSender = std::make_shared<CdmaSmsSender>(runner, INVALID_SLOTID, fun);
    auto gsmSmsSender = std::make_shared<GsmSmsSender>(runner, INVALID_SLOTID, fun);
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
HWTEST_F(BranchTest, SmsSendManager_0002, Function | MediumTest | Level1)
{
    auto smsSendManager = std::make_shared<SmsSendManager>(INVALID_SLOTID);
    std::shared_ptr<AppExecFwk::EventRunner> runner = AppExecFwk::EventRunner::Create("test");
    std::function<void(std::shared_ptr<SmsSendIndexer>)> fun = nullptr;
    std::string desAddr = "";
    std::string scAddr = "123";
    std::string text = "";
    bool isSupported = true;
    std::vector<std::u16string> splitMessage;
    LengthInfo lenInfo;
    const sptr<ISendShortMessageCallback> sendCallback =
        iface_cast<ISendShortMessageCallback>(new SendShortMessageCallbackStub());
    const sptr<IDeliveryShortMessageCallback> deliveryCallback =
        iface_cast<IDeliveryShortMessageCallback>(new DeliveryShortMessageCallbackStub());
    auto smsIndexer = std::make_shared<SmsSendIndexer>(desAddr, scAddr, text, sendCallback, deliveryCallback);
    EXPECT_GT(smsSendManager->SplitMessage(scAddr, splitMessage), TELEPHONY_ERR_SUCCESS);
    EXPECT_GT(smsSendManager->GetSmsSegmentsInfo(scAddr, true, lenInfo), TELEPHONY_ERR_SUCCESS);
    EXPECT_GT(smsSendManager->IsImsSmsSupported(INVALID_SLOTID, isSupported), TELEPHONY_ERR_SUCCESS);
    EXPECT_FALSE(smsSendManager->SetImsSmsConfig(INVALID_SLOTID, 1));
    smsSendManager->networkManager_ = std::make_shared<SmsNetworkPolicyManager>(runner, INVALID_SLOTID);
    EXPECT_FALSE(smsSendManager->SetImsSmsConfig(INVALID_SLOTID, 1));
    EXPECT_GT(smsSendManager->IsImsSmsSupported(INVALID_SLOTID, isSupported), TELEPHONY_ERR_SUCCESS);
    smsSendManager->gsmSmsSender_ = std::make_shared<CdmaSmsSender>(runner, INVALID_SLOTID, fun);
    EXPECT_FALSE(smsSendManager->SetImsSmsConfig(INVALID_SLOTID, 1));
    EXPECT_GT(smsSendManager->IsImsSmsSupported(INVALID_SLOTID, isSupported), TELEPHONY_ERR_SUCCESS);
    smsSendManager->cdmaSmsSender_ = std::make_shared<GsmSmsSender>(runner, INVALID_SLOTID, fun);
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
 * @tc.number   Telephony_SmsMmsGtest_CdmaSmsPduCodec_0001
 * @tc.name     Test CdmaSmsPduCodec
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, CdmaSmsPduCodec_0001, Function | MediumTest | Level1)
{
    auto cdmaSmsPduCodec = std::make_shared<CdmaSmsPduCodec>();
    std::vector<unsigned char> pduStr;
    SmsTransMsg transMsg;
    SmsTeleSvcMsg svcMsg;
    SmsTransBroadCastMsg cbMsg;
    unsigned char *pMsgText = (unsigned char *)TEXT_SMS_CONTENT.c_str();
    pduStr.push_back(SmsTransMsgType::SMS_TRANS_P2P_MSG);
    pduStr.push_back(SmsTransMsgType::SMS_TRANS_BROADCAST_MSG);
    EXPECT_FALSE(cdmaSmsPduCodec->CheckInvalidPDU(pduStr));
    pduStr.clear();
    pduStr.push_back(SmsTransMsgType::SMS_TRANS_TYPE_RESERVED);
    pduStr.push_back(SmsTransMsgType::SMS_TRANS_BROADCAST_MSG);
    pduStr.push_back(SmsTransMsgType::SMS_TRANS_P2P_MSG);
    EXPECT_FALSE(cdmaSmsPduCodec->CheckInvalidPDU(pduStr));
    transMsg.type = SmsTransMsgType::SMS_TRANS_P2P_MSG;
    EXPECT_EQ(cdmaSmsPduCodec->EncodeMsg(transMsg, pMsgText, 1), 0);
    transMsg.type = SmsTransMsgType::SMS_TRANS_BROADCAST_MSG;
    EXPECT_GE(cdmaSmsPduCodec->EncodeMsg(transMsg, pMsgText, 1), 0);
    transMsg.type = SmsTransMsgType::SMS_TRANS_ACK_MSG;
    EXPECT_EQ(cdmaSmsPduCodec->EncodeMsg(transMsg, pMsgText, 1), 0);
    transMsg.type = SmsTransMsgType::SMS_TRANS_TYPE_RESERVED;
    EXPECT_EQ(cdmaSmsPduCodec->EncodeMsg(transMsg, pMsgText, 1), 0);
    EXPECT_EQ(cdmaSmsPduCodec->DecodeMsg(nullptr, 1, transMsg), 0);
    EXPECT_EQ(cdmaSmsPduCodec->DecodeMsg(pMsgText, 0, transMsg), 0);
    EXPECT_EQ(cdmaSmsPduCodec->EncodeCBMsg(cbMsg, nullptr, 1), 0);
    svcMsg.type = SmsMessageType::SMS_TYPE_CANCEL;
    EXPECT_GT(cdmaSmsPduCodec->EncodeTelesvcMsg(svcMsg, pMsgText, 1), 0);
    svcMsg.type = SmsMessageType::SMS_TYPE_DELIVER_REPORT;
    EXPECT_GT(cdmaSmsPduCodec->EncodeTelesvcMsg(svcMsg, pMsgText, 1), 0);
    svcMsg.type = SmsMessageType::SMS_TYPE_DELIVER;
    EXPECT_EQ(cdmaSmsPduCodec->EncodeTelesvcMsg(svcMsg, pMsgText, 1), 0);
    svcMsg.type = SmsMessageType::SMS_TYPE_USER_ACK;
    EXPECT_EQ(cdmaSmsPduCodec->EncodeTelesvcMsg(svcMsg, pMsgText, 1), 0);
    svcMsg.type = SmsMessageType::SMS_TYPE_READ_ACK;
    EXPECT_EQ(cdmaSmsPduCodec->EncodeTelesvcMsg(svcMsg, pMsgText, 1), 0);
    EXPECT_EQ(cdmaSmsPduCodec->DecodeCBMsg(nullptr, 1, cbMsg), 0);
    pMsgText[0] = SmsTransParamId::SMS_TRANS_PARAM_SERVICE_CATEGORY;
    EXPECT_GT(cdmaSmsPduCodec->DecodeCBMsg(pMsgText, 1, cbMsg), 0);
    pMsgText[0] = SmsTransParamId::SMS_TRANS_PARAM_BEARER_DATA;
    EXPECT_GT(cdmaSmsPduCodec->DecodeCBMsg(pMsgText, 1, cbMsg), 0);
    cbMsg.transSvcCtg = SMS_TRANS_SVC_CTG_CMAS_EXTREME;
    EXPECT_GT(cdmaSmsPduCodec->DecodeCBMsg(pMsgText, 1, cbMsg), 0);
    pMsgText[0] = SmsTransParamId::SMS_TRANS_PARAM_TELESVC_IDENTIFIER;
    EXPECT_GE(cdmaSmsPduCodec->DecodeCBMsg(pMsgText, 1, cbMsg), 0);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_CdmaSmsPduCodec_0002
 * @tc.name     Test CdmaSmsPduCodec
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, CdmaSmsPduCodec_0002, Function | MediumTest | Level1)
{
    auto cdmaSmsPduCodec = std::make_shared<CdmaSmsPduCodec>();
    unsigned char *pMsgText = (unsigned char *)TEXT_SMS_CONTENT.c_str();
    SmsTeleSvcMsg svcMsg;
    SmsTeleSvcDeliverReport dRMsg;
    SmsTransP2PMsg p2pMsg;
    SmsTransAddr address;
    svcMsg.type = SmsMessageType::SMS_TYPE_MAX_VALUE;
    EXPECT_EQ(cdmaSmsPduCodec->EncodeTelesvcMsg(svcMsg, pMsgText, 1), 0);
    SmsTeleSvcSubmit sbMsg;
    EXPECT_EQ(cdmaSmsPduCodec->EncodeTelesvcSubmitMsg(sbMsg, nullptr, 1), 0);
    sbMsg.deferValPeriod.format = SmsTimeFormat::SMS_TIME_ABSOLUTE;
    EXPECT_GT(cdmaSmsPduCodec->EncodeTelesvcSubmitMsg(sbMsg, pMsgText, 1), 0);
    sbMsg.deferValPeriod.format = SmsTimeFormat::SMS_TIME_RELATIVE;
    sbMsg.priority = SmsPriorityIndicator::SMS_PRIORITY_URGENT;
    sbMsg.callbackNumber.addrLen = 1;
    sbMsg.replyOpt.userAckReq = true;
    sbMsg.replyOpt.deliverAckReq = true;
    sbMsg.replyOpt.readAckReq = true;
    sbMsg.replyOpt.reportReq = true;
    sbMsg.callbackNumber.digitMode = true;
    EXPECT_GT(cdmaSmsPduCodec->EncodeTelesvcSubmitMsg(sbMsg, pMsgText, 1), 0);
    EXPECT_EQ(cdmaSmsPduCodec->EncodeTelesvcDeliverReportMsg(dRMsg, nullptr, 1), 0);
    dRMsg.tpFailCause = TP_FAIL_CAUSE;
    EXPECT_GT(cdmaSmsPduCodec->EncodeTelesvcDeliverReportMsg(dRMsg, pMsgText, 1), 0);
    EXPECT_EQ(cdmaSmsPduCodec->EncodeAddress(address, nullptr, 1), 0);
    EXPECT_GT(cdmaSmsPduCodec->EncodeAddress(address, pMsgText, 1), 0);
    address.digitMode = true;
    address.numberMode = true;
    EXPECT_GT(cdmaSmsPduCodec->EncodeAddress(address, pMsgText, 1), 0);
    EXPECT_EQ(cdmaSmsPduCodec->DecodeP2PMsg(nullptr, 1, p2pMsg), 0);
    pMsgText[0] = SmsTransParamId::SMS_TRANS_PARAM_TELESVC_IDENTIFIER;
    EXPECT_GT(cdmaSmsPduCodec->DecodeP2PMsg(pMsgText, 1, p2pMsg), 0);
    pMsgText[0] = SmsTransParamId::SMS_TRANS_PARAM_SERVICE_CATEGORY;
    EXPECT_GT(cdmaSmsPduCodec->DecodeP2PMsg(pMsgText, 1, p2pMsg), 0);
    pMsgText[0] = SmsTransParamId::SMS_TRANS_PARAM_ORG_ADDRESS;
    EXPECT_GT(cdmaSmsPduCodec->DecodeP2PMsg(pMsgText, 1, p2pMsg), 0);
    pMsgText[0] = SmsTransParamId::SMS_TRANS_PARAM_DEST_ADDRESS;
    EXPECT_GT(cdmaSmsPduCodec->DecodeP2PMsg(pMsgText, 1, p2pMsg), 0);
    pMsgText[0] = SmsTransParamId::SMS_TRANS_PARAM_ORG_SUB_ADDRESS;
    EXPECT_GT(cdmaSmsPduCodec->DecodeP2PMsg(pMsgText, 1, p2pMsg), 0);
    pMsgText[0] = SmsTransParamId::SMS_TRANS_PARAM_DEST_SUB_ADDRESS;
    EXPECT_GT(cdmaSmsPduCodec->DecodeP2PMsg(pMsgText, 1, p2pMsg), 0);
    pMsgText[0] = SmsTransParamId::SMS_TRANS_PARAM_BEARER_REPLY_OPTION;
    EXPECT_GT(cdmaSmsPduCodec->DecodeP2PMsg(pMsgText, 1, p2pMsg), 0);
    pMsgText[0] = SmsTransParamId::SMS_TRANS_PARAM_BEARER_DATA;
    EXPECT_GT(cdmaSmsPduCodec->DecodeP2PMsg(pMsgText, 1, p2pMsg), 0);
    pMsgText[0] = SmsTransParamId::SMS_TRANS_PARAM_RESERVED;
    EXPECT_GE(cdmaSmsPduCodec->DecodeP2PMsg(pMsgText, 1, p2pMsg), 0);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_CdmaSmsPduCodec_0003
 * @tc.name     Test CdmaSmsPduCodec
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, CdmaSmsPduCodec_0003, Function | MediumTest | Level1)
{
    auto cdmaSmsPduCodec = std::make_shared<CdmaSmsPduCodec>();
    unsigned char *pMsgText = (unsigned char *)TEXT_SMS_CONTENT.c_str();
    SmsTransAckMsg ackMsg;
    SmsTeleSvcMsg svcMsg;
    SmsTeleSvcDeliverAck delAckMsg;
    SmsTeleSvcDeliverReport subReport;
    SmsTeleSvcUserAck userAck;
    cdmaSmsPduCodec->DecodeP2PTelesvcMsg(nullptr, 1, svcMsg);
    cdmaSmsPduCodec->DecodeP2PTelesvcMsg(pMsgText, 1, svcMsg);
    pMsgText[0] = SmsBearerSubParam::SMS_BEARER_MESSAGE_IDENTIFIER;
    cdmaSmsPduCodec->DecodeP2PUserAckMsg(pMsgText, 1, userAck);
    cdmaSmsPduCodec->DecodeP2PSubmitReportMsg(pMsgText, 1, subReport);
    cdmaSmsPduCodec->DecodeP2PDeliveryAckMsg(pMsgText, 1, delAckMsg);
    pMsgText[0] = SmsBearerSubParam::SMS_BEARER_USER_DATA;
    pMsgText[1] = SmsBearerSubParam::SMS_BEARER_ALERT_ON_MSG_DELIVERY;
    cdmaSmsPduCodec->DecodeP2PUserAckMsg(pMsgText, 1, userAck);
    cdmaSmsPduCodec->DecodeP2PSubmitReportMsg(pMsgText, 1, subReport);
    cdmaSmsPduCodec->DecodeP2PDeliveryAckMsg(pMsgText, 1, delAckMsg);
    pMsgText[0] = SmsBearerSubParam::SMS_BEARER_MSG_CENTER_TIME_STAMP;
    cdmaSmsPduCodec->DecodeP2PUserAckMsg(pMsgText, 1, userAck);
    cdmaSmsPduCodec->DecodeP2PDeliveryAckMsg(pMsgText, 1, delAckMsg);
    pMsgText[0] = SmsBearerSubParam::SMS_BEARER_LANGUAGE_INDICATOR;
    cdmaSmsPduCodec->DecodeP2PSubmitReportMsg(pMsgText, 1, subReport);
    pMsgText[0] = SmsBearerSubParam::SMS_BEARER_MULTI_ENCODING_USER_DATA;
    cdmaSmsPduCodec->DecodeP2PUserAckMsg(pMsgText, 1, userAck);
    cdmaSmsPduCodec->DecodeP2PSubmitReportMsg(pMsgText, 1, subReport);
    cdmaSmsPduCodec->DecodeP2PDeliveryAckMsg(pMsgText, 1, delAckMsg);
    pMsgText[0] = SmsBearerSubParam::SMS_BEARER_TP_FAILURE_CAUSE;
    cdmaSmsPduCodec->DecodeP2PSubmitReportMsg(pMsgText, 1, subReport);
    pMsgText[0] = SmsBearerSubParam::SMS_BEARER_MESSAGE_STATUS;
    cdmaSmsPduCodec->DecodeP2PDeliveryAckMsg(pMsgText, 1, delAckMsg);
    pMsgText[0] = SmsBearerSubParam::SMS_BEARER_MSG_DEPOSIT_INDEX;
    cdmaSmsPduCodec->DecodeP2PUserAckMsg(pMsgText, 1, userAck);
    pMsgText[0] = SmsBearerSubParam::SMS_BEARER_MAX_VALUE;
    cdmaSmsPduCodec->DecodeP2PUserAckMsg(pMsgText, 1, userAck);
    cdmaSmsPduCodec->DecodeP2PDeliveryAckMsg(pMsgText, 1, delAckMsg);
    cdmaSmsPduCodec->DecodeP2PSubmitReportMsg(pMsgText, 1, subReport);
    cdmaSmsPduCodec->DecodeP2PUserAckMsg(pMsgText, 1, userAck);
    EXPECT_EQ(cdmaSmsPduCodec->DecodeAckMsg(nullptr, 1, ackMsg), 0);
    pMsgText[0] = SmsTransParamId::SMS_TRANS_PARAM_DEST_ADDRESS;
    EXPECT_GT(cdmaSmsPduCodec->DecodeAckMsg(pMsgText, 1, ackMsg), 0);
    pMsgText[0] = SmsTransParamId::SMS_TRANS_PARAM_DEST_SUB_ADDRESS;
    EXPECT_GT(cdmaSmsPduCodec->DecodeAckMsg(pMsgText, 1, ackMsg), 0);
    pMsgText[0] = SmsTransParamId::SMS_TRANS_PARAM_CAUSE_CODES;
    EXPECT_GT(cdmaSmsPduCodec->DecodeAckMsg(pMsgText, 1, ackMsg), 0);
    pMsgText[0] = SmsTransParamId::SMS_TRANS_PARAM_ORG_SUB_ADDRESS;
    EXPECT_GE(cdmaSmsPduCodec->DecodeAckMsg(pMsgText, 1, ackMsg), 0);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_CdmaSmsPduCodec_0004
 * @tc.name     Test CdmaSmsPduCodec
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, CdmaSmsPduCodec_0004, Function | MediumTest | Level1)
{
    auto cdmaSmsPduCodec = std::make_shared<CdmaSmsPduCodec>();
    unsigned char *pMsgText = (unsigned char *)TEXT_SMS_CONTENT.c_str();
    SmsTeleSvcReadAck readAck;
    SmsTeleSvcDeliver delMsg;
    pMsgText[0] = SmsBearerSubParam::SMS_BEARER_MESSAGE_IDENTIFIER;
    cdmaSmsPduCodec->DecodeP2PDeliverMsg(pMsgText, 1, delMsg);
    cdmaSmsPduCodec->DecodeP2PReadAckMsg(pMsgText, 1, readAck);
    pMsgText[0] = SmsBearerSubParam::SMS_BEARER_USER_DATA;
    pMsgText[1] = SmsBearerSubParam::SMS_BEARER_ALERT_ON_MSG_DELIVERY;
    cdmaSmsPduCodec->DecodeP2PDeliverMsg(pMsgText, 1, delMsg);
    cdmaSmsPduCodec->DecodeP2PReadAckMsg(pMsgText, 1, readAck);
    pMsgText[0] = SmsBearerSubParam::SMS_BEARER_MSG_CENTER_TIME_STAMP;
    cdmaSmsPduCodec->DecodeP2PDeliverMsg(pMsgText, 1, delMsg);
    cdmaSmsPduCodec->DecodeP2PReadAckMsg(pMsgText, 1, readAck);
    pMsgText[0] = SmsBearerSubParam::SMS_BEARER_MULTI_ENCODING_USER_DATA;
    cdmaSmsPduCodec->DecodeP2PDeliverMsg(pMsgText, 1, delMsg);
    cdmaSmsPduCodec->DecodeP2PReadAckMsg(pMsgText, 1, readAck);
    pMsgText[0] = SmsBearerSubParam::SMS_BEARER_MSG_DEPOSIT_INDEX;
    cdmaSmsPduCodec->DecodeP2PDeliverMsg(pMsgText, 1, delMsg);
    cdmaSmsPduCodec->DecodeP2PReadAckMsg(pMsgText, 1, readAck);
    pMsgText[0] = SmsBearerSubParam::SMS_BEARER_MAX_VALUE;
    cdmaSmsPduCodec->DecodeP2PDeliverMsg(pMsgText, 1, delMsg);
    cdmaSmsPduCodec->DecodeP2PReadAckMsg(pMsgText, 1, readAck);
    cdmaSmsPduCodec->DecodeP2PDeliverMsg(nullptr, 1, delMsg);
    pMsgText[0] = SmsBearerSubParam::SMS_BEARER_VALIDITY_PERIOD_ABSOLUTE;
    cdmaSmsPduCodec->DecodeP2PDeliverMsg(pMsgText, 1, delMsg);
    pMsgText[0] = SmsBearerSubParam::SMS_BEARER_VALIDITY_PERIOD_RELATIVE;
    cdmaSmsPduCodec->DecodeP2PDeliverMsg(pMsgText, 1, delMsg);
    pMsgText[0] = SmsBearerSubParam::SMS_BEARER_PRIORITY_INDICATOR;
    cdmaSmsPduCodec->DecodeP2PDeliverMsg(pMsgText, 1, delMsg);
    pMsgText[0] = SmsBearerSubParam::SMS_BEARER_PRIVACY_INDICATOR;
    cdmaSmsPduCodec->DecodeP2PDeliverMsg(pMsgText, 1, delMsg);
    pMsgText[0] = SmsBearerSubParam::SMS_BEARER_REPLY_OPTION;
    cdmaSmsPduCodec->DecodeP2PDeliverMsg(pMsgText, 1, delMsg);
    pMsgText[0] = SmsBearerSubParam::SMS_BEARER_NUMBER_OF_MESSAGES;
    cdmaSmsPduCodec->DecodeP2PDeliverMsg(pMsgText, 1, delMsg);
    pMsgText[0] = SmsBearerSubParam::SMS_BEARER_ALERT_ON_MSG_DELIVERY;
    cdmaSmsPduCodec->DecodeP2PDeliverMsg(pMsgText, 1, delMsg);
    pMsgText[0] = SmsBearerSubParam::SMS_BEARER_CALLBACK_NUMBER;
    cdmaSmsPduCodec->DecodeP2PDeliverMsg(pMsgText, 1, delMsg);
    pMsgText[0] = SmsBearerSubParam::SMS_BEARER_MSG_DISPLAY_MODE;
    cdmaSmsPduCodec->DecodeP2PDeliverMsg(pMsgText, 1, delMsg);
    pMsgText[0] = SmsBearerSubParam::SMS_BEARER_LANGUAGE_INDICATOR;
    cdmaSmsPduCodec->DecodeP2PDeliverMsg(pMsgText, 1, delMsg);
    SmsTeleSvcAddr cbNumber;
    std::vector<unsigned char> pdustr;
    cbNumber.digitMode = true;
    EXPECT_GT(cdmaSmsPduCodec->EncodeCbNumber(cbNumber, pdustr), 0);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_CdmaSmsPduCodec_0005
 * @tc.name     Test CdmaSmsPduCodec
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, CdmaSmsPduCodec_0005, Function | MediumTest | Level1)
{
    auto cdmaSmsPduCodec = std::make_shared<CdmaSmsPduCodec>();
    unsigned char *pMsgText = (unsigned char *)TEXT_SMS_CONTENT.c_str();
    SmsTeleSvcMsg telesvc;
    SmsTeleSvcSubmit subMsg;
    pMsgText[0] = SmsBearerSubParam::SMS_BEARER_MESSAGE_IDENTIFIER;
    cdmaSmsPduCodec->DecodeCBBearerData(pMsgText, 1, telesvc, true);
    cdmaSmsPduCodec->DecodeP2PSubmitMsg(pMsgText, 1, subMsg);
    pMsgText[0] = SmsBearerSubParam::SMS_BEARER_USER_DATA;
    pMsgText[1] = SmsBearerSubParam::SMS_BEARER_ALERT_ON_MSG_DELIVERY;
    cdmaSmsPduCodec->DecodeCBBearerData(pMsgText, 1, telesvc, true);
    cdmaSmsPduCodec->DecodeP2PSubmitMsg(pMsgText, 1, subMsg);
    pMsgText[0] = SmsBearerSubParam::SMS_BEARER_MSG_CENTER_TIME_STAMP;
    cdmaSmsPduCodec->DecodeCBBearerData(pMsgText, 1, telesvc, true);
    pMsgText[0] = SmsBearerSubParam::SMS_BEARER_DEFERRED_DELIVERY_TIME_ABSOLUTE;
    cdmaSmsPduCodec->DecodeP2PSubmitMsg(pMsgText, 1, subMsg);
    pMsgText[0] = SmsBearerSubParam::SMS_BEARER_MULTI_ENCODING_USER_DATA;
    cdmaSmsPduCodec->DecodeCBBearerData(pMsgText, 1, telesvc, true);
    cdmaSmsPduCodec->DecodeP2PSubmitMsg(pMsgText, 1, subMsg);
    pMsgText[0] = SmsBearerSubParam::SMS_BEARER_MSG_DEPOSIT_INDEX;
    cdmaSmsPduCodec->DecodeP2PSubmitMsg(pMsgText, 1, subMsg);
    pMsgText[0] = SmsBearerSubParam::SMS_BEARER_MAX_VALUE;
    cdmaSmsPduCodec->DecodeCBBearerData(pMsgText, 1, telesvc, true);
    cdmaSmsPduCodec->DecodeP2PSubmitMsg(pMsgText, 1, subMsg);
    pMsgText[0] = SmsBearerSubParam::SMS_BEARER_VALIDITY_PERIOD_ABSOLUTE;
    cdmaSmsPduCodec->DecodeCBBearerData(pMsgText, 1, telesvc, true);
    cdmaSmsPduCodec->DecodeP2PSubmitMsg(pMsgText, 1, subMsg);
    pMsgText[0] = SmsBearerSubParam::SMS_BEARER_VALIDITY_PERIOD_RELATIVE;
    cdmaSmsPduCodec->DecodeCBBearerData(pMsgText, 1, telesvc, true);
    cdmaSmsPduCodec->DecodeP2PSubmitMsg(pMsgText, 1, subMsg);
    pMsgText[0] = SmsBearerSubParam::SMS_BEARER_PRIORITY_INDICATOR;
    cdmaSmsPduCodec->DecodeCBBearerData(pMsgText, 1, telesvc, true);
    cdmaSmsPduCodec->DecodeP2PSubmitMsg(pMsgText, 1, subMsg);
    pMsgText[0] = SmsBearerSubParam::SMS_BEARER_PRIVACY_INDICATOR;
    cdmaSmsPduCodec->DecodeP2PSubmitMsg(pMsgText, 1, subMsg);
    pMsgText[0] = SmsBearerSubParam::SMS_BEARER_REPLY_OPTION;
    cdmaSmsPduCodec->DecodeP2PSubmitMsg(pMsgText, 1, subMsg);
    pMsgText[0] = SmsBearerSubParam::SMS_BEARER_DEFERRED_DELIVERY_TIME_RELATIVE;
    cdmaSmsPduCodec->DecodeP2PSubmitMsg(pMsgText, 1, subMsg);
    pMsgText[0] = SmsBearerSubParam::SMS_BEARER_ALERT_ON_MSG_DELIVERY;
    cdmaSmsPduCodec->DecodeCBBearerData(pMsgText, 1, telesvc, true);
    cdmaSmsPduCodec->DecodeP2PSubmitMsg(pMsgText, 1, subMsg);
    pMsgText[0] = SmsBearerSubParam::SMS_BEARER_CALLBACK_NUMBER;
    cdmaSmsPduCodec->DecodeCBBearerData(pMsgText, 1, telesvc, true);
    cdmaSmsPduCodec->DecodeP2PSubmitMsg(pMsgText, 1, subMsg);
    SmsTransMsgId smgId;
    EXPECT_EQ(cdmaSmsPduCodec->DecodeMsgId(pMsgText, 1, smgId), 0);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_CdmaSmsPduCodec_0006
 * @tc.name     Test CdmaSmsPduCodec
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, CdmaSmsPduCodec_0006, Function | MediumTest | Level1)
{
    auto cdmaSmsPduCodec = std::make_shared<CdmaSmsPduCodec>();
    unsigned char *dest = (unsigned char *)TEXT_SMS_CONTENT.c_str();
    int remainBits = 0;
    SmsTeleSvcUserData svcUserData;
    SmsTeleSvcCmasData cmasData;
    SmsUserData userData;
    SmsTeleSvcAddr svcAddr;
    SmsEnhancedVmnAck enhancedVmnAck;
    SmsUDH smsUDH;
    smsUDH.udhType = SmsUDHType::SMS_UDH_APP_PORT_8BIT;
    userData.headerCnt = 1;
    userData.length = 1;
    userData.header[0] = smsUDH;
    cdmaSmsPduCodec->DecodeUserData(nullptr, 1, svcUserData, true);
    cdmaSmsPduCodec->DecodeUserData(dest, 1, svcUserData, true);
    dest[1] = SmsUDHType::SMS_UDH_CONCAT_8BIT;
    cdmaSmsPduCodec->Decode7BitHeader(dest, 1, 1, userData);
    dest[0] = SmsUDHType::SMS_UDH_CONCAT_16BIT;
    dest[1] = SmsUDHType::SMS_UDH_CONCAT_16BIT;
    cdmaSmsPduCodec->Decode7BitHeader(dest, 1, 1, userData);
    cdmaSmsPduCodec->Decode7BitHeader(dest, 1, 0, userData);
    dest[VALUE_LENGTH] = SmsUDHType::SMS_UDH_CONCAT_16BIT;
    cdmaSmsPduCodec->DecodeCMASData(dest, 1, cmasData);
    dest[VALUE_LENGTH] = 0;
    cdmaSmsPduCodec->DecodeCMASData(dest, START_BIT, cmasData);
    dest[0] = PDU_BUFFE_LENGTH_ONE;
    cdmaSmsPduCodec->DecodeCallBackNum(dest, 1, svcAddr);
    svcAddr.numberType = SmsNumberType::SMS_NUMBER_TYPE_INTERNATIONAL;
    cdmaSmsPduCodec->DecodeCallBackNum(dest, 1, svcAddr);
    cdmaSmsPduCodec->DecodeP2PDeliverVmnAck(nullptr, 1, enhancedVmnAck);
    dest[VALUE_LENGTH] = SmsBearerSubParam::SMS_BEARER_LANGUAGE_INDICATOR;
    cdmaSmsPduCodec->DecodeP2PDeliverVmnAck(dest, 0, enhancedVmnAck);
    SmsTeleSvcMsg telesvc;
    SmsTeleSvcSubmit subMsg;
    dest[0] = SmsBearerSubParam::SMS_BEARER_LANGUAGE_INDICATOR;
    cdmaSmsPduCodec->DecodeCBBearerData(dest, 1, telesvc, true);
    cdmaSmsPduCodec->DecodeP2PSubmitMsg(dest, 1, subMsg);
    dest[0] = SmsBearerSubParam::SMS_BEARER_MSG_DISPLAY_MODE;
    cdmaSmsPduCodec->DecodeCBBearerData(dest, 1, telesvc, true);
    EXPECT_GT(cdmaSmsPduCodec->Encode7BitASCIIData(userData, dest, remainBits), 0);
    EXPECT_GT(cdmaSmsPduCodec->Encode7BitGSMData(userData, dest, remainBits), 0);
    userData.length = BUF_SIZE;
    EXPECT_EQ(cdmaSmsPduCodec->EncodeUCS2Data(userData, dest, remainBits), 0);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_CdmaSmsPduCodec_0007
 * @tc.name     Test CdmaSmsPduCodec
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, CdmaSmsPduCodec_0007, Function | MediumTest | Level1)
{
    auto cdmaSmsPduCodec = std::make_shared<CdmaSmsPduCodec>();
    unsigned char *dest = (unsigned char *)TEXT_SMS_CONTENT.c_str();
    SmsTeleSvcUserData userData;
    cdmaSmsPduCodec->ShiftNBitForDecode(nullptr, 1, 1);
    cdmaSmsPduCodec->ShiftNBitForDecode(nullptr, 1, SHIFT_BIT);
    cdmaSmsPduCodec->ShiftRNBit(nullptr, 1, 1);
    cdmaSmsPduCodec->ShiftRNBit(nullptr, 1, SHIFT_BIT);
    cdmaSmsPduCodec->ShiftNBit(nullptr, 1, 1);
    cdmaSmsPduCodec->ShiftNBit(nullptr, 1, SHIFT_BIT);
    EXPECT_EQ(cdmaSmsPduCodec->DecodeDigitModeNumberPlan(SmsNumberPlanType::SMS_NPI_UNKNOWN),
        SmsNumberPlanType::SMS_NPI_UNKNOWN);
    EXPECT_EQ(
        cdmaSmsPduCodec->DecodeDigitModeNumberPlan(SmsNumberPlanType::SMS_NPI_ISDN), SmsNumberPlanType::SMS_NPI_ISDN);
    EXPECT_EQ(
        cdmaSmsPduCodec->DecodeDigitModeNumberPlan(SmsNumberPlanType::SMS_NPI_DATA), SmsNumberPlanType::SMS_NPI_DATA);
    EXPECT_EQ(
        cdmaSmsPduCodec->DecodeDigitModeNumberPlan(SmsNumberPlanType::SMS_NPI_TELEX), SmsNumberPlanType::SMS_NPI_TELEX);
    EXPECT_EQ(cdmaSmsPduCodec->DecodeDigitModeNumberPlan(SmsNumberPlanType::SMS_NPI_PRIVATE),
        SmsNumberPlanType::SMS_NPI_PRIVATE);
    EXPECT_EQ(cdmaSmsPduCodec->DecodeDigitModeNumberPlan(SmsNumberPlanType::SMS_NPI_RESERVED),
        SmsNumberPlanType::SMS_NPI_RESERVED);
    EXPECT_EQ(cdmaSmsPduCodec->EncodeBearerUserData(userData, nullptr, 1), 0);
    userData.userData.length = 0;
    EXPECT_EQ(cdmaSmsPduCodec->EncodeBearerUserData(userData, dest, 1), 0);
    userData.userData.length = 1;
    userData.encodeType = SmsEncodingType::SMS_ENCODE_EPM;
    EXPECT_GT(cdmaSmsPduCodec->EncodeBearerUserData(userData, dest, 1), 0);
    userData.encodeType = SmsEncodingType::SMS_ENCODE_GSMDCS;
    EXPECT_GT(cdmaSmsPduCodec->EncodeBearerUserData(userData, dest, 1), 0);
    userData.encodeType = SmsEncodingType::SMS_ENCODE_7BIT_ASCII;
    EXPECT_GT(cdmaSmsPduCodec->EncodeBearerUserData(userData, dest, 1), 0);
    userData.encodeType = SmsEncodingType::SMS_ENCODE_GSM7BIT;
    EXPECT_GT(cdmaSmsPduCodec->EncodeBearerUserData(userData, dest, 1), 0);
    userData.encodeType = SmsEncodingType::SMS_ENCODE_UNICODE;
    EXPECT_GT(cdmaSmsPduCodec->EncodeBearerUserData(userData, dest, 1), 0);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_MmsBodyPart_0001
 * @tc.name     Test MmsBodyPart
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, MmsBodyPart_0001, Function | MediumTest | Level3)
{
    MmsBodyPart mmsBodyPart;
    MmsBodyPart testBodyPart;
    mmsBodyPart = testBodyPart;
    mmsBodyPart.AssignBodyPart(testBodyPart);
    MmsDecodeBuffer decoderBuffer;
    decoderBuffer.curPosition_ = 0;
    decoderBuffer.totolLength_ = 0;
    ASSERT_FALSE(mmsBodyPart.DecodePart(decoderBuffer));
    decoderBuffer.pduBuffer_[0] = 0;
    decoderBuffer.curPosition_ = 0;
    decoderBuffer.totolLength_ = 1;
    ASSERT_FALSE(mmsBodyPart.DecodePart(decoderBuffer));
    decoderBuffer.pduBuffer_[0] = 0;
    decoderBuffer.pduBuffer_[1] = 0;
    decoderBuffer.curPosition_ = 0;
    decoderBuffer.totolLength_ = 2;
    ASSERT_FALSE(mmsBodyPart.DecodePart(decoderBuffer));
    decoderBuffer.pduBuffer_[0] = 0;
    decoderBuffer.pduBuffer_[1] = 0;
    decoderBuffer.pduBuffer_[2] = 0x70;
    decoderBuffer.curPosition_ = 0;
    decoderBuffer.totolLength_ = 3;
    ASSERT_FALSE(mmsBodyPart.DecodePart(decoderBuffer));
    decoderBuffer.pduBuffer_[0] = 0x7F;
    decoderBuffer.pduBuffer_[1] = 0;
    decoderBuffer.pduBuffer_[2] = 0x70;
    decoderBuffer.curPosition_ = 0;
    decoderBuffer.totolLength_ = 10;
    ASSERT_FALSE(mmsBodyPart.DecodePart(decoderBuffer));
    ASSERT_TRUE(mmsBodyPart.DecodePartHeader(decoderBuffer, 0));
    decoderBuffer.curPosition_ = 0;
    decoderBuffer.totolLength_ = 0;
    int32_t testLen = 3;
    ASSERT_FALSE(mmsBodyPart.DecodePartHeader(decoderBuffer, testLen));
    decoderBuffer.curPosition_ = 0;
    decoderBuffer.totolLength_ = 1;
    decoderBuffer.pduBuffer_[0] = 0x80;
    ASSERT_FALSE(mmsBodyPart.DecodePartHeader(decoderBuffer, testLen));
    decoderBuffer.curPosition_ = 0;
    decoderBuffer.totolLength_ = 1;
    decoderBuffer.pduBuffer_[0] = 0x20;
    ASSERT_FALSE(mmsBodyPart.DecodePartHeader(decoderBuffer, testLen));
    decoderBuffer.curPosition_ = 0;
    decoderBuffer.totolLength_ = 1;
    decoderBuffer.pduBuffer_[0] = 0x5;
    ASSERT_FALSE(mmsBodyPart.DecodePartHeader(decoderBuffer, testLen));
}

/**
 * @tc.number   Telephony_SmsMmsGtest_MmsBodyPart_0002
 * @tc.name     Test MmsBodyPart
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, MmsBodyPart_0002, Function | MediumTest | Level3)
{
    MmsBodyPart mmsBodyPart;

    MmsDecodeBuffer decoderBuffer;
    decoderBuffer.curPosition_ = 0;
    decoderBuffer.totolLength_ = 0;
    ASSERT_FALSE(mmsBodyPart.DecodePartBody(decoderBuffer, 3));
    decoderBuffer.curPosition_ = 0;
    int32_t offset = 2;
    decoderBuffer.totolLength_ = MAX_MMS_MSG_PART_LEN + offset;
    offset = 1;
    ASSERT_FALSE(mmsBodyPart.DecodePartBody(decoderBuffer, MAX_MMS_MSG_PART_LEN + offset));
    MmsAttachment mmsAttachment;
    ASSERT_FALSE(mmsBodyPart.SetAttachment(mmsAttachment));
    MmsEncodeBuffer encodeBuffer;
    ASSERT_FALSE(mmsBodyPart.EncodeMmsBodyPart(encodeBuffer));
    mmsBodyPart.DecodeSetFileName();
    ASSERT_FALSE(mmsBodyPart.WriteBodyFromFile(""));
    ASSERT_FALSE(mmsBodyPart.WriteBodyFromAttachmentBuffer(mmsAttachment));
    mmsAttachment.strFileName_ = "test";
    ASSERT_FALSE(mmsBodyPart.WriteBodyFromAttachmentBuffer(mmsAttachment));
    uint32_t len = 0;
    ASSERT_TRUE(mmsBodyPart.ReadBodyPartBuffer(len) == nullptr);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_MmsContentType_0001
 * @tc.name     Test MmsContentType
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, MmsContentType_0001, Function | MediumTest | Level3)
{
    MmsContentType mmsContentType;
    MmsContentType mmsTContentType;
    mmsTContentType.contentType_ = "lll";
    mmsContentType = mmsTContentType;
    MmsDecodeBuffer decoderBuffer;
    int32_t testPduBuffer = 0x8f;
    decoderBuffer.pduBuffer_[0] = testPduBuffer;
    decoderBuffer.curPosition_ = 0;
    decoderBuffer.totolLength_ = 1;
    int32_t length;
    ASSERT_TRUE(mmsContentType.DecodeMmsContentType(decoderBuffer, length));
    decoderBuffer.curPosition_ = 0;
    decoderBuffer.totolLength_ = 1;
    testPduBuffer = 0x2f;
    decoderBuffer.pduBuffer_[0] = testPduBuffer;
    ASSERT_FALSE(mmsContentType.DecodeMmsCTGeneralForm(decoderBuffer, length));
    decoderBuffer.curPosition_ = 0;
    decoderBuffer.totolLength_ = 0;
    ASSERT_FALSE(mmsContentType.DecodeMmsCTGeneralForm(decoderBuffer, length));
    decoderBuffer.curPosition_ = 0;
    decoderBuffer.totolLength_ = 1;
    testPduBuffer = 0x8f;
    decoderBuffer.pduBuffer_[0] = testPduBuffer;
    ASSERT_FALSE(mmsContentType.DecodeMmsCTGeneralForm(decoderBuffer, length));
    decoderBuffer.curPosition_ = 0;
    decoderBuffer.totolLength_ = 1;
    decoderBuffer.pduBuffer_[0] = 0;
    ASSERT_FALSE(mmsContentType.DecodeMmsCTGeneralForm(decoderBuffer, length));
}

/**
 * @tc.number   Telephony_SmsMmsGtest_MmsContentType_0002
 * @tc.name     Test MmsContentType
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, MmsContentType_0002, Function | MediumTest | Level3)
{
    MmsContentType mmsContentType;
    MmsDecodeBuffer decoderBuffer;
    ASSERT_TRUE(mmsContentType.DecodeParameter(decoderBuffer, 0));
    decoderBuffer.curPosition_ = 0;
    decoderBuffer.totolLength_ = 0;
    ASSERT_FALSE(mmsContentType.DecodeParameter(decoderBuffer, 1));
    int32_t testPduBuffer = 129;
    decoderBuffer.pduBuffer_[0] = testPduBuffer;
    decoderBuffer.curPosition_ = 0;
    decoderBuffer.totolLength_ = 1;
    ASSERT_FALSE(mmsContentType.DecodeParameter(decoderBuffer, 1));
    testPduBuffer = 154;
    decoderBuffer.pduBuffer_[0] = testPduBuffer;
    decoderBuffer.curPosition_ = 0;
    decoderBuffer.totolLength_ = 1;
    ASSERT_FALSE(mmsContentType.DecodeParameter(decoderBuffer, 1));
    testPduBuffer = 137;
    decoderBuffer.pduBuffer_[0] = testPduBuffer;
    decoderBuffer.curPosition_ = 0;
    decoderBuffer.totolLength_ = 1;
    ASSERT_FALSE(mmsContentType.DecodeParameter(decoderBuffer, 1));
    decoderBuffer.pduBuffer_[0] = 0;
    decoderBuffer.curPosition_ = 0;
    decoderBuffer.totolLength_ = 1;
    ASSERT_FALSE(mmsContentType.DecodeParameter(decoderBuffer, 4));
    decoderBuffer.curPosition_ = 0;
    decoderBuffer.totolLength_ = 0;
    int32_t len;
    ASSERT_FALSE(mmsContentType.DecodeTextField(decoderBuffer, 0, len));
    ASSERT_FALSE(mmsContentType.DecodeCharsetField(decoderBuffer, len));
    testPduBuffer = 100;
    decoderBuffer.pduBuffer_[0] = testPduBuffer;
    decoderBuffer.curPosition_ = 0;
    decoderBuffer.totolLength_ = 1;
    ASSERT_FALSE(mmsContentType.DecodeCharsetField(decoderBuffer, len));
    testPduBuffer = 30;
    decoderBuffer.pduBuffer_[0] = testPduBuffer;
    decoderBuffer.curPosition_ = 0;
    decoderBuffer.totolLength_ = 1;
    ASSERT_FALSE(mmsContentType.DecodeCharsetField(decoderBuffer, len));
}

/**
 * @tc.number   Telephony_SmsMmsGtest_MmsContentType_0003
 * @tc.name     Test MmsContentType
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, MmsContentType_0003, Function | MediumTest | Level3)
{
    MmsEncodeBuffer mmsEncodeBuffer;
    MmsContentType mmsContentType;
    MmsDecodeBuffer decoderBuffer;
    decoderBuffer.curPosition_ = 0;
    decoderBuffer.totolLength_ = 0;
    int32_t valueLen = 0;
    ASSERT_FALSE(mmsContentType.DecodeTypeField(decoderBuffer, valueLen));
    decoderBuffer.pduBuffer_[0] = 0x8f;
    decoderBuffer.curPosition_ = 0;
    decoderBuffer.totolLength_ = 1;
    ASSERT_TRUE(mmsContentType.DecodeTypeField(decoderBuffer, valueLen));
    decoderBuffer.pduBuffer_[0] = 0;
    decoderBuffer.curPosition_ = 0;
    decoderBuffer.totolLength_ = 1;
    ASSERT_FALSE(mmsContentType.DecodeTypeField(decoderBuffer, valueLen));
    mmsContentType.msgContentParm_.textMap_[152] = "";
    int32_t offset = 1;
    mmsEncodeBuffer.curPosition_ = CODE_BUFFER_MAX_SIZE + offset;
    mmsContentType.EncodeTextField(mmsEncodeBuffer);
    mmsContentType.msgContentParm_.textMap_[152] = "";
    mmsEncodeBuffer.curPosition_ = 0;
    ASSERT_TRUE(mmsContentType.EncodeTextField(mmsEncodeBuffer));
    ASSERT_TRUE(mmsContentType.EncodeCharsetField(mmsEncodeBuffer));
    mmsContentType.msgContentParm_.charset_ = offset;
    ASSERT_TRUE(mmsContentType.EncodeCharsetField(mmsEncodeBuffer));
    mmsContentType.msgContentParm_.charset_ = offset;
    mmsEncodeBuffer.curPosition_ = CODE_BUFFER_MAX_SIZE + offset;
    mmsContentType.EncodeCharsetField(mmsEncodeBuffer);
    mmsContentType.msgContentParm_.charset_ = offset;
    mmsEncodeBuffer.curPosition_ = CODE_BUFFER_MAX_SIZE - offset;
    mmsContentType.EncodeCharsetField(mmsEncodeBuffer);
    mmsContentType.msgContentParm_.charset_ = offset;
    mmsEncodeBuffer.curPosition_ = 0;
    ASSERT_TRUE(mmsContentType.EncodeCharsetField(mmsEncodeBuffer));
}

/**
 * @tc.number   Telephony_SmsMmsGtest_MmsContentType_0004
 * @tc.name     Test MmsContentType
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, MmsContentType_0004, Function | MediumTest | Level3)
{
    MmsContentType mmsContentType;
    MmsEncodeBuffer mmsEncodeBuffer;
    ASSERT_TRUE(mmsContentType.EncodeTypeField(mmsEncodeBuffer));
    mmsContentType.msgContentParm_.type_ = "1";
    int32_t offset = 1;
    mmsEncodeBuffer.curPosition_ = CODE_BUFFER_MAX_SIZE + offset;
    ASSERT_FALSE(mmsContentType.EncodeTypeField(mmsEncodeBuffer));
    mmsContentType.msgContentParm_.type_ = "1";
    mmsEncodeBuffer.curPosition_ = CODE_BUFFER_MAX_SIZE - offset;
    ASSERT_FALSE(mmsContentType.EncodeTypeField(mmsEncodeBuffer));
    mmsContentType.msgContentParm_.type_ = "1";
    mmsEncodeBuffer.curPosition_ = CODE_BUFFER_MAX_SIZE - offset;
    ASSERT_FALSE(mmsContentType.EncodeTypeField(mmsEncodeBuffer));
    mmsContentType.msgContentParm_.textMap_[152] = "";
    mmsEncodeBuffer.curPosition_ = CODE_BUFFER_MAX_SIZE + offset;
    ASSERT_FALSE(mmsContentType.EncodeMmsBodyPartContentParam(mmsEncodeBuffer));
    mmsContentType.msgContentParm_.textMap_[152] = "";
    mmsEncodeBuffer.curPosition_ = CODE_BUFFER_MAX_SIZE - offset;
    ASSERT_FALSE(mmsContentType.EncodeMmsBodyPartContentParam(mmsEncodeBuffer));
    mmsContentType.msgContentParm_.textMap_[152] = "";
    offset = 2;
    mmsEncodeBuffer.curPosition_ = CODE_BUFFER_MAX_SIZE - offset;
    ASSERT_FALSE(mmsContentType.EncodeMmsBodyPartContentParam(mmsEncodeBuffer));
    mmsContentType.contentType_ = "";
    ASSERT_FALSE(mmsContentType.EncodeMmsBodyPartContentType(mmsEncodeBuffer));
    mmsContentType.contentType_ = "*/*";
    ASSERT_FALSE(mmsContentType.EncodeMmsBodyPartContentType(mmsEncodeBuffer));
    mmsEncodeBuffer.curPosition_ = CODE_BUFFER_MAX_SIZE + offset;
    ASSERT_FALSE(mmsContentType.EncodeMmsBodyPartContentType(mmsEncodeBuffer));
}

/**
 * @tc.number   Telephony_SmsMmsGtest_GsmSmsParamCodec_0001
 * @tc.name     Test GsmSmsParamCodec
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, GsmSmsParamCodec_0001, Function | MediumTest | Level1)
{
    auto gsmSmsParamCodec = std::make_shared<GsmSmsParamCodec>();
    char **ppParam = nullptr;
    ppParam = (char **)malloc(sizeof(char *) * (1024));
    SmsAddress *pAddress = new SmsAddress();
    SmsTimeStamp *pTimeStamp = new SmsTimeStamp();
    pTimeStamp->format = SmsTimeFormat::SMS_TIME_ABSOLUTE;
    pTimeStamp->time.absolute.timeZone = -1;
    EXPECT_GT(gsmSmsParamCodec->EncodeAddress(pAddress, ppParam), 0);
    pAddress->address[0] = '+';
    EXPECT_GT(gsmSmsParamCodec->EncodeAddress(pAddress, ppParam), 0);
    EXPECT_EQ(gsmSmsParamCodec->EncodeAddress(nullptr, ppParam), 0);
    EXPECT_EQ(gsmSmsParamCodec->EncodeTime(nullptr, ppParam), 0);
    EXPECT_GT(gsmSmsParamCodec->EncodeTime(pTimeStamp, ppParam), 0);
    pTimeStamp->format = SmsTimeFormat::SMS_TIME_RELATIVE;
    EXPECT_EQ(gsmSmsParamCodec->EncodeTime(pTimeStamp, ppParam), 1);
    SmsDcs *pDCS = new SmsDcs();
    EXPECT_EQ(gsmSmsParamCodec->EncodeDCS(nullptr, ppParam), 0);
    pDCS->codingGroup = SmsCodingGroup::SMS_DELETION_GROUP;
    EXPECT_EQ(gsmSmsParamCodec->EncodeDCS(pDCS, ppParam), 1);
    pDCS->codingGroup = SmsCodingGroup::SMS_DISCARD_GROUP;
    EXPECT_EQ(gsmSmsParamCodec->EncodeDCS(pDCS, ppParam), 1);
    pDCS->codingGroup = SmsCodingGroup::SMS_STORE_GROUP;
    EXPECT_EQ(gsmSmsParamCodec->EncodeDCS(pDCS, ppParam), 1);
    pDCS->codingGroup = SmsCodingGroup::SMS_GENERAL_GROUP;
    pDCS->msgClass = SmsMessageClass::SMS_FORWARD_MESSAGE;
    pDCS->bCompressed = true;
    EXPECT_EQ(gsmSmsParamCodec->EncodeDCS(pDCS, ppParam), 1);
    pDCS->codingGroup = SmsCodingGroup::SMS_CODING_CLASS_GROUP;
    EXPECT_EQ(gsmSmsParamCodec->EncodeDCS(pDCS, ppParam), 1);
    pDCS->codingScheme = SmsCodingScheme::SMS_CODING_7BIT;
    EXPECT_EQ(gsmSmsParamCodec->EncodeDCS(pDCS, ppParam), 1);
    pDCS->codingScheme = SmsCodingScheme::SMS_CODING_UCS2;
    EXPECT_EQ(gsmSmsParamCodec->EncodeDCS(pDCS, ppParam), 1);
    pDCS->codingScheme = SmsCodingScheme::SMS_CODING_8BIT;
    EXPECT_EQ(gsmSmsParamCodec->EncodeDCS(pDCS, ppParam), 1);
    pDCS->codingScheme = SmsCodingScheme::SMS_CODING_EUCKR;
    EXPECT_EQ(gsmSmsParamCodec->EncodeDCS(pDCS, ppParam), 0);
    pDCS->codingGroup = SmsCodingGroup::SMS_UNKNOWN_GROUP;
    EXPECT_EQ(gsmSmsParamCodec->EncodeDCS(pDCS, ppParam), 0);
    unsigned char encodeData[BUF_SIZE];
    char addressData[BUF_SIZE];
    char *address = addressData;
    unsigned char *pEncodeAddr = encodeData;
    EXPECT_EQ(gsmSmsParamCodec->EncodeSMSC(nullptr, nullptr), 0);
    EXPECT_EQ(gsmSmsParamCodec->EncodeSMSC(address, nullptr), 0);
    EXPECT_EQ(gsmSmsParamCodec->EncodeSMSC(address, pEncodeAddr), 0);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_GsmSmsParamCodec_0002
 * @tc.name     Test GsmSmsParamCodec
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, GsmSmsParamCodec_0002, Function | MediumTest | Level1)
{
    auto gsmSmsParamCodec = std::make_shared<GsmSmsParamCodec>();
    SmsAddress *pAddress = new SmsAddress();
    SmsTimeStamp *pTimeStamp = new SmsTimeStamp();
    SmsDcs *pDCS = new SmsDcs();
    unsigned char encodeData[BUF_SIZE];
    unsigned char *pSMSC = encodeData;
    char addressData[BUF_SIZE];
    char *pDecodeAddr = addressData;
    gsmSmsParamCodec->DecodeSMSC(pSMSC, 0, SmsTon::SMS_TON_UNKNOWN, pDecodeAddr);
    gsmSmsParamCodec->DecodeSMSC(nullptr, 1, SmsTon::SMS_TON_UNKNOWN, pDecodeAddr);
    gsmSmsParamCodec->DecodeSMSC(pSMSC, 1, SmsTon::SMS_TON_UNKNOWN, nullptr);
    gsmSmsParamCodec->DecodeSMSC(pSMSC, 1, SmsTon::SMS_TON_UNKNOWN, pDecodeAddr);
    gsmSmsParamCodec->DecodeSMSC(pSMSC, 1, SmsTon::SMS_TON_INTERNATIONAL, pDecodeAddr);
    EXPECT_EQ(gsmSmsParamCodec->EncodeSMSC(nullptr, pSMSC, 0), 0);
    EXPECT_EQ(gsmSmsParamCodec->EncodeSMSC(pAddress, nullptr, 0), 0);
    EXPECT_EQ(gsmSmsParamCodec->EncodeSMSC(pAddress, pSMSC, 0), 0);
    EXPECT_EQ(gsmSmsParamCodec->DecodeAddress(nullptr, 1, pAddress), 0);
    EXPECT_EQ(gsmSmsParamCodec->DecodeAddress(pSMSC, 1, nullptr), 0);
    EXPECT_GT(gsmSmsParamCodec->DecodeAddress(pSMSC, 1, pAddress), 0);
    EXPECT_EQ(gsmSmsParamCodec->DecodeTime(nullptr, pTimeStamp), 0);
    EXPECT_EQ(gsmSmsParamCodec->DecodeTime(pSMSC, nullptr), 0);
    EXPECT_EQ(gsmSmsParamCodec->DecodeDCS(nullptr, pDCS), 0);
    EXPECT_EQ(gsmSmsParamCodec->DecodeDCS(pSMSC, nullptr), 0);
    EXPECT_EQ(gsmSmsParamCodec->DecodeDCS(pSMSC, pDCS), 1);
    SmsAddress smsAddress;
    EXPECT_EQ(gsmSmsParamCodec->DecodeSMSC(nullptr, 1, smsAddress), 0);
    EXPECT_GT(gsmSmsParamCodec->DecodeSMSC(pSMSC, 1, smsAddress), 0);
    EXPECT_EQ(gsmSmsParamCodec->CheckCphsVmiMsg(nullptr, nullptr, nullptr), 0);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_GsmSmsUDataCodec_0001
 * @tc.name     Test GsmSmsUDataCodec
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, GsmSmsUDataCodec_0001, Function | MediumTest | Level1)
{
    auto gsmSmsUDataCodec = std::make_shared<GsmSmsUDataCodec>();
    SmsUserData smsUserData;
    SmsTpud *pTPUD = new SmsTpud();
    const struct SmsUserData *pUserData = &(smsUserData);
    char encodeData[BUF_SIZE];
    unsigned char addressData[BUF_SIZE];
    char *pSMSC = encodeData;
    unsigned char *pTpdu = addressData;
    char *pEncodeData = &(pSMSC[0]);
    EXPECT_GT(gsmSmsUDataCodec->EncodeUserData(pUserData, SmsCodingScheme::SMS_CODING_7BIT, pEncodeData), 0);
    EXPECT_GT(gsmSmsUDataCodec->EncodeUserData(pUserData, SmsCodingScheme::SMS_CODING_8BIT, pEncodeData), 0);
    EXPECT_GT(gsmSmsUDataCodec->EncodeUserData(pUserData, SmsCodingScheme::SMS_CODING_UCS2, pEncodeData), 0);
    EXPECT_EQ(gsmSmsUDataCodec->EncodeUserData(pUserData, SmsCodingScheme::SMS_CODING_ASCII7BIT, pEncodeData), 0);
    SmsUserData *userData = new SmsUserData();
    EXPECT_EQ(gsmSmsUDataCodec->DecodeUserData(nullptr, 1, true, SmsCodingScheme::SMS_CODING_7BIT, userData, pTPUD), 0);
    EXPECT_GE(gsmSmsUDataCodec->DecodeUserData(pTpdu, 1, true, SmsCodingScheme::SMS_CODING_7BIT, userData, pTPUD), 0);
    EXPECT_GE(gsmSmsUDataCodec->DecodeUserData(pTpdu, 1, true, SmsCodingScheme::SMS_CODING_8BIT, userData, pTPUD), 0);
    EXPECT_GE(gsmSmsUDataCodec->DecodeUserData(pTpdu, 1, true, SmsCodingScheme::SMS_CODING_UCS2, userData, pTPUD), 0);
    EXPECT_EQ(
        gsmSmsUDataCodec->DecodeUserData(pTpdu, 1, true, SmsCodingScheme::SMS_CODING_ASCII7BIT, userData, pTPUD), 0);
    EXPECT_EQ(gsmSmsUDataCodec->DecodeUserData(nullptr, 1, true, SmsCodingScheme::SMS_CODING_7BIT, userData), 0);
    EXPECT_GE(gsmSmsUDataCodec->DecodeUserData(pTpdu, 1, true, SmsCodingScheme::SMS_CODING_7BIT, userData), 0);
    EXPECT_GE(gsmSmsUDataCodec->DecodeUserData(pTpdu, 1, true, SmsCodingScheme::SMS_CODING_8BIT, userData), 0);
    EXPECT_GE(gsmSmsUDataCodec->DecodeUserData(pTpdu, 1, true, SmsCodingScheme::SMS_CODING_UCS2, userData), 0);
    EXPECT_EQ(gsmSmsUDataCodec->DecodeUserData(pTpdu, 1, true, SmsCodingScheme::SMS_CODING_ASCII7BIT, userData), 0);
    EXPECT_GT(gsmSmsUDataCodec->Encode8bitData(userData, pSMSC), 0);
    EXPECT_GT(gsmSmsUDataCodec->EncodeUCS2Data(userData, pSMSC), 0);
    userData->headerCnt = 1;
    userData->length = BUF_SIZE;
    EXPECT_EQ(gsmSmsUDataCodec->Encode8bitData(userData, pSMSC), 0);
    EXPECT_EQ(gsmSmsUDataCodec->EncodeUCS2Data(userData, pSMSC), 0);
    EXPECT_EQ(gsmSmsUDataCodec->DecodeGSMData(pTpdu, -1, true, userData, pTPUD), 0);
    EXPECT_EQ(gsmSmsUDataCodec->DecodeGSMData(pTpdu, -1, true, userData, pTPUD), 0);
    pTpdu[1] = DATA_LENGTH;
    EXPECT_EQ(gsmSmsUDataCodec->Decode8bitData(pTpdu, 1, true, userData, pTPUD), 0);
    pTpdu[1] = SHIFT_BIT;
    EXPECT_EQ(gsmSmsUDataCodec->Decode8bitData(pTpdu, 1, true, userData, pTPUD), 0);
    pTpdu[1] = 1;
    EXPECT_EQ(gsmSmsUDataCodec->Decode8bitData(pTpdu, 1, true, userData, pTPUD), 0);
    EXPECT_EQ(gsmSmsUDataCodec->DecodeUCS2Data(pTpdu, -1, true, userData, pTPUD), 0);
    EXPECT_EQ(gsmSmsUDataCodec->DecodeUCS2Data(pTpdu, 1, true, userData, pTPUD), 0);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_GsmSmsUDataCodec_0002
 * @tc.name     Test GsmSmsUDataCodec
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, GsmSmsUDataCodec_0002, Function | MediumTest | Level1)
{
    auto gsmSmsUDataCodec = std::make_shared<GsmSmsUDataCodec>();
    char encodeData[BUF_SIZE];
    char *pSMSC = encodeData;
    unsigned char addressData[BUF_SIZE];
    unsigned char *pTpdu = addressData;
    SmsUDH header;
    header.udhType = SMS_UDH_CONCAT_8BIT;
    EXPECT_GT(gsmSmsUDataCodec->EncodeHeaderConcat(header, pSMSC), 0);
    EXPECT_GT(gsmSmsUDataCodec->EncodeHeader(header, pSMSC), 0);
    header.udhType = SMS_UDH_CONCAT_16BIT;
    EXPECT_GT(gsmSmsUDataCodec->EncodeHeaderConcat(header, pSMSC), 0);
    EXPECT_GT(gsmSmsUDataCodec->EncodeHeader(header, pSMSC), 0);
    header.udhType = SMS_UDH_APP_PORT_8BIT;
    EXPECT_GT(gsmSmsUDataCodec->EncodeHeader(header, pSMSC), 0);
    header.udhType = SMS_UDH_APP_PORT_16BIT;
    EXPECT_GT(gsmSmsUDataCodec->EncodeHeader(header, pSMSC), 0);
    header.udhType = SMS_UDH_ALTERNATE_REPLY_ADDRESS;
    EXPECT_GT(gsmSmsUDataCodec->EncodeHeader(header, pSMSC), 0);
    header.udhType = SMS_UDH_SINGLE_SHIFT;
    EXPECT_GT(gsmSmsUDataCodec->EncodeHeader(header, pSMSC), 0);
    header.udhType = SMS_UDH_LOCKING_SHIFT;
    EXPECT_GT(gsmSmsUDataCodec->EncodeHeader(header, pSMSC), 0);
    header.udhType = SMS_UDH_NONE;
    EXPECT_EQ(gsmSmsUDataCodec->EncodeHeaderConcat(header, pSMSC), 0);
    EXPECT_EQ(gsmSmsUDataCodec->EncodeHeader(header, pSMSC), 0);
    header.udhType = SMS_UDH_EMS_LAST;
    EXPECT_EQ(gsmSmsUDataCodec->EncodeHeaderConcat(header, pSMSC), 0);
    EXPECT_EQ(gsmSmsUDataCodec->EncodeHeader(header, pSMSC), 0);
    SmsUDH *pHeader = new SmsUDH();
    pHeader->udhType = SmsUDHType::SMS_UDH_CONCAT_8BIT;
    EXPECT_EQ(gsmSmsUDataCodec->DecodeHeader(pTpdu, 1, pHeader), 0);
    pHeader->udhType = SmsUDHType::SMS_UDH_CONCAT_16BIT;
    EXPECT_EQ(gsmSmsUDataCodec->DecodeHeader(pTpdu, 1, pHeader), 0);
    pHeader->udhType = SmsUDHType::SMS_UDH_APP_PORT_8BIT;
    EXPECT_EQ(gsmSmsUDataCodec->DecodeHeader(pTpdu, 1, pHeader), 0);
    pHeader->udhType = SmsUDHType::SMS_UDH_APP_PORT_16BIT;
    EXPECT_EQ(gsmSmsUDataCodec->DecodeHeader(pTpdu, 1, pHeader), 0);
    pHeader->udhType = SmsUDHType::SMS_UDH_SPECIAL_SMS;
    EXPECT_EQ(gsmSmsUDataCodec->DecodeHeader(pTpdu, 1, pHeader), 0);
    pHeader->udhType = SmsUDHType::SMS_UDH_ALTERNATE_REPLY_ADDRESS;
    EXPECT_EQ(gsmSmsUDataCodec->DecodeHeader(pTpdu, 1, pHeader), 0);
    pHeader->udhType = SmsUDHType::SMS_UDH_SINGLE_SHIFT;
    EXPECT_EQ(gsmSmsUDataCodec->DecodeHeader(pTpdu, 1, pHeader), 0);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_GsmSmsUDataCodec_0003
 * @tc.name     Test GsmSmsUDataCodec
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, GsmSmsUDataCodec_0003, Function | MediumTest | Level1)
{
    auto gsmSmsUDataCodec = std::make_shared<GsmSmsUDataCodec>();
    SmsUDH *pHeader = new SmsUDH();
    gsmSmsUDataCodec->DebugDecodeHeader(nullptr);
    pHeader->udhType = SMS_UDH_CONCAT_8BIT;
    gsmSmsUDataCodec->DebugDecodeHeader(pHeader);
    pHeader->udhType = SMS_UDH_CONCAT_16BIT;
    gsmSmsUDataCodec->DebugDecodeHeader(pHeader);
    pHeader->udhType = SMS_UDH_APP_PORT_8BIT;
    gsmSmsUDataCodec->DebugDecodeHeader(pHeader);
    pHeader->udhType = SMS_UDH_APP_PORT_16BIT;
    gsmSmsUDataCodec->DebugDecodeHeader(pHeader);
    pHeader->udhType = SMS_UDH_SPECIAL_SMS;
    gsmSmsUDataCodec->DebugDecodeHeader(pHeader);
    pHeader->udhType = SMS_UDH_ALTERNATE_REPLY_ADDRESS;
    gsmSmsUDataCodec->DebugDecodeHeader(pHeader);
    pHeader->udhType = SMS_UDH_SINGLE_SHIFT;
    gsmSmsUDataCodec->DebugDecodeHeader(pHeader);
    pHeader->udhType = SMS_UDH_LOCKING_SHIFT;
    gsmSmsUDataCodec->DebugDecodeHeader(pHeader);
    pHeader->udhType = SMS_UDH_NONE;
    gsmSmsUDataCodec->DebugDecodeHeader(pHeader);
    unsigned char addressData[BUF_SIZE];
    unsigned char *pTpdu = addressData;
    pHeader->udhType = SmsUDHType::SMS_UDH_LOCKING_SHIFT;
    EXPECT_EQ(gsmSmsUDataCodec->DecodeHeader(pTpdu, 1, pHeader), 0);
    pHeader->udhType = SmsUDHType::SMS_UDH_NONE;
    EXPECT_EQ(gsmSmsUDataCodec->DecodeHeader(pTpdu, 1, pHeader), 0);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_GsmSmsTpduCodec_0001
 * @tc.name     Test GsmSmsTpduCodec
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, GsmSmsTpduCodec_0001, Function | MediumTest | Level1)
{
    auto gsmSmsTpduCodec = std::make_shared<GsmSmsTpduCodec>();
    unsigned char encodeData[BUF_SIZE];
    unsigned char *pSMSC = encodeData;
    char addressData[BUF_SIZE];
    char *pDecodeAddr = addressData;
    SmsTpdu *pSmsTpdu = new SmsTpdu();
    pSmsTpdu->tpduType = SmsTpduType::SMS_TPDU_SUBMIT;
    EXPECT_GT(gsmSmsTpduCodec->EncodeTpdu(pSmsTpdu, pDecodeAddr, 1), 0);
    pSmsTpdu->tpduType = SmsTpduType::SMS_TPDU_DELIVER;
    EXPECT_GT(gsmSmsTpduCodec->EncodeTpdu(pSmsTpdu, pDecodeAddr, 1), 0);
    pSmsTpdu->tpduType = SmsTpduType::SMS_TPDU_DELIVER_REP;
    EXPECT_GT(gsmSmsTpduCodec->EncodeTpdu(pSmsTpdu, pDecodeAddr, 1), 0);
    pSmsTpdu->tpduType = SmsTpduType::SMS_TPDU_STATUS_REP;
    EXPECT_GT(gsmSmsTpduCodec->EncodeTpdu(pSmsTpdu, pDecodeAddr, 1), 0);
    EXPECT_EQ(gsmSmsTpduCodec->DecodeTpdu(nullptr, 1, pSmsTpdu), 0);
    EXPECT_EQ(gsmSmsTpduCodec->DecodeTpdu(pSMSC, 1, nullptr), 0);
    EXPECT_GT(gsmSmsTpduCodec->DecodeTpdu(pSMSC, 1, pSmsTpdu), 0);
    pSMSC[0] = 1;
    EXPECT_GT(gsmSmsTpduCodec->DecodeTpdu(pSMSC, 1, pSmsTpdu), 0);
    pSMSC[0] = VALUE_LENGTH;
    EXPECT_GT(gsmSmsTpduCodec->DecodeTpdu(pSMSC, 1, pSmsTpdu), 0);
    pSMSC[0] = HEADER_LENGTH;
    EXPECT_EQ(gsmSmsTpduCodec->DecodeTpdu(pSMSC, 1, pSmsTpdu), 0);
    SmsSubmit *pSubmit = new SmsSubmit();
    SmsDeliver *pDeliver = new SmsDeliver();
    SmsDeliverReport *pDeliverRep = new SmsDeliverReport();
    EXPECT_EQ(gsmSmsTpduCodec->EncodeSubmit(nullptr, pDecodeAddr), 0);
    EXPECT_EQ(gsmSmsTpduCodec->EncodeSubmit(pSubmit, nullptr), 0);
    EXPECT_GT(gsmSmsTpduCodec->EncodeSubmit(pSubmit, pDecodeAddr), 0);
    EXPECT_EQ(gsmSmsTpduCodec->EncodeDeliver(nullptr, pDecodeAddr), 0);
    EXPECT_EQ(gsmSmsTpduCodec->EncodeDeliver(pDeliver, nullptr), 0);
    pDeliver->bMoreMsg = false;
    pDeliver->bStatusReport = true;
    pDeliver->bHeaderInd = true;
    pDeliver->bReplyPath = true;
    EXPECT_GT(gsmSmsTpduCodec->EncodeDeliver(pDeliver, pDecodeAddr), 0);
    EXPECT_EQ(gsmSmsTpduCodec->EncodeDeliverReport(nullptr, pDecodeAddr), 0);
    EXPECT_EQ(gsmSmsTpduCodec->EncodeDeliverReport(pDeliverRep, nullptr), 0);
    pDeliverRep->bHeaderInd = true;
    pDeliverRep->reportType = _SMS_REPORT_TYPE_E::SMS_REPORT_NEGATIVE;
    pDeliverRep->paramInd = 1;
    EXPECT_GT(gsmSmsTpduCodec->EncodeDeliverReport(pDeliverRep, pDecodeAddr), 0);
    pDeliverRep->paramInd = VALUE_LENGTH;
    EXPECT_GT(gsmSmsTpduCodec->EncodeDeliverReport(pDeliverRep, pDecodeAddr), 0);
    pDeliverRep->paramInd = START_BIT;
    EXPECT_GT(gsmSmsTpduCodec->EncodeDeliverReport(pDeliverRep, pDecodeAddr), 0);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_GsmSmsTpduCodec_0002
 * @tc.name     Test GsmSmsTpduCodec
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, GsmSmsTpduCodec_0002, Function | MediumTest | Level1)
{
    auto gsmSmsTpduCodec = std::make_shared<GsmSmsTpduCodec>();
    unsigned char encodeData[BUF_SIZE];
    unsigned char *pTpdu = encodeData;
    char addressData[BUF_SIZE];
    char *pDecodeAddr = addressData;
    SmsStatusReport *pStatusRep = new SmsStatusReport();
    SmsSubmit *pSmsSub = new SmsSubmit();
    EXPECT_EQ(gsmSmsTpduCodec->EncodeStatusReport(nullptr, pDecodeAddr), 0);
    EXPECT_EQ(gsmSmsTpduCodec->EncodeStatusReport(pStatusRep, nullptr), 0);
    pStatusRep->bMoreMsg = true;
    pStatusRep->bStatusReport = true;
    pStatusRep->bHeaderInd = true;
    pStatusRep->paramInd = 1;
    EXPECT_GT(gsmSmsTpduCodec->EncodeStatusReport(pStatusRep, pDecodeAddr), 0);
    pStatusRep->paramInd = VALUE_LENGTH;
    EXPECT_GT(gsmSmsTpduCodec->EncodeStatusReport(pStatusRep, pDecodeAddr), 0);
    pStatusRep->paramInd = START_BIT;
    EXPECT_GT(gsmSmsTpduCodec->EncodeStatusReport(pStatusRep, pDecodeAddr), 0);
    EXPECT_EQ(gsmSmsTpduCodec->DecodeSubmit(nullptr, 1, pSmsSub), 0);
    EXPECT_EQ(gsmSmsTpduCodec->DecodeSubmit(pTpdu, 1, nullptr), 0);
    EXPECT_GT(gsmSmsTpduCodec->DecodeSubmit(pTpdu, 1, pSmsSub), 0);
    pTpdu[0] = START_BIT;
    EXPECT_GT(gsmSmsTpduCodec->DecodeSubmit(pTpdu, 1, pSmsSub), 0);
    pTpdu[0] = PDU_BUFFE_LENGTH_ONE;
    EXPECT_GT(gsmSmsTpduCodec->DecodeSubmit(pTpdu, 1, pSmsSub), 0);
    pTpdu[0] = PID_LENGTH;
    EXPECT_GT(gsmSmsTpduCodec->DecodeSubmit(pTpdu, 1, pSmsSub), 0);
    SmsDeliver *pDeliver = new SmsDeliver();
    EXPECT_EQ(gsmSmsTpduCodec->DecodeDeliver(nullptr, 1, pDeliver), 0);
    EXPECT_EQ(gsmSmsTpduCodec->DecodeDeliver(pTpdu, 1, nullptr), 0);
    EXPECT_EQ(gsmSmsTpduCodec->DecodeStatusReport(nullptr, 1, pStatusRep), 0);
    EXPECT_EQ(gsmSmsTpduCodec->DecodeStatusReport(pTpdu, 1, nullptr), 0);
    EXPECT_GT(gsmSmsTpduCodec->DecodeStatusReport(pTpdu, 1, pStatusRep), 0);
    pTpdu[1] = 1;
    EXPECT_GT(gsmSmsTpduCodec->DecodeStatusReport(pTpdu, 1, pStatusRep), 0);
    pTpdu[1] = VALUE_LENGTH;
    EXPECT_GT(gsmSmsTpduCodec->DecodeStatusReport(pTpdu, 1, pStatusRep), 0);
    pTpdu[1] = START_BIT;
    EXPECT_GT(gsmSmsTpduCodec->DecodeStatusReport(pTpdu, 1, pStatusRep), 0);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_GsmSmsTpduCodec_0003
 * @tc.name     Test GsmSmsTpduCodec
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, GsmSmsTpduCodec_0003, Function | MediumTest | Level1)
{
    auto gsmSmsTpduCodec = std::make_shared<GsmSmsTpduCodec>();
    unsigned char encodeData[BUF_SIZE];
    unsigned char *pTpdu = encodeData;
    SmsSubmit pSubmit;
    pSubmit.bRejectDup = true;
    pSubmit.bStatusReport = true;
    pSubmit.bHeaderInd = true;
    pSubmit.bReplyPath = true;
    gsmSmsTpduCodec->DebugTpdu(pTpdu, 1, DecodeType::DECODE_SUBMIT_TYPE);
    EXPECT_GT(gsmSmsTpduCodec->EncodeSubmitTpduType(pSubmit, pTpdu), 0);
    pSubmit.vpf = SmsVpf::SMS_VPF_NOT_PRESENT;
    EXPECT_GT(gsmSmsTpduCodec->EncodeSubmitTpduType(pSubmit, pTpdu), 0);
    pSubmit.vpf = SmsVpf::SMS_VPF_ENHANCED;
    EXPECT_GT(gsmSmsTpduCodec->EncodeSubmitTpduType(pSubmit, pTpdu), 0);
    pSubmit.vpf = SmsVpf::SMS_VPF_RELATIVE;
    EXPECT_GT(gsmSmsTpduCodec->EncodeSubmitTpduType(pSubmit, pTpdu), 0);
    pSubmit.vpf = SmsVpf::SMS_VPF_ABSOLUTE;
    EXPECT_GT(gsmSmsTpduCodec->EncodeSubmitTpduType(pSubmit, pTpdu), 0);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_SmsBaseMessage_0001
 * @tc.name     Test SmsBaseMessage
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, SmsBaseMessage_0001, Function | MediumTest | Level1)
{
    auto smsBaseMessage = std::make_shared<GsmSmsMessage>();
    smsBaseMessage->ConvertMessageClass(SmsMessageClass::SMS_SIM_MESSAGE);
    smsBaseMessage->ConvertMessageClass(SmsMessageClass::SMS_INSTANT_MESSAGE);
    smsBaseMessage->ConvertMessageClass(SmsMessageClass::SMS_OPTIONAL_MESSAGE);
    smsBaseMessage->ConvertMessageClass(SmsMessageClass::SMS_FORWARD_MESSAGE);
    smsBaseMessage->ConvertMessageClass(SmsMessageClass::SMS_CLASS_UNKNOWN);
    SplitInfo split;
    SmsCodingScheme codingType = SmsCodingScheme::SMS_CODING_7BIT;
    smsBaseMessage->ConvertSpiltToUtf8(split, codingType);
    split.encodeData.push_back(1);
    codingType = SmsCodingScheme::SMS_CODING_UCS2;
    smsBaseMessage->ConvertSpiltToUtf8(split, codingType);
    codingType = SmsCodingScheme::SMS_CODING_AUTO;
    smsBaseMessage->ConvertSpiltToUtf8(split, codingType);
    smsBaseMessage->smsUserData_.headerCnt = 1;
    smsBaseMessage->smsUserData_.header[0].udhType = SmsUDHType::SMS_UDH_CONCAT_8BIT;
    EXPECT_FALSE(smsBaseMessage->GetConcatMsg() == nullptr);
    smsBaseMessage->smsUserData_.header[0].udhType = SmsUDHType::SMS_UDH_CONCAT_16BIT;
    EXPECT_FALSE(smsBaseMessage->GetConcatMsg() == nullptr);
    smsBaseMessage->smsUserData_.header[0].udhType = SmsUDHType::SMS_UDH_APP_PORT_8BIT;
    EXPECT_FALSE(smsBaseMessage->GetPortAddress() == nullptr);
    smsBaseMessage->smsUserData_.header[0].udhType = SmsUDHType::SMS_UDH_APP_PORT_16BIT;
    EXPECT_FALSE(smsBaseMessage->GetPortAddress() == nullptr);
    smsBaseMessage->smsUserData_.header[0].udhType = SmsUDHType::SMS_UDH_SPECIAL_SMS;
    EXPECT_FALSE(smsBaseMessage->GetSpecialSmsInd() == nullptr);
    smsBaseMessage->smsUserData_.header[0].udhType = SmsUDHType::SMS_UDH_APP_PORT_8BIT;
    EXPECT_FALSE(smsBaseMessage->IsWapPushMsg());
    MSG_LANGUAGE_ID_T langId = 1;
    codingType = SmsCodingScheme::SMS_CODING_7BIT;
    EXPECT_GT(smsBaseMessage->GetMaxSegmentSize(codingType, 1, true, langId, 1), 0);
    EXPECT_GT(smsBaseMessage->GetSegmentSize(codingType, 1, true, langId, 1), 0);
    codingType = SmsCodingScheme::SMS_CODING_ASCII7BIT;
    EXPECT_GT(smsBaseMessage->GetMaxSegmentSize(codingType, 1, true, langId, 1), 0);
    EXPECT_GT(smsBaseMessage->GetSegmentSize(codingType, 1, true, langId, 1), 0);
    codingType = SmsCodingScheme::SMS_CODING_8BIT;
    EXPECT_GT(smsBaseMessage->GetMaxSegmentSize(codingType, 1, true, langId, 1), 0);
    EXPECT_GT(smsBaseMessage->GetSegmentSize(codingType, 1, true, langId, 1), 0);
    codingType = SmsCodingScheme::SMS_CODING_UCS2;
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
HWTEST_F(BranchTest, GsmSmsReceiveHandler_0001, Function | MediumTest | Level1)
{
    std::shared_ptr<AppExecFwk::EventRunner> runner = AppExecFwk::EventRunner::Create("test");
    auto gsmSmsReceiveHandler = std::make_shared<GsmSmsReceiveHandler>(runner, 1);
    gsmSmsReceiveHandler->UnRegisterHandler();
    EXPECT_NE(gsmSmsReceiveHandler->HandleSmsByType(nullptr), TELEPHONY_ERR_SUCCESS);
    EXPECT_EQ(gsmSmsReceiveHandler->TransformMessageInfo(nullptr), nullptr);
}

/**

 * @tc.number   Telephony_SmsMmsGtest_MmsBody_0001
 * @tc.name     Test MmsBody
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, SmsMmsGtest_MmsBody_0001, Function | MediumTest | Level1)
{
    MmsBody mmsBody;
    MmsDecodeBuffer mmsDecodeBuffer;
    MmsHeader mmsHeader;
    MmsEncodeBuffer mmsEncodeBuffer;
    MmsBodyPart mmsBodyPart;
    mmsDecodeBuffer.curPosition_ = 1;
    mmsDecodeBuffer.totolLength_ = 0;
    EXPECT_FALSE(mmsBody.DecodeMultipart(mmsDecodeBuffer));
    EXPECT_FALSE(mmsBody.DecodeMmsBody(mmsDecodeBuffer, mmsHeader));
    EXPECT_TRUE(mmsBody.EncodeMmsBody(mmsEncodeBuffer));
    EXPECT_FALSE(mmsBody.EncodeMmsHeaderContentType(mmsHeader, mmsEncodeBuffer));
    EXPECT_FALSE(mmsBody.IsContentLocationPartExist(""));
    EXPECT_FALSE(mmsBody.IsContentIdPartExist(""));
    EXPECT_FALSE(mmsBody.IsBodyPartExist(mmsBodyPart));
    EXPECT_TRUE(mmsBody.AddMmsBodyPart(mmsBodyPart));
    mmsBodyPart.isSmilFile_ = true;
    EXPECT_TRUE(mmsBody.AddMmsBodyPart(mmsBodyPart));
    mmsBody.bHaveSmilPart_ = true;
    EXPECT_FALSE(mmsBody.AddMmsBodyPart(mmsBodyPart));
}

/**
 * @tc.number   Telephony_SmsMmsGtest_MmsMsg_0001
 * @tc.name     Test MmsMsg
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, SmsMmsGtest_MmsMsg_0001, Function | MediumTest | Level1)
{
    MmsMsg mmsMsg;
    uint32_t outLen;
    std::vector<MmsAddress> toAddrs = {};
    MmsAttachment mmsAttachment;
    std::vector<MmsAttachment> attachments = {};
    EXPECT_FALSE(mmsMsg.DecodeMsg(""));
    EXPECT_FALSE(mmsMsg.DecodeMsg(nullptr, 0));
    mmsMsg.EncodeMsg(outLen);
    mmsMsg.GetMmsFrom();
    EXPECT_FALSE(mmsMsg.SetMmsTo(toAddrs));
    mmsMsg.GetHeaderStringValue(0);
    mmsMsg.GetHeaderContentTypeStart();
    EXPECT_FALSE(mmsMsg.AddAttachment(mmsAttachment));
    EXPECT_FALSE(mmsMsg.GetAllAttachment(attachments));
}

/**
 * @tc.number   Telephony_SmsMmsGtest_GsmSmsSender_0001
 * @tc.name     Test GsmSmsSender
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, GsmSmsSender_0001, Function | MediumTest | Level1)
{
    std::function<void(std::shared_ptr<SmsSendIndexer>)> sendRetryFun = nullptr;
    std::shared_ptr<AppExecFwk::EventRunner> runner = AppExecFwk::EventRunner::Create("test");
    auto gsmSmsSender = std::make_shared<GsmSmsSender>(runner, INVALID_SLOTID, sendRetryFun);
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
 * @tc.number   Telephony_SmsMmsGtest_SmsMiscManager_0001
 * @tc.name     Test SmsMiscManager
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, SmsMiscManager_0001, Function | MediumTest | Level1)
{
    std::shared_ptr<AppExecFwk::EventRunner> runner = AppExecFwk::EventRunner::Create("test");
    auto smsMiscManager = std::make_shared<SmsMiscManager>(runner, INVALID_SLOTID);
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
    EXPECT_EQ(smsMiscManager->SetCBConfig(false, 1, 1, 1), TELEPHONY_ERR_SUCCESS);
    EXPECT_GE(smsMiscManager->SetCBConfig(true, 1, 1, 1), TELEPHONY_ERR_SUCCESS);
    EXPECT_TRUE(smsMiscManager->OpenCBRange(1, 1));
    smsMiscManager->rangeList_.clear();
    smsMiscManager->rangeList_.emplace_back(VALUE_LENGTH, 1);
    EXPECT_EQ(smsMiscManager->SetCBConfig(true, 1, 1, 1), TELEPHONY_ERR_RIL_CMD_FAIL);
    auto oldIter = smsMiscManager->rangeList_.begin();
    SmsMiscManager::infoData data(1, 1);
    EXPECT_TRUE(smsMiscManager->ExpandMsgId(1, 1, oldIter, data));
    EXPECT_TRUE(smsMiscManager->ExpandMsgId(1, 0, oldIter, data));
    std::string smsc = "";
    std::string pdu = "";
    std::vector<ShortMessage> message;
    EXPECT_GE(
        smsMiscManager->AddSimMessage(smsc, pdu, ISmsServiceInterface::SimMessageStatus::SIM_MESSAGE_STATUS_UNREAD), 0);
    EXPECT_GE(smsMiscManager->UpdateSimMessage(
                  1, ISmsServiceInterface::SimMessageStatus::SIM_MESSAGE_STATUS_UNREAD, pdu, smsc),
        0);
    EXPECT_EQ(smsMiscManager->GetAllSimMessages(message), TELEPHONY_ERR_UNKNOWN_NETWORK_TYPE);
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
 * @tc.number   Telephony_SmsMmsGtest_SmsService_0001
 * @tc.name     Test SmsMiscManager
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, SmsService_0001, Function | MediumTest | Level1)
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
 * @tc.name     Test SmsMiscManager
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, SmsService_0002, Function | MediumTest | Level1)
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
 * @tc.number   Telephony_SmsMmsGtest_MmsBodyPartHeader_0001
 * @tc.name     Test SmsMiscManager
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, MmsBodyPartHeader_0001, Function | MediumTest | Level1)
{
    auto mmsBodyPartHeader = std::make_shared<MmsBodyPartHeader>();
    MmsDecodeBuffer decodeBuffer;
    uint32_t len = 1;
    decodeBuffer.curPosition_ = 0;
    decodeBuffer.totolLength_ = 0;
    EXPECT_FALSE(mmsBodyPartHeader->DecodeContentDisposition(decodeBuffer, len));
    MmsEncodeBuffer encodeBuffer;
    mmsBodyPartHeader->strContentTransferEncoding_ = "";
    EXPECT_TRUE(mmsBodyPartHeader->EncodeContentTransferEncoding(encodeBuffer));
    EXPECT_TRUE(mmsBodyPartHeader->EncodeContentLocation(encodeBuffer));
    EXPECT_TRUE(mmsBodyPartHeader->EncodeContentId(encodeBuffer));
    encodeBuffer.curPosition_ = CODE_BUFFER_MAX_SIZE;
    mmsBodyPartHeader->strContentLocation_ = "123";
    EXPECT_FALSE(mmsBodyPartHeader->EncodeMmsBodyPartHeader(encodeBuffer));
    mmsBodyPartHeader->strContentID_ = "123";
    encodeBuffer.curPosition_ = CODE_BUFFER_MAX_SIZE;
    EXPECT_FALSE(mmsBodyPartHeader->EncodeContentLocation(encodeBuffer));
    EXPECT_FALSE(mmsBodyPartHeader->EncodeContentId(encodeBuffer));
    EXPECT_FALSE(mmsBodyPartHeader->EncodeMmsBodyPartHeader(encodeBuffer));
}

/**
 * @tc.number   Telephony_SmsMmsGtest_SmsReceiveReliabilityHandler_0001
 * @tc.name     Test SmsReceiveReliabilityHandler
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, SmsReceiveReliabilityHandler_0001, Function | MediumTest | Level1)
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
} // namespace Telephony
} // namespace OHOS
