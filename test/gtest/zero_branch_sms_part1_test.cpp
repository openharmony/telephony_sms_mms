/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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

#include <gmock/gmock.h>
#include <random>

#include "core_service_client.h"
#include "gtest/gtest.h"
#include "sms_service.h"
#include "sms_short_code_matcher.h"
#include "gsm_sms_message.h"

namespace OHOS {
namespace Telephony {
using namespace testing::ext;

class BranchSmsPart1Test : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void BranchSmsPart1Test::SetUpTestCase() {}
void BranchSmsPart1Test::TearDownTestCase() {}
void BranchSmsPart1Test::SetUp() {}
void BranchSmsPart1Test::TearDown() {}

/**
 * @tc.number   Telephony_BranchSmsPart1Test_SmsShortCodeMatcher_0001
 * @tc.name     Test SmsShortCodeMatcher
 * @tc.desc     Function test
 */
HWTEST_F(BranchSmsPart1Test, SmsShortCodeMatcher_0001, TestSize.Level0)
{
    auto smsShortCodeMatcher = std::make_shared<SmsShortCodeMatcher>();
    const std::string desAddr = "+10660";
    int32_t slotId = 0;
    const std::string expectedCountryCode = "cn";
    std::string countryCode;
    smsShortCodeMatcher->GetCountryCode(slotId, countryCode);
    EXPECT_EQ(countryCode, expectedCountryCode);
    SmsShortCodeType smsShortCodeType = smsShortCodeMatcher->GetSmsShortCodeType(slotId, desAddr);
    EXPECT_EQ(smsShortCodeType, SmsShortCodeType::SMS_SHORT_CODE_TYPE_POSSIBLE_PREMIUM);
}

/**
 * @tc.number   Telephony_BranchSmsPart1Test_SmsShortCodeMatcher_0002
 * @tc.name     Test SmsShortCodeMatcher
 * @tc.desc     Function test
 */
HWTEST_F(BranchSmsPart1Test, SmsShortCodeMatcher_0002, TestSize.Level0)
{
    auto smsShortCodeMatcher = std::make_shared<SmsShortCodeMatcher>();
    const std::string desAddr = "+10661";
    int32_t slotId = 0;
    const std::string expectedCountryCode = "cn";
    std::string countryCode;
    smsShortCodeMatcher->GetCountryCode(slotId, countryCode);
    EXPECT_EQ(countryCode, expectedCountryCode);
    SmsShortCodeType smsShortCodeType = smsShortCodeMatcher->GetSmsShortCodeType(slotId, desAddr);
    EXPECT_EQ(smsShortCodeType, SmsShortCodeType::SMS_SHORT_CODE_TYPE_POSSIBLE_PREMIUM);
}

/**
 * @tc.number   Telephony_BranchSmsPart1Test_SmsShortCodeMatcher_0003
 * @tc.name     Test SmsShortCodeMatcher
 * @tc.desc     Function test
 */
HWTEST_F(BranchSmsPart1Test, SmsShortCodeMatcher_0003, TestSize.Level0)
{
    auto smsShortCodeMatcher = std::make_shared<SmsShortCodeMatcher>();
    const std::string desAddr = "+10661";
    int32_t slotId = 2;
    const std::string expectedCountryCode = "";
    std::string countryCode;
    smsShortCodeMatcher->GetCountryCode(slotId, countryCode);
    EXPECT_EQ(countryCode, expectedCountryCode);
    SmsShortCodeType smsShortCodeType = smsShortCodeMatcher->GetSmsShortCodeType(slotId, desAddr);
    EXPECT_EQ(smsShortCodeType, SmsShortCodeType::SMS_SHORT_CODE_TYPE_UNKNOWN);
}

/**
 * @tc.number   Telephony_BranchSmsPart1Test_SmsShortCodeMatcher_0004
 * @tc.name     Test SmsShortCodeMatcher
 * @tc.desc     Function test
 */
HWTEST_F(BranchSmsPart1Test, SmsShortCodeMatcher_0004, TestSize.Level0)
{
    auto smsShortCodeMatcher = std::make_shared<SmsShortCodeMatcher>();
    const std::string desAddr = "+10650";
    int32_t slotId = 1;
    SmsShortCodeType smsShortCodeType = smsShortCodeMatcher->GetSmsShortCodeType(slotId, desAddr);
    EXPECT_EQ(smsShortCodeType, SmsShortCodeType::SMS_SHORT_CODE_TYPE_NOT_PREMIUM);
}

/**
 * @tc.number   Telephony_BranchSmsPart1Test_SmsShortCodeMatcher_0005
 * @tc.name     Test SmsShortCodeMatcher
 * @tc.desc     Function test
 */
HWTEST_F(BranchSmsPart1Test, SmsShortCodeMatcher_0005, TestSize.Level0)
{
    auto smsShortCodeMatcher = std::make_shared<SmsShortCodeMatcher>();
    const std::string desAddr = "+";
    int32_t slotId = 0;
    SmsShortCodeType smsShortCodeType = smsShortCodeMatcher->GetSmsShortCodeType(slotId, desAddr);
    EXPECT_EQ(smsShortCodeType, SmsShortCodeType::SMS_SHORT_CODE_TYPE_UNKNOWN);
}

/**
 * @tc.number   Telephony_BranchSmsPart1Test_SmsShortCodeMatcher_0006
 * @tc.name     Test SmsShortCodeMatcher
 * @tc.desc     Function test
 */
HWTEST_F(BranchSmsPart1Test, SmsShortCodeMatcher_0006, TestSize.Level0)
{
    auto smsShortCodeMatcher = std::make_shared<SmsShortCodeMatcher>();
    const std::string countryCode = "ca";
    const std::string desAddr = "+1234567";
    SmsShortCodeType smsShortCodeType = smsShortCodeMatcher->MatchShortCodeType(countryCode, desAddr);
    EXPECT_EQ(smsShortCodeType, SmsShortCodeType::SMS_SHORT_CODE_TYPE_NOT_PREMIUM);
}

/**
 * @tc.number   Telephony_BranchSmsPart1Test_SmsShortCodeMatcher_0007
 * @tc.name     Test SmsShortCodeMatcher
 * @tc.desc     Function test
 */
HWTEST_F(BranchSmsPart1Test, SmsShortCodeMatcher_0007, TestSize.Level0)
{
    auto smsShortCodeMatcher = std::make_shared<SmsShortCodeMatcher>();
    const std::string countryCode = "ca";
    const std::string desAddr = "244444";
    SmsShortCodeType smsShortCodeType = smsShortCodeMatcher->MatchShortCodeType(countryCode, desAddr);
    EXPECT_EQ(smsShortCodeType, SmsShortCodeType::SMS_SHORT_CODE_TYPE_NOT_PREMIUM);
}

/**
 * @tc.number   Telephony_BranchSmsPart1Test_SmsShortCodeMatcher_0008
 * @tc.name     Test SmsShortCodeMatcher
 * @tc.desc     Function test
 */
HWTEST_F(BranchSmsPart1Test, SmsShortCodeMatcher_0008, TestSize.Level0)
{
    auto smsShortCodeMatcher = std::make_shared<SmsShortCodeMatcher>();
    const std::string countryCode = "ca";
    const std::string desAddr = "+77777";
    SmsShortCodeType smsShortCodeType = smsShortCodeMatcher->MatchShortCodeType(countryCode, desAddr);
    EXPECT_EQ(smsShortCodeType, SmsShortCodeType::SMS_SHORT_CODE_TYPE_POSSIBLE_PREMIUM);
}

/**
 * @tc.number   Telephony_BranchSmsPart1Test_SmsShortCodeMatcher_0009
 * @tc.name     Test SmsShortCodeMatcher
 * @tc.desc     Function test
 */
HWTEST_F(BranchSmsPart1Test, SmsShortCodeMatcher_0009, TestSize.Level0)
{
    auto smsShortCodeMatcher = std::make_shared<SmsShortCodeMatcher>();
    const std::string countryCode = "pl";
    const std::string desAddr = "7910";
    SmsShortCodeType smsShortCodeType = smsShortCodeMatcher->MatchShortCodeType(countryCode, desAddr);
    EXPECT_EQ(smsShortCodeType, SmsShortCodeType::SMS_SHORT_CODE_TYPE_POSSIBLE_PREMIUM);
}

/**
 * @tc.number   Telephony_BranchSmsPart1Test_SmsShortCodeMatcher_0010
 * @tc.name     Test SmsShortCodeMatcher
 * @tc.desc     Function test
 */
HWTEST_F(BranchSmsPart1Test, SmsShortCodeMatcher_0010, TestSize.Level0)
{
    auto smsShortCodeMatcher = std::make_shared<SmsShortCodeMatcher>();
    const std::string countryCode = "pl";
    const std::string desAddr = "116123";
    SmsShortCodeType smsShortCodeType = smsShortCodeMatcher->MatchShortCodeType(countryCode, desAddr);
    EXPECT_EQ(smsShortCodeType, SmsShortCodeType::SMS_SHORT_CODE_TYPE_NOT_PREMIUM);
}

/**
 * @tc.number   Telephony_BranchSmsPart1Test_SmsShortCodeMatcher_0011
 * @tc.name     Test SmsShortCodeMatcher
 * @tc.desc     Function test
 */
HWTEST_F(BranchSmsPart1Test, SmsShortCodeMatcher_0011, TestSize.Level0)
{
    auto smsShortCodeMatcher = std::make_shared<SmsShortCodeMatcher>();
    const std::string countryCode = "";
    const std::string desAddr = "12345";
    SmsShortCodeType smsShortCodeType = smsShortCodeMatcher->MatchShortCodeType(countryCode, desAddr);
    EXPECT_EQ(smsShortCodeType, SmsShortCodeType::SMS_SHORT_CODE_TYPE_UNKNOWN);
}

/**
 * @tc.number   Telephony_BranchSmsPart1Test_SmsShortCodeMatcher_0012
 * @tc.name     Test SmsShortCodeMatcher
 * @tc.desc     Function test
 */
HWTEST_F(BranchSmsPart1Test, SmsShortCodeMatcher_0012, TestSize.Level0)
{
    auto smsShortCodeMatcher = std::make_shared<SmsShortCodeMatcher>();
    const std::string countryCode = "zz";
    const std::string desAddr = "12345";
    SmsShortCodeType smsShortCodeType = smsShortCodeMatcher->MatchShortCodeType(countryCode, desAddr);
    EXPECT_EQ(smsShortCodeType, SmsShortCodeType::SMS_SHORT_CODE_TYPE_UNKNOWN);
}

/**
 * @tc.number   Telephony_BranchSmsPart1Test_CreateMessage_0001
 * @tc.name     Test CreateMessage
 * @tc.desc     Function test
 */
HWTEST_F(BranchSmsPart1Test, CreateMessage_0001, Function | MediumTest | Level1)
{
    /*
        step1: The pdu whose mti is 0
    */
    std::string pduHex = "07914151551512f2040B916105551511f100006060605130308A04D4F29C0E";
    /*
        step2: Decoding pdu packets
    */
    GsmSmsMessage message;
    auto result = message.CreateMessage(pduHex);
    EXPECT_TRUE(result != nullptr);
    EXPECT_TRUE(result->GetSmscAddr()== "+14155551212");
    EXPECT_TRUE(result->GetOriginatingAddress() == "+16505551111");
}

/**
 * @tc.number   Telephony_BranchSmsPart1Test_CreateMessage_0002
 * @tc.name     Test CreateMessage
 * @tc.desc     Function test
 */
HWTEST_F(BranchSmsPart1Test, CreateMessage_0002, Function | MediumTest | Level1)
{
    /*
        step1: The pdu whose mti is 1
    */
    std::string pduHex = "07914151551512f2050B916105551511f100006060605130308A04D4F29C0E";
    /*
        step2: Decoding pdu packets
    */
    GsmSmsMessage message;
    auto result = message.CreateMessage(pduHex);
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.number   Telephony_BranchSmsPart1Test_CreateMessage_0003
 * @tc.name     Test CreateMessage
 * @tc.desc     Function test
 */
HWTEST_F(BranchSmsPart1Test, CreateMessage_0003, Function | MediumTest | Level1)
{
    /*
        step1: The pdu whose mti is 2
    */
    std::string pduHex = "07914151551512f2060B916105551511f100006060605130308A04D4F29C0E";
    /*
        step2: Decoding pdu packets
    */
    GsmSmsMessage message;
    auto result = message.CreateMessage(pduHex);
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.number   Telephony_BranchSmsPart1Test_CreateMessage_0004
 * @tc.name     Test CreateMessage
 * @tc.desc     Function test
 */
HWTEST_F(BranchSmsPart1Test, CreateMessage_0004, Function | MediumTest | Level1)
{
    /*
        step1: The pdu whose mti is 3
    */
    std::string pduHex = "07914151551512f2070B916105551511f100006060605130308A04D4F29C0E";
    /*
        step2: Decoding pdu packets
    */
    GsmSmsMessage message;
    auto result = message.CreateMessage(pduHex);
    EXPECT_TRUE(result != nullptr);
    EXPECT_TRUE(result->GetSmscAddr() == "+14155551212");
    EXPECT_TRUE(result->GetOriginatingAddress() == "+16505551111");
}

/**
 * @tc.number   Telephony_BranchSmsPart1Test_CreateMessage_0005
 * @tc.name     Test CreateMessage
 * @tc.desc     Function test
 */
HWTEST_F(BranchSmsPart1Test, CreateMessage_0005, Function | MediumTest | Level1)
{
    /*
        step1: The pdu whose ton is TYPE_ALPHA_NUMERIC
    */
    const std::string pduHex =
        "07915892208800F0040ED0A3F19CDD7A52A10008424011119582235C4F60768400630073006C00200041007000"
        "704E006B2160275BC678BC70BA0034003800370033003200373002598267097591554FFF0C8ACB806F7D61006300"
        "73006C670D52D971B17DDA003200350031003200330031003200333002";
    /*
        step2: Decoding pdu packets
    */
    GsmSmsMessage message;
    auto result = message.CreateMessage(pduHex);
    EXPECT_TRUE(result != nullptr);
    EXPECT_TRUE(result->GetSmscAddr() == "+85290288000");
    EXPECT_TRUE(result->GetOriginatingAddress() == "#csl-OTP");
}

/**
 * @tc.number   Telephony_BranchSmsPart1Test_CreateMessage_0006
 * @tc.name     Test CreateMessage
 * @tc.desc     Function test
 */
HWTEST_F(BranchSmsPart1Test, CreateMessage_0006, Function | MediumTest | Level1)
{
    /*
        step1: The pdu whose ton is TYPE_ALPHA_NUMERIC
    */
    const std::string pduHex =
        "07915892208800F0040ED0B4F19CDD8B61A10108424011119582235C4F60768400630073006C0020004100700"
        "0704E006B2160275BC678BC70BA0034003800370033003200373002598267097591554FFF0C8ACB806F7D610063"
        "0073006C670D52D971B17DDA003200350031003200330031003200333002";
    /*
        step2: Decoding pdu packets
    */
    GsmSmsMessage message;
    auto result = message.CreateMessage(pduHex);
    EXPECT_TRUE(result != nullptr);
    EXPECT_TRUE(result->GetSmscAddr() == "+85290288000");
    EXPECT_TRUE(result->GetOriginatingAddress() == "4csl=1XP");
}

/**
 * @tc.number   Telephony_BranchSmsPart1Test_CreateMessage_0007
 * @tc.name     Test CreateMessage special sms
 * @tc.desc     Function test
 */
HWTEST_F(BranchSmsPart1Test, CreateMessage_0007, Function | MediumTest | Level1)
{
    /*
        step1: The pdu whose ton is TYPE_ALPHA_NUMERIC
    */
    const std::string pduHex =
        "069168310992004412D0C87AF85A4E53CB733A00D85270709182850014"
        "0401020001D0CB733AC8FE4E8FCBED709A0D";
    /*
        step2: Decoding pdu packets
    */
    GsmSmsMessage message;
    auto result = message.CreateMessage(pduHex);
    EXPECT_TRUE(result != nullptr);
}
} // namespace Telephony
} // namespace OHOS