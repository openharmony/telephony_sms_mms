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

#include "cdma_sms_transport_message.h"
#include "core_service_client.h"
#include "gtest/gtest.h"
#include "i_sms_service_interface.h"
#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "mms_address.h"
#include "mms_attachment.h"
#include "mms_base64.h"
#include "mms_body.h"
#include "mms_body_part.h"
#include "mms_body_part_header.h"
#include "mms_charset.h"
#include "mms_codec_type.h"
#include "mms_decode_buffer.h"
#include "mms_header.h"
#include "mms_msg.h"
#include "mms_quoted_printable.h"
#include "radio_event.h"
#include "sms_broadcast_subscriber_gtest.h"
#include "sms_mms_gtest.h"
#include "sms_mms_test_helper.h"
#include "sms_service.h"
#include "sms_service_manager_client.h"
#include "string_utils.h"
#include "telephony_log_wrapper.h"
#include "telephony_types.h"

namespace OHOS {
namespace Telephony {
namespace {
sptr<ISmsServiceInterface> g_telephonyService = nullptr;
} // namespace
using namespace testing::ext;

class MmsGtest : public testing::Test {
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

void MmsGtest::TearDownTestCase() {}

void MmsGtest::SetUp() {}

void MmsGtest::TearDown() {}

const int32_t DEFAULT_SIM_SLOT_ID_1 = 1;
const uint16_t MESSAGE_TYPE = 4;
const uint16_t WAPPUSH_PDU_LEN = 164;
const uint32_t MAX_MMS_ATTACHMENT_LEN = 300 * 1024;
const char PDU_TEST_DATA = 35;
const char MIN_SHORT_OCTET = 30;
const char QUOTE_CHAR_LEN = 0X7F - 1;
const char VALUE_CHAR = 8;
const char PDU_COUNT = 10;
const char PDU_CUSITION = 5;
const char PDU_LENGTH = 4;
const char *ENCODE_CHARSET = "01010101";
const uint32_t ENCODE_CHARSET1 = 0x0100;
const uint32_t ENCODE_CHARSET2 = 0x0001;

void MmsGtest::SetUpTestCase()
{
    TELEPHONY_LOGI("SetUpTestCase slotId%{public}d", DEFAULT_SIM_SLOT_ID_1);
    g_telephonyService = GetProxy();
    if (g_telephonyService == nullptr) {
        return;
    }
    DelayedSingleton<SmsServiceManagerClient>::GetInstance()->ResetSmsServiceProxy();
    DelayedSingleton<SmsServiceManagerClient>::GetInstance()->InitSmsServiceProxy();
}

sptr<ISmsServiceInterface> MmsGtest::GetProxy()
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
void ReceiveWapPushTestFunc(SmsMmsTestHelper &helper)
{
    std::shared_ptr<AppExecFwk::EventRunner> runner = AppExecFwk::EventRunner::Create("test");
    auto smsReceiveHandler = std::make_shared<GsmSmsReceiveHandler>(runner, helper.slotId);
    auto message = std::make_shared<SmsMessageInfo>();
    message->indicationType = MESSAGE_TYPE;
    message->size = WAPPUSH_PDU_LEN;
    message->pdu = StringUtils::HexToByteVector(
        "0891683110205005F0640BA10156455102F1000432109261715023880605040B8423F04C06246170706C69636174696F6E2F766E642E77"
        "61702E6D6D732D6D65737361676500B487AF848C829850765030303031365A645430008D9089178031363630373532313930382F545950"
        "453D504C4D4E008A808E040001298D"
        "8805810303F47B83687474703A2F2F31302E3132332E31382E38303A3138302F76564F455F3000");

    AppExecFwk::InnerEvent::Pointer event = AppExecFwk::InnerEvent::Get(RadioEvent::RADIO_GSM_SMS, message);
    smsReceiveHandler->ProcessEvent(event);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_Receive_Wap_Push_0001
 * @tc.name     Receive a Wap Push
 * @tc.desc     Function test
 */
HWTEST_F(MmsGtest, Receive_Wap_Push_0001, Function | MediumTest | Level2)
{
    AccessMmsToken token;
    TELEPHONY_LOGI("TelSMSMMSTest::Receive_Wap_Push_0001 -->");
    int32_t slotId = DEFAULT_SIM_SLOT_ID;
    if (!(MmsGtest::HasSimCard(slotId))) {
        TELEPHONY_LOGI("TelephonyTestService has no sim card");
        ASSERT_TRUE(true);
        return;
    }
    SmsMmsTestHelper helper;
    helper.slotId = slotId;

    EventFwk::MatchingSkills matchingSkills;
    matchingSkills.AddEvent(EventFwk::CommonEventSupport::COMMON_EVENT_SMS_WAPPUSH_RECEIVE_COMPLETED);
    EventFwk::CommonEventSubscribeInfo subscriberInfo(matchingSkills);
    std::shared_ptr<SmsBroadcastSubscriberGtest> subscriberTest =
        std::make_shared<SmsBroadcastSubscriberGtest>(subscriberInfo, helper);
    if (subscriberTest == nullptr) {
        ASSERT_TRUE(false);
        return;
    }
    bool subscribeResult = EventFwk::CommonEventManager::SubscribeCommonEvent(subscriberTest);
    TELEPHONY_LOGI("subscribeResult is : %{public}d", subscribeResult);

    if (!helper.Run(ReceiveWapPushTestFunc, helper)) {
        TELEPHONY_LOGI("ReceiveWapPushTestFunc out of time");
        ASSERT_TRUE(true);
        return;
    }
    TELEPHONY_LOGI("TelSMSMMSTest::Receive_Wap_Push_0001 -->finished");
    EXPECT_TRUE(helper.GetBoolResult());
}

/**
 * @tc.number   Telephony_SmsMmsGtest_Receive_Wap_Push_0002
 * @tc.name     Receive a Wap Push
 * @tc.desc     Function test
 */
HWTEST_F(MmsGtest, Receive_Wap_Push_0002, Function | MediumTest | Level2)
{
    AccessMmsToken token;
    TELEPHONY_LOGI("TelSMSMMSTest::Receive_Wap_Push_0002 -->");
    int32_t slotId = DEFAULT_SIM_SLOT_ID_1;
    if (!(MmsGtest::HasSimCard(slotId))) {
        TELEPHONY_LOGI("TelephonyTestService has no sim card");
        ASSERT_TRUE(true);
        return;
    }
    SmsMmsTestHelper helper;
    helper.slotId = slotId;

    EventFwk::MatchingSkills matchingSkills;
    matchingSkills.AddEvent(EventFwk::CommonEventSupport::COMMON_EVENT_SMS_WAPPUSH_RECEIVE_COMPLETED);
    EventFwk::CommonEventSubscribeInfo subscriberInfo(matchingSkills);
    std::shared_ptr<SmsBroadcastSubscriberGtest> subscriberTest =
        std::make_shared<SmsBroadcastSubscriberGtest>(subscriberInfo, helper);
    if (subscriberTest == nullptr) {
        ASSERT_TRUE(false);
        return;
    }
    bool subscribeResult = EventFwk::CommonEventManager::SubscribeCommonEvent(subscriberTest);
    TELEPHONY_LOGI("subscribeResult is : %{public}d", subscribeResult);

    if (!helper.Run(ReceiveWapPushTestFunc, helper)) {
        TELEPHONY_LOGI("ReceiveWapPushTestFunc out of time");
        ASSERT_TRUE(true);
        return;
    }
    TELEPHONY_LOGI("TelSMSMMSTest::Receive_Wap_Push_0002 -->finished");
    EXPECT_TRUE(helper.GetBoolResult());
}

/**
 * @tc.number   Telephony_SmsMmsGtest_MmsAddress_0001
 * @tc.name     Test MmsAddress
 * @tc.desc     Function test
 */
HWTEST_F(MmsGtest, MmsAddress_0001, Function | MediumTest | Level1)
{
    TELEPHONY_LOGI("TelSMSMMSTest::MmsAddress_0001 -->");
    MmsAddress address;
    address.SetMmsAddressString("12345678/TYPE=PLMN");
    address.SetMmsAddressString("12345678/TYPE=IPv4");
    address.SetMmsAddressString("12345678/TYPE=IPv6");
    address.SetMmsAddressString("12345678/TYPE=UNKNOWN");
    address.SetMmsAddressString("12345678/TYPE=EMAIL");
    std::string ret = address.GetAddressString();
    EXPECT_STREQ(ret.c_str(), "12345678/TYPE=EMAIL");
    MmsAddress::MmsAddressType type = address.GetAddressType();
    EXPECT_EQ(type, MmsAddress::MmsAddressType::ADDRESS_TYPE_PLMN);
    MmsCharSets charset = address.GetAddressCharset();
    EXPECT_EQ(charset, MmsCharSets::UTF_8);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_MmsAttachment_0001
 * @tc.name     Test MmsAttachment
 * @tc.desc     Function test
 */
HWTEST_F(MmsGtest, MmsAttachment_0001, Function | MediumTest | Level1)
{
    TELEPHONY_LOGI("TelSMSMMSTest::MmsAttachment_0001 -->");
    const std::string pathName = "/data/app/enSrc/618C0A89.smil";
    std::size_t pos = pathName.find_last_of('/');
    std::string fileName(pathName.substr(pos + 1));
    MmsAttachment attachment;
    bool retBool;
    std::string retStr;
    retBool = attachment.SetAttachmentFilePath("", false);
    EXPECT_EQ(false, retBool);
    retBool = attachment.SetAttachmentFilePath(pathName, true);
    EXPECT_EQ(true, retBool);
    retStr = attachment.GetAttachmentFilePath();
    EXPECT_STREQ(retStr.c_str(), pathName.c_str());
    retBool = attachment.SetContentId("");
    EXPECT_EQ(false, retBool);
    retBool = attachment.SetContentId("0000");
    EXPECT_EQ(true, retBool);
    retBool = attachment.SetContentId("<0000>");
    EXPECT_EQ(true, retBool);
    retStr = attachment.GetContentId();
    EXPECT_STREQ(retStr.c_str(), "<0000>");
    retBool = attachment.SetContentLocation("");
    EXPECT_EQ(false, retBool);
    retBool = attachment.SetContentLocation("SetContentLocation");
    EXPECT_EQ(true, retBool);
    retStr = attachment.GetContentLocation();
    EXPECT_STREQ(retStr.c_str(), "SetContentLocation");
    retBool = attachment.SetContentDisposition("");
    EXPECT_EQ(false, retBool);
    retBool = attachment.SetContentDisposition("attachment");
    EXPECT_EQ(true, retBool);
    retStr = attachment.GetContentDisposition();
    EXPECT_STREQ(retStr.c_str(), "attachment");
    retBool = attachment.SetContentTransferEncoding("");
    EXPECT_EQ(false, retBool);
    retBool = attachment.SetFileName(fileName);
    EXPECT_EQ(true, retBool);
    retStr = attachment.GetFileName();
    EXPECT_STREQ(retStr.c_str(), fileName.c_str());
    attachment.strFileName_ = "";
    retStr = attachment.GetFileName();
    EXPECT_STRNE(retStr.c_str(), fileName.c_str());
}

/**
 * @tc.number   Telephony_SmsMmsGtest_MmsAttachment_0002
 * @tc.name     Test MmsAttachment
 * @tc.desc     Function test
 */
HWTEST_F(MmsGtest, MmsAttachment_0002, Function | MediumTest | Level1)
{
    TELEPHONY_LOGI("TelSMSMMSTest::MmsAttachment_0002 -->");
    MmsAttachment attachment;
    bool retBool;
    std::string retStr;
    uint32_t retU32t;
    uint32_t len = 300 * 1024;
    uint32_t charset = 0;
    retBool = attachment.SetContentTransferEncoding("SetContentTransferEncoding");
    EXPECT_EQ(true, retBool);
    retStr = attachment.GetContentTransferEncoding();
    EXPECT_STREQ(retStr.c_str(), "SetContentTransferEncoding");
    retBool = attachment.SetContentType("");
    EXPECT_EQ(false, retBool);
    attachment.SetIsSmilFile(true);
    retBool = attachment.IsSmilFile();
    EXPECT_EQ(true, retBool);
    attachment.SetCharSet(charset);
    retU32t = attachment.GetCharSet();
    EXPECT_EQ(charset, retU32t);
    retBool = attachment.SetDataBuffer(nullptr, 0);
    retBool = attachment.SetDataBuffer(nullptr, MAX_MMS_ATTACHMENT_LEN + 1);
    EXPECT_EQ(false, retBool);
    retBool = attachment.SetDataBuffer(std::make_unique<char[]>(len + 1), len + 1);
    EXPECT_EQ(false, retBool);
    retBool = attachment.SetDataBuffer(std::make_unique<char[]>(len - 1), len - 1);
    EXPECT_EQ(true, retBool);
    retBool = attachment.SetDataBuffer(std::make_unique<char[]>(len - 1), len + 1);
    EXPECT_EQ(false, retBool);
    EXPECT_FALSE(attachment.GetDataBuffer(len) == nullptr);
    attachment.dataLength_ = MAX_MMS_ATTACHMENT_LEN + 1;
    EXPECT_TRUE(attachment.GetDataBuffer(len) == nullptr);
    MmsAttachment attachment1(attachment);
    attachment1.SetContentType("application/smil");
    retStr = attachment1.GetContentType();
    EXPECT_STREQ(retStr.c_str(), "application/smil");
}

/**
 * @tc.number   Telephony_SmsMmsGtest_MmsBodyPartHeader_0001
 * @tc.name     Test MmsBodyPartHeader
 * @tc.desc     Function test
 */
HWTEST_F(MmsGtest, MmsBodyPartHeader_0001, Function | MediumTest | Level1)
{
    TELEPHONY_LOGI("TelSMSMMSTest::MmsBodyPartHeader_0001 -->");
    MmsBodyPartHeader mmsBodyPartHeader;
    MmsDecodeBuffer decodeBuffer;
    std::string testStr;
    uint32_t len = 0;
    uint32_t lenErr = -1;
    uint32_t lenMax = 300 * 1024;
    mmsBodyPartHeader.DumpBodyPartHeader();
    mmsBodyPartHeader.DecodeContentLocation(decodeBuffer, len);
    mmsBodyPartHeader.DecodeContentId(decodeBuffer, len);
    mmsBodyPartHeader.DecodeContentDisposition(decodeBuffer, len);
    mmsBodyPartHeader.DecodeDispositionParameter(decodeBuffer, lenMax, len);
    mmsBodyPartHeader.DecodeDispositionParameter(decodeBuffer, lenErr, len);
    mmsBodyPartHeader.DecodeWellKnownHeader(decodeBuffer, len);
    mmsBodyPartHeader.DecodeApplicationHeader(decodeBuffer, len);
    mmsBodyPartHeader.SetContentId("contentId");
    mmsBodyPartHeader.GetContentId(testStr);
    EXPECT_STREQ(testStr.c_str(), "contentId");
    mmsBodyPartHeader.SetContentTransferEncoding("contentTransferEncoding");
    mmsBodyPartHeader.GetContentTransferEncoding(testStr);
    EXPECT_STREQ(testStr.c_str(), "contentTransferEncoding");
    mmsBodyPartHeader.SetContentLocation("contentLocation");
    mmsBodyPartHeader.GetContentLocation(testStr);
    EXPECT_STREQ(testStr.c_str(), "contentLocation");
    MmsEncodeBuffer encodeBuffer;
    mmsBodyPartHeader.EncodeContentLocation(encodeBuffer);
    mmsBodyPartHeader.EncodeContentId(encodeBuffer);
    mmsBodyPartHeader.EncodeContentDisposition(encodeBuffer);
    mmsBodyPartHeader.EncodeContentTransferEncoding(encodeBuffer);
    mmsBodyPartHeader.EncodeMmsBodyPartHeader(encodeBuffer);
    MmsBodyPartHeader mmsBodyPartHeader2;
    MmsBodyPartHeader mmsBodyPartHeader3 = MmsBodyPartHeader(mmsBodyPartHeader);
    mmsBodyPartHeader2 = mmsBodyPartHeader;
    mmsBodyPartHeader2.SetContentDisposition("contentDisposition");
    mmsBodyPartHeader2.GetContentDisposition(testStr);
    EXPECT_STREQ(testStr.c_str(), "contentDisposition");
}

/**
 * @tc.number   Telephony_SmsMmsGtest_MmsBuffer_0001
 * @tc.name     Test MmsBuffer
 * @tc.desc     Function test
 */
HWTEST_F(MmsGtest, MmsBuffer_0001, Function | MediumTest | Level1)
{
    TELEPHONY_LOGI("TelSMSMMSTest::MmsBuffer_0001 -->");
    MmsBuffer mmsBuffer;
    uint32_t len = 10;
    bool retBool;
    std::string strPathName = "/data/app/enSrc/618C0A89.smil";
    mmsBuffer.ReadDataBuffer(len);
    mmsBuffer.ReadDataBuffer(len, len);
    retBool = mmsBuffer.WriteDataBuffer(std::make_unique<char[]>(len), 0);
    EXPECT_EQ(false, retBool);
    retBool = mmsBuffer.WriteDataBuffer(std::make_unique<char[]>(len), len);
    EXPECT_EQ(true, retBool);
    mmsBuffer.WriteBufferFromFile(strPathName);
    mmsBuffer.GetCurPosition();
    uint32_t ret = mmsBuffer.GetSize();
    EXPECT_GE(ret, 0);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_MmsContentParam_0001
 * @tc.name     Test MmsContentParam
 * @tc.desc     Function test
 */
HWTEST_F(MmsGtest, MmsContentParam_0001, Function | MediumTest | Level1)
{
    TELEPHONY_LOGI("TelSMSMMSTest::MmsContentParam_0001 -->");
    MmsContentParam mmsContentParam;
    MmsContentParam mmsContentParam2;
    uint8_t field = 1;
    uint32_t charset = 10;
    std::string testStr;
    uint32_t retU32t;
    mmsContentParam.DumpContentParam();
    mmsContentParam.SetCharSet(charset);
    retU32t = mmsContentParam.GetCharSet();
    EXPECT_EQ(charset, retU32t);
    mmsContentParam.SetType("type");
    testStr = mmsContentParam.GetType();
    EXPECT_STREQ(testStr.c_str(), "type");
    mmsContentParam.SetFileName("");
    mmsContentParam.SetStart("");
    mmsContentParam.SetStart("start");
    mmsContentParam.GetStart(testStr);
    EXPECT_STREQ(testStr.c_str(), "start");
    mmsContentParam.AddNormalField(field, "value");
    mmsContentParam.GetNormalField(field, testStr);
    EXPECT_STREQ(testStr.c_str(), "value");
    mmsContentParam.GetParamMap();
    mmsContentParam2 = mmsContentParam;
    mmsContentParam2.SetFileName("fileName");
    mmsContentParam2.GetFileName(testStr);
    EXPECT_STREQ(testStr.c_str(), "fileName");

    mmsContentParam.textMap_.clear();
    mmsContentParam.AddNormalField(field, "value");
    mmsContentParam.textMap_.clear();
    mmsContentParam.GetNormalField(field, testStr);
    mmsContentParam.GetStart(testStr);
    mmsContentParam2.GetFileName(testStr);
    mmsContentParam.DumpContentParam();
    EXPECT_STREQ(testStr.c_str(), "fileName");
}

/**
 * @tc.number   Telephony_SmsMmsGtest_MmsContentType_0001
 * @tc.name     Test MmsContentType
 * @tc.desc     Function test
 */
HWTEST_F(MmsGtest, MmsContentType_0001, Function | MediumTest | Level1)
{
    TELEPHONY_LOGI("TelSMSMMSTest::MmsContentType_0001 -->");
    MmsContentType mmsContentType;
    MmsDecodeBuffer decodeBuffer;
    MmsContentParam contentParam;
    int32_t len = 10;
    uint8_t type = 10;
    std::string testStr;
    uint8_t count = 10;
    uint8_t index = 0;
    while (index < count) {
        decodeBuffer.pduBuffer_[index] = '1';
        index++;
    }
    mmsContentType.DumpMmsContentType();

    decodeBuffer.curPosition_ = 0;
    decodeBuffer.pduBuffer_[decodeBuffer.curPosition_++] = PDU_TEST_DATA;
    decodeBuffer.pduBuffer_[decodeBuffer.curPosition_++] = PDU_TEST_DATA;
    decodeBuffer.pduBuffer_[decodeBuffer.curPosition_] = PDU_TEST_DATA;
    mmsContentType.DecodeMmsContentType(decodeBuffer, len);

    decodeBuffer.curPosition_ = 0;
    mmsContentType.DecodeMmsCTGeneralForm(decodeBuffer, len);
    mmsContentType.GetContentTypeFromInt(type);
    mmsContentType.GetContentTypeFromString("");

    decodeBuffer.curPosition_ = 0;
    mmsContentType.DecodeParameter(decodeBuffer, len);
    mmsContentType.SetContentParam(contentParam);
    mmsContentType.DecodeTextField(decodeBuffer, type, len);

    decodeBuffer.curPosition_ = 0;
    mmsContentType.DecodeCharsetField(decodeBuffer, len);
    mmsContentType.DecodeTypeField(decodeBuffer, len);

    MmsEncodeBuffer encodeBuffer;
    mmsContentType.EncodeTextField(encodeBuffer);
    mmsContentType.EncodeCharsetField(encodeBuffer);
    mmsContentType.EncodeTypeField(encodeBuffer);
    mmsContentType.EncodeMmsBodyPartContentParam(encodeBuffer);
    mmsContentType.EncodeMmsBodyPartContentType(encodeBuffer);
    mmsContentType.GetContentParam();
    MmsContentType mmsContentType2(mmsContentType);
    mmsContentType2 = mmsContentType;
    mmsContentType2.SetContentType("contentType");
    mmsContentType2.GetContentType(testStr);
    EXPECT_STREQ(testStr.c_str(), "contentType");
}

/**
 * @tc.number   Telephony_SmsMmsGtest_MmsDecodeBuffer_0001
 * @tc.name     Test MmsDecodeBuffer
 * @tc.desc     Function test
 */
HWTEST_F(MmsGtest, MmsDecodeBuffer_0001, Function | MediumTest | Level1)
{
    TELEPHONY_LOGI("TelSMSMMSTest::MmsDecodeBuffer_0001 -->");
    uint8_t byteVar = 1;
    uint32_t intVar = PDU_COUNT;
    std::string testStr;
    MmsDecodeBuffer mmsDecodeBuffer;
    uint8_t count = PDU_COUNT;
    uint8_t index = 0;
    while (index < count) {
        mmsDecodeBuffer.pduBuffer_[index] = '1';
        index++;
    }

    mmsDecodeBuffer.PeekOneByte(byteVar);
    mmsDecodeBuffer.GetOneByte(byteVar);
    mmsDecodeBuffer.IncreasePointer(intVar);
    mmsDecodeBuffer.DecreasePointer(intVar);
    mmsDecodeBuffer.curPosition_ = PDU_CUSITION;
    mmsDecodeBuffer.totolLength_ = PDU_LENGTH;
    mmsDecodeBuffer.PeekOneByte(byteVar);
    mmsDecodeBuffer.GetOneByte(byteVar);
    mmsDecodeBuffer.IncreasePointer(intVar);
    mmsDecodeBuffer.DecreasePointer(intVar);

    mmsDecodeBuffer.curPosition_ = 0;
    mmsDecodeBuffer.DecodeUintvar(intVar, intVar);
    mmsDecodeBuffer.DecodeShortLength(byteVar);

    char testChar = MIN_SHORT_OCTET + 1;
    mmsDecodeBuffer.pduBuffer_[mmsDecodeBuffer.curPosition_] = testChar;
    mmsDecodeBuffer.DecodeShortLength(byteVar);
    mmsDecodeBuffer.curPosition_ = 0;
    mmsDecodeBuffer.pduBuffer_[mmsDecodeBuffer.curPosition_] = MIN_SHORT_OCTET;
    mmsDecodeBuffer.DecodeValueLengthReturnLen(intVar, intVar);
    mmsDecodeBuffer.pduBuffer_[mmsDecodeBuffer.curPosition_] = testChar;
    mmsDecodeBuffer.DecodeValueLengthReturnLen(intVar, intVar);
    mmsDecodeBuffer.pduBuffer_[mmsDecodeBuffer.curPosition_] = testChar + 1;
    mmsDecodeBuffer.DecodeValueLengthReturnLen(intVar, intVar);

    uint8_t errVar = -1;
    bool ret = mmsDecodeBuffer.CharIsToken(errVar);
    EXPECT_EQ(false, ret);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_MmsDecodeBuffer_0002
 * @tc.name     Test MmsDecodeBuffer
 * @tc.desc     Function test
 */
HWTEST_F(MmsGtest, MmsDecodeBuffer_0002, Function | MediumTest | Level1)
{
    TELEPHONY_LOGI("TelSMSMMSTest::MmsDecodeBuffer_0002 -->");
    uint32_t intVar = 10;
    std::string testStr;
    MmsDecodeBuffer mmsDecodeBuffer;
    uint8_t count = 10;
    uint8_t index = 0;
    while (index < count) {
        mmsDecodeBuffer.pduBuffer_[index] = '1';
        index++;
    }
    mmsDecodeBuffer.curPosition_ = 0;
    char testChar = MIN_SHORT_OCTET + 1;
    mmsDecodeBuffer.pduBuffer_[mmsDecodeBuffer.curPosition_] = MIN_SHORT_OCTET;
    mmsDecodeBuffer.DecodeValueLength(intVar);
    mmsDecodeBuffer.pduBuffer_[mmsDecodeBuffer.curPosition_] = testChar;
    mmsDecodeBuffer.DecodeValueLength(intVar);
    mmsDecodeBuffer.pduBuffer_[mmsDecodeBuffer.curPosition_] = testChar + 1;
    mmsDecodeBuffer.DecodeValueLength(intVar);

    mmsDecodeBuffer.curPosition_ = 0;
    testChar += 1;
    mmsDecodeBuffer.pduBuffer_[mmsDecodeBuffer.curPosition_] = testChar;
    mmsDecodeBuffer.DecodeTokenText(testStr, intVar);
    mmsDecodeBuffer.pduBuffer_[mmsDecodeBuffer.curPosition_] = testChar + 1;
    mmsDecodeBuffer.DecodeTokenText(testStr, intVar);
    mmsDecodeBuffer.pduBuffer_[mmsDecodeBuffer.curPosition_] = QUOTE_CHAR_LEN;
    mmsDecodeBuffer.DecodeTokenText(testStr, intVar);

    mmsDecodeBuffer.curPosition_ = 0;
    mmsDecodeBuffer.pduBuffer_[mmsDecodeBuffer.curPosition_] = QUOTE_CHAR_LEN - 1;
    mmsDecodeBuffer.DecodeText(testStr, intVar);
    mmsDecodeBuffer.pduBuffer_[mmsDecodeBuffer.curPosition_] = QUOTE_CHAR_LEN;
    mmsDecodeBuffer.DecodeText(testStr, intVar);

    mmsDecodeBuffer.curPosition_ = 0;
    testChar += 1;
    mmsDecodeBuffer.pduBuffer_[mmsDecodeBuffer.curPosition_] = testChar;
    mmsDecodeBuffer.DecodeQuotedText(testStr, intVar);
    mmsDecodeBuffer.pduBuffer_[mmsDecodeBuffer.curPosition_] = testChar + 1;
    mmsDecodeBuffer.DecodeQuotedText(testStr, intVar);

    uint8_t errVar = -1;
    bool ret = mmsDecodeBuffer.CharIsToken(errVar);
    EXPECT_EQ(false, ret);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_MmsDecodeBuffer_0003
 * @tc.name     Test MmsDecodeBuffer
 * @tc.desc     Function test
 */
HWTEST_F(MmsGtest, MmsDecodeBuffer_0003, Function | MediumTest | Level1)
{
    TELEPHONY_LOGI("TelSMSMMSTest::MmsDecodeBuffer_0003 -->");
    uint8_t byteVar = 1;
    uint64_t longVar = 10;
    std::string testStr;
    MmsDecodeBuffer mmsDecodeBuffer;
    uint8_t count = 10;
    uint8_t index = 0;
    while (index < count) {
        mmsDecodeBuffer.pduBuffer_[index] = '1';
        index++;
    }

    mmsDecodeBuffer.curPosition_ = 0;
    char testChar = QUOTE_CHAR_LEN + 1;
    mmsDecodeBuffer.pduBuffer_[mmsDecodeBuffer.curPosition_] = testChar;
    mmsDecodeBuffer.DecodeShortInteger(byteVar);
    mmsDecodeBuffer.pduBuffer_[mmsDecodeBuffer.curPosition_] = testChar + 1;
    mmsDecodeBuffer.DecodeShortInteger(byteVar);

    mmsDecodeBuffer.curPosition_ = 0;
    mmsDecodeBuffer.pduBuffer_[mmsDecodeBuffer.curPosition_] = VALUE_CHAR;
    mmsDecodeBuffer.DecodeLongInteger(longVar);
    mmsDecodeBuffer.pduBuffer_[mmsDecodeBuffer.curPosition_] = VALUE_CHAR + 1;
    mmsDecodeBuffer.DecodeLongInteger(longVar);

    mmsDecodeBuffer.curPosition_ = 0;
    mmsDecodeBuffer.DecodeInteger(longVar);
    mmsDecodeBuffer.curPosition_ = 0;
    mmsDecodeBuffer.pduBuffer_[mmsDecodeBuffer.curPosition_] = testChar;
    mmsDecodeBuffer.DecodeIsShortInt();
    mmsDecodeBuffer.pduBuffer_[mmsDecodeBuffer.curPosition_] = testChar + 1;
    mmsDecodeBuffer.DecodeIsShortInt();

    mmsDecodeBuffer.curPosition_ = 0;
    char srcChar = MIN_SHORT_OCTET + 1;
    mmsDecodeBuffer.pduBuffer_[mmsDecodeBuffer.curPosition_] = srcChar;
    mmsDecodeBuffer.DecodeIsString();
    mmsDecodeBuffer.pduBuffer_[mmsDecodeBuffer.curPosition_] = srcChar + 1;
    mmsDecodeBuffer.DecodeIsString();
    mmsDecodeBuffer.pduBuffer_[mmsDecodeBuffer.curPosition_] = testChar + 1;
    mmsDecodeBuffer.DecodeIsString();

    mmsDecodeBuffer.curPosition_ = 0;
    mmsDecodeBuffer.pduBuffer_[mmsDecodeBuffer.curPosition_] = srcChar;
    mmsDecodeBuffer.DecodeIsValueLength();
    mmsDecodeBuffer.pduBuffer_[mmsDecodeBuffer.curPosition_] = srcChar + 1;
    mmsDecodeBuffer.DecodeIsValueLength();

    mmsDecodeBuffer.MarkPosition();
    mmsDecodeBuffer.UnMarkPosition();
    uint8_t errVar = -1;
    bool ret = mmsDecodeBuffer.CharIsToken(errVar);
    EXPECT_EQ(false, ret);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_MmsEncodeString_0001
 * @tc.name     Test MmsEncodeString
 * @tc.desc     Function test
 */
HWTEST_F(MmsGtest, MmsEncodeString_0001, Function | MediumTest | Level1)
{
    TELEPHONY_LOGI("TelSMSMMSTest::MmsEncodeString_0001 -->");
    MmsEncodeString mmsEncodeString;
    MmsDecodeBuffer decodeBuffer;
    MmsEncodeBuffer encodeBuffer;
    std::string testStr;
    uint32_t charset = PDU_COUNT;
    MmsAddress addrsss;
    uint32_t len = PDU_COUNT;
    std::unique_ptr<char[]> inBuff = std::make_unique<char[]>(len);
    for (uint32_t i = 0; i < len; i++) {
        inBuff[i] = 1;
    }
    decodeBuffer.WriteDataBuffer(std::move(inBuff), len);
    mmsEncodeString.DecodeEncodeString(decodeBuffer);
    decodeBuffer.curPosition_ = MAX_MMS_ATTACHMENT_LEN + 1;
    mmsEncodeString.DecodeEncodeString(decodeBuffer);
    mmsEncodeString.charset_ = ENCODE_CHARSET1;
    mmsEncodeString.EncodeEncodeString(encodeBuffer);
    mmsEncodeString.charset_ = ENCODE_CHARSET2;
    mmsEncodeString.EncodeEncodeString(encodeBuffer);
    mmsEncodeString.GetEncodeString(testStr);
    mmsEncodeString.SetAddressString(addrsss);
    mmsEncodeString.GetStrEncodeString();
    MmsEncodeString mmsEncodeString1(mmsEncodeString);
    charset = 0;
    bool ret = mmsEncodeString1.SetEncodeString(charset, testStr);
    EXPECT_EQ(true, ret);
    ret = mmsEncodeString1.SetEncodeString(charset, testStr);
    EXPECT_EQ(true, ret);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_MmsHeaderCateg_0001
 * @tc.name     Test MmsHeaderCateg
 * @tc.desc     Function test
 */
HWTEST_F(MmsGtest, MmsHeaderCateg_0001, Function | MediumTest | Level1)
{
    TELEPHONY_LOGI("TelSMSMMSTest::MmsHeaderCateg_0001 -->");
    MmsHeaderCateg mmsHeaderCateg;
    uint8_t fieldId = 0;
    mmsHeaderCateg.FindSendReqOptType(fieldId);
    mmsHeaderCateg.FindSendConfOptType(fieldId);
    mmsHeaderCateg.FindFieldDes(fieldId);
    mmsHeaderCateg.CheckIsValueLen(fieldId);
    bool ret = mmsHeaderCateg.CheckIsValueLen(fieldId);
    EXPECT_EQ(false, ret);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_MmsBase64_0001
 * @tc.name     Test MmsBase64
 * @tc.desc     Function test
 */
HWTEST_F(MmsGtest, MmsBase64_0001, Function | MediumTest | Level1)
{
    TELEPHONY_LOGI("TelSMSMMSTest::MmsBase64_0001 -->");
    MmsBase64 mmsBase64;
    std::string valueStr = "valueStr";
    mmsBase64.Encode(valueStr);
    std::string ret = mmsBase64.Decode(valueStr);
    EXPECT_STRNE(ret.c_str(), "test");
}

/**
 * @tc.number   Telephony_SmsMmsGtest_MmsCharSet_0001
 * @tc.name     Test MmsCharSet
 * @tc.desc     Function test
 */
HWTEST_F(MmsGtest, MmsCharSet_0001, Function | MediumTest | Level1)
{
    TELEPHONY_LOGI("TelSMSMMSTest::MmsCharSet_0001 -->");
    MmsCharSet mmsCharSet;
    uint32_t charSet = 10;
    std::string strCharSet = "US-ASCII";
    mmsCharSet.GetCharSetStrFromInt(strCharSet, charSet);
    bool ret = mmsCharSet.GetCharSetIntFromString(charSet, strCharSet);
    EXPECT_EQ(true, ret);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_MmsQuotedPrintable_0001
 * @tc.name     Test MmsQuotedPrintable
 * @tc.desc     Function test
 */
HWTEST_F(MmsGtest, MmsQuotedPrintable_0001, Function | MediumTest | Level1)
{
    TELEPHONY_LOGI("TelSMSMMSTest::MmsQuotedPrintable_0001 -->");
    MmsQuotedPrintable mmsQuotedPrintable;
    std::string valueStr = "123";
    mmsQuotedPrintable.Encode(valueStr);
    mmsQuotedPrintable.Decode(valueStr, valueStr);
    bool ret = mmsQuotedPrintable.Decode("", valueStr);
    EXPECT_EQ(false, ret);
}
#endif // TEL_TEST_UNSUPPORT
} // namespace Telephony
} // namespace OHOS