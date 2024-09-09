/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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

#include "cdma_sms_common.h"
#include "cdma_sms_sub_parameter.h"
#include "data_request.h"
#include "gtest/gtest.h"
#include "mms_apn_info.h"
#include "sms_pdu_buffer.h"
#include "telephony_errors.h"

namespace OHOS {
namespace Telephony {
namespace {
static constexpr int TEST_MAX_UD_HEADER_NUM = 8;
static constexpr int TEST_MAX_USER_DATA_LEN = 170;
static constexpr uint8_t TEST_MAX_FIELD_LEN = 170;
static constexpr uint8_t MAX_FIELD_LEN = 255;
static constexpr int TEST_DATA_LEN = 100;
static constexpr int TEST_SEND_CONF_MAX_SIZE = 600;
} // namespace
using namespace testing::ext;

class SmsServicesMmsGtest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void SmsServicesMmsGtest::TearDownTestCase() {}

void SmsServicesMmsGtest::SetUp() {}

void SmsServicesMmsGtest::TearDown() {}

void SmsServicesMmsGtest::SetUpTestCase() {}

HWTEST_F(SmsServicesMmsGtest, DataRequest_0001, Function | MediumTest | Level2)
{
    int32_t slotId = 0;
    auto dataRequest = std::make_shared<DataRequest>(slotId);
    std::string testStr = "";
    EXPECT_EQ(dataRequest->HttpRequest(slotId, testStr, nullptr, testStr, testStr, "ua", "uaprof"),
        TELEPHONY_ERR_LOCAL_PTR_NULL);
}

HWTEST_F(SmsServicesMmsGtest, MmsNetworkClient_0001, Function | MediumTest | Level2)
{
    int32_t slotId = 0;
    MmsNetworkClient client(0);
    std::string testStr = "testStr";
    auto mmsApnInfo = std::make_shared<MmsApnInfo>(slotId);
    mmsApnInfo->setMmscUrl("");
    EXPECT_EQ(client.PostUrl(testStr, testStr, "ua", "uaprof"), TELEPHONY_ERR_ARGUMENT_INVALID);

    client.responseData_ = std::string(TEST_SEND_CONF_MAX_SIZE, 'a');
    EXPECT_FALSE(client.CheckSendConf());\

    client.responseData_ = "";
    EXPECT_FALSE(client.CheckSendConf());

    client.responseData_ = "responseData";
    EXPECT_FALSE(client.CheckSendConf());
}

HWTEST_F(SmsServicesMmsGtest, MmsConnCallbackStub_0001, Function | MediumTest | Level2)
{
    MmsConnCallbackStub connCallback;
    sptr<NetManagerStandard::NetHandle> netHandle = new NetManagerStandard::NetHandle;
    EXPECT_EQ(connCallback.NetAvailable(netHandle), ERR_NONE);
}

HWTEST_F(SmsServicesMmsGtest, CdmaSmsMessageId_0001, Function | MediumTest | Level2)
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

    rBuffer.bitIndex_ = 0;
    rBuffer.index_ = 0;
    rBuffer.length_ = 0;
    EXPECT_FALSE(message2->Decode(rBuffer));
}

HWTEST_F(SmsServicesMmsGtest, CdmaSmsAbsoluteTime_0001, Function | MediumTest | Level2)
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

    rBuffer.bitIndex_ = 0;
    rBuffer.index_ = 0;
    rBuffer.length_ = 0;
    EXPECT_FALSE(message2->Decode(rBuffer));
}

HWTEST_F(SmsServicesMmsGtest, SmsTeleSvcUserData_0001, Function | MediumTest | Level2)
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

    message2->data_.userData.headerCnt = TEST_MAX_UD_HEADER_NUM;
    EXPECT_FALSE(message2->EncodeHeader7Bit(wBuffer));
    EXPECT_FALSE(message2->EncodeAscii7Bit(wBuffer));
    EXPECT_FALSE(message2->EncodeGsm7Bit(wBuffer));
    EXPECT_FALSE(message2->EncodeHeaderUnicode(wBuffer));
    EXPECT_FALSE(message2->EncodeUnicode(wBuffer));
}

HWTEST_F(SmsServicesMmsGtest, SmsTeleSvcUserData_0002, Function | MediumTest | Level2)
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

    message2->data_.userData.length = TEST_MAX_USER_DATA_LEN;
    EXPECT_FALSE(message2->Encode8BitData(wBuffer));

    rBuffer.bitIndex_ = 0;
    rBuffer.index_ = 0;
    rBuffer.length_ = 0;
    EXPECT_FALSE(message2->Decode(rBuffer));

    message2->headerInd_ = true;
    EXPECT_EQ(message2->DecodeHeader7Bit(rBuffer), 0);
    EXPECT_FALSE(message2->DecodeAscii7Bit(rBuffer, 0, BIT7));
    EXPECT_FALSE(message2->DecodeAscii7Bit(rBuffer, TEST_MAX_FIELD_LEN, 0));
    EXPECT_FALSE(message2->DecodeAscii7Bit(rBuffer, 1, 0));
}

HWTEST_F(SmsServicesMmsGtest, SmsTeleSvcUserData_0003, Function | MediumTest | Level2)
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

    rBuffer.bitIndex_ = 0;
    rBuffer.index_ = 0;
    rBuffer.length_ = 0;
    EXPECT_FALSE(message2->DecodeGsm7Bit(rBuffer, 0, BIT7));
    EXPECT_FALSE(message2->DecodeGsm7Bit(rBuffer, MAX_FIELD_LEN, 0));

    message2->data_.userData.length = TEST_MAX_USER_DATA_LEN;
    EXPECT_FALSE(message2->Decode8BitData(rBuffer));
}

HWTEST_F(SmsServicesMmsGtest, CdmaSmsCmasData_0001, Function | MediumTest | Level2)
{
    SmsTeleSvcCmasData v1;
    std::stringstream ss;
    ss.clear();
    ss << static_cast<uint8_t>(CdmaSmsSubParameter::USER_DATA);
    uint8_t len = TEST_DATA_LEN;
    ss << static_cast<uint8_t>(len);
    for (uint8_t i = 0; i < len; i++) {
        ss << static_cast<uint8_t>(0x00);
    }
    SmsReadBuffer rBuffer(ss.str());
    auto message1 = std::make_shared<CdmaSmsCmasData>(v1);
    rBuffer.bitIndex_ = 0;
    rBuffer.index_ = 0;
    rBuffer.length_ = 0;
    EXPECT_FALSE(message1->DecodeData(rBuffer));
    EXPECT_FALSE(message1->DecodeType0Data(rBuffer));
    EXPECT_FALSE(message1->DecodeType1Data(rBuffer));
    EXPECT_FALSE(message1->DecodeType2Data(rBuffer));
    EXPECT_FALSE(message1->DecodeAbsTime(rBuffer));
}
} // namespace Telephony
} // namespace OHOS