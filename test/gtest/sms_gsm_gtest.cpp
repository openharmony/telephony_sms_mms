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

#include "gtest/gtest.h"
#define private public
#define protected public
#include "cb_start_ability.h"
#include "os_account.h"
#include "ios_account.h"
#include "gsm_sms_param_codec.h"
#include "gsm_pdu_hex_value.h"
#include "gsm_sms_common_utils.h"
#include "gsm_sms_message.h"
#include "gsm_sms_receive_handler.h"

namespace OHOS {
namespace Telephony {
using namespace testing::ext;

static constexpr uint8_t MAX_GSM_7BIT_DATA_LEN = 160;

class SmsGsmTest : public testing::Test {
public:
    static void SetUpTestCase()
    {}
    static void TearDownTestCase()
    {}
    void SetUp()
    {}
    void TearDown()
    {}
};

/**
 * @tc.number   Telephony_SmsMmsGtest_SmsGsmTest_001
 * @tc.name     SmsGsmTest_001
 * @tc.desc     Function test
 */
HWTEST_F(SmsGsmTest, SmsGsmTest_001, Function | MediumTest | Level1)
{
    AAFwk::Want want;
    DelayedSingleton<CbStartAbility>::GetInstance()->StartAbility(want);
    EXPECT_EQ(want.GetBundle(), "com.huawei.hmos.cellbroadcast");
}

/**
 * @tc.number   Telephony_SmsMmsGtest_SmsGsmTest_002
 * @tc.name     SmsGsmTest_002
 * @tc.desc     Function test
 */
HWTEST_F(SmsGsmTest, SmsGsmTest_002, Function | MediumTest | Level1)
{
    std::shared_ptr<GsmSmsParamCodec> codec = std::make_shared<GsmSmsParamCodec>();

    string strBuff;
    int32_t setType = 0;
    int32_t indType = 0;
    strBuff += HEX_VALUE_0D;
    SmsReadBuffer smsReadBuff(strBuff);
    EXPECT_FALSE(codec->CheckVoicemail(smsReadBuff, &setType, &indType));
}

/**
 * @tc.number   Telephony_SmsMmsGtest_SmsGsmTest_003
 * @tc.name     SmsGsmTest_003
 * @tc.desc     Function test
 */
HWTEST_F(SmsGsmTest, SmsGsmTest_003, Function | MediumTest | Level1)
{
    std::shared_ptr<GsmSmsParamCodec> codec = std::make_shared<GsmSmsParamCodec>();

    string strBuff;
    int32_t setType = 0;
    int32_t indType = 0;
    strBuff += HEX_VALUE_04;
    SmsReadBuffer smsReadBuff(strBuff);
    EXPECT_FALSE(codec->CheckVoicemail(smsReadBuff, &setType, &indType));
}

/**
 * @tc.number   Telephony_SmsMmsGtest_SmsGsmTest_004
 * @tc.name     SmsGsmTest_004
 * @tc.desc     Function test
 */
HWTEST_F(SmsGsmTest, SmsGsmTest_004, Function | MediumTest | Level1)
{
    std::shared_ptr<GsmSmsParamCodec> codec = std::make_shared<GsmSmsParamCodec>();

    string strBuff;
    int32_t setType = 0;
    int32_t indType = 0;
    strBuff += HEX_VALUE_04;
    strBuff += HEX_VALUE_04;
    SmsReadBuffer smsReadBuff(strBuff);
    EXPECT_FALSE(codec->CheckVoicemail(smsReadBuff, &setType, &indType));
}

/**
 * @tc.number   Telephony_SmsMmsGtest_SmsGsmTest_005
 * @tc.name     SmsGsmTest_005
 * @tc.desc     Function test
 */
HWTEST_F(SmsGsmTest, SmsGsmTest_005, Function | MediumTest | Level1)
{
    std::shared_ptr<GsmSmsParamCodec> codec = std::make_shared<GsmSmsParamCodec>();

    string strBuff;
    int32_t setType = 0;
    int32_t indType = 0;
    strBuff += HEX_VALUE_04;
    strBuff += HEX_VALUE_D0;
    SmsReadBuffer smsReadBuff(strBuff);
    EXPECT_FALSE(codec->CheckVoicemail(smsReadBuff, &setType, &indType));
}


/**
 * @tc.number   Telephony_SmsMmsGtest_SmsGsmTest_006
 * @tc.name     SmsGsmTest_006
 * @tc.desc     Function test
 */
HWTEST_F(SmsGsmTest, SmsGsmTest_006, Function | MediumTest | Level1)
{
    std::shared_ptr<GsmSmsParamCodec> codec = std::make_shared<GsmSmsParamCodec>();
    string strBuff;
    int32_t setType = 0;
    int32_t indType = 0;
    strBuff += HEX_VALUE_04;
    strBuff += HEX_VALUE_D0;
    strBuff += HEX_VALUE_D0;
    SmsReadBuffer smsReadBuff(strBuff);
    EXPECT_FALSE(codec->CheckVoicemail(smsReadBuff, &setType, &indType));
}

/**
 * @tc.number   Telephony_SmsMmsGtest_SmsGsmTest_007
 * @tc.name     SmsGsmTest_007
 * @tc.desc     Function test
 */
HWTEST_F(SmsGsmTest, SmsGsmTest_007, Function | MediumTest | Level1)
{
    std::shared_ptr<GsmSmsParamCodec> codec = std::make_shared<GsmSmsParamCodec>();

    string strBuff;
    int32_t setType = 0;
    int32_t indType = 0;
    strBuff += HEX_VALUE_04;
    strBuff += HEX_VALUE_D0;
    strBuff += HEX_VALUE_11;
    SmsReadBuffer smsReadBuff(strBuff);
    EXPECT_FALSE(codec->CheckVoicemail(smsReadBuff, &setType, &indType));
}

/**
 * @tc.number   Telephony_SmsMmsGtest_SmsGsmTest_008
 * @tc.name     SmsGsmTest_008
 * @tc.desc     Function test
 */
HWTEST_F(SmsGsmTest, SmsGsmTest_008, Function | MediumTest | Level1)
{
    std::shared_ptr<GsmSmsParamCodec> codec = std::make_shared<GsmSmsParamCodec>();

    string strBuff;
    int32_t setType = 0;
    int32_t indType = 0;
    strBuff += HEX_VALUE_04;
    strBuff += HEX_VALUE_D0;
    strBuff += HEX_VALUE_10;
    strBuff += HEX_VALUE_10;
    SmsReadBuffer smsReadBuff(strBuff);
    EXPECT_TRUE(codec->CheckVoicemail(smsReadBuff, &setType, &indType));
}

/**
 * @tc.number   Telephony_SmsMmsGtest_SmsGsmTest_009
 * @tc.name     SmsGsmTest_009
 * @tc.desc     Function test
 */
HWTEST_F(SmsGsmTest, SmsGsmTest_009, Function | MediumTest | Level1)
{
    std::shared_ptr<GsmSmsCommonUtils> utils = std::make_shared<GsmSmsCommonUtils>();
    SmsWriteBuffer buffer;
    EXPECT_FALSE(utils->Pack7bitChar(buffer, nullptr, 0, 0));
}

/**
 * @tc.number   Telephony_SmsMmsGtest_SmsGsmTest_010
 * @tc.name     SmsGsmTest_010
 * @tc.desc     Function test
 */
HWTEST_F(SmsGsmTest, SmsGsmTest_010, Function | MediumTest | Level1)
{
    std::shared_ptr<GsmSmsCommonUtils> utils = std::make_shared<GsmSmsCommonUtils>();
    std::vector<uint8_t> vectData;
    uint8_t fillBits = 1;

    SmsWriteBuffer buffer;
    EXPECT_FALSE(utils->Pack7bitChar(buffer, static_cast<const uint8_t*>(&vectData[0]), 0, fillBits));
}

/**
 * @tc.number   Telephony_SmsMmsGtest_SmsGsmTest_011
 * @tc.name     SmsGsmTest_011
 * @tc.desc     Function test
 */
HWTEST_F(SmsGsmTest, SmsGsmTest_011, Function | MediumTest | Level1)
{
    std::shared_ptr<GsmSmsCommonUtils> utils = std::make_shared<GsmSmsCommonUtils>();
    std::vector<uint8_t> vectData;
    uint8_t fillBits = 1;
    vectData.push_back(1);
    vectData.push_back(2);
    vectData.push_back(3);
    vectData.push_back(4);
    SmsWriteBuffer buffer;
    EXPECT_TRUE(utils->Pack7bitChar(buffer, static_cast<const uint8_t*>(&vectData[0]), vectData.size(), fillBits));

    vectData.clear();
    vectData.assign(MAX_GSM_7BIT_DATA_LEN + 1, 1); // first branch
    EXPECT_FALSE(utils->Pack7bitChar(buffer, static_cast<const uint8_t*>(&vectData[0]), vectData.size(), fillBits));
    vectData.clear();
    EXPECT_TRUE(utils->Pack7bitChar(buffer, static_cast<const uint8_t*>(&vectData[0]), vectData.size(), fillBits));
}

/**
 * @tc.number   Telephony_SmsMmsGtest_SmsGsmTest_012
 * @tc.name     SmsGsmTest_012
 * @tc.desc     Function test
 */
HWTEST_F(SmsGsmTest, SmsGsmTest_012, Function | MediumTest | Level1)
{
    std::shared_ptr<GsmSmsCommonUtils> utils = std::make_shared<GsmSmsCommonUtils>();
    std::string strBuffer;
    uint8_t fillBits = 0;
    uint8_t dstIdx = 0;

    SmsReadBuffer buffer(strBuffer);
    EXPECT_FALSE(utils->Unpack7bitChar(buffer, strBuffer.size(), fillBits,  nullptr, 0, dstIdx));
    fillBits = 8;
    EXPECT_FALSE(utils->Unpack7bitChar(buffer, strBuffer.size(), fillBits,  nullptr, 0, dstIdx));
}

/**
 * @tc.number   Telephony_SmsMmsGtest_SmsGsmTest_013
 * @tc.name     SmsGsmTest_013
 * @tc.desc     Function test
 */
HWTEST_F(SmsGsmTest, SmsGsmTest_013, Function | MediumTest | Level1)
{
    std::shared_ptr<GsmSmsCommonUtils> utils = std::make_shared<GsmSmsCommonUtils>();
    std::string strBuffer;
    std::vector<uint8_t> vectData;
    uint8_t fillBits = 1;
    uint8_t dstIdx = 0;

    SmsReadBuffer buffer(strBuffer);
    EXPECT_FALSE(utils->Unpack7bitChar(buffer, strBuffer.size(), fillBits,
        static_cast<uint8_t*>(&vectData[0]), vectData.size(), dstIdx));
}

/**
 * @tc.number   Telephony_SmsMmsGtest_SmsGsmTest_014
 * @tc.name     SmsGsmTest_014
 * @tc.desc     Function test Unpack7bitCharForMiddlePart
 */
HWTEST_F(SmsGsmTest, SmsGsmTest_014, Function | MediumTest | Level1)
{
    std::shared_ptr<GsmSmsCommonUtils> utils = std::make_shared<GsmSmsCommonUtils>();
    std::string dataStr("1141515");
    std::vector<uint8_t> vectData;

    const uint8_t *buffer = reinterpret_cast<const uint8_t *>(dataStr.c_str());
    EXPECT_FALSE(utils->Unpack7bitCharForMiddlePart(nullptr, 0, static_cast<uint8_t*>(&vectData[0])));
    EXPECT_FALSE(utils->Unpack7bitCharForMiddlePart(buffer, dataStr.size(), nullptr));
    EXPECT_FALSE(utils->Unpack7bitCharForMiddlePart(buffer, 0, static_cast<uint8_t*>(&vectData[0])));
}
}
}