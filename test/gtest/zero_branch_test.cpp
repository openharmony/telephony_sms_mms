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
#include "cdma_sms_receive_handler.h"
#include "cdma_sms_sender.h"
#include "delivery_short_message_callback_stub.h"
#include "gsm_sms_cb_handler.h"
#include "gsm_sms_message.h"
#include "gtest/gtest.h"
#include "mms_header.h"
#include "msg_text_convert.h"
#include "radio_event.h"
#include "send_short_message_callback_stub.h"
#include "short_message.h"
#include "sms_cb_message.h"
#include "sms_common_utils.h"
#include "sms_receive_handler.h"
#include "sms_wap_push_buffer.h"
#include "telephony_errors.h"
#include "telephony_hisysevent.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
using namespace testing::ext;

namespace {
const std::string TEXT_SMS_CONTENT = "hello world";
const int8_t TEXT_PORT_NUM = -1;
const uint32_t TRANSACTION_ID_LENGTH = 3;
const int16_t WAP_PUSH_PORT = 2948;
const uint8_t MIN_TOKEN = 30;
const uint8_t MAX_TOKEN = 127;
const int BUF_SIZE = 2401;
const int TEXT_LENGTH = 2;
const int FILL_BITS = 2;
const int DIGIT_LEN = 3;
const int START_BIT = 4;
const unsigned char SRC_TEXT = 2;
const uint8_t FIELD_ID = 2;
const int32_t INVALID_SLOTID = 2;
const uint16_t PWS_FIRST_ID = 0x1100;
const size_t DATA_LENGTH = 162;
const int32_t HEADER_LENGTH = 7;
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
    MsgTextConvert *textCvt = MsgTextConvert::Instance();
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
    EXPECT_GE(textCvt->ConvertUTF8ToGSM7bit(paras), 0);
    EXPECT_GE(textCvt->ConvertCdmaUTF8ToAuto(pDestText, 1, pMsgText, 1, &smsCodingScheme), -1);
    EXPECT_GE(textCvt->ConvertGsmUTF8ToAuto(pDestText, 1, pMsgText, 1, &smsCodingScheme), -1);
    EXPECT_EQ(textCvt->ConvertUTF8ToUCS2(pDestText, 1, pMsgText, -1), -1);
    EXPECT_EQ(textCvt->ConvertUTF8ToUCS2(pDestText, 0, pMsgText, -1), -1);
    EXPECT_EQ(textCvt->ConvertCdmaUTF8ToAuto(pDestText, 1, pMsgText, 0, &smsCodingScheme), 0);
    EXPECT_EQ(textCvt->ConvertGsmUTF8ToAuto(pDestText, 1, pMsgText, 0, &smsCodingScheme), 0);
    EXPECT_EQ(textCvt->ConvertGSM7bitToUTF8(pDestText, 0, pMsgText, 0, &msgLangInfo), 0);
    EXPECT_GE(textCvt->ConvertSHIFTJISToUTF8(pDestText, 1, pMsgText, -1), 0);
    EXPECT_GE(textCvt->ConvertUCS2ToUTF8(pDestText, 1, pMsgText, -1), 0);
    EXPECT_GE(textCvt->ConvertEUCKRToUTF8(pDestText, 1, pMsgText, -1), 0);
    EXPECT_FALSE(textCvt->ConvertUCS2ToUTF8(pDestText, 0, pMsgText, 0));
    EXPECT_FALSE(textCvt->ConvertEUCKRToUTF8(pDestText, 0, pMsgText, 1));
    EXPECT_FALSE(textCvt->ConvertSHIFTJISToUTF8(pDestText, 0, pMsgText, 1));
    EXPECT_EQ(textCvt->ConvertUCS2ToGSM7bit(pDestText, 0, pMsgText, 0, pLangId, pIncludeAbnormalChar), -1);
    EXPECT_GT(textCvt->ConvertUCS2ToGSM7bit(pDestText, 1, pMsgText, TEXT_LENGTH, pLangId, pIncludeAbnormalChar), 0);
    EXPECT_EQ(textCvt->ConvertUCS2ToGSM7bitAuto(pDestText, 0, pMsgText, 0, pIncludeAbnormalChar), -1);
    EXPECT_GE(textCvt->ConvertUCS2ToGSM7bitAuto(pDestText, 1, pMsgText, 1, pIncludeAbnormalChar), 0);
    EXPECT_EQ(textCvt->ConvertUCS2ToASCII(pDestText, 0, pMsgText, 0, pIncludeAbnormalChar), -1);
    EXPECT_GE(textCvt->ConvertUCS2ToASCII(pDestText, 1, pMsgText, 1, pIncludeAbnormalChar), 0);
    EXPECT_EQ(textCvt->GetLangType(pMsgText, 0), MSG_DEFAULT_CHAR);
    EXPECT_GE(textCvt->GetLangType(pMsgText, 0), MSG_DEFAULT_CHAR);
    EXPECT_EQ(textCvt->FindUCS2toGSM7Ext(pDestText, 0, inText, bAbnormal), 0);
    EXPECT_GE(textCvt->FindUCS2toGSM7Ext(pDestText, 1, inText, bAbnormal), 0);
    EXPECT_EQ(textCvt->FindUCS2toTurkish(pDestText, 0, inText, bAbnormal), 0);
    EXPECT_GE(textCvt->FindUCS2toTurkish(pDestText, 1, inText, bAbnormal), 0);
    EXPECT_EQ(textCvt->FindUCS2toSpanish(pDestText, 0, inText, bAbnormal), 0);
    EXPECT_GE(textCvt->FindUCS2toSpanish(pDestText, 1, inText, bAbnormal), 0);
    EXPECT_EQ(textCvt->FindUCS2toPortu(pDestText, 0, inText, bAbnormal), 0);
    EXPECT_GE(textCvt->FindUCS2toPortu(pDestText, 1, inText, bAbnormal), 0);
    EXPECT_GE(textCvt->FindUCS2ReplaceChar(inText), MSG_DEFAULT_CHAR);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_MsgTextConvert_0002
 * @tc.name     Test MsgTextConvert
 * @tc.desc     Function test
 */
HWTEST_F(BranchTest, MsgTextConvert_0002, Function | MediumTest | Level1)
{
    MsgTextConvert *textCvt = MsgTextConvert::Instance();
    MsgLangInfo pLangInfo;
    pLangInfo.bLockingShift = true;
    pLangInfo.bSingleShift = true;
    unsigned char encodeData[BUF_SIZE];
    unsigned short result = 1;
    const unsigned char *pText = nullptr;
    const unsigned char *pMsgText = (const unsigned char *)TEXT_SMS_CONTENT.c_str();
    unsigned char *pDestText = encodeData;
    textCvt->ConvertDumpTextToHex(pText, 1);
    textCvt->ConvertDumpTextToHex(pMsgText, 1);
    EXPECT_EQ(textCvt->ConvertGSM7bitToUCS2(pDestText, 0, pMsgText, 0, &pLangInfo), -1);
    pLangInfo.lockingLang = MSG_ID_TURKISH_LANG;
    EXPECT_GT(textCvt->ConvertGSM7bitToUCS2(pDestText, 1, pMsgText, 1, &pLangInfo), 0);
    EXPECT_GT(textCvt->EscapeToUCS2(SRC_TEXT, pLangInfo), 0);
    pLangInfo.lockingLang = MSG_ID_PORTUGUESE_LANG;
    EXPECT_GT(textCvt->ConvertGSM7bitToUCS2(pDestText, 1, pMsgText, 1, &pLangInfo), 0);
    EXPECT_GT(textCvt->EscapeToUCS2(SRC_TEXT, pLangInfo), 0);
    pLangInfo.bLockingShift = false;
    EXPECT_GT(textCvt->ConvertGSM7bitToUCS2(pDestText, 1, pMsgText, 1, &pLangInfo), 0);
    EXPECT_EQ(textCvt->EscapeTurkishLockingToUCS2(pMsgText, 0, pLangInfo, result), 0);
    EXPECT_EQ(textCvt->EscapeTurkishLockingToUCS2(pMsgText, 1, pLangInfo, result), 0);
    EXPECT_EQ(textCvt->EscapePortuLockingToUCS2(pMsgText, 0, pLangInfo, result), 0);
    EXPECT_EQ(textCvt->EscapePortuLockingToUCS2(pMsgText, 1, pLangInfo, result), 0);
    EXPECT_EQ(textCvt->EscapeGSM7BitToUCS2(pMsgText, 0, pLangInfo, result), 0);
    EXPECT_EQ(textCvt->EscapeGSM7BitToUCS2(pMsgText, 1, pLangInfo, result), 0);
    pLangInfo.singleLang = MSG_ID_SPANISH_LANG;
    EXPECT_GT(textCvt->EscapeToUCS2(SRC_TEXT, pLangInfo), 0);
    pLangInfo.singleLang = MSG_ID_RESERVED_LANG;
    EXPECT_GT(textCvt->EscapeToUCS2(SRC_TEXT, pLangInfo), 0);
    pLangInfo.bSingleShift = false;
    EXPECT_GT(textCvt->EscapeToUCS2(SRC_TEXT, pLangInfo), 0);
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
    uint64_t iInteger = 0;
    uint32_t length = 1;
    std::string str = "qwe";
    bool isNoValue = true;
    EXPECT_FALSE(smsWapPushBuffer->GetOneByte(oneByte));
    EXPECT_FALSE(smsWapPushBuffer->IncreasePointer(1));
    EXPECT_FALSE(smsWapPushBuffer->DecreasePointer(1));
    smsWapPushBuffer->MarkPosition();
    smsWapPushBuffer->UnMarkPosition();
    EXPECT_FALSE(smsWapPushBuffer->DecodeShortLength(sValueLength));
    EXPECT_FALSE(smsWapPushBuffer->DecodeValueLengthReturnLen(valueLength, length));
    EXPECT_FALSE(smsWapPushBuffer->DecodeValueLength(valueLength));
    EXPECT_FALSE(smsWapPushBuffer->DecodeUintvar(valueLength, length));
    EXPECT_FALSE(smsWapPushBuffer->DecodeText(str, length));
    EXPECT_FALSE(smsWapPushBuffer->CharIsToken(MIN_TOKEN));
    EXPECT_FALSE(smsWapPushBuffer->CharIsToken(MAX_TOKEN));
    EXPECT_FALSE(smsWapPushBuffer->DecodeTokenText(str, length));
    EXPECT_FALSE(smsWapPushBuffer->DecodeShortInteger(oneByte));
    EXPECT_FALSE(smsWapPushBuffer->DecodeLongInteger(iInteger));
    EXPECT_FALSE(smsWapPushBuffer->DecodeExtensionMedia());
    EXPECT_FALSE(smsWapPushBuffer->DecodeConstrainedEncoding());
    EXPECT_FALSE(smsWapPushBuffer->DecodeTextValue(str, isNoValue));
    EXPECT_FALSE(smsWapPushBuffer->DecodeNoValue(isNoValue));
    EXPECT_TRUE(smsWapPushBuffer->ReadDataBuffer(1) == nullptr);
    EXPECT_TRUE(smsWapPushBuffer->ReadDataBuffer(1, 0) == nullptr);
    EXPECT_TRUE(smsWapPushBuffer->ReadDataBuffer(0, 0) == nullptr);
    EXPECT_FALSE(smsWapPushBuffer->WriteRawStringBuffer(str));
    EXPECT_FALSE(smsWapPushBuffer->WriteDataBuffer(nullptr, 1));
    smsWapPushBuffer->GetCurPosition();
    smsWapPushBuffer->GetSize();
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
    std::shared_ptr<vector<string>> pdus;
    smsReceiveHandler->DeleteMessageFormDb(indexer);
    smsReceiveHandler->CombineMessagePart(indexer);
    smsReceiveHandler->CheckBlockPhone(indexer);
    smsReceiveHandler->SendBroadcast(indexer, pdus);
    smsReceiveHandler->HandleReceivedSms(smsBaseMessage);
    EXPECT_FALSE(smsReceiveHandler->AddMsgToDB(indexer));
    indexer = std::make_shared<SmsReceiveIndexer>();
    smsReceiveHandler->CombineMessagePart(indexer);
    indexer->msgCount_ = 1;
    indexer->destPort_ = WAP_PUSH_PORT;
    smsReceiveHandler->CombineMessagePart(indexer);
    smsReceiveHandler->SendBroadcast(indexer, pdus);
    pdus = std::make_shared<vector<string>>();
    string pud = "qwe";
    pdus->push_back(pud);
    smsReceiveHandler->SendBroadcast(indexer, pdus);
    indexer->destPort_ = TEXT_PORT_NUM;
    smsReceiveHandler->SendBroadcast(indexer, pdus);
    smsReceiveHandler->AddMsgToDB(indexer);
    smsReceiveHandler->IsRepeatedMessagePart(indexer);
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
    long recvTime = 1;
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
    // smsCbMessage->cbHeader_->dcs.iso639Lang[0] = 1;
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
    EXPECT_FALSE(smsWapPushHandler->DecodeWapPushPdu(wapPdu));
    EXPECT_FALSE(smsWapPushHandler->DeocdeCheckIsBlock(wapPdu));
    wapPdu = "qwer";
    EXPECT_FALSE(smsWapPushHandler->DecodeWapPushPdu(wapPdu));
    EXPECT_FALSE(smsWapPushHandler->DecodePushType(decodeBuffer));
    EXPECT_FALSE(smsWapPushHandler->DeocdeCheckIsBlock(wapPdu));
    EXPECT_FALSE(smsWapPushHandler->DecodeXWapApplication(decodeBuffer, 1));
    EXPECT_FALSE(smsWapPushHandler->DecodeXWapAbandonHeaderValue(decodeBuffer));
    decodeBuffer.totolLength_ = 1;
    EXPECT_FALSE(smsWapPushHandler->DecodeXWapApplication(decodeBuffer, 1));
    EXPECT_TRUE(smsWapPushHandler->SendWapPushMessageBroadcast());
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
    std::u16string specification = u"3gpp";
    std::string str = "3gpp";
    Parcel parcel;
    EXPECT_TRUE(shortMessage->CreateMessage(pdu, specification) == nullptr);
    pdu.push_back(ShortMessage::SmsSimMessageStatus::SMS_SIM_MESSAGE_STATUS_READ);
    pdu.push_back(ShortMessage::SmsSimMessageStatus::SMS_SIM_MESSAGE_STATUS_FREE);
    EXPECT_EQ(shortMessage->CreateIccMessage(pdu, str, 1).indexOnSim_, 0);
    specification = u"3gpp2";
    str = "3gpp2";
    EXPECT_EQ(shortMessage->CreateIccMessage(pdu, str, 1).indexOnSim_, 0);
    pdu.clear();
    EXPECT_EQ(shortMessage->CreateIccMessage(pdu, str, 1).indexOnSim_, 0);
    pdu.push_back(ShortMessage::SmsSimMessageStatus::SMS_SIM_MESSAGE_STATUS_UNREAD);
    pdu.push_back(ShortMessage::SmsSimMessageStatus::SMS_SIM_MESSAGE_STATUS_FREE);
    EXPECT_EQ(shortMessage->CreateIccMessage(pdu, str, 1).indexOnSim_, 0);
    pdu.clear();
    pdu.push_back(ShortMessage::SmsSimMessageStatus::SMS_SIM_MESSAGE_STATUS_SENT);
    pdu.push_back(ShortMessage::SmsSimMessageStatus::SMS_SIM_MESSAGE_STATUS_FREE);
    EXPECT_EQ(shortMessage->CreateIccMessage(pdu, str, 1).indexOnSim_, 0);
    pdu.clear();
    pdu.push_back(ShortMessage::SmsSimMessageStatus::SMS_SIM_MESSAGE_STATUS_UNSENT);
    pdu.push_back(ShortMessage::SmsSimMessageStatus::SMS_SIM_MESSAGE_STATUS_FREE);
    EXPECT_EQ(shortMessage->CreateIccMessage(pdu, str, 1).indexOnSim_, 0);
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
 * @tc.name     Test SmsBaseMessage
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
 * @tc.name     Test SmsBaseMessage
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
} // namespace Telephony
} // namespace OHOS
