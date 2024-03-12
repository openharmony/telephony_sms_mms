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

#include "core_service_client.h"
#include "gtest/gtest.h"
#include "i_sms_service_interface.h"
#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "radio_event.h"
#include "sms_broadcast_subscriber_gtest.h"
#include "sms_mms_gtest.h"
#include "sms_mms_test_helper.h"
#include "sms_service.h"
#include "sms_service_manager_client.h"
#include "telephony_log_wrapper.h"
#include "telephony_types.h"

namespace OHOS {
namespace Telephony {
namespace {
sptr<ISmsServiceInterface> g_telephonyService = nullptr;
} // namespace
using namespace testing::ext;

class CbGtest : public testing::Test {
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

void CbGtest::TearDownTestCase() {}

void CbGtest::SetUp() {}

void CbGtest::TearDown() {}

const int32_t DEFAULT_SIM_SLOT_ID_1 = 1;
const uint16_t MESSAGE_TYPE = 4;
const uint16_t CB_PDU_LEN = 100;

void CbGtest::SetUpTestCase()
{
    TELEPHONY_LOGI("SetUpTestCase slotId%{public}d", DEFAULT_SIM_SLOT_ID_1);
    g_telephonyService = GetProxy();
    if (g_telephonyService == nullptr) {
        return;
    }
    DelayedSingleton<SmsServiceManagerClient>::GetInstance()->ResetSmsServiceProxy();
    DelayedSingleton<SmsServiceManagerClient>::GetInstance()->InitSmsServiceProxy();
}

sptr<ISmsServiceInterface> CbGtest::GetProxy()
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
void OpenCellBroadcastTestFuc(SmsMmsTestHelper &helper)
{
    AccessMmsToken token;
    bool enable = true;
    uint32_t fromMsgId = 0;
    uint32_t toMsgId = 10;
    uint8_t netType = 1;
    int32_t result = DelayedSingleton<SmsServiceManagerClient>::GetInstance()->SetCBConfig(
        helper.slotId, enable, fromMsgId, toMsgId, netType);
    helper.SetIntResult(result);
    helper.NotifyAll();
}

/**
 * @tc.number   Telephony_SmsMmsGtest_OpenCellBroadcast_0001
 * @tc.name     Open cellBroadcast slotId is -1
 * @tc.desc     Function test
 */
HWTEST_F(CbGtest, OpenCellBroadcast_0001, Function | MediumTest | Level3)
{
    TELEPHONY_LOGI("TelSMSMMSTest::OpenCellBroadcast_0001 -->");
    if (!(CbGtest::HasSimCard(DEFAULT_SIM_SLOT_ID))) {
        TELEPHONY_LOGI("TelephonyTestService has no sim card");
        ASSERT_TRUE(true);
        return;
    }
    SmsMmsTestHelper helper;
    helper.slotId = DEFAULT_SIM_SLOT_ID_REMOVE;
    if (!helper.Run(OpenCellBroadcastTestFuc, std::ref(helper))) {
        TELEPHONY_LOGI("OpenCellBroadcastTestFuc out of time");
        ASSERT_TRUE(false);
    }
    TELEPHONY_LOGI("TelSMSMMSTest::OpenCellBroadcast_0001 -->finished");
    EXPECT_NE(helper.GetIntResult(), 0);
}

void OpenCellBroadcastTestFuc2(SmsMmsTestHelper &helper)
{
    AccessMmsToken token;
    bool enable = true;
    uint32_t fromMsgId = 20;
    uint32_t toMsgId = 10;
    uint8_t netType = 1;
    int32_t result = DelayedSingleton<SmsServiceManagerClient>::GetInstance()->SetCBConfig(
        helper.slotId, enable, fromMsgId, toMsgId, netType);
    helper.SetIntResult(result);
    helper.NotifyAll();
}

/**
 * @tc.number   Telephony_SmsMmsGtest_OpenCellBroadcast_0002
 * @tc.name     Open cellBroadcast fromMsgId less than toMsgId
 * @tc.desc     Function test
 */
HWTEST_F(CbGtest, OpenCellBroadcast_0002, Function | MediumTest | Level3)
{
    TELEPHONY_LOGI("TelSMSMMSTest::OpenCellBroadcast_0002 -->");
    if (!(CbGtest::HasSimCard(DEFAULT_SIM_SLOT_ID))) {
        TELEPHONY_LOGI("TelephonyTestService has no sim card");
        ASSERT_TRUE(true);
        return;
    }
    SmsMmsTestHelper helper;
    helper.slotId = DEFAULT_SIM_SLOT_ID;
    if (!helper.Run(OpenCellBroadcastTestFuc2, std::ref(helper))) {
        TELEPHONY_LOGI("OpenCellBroadcastTestFuc2 out of time");
        ASSERT_TRUE(false);
    }
    TELEPHONY_LOGI("TelSMSMMSTest::OpenCellBroadcast_0002 -->finished");
    EXPECT_NE(helper.GetIntResult(), 0);
}

void OpenCellBroadcastTestFuc3(SmsMmsTestHelper &helper)
{
    AccessMmsToken token;
    bool enable = true;
    uint32_t fromMsgId = 0;
    uint32_t toMsgId = 10;
    uint8_t netType = 3;
    int32_t result = DelayedSingleton<SmsServiceManagerClient>::GetInstance()->SetCBConfig(
        helper.slotId, enable, fromMsgId, toMsgId, netType);
    helper.SetIntResult(result);
    helper.NotifyAll();
}

/**
 * @tc.number   Telephony_SmsMmsGtest_OpenCellBroadcast_0003
 * @tc.name     Open cellBroadcast netType is unknown.
 * @tc.desc     Function test
 */
HWTEST_F(CbGtest, OpenCellBroadcast_0003, Function | MediumTest | Level3)
{
    TELEPHONY_LOGI("TelSMSMMSTest::OpenCellBroadcast_0003 -->");
    if (!(CbGtest::HasSimCard(DEFAULT_SIM_SLOT_ID))) {
        TELEPHONY_LOGI("TelephonyTestService has no sim card");
        ASSERT_TRUE(true);
        return;
    }
    SmsMmsTestHelper helper;
    helper.slotId = DEFAULT_SIM_SLOT_ID;
    if (!helper.Run(OpenCellBroadcastTestFuc3, std::ref(helper))) {
        TELEPHONY_LOGI("OpenCellBroadcastTestFuc3 out of time");
        ASSERT_TRUE(false);
    }
    TELEPHONY_LOGI("TelSMSMMSTest::OpenCellBroadcast_0003 -->finished");
    EXPECT_NE(helper.GetIntResult(), 0);
}

void OpenCellBroadcastTestFuc4(SmsMmsTestHelper &helper)
{
    AccessMmsToken token;
    bool enable = true;
    uint32_t fromMsgId = 0;
    uint32_t toMsgId = 10;
    uint8_t netType = 1;
    int32_t result = DelayedSingleton<SmsServiceManagerClient>::GetInstance()->SetCBConfig(
        helper.slotId, enable, fromMsgId, toMsgId, netType);
    helper.SetIntResult(result);
    helper.NotifyAll();
}

/**
 * @tc.number   Telephony_SmsMmsGtest_OpenCellBroadcast_0004
 * @tc.name     Open cellBroadcast parameter is valid.
 * @tc.desc     Function test
 */
HWTEST_F(CbGtest, OpenCellBroadcast_0004, Function | MediumTest | Level3)
{
    TELEPHONY_LOGI("TelSMSMMSTest::OpenCellBroadcast_0004 -->");
    if (!(CbGtest::HasSimCard(DEFAULT_SIM_SLOT_ID))) {
        TELEPHONY_LOGI("TelephonyTestService has no sim card");
        ASSERT_TRUE(true);
        return;
    }
    SmsMmsTestHelper helper;
    helper.slotId = DEFAULT_SIM_SLOT_ID;
    if (!helper.Run(OpenCellBroadcastTestFuc4, std::ref(helper))) {
        TELEPHONY_LOGI("OpenCellBroadcastTestFuc4 out of time");
        ASSERT_TRUE(false);
    }
    TELEPHONY_LOGI("TelSMSMMSTest::OpenCellBroadcast_0004 -->finished");
    ASSERT_GE(helper.GetIntResult(), 0);
}

void OpenCellBroadcastTestFuc5(SmsMmsTestHelper &helper)
{
    AccessMmsToken token;
    bool enable = true;
    uint32_t fromMsgId = 0;
    uint32_t toMsgId = 1000;
    uint8_t netType = 1;
    int32_t result = DelayedSingleton<SmsServiceManagerClient>::GetInstance()->SetCBConfig(
        helper.slotId, enable, fromMsgId, toMsgId, netType);
    helper.SetIntResult(result);
    helper.NotifyAll();
}

/**
 * @tc.number   Telephony_SmsMmsGtest_OpenCellBroadcast_0005
 * @tc.name     Open cellBroadcast parameter is valid
 * @tc.desc     Function test
 */
HWTEST_F(CbGtest, OpenCellBroadcast_0005, Function | MediumTest | Level3)
{
    TELEPHONY_LOGI("TelSMSMMSTest::OpenCellBroadcast_0005 -->");
    if (!(CbGtest::HasSimCard(DEFAULT_SIM_SLOT_ID))) {
        TELEPHONY_LOGI("TelephonyTestService has no sim card");
        ASSERT_TRUE(true);
        return;
    }
    SmsMmsTestHelper helper;
    helper.slotId = DEFAULT_SIM_SLOT_ID;
    if (!helper.Run(OpenCellBroadcastTestFuc5, std::ref(helper))) {
        TELEPHONY_LOGI("OpenCellBroadcastTestFuc5 out of time");
        ASSERT_TRUE(false);
    }
    TELEPHONY_LOGI("TelSMSMMSTest::OpenCellBroadcast_0005 -->finished");
    ASSERT_GE(helper.GetIntResult(), 0);
}

void OpenCellBroadcastTestFuc6(SmsMmsTestHelper &helper)
{
    bool enable = true;
    uint32_t fromMsgId = 0;
    uint32_t toMsgId = 0;
    uint8_t netType = 1;
    int32_t result = DelayedSingleton<SmsServiceManagerClient>::GetInstance()->SetCBConfig(
        helper.slotId, enable, fromMsgId, toMsgId, netType);
    helper.SetIntResult(result);
    helper.NotifyAll();
}

void OpenCellBroadcastTestFuc6WithToken(SmsMmsTestHelper &helper)
{
    AccessMmsToken token;
    OpenCellBroadcastTestFuc6(helper);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_OpenCellBroadcast_0006
 * @tc.name     Open cellBroadcast parameter is valid
 * @tc.desc     Function test
 */
HWTEST_F(CbGtest, OpenCellBroadcast_0006, Function | MediumTest | Level3)
{
    TELEPHONY_LOGI("TelSMSMMSTest::OpenCellBroadcast_0006 -->");
    if (!(CbGtest::HasSimCard(DEFAULT_SIM_SLOT_ID))) {
        TELEPHONY_LOGI("TelephonyTestService has no sim card");
        ASSERT_TRUE(true);
        return;
    }
    SmsMmsTestHelper helper;
    helper.slotId = DEFAULT_SIM_SLOT_ID;
    if (!helper.Run(OpenCellBroadcastTestFuc6WithToken, std::ref(helper))) {
        TELEPHONY_LOGI("OpenCellBroadcastTestFuc6WithToken out of time");
        ASSERT_TRUE(false);
    }
    TELEPHONY_LOGI("TelSMSMMSTest::OpenCellBroadcast_0006 -->finished");
    ASSERT_GE(helper.GetIntResult(), 0);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_OpenCellBroadcast_0007
 * @tc.name     Open cellBroadcast fromMsgId less than toMsgId
 * @tc.desc     Function test
 */
HWTEST_F(CbGtest, OpenCellBroadcast_0007, Function | MediumTest | Level3)
{
    TELEPHONY_LOGI("TelSMSMMSTest::OpenCellBroadcast_0007 -->");
    if (!(CbGtest::HasSimCard(DEFAULT_SIM_SLOT_ID_1))) {
        TELEPHONY_LOGI("TelephonyTestService has no sim card");
        ASSERT_TRUE(true);
        return;
    }
    SmsMmsTestHelper helper;
    helper.slotId = DEFAULT_SIM_SLOT_ID_1;
    if (!helper.Run(OpenCellBroadcastTestFuc2, std::ref(helper))) {
        TELEPHONY_LOGI("OpenCellBroadcastTestFuc2 out of time");
        ASSERT_TRUE(false);
    }
    TELEPHONY_LOGI("TelSMSMMSTest::OpenCellBroadcast_0007 -->finished");
    EXPECT_NE(helper.GetIntResult(), 0);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_OpenCellBroadcast_0008
 * @tc.name     Open cellBroadcast netType is unknown.
 * @tc.desc     Function test
 */
HWTEST_F(CbGtest, OpenCellBroadcast_0008, Function | MediumTest | Level3)
{
    TELEPHONY_LOGI("TelSMSMMSTest::OpenCellBroadcast_0008 -->");
    if (!(CbGtest::HasSimCard(DEFAULT_SIM_SLOT_ID_1))) {
        TELEPHONY_LOGI("TelephonyTestService has no sim card");
        ASSERT_TRUE(true);
        return;
    }
    SmsMmsTestHelper helper;
    helper.slotId = DEFAULT_SIM_SLOT_ID_1;
    if (!helper.Run(OpenCellBroadcastTestFuc3, std::ref(helper))) {
        TELEPHONY_LOGI("OpenCellBroadcastTestFuc3 out of time");
        ASSERT_TRUE(false);
    }
    TELEPHONY_LOGI("TelSMSMMSTest::OpenCellBroadcast_0008 -->finished");
    EXPECT_NE(helper.GetIntResult(), 0);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_OpenCellBroadcast_0009
 * @tc.name     Open cellBroadcast parameter is valid.
 * @tc.desc     Function test
 */
HWTEST_F(CbGtest, OpenCellBroadcast_0009, Function | MediumTest | Level3)
{
    TELEPHONY_LOGI("TelSMSMMSTest::OpenCellBroadcast_0009 -->");
    if (!(CbGtest::HasSimCard(DEFAULT_SIM_SLOT_ID_1))) {
        TELEPHONY_LOGI("TelephonyTestService has no sim card");
        ASSERT_TRUE(true);
        return;
    }
    SmsMmsTestHelper helper;
    helper.slotId = DEFAULT_SIM_SLOT_ID_1;
    if (!helper.Run(OpenCellBroadcastTestFuc4, std::ref(helper))) {
        TELEPHONY_LOGI("OpenCellBroadcastTestFuc4 out of time");
        ASSERT_TRUE(false);
    }
    TELEPHONY_LOGI("TelSMSMMSTest::OpenCellBroadcast_0009 -->finished");
    ASSERT_EQ(helper.GetIntResult(), 0);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_OpenCellBroadcast_0010
 * @tc.name     Open cellBroadcast parameter is valid
 * @tc.desc     Function test
 */
HWTEST_F(CbGtest, OpenCellBroadcast_00010, Function | MediumTest | Level3)
{
    TELEPHONY_LOGI("TelSMSMMSTest::OpenCellBroadcast_0010 -->");
    if (!(CbGtest::HasSimCard(DEFAULT_SIM_SLOT_ID_1))) {
        TELEPHONY_LOGI("TelephonyTestService has no sim card");
        ASSERT_TRUE(true);
        return;
    }
    SmsMmsTestHelper helper;
    helper.slotId = DEFAULT_SIM_SLOT_ID_1;
    if (!helper.Run(OpenCellBroadcastTestFuc5, std::ref(helper))) {
        TELEPHONY_LOGI("OpenCellBroadcastTestFuc5 out of time");
        ASSERT_TRUE(false);
    }
    TELEPHONY_LOGI("TelSMSMMSTest::OpenCellBroadcast_0010 -->finished");
    ASSERT_EQ(helper.GetIntResult(), 0);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_OpenCellBroadcast_0011
 * @tc.name     Open cellBroadcast parameter is valid
 * @tc.desc     Function test
 */
HWTEST_F(CbGtest, OpenCellBroadcast_0011, Function | MediumTest | Level3)
{
    TELEPHONY_LOGI("TelSMSMMSTest::OpenCellBroadcast_0011 -->");
    if (!(CbGtest::HasSimCard(DEFAULT_SIM_SLOT_ID_1))) {
        TELEPHONY_LOGI("TelephonyTestService has no sim card");
        ASSERT_TRUE(true);
        return;
    }
    SmsMmsTestHelper helper;
    helper.slotId = DEFAULT_SIM_SLOT_ID_1;
    if (!helper.Run(OpenCellBroadcastTestFuc6WithToken, std::ref(helper))) {
        TELEPHONY_LOGI("OpenCellBroadcastTestFuc6WithToken out of time");
        ASSERT_TRUE(false);
    }
    TELEPHONY_LOGI("TelSMSMMSTest::OpenCellBroadcast_0011 -->finished");
    ASSERT_EQ(helper.GetIntResult(), 0);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_OpenCellBroadcast_0012
 * @tc.name     Open cellBroadcast parameter is valid
 * @tc.desc     Function test
 */
HWTEST_F(CbGtest, OpenCellBroadcast_0012, Function | MediumTest | Level3)
{
    TELEPHONY_LOGI("TelSMSMMSTest::OpenCellBroadcast_0012 -->");
    if (!(CbGtest::HasSimCard(DEFAULT_SIM_SLOT_ID))) {
        TELEPHONY_LOGI("TelephonyTestService has no sim card");
        ASSERT_TRUE(true);
        return;
    }
    SmsMmsTestHelper helper;
    helper.slotId = DEFAULT_SIM_SLOT_ID;
    if (!helper.Run(OpenCellBroadcastTestFuc6, std::ref(helper))) {
        TELEPHONY_LOGI("OpenCellBroadcastTestFuc6 out of time");
        ASSERT_TRUE(false);
    }
    TELEPHONY_LOGI("TelSMSMMSTest::OpenCellBroadcast_0012 -->finished");
    EXPECT_NE(helper.GetIntResult(), 0);
}

void CloseCellBroadcastTestFuc(SmsMmsTestHelper &helper)
{
    AccessMmsToken token;
    bool enable = false;
    uint32_t fromMsgId = 0;
    uint32_t toMsgId = 10;
    uint8_t netType = 1;
    int32_t result = DelayedSingleton<SmsServiceManagerClient>::GetInstance()->SetCBConfig(
        helper.slotId, enable, fromMsgId, toMsgId, netType);
    helper.SetIntResult(result);
    helper.NotifyAll();
}

/**
 * @tc.number   Telephony_SmsMmsGtest_CloseCellBroadcast_0001
 * @tc.name     Close cellBroadcast slotId is -1
 * @tc.desc     Function test
 */
HWTEST_F(CbGtest, CloseCellBroadcast_0001, Function | MediumTest | Level3)
{
    TELEPHONY_LOGI("TelSMSMMSTest::CloseCellBroadcast_0001 -->");
    if (!(CbGtest::HasSimCard(DEFAULT_SIM_SLOT_ID))) {
        TELEPHONY_LOGI("TelephonyTestService has no sim card");
        ASSERT_TRUE(true);
        return;
    }
    SmsMmsTestHelper helper;
    helper.slotId = DEFAULT_SIM_SLOT_ID_REMOVE;
    if (!helper.Run(CloseCellBroadcastTestFuc, std::ref(helper))) {
        TELEPHONY_LOGI("CloseCellBroadcastTestFuc out of time");
        ASSERT_TRUE(false);
    }
    TELEPHONY_LOGI("TelSMSMMSTest::CloseCellBroadcast_0001 -->finished");
    EXPECT_NE(helper.GetIntResult(), 0);
}

void CloseCellBroadcastTestFuc2(SmsMmsTestHelper &helper)
{
    AccessMmsToken token;
    bool enable = false;
    uint32_t fromMsgId = 20;
    uint32_t toMsgId = 10;
    uint8_t netType = 1;
    int32_t result = DelayedSingleton<SmsServiceManagerClient>::GetInstance()->SetCBConfig(
        helper.slotId, enable, fromMsgId, toMsgId, netType);
    helper.SetIntResult(result);
    helper.NotifyAll();
}

/**
 * @tc.number   Telephony_SmsMmsGtest_CloseCellBroadcast_0002
 * @tc.name     Close cellBroadcast fromMsgId less than toMsgId
 * @tc.desc     Function test
 */
HWTEST_F(CbGtest, CloseCellBroadcast_0002, Function | MediumTest | Level3)
{
    TELEPHONY_LOGI("TelSMSMMSTest::CloseCellBroadcast_0002 -->");
    if (!(CbGtest::HasSimCard(DEFAULT_SIM_SLOT_ID))) {
        TELEPHONY_LOGI("TelephonyTestService has no sim card");
        ASSERT_TRUE(true);
        return;
    }
    SmsMmsTestHelper helper;
    helper.slotId = DEFAULT_SIM_SLOT_ID;
    if (!helper.Run(CloseCellBroadcastTestFuc2, std::ref(helper))) {
        TELEPHONY_LOGI("CloseCellBroadcastTestFuc2 out of time");
        ASSERT_TRUE(false);
    }
    TELEPHONY_LOGI("TelSMSMMSTest::CloseCellBroadcast_0002 -->finished");
    EXPECT_NE(helper.GetIntResult(), 0);
}

void CloseCellBroadcastTestFuc3(SmsMmsTestHelper &helper)
{
    AccessMmsToken token;
    bool enable = false;
    uint32_t fromMsgId = 0;
    uint32_t toMsgId = 10;
    uint8_t netType = 3;
    int32_t result = DelayedSingleton<SmsServiceManagerClient>::GetInstance()->SetCBConfig(
        helper.slotId, enable, fromMsgId, toMsgId, netType);
    helper.SetIntResult(result);
    helper.NotifyAll();
}

/**
 * @tc.number   Telephony_SmsMmsGtest_CloseCellBroadcast_0003
 * @tc.name     Close cellBroadcast netType is unknown
 * @tc.desc     Function test
 */
HWTEST_F(CbGtest, CloseCellBroadcast_0003, Function | MediumTest | Level3)
{
    TELEPHONY_LOGI("TelSMSMMSTest::CloseCellBroadcast_0003 -->");
    if (!(CbGtest::HasSimCard(DEFAULT_SIM_SLOT_ID))) {
        TELEPHONY_LOGI("TelephonyTestService has no sim card");
        ASSERT_TRUE(true);
        return;
    }
    SmsMmsTestHelper helper;
    helper.slotId = DEFAULT_SIM_SLOT_ID;
    if (!helper.Run(CloseCellBroadcastTestFuc3, std::ref(helper))) {
        TELEPHONY_LOGI("CloseCellBroadcastTestFuc3 out of time");
        ASSERT_TRUE(false);
    }
    TELEPHONY_LOGI("TelSMSMMSTest::CloseCellBroadcast_0003-->finished");
    EXPECT_NE(helper.GetIntResult(), 0);
}

void CloseCellBroadcastTestFuc4(SmsMmsTestHelper &helper)
{
    AccessMmsToken token;
    bool enable = false;
    uint32_t fromMsgId = 0;
    uint32_t toMsgId = 10;
    uint8_t netType = 1;
    int32_t result = DelayedSingleton<SmsServiceManagerClient>::GetInstance()->SetCBConfig(
        helper.slotId, enable, fromMsgId, toMsgId, netType);
    helper.SetIntResult(result);
    helper.NotifyAll();
}

/**
 * @tc.number   Telephony_SmsMmsGtest_CloseCellBroadcast_0004
 * @tc.name     Close cellBroadcast parameter is valid
 * @tc.desc     Function test
 */
HWTEST_F(CbGtest, CloseCellBroadcast_0004, Function | MediumTest | Level3)
{
    TELEPHONY_LOGI("TelSMSMMSTest::CloseCellBroadcast_0004 -->");
    if (!(CbGtest::HasSimCard(DEFAULT_SIM_SLOT_ID))) {
        TELEPHONY_LOGI("TelephonyTestService has no sim card");
        ASSERT_TRUE(true);
        return;
    }
    SmsMmsTestHelper helper;
    helper.slotId = DEFAULT_SIM_SLOT_ID;
    if (!helper.Run(CloseCellBroadcastTestFuc4, std::ref(helper))) {
        TELEPHONY_LOGI("CloseCellBroadcastTestFuc4 out of time");
        ASSERT_TRUE(false);
    }
    TELEPHONY_LOGI("TelSMSMMSTest::CloseCellBroadcast_0004 -->finished");
    ASSERT_GE(helper.GetIntResult(), 0);
}

void CloseCellBroadcastTestFuc5(SmsMmsTestHelper &helper)
{
    AccessMmsToken token;
    bool enable = false;
    uint32_t fromMsgId = 0;
    uint32_t toMsgId = 1000;
    uint8_t netType = 1;
    int32_t result = DelayedSingleton<SmsServiceManagerClient>::GetInstance()->SetCBConfig(
        helper.slotId, enable, fromMsgId, toMsgId, netType);
    helper.SetIntResult(result);
    helper.NotifyAll();
}

/**
 * @tc.number   Telephony_SmsMmsGtest_CloseCellBroadcast_0005
 * @tc.name     Close cellBroadcast parameter is valid
 * @tc.desc     Function test
 */
HWTEST_F(CbGtest, CloseCellBroadcast_0005, Function | MediumTest | Level3)
{
    TELEPHONY_LOGI("TelSMSMMSTest::CloseCellBroadcast_0005 -->");
    if (!(CbGtest::HasSimCard(DEFAULT_SIM_SLOT_ID))) {
        TELEPHONY_LOGI("TelephonyTestService has no sim card");
        ASSERT_TRUE(true);
        return;
    }
    SmsMmsTestHelper helper;
    helper.slotId = DEFAULT_SIM_SLOT_ID;
    if (!helper.Run(CloseCellBroadcastTestFuc5, std::ref(helper))) {
        TELEPHONY_LOGI("CloseCellBroadcastTestFuc5 out of time");
        ASSERT_TRUE(false);
    }
    TELEPHONY_LOGI("TelSMSMMSTest::CloseCellBroadcast_0005 -->finished");
    ASSERT_GE(helper.GetIntResult(), 0);
}

void CloseCellBroadcastTestFuc6(SmsMmsTestHelper &helper)
{
    bool enable = false;
    uint32_t fromMsgId = 0;
    uint32_t toMsgId = 0;
    uint8_t netType = 1;
    int32_t result = DelayedSingleton<SmsServiceManagerClient>::GetInstance()->SetCBConfig(
        helper.slotId, enable, fromMsgId, toMsgId, netType);
    helper.SetIntResult(result);
    helper.NotifyAll();
}

void CloseCellBroadcastTestFuc6WithToken(SmsMmsTestHelper &helper)
{
    AccessMmsToken token;
    CloseCellBroadcastTestFuc6(helper);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_CloseCellBroadcast_0006
 * @tc.name     Close cellBroadcast parameter is valid
 * @tc.desc     Function test
 */
HWTEST_F(CbGtest, CloseCellBroadcast_0006, Function | MediumTest | Level3)
{
    TELEPHONY_LOGI("TelSMSMMSTest::CloseCellBroadcast_0006 -->");
    if (!(CbGtest::HasSimCard(DEFAULT_SIM_SLOT_ID))) {
        TELEPHONY_LOGI("TelephonyTestService has no sim card");
        ASSERT_TRUE(true);
        return;
    }
    SmsMmsTestHelper helper;
    helper.slotId = DEFAULT_SIM_SLOT_ID;
    if (!helper.Run(CloseCellBroadcastTestFuc6WithToken, std::ref(helper))) {
        TELEPHONY_LOGI("CloseCellBroadcastTestFuc6WithToken out of time");
        ASSERT_TRUE(false);
    }
    TELEPHONY_LOGI("TelSMSMMSTest::CloseCellBroadcast_0006 -->finished");
    ASSERT_GE(helper.GetIntResult(), 0);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_CloseCellBroadcast_0007
 * @tc.name     Close cellBroadcast fromMsgId less than toMsgId
 * @tc.desc     Function test
 */
HWTEST_F(CbGtest, CloseCellBroadcast_0007, Function | MediumTest | Level3)
{
    TELEPHONY_LOGI("TelSMSMMSTest::CloseCellBroadcast_0007 -->");
    if (!(CbGtest::HasSimCard(DEFAULT_SIM_SLOT_ID_1))) {
        TELEPHONY_LOGI("TelephonyTestService has no sim card");
        ASSERT_TRUE(true);
        return;
    }
    SmsMmsTestHelper helper;
    helper.slotId = DEFAULT_SIM_SLOT_ID_1;
    if (!helper.Run(CloseCellBroadcastTestFuc2, std::ref(helper))) {
        TELEPHONY_LOGI("CloseCellBroadcastTestFuc7 out of time");
        ASSERT_TRUE(false);
    }
    TELEPHONY_LOGI("TelSMSMMSTest::CloseCellBroadcast_0007 -->finished");
    EXPECT_NE(helper.GetIntResult(), 0);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_CloseCellBroadcast_0008
 * @tc.name     Close cellBroadcast netType is unknown
 * @tc.desc     Function test
 */
HWTEST_F(CbGtest, CloseCellBroadcast_0008, Function | MediumTest | Level3)
{
    TELEPHONY_LOGI("TelSMSMMSTest::CloseCellBroadcast_0008 -->");
    if (!(CbGtest::HasSimCard(DEFAULT_SIM_SLOT_ID_1))) {
        TELEPHONY_LOGI("TelephonyTestService has no sim card");
        ASSERT_TRUE(true);
        return;
    }
    SmsMmsTestHelper helper;
    helper.slotId = DEFAULT_SIM_SLOT_ID_1;
    if (!helper.Run(CloseCellBroadcastTestFuc3, std::ref(helper))) {
        TELEPHONY_LOGI("CloseCellBroadcastTestFuc8 out of time");
        ASSERT_TRUE(false);
    }
    TELEPHONY_LOGI("TelSMSMMSTest::CloseCellBroadcast_0008-->finished");
    EXPECT_NE(helper.GetIntResult(), 0);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_CloseCellBroadcast_0009
 * @tc.name     Close cellBroadcast parameter is valid
 * @tc.desc     Function test
 */
HWTEST_F(CbGtest, CloseCellBroadcast_0009, Function | MediumTest | Level3)
{
    TELEPHONY_LOGI("TelSMSMMSTest::CloseCellBroadcast_0009 -->");
    if (!(CbGtest::HasSimCard(DEFAULT_SIM_SLOT_ID_1))) {
        TELEPHONY_LOGI("TelephonyTestService has no sim card");
        ASSERT_TRUE(true);
        return;
    }
    SmsMmsTestHelper helper;
    helper.slotId = DEFAULT_SIM_SLOT_ID_1;
    if (!helper.Run(CloseCellBroadcastTestFuc4, std::ref(helper))) {
        TELEPHONY_LOGI("CloseCellBroadcastTestFuc9 out of time");
        ASSERT_TRUE(false);
    }
    TELEPHONY_LOGI("TelSMSMMSTest::CloseCellBroadcast_0009 -->finished");
    ASSERT_GE(helper.GetIntResult(), 0);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_CloseCellBroadcast_0010
 * @tc.name     Close cellBroadcast parameter is valid
 * @tc.desc     Function test
 */
HWTEST_F(CbGtest, CloseCellBroadcast_0010, Function | MediumTest | Level3)
{
    TELEPHONY_LOGI("TelSMSMMSTest::CloseCellBroadcast_0010 -->");
    if (!(CbGtest::HasSimCard(DEFAULT_SIM_SLOT_ID_1))) {
        TELEPHONY_LOGI("TelephonyTestService has no sim card");
        ASSERT_TRUE(true);
        return;
    }
    SmsMmsTestHelper helper;
    helper.slotId = DEFAULT_SIM_SLOT_ID_1;
    if (!helper.Run(CloseCellBroadcastTestFuc5, std::ref(helper))) {
        TELEPHONY_LOGI("CloseCellBroadcastTestFuc10 out of time");
        ASSERT_TRUE(false);
    }
    TELEPHONY_LOGI("TelSMSMMSTest::CloseCellBroadcast_0010 -->finished");
    ASSERT_GE(helper.GetIntResult(), 0);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_CloseCellBroadcast_0011
 * @tc.name     Close cellBroadcast parameter is valid
 * @tc.desc     Function test
 */
HWTEST_F(CbGtest, CloseCellBroadcast_00011, Function | MediumTest | Level3)
{
    TELEPHONY_LOGI("TelSMSMMSTest::CloseCellBroadcast_0011 -->");
    if (!(CbGtest::HasSimCard(DEFAULT_SIM_SLOT_ID_1))) {
        TELEPHONY_LOGI("TelephonyTestService has no sim card");
        ASSERT_TRUE(true);
        return;
    }
    SmsMmsTestHelper helper;
    helper.slotId = DEFAULT_SIM_SLOT_ID_1;
    if (!helper.Run(CloseCellBroadcastTestFuc6WithToken, std::ref(helper))) {
        TELEPHONY_LOGI("CloseCellBroadcastTestFuc6WithToken out of time");
        ASSERT_TRUE(false);
    }
    TELEPHONY_LOGI("TelSMSMMSTest::CloseCellBroadcast_0011 -->finished");
    ASSERT_GE(helper.GetIntResult(), 0);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_CloseCellBroadcast_0012
 * @tc.name     Close cellBroadcast parameter is valid
 * @tc.desc     Function test
 */
HWTEST_F(CbGtest, CloseCellBroadcast_0012, Function | MediumTest | Level3)
{
    TELEPHONY_LOGI("TelSMSMMSTest::CloseCellBroadcast_0012 -->");
    if (!(CbGtest::HasSimCard(DEFAULT_SIM_SLOT_ID))) {
        TELEPHONY_LOGI("TelephonyTestService has no sim card");
        ASSERT_TRUE(true);
        return;
    }
    SmsMmsTestHelper helper;
    helper.slotId = DEFAULT_SIM_SLOT_ID;
    if (!helper.Run(CloseCellBroadcastTestFuc6, std::ref(helper))) {
        TELEPHONY_LOGI("CloseCellBroadcastTestFuc6 out of time");
        ASSERT_TRUE(false);
    }
    TELEPHONY_LOGI("TelSMSMMSTest::CloseCellBroadcast_0012 -->finished");
    EXPECT_NE(helper.GetIntResult(), 0);
}

void ReceiveCellBroadCastTestFunc(SmsMmsTestHelper &helper)
{
    AccessMmsToken token;
    auto gsmSmsCbHandler = std::make_shared<GsmSmsCbHandler>(helper.slotId);
    auto message = std::make_shared<CBConfigReportInfo>();
    message->indicationType = MESSAGE_TYPE;
    message->sn = 0;
    message->mid = 0;
    message->page = 0;
    message->pages = 0;
    message->dcs = "";
    message->data = "";
    message->length = CB_PDU_LEN;
    message->pdu = "01a41f51101102ea3030a830ea30a230e130fc30eb914d4fe130c630b930c8000d000a3053308c306f8a669a137528306e3"
                   "0e130c330bb30fc30b8306730593002000d000aff080032003000310033002f00310031002f003252ea3000370020003100"
                   "35003a00340034ff09000d000aff0830a830ea30a25e02ff090000000000000000000000000000000000000000000000000"
                   "0000000000000000000000000000000000000000000000022";
    AppExecFwk::InnerEvent::Pointer event = AppExecFwk::InnerEvent::Get(RadioEvent::RADIO_CELL_BROADCAST, message);
    gsmSmsCbHandler->ProcessEvent(event);
}

void ReceiveCellBroadCastTestFunc2(SmsMmsTestHelper &helper)
{
    AccessMmsToken token;
    auto gsmSmsCbHandler = std::make_shared<GsmSmsCbHandler>(helper.slotId);
    auto message = std::make_shared<CBConfigReportInfo>();
    message->indicationType = MESSAGE_TYPE;
    message->sn = 0;
    message->mid = 0;
    message->page = 0;
    message->pages = 0;
    message->dcs = "";
    message->data = "";
    message->length = CB_PDU_LEN;
    message->pdu = "C0000032401174747A0E4ACF41E8B0BCFD76E741EF39685C66B34162F93B4C1E87E77410BD3CA7836EC2341D440ED3C321";
    AppExecFwk::InnerEvent::Pointer event = AppExecFwk::InnerEvent::Get(RadioEvent::RADIO_CELL_BROADCAST, message);
    gsmSmsCbHandler->ProcessEvent(event);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_Receive_Cell_BroadCast_0001
 * @tc.name     Receive a 3g Cell Broadcast
 * @tc.desc     Function test
 */
HWTEST_F(CbGtest, Receive_Cell_BroadCast_0001, Function | MediumTest | Level2)
{
    TELEPHONY_LOGI("TelSMSMMSTest::Receive_Cell_BroadCast_0001 -->");
    int32_t slotId = DEFAULT_SIM_SLOT_ID;
    if (!(CbGtest::HasSimCard(slotId))) {
        TELEPHONY_LOGI("TelephonyTestService has no sim card");
        ASSERT_TRUE(true);
        return;
    }
    SmsMmsTestHelper helper;
    helper.slotId = slotId;

    EventFwk::MatchingSkills matchingSkills;
    matchingSkills.AddEvent(EventFwk::CommonEventSupport::COMMON_EVENT_SMS_EMERGENCY_CB_RECEIVE_COMPLETED);
    matchingSkills.AddEvent(EventFwk::CommonEventSupport::COMMON_EVENT_SMS_CB_RECEIVE_COMPLETED);
    EventFwk::CommonEventSubscribeInfo subscriberInfo(matchingSkills);
    std::shared_ptr<SmsBroadcastSubscriberGtest> subscriberTest =
        std::make_shared<SmsBroadcastSubscriberGtest>(subscriberInfo, helper);
    if (subscriberTest == nullptr) {
        ASSERT_TRUE(false);
        return;
    }
    bool subscribeResult = EventFwk::CommonEventManager::SubscribeCommonEvent(subscriberTest);
    TELEPHONY_LOGI("subscribeResult is : %{public}d", subscribeResult);

    if (!helper.Run(ReceiveCellBroadCastTestFunc, helper)) {
        TELEPHONY_LOGI("ReceiveCellBroadCastTestFunc out of time");
        ASSERT_TRUE(true);
        return;
    }
    TELEPHONY_LOGI("TelSMSMMSTest::Receive_Cell_BroadCast_0001 -->finished");
    EXPECT_TRUE(helper.GetBoolResult());
}

/**
 * @tc.number   Telephony_SmsMmsGtest_Receive_Cell_BroadCast_0002
 * @tc.name     Receive a 2g Cell Broadcast
 * @tc.desc     Function test
 */
HWTEST_F(CbGtest, Receive_Cell_BroadCast_0002, Function | MediumTest | Level2)
{
    TELEPHONY_LOGI("TelSMSMMSTest::Receive_Cell_BroadCast_0002 -->");
    int32_t slotId = DEFAULT_SIM_SLOT_ID;
    if (!(CbGtest::HasSimCard(slotId))) {
        TELEPHONY_LOGI("TelephonyTestService has no sim card");
        ASSERT_TRUE(true);
        return;
    }
    SmsMmsTestHelper helper;
    helper.slotId = slotId;

    EventFwk::MatchingSkills matchingSkills;
    matchingSkills.AddEvent(EventFwk::CommonEventSupport::COMMON_EVENT_SMS_EMERGENCY_CB_RECEIVE_COMPLETED);
    matchingSkills.AddEvent(EventFwk::CommonEventSupport::COMMON_EVENT_SMS_CB_RECEIVE_COMPLETED);
    EventFwk::CommonEventSubscribeInfo subscriberInfo(matchingSkills);
    std::shared_ptr<SmsBroadcastSubscriberGtest> subscriberTest =
        std::make_shared<SmsBroadcastSubscriberGtest>(subscriberInfo, helper);
    if (subscriberTest == nullptr) {
        ASSERT_TRUE(false);
        return;
    }
    bool subscribeResult = EventFwk::CommonEventManager::SubscribeCommonEvent(subscriberTest);
    TELEPHONY_LOGI("subscribeResult is : %{public}d", subscribeResult);

    if (!helper.Run(ReceiveCellBroadCastTestFunc2, helper)) {
        TELEPHONY_LOGI("ReceiveCellBroadCastTestFunc2 out of time");
        ASSERT_TRUE(true);
        return;
    }
    TELEPHONY_LOGI("TelSMSMMSTest::Receive_Cell_BroadCast_0002 -->finished");
    EXPECT_TRUE(helper.GetBoolResult());
}

/**
 * @tc.number   Telephony_SmsMmsGtest_Receive_Cell_BroadCast_0003
 * @tc.name     Receive a 3g Cell Broadcast
 * @tc.desc     Function test
 */
HWTEST_F(CbGtest, Receive_Cell_BroadCast_0003, Function | MediumTest | Level2)
{
    TELEPHONY_LOGI("TelSMSMMSTest::Receive_Cell_BroadCast_0003 -->");
    int32_t slotId = DEFAULT_SIM_SLOT_ID_1;
    if (!(CbGtest::HasSimCard(slotId))) {
        TELEPHONY_LOGI("TelephonyTestService has no sim card");
        ASSERT_TRUE(true);
        return;
    }
    SmsMmsTestHelper helper;
    helper.slotId = slotId;

    EventFwk::MatchingSkills matchingSkills;
    matchingSkills.AddEvent(EventFwk::CommonEventSupport::COMMON_EVENT_SMS_EMERGENCY_CB_RECEIVE_COMPLETED);
    matchingSkills.AddEvent(EventFwk::CommonEventSupport::COMMON_EVENT_SMS_CB_RECEIVE_COMPLETED);
    EventFwk::CommonEventSubscribeInfo subscriberInfo(matchingSkills);
    std::shared_ptr<SmsBroadcastSubscriberGtest> subscriberTest =
        std::make_shared<SmsBroadcastSubscriberGtest>(subscriberInfo, helper);
    if (subscriberTest == nullptr) {
        ASSERT_TRUE(false);
        return;
    }
    bool subscribeResult = EventFwk::CommonEventManager::SubscribeCommonEvent(subscriberTest);
    TELEPHONY_LOGI("subscribeResult is : %{public}d", subscribeResult);

    if (!helper.Run(ReceiveCellBroadCastTestFunc, helper)) {
        TELEPHONY_LOGI("ReceiveCellBroadCastTestFunc out of time");
        ASSERT_TRUE(true);
        return;
    }
    TELEPHONY_LOGI("TelSMSMMSTest::Receive_Cell_BroadCast_0003 -->finished");
    EXPECT_TRUE(helper.GetBoolResult());
}

/**
 * @tc.number   Telephony_SmsMmsGtest_Receive_Cell_BroadCast_0004
 * @tc.name     Receive a 2g Cell Broadcast
 * @tc.desc     Function test
 */
HWTEST_F(CbGtest, Receive_Cell_BroadCast_0004, Function | MediumTest | Level2)
{
    TELEPHONY_LOGI("TelSMSMMSTest::Receive_Cell_BroadCast_0004 -->");
    int32_t slotId = DEFAULT_SIM_SLOT_ID_1;
    if (!(CbGtest::HasSimCard(slotId))) {
        TELEPHONY_LOGI("TelephonyTestService has no sim card");
        ASSERT_TRUE(true);
        return;
    }
    SmsMmsTestHelper helper;
    helper.slotId = slotId;

    EventFwk::MatchingSkills matchingSkills;
    matchingSkills.AddEvent(EventFwk::CommonEventSupport::COMMON_EVENT_SMS_EMERGENCY_CB_RECEIVE_COMPLETED);
    matchingSkills.AddEvent(EventFwk::CommonEventSupport::COMMON_EVENT_SMS_CB_RECEIVE_COMPLETED);
    EventFwk::CommonEventSubscribeInfo subscriberInfo(matchingSkills);
    std::shared_ptr<SmsBroadcastSubscriberGtest> subscriberTest =
        std::make_shared<SmsBroadcastSubscriberGtest>(subscriberInfo, helper);
    if (subscriberTest == nullptr) {
        ASSERT_TRUE(false);
        return;
    }
    bool subscribeResult = EventFwk::CommonEventManager::SubscribeCommonEvent(subscriberTest);
    TELEPHONY_LOGI("subscribeResult is : %{public}d", subscribeResult);

    if (!helper.Run(ReceiveCellBroadCastTestFunc2, helper)) {
        TELEPHONY_LOGI("ReceiveCellBroadCastTestFunc2 out of time");
        ASSERT_TRUE(true);
        return;
    }
    TELEPHONY_LOGI("TelSMSMMSTest::Receive_Cell_BroadCast_0004 -->finished");
    EXPECT_TRUE(helper.GetBoolResult());
}
#endif // TEL_TEST_UNSUPPORT
} // namespace Telephony
} // namespace OHOS