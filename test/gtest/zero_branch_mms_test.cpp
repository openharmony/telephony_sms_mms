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

#include "gtest/gtest.h"
#include "mms_body.h"
#include "mms_body_part.h"
#include "mms_content_type.h"
#include "mms_encode_buffer.h"
#include "mms_header.h"
#include "mms_msg.h"
#include "sms_service.h"
#include "sms_wap_push_buffer.h"
#include "sms_wap_push_content_type.h"

namespace OHOS {
namespace Telephony {
using namespace testing::ext;

namespace {
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
const uint8_t MIN_TOKEN = 30;
const uint8_t MAX_TOKEN = 127;
const uint8_t FIELD_ID = 2;
const int32_t INVALID_SLOTID = 2;
const int32_t VALUE_LENGTH = 2;
static constexpr uint32_t MAX_MMS_MSG_PART_LEN = 300 * 1024;
const uint32_t CODE_BUFFER_MAX_SIZE = 300 * 1024;
} // namespace

class BranchMmsTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};
void BranchMmsTest::SetUpTestCase() {}

void BranchMmsTest::TearDownTestCase() {}

void BranchMmsTest::SetUp() {}

void BranchMmsTest::TearDown() {}

/**
 * @tc.number   Telephony_SmsMmsGtest_MmsHeader_0001
 * @tc.name     Test MmsHeader
 * @tc.desc     Function test
 */
HWTEST_F(BranchMmsTest, MmsHeader_0001, Function | MediumTest | Level1)
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
HWTEST_F(BranchMmsTest, MmsHeader_0002, Function | MediumTest | Level1)
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
HWTEST_F(BranchMmsTest, MmsHeader_0003, Function | MediumTest | Level1)
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
HWTEST_F(BranchMmsTest, MmsHeader_0004, Function | MediumTest | Level1)
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
HWTEST_F(BranchMmsTest, MmsHeader_0005, Function | MediumTest | Level1)
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
HWTEST_F(BranchMmsTest, SmsWapPushBuffer_0001, Function | MediumTest | Level1)
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
    smsWapPushBuffer->pduBuffer_[0] = PDU_BUFFE_LENGTH;
    EXPECT_TRUE(smsWapPushBuffer->DecodeTokenText(str, length));
}

/**
 * @tc.number   Telephony_SmsMmsGtest_SmsWapPushBuffer_0002
 * @tc.name     Test SmsWapPushBuffer
 * @tc.desc     Function test
 */
HWTEST_F(BranchMmsTest, SmsWapPushBuffer_0002, Function | MediumTest | Level1)
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
 * @tc.number   Telephony_SmsMmsGtest_SmsWapPushHandler_0001
 * @tc.name     Test SmsWapPushHandler
 * @tc.desc     Function test
 */
HWTEST_F(BranchMmsTest, SmsWapPushHandler_0001, Function | MediumTest | Level1)
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
 * @tc.number   Telephony_SmsMmsGtest_SmsWapPushContentType_0001
 * @tc.name     Test SmsWapPushContentType
 * @tc.desc     Function test
 */
HWTEST_F(BranchMmsTest, SmsWapPushContentType_0001, Function | MediumTest | Level1)
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
}

/**
 * @tc.number   Telephony_SmsMmsGtest_SmsWapPushContentType_0002
 * @tc.name     Test SmsWapPushContentType
 * @tc.desc     Function test
 */
HWTEST_F(BranchMmsTest, SmsWapPushContentType_0002, Function | MediumTest | Level1)
{
    auto smsWapPushContentType = std::make_shared<SmsWapPushContentType>();
    SmsWapPushBuffer decodeBuffer;
    decodeBuffer.totolLength_ = 1;
    decodeBuffer.pduBuffer_ = std::make_unique<char[]>(1);
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
 * @tc.number   Telephony_SmsMmsGtest_SmsWapPushContentType_0003
 * @tc.name     Test SmsWapPushContentType
 * @tc.desc     Function test
 */
HWTEST_F(BranchMmsTest, SmsWapPushContentType_0003, Function | MediumTest | Level1)
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
 * @tc.number   Telephony_SmsMmsGtest_MmsBodyPart_0001
 * @tc.name     Test MmsBodyPart
 * @tc.desc     Function test
 */
HWTEST_F(BranchMmsTest, MmsBodyPart_0001, Function | MediumTest | Level3)
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
HWTEST_F(BranchMmsTest, MmsBodyPart_0002, Function | MediumTest | Level3)
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
HWTEST_F(BranchMmsTest, MmsContentType_0001, Function | MediumTest | Level3)
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
HWTEST_F(BranchMmsTest, MmsContentType_0002, Function | MediumTest | Level3)
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
HWTEST_F(BranchMmsTest, MmsContentType_0003, Function | MediumTest | Level3)
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
HWTEST_F(BranchMmsTest, MmsContentType_0004, Function | MediumTest | Level3)
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

 * @tc.number   Telephony_SmsMmsGtest_MmsBody_0001
 * @tc.name     Test MmsBody
 * @tc.desc     Function test
 */
HWTEST_F(BranchMmsTest, SmsMmsGtest_MmsBody_0001, Function | MediumTest | Level1)
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
HWTEST_F(BranchMmsTest, SmsMmsGtest_MmsMsg_0001, Function | MediumTest | Level1)
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
 * @tc.number   Telephony_SmsMmsGtest_MmsBodyPartHeader_0001
 * @tc.name     Test SmsMiscManager
 * @tc.desc     Function test
 */
HWTEST_F(BranchMmsTest, MmsBodyPartHeader_0001, Function | MediumTest | Level1)
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
} // namespace Telephony
} // namespace OHOS