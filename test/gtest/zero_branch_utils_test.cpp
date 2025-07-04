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

#include <vector>

#include "gsm_pdu_hex_value.h"
#include "gsm_sms_common_utils.h"
#include "gtest/gtest.h"
#include "ims_sms_client.h"
#include "sms_common_utils.h"
#include "sms_persist_helper.h"
#include "text_coder.h"

namespace OHOS {
namespace Telephony {
using namespace testing::ext;

namespace {
const std::string TEXT_SMS_CONTENT = "hello world";
const int BUF_SIZE = 2401;
const int TEXT_LENGTH = 2;
const int FILL_BITS = 2;
const int DIGIT_LEN = 3;
const int DIGIT_LEN2 = 6;
const unsigned char SRC_TEXT = 2;
} // namespace

class BranchUtilsTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};
void BranchUtilsTest::SetUpTestCase() {}

constexpr uint32_t EVENT_RELEASE_DATA_SHARE_HELPER = 10000;
void BranchUtilsTest::TearDownTestCase()
{
    DelayedSingleton<ImsSmsClient>::GetInstance()->UnInit();
    DelayedSingleton<SmsPersistHelper>::GetInstance()->RemoveEvent(EVENT_RELEASE_DATA_SHARE_HELPER);
}

void BranchUtilsTest::SetUp() {}

void BranchUtilsTest::TearDown() {}

/**
 * @tc.number   Telephony_SmsMmsGtest_TextCoder_0001
 * @tc.name     Test TextCoder
 * @tc.desc     Function test
 */
HWTEST_F(BranchUtilsTest, TextCoder_0001, Function | MediumTest | Level1)
{
    DataCodingScheme DataCodingScheme;
    MsgLangInfo msgLangInfo;
    unsigned char encodeData[BUF_SIZE];
    MSG_LANGUAGE_ID_T langId = 0;
    bool unknown = false;
    unsigned short inText = 1;
    const uint8_t *pMsgText = (const uint8_t *)TEXT_SMS_CONTENT.c_str();
    uint8_t *pDestText = encodeData;
    SmsCodingNationalType codingNational = SMS_CODING_NATIONAL_TYPE_DEFAULT;
    EXPECT_GE(TextCoder::Instance().Utf8ToGsm7bit(pDestText, BUF_SIZE, const_cast<uint8_t *>(pMsgText), 0, langId), 0);
    EXPECT_GE(TextCoder::Instance().CdmaUtf8ToAuto(pDestText, 1, pMsgText, 1, DataCodingScheme), -1);
    EXPECT_GE(TextCoder::Instance().GsmUtf8ToAuto(pDestText, 1, pMsgText, 1,
        DataCodingScheme, codingNational, langId), -1);
    EXPECT_EQ(TextCoder::Instance().Utf8ToUcs2(pDestText, 1, pMsgText, -1), -1);
    EXPECT_EQ(TextCoder::Instance().Utf8ToUcs2(pDestText, 0, pMsgText, -1), 0);
    EXPECT_EQ(TextCoder::Instance().CdmaUtf8ToAuto(pDestText, 1, pMsgText, 0, DataCodingScheme), 0);
    EXPECT_EQ(TextCoder::Instance().GsmUtf8ToAuto(pDestText, 1, pMsgText, 0,
        DataCodingScheme, codingNational, langId), 0);
    EXPECT_EQ(TextCoder::Instance().Gsm7bitToUtf8(pDestText, 0, pMsgText, 0, msgLangInfo), 0);
    EXPECT_GE(TextCoder::Instance().ShiftjisToUtf8(pDestText, 1, pMsgText, -1), 0);
    EXPECT_GE(TextCoder::Instance().Ucs2ToUtf8(pDestText, 1, pMsgText, -1), 0);
    EXPECT_GE(TextCoder::Instance().EuckrToUtf8(pDestText, 1, pMsgText, -1), 0);
    EXPECT_FALSE(TextCoder::Instance().Ucs2ToUtf8(pDestText, 0, pMsgText, 0));
    EXPECT_FALSE(TextCoder::Instance().EuckrToUtf8(pDestText, 0, pMsgText, 1));
    EXPECT_FALSE(TextCoder::Instance().ShiftjisToUtf8(pDestText, 0, pMsgText, 1));
    EXPECT_EQ(TextCoder::Instance().Ucs2ToGsm7bit(pDestText, 0, pMsgText, 0, langId), -1);
    EXPECT_GT(TextCoder::Instance().Ucs2ToGsm7bit(pDestText, 1, pMsgText, TEXT_LENGTH, langId), 0);
    EXPECT_EQ(TextCoder::Instance().Ucs2ToGsm7bitAuto(pDestText, 0, pMsgText, 0, unknown, codingNational), -1);
    EXPECT_GE(TextCoder::Instance().Ucs2ToGsm7bitAuto(pDestText, 1, pMsgText, 1, unknown, codingNational), 0);
    EXPECT_EQ(TextCoder::Instance().Ucs2ToAscii(pDestText, 0, pMsgText, 0, unknown), -1);
    EXPECT_GE(TextCoder::Instance().Ucs2ToAscii(pDestText, 1, pMsgText, 1, unknown), 0);
    EXPECT_EQ(TextCoder::Instance().GetLangType(pMsgText, 0), MSG_DEFAULT_CHAR);
    EXPECT_GE(TextCoder::Instance().GetLangType(pMsgText, 0), MSG_DEFAULT_CHAR);
    EXPECT_EQ(TextCoder::Instance().FindGsm7bitExt(pDestText, 0, inText), 0);
    EXPECT_GE(TextCoder::Instance().FindGsm7bitExt(pDestText, 1, inText), 0);
    EXPECT_EQ(TextCoder::Instance().FindTurkish(pDestText, 0, inText), 0);
    EXPECT_GE(TextCoder::Instance().FindTurkish(pDestText, 1, inText), 0);
    EXPECT_EQ(TextCoder::Instance().FindSpanish(pDestText, 0, inText), 0);
    EXPECT_GE(TextCoder::Instance().FindSpanish(pDestText, 1, inText), 0);
    EXPECT_EQ(TextCoder::Instance().FindPortu(pDestText, 0, inText), 0);
    EXPECT_GE(TextCoder::Instance().FindPortu(pDestText, 1, inText), 0);
    EXPECT_GE(TextCoder::Instance().FindReplaceChar(inText), MSG_DEFAULT_CHAR);
    auto extMap = TextCoder::Instance().Get7BitCodingExtMap(codingNational);
    EXPECT_GE(extMap.size(), 0);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_TextCoder_0002
 * @tc.name     Test TextCoder
 * @tc.desc     Function test
 */
HWTEST_F(BranchUtilsTest, TextCoder_0002, Function | MediumTest | Level1)
{
    MsgLangInfo pLangInfo;
    pLangInfo.bLockingShift = true;
    pLangInfo.bSingleShift = true;
    unsigned char encodeData[BUF_SIZE];
    unsigned short result = 1;
    const uint8_t *pMsgText = (const uint8_t *)TEXT_SMS_CONTENT.c_str();
    uint8_t *pDestText = encodeData;
    EXPECT_EQ(TextCoder::Instance().Gsm7bitToUcs2(pDestText, 0, pMsgText, 0, pLangInfo), -1);
    pLangInfo.lockingLang = MSG_ID_TURKISH_LANG;
    EXPECT_GE(TextCoder::Instance().Gsm7bitToUcs2(pDestText, 1, pMsgText, 1, pLangInfo), 0);
    EXPECT_GT(TextCoder::Instance().EscapeToUcs2(SRC_TEXT, pLangInfo), 0);
    pLangInfo.lockingLang = MSG_ID_PORTUGUESE_LANG;
    EXPECT_GE(TextCoder::Instance().Gsm7bitToUcs2(pDestText, 1, pMsgText, 1, pLangInfo), 0);
    EXPECT_GT(TextCoder::Instance().EscapeToUcs2(SRC_TEXT, pLangInfo), 0);
    pLangInfo.bLockingShift = false;
    EXPECT_GE(TextCoder::Instance().Gsm7bitToUcs2(pDestText, 1, pMsgText, 1, pLangInfo), 0);
    EXPECT_EQ(TextCoder::Instance().EscapeTurkishLockingToUcs2(pMsgText, 0, pLangInfo, result), 0);
    EXPECT_EQ(TextCoder::Instance().EscapeTurkishLockingToUcs2(pMsgText, 1, pLangInfo, result), 0);
    EXPECT_EQ(TextCoder::Instance().EscapePortuLockingToUcs2(pMsgText, 0, pLangInfo, result), 0);
    EXPECT_EQ(TextCoder::Instance().EscapePortuLockingToUcs2(pMsgText, 1, pLangInfo, result), 0);
    EXPECT_EQ(TextCoder::Instance().EscapeGsm7bitToUcs2(pMsgText, 0, pLangInfo, result), 0);
    EXPECT_EQ(TextCoder::Instance().EscapeGsm7bitToUcs2(pMsgText, 1, pLangInfo, result), 0);
    pLangInfo.singleLang = MSG_ID_SPANISH_LANG;
    EXPECT_GT(TextCoder::Instance().EscapeToUcs2(SRC_TEXT, pLangInfo), 0);
    pLangInfo.singleLang = MSG_ID_RESERVED_LANG;
    EXPECT_GT(TextCoder::Instance().EscapeToUcs2(SRC_TEXT, pLangInfo), 0);
    pLangInfo.bSingleShift = false;
    EXPECT_GT(TextCoder::Instance().EscapeToUcs2(SRC_TEXT, pLangInfo), 0);
    std::string convertCharseInput = "123ascQ世界!@";
    std::string convertCharseOUtput = "";
    std::string convertCharseOUtputContrasts = "123ascQ荳也阜!@";
    uint32_t valLength = convertCharseInput.length();
    EXPECT_GE(TextCoder::Instance().GetEncodeString(convertCharseOUtput, 17, valLength, convertCharseInput), true);
    EXPECT_GE(convertCharseOUtput, convertCharseOUtputContrasts);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_TextCoder_0003
 * @tc.name     Test TextCoder
 * @tc.desc     Function test
 */
HWTEST_F(BranchUtilsTest, TextCoder_0003, Function | MediumTest | Level1)
{
    MSG_LANGUAGE_ID_T langId = 0;
    MsgLangInfo pLangInfo;
    pLangInfo.bLockingShift = true;
    pLangInfo.bSingleShift = true;
    unsigned char encodeData[BUF_SIZE];
    unsigned short result = 1;
    const uint8_t *pMsgText = (const uint8_t *)TEXT_SMS_CONTENT.c_str();
    uint8_t *pDestText = encodeData;
    EXPECT_EQ(TextCoder::Instance().Utf8ToGsm7bit(nullptr, 0, nullptr, -1, langId), 0);
    EXPECT_EQ(TextCoder::Instance().Utf8ToGsm7bit(nullptr, 1, pMsgText, -1, langId), 0);
    EXPECT_EQ(TextCoder::Instance().Utf8ToGsm7bit(pDestText, 0, pMsgText, 0, langId), 0);
    EXPECT_EQ(TextCoder::Instance().Utf8ToGsm7bit(pDestText, 0, pMsgText, 1, langId), 0);
    EXPECT_EQ(TextCoder::Instance().Gsm7bitToUcs2(nullptr, 0, nullptr, -1, pLangInfo), -1);
    EXPECT_EQ(TextCoder::Instance().Gsm7bitToUcs2(nullptr, 1, pMsgText, -1, pLangInfo), -1);
    EXPECT_EQ(TextCoder::Instance().Gsm7bitToUcs2(pDestText, 0, pMsgText, 0, pLangInfo), -1);
    EXPECT_EQ(TextCoder::Instance().Gsm7bitToUcs2(pDestText, 0, pMsgText, 1, pLangInfo), -1);
    EXPECT_EQ(TextCoder::Instance().Utf8ToUcs2(nullptr, 0, nullptr, -1), 0);
    EXPECT_EQ(TextCoder::Instance().Utf8ToUcs2(nullptr, 1, pMsgText, -1), 0);
    EXPECT_EQ(TextCoder::Instance().Utf8ToUcs2(pDestText, 0, pMsgText, 0), 0);
    EXPECT_EQ(TextCoder::Instance().Utf8ToUcs2(pDestText, 0, pMsgText, 1), 0);
    EXPECT_EQ(TextCoder::Instance().Get7BitCodingExtMap(SMS_CODING_NATIONAL_TYPE_DEFAULT),
        TextCoder::Instance().gsm7bitExtMap_);
    EXPECT_EQ(TextCoder::Instance().Get7BitCodingExtMap(SMS_CODING_NATIONAL_TYPE_TURKISH),
        TextCoder::Instance().turkishMap_);
    EXPECT_EQ(TextCoder::Instance().Get7BitCodingExtMap(SMS_CODING_NATIONAL_TYPE_SPANISH),
        TextCoder::Instance().spanishMap_);
    EXPECT_EQ(TextCoder::Instance().Get7BitCodingExtMap(SMS_CODING_NATIONAL_TYPE_PORTUGUESE),
        TextCoder::Instance().portuMap_);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_SmsCommonUtils_0001
 * @tc.name     Test SmsCommonUtils
 * @tc.desc     Function test
 */
HWTEST_F(BranchUtilsTest, SmsCommonUtils_0001, Function | MediumTest | Level1)
{
    auto smsCommonUtils = std::make_shared<SmsCommonUtils>();
    const unsigned char *userData = (const unsigned char *)TEXT_SMS_CONTENT.c_str();
    const std::string str = "*#PPQQ";
    const char *digit = (const char *)str.c_str();
    unsigned char *packData = (unsigned char *)TEXT_SMS_CONTENT.c_str();
    EXPECT_EQ(smsCommonUtils->Pack7bitChar(nullptr, 0, 0, nullptr, 0), 0);
    EXPECT_EQ(smsCommonUtils->Unpack7bitChar(nullptr, 1, 1, nullptr, 1), 0);
    EXPECT_EQ(smsCommonUtils->Pack7bitChar(userData, 1, 0, packData, 1), 1);
    EXPECT_EQ(smsCommonUtils->Pack7bitChar(userData, 1, 1, packData, 1), 1);
    EXPECT_EQ(smsCommonUtils->Pack7bitChar(userData, 1, FILL_BITS, packData, 1), 1);
    EXPECT_EQ(smsCommonUtils->Unpack7bitChar(userData, 1, 0, packData, 1), 0);
    EXPECT_EQ(smsCommonUtils->Unpack7bitChar(userData, 1, FILL_BITS, packData, 1), 0);
    EXPECT_EQ(smsCommonUtils->DigitToDtmfChar('*'), static_cast<char>(0x0B));
    EXPECT_EQ(smsCommonUtils->DtmfCharToDigit(static_cast<char>(0x0B)), '*');

    auto gsmUtils = std::make_shared<GsmSmsCommonUtils>();
    uint8_t len;
    EXPECT_EQ(gsmUtils->DigitToBcd(digit, 1, nullptr, 0, len), 0);
    EXPECT_EQ(gsmUtils->DigitToBcd(nullptr, 1, packData, 1, len), 0);
    EXPECT_FALSE(gsmUtils->DigitToBcd(digit, DIGIT_LEN, packData, 1, len));
    std::string digits;
    EXPECT_EQ(gsmUtils->BcdToDigit(userData, 1, digits, 1), 0);
    EXPECT_EQ(gsmUtils->BcdToDigit(nullptr, 1, digits, 1), 0);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_SmsCommonUtils_0002
 * @tc.name     Test SmsCommonUtils DigitToBcd support for +*#
 * @tc.desc     Function test
 */
HWTEST_F(BranchUtilsTest, SmsCommonUtils_0002, Function | MediumTest | Level1)
{
    // input data with pure number
    const std::string packDataStr = "hello world";
    const std::string digitTestStr = "17288424569";
    const char *digit = (const char *)digitTestStr.c_str();
    unsigned char *packData = (unsigned char *)packDataStr.c_str();
    auto gsmUtils = std::make_shared<GsmSmsCommonUtils>();
    EXPECT_NE(gsmUtils, nullptr);
    uint8_t len = static_cast<uint8_t>(DIGIT_LEN);
    EXPECT_FALSE(gsmUtils->DigitToBcd(digit, DIGIT_LEN, packData, DIGIT_LEN, len));
    EXPECT_FALSE(gsmUtils->DigitToBcd(digit, DIGIT_LEN, packData, DIGIT_LEN - 1, len));
    len = 0;
    EXPECT_TRUE(gsmUtils->DigitToBcd(digit, DIGIT_LEN2, packData, DIGIT_LEN2, len));
    std::vector<uint8_t> userDataVec(packData, packData + DIGIT_LEN2);
    EXPECT_TRUE(std::find(userDataVec.begin(), userDataVec.end(), 0x71) != userDataVec.end());
    EXPECT_TRUE(std::find(userDataVec.begin(), userDataVec.end(), 0x82) != userDataVec.end());
}

/**
 * @tc.number   Telephony_SmsMmsGtest_SmsCommonUtils_0003
 * @tc.name     Test SmsCommonUtils DigitToBcd
 * @tc.desc     Function test
 */
HWTEST_F(BranchUtilsTest, SmsCommonUtils_0003, Function | MediumTest | Level1)
{
    // input data with number and +
    const std::string packDataStr = "hello world";
    const std::string digitTestStr = "+17288424569";
    const char *digit = (const char *)digitTestStr.c_str();
    unsigned char *packData = (unsigned char *)packDataStr.c_str();
    auto gsmUtils = std::make_shared<GsmSmsCommonUtils>();
    EXPECT_NE(gsmUtils, nullptr);
    uint8_t len;
    EXPECT_TRUE(gsmUtils->DigitToBcd(digit, DIGIT_LEN2, packData, DIGIT_LEN2, len));
    std::vector<uint8_t> userDataVec(packData, packData + DIGIT_LEN2);
    EXPECT_TRUE(std::find(userDataVec.begin(), userDataVec.end(), 0x78) != userDataVec.end());
    EXPECT_TRUE(std::find(userDataVec.begin(), userDataVec.end(), 0x27) != userDataVec.end());
}

/**
 * @tc.number   Telephony_SmsMmsGtest_SmsCommonUtils_0004
 * @tc.name     Test SmsCommonUtils DigitToBcd
 * @tc.desc     Function test
 */
HWTEST_F(BranchUtilsTest, SmsCommonUtils_0004, Function | MediumTest | Level1)
{
    // input data with number and #
    const std::string packDataStr = "hello world";
    const std::string digitTestStr = "#17288424569";
    const char *digit = (const char *)digitTestStr.c_str();
    unsigned char *packData = (unsigned char *)packDataStr.c_str();
    auto gsmUtils = std::make_shared<GsmSmsCommonUtils>();
    EXPECT_NE(gsmUtils, nullptr);
    uint8_t len;
    EXPECT_TRUE(gsmUtils->DigitToBcd(digit, DIGIT_LEN2, packData, DIGIT_LEN2, len));
    std::vector<uint8_t> userDataVec(packData, packData + DIGIT_LEN2);
    EXPECT_TRUE(std::find(userDataVec.begin(), userDataVec.end(), 0x1b) != userDataVec.end());
}

/**
 * @tc.number   Telephony_SmsMmsGtest_SmsCommonUtils_0005
 * @tc.name     Test SmsCommonUtils DigitToBcd
 * @tc.desc     Function test
 */
HWTEST_F(BranchUtilsTest, SmsCommonUtils_0005, Function | MediumTest | Level1)
{
    // input data with # and *
    const std::string packDataStr = "hello world";
    const std::string digitTestStr = "*0#0765";
    const char *digit = (const char *)digitTestStr.c_str();
    unsigned char *packData = (unsigned char *)packDataStr.c_str();
    auto gsmUtils = std::make_shared<GsmSmsCommonUtils>();
    EXPECT_NE(gsmUtils, nullptr);
    uint8_t len;
    EXPECT_TRUE(gsmUtils->DigitToBcd(digit, DIGIT_LEN2, packData, DIGIT_LEN2, len));
    std::vector<uint8_t> userDataVec(packData, packData + DIGIT_LEN2);
    EXPECT_TRUE(std::find(userDataVec.begin(), userDataVec.end(), HEX_VALUE_0B) != userDataVec.end());
    EXPECT_TRUE(std::find(userDataVec.begin(), userDataVec.end(), HEX_VALUE_0A) != userDataVec.end());
}

/**
 * @tc.number   Telephony_SmsMmsGtest_SmsCommonUtils_0006
 * @tc.name     Test SmsCommonUtils DigitToBcd
 * @tc.desc     Function test
 */
HWTEST_F(BranchUtilsTest, SmsCommonUtils_0006, Function | MediumTest | Level1)
{
    // input data with # and *
    const std::string packDataStr = "hello world";
    const std::string digitTestStr = "*21#13105550020#";
    const char *digit = (const char *)digitTestStr.c_str();
    unsigned char *packData = (unsigned char *)packDataStr.c_str();
    auto gsmUtils = std::make_shared<GsmSmsCommonUtils>();
    EXPECT_NE(gsmUtils, nullptr);
    uint8_t len;
    EXPECT_TRUE(gsmUtils->DigitToBcd(digit, DIGIT_LEN2, packData, DIGIT_LEN2, len));
    std::vector<uint8_t> userDataVec(packData, packData + DIGIT_LEN2);
    EXPECT_TRUE(std::find(userDataVec.begin(), userDataVec.end(),  0x2a) != userDataVec.end());
    EXPECT_TRUE(std::find(userDataVec.begin(), userDataVec.end(),  0xb1) != userDataVec.end());
    EXPECT_TRUE(std::find(userDataVec.begin(), userDataVec.end(),  0x6f) != userDataVec.end());
}

/**
 * @tc.number   Telephony_SmsMmsGtest_SmsCommonUtils_0007
 * @tc.name     Test SmsCommonUtils DigitToBcd
 * @tc.desc     Function test
 */
HWTEST_F(BranchUtilsTest, SmsCommonUtils_0007, Function | MediumTest | Level1)
{
    auto smsCommonUtils = std::make_shared<SmsCommonUtils>();
    const uint8_t *userData = reinterpret_cast<const uint8_t *>("hello");
    uint8_t *packData = static_cast<uint8_t *>(malloc(1));
    uint8_t *nullData = nullptr;
    EXPECT_EQ(smsCommonUtils->Pack7bitChar(userData, 161, 0, packData, 1), 0);
    EXPECT_EQ(smsCommonUtils->Pack7bitChar(nullptr, 161, 0, packData, 1), 0);
    EXPECT_EQ(smsCommonUtils->Pack7bitChar(userData, 161, 0, nullData, 1), 0);
    EXPECT_EQ(smsCommonUtils->Pack7bitChar(nullptr, 161, 0, packData, 1), 0);
    EXPECT_EQ(smsCommonUtils->Pack7bitChar(nullptr, 0, 0, packData, 1), 0);
    EXPECT_EQ(smsCommonUtils->Pack7bitChar(userData, 0, 0, nullData, 1), 0);
    EXPECT_EQ(smsCommonUtils->Pack7bitChar(nullptr, 0, 0, packData, 1), 0);

    EXPECT_EQ(smsCommonUtils->Unpack7bitChar(userData, 1, 0, packData, 1), 0);
    EXPECT_EQ(smsCommonUtils->Unpack7bitChar(nullptr, 1, 0, packData, 1), 0);
    EXPECT_EQ(smsCommonUtils->Unpack7bitChar(userData, 1, 0, nullData, 1), 0);
    EXPECT_EQ(smsCommonUtils->Unpack7bitChar(nullptr, 1, 0, packData, 1), 0);
    EXPECT_EQ(smsCommonUtils->Unpack7bitChar(nullptr, 0, 0, packData, 1), 0);
    EXPECT_EQ(smsCommonUtils->Unpack7bitChar(userData, 0, 0, nullData, 1), 0);
    EXPECT_EQ(smsCommonUtils->Unpack7bitChar(nullptr, 0, 0, packData, 1), 0);

    EXPECT_EQ(smsCommonUtils->Unpack7bitCharForCBPdu(userData, 1, 0, packData, 1), 0);
    EXPECT_EQ(smsCommonUtils->Unpack7bitCharForCBPdu(nullptr, 1, 0, packData, 1), 0);
    EXPECT_EQ(smsCommonUtils->Unpack7bitCharForCBPdu(userData, 1, 0, nullData, 1), 0);
    EXPECT_EQ(smsCommonUtils->Unpack7bitCharForCBPdu(nullptr, 1, 0, packData, 1), 0);
    EXPECT_EQ(smsCommonUtils->Unpack7bitCharForCBPdu(nullptr, 0, 0, packData, 1), 0);
    EXPECT_EQ(smsCommonUtils->Unpack7bitCharForCBPdu(userData, 0, 0, nullData, 1), 0);
    EXPECT_EQ(smsCommonUtils->Unpack7bitCharForCBPdu(nullptr, 0, 0, packData, 1), 0);

    EXPECT_EQ(smsCommonUtils->DtmfCharToDigit(smsCommonUtils->DigitToDtmfChar('0')), '0');
    EXPECT_EQ(smsCommonUtils->DtmfCharToDigit(smsCommonUtils->DigitToDtmfChar('#')), '#');
    free(packData);
}
} // namespace Telephony
} // namespace OHOS
