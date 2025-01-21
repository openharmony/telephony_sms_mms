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
#include "sms_mms_common.h"

namespace OHOS {
namespace Telephony {
using namespace testing::ext;

class SmsMmsCommonTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};
void SmsMmsCommonTest::SetUpTestCase() {}

void SmsMmsCommonTest::TearDownTestCase() {}

void SmsMmsCommonTest::SetUp() {}

void SmsMmsCommonTest::TearDown() {}

/**
 * @tc.number   Telephony_SmsMmsCommonTest_SendBroadcast_0001
 * @tc.name     Test SendBroadcast
 * @tc.desc     Function test
 */
HWTEST_F(SmsMmsCommonTest, SendBroadcast_0001, Function | MediumTest | Level1)
{
    std::string sendStatus = "";
    uint16_t dataBaseId = 0;
    ASSERT_NO_THROW(DelayedSingleton<SmsMmsCommon>::GetInstance()->SendBroadcast(dataBaseId,
        SmsMmsCommonData::SMS_MMS_SENT_RESULT_NOTIFY, sendStatus,
        SmsMmsCommonData::SMS_MMS_INFO_MMS_TYPE));
}
} // namespace Telephony
} // namespace OHOS