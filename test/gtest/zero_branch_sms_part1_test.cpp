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
    PremiumSmsType premiumSmsType = smsShortCodeMatcher->GetPremiumSmsType(slotId, desAddr);
    EXPECT_EQ(premiumSmsType, PremiumSmsType::PREMIUM_OR_POSSIBLE_PREMIUM);
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
    PremiumSmsType premiumSmsType = smsShortCodeMatcher->GetPremiumSmsType(slotId, desAddr);
    EXPECT_EQ(premiumSmsType, PremiumSmsType::PREMIUM_OR_POSSIBLE_PREMIUM);
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
    const std::string expectedCountryCode = "cn";
    std::string countryCode;
    smsShortCodeMatcher->GetCountryCode(slotId, countryCode);
    EXPECT_EQ(countryCode, expectedCountryCode);
    PremiumSmsType premiumSmsType = smsShortCodeMatcher->GetPremiumSmsType(slotId, desAddr);
    EXPECT_EQ(premiumSmsType, PremiumSmsType::PREMIUM_OR_POSSIBLE_PREMIUM);
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
    int32_t slotId = 0;
    PremiumSmsType premiumSmsType = smsShortCodeMatcher->GetPremiumSmsType(slotId, desAddr);
    EXPECT_EQ(premiumSmsType, PremiumSmsType::NOT_PREMIUM);
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
    PremiumSmsType premiumSmsType = smsShortCodeMatcher->GetPremiumSmsType(slotId, desAddr);
    EXPECT_EQ(premiumSmsType, PremiumSmsType::UNKNOWN);
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
    EXPECT_EQ(smsShortCodeType, SmsShortCodeType::SMS_SHORT_CODE_TYPE_NOT_SHORT_CODE);
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
    EXPECT_EQ(smsShortCodeType, SmsShortCodeType::SMS_SHORT_CODE_TYPE_STANDARD);
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
    EXPECT_EQ(smsShortCodeType, SmsShortCodeType::SMS_SHORT_CODE_TYPE_PREMIUM);
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
    EXPECT_EQ(smsShortCodeType, SmsShortCodeType::SMS_SHORT_CODE_TYPE_FREE);
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
} // namespace Telephony
} // namespace OHOS