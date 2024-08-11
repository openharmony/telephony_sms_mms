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
#include "satellite_sms_callback.h"
#include "sms_common.h"
#include "telephony_errors.h"
#include "gtest/gtest.h"
#define private public
#define protected public
#include "mms_receive_manager.h"
#include "satellite_sms_client.h"
#include "mms_send_manager.h"
#undef private
#undef protected

namespace OHOS {
namespace Telephony {
using namespace testing::ext;

class MmsReceiveManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};
void MmsReceiveManagerTest::SetUpTestCase() {}
void MmsReceiveManagerTest::TearDownTestCase() {}
void MmsReceiveManagerTest::SetUp() {}
void MmsReceiveManagerTest::TearDown() {}

/**
 * @tc.number   MmsReceiveManagerTest_001
 * @tc.name     MmsReceiveManagerTest_001
 * @tc.desc     Function test
 */
HWTEST_F(MmsReceiveManagerTest, MmsReceiveManagerTest_001, Function | MediumTest | Level1)
{
    MmsReceiveManager recvMgr(1);
    std::u16string mmsc = u"mmsc";
    std::u16string data;
    std::u16string ua = u"ua";
    std::u16string uaprof = u"uaprof";
    recvMgr.Init();
    EXPECT_NE(recvMgr.DownloadMms(mmsc, data, ua, uaprof), TELEPHONY_ERR_SUCCESS);
}

/**
 * @tc.number   MmsReceiveManagerTest_002
 * @tc.name     MmsReceiveManagerTest_001
 * @tc.desc     Function test
 */
HWTEST_F(MmsReceiveManagerTest, MmsReceiveManagerTest_002, Function | MediumTest | Level1)
{
    MmsReceiveManager recvMgr(1);
    std::u16string mmsc = u"mmsc";
    std::u16string data;
    std::u16string ua = u"ua";
    std::u16string uaprof = u"uaprof";
    recvMgr.Init();
    recvMgr.mmsReceiver_ = nullptr;
    EXPECT_EQ(recvMgr.DownloadMms(mmsc, data, ua, uaprof), TELEPHONY_ERR_LOCAL_PTR_NULL);
}

/**
 * @tc.number   SatelliteSmsClient_001
 * @tc.name     SatelliteSmsClient_001
 * @tc.desc     Function test
 */
HWTEST_F(MmsReceiveManagerTest, SatelliteSmsClient_001, Function | MediumTest | Level1)
{
    auto &satelliteSmsClient = SatelliteSmsClient::GetInstance();
    EXPECT_EQ(nullptr, satelliteSmsClient.GetProxy());
    satelliteSmsClient.RemoveDeathRecipient(nullptr, true);
    satelliteSmsClient.RemoveDeathRecipient(nullptr, false);

    int32_t slotId = 1;
    EXPECT_EQ(TELEPHONY_ERR_LOCAL_PTR_NULL, satelliteSmsClient.AddSendHandler(slotId, nullptr));
    const std::shared_ptr<TelEventHandler> sender = std::make_shared<MmsSendManager>(slotId);
    EXPECT_EQ(TELEPHONY_SUCCESS, satelliteSmsClient.AddSendHandler(slotId, sender));
}
/**
 * @tc.number   SatelliteSmsClient_002
 * @tc.name     SatelliteSmsClient_002
 * @tc.desc     Function test
 */
HWTEST_F(MmsReceiveManagerTest, SatelliteSmsClient_002, Function | MediumTest | Level1)
{
    auto &satelliteSmsClient = SatelliteSmsClient::GetInstance();
    EXPECT_EQ(nullptr, satelliteSmsClient.GetProxy());

    int32_t slotId = 1;
    EXPECT_EQ(TELEPHONY_ERR_LOCAL_PTR_NULL, satelliteSmsClient.AddReceiveHandler(slotId, nullptr));
    const std::shared_ptr<TelEventHandler> receiver = std::make_shared<MmsReceiveManager>(slotId);
    EXPECT_EQ(TELEPHONY_SUCCESS, satelliteSmsClient.AddReceiveHandler(slotId, receiver));
}

/**
 * @tc.number   SatelliteSmsClient_003
 * @tc.name     SatelliteSmsClient_003
 * @tc.desc     Function test
 */
HWTEST_F(MmsReceiveManagerTest, SatelliteSmsClient_003, Function | MediumTest | Level1)
{
    auto &satelliteSmsClient = SatelliteSmsClient::GetInstance();
    EXPECT_EQ(nullptr, satelliteSmsClient.GetProxy());
    int32_t slotId = 1;
    satelliteSmsClient.senderMap_.clear();
    satelliteSmsClient.receiverMap_.clear();
    satelliteSmsClient.senderMap_[slotId] = nullptr;
    ASSERT_EQ(satelliteSmsClient.senderMap_.size(), 1);
    satelliteSmsClient.ServiceOn();
    satelliteSmsClient.senderMap_.clear();

    satelliteSmsClient.receiverMap_[slotId] = nullptr;
    satelliteSmsClient.ServiceOn();
    satelliteSmsClient.receiverMap_.clear();
}
}
}
