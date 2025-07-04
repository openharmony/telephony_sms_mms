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
#include "sms_persist_helper.h"
#include "string_utils.h"

namespace OHOS {
namespace Telephony {
using namespace testing::ext;
constexpr uint32_t EVENT_RELEASE_DATA_SHARE_HELPER = 10000;

class SmsUtilsTest : public testing::Test {
public:
    static void SetUpTestCase()
    {}
    static void TearDownTestCase()
    {
        DelayedSingleton<SmsPersistHelper>::GetInstance()->RemoveEvent(EVENT_RELEASE_DATA_SHARE_HELPER);
    }
    void SetUp()
    {}
    void TearDown()
    {}
};

/**
 * @tc.number   Telephony_SmsUtilsTest_SmsUtilsTest_001
 * @tc.name     SmsUtilsTest_001
 * @tc.desc     Function test
 */
HWTEST_F(SmsUtilsTest, SmsUtilsTest_001, Function | MediumTest | Level1)
{
    std::string str = "Abcd0";
    auto vect = StringUtils::HexToByteVector(str);
    EXPECT_NE(vect.size(), 0);
}

/**
 * @tc.number   Telephony_SmsUtilsTest_SmsUtilsTest_002
 * @tc.name     SmsUtilsTest_002
 * @tc.desc     Function test
 */
HWTEST_F(SmsUtilsTest, SmsUtilsTest_002, Function | MediumTest | Level1)
{
    std::string str = "";
    EXPECT_TRUE(StringUtils::IsEmpty(str));
    str = "abc";
    EXPECT_FALSE(StringUtils::IsEmpty(str));
}
}
}