/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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
#ifndef SMS_MMS_GTEST_H
#define SMS_MMS_GTEST_H

#include "accesstoken_kit.h"
#include "core_service_client.h"
#include "gtest/gtest.h"
#include "i_sms_service_interface.h"
#include "if_system_ability_manager.h"
#include "ipc_skeleton.h"
#include "iservice_registry.h"
#include "sms_mms_test_helper.h"
#include "sms_service_proxy.h"
#include "string_utils.h"
#include "system_ability_definition.h"
#include "telephony_log_wrapper.h"
#include "telephony_types.h"
#include "token_setproc.h"

namespace OHOS {
namespace Telephony {
namespace {
sptr<ISmsServiceInterface> g_telephonyService = nullptr;
} // namespace
using namespace testing::ext;
using namespace Security::AccessToken;
using Security::AccessToken::AccessTokenID;

HapInfoParams testMmsInfoParams = {
    .bundleName = "tel_sms_mms_gtest",
    .userID = 1,
    .instIndex = 0,
    .appIDDesc = "test",
};

PermissionDef testPermReceiveSmsDef = {
    .permissionName = "ohos.permission.RECEIVE_SMS",
    .bundleName = "tel_sms_mms_gtest",
    .grantMode = 1, // SYSTEM_GRANT
    .label = "label",
    .labelId = 1,
    .description = "Test sms manager",
    .descriptionId = 1,
    .availableLevel = APL_SYSTEM_BASIC,
};

PermissionStateFull testReceiveSmsState = {
    .grantFlags = { 2 }, // PERMISSION_USER_SET
    .grantStatus = { PermissionState::PERMISSION_GRANTED },
    .isGeneral = true,
    .permissionName = "ohos.permission.RECEIVE_SMS",
    .resDeviceID = { "local" },
};

PermissionDef testPermSendSmsDef = {
    .permissionName = "ohos.permission.SEND_MESSAGES",
    .bundleName = "tel_sms_mms_gtest",
    .grantMode = 1, // SYSTEM_GRANT
    .label = "label",
    .labelId = 1,
    .description = "Test sms manager",
    .descriptionId = 1,
    .availableLevel = APL_SYSTEM_BASIC,
};

PermissionStateFull testSendSmsState = {
    .grantFlags = { 2 }, // PERMISSION_USER_SET
    .grantStatus = { PermissionState::PERMISSION_GRANTED },
    .isGeneral = true,
    .permissionName = "ohos.permission.SEND_MESSAGES",
    .resDeviceID = { "local" },
};

PermissionDef testPermSetTelephonyDef = {
    .permissionName = "ohos.permission.SET_TELEPHONY_STATE",
    .bundleName = "tel_sms_mms_gtest",
    .grantMode = 1, // SYSTEM_GRANT
    .label = "label",
    .labelId = 1,
    .description = "Test sms manager",
    .descriptionId = 1,
    .availableLevel = APL_SYSTEM_BASIC,
};

PermissionStateFull testSetTelephonyState = {
    .grantFlags = { 2 }, // PERMISSION_USER_SET
    .grantStatus = { PermissionState::PERMISSION_GRANTED },
    .isGeneral = true,
    .permissionName = "ohos.permission.SET_TELEPHONY_STATE",
    .resDeviceID = { "local" },
};

PermissionDef testPermGetTelephonyDef = {
    .permissionName = "ohos.permission.GET_TELEPHONY_STATE",
    .bundleName = "tel_sms_mms_gtest",
    .grantMode = 1, // SYSTEM_GRANT
    .label = "label",
    .labelId = 1,
    .description = "Test sms manager",
    .descriptionId = 1,
    .availableLevel = APL_SYSTEM_BASIC,
};

PermissionStateFull testGetTelephonyState = {
    .grantFlags = { 2 }, // PERMISSION_USER_SET
    .grantStatus = { PermissionState::PERMISSION_GRANTED },
    .isGeneral = true,
    .permissionName = "ohos.permission.GET_TELEPHONY_STATE",
    .resDeviceID = { "local" },
};

HapPolicyParams testMmsPolicyParams = {
    .apl = APL_SYSTEM_BASIC,
    .domain = "test.domain",
    .permList = { testPermReceiveSmsDef, testPermSendSmsDef, testPermSetTelephonyDef, testPermGetTelephonyDef },
    .permStateList = { testReceiveSmsState, testSendSmsState, testSetTelephonyState, testGetTelephonyState },
};

class AccessMmsToken {
public:
    AccessMmsToken()
    {
        currentID_ = GetSelfTokenID();
        TELEPHONY_LOGI("AccessMmsToken currentID_%{public}d", currentID_);
        AccessTokenIDEx tokenIdEx = AccessTokenKit::AllocHapToken(testMmsInfoParams, testMmsPolicyParams);
        accessID_ = tokenIdEx.tokenIdExStruct.tokenID;
        SetSelfTokenID(accessID_);
    }
    ~AccessMmsToken()
    {
        TELEPHONY_LOGI("AccessMmsToken  ~AccessMmsToken");
        AccessTokenKit::DeleteToken(accessID_);
        SetSelfTokenID(currentID_);
    }

private:
    AccessTokenID currentID_ = 0;
    AccessTokenID accessID_ = 0;
};

class SmsMmsGtest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    static sptr<ISmsServiceInterface> GetProxy();
    static bool HasSimCard(int32_t slotId)
    {
        if (CoreServiceClient::GetInstance().GetProxy() == nullptr) {
            return false;
        }
        return CoreServiceClient::GetInstance().HasSimCard(slotId);
    }
};

void SmsMmsGtest::TearDownTestCase() {}

void SmsMmsGtest::SetUp() {}

void SmsMmsGtest::TearDown() {}

const int32_t DEFAULT_SIM_SLOT_ID_1 = 1;

void SmsMmsGtest::SetUpTestCase()
{
    TELEPHONY_LOGI("SetUpTestCase slotId%{public}d", DEFAULT_SIM_SLOT_ID_1);
    g_telephonyService = GetProxy();
    if (g_telephonyService == nullptr) {
        return;
    }
}

sptr<ISmsServiceInterface> SmsMmsGtest::GetProxy()
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
/**
 * @tc.number   Telephony_SmsMmsGtest_GetProxy_0001
 * @tc.name     Get SmsMms service
 * @tc.desc     Function test
 */
HWTEST_F(SmsMmsGtest, GetProxy_0001, Function | MediumTest | Level0)
{
    TELEPHONY_LOGI("TelSMSMMSTest::GetProxy_0001 -->");
    g_telephonyService = SmsMmsGtest::GetProxy();
    TELEPHONY_LOGI("TelSMSMMSTest::GetProxy_0001 -->finished");
    EXPECT_FALSE(g_telephonyService == nullptr);
}

void OpenCellBroadcastTestFuc(SmsMmsTestHelper &helper)
{
    bool result = true;
    bool enable = true;
    uint32_t fromMsgId = 0;
    uint32_t toMsgId = 10;
    uint8_t netType = 1;
    result = g_telephonyService->SetCBConfig(helper.slotId, enable, fromMsgId, toMsgId, netType);
    helper.SetBoolResult(result);
    helper.NotifyAll();
}

/**
 * @tc.number   Telephony_SmsMmsGtest_OpenCellBroadcast_0001
 * @tc.name     Open cellBroadcast slotId is -1
 * @tc.desc     Function test
 */
HWTEST_F(SmsMmsGtest, OpenCellBroadcast_0001, Function | MediumTest | Level1)
{
    AccessMmsToken token;
    TELEPHONY_LOGI("TelSMSMMSTest::OpenCellBroadcast_0001 -->");
    if (g_telephonyService == nullptr) {
        TELEPHONY_LOGI("TelephonyTestService Remote service is null");
        g_telephonyService = SmsMmsGtest::GetProxy();
        ASSERT_TRUE(false);
        return;
    }
    SmsMmsTestHelper helper;
    helper.slotId = DEFAULT_SIM_SLOT_ID_REMOVE;
    if (!helper.Run(OpenCellBroadcastTestFuc, std::ref(helper))) {
        TELEPHONY_LOGI("OpenCellBroadcastTestFuc out of time");
        ASSERT_TRUE(false);
    }
    TELEPHONY_LOGI("TelSMSMMSTest::OpenCellBroadcast_0001 -->finished");
    EXPECT_FALSE(helper.GetBoolResult());
}

void OpenCellBroadcastTestFuc2(SmsMmsTestHelper &helper)
{
    bool result = true;
    bool enable = true;
    uint32_t fromMsgId = 20;
    uint32_t toMsgId = 10;
    uint8_t netType = 1;
    result = g_telephonyService->SetCBConfig(helper.slotId, enable, fromMsgId, toMsgId, netType);
    helper.SetBoolResult(result);
    helper.NotifyAll();
}

/**
 * @tc.number   Telephony_SmsMmsGtest_OpenCellBroadcast_0002
 * @tc.name     Open cellBroadcast fromMsgId less than toMsgId
 * @tc.desc     Function test
 */
HWTEST_F(SmsMmsGtest, OpenCellBroadcast_0002, Function | MediumTest | Level2)
{
    AccessMmsToken token;
    TELEPHONY_LOGI("TelSMSMMSTest::OpenCellBroadcast_0002 -->");
    if (g_telephonyService == nullptr || !(SmsMmsGtest::HasSimCard(DEFAULT_SIM_SLOT_ID))) {
        TELEPHONY_LOGI("TelephonyTestService Remote service is null");
        g_telephonyService = SmsMmsGtest::GetProxy();
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
    EXPECT_FALSE(helper.GetBoolResult());
}

void OpenCellBroadcastTestFuc3(SmsMmsTestHelper &helper)
{
    bool result = true;
    bool enable = true;
    uint32_t fromMsgId = 0;
    uint32_t toMsgId = 10;
    uint8_t netType = 3;
    result = g_telephonyService->SetCBConfig(helper.slotId, enable, fromMsgId, toMsgId, netType);
    helper.SetBoolResult(result);
    helper.NotifyAll();
}

/**
 * @tc.number   Telephony_SmsMmsGtest_OpenCellBroadcast_0003
 * @tc.name     Open cellBroadcast netType is unknown.
 * @tc.desc     Function test
 */
HWTEST_F(SmsMmsGtest, OpenCellBroadcast_0003, Function | MediumTest | Level3)
{
    AccessMmsToken token;
    TELEPHONY_LOGI("TelSMSMMSTest::OpenCellBroadcast_0003 -->");
    if (g_telephonyService == nullptr || !(SmsMmsGtest::HasSimCard(DEFAULT_SIM_SLOT_ID))) {
        TELEPHONY_LOGI("TelephonyTestService Remote service is null");
        g_telephonyService = SmsMmsGtest::GetProxy();
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
    EXPECT_FALSE(helper.GetBoolResult());
}

void OpenCellBroadcastTestFuc4(SmsMmsTestHelper &helper)
{
    bool result = false;
    bool enable = true;
    uint32_t fromMsgId = 0;
    uint32_t toMsgId = 10;
    uint8_t netType = 1;
    result = g_telephonyService->SetCBConfig(helper.slotId, enable, fromMsgId, toMsgId, netType);
    helper.SetBoolResult(result);
    helper.NotifyAll();
}

/**
 * @tc.number   Telephony_SmsMmsGtest_OpenCellBroadcast_0004
 * @tc.name     Open cellBroadcast parameter is valid.
 * @tc.desc     Function test
 */
HWTEST_F(SmsMmsGtest, OpenCellBroadcast_0004, Function | MediumTest | Level3)
{
    AccessMmsToken token;
    TELEPHONY_LOGI("TelSMSMMSTest::OpenCellBroadcast_0004 -->");
    if (g_telephonyService == nullptr || !(SmsMmsGtest::HasSimCard(DEFAULT_SIM_SLOT_ID))) {
        TELEPHONY_LOGI("TelephonyTestService Remote service is null");
        g_telephonyService = SmsMmsGtest::GetProxy();
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
    ASSERT_TRUE(helper.GetBoolResult());
}

void OpenCellBroadcastTestFuc5(SmsMmsTestHelper &helper)
{
    bool result = false;
    bool enable = true;
    uint32_t fromMsgId = 0;
    uint32_t toMsgId = 1000;
    uint8_t netType = 1;
    result = g_telephonyService->SetCBConfig(helper.slotId, enable, fromMsgId, toMsgId, netType);
    helper.SetBoolResult(result);
    helper.NotifyAll();
}

/**
 * @tc.number   Telephony_SmsMmsGtest_OpenCellBroadcast_0005
 * @tc.name     Open cellBroadcast parameter is valid
 * @tc.desc     Function test
 */
HWTEST_F(SmsMmsGtest, OpenCellBroadcast_0005, Function | MediumTest | Level3)
{
    AccessMmsToken token;
    TELEPHONY_LOGI("TelSMSMMSTest::OpenCellBroadcast_0005 -->");
    if (g_telephonyService == nullptr || !(SmsMmsGtest::HasSimCard(DEFAULT_SIM_SLOT_ID))) {
        TELEPHONY_LOGI("TelephonyTestService Remote service is null");
        g_telephonyService = SmsMmsGtest::GetProxy();
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
    ASSERT_TRUE(helper.GetBoolResult());
}

void OpenCellBroadcastTestFuc6(SmsMmsTestHelper &helper)
{
    bool result = false;
    bool enable = true;
    uint32_t fromMsgId = 0;
    uint32_t toMsgId = 0;
    uint8_t netType = 1;
    result = g_telephonyService->SetCBConfig(helper.slotId, enable, fromMsgId, toMsgId, netType);
    helper.SetBoolResult(result);
    helper.NotifyAll();
}

/**
 * @tc.number   Telephony_SmsMmsGtest_OpenCellBroadcast_0006
 * @tc.name     Open cellBroadcast parameter is valid
 * @tc.desc     Function test
 */
HWTEST_F(SmsMmsGtest, OpenCellBroadcast_0006, Function | MediumTest | Level4)
{
    AccessMmsToken token;
    TELEPHONY_LOGI("TelSMSMMSTest::OpenCellBroadcast_0006 -->");
    if (g_telephonyService == nullptr || !(SmsMmsGtest::HasSimCard(DEFAULT_SIM_SLOT_ID))) {
        TELEPHONY_LOGI("TelephonyTestService Remote service is null");
        g_telephonyService = SmsMmsGtest::GetProxy();
        ASSERT_TRUE(true);
        return;
    }
    SmsMmsTestHelper helper;
    helper.slotId = DEFAULT_SIM_SLOT_ID;
    if (!helper.Run(OpenCellBroadcastTestFuc6, std::ref(helper))) {
        TELEPHONY_LOGI("OpenCellBroadcastTestFuc6 out of time");
        ASSERT_TRUE(false);
    }
    TELEPHONY_LOGI("TelSMSMMSTest::OpenCellBroadcast_0006 -->finished");
    ASSERT_TRUE(helper.GetBoolResult());
}

/**
 * @tc.number   Telephony_SmsMmsGtest_OpenCellBroadcast_0007
 * @tc.name     Open cellBroadcast fromMsgId less than toMsgId
 * @tc.desc     Function test
 */
HWTEST_F(SmsMmsGtest, OpenCellBroadcast_0007, Function | MediumTest | Level2)
{
    AccessMmsToken token;
    TELEPHONY_LOGI("TelSMSMMSTest::OpenCellBroadcast_0007 -->");
    if (g_telephonyService == nullptr || !(SmsMmsGtest::HasSimCard(DEFAULT_SIM_SLOT_ID_1))) {
        TELEPHONY_LOGI("TelephonyTestService Remote service is null");
        g_telephonyService = SmsMmsGtest::GetProxy();
        ASSERT_TRUE(true);
        return;
    }
    SmsMmsTestHelper helper;
    helper.slotId = DEFAULT_SIM_SLOT_ID_1;
    if (!helper.Run(OpenCellBroadcastTestFuc2, std::ref(helper))) {
        TELEPHONY_LOGI("OpenCellBroadcastTestFuc7 out of time");
        ASSERT_TRUE(false);
    }
    TELEPHONY_LOGI("TelSMSMMSTest::OpenCellBroadcast_0007 -->finished");
    EXPECT_FALSE(helper.GetBoolResult());
}

/**
 * @tc.number   Telephony_SmsMmsGtest_OpenCellBroadcast_0008
 * @tc.name     Open cellBroadcast netType is unknown.
 * @tc.desc     Function test
 */
HWTEST_F(SmsMmsGtest, OpenCellBroadcast_0008, Function | MediumTest | Level3)
{
    AccessMmsToken token;
    TELEPHONY_LOGI("TelSMSMMSTest::OpenCellBroadcast_0008 -->");
    if (g_telephonyService == nullptr || !(SmsMmsGtest::HasSimCard(DEFAULT_SIM_SLOT_ID_1))) {
        TELEPHONY_LOGI("TelephonyTestService Remote service is null");
        g_telephonyService = SmsMmsGtest::GetProxy();
        ASSERT_TRUE(true);
        return;
    }
    SmsMmsTestHelper helper;
    helper.slotId = DEFAULT_SIM_SLOT_ID_1;
    if (!helper.Run(OpenCellBroadcastTestFuc3, std::ref(helper))) {
        TELEPHONY_LOGI("OpenCellBroadcastTestFuc8 out of time");
        ASSERT_TRUE(false);
    }
    TELEPHONY_LOGI("TelSMSMMSTest::OpenCellBroadcast_0008 -->finished");
    EXPECT_FALSE(helper.GetBoolResult());
}

/**
 * @tc.number   Telephony_SmsMmsGtest_OpenCellBroadcast_0009
 * @tc.name     Open cellBroadcast parameter is valid.
 * @tc.desc     Function test
 */
HWTEST_F(SmsMmsGtest, OpenCellBroadcast_0009, Function | MediumTest | Level3)
{
    AccessMmsToken token;
    TELEPHONY_LOGI("TelSMSMMSTest::OpenCellBroadcast_0009 -->");
    if (g_telephonyService == nullptr || !(SmsMmsGtest::HasSimCard(DEFAULT_SIM_SLOT_ID_1))) {
        TELEPHONY_LOGI("TelephonyTestService Remote service is null");
        g_telephonyService = SmsMmsGtest::GetProxy();
        ASSERT_TRUE(true);
        return;
    }
    SmsMmsTestHelper helper;
    helper.slotId = DEFAULT_SIM_SLOT_ID_1;
    if (!helper.Run(OpenCellBroadcastTestFuc4, std::ref(helper))) {
        TELEPHONY_LOGI("OpenCellBroadcastTestFuc9 out of time");
        ASSERT_TRUE(false);
    }
    TELEPHONY_LOGI("TelSMSMMSTest::OpenCellBroadcast_0009 -->finished");
    ASSERT_TRUE(helper.GetBoolResult());
}

/**
 * @tc.number   Telephony_SmsMmsGtest_OpenCellBroadcast_0010
 * @tc.name     Open cellBroadcast parameter is valid
 * @tc.desc     Function test
 */
HWTEST_F(SmsMmsGtest, OpenCellBroadcast_00010, Function | MediumTest | Level3)
{
    AccessMmsToken token;
    TELEPHONY_LOGI("TelSMSMMSTest::OpenCellBroadcast_0010 -->");
    if (g_telephonyService == nullptr || !(SmsMmsGtest::HasSimCard(DEFAULT_SIM_SLOT_ID_1))) {
        TELEPHONY_LOGI("TelephonyTestService Remote service is null");
        g_telephonyService = SmsMmsGtest::GetProxy();
        ASSERT_TRUE(true);
        return;
    }
    SmsMmsTestHelper helper;
    helper.slotId = DEFAULT_SIM_SLOT_ID_1;
    if (!helper.Run(OpenCellBroadcastTestFuc5, std::ref(helper))) {
        TELEPHONY_LOGI("OpenCellBroadcastTestFuc10 out of time");
        ASSERT_TRUE(false);
    }
    TELEPHONY_LOGI("TelSMSMMSTest::OpenCellBroadcast_0010 -->finished");
    ASSERT_TRUE(helper.GetBoolResult());
}

/**
 * @tc.number   Telephony_SmsMmsGtest_OpenCellBroadcast_0011
 * @tc.name     Open cellBroadcast parameter is valid
 * @tc.desc     Function test
 */
HWTEST_F(SmsMmsGtest, OpenCellBroadcast_0011, Function | MediumTest | Level4)
{
    AccessMmsToken token;
    TELEPHONY_LOGI("TelSMSMMSTest::OpenCellBroadcast_0011 -->");
    if (g_telephonyService == nullptr || !(SmsMmsGtest::HasSimCard(DEFAULT_SIM_SLOT_ID_1))) {
        TELEPHONY_LOGI("TelephonyTestService Remote service is null");
        g_telephonyService = SmsMmsGtest::GetProxy();
        ASSERT_TRUE(true);
        return;
    }
    SmsMmsTestHelper helper;
    helper.slotId = DEFAULT_SIM_SLOT_ID_1;
    if (!helper.Run(OpenCellBroadcastTestFuc6, std::ref(helper))) {
        TELEPHONY_LOGI("OpenCellBroadcastTestFuc11 out of time");
        ASSERT_TRUE(false);
    }
    TELEPHONY_LOGI("TelSMSMMSTest::OpenCellBroadcast_0011 -->finished");
    ASSERT_TRUE(helper.GetBoolResult());
}

void CloseCellBroadcastTestFuc(SmsMmsTestHelper &helper)
{
    bool result = true;
    bool enable = false;
    uint32_t fromMsgId = 0;
    uint32_t toMsgId = 10;
    uint8_t netType = 1;
    result = g_telephonyService->SetCBConfig(helper.slotId, enable, fromMsgId, toMsgId, netType);
    helper.SetBoolResult(result);
    helper.NotifyAll();
}

/**
 * @tc.number   Telephony_SmsMmsGtest_CloseCellBroadcast_0001
 * @tc.name     Close cellBroadcast slotId is -1
 * @tc.desc     Function test
 */
HWTEST_F(SmsMmsGtest, CloseCellBroadcast_0001, Function | MediumTest | Level1)
{
    AccessMmsToken token;
    TELEPHONY_LOGI("TelSMSMMSTest::CloseCellBroadcast_0001 -->");
    if (g_telephonyService == nullptr) {
        TELEPHONY_LOGI("TelephonyTestService Remote service is null");
        g_telephonyService = SmsMmsGtest::GetProxy();
        ASSERT_TRUE(false);
        return;
    }
    SmsMmsTestHelper helper;
    helper.slotId = DEFAULT_SIM_SLOT_ID_REMOVE;
    if (!helper.Run(CloseCellBroadcastTestFuc, std::ref(helper))) {
        TELEPHONY_LOGI("CloseCellBroadcastTestFuc out of time");
        ASSERT_TRUE(false);
    }
    TELEPHONY_LOGI("TelSMSMMSTest::CloseCellBroadcast_0001 -->finished");
    EXPECT_FALSE(helper.GetBoolResult());
}

void CloseCellBroadcastTestFuc2(SmsMmsTestHelper &helper)
{
    bool result = true;
    bool enable = false;
    uint32_t fromMsgId = 20;
    uint32_t toMsgId = 10;
    uint8_t netType = 1;
    result = g_telephonyService->SetCBConfig(helper.slotId, enable, fromMsgId, toMsgId, netType);
    helper.SetBoolResult(result);
    helper.NotifyAll();
}

/**
 * @tc.number   Telephony_SmsMmsGtest_CloseCellBroadcast_0002
 * @tc.name     Close cellBroadcast fromMsgId less than toMsgId
 * @tc.desc     Function test
 */
HWTEST_F(SmsMmsGtest, CloseCellBroadcast_0002, Function | MediumTest | Level2)
{
    AccessMmsToken token;
    TELEPHONY_LOGI("TelSMSMMSTest::CloseCellBroadcast_0002 -->");
    if (g_telephonyService == nullptr || !(SmsMmsGtest::HasSimCard(DEFAULT_SIM_SLOT_ID))) {
        TELEPHONY_LOGI("TelephonyTestService Remote service is null");
        g_telephonyService = SmsMmsGtest::GetProxy();
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
    EXPECT_FALSE(helper.GetBoolResult());
}

void CloseCellBroadcastTestFuc3(SmsMmsTestHelper &helper)
{
    bool result = true;
    bool enable = false;
    uint32_t fromMsgId = 0;
    uint32_t toMsgId = 10;
    uint8_t netType = 3;
    result = g_telephonyService->SetCBConfig(helper.slotId, enable, fromMsgId, toMsgId, netType);
    helper.SetBoolResult(result);
    helper.NotifyAll();
}

/**
 * @tc.number   Telephony_SmsMmsGtest_CloseCellBroadcast_0003
 * @tc.name     Close cellBroadcast netType is unknown
 * @tc.desc     Function test
 */
HWTEST_F(SmsMmsGtest, CloseCellBroadcast_0003, Function | MediumTest | Level3)
{
    AccessMmsToken token;
    TELEPHONY_LOGI("TelSMSMMSTest::CloseCellBroadcast_0003 -->");
    if (g_telephonyService == nullptr || !(SmsMmsGtest::HasSimCard(DEFAULT_SIM_SLOT_ID))) {
        TELEPHONY_LOGI("TelephonyTestService Remote service is null");
        g_telephonyService = SmsMmsGtest::GetProxy();
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
    EXPECT_FALSE(helper.GetBoolResult());
}

void CloseCellBroadcastTestFuc4(SmsMmsTestHelper &helper)
{
    bool result = false;
    bool enable = false;
    uint32_t fromMsgId = 0;
    uint32_t toMsgId = 10;
    uint8_t netType = 1;
    result = g_telephonyService->SetCBConfig(helper.slotId, enable, fromMsgId, toMsgId, netType);
    helper.SetBoolResult(result);
    helper.NotifyAll();
}

/**
 * @tc.number   Telephony_SmsMmsGtest_CloseCellBroadcast_0004
 * @tc.name     Close cellBroadcast parameter is valid
 * @tc.desc     Function test
 */
HWTEST_F(SmsMmsGtest, CloseCellBroadcast_0004, Function | MediumTest | Level3)
{
    AccessMmsToken token;
    TELEPHONY_LOGI("TelSMSMMSTest::CloseCellBroadcast_0004 -->");
    if (g_telephonyService == nullptr || !(SmsMmsGtest::HasSimCard(DEFAULT_SIM_SLOT_ID))) {
        TELEPHONY_LOGI("TelephonyTestService Remote service is null");
        g_telephonyService = SmsMmsGtest::GetProxy();
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
    ASSERT_TRUE(helper.GetBoolResult());
}

void CloseCellBroadcastTestFuc5(SmsMmsTestHelper &helper)
{
    bool result = false;
    bool enable = false;
    uint32_t fromMsgId = 0;
    uint32_t toMsgId = 1000;
    uint8_t netType = 1;
    result = g_telephonyService->SetCBConfig(helper.slotId, enable, fromMsgId, toMsgId, netType);
    helper.SetBoolResult(result);
    helper.NotifyAll();
}

/**
 * @tc.number   Telephony_SmsMmsGtest_CloseCellBroadcast_0005
 * @tc.name     Close cellBroadcast parameter is valid
 * @tc.desc     Function test
 */
HWTEST_F(SmsMmsGtest, CloseCellBroadcast_0005, Function | MediumTest | Level3)
{
    AccessMmsToken token;
    TELEPHONY_LOGI("TelSMSMMSTest::CloseCellBroadcast_0005 -->");
    if (g_telephonyService == nullptr || !(SmsMmsGtest::HasSimCard(DEFAULT_SIM_SLOT_ID))) {
        TELEPHONY_LOGI("TelephonyTestService Remote service is null");
        g_telephonyService = SmsMmsGtest::GetProxy();
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
    ASSERT_TRUE(helper.GetBoolResult());
}

void CloseCellBroadcastTestFuc6(SmsMmsTestHelper &helper)
{
    bool result = false;
    bool enable = false;
    uint32_t fromMsgId = 0;
    uint32_t toMsgId = 0;
    uint8_t netType = 1;
    result = g_telephonyService->SetCBConfig(helper.slotId, enable, fromMsgId, toMsgId, netType);
    helper.SetBoolResult(result);
    helper.NotifyAll();
}

/**
 * @tc.number   Telephony_SmsMmsGtest_CloseCellBroadcast_0006
 * @tc.name     Close cellBroadcast parameter is valid
 * @tc.desc     Function test
 */
HWTEST_F(SmsMmsGtest, CloseCellBroadcast_0006, Function | MediumTest | Level4)
{
    AccessMmsToken token;
    TELEPHONY_LOGI("TelSMSMMSTest::CloseCellBroadcast_0006 -->");
    if (g_telephonyService == nullptr || !(SmsMmsGtest::HasSimCard(DEFAULT_SIM_SLOT_ID))) {
        TELEPHONY_LOGI("TelephonyTestService Remote service is null");
        g_telephonyService = SmsMmsGtest::GetProxy();
        ASSERT_TRUE(true);
        return;
    }
    SmsMmsTestHelper helper;
    helper.slotId = DEFAULT_SIM_SLOT_ID;
    if (!helper.Run(CloseCellBroadcastTestFuc6, std::ref(helper))) {
        TELEPHONY_LOGI("CloseCellBroadcastTestFuc6 out of time");
        ASSERT_TRUE(false);
    }
    TELEPHONY_LOGI("TelSMSMMSTest::CloseCellBroadcast_0006 -->finished");
    ASSERT_TRUE(helper.GetBoolResult());
}

/**
 * @tc.number   Telephony_SmsMmsGtest_CloseCellBroadcast_0007
 * @tc.name     Close cellBroadcast fromMsgId less than toMsgId
 * @tc.desc     Function test
 */
HWTEST_F(SmsMmsGtest, CloseCellBroadcast_0007, Function | MediumTest | Level2)
{
    AccessMmsToken token;
    TELEPHONY_LOGI("TelSMSMMSTest::CloseCellBroadcast_0007 -->");
    if (g_telephonyService == nullptr || !(SmsMmsGtest::HasSimCard(DEFAULT_SIM_SLOT_ID_1))) {
        TELEPHONY_LOGI("TelephonyTestService Remote service is null");
        g_telephonyService = SmsMmsGtest::GetProxy();
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
    EXPECT_FALSE(helper.GetBoolResult());
}

/**
 * @tc.number   Telephony_SmsMmsGtest_CloseCellBroadcast_0008
 * @tc.name     Close cellBroadcast netType is unknown
 * @tc.desc     Function test
 */
HWTEST_F(SmsMmsGtest, CloseCellBroadcast_0008, Function | MediumTest | Level3)
{
    AccessMmsToken token;
    TELEPHONY_LOGI("TelSMSMMSTest::CloseCellBroadcast_0008 -->");
    if (g_telephonyService == nullptr || !(SmsMmsGtest::HasSimCard(DEFAULT_SIM_SLOT_ID_1))) {
        TELEPHONY_LOGI("TelephonyTestService Remote service is null");
        g_telephonyService = SmsMmsGtest::GetProxy();
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
    EXPECT_FALSE(helper.GetBoolResult());
}

/**
 * @tc.number   Telephony_SmsMmsGtest_CloseCellBroadcast_0009
 * @tc.name     Close cellBroadcast parameter is valid
 * @tc.desc     Function test
 */
HWTEST_F(SmsMmsGtest, CloseCellBroadcast_0009, Function | MediumTest | Level3)
{
    AccessMmsToken token;
    TELEPHONY_LOGI("TelSMSMMSTest::CloseCellBroadcast_0009 -->");
    if (g_telephonyService == nullptr || !(SmsMmsGtest::HasSimCard(DEFAULT_SIM_SLOT_ID_1))) {
        TELEPHONY_LOGI("TelephonyTestService Remote service is null");
        g_telephonyService = SmsMmsGtest::GetProxy();
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
    ASSERT_TRUE(helper.GetBoolResult());
}

/**
 * @tc.number   Telephony_SmsMmsGtest_CloseCellBroadcast_0010
 * @tc.name     Close cellBroadcast parameter is valid
 * @tc.desc     Function test
 */
HWTEST_F(SmsMmsGtest, CloseCellBroadcast_0010, Function | MediumTest | Level3)
{
    AccessMmsToken token;
    TELEPHONY_LOGI("TelSMSMMSTest::CloseCellBroadcast_0010 -->");
    if (g_telephonyService == nullptr || !(SmsMmsGtest::HasSimCard(DEFAULT_SIM_SLOT_ID_1))) {
        TELEPHONY_LOGI("TelephonyTestService Remote service is null");
        g_telephonyService = SmsMmsGtest::GetProxy();
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
    ASSERT_TRUE(helper.GetBoolResult());
}

/**
 * @tc.number   Telephony_SmsMmsGtest_CloseCellBroadcast_0011
 * @tc.name     Close cellBroadcast parameter is valid
 * @tc.desc     Function test
 */
HWTEST_F(SmsMmsGtest, CloseCellBroadcast_00011, Function | MediumTest | Level4)
{
    AccessMmsToken token;
    TELEPHONY_LOGI("TelSMSMMSTest::CloseCellBroadcast_0011 -->");
    if (g_telephonyService == nullptr || !(SmsMmsGtest::HasSimCard(DEFAULT_SIM_SLOT_ID_1))) {
        TELEPHONY_LOGI("TelephonyTestService Remote service is null");
        g_telephonyService = SmsMmsGtest::GetProxy();
        ASSERT_TRUE(true);
        return;
    }
    SmsMmsTestHelper helper;
    helper.slotId = DEFAULT_SIM_SLOT_ID_1;
    if (!helper.Run(CloseCellBroadcastTestFuc6, std::ref(helper))) {
        TELEPHONY_LOGI("CloseCellBroadcastTestFuc11 out of time");
        ASSERT_TRUE(false);
    }
    TELEPHONY_LOGI("TelSMSMMSTest::CloseCellBroadcast_0011 -->finished");
    ASSERT_TRUE(helper.GetBoolResult());
}

void SetDefaultSmsSlotIdTestFuc(SmsMmsTestHelper &helper)
{
    bool result = false;
    result = g_telephonyService->SetDefaultSmsSlotId(helper.slotId);
    helper.SetBoolResult(result);
    helper.NotifyAll();
}

/**
 * @tc.number   Telephony_SmsMmsGtest_SetDefaultSmsSlotId_0001
 * @tc.name     Set Default Sms SlotId slotId is valid
 * @tc.desc     Function test
 */
HWTEST_F(SmsMmsGtest, SetDefaultSmsSlotId_0001, Function | MediumTest | Level3)
{
    AccessMmsToken token;
    TELEPHONY_LOGI("TelSMSMMSTest::SetDefaultSmsSlotId_0001 -->");
    if ((g_telephonyService == nullptr) || !(SmsMmsGtest::HasSimCard(DEFAULT_SIM_SLOT_ID_1))) {
        TELEPHONY_LOGI("TelephonyTestService Remote service is null");
        g_telephonyService = SmsMmsGtest::GetProxy();
        ASSERT_TRUE(true);
        return;
    }
    SmsMmsTestHelper helper;
    helper.slotId = DEFAULT_SIM_SLOT_ID;
    if (!helper.Run(SetDefaultSmsSlotIdTestFuc, std::ref(helper))) {
        TELEPHONY_LOGI("SetDefaultSmsSlotIdTestFuc out of time");
        ASSERT_TRUE(false);
    }
    TELEPHONY_LOGI("TelSMSMMSTest::SetDefaultSmsSlotId_0001 -->finished");
    ASSERT_TRUE(helper.GetBoolResult());
}

/**
 * @tc.number   Telephony_SmsMmsGtest_SetDefaultSmsSlotId_0002
 * @tc.name     Set Default Sms SlotId slotId is valid
 * @tc.desc     Function test
 */
HWTEST_F(SmsMmsGtest, SetDefaultSmsSlotId_0002, Function | MediumTest | Level3)
{
    AccessMmsToken token;
    TELEPHONY_LOGI("TelSMSMMSTest::SetDefaultSmsSlotId_0002 -->");
    if ((g_telephonyService == nullptr) || !(SmsMmsGtest::HasSimCard(DEFAULT_SIM_SLOT_ID_1))) {
        TELEPHONY_LOGI("TelephonyTestService Remote service is null");
        g_telephonyService = SmsMmsGtest::GetProxy();
        ASSERT_TRUE(true);
        return;
    }
    SmsMmsTestHelper helper;
    helper.slotId = DEFAULT_SIM_SLOT_ID_1;
    if (!helper.Run(SetDefaultSmsSlotIdTestFuc, std::ref(helper))) {
        TELEPHONY_LOGI("SetDefaultSmsSlotIdTestFuc2 out of time");
        ASSERT_TRUE(false);
    }
    TELEPHONY_LOGI("TelSMSMMSTest::SetDefaultSmsSlotId_0002 -->finished");
    EXPECT_FALSE(helper.GetBoolResult());
}

void GetDefaultSmsSlotIdTestFuc(SmsMmsTestHelper &helper)
{
    int32_t slotId = g_telephonyService->GetDefaultSmsSlotId();
    helper.SetIntResult(slotId);
    helper.NotifyAll();
}

/**
 * @tc.number   Telephony_SmsMmsGtest_GetDefaultSmsSlotId_0001
 * @tc.name     Get Default Sms SlotId
 * @tc.desc     Function test
 */
HWTEST_F(SmsMmsGtest, GetDefaultSmsSlotId_0001, Function | MediumTest | Level3)
{
    TELEPHONY_LOGI("TelSMSMMSTest::GetDefaultSmsSlotId_0001 -->");
    if ((g_telephonyService == nullptr) || !(SmsMmsGtest::HasSimCard(DEFAULT_SIM_SLOT_ID))) {
        TELEPHONY_LOGI("TelephonyTestService Remote service is null");
        g_telephonyService = SmsMmsGtest::GetProxy();
        ASSERT_TRUE(true);
        return;
    }
    SmsMmsTestHelper helper;
    const int32_t error = DEFAULT_SIM_SLOT_ID_REMOVE;
    if (!helper.Run(GetDefaultSmsSlotIdTestFuc, std::ref(helper))) {
        TELEPHONY_LOGI("GetDefaultSmsSlotIdTestFuc out of time");
        ASSERT_TRUE(false);
    }
    TELEPHONY_LOGI("TelSMSMMSTest::GetDefaultSmsSlotId_0001 -->finished");
    ASSERT_TRUE(helper.GetIntResult() != error);
}

void SetSmscAddrTestFuc(SmsMmsTestHelper &helper)
{
    bool result = true;
    // invalid slotID scenario, a invalid smsc addr is OKAY
    std::string scAddr("1234");
    result = g_telephonyService->SetSmscAddr(helper.slotId, StringUtils::ToUtf16(scAddr));
    helper.SetBoolResult(result);
    helper.NotifyAll();
}

/**
 * @tc.number   Telephony_SmsMmsGtest_SetSmscAddr_0001
 * @tc.name     Set smsc addr slotId is invalid
 * @tc.desc     Function test
 * @tc.require: issueI5JI0H
 */
HWTEST_F(SmsMmsGtest, SetSmscAddr_0001, Function | MediumTest | Level3)
{
    AccessMmsToken token;
    TELEPHONY_LOGI("TelSMSMMSTest::SetSmscAddr_0001 -->");
    if (g_telephonyService == nullptr) {
        TELEPHONY_LOGI("TelephonyTestService Remote service is null");
        g_telephonyService = SmsMmsGtest::GetProxy();
        ASSERT_TRUE(false);
        return;
    }
    SmsMmsTestHelper helper;
    helper.slotId = DEFAULT_SIM_SLOT_ID_REMOVE;
    if (!helper.Run(SetSmscAddrTestFuc, std::ref(helper))) {
        TELEPHONY_LOGI("SetSmscAddrTestFuc out of time");
        ASSERT_TRUE(false);
    }
    TELEPHONY_LOGI("TelSMSMMSTest::SetSmscAddr_0001 -->finished");
    EXPECT_FALSE(helper.GetBoolResult());
}

void AddSimMessageTestFuc(SmsMmsTestHelper &helper)
{
    bool result = false;
    std::u16string smscData(u"");
    std::u16string pduData(u"01000B818176251308F4000007E8B0BCFD76E701");
    uint32_t status = 3;
    result = g_telephonyService->AddSimMessage(
        helper.slotId, smscData, pduData, static_cast<ISmsServiceInterface::SimMessageStatus>(status));
    helper.SetBoolResult(result);
    helper.NotifyAll();
}

/**
 * @tc.number   Telephony_SmsMmsGtest_AddSimMessage_0001
 * @tc.name     Add Sim Message
 * @tc.desc     Function test
 */
HWTEST_F(SmsMmsGtest, AddSimMessage_0001, Function | MediumTest | Level3)
{
    AccessMmsToken token;
    TELEPHONY_LOGI("TelSMSMMSTest::AddSimMessage_0001 -->");
    if ((g_telephonyService == nullptr) || !(SmsMmsGtest::HasSimCard(DEFAULT_SIM_SLOT_ID))) {
        TELEPHONY_LOGI("TelephonyTestService Remote service is null");
        g_telephonyService = SmsMmsGtest::GetProxy();
        ASSERT_TRUE(true);
        return;
    }
    SmsMmsTestHelper helper;
    helper.slotId = DEFAULT_SIM_SLOT_ID;
    if (!helper.Run(AddSimMessageTestFuc, std::ref(helper))) {
        TELEPHONY_LOGI("AddSimMessageTestFuc out of time");
        ASSERT_TRUE(false);
    }
    TELEPHONY_LOGI("TelSMSMMSTest::AddSimMessage_0001 -->finished");
    ASSERT_TRUE(helper.GetBoolResult());
}

/**
 * @tc.number   Telephony_SmsMmsGtest_AddSimMessage_0002
 * @tc.name     Add Sim Message
 * @tc.desc     Function test
 */
HWTEST_F(SmsMmsGtest, AddSimMessage_0002, Function | MediumTest | Level3)
{
    AccessMmsToken token;
    TELEPHONY_LOGI("TelSMSMMSTest::AddSimMessage_0002 -->");
    if ((g_telephonyService == nullptr) || !(SmsMmsGtest::HasSimCard(DEFAULT_SIM_SLOT_ID_1))) {
        TELEPHONY_LOGI("TelephonyTestService Remote service is null");
        g_telephonyService = SmsMmsGtest::GetProxy();
        ASSERT_TRUE(true);
        return;
    }
    SmsMmsTestHelper helper;
    helper.slotId = DEFAULT_SIM_SLOT_ID_1;
    if (!helper.Run(AddSimMessageTestFuc, std::ref(helper))) {
        TELEPHONY_LOGI("AddSimMessageTestFuc2 out of time");
        ASSERT_TRUE(false);
    }
    TELEPHONY_LOGI("TelSMSMMSTest::AddSimMessage_0002 -->finished");
    ASSERT_TRUE(helper.GetBoolResult());
}

void GetAllSimMessagesTestFuc(SmsMmsTestHelper &helper)
{
    std::vector<ShortMessage> result;
    result = g_telephonyService->GetAllSimMessages(helper.slotId);
    bool empty = result.empty();
    helper.SetBoolResult(empty);
    helper.NotifyAll();
}

/**
 * @tc.number   Telephony_SmsMmsGtest_GetAllSimMessages_0001
 * @tc.name     Get All Sim Messages
 * @tc.desc     Function test
 */
HWTEST_F(SmsMmsGtest, GetAllSimMessages_0001, Function | MediumTest | Level3)
{
    AccessMmsToken token;
    TELEPHONY_LOGI("TelSMSMMSTest::GetAllSimMessages_0001 -->");
    if ((g_telephonyService == nullptr) || !(SmsMmsGtest::HasSimCard(DEFAULT_SIM_SLOT_ID))) {
        TELEPHONY_LOGI("TelephonyTestService Remote service is null");
        g_telephonyService = SmsMmsGtest::GetProxy();
        ASSERT_TRUE(true);
        return;
    }
    SmsMmsTestHelper helper;
    helper.slotId = DEFAULT_SIM_SLOT_ID;
    if (!helper.Run(GetAllSimMessagesTestFuc, std::ref(helper))) {
        TELEPHONY_LOGI("GetAllSimMessagesTestFuc out of time");
        ASSERT_TRUE(false);
    }
    TELEPHONY_LOGI("TelSMSMMSTest::GetAllSimMessages_0001 -->finished");
    EXPECT_FALSE(helper.GetBoolResult());
}

/**
 * @tc.number   Telephony_SmsMmsGtest_GetAllSimMessages_0002
 * @tc.name     Get All Sim Messages
 * @tc.desc     Function test
 */
HWTEST_F(SmsMmsGtest, GetAllSimMessages_0002, Function | MediumTest | Level3)
{
    AccessMmsToken token;
    TELEPHONY_LOGI("TelSMSMMSTest::GetAllSimMessages_0002 -->");
    if ((g_telephonyService == nullptr) || !(SmsMmsGtest::HasSimCard(DEFAULT_SIM_SLOT_ID_1))) {
        TELEPHONY_LOGI("TelephonyTestService Remote service is null");
        g_telephonyService = SmsMmsGtest::GetProxy();
        ASSERT_TRUE(true);
        return;
    }
    SmsMmsTestHelper helper;
    helper.slotId = DEFAULT_SIM_SLOT_ID_1;
    if (!helper.Run(GetAllSimMessagesTestFuc, std::ref(helper))) {
        TELEPHONY_LOGI("GetAllSimMessagesTestFuc2 out of time");
        ASSERT_TRUE(false);
    }
    TELEPHONY_LOGI("TelSMSMMSTest::GetAllSimMessages_0002 -->finished");
    EXPECT_FALSE(helper.GetBoolResult());
}

void DelSimMessageTestFuc(SmsMmsTestHelper &helper)
{
    bool result = false;
    uint32_t msgIndex = 0;
    result = g_telephonyService->DelSimMessage(helper.slotId, msgIndex);
    helper.SetBoolResult(result);
    helper.NotifyAll();
}

/**
 * @tc.number   Telephony_SmsMmsGtest_DelSimMessage_0001
 * @tc.name     Del Sim Message
 * @tc.desc     Function test
 */
HWTEST_F(SmsMmsGtest, DelSimMessage_0001, Function | MediumTest | Level3)
{
    AccessMmsToken token;
    TELEPHONY_LOGI("TelSMSMMSTest::DelSimMessage_0001 -->");
    if ((g_telephonyService == nullptr) || !(SmsMmsGtest::HasSimCard(DEFAULT_SIM_SLOT_ID))) {
        TELEPHONY_LOGI("TelephonyTestService Remote service is null");
        g_telephonyService = SmsMmsGtest::GetProxy();
        ASSERT_TRUE(true);
        return;
    }
    SmsMmsTestHelper helper;
    helper.slotId = DEFAULT_SIM_SLOT_ID;
    if (!helper.Run(DelSimMessageTestFuc, std::ref(helper))) {
        TELEPHONY_LOGI("DelSimMessageTestFuc out of time");
        ASSERT_TRUE(false);
    }
    TELEPHONY_LOGI("TelSMSMMSTest::DelSimMessage_0001 -->finished");
    ASSERT_TRUE(helper.GetBoolResult());
}

/**
 * @tc.number   Telephony_SmsMmsGtest_DelSimMessage_0002
 * @tc.name     Del Sim Message
 * @tc.desc     Function test
 */
HWTEST_F(SmsMmsGtest, DelSimMessage_0002, Function | MediumTest | Level3)
{
    AccessMmsToken token;
    TELEPHONY_LOGI("TelSMSMMSTest::DelSimMessage_0002 -->");
    if ((g_telephonyService == nullptr) || !(SmsMmsGtest::HasSimCard(DEFAULT_SIM_SLOT_ID_1))) {
        TELEPHONY_LOGI("TelephonyTestService Remote service is null");
        g_telephonyService = SmsMmsGtest::GetProxy();
        ASSERT_TRUE(true);
        return;
    }
    SmsMmsTestHelper helper;
    helper.slotId = DEFAULT_SIM_SLOT_ID_1;
    if (!helper.Run(DelSimMessageTestFuc, std::ref(helper))) {
        TELEPHONY_LOGI("DelSimMessageTestFuc2 out of time");
        ASSERT_TRUE(false);
    }
    TELEPHONY_LOGI("TelSMSMMSTest::DelSimMessage_0002 -->finished");
    ASSERT_TRUE(helper.GetBoolResult());
}

void UpdateSimMessageTestFuc(SmsMmsTestHelper &helper)
{
    bool result = false;
    uint32_t msgIndex = 0;
    std::u16string smscData(u"");
    std::u16string pduData(u"01000B818176251308F4000007E8B0BCFD76E701");
    uint32_t status = 3;
    result = g_telephonyService->UpdateSimMessage(
        helper.slotId, msgIndex, static_cast<ISmsServiceInterface::SimMessageStatus>(status), pduData, smscData);
    helper.SetBoolResult(result);
    helper.NotifyAll();
}

/**
 * @tc.number   Telephony_SmsMmsGtest_UpdateSimMessage_0001
 * @tc.name     Update Sim Message
 * @tc.desc     Function test
 */
HWTEST_F(SmsMmsGtest, UpdateSimMessage_0001, Function | MediumTest | Level3)
{
    AccessMmsToken token;
    TELEPHONY_LOGI("TelSMSMMSTest::UpdateSimMessage_0001 -->");
    if ((g_telephonyService == nullptr) || !(SmsMmsGtest::HasSimCard(DEFAULT_SIM_SLOT_ID))) {
        TELEPHONY_LOGI("TelephonyTestService Remote service is null");
        g_telephonyService = SmsMmsGtest::GetProxy();
        ASSERT_TRUE(true);
        return;
    }
    SmsMmsTestHelper helper;
    helper.slotId = DEFAULT_SIM_SLOT_ID;
    if (!helper.Run(UpdateSimMessageTestFuc, std::ref(helper))) {
        TELEPHONY_LOGI("UpdateSimMessageTestFuc out of time");
        ASSERT_TRUE(false);
    }
    TELEPHONY_LOGI("TelSMSMMSTest::UpdateSimMessage_0001 -->finished");
    ASSERT_TRUE(helper.GetBoolResult());
}

/**
 * @tc.number   Telephony_SmsMmsGtest_UpdateSimMessage_0002
 * @tc.name     Update Sim Message
 * @tc.desc     Function test
 * @tc.require: issueI5K12U
 */
HWTEST_F(SmsMmsGtest, UpdateSimMessage_0002, Function | MediumTest | Level3)
{
    AccessMmsToken token;
    TELEPHONY_LOGI("TelSMSMMSTest::UpdateSimMessage_0002 -->");
    if ((g_telephonyService == nullptr) || !(SmsMmsGtest::HasSimCard(DEFAULT_SIM_SLOT_ID_1))) {
        TELEPHONY_LOGI("TelephonyTestService Remote service is null");
        g_telephonyService = SmsMmsGtest::GetProxy();
        ASSERT_TRUE(true);
        return;
    }
    SmsMmsTestHelper helper;
    helper.slotId = DEFAULT_SIM_SLOT_ID_1;
    if (!helper.Run(UpdateSimMessageTestFuc, std::ref(helper))) {
        TELEPHONY_LOGI("UpdateSimMessageTestFuc2 out of time");
        ASSERT_TRUE(false);
    }
    TELEPHONY_LOGI("TelSMSMMSTest::UpdateSimMessage_0002 -->finished");
    ASSERT_TRUE(helper.GetBoolResult());
}

void SetImsSmsConfigTestFuc(SmsMmsTestHelper &helper)
{
    bool result = false;
    g_telephonyService->SetImsSmsConfig(helper.slotId, 1);
    result = g_telephonyService->IsImsSmsSupported(helper.slotId);
    helper.SetBoolResult(result);
    helper.NotifyAll();
}

/**
 * @tc.number   Telephony_SmsMmsGtest_SetImsSmsConfig_0001
 * @tc.name     Enable IMS SMS
 * @tc.desc     Function test
 * @tc.require: issueI5K12U
 */
HWTEST_F(SmsMmsGtest, SetImsSmsConfig_0001, Function | MediumTest | Level3)
{
    AccessMmsToken token;
    TELEPHONY_LOGI("TelSMSMMSTest::SetImsSmsConfig_0001 -->");
    if ((g_telephonyService == nullptr) || !(SmsMmsGtest::HasSimCard(DEFAULT_SIM_SLOT_ID))) {
        TELEPHONY_LOGI("TelephonyTestService Remote service is null");
        g_telephonyService = SmsMmsGtest::GetProxy();
        ASSERT_TRUE(true);
        return;
    }
    SmsMmsTestHelper helper;
    helper.slotId = DEFAULT_SIM_SLOT_ID;
    if (!helper.Run(SetImsSmsConfigTestFuc, std::ref(helper))) {
        TELEPHONY_LOGI("SetImsSmsConfigTestFuc out of time");
        ASSERT_TRUE(false);
    }
    TELEPHONY_LOGI("TelSMSMMSTest::SetImsSmsConfig_0001 -->finished");
    ASSERT_TRUE(helper.GetBoolResult());
}

void SetImsSmsConfigTestFuc2(SmsMmsTestHelper &helper)
{
    bool result = true;
    g_telephonyService->SetImsSmsConfig(helper.slotId, 0);
    result = g_telephonyService->IsImsSmsSupported(helper.slotId);
    helper.SetBoolResult(result);
    helper.NotifyAll();
}

/**
 * @tc.number   Telephony_SmsMmsGtest_SetImsSmsConfig_0002
 * @tc.name     Disable Ims Sms
 * @tc.desc     Function test
 * @tc.require: issueI5K12U
 */
HWTEST_F(SmsMmsGtest, SetImsSmsConfig_0002, Function | MediumTest | Level3)
{
    AccessMmsToken token;
    TELEPHONY_LOGI("TelSMSMMSTest::SetImsSmsConfig_0002 -->");
    if ((g_telephonyService == nullptr) || !(SmsMmsGtest::HasSimCard(DEFAULT_SIM_SLOT_ID))) {
        TELEPHONY_LOGI("TelephonyTestService Remote service is null");
        g_telephonyService = SmsMmsGtest::GetProxy();
        ASSERT_TRUE(true);
        return;
    }
    SmsMmsTestHelper helper;
    helper.slotId = DEFAULT_SIM_SLOT_ID;
    if (!helper.Run(SetImsSmsConfigTestFuc2, std::ref(helper))) {
        TELEPHONY_LOGI("SetImsSmsConfigTestFuc2 out of time");
        ASSERT_TRUE(false);
    }
    TELEPHONY_LOGI("TelSMSMMSTest::SetImsSmsConfig_0002 -->finished");
    EXPECT_FALSE(helper.GetBoolResult());
}

/**
 * @tc.number   Telephony_SmsMmsGtest_SetImsSmsConfig_0003
 * @tc.name     Enable IMS SMS
 * @tc.desc     Function test
 * @tc.require: issueI5K12U
 */
HWTEST_F(SmsMmsGtest, SetImsSmsConfig_0003, Function | MediumTest | Level3)
{
    AccessMmsToken token;
    TELEPHONY_LOGI("TelSMSMMSTest::SetImsSmsConfig_0003 -->");
    if ((g_telephonyService == nullptr) || !(SmsMmsGtest::HasSimCard(DEFAULT_SIM_SLOT_ID_1))) {
        TELEPHONY_LOGI("TelephonyTestService Remote service is null");
        g_telephonyService = SmsMmsGtest::GetProxy();
        ASSERT_TRUE(true);
        return;
    }
    SmsMmsTestHelper helper;
    helper.slotId = DEFAULT_SIM_SLOT_ID_1;
    if (!helper.Run(SetImsSmsConfigTestFuc, std::ref(helper))) {
        TELEPHONY_LOGI("SetImsSmsConfigTestFuc3 out of time");
        ASSERT_TRUE(false);
    }
    TELEPHONY_LOGI("TelSMSMMSTest::SetImsSmsConfig_0003 -->finished");
    ASSERT_TRUE(helper.GetBoolResult());
}

/**
 * @tc.number   Telephony_SmsMmsGtest_SetImsSmsConfig_0004
 * @tc.name     Disable Ims Sms
 * @tc.desc     Function test
 */
HWTEST_F(SmsMmsGtest, SetImsSmsConfig_0004, Function | MediumTest | Level3)
{
    AccessMmsToken token;
    TELEPHONY_LOGI("TelSMSMMSTest::SetImsSmsConfig_0004 -->");
    if ((g_telephonyService == nullptr) || !(SmsMmsGtest::HasSimCard(DEFAULT_SIM_SLOT_ID_1))) {
        TELEPHONY_LOGI("TelephonyTestService Remote service is null");
        g_telephonyService = SmsMmsGtest::GetProxy();
        ASSERT_TRUE(true);
        return;
    }
    SmsMmsTestHelper helper;
    helper.slotId = DEFAULT_SIM_SLOT_ID_1;
    if (!helper.Run(SetImsSmsConfigTestFuc2, std::ref(helper))) {
        TELEPHONY_LOGI("SetImsSmsConfigTestFuc4 out of time");
        ASSERT_TRUE(false);
    }
    TELEPHONY_LOGI("TelSMSMMSTest::SetImsSmsConfig_0004 -->finished");
    EXPECT_FALSE(helper.GetBoolResult());
}

#else // TEL_TEST_UNSUPPORT
/**
 * @tc.number   Telephony_SmsMms_MockTest_0001
 * @tc.name     Mock test for unsupported platform
 * @tc.desc     Function test
 */
HWTEST_F(SmsMmsGtest, MockTest_0001, Function | MediumTest | Level3)
{
    ASSERT_TRUE(true);
}
#endif // TEL_TEST_UNSUPPORT
} // namespace Telephony
} // namespace OHOS
#endif
