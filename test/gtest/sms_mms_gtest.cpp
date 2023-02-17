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
#define private public
#define protected public

#include "accesstoken_kit.h"
#include "cdma_sms_message.h"
#include "cdma_sms_pdu_codec.h"
#include "cdma_sms_receive_handler.h"
#include "core_service_client.h"
#include "delivery_short_message_callback_stub.h"
#include "gtest/gtest.h"
#include "i_sms_service_interface.h"
#include "if_system_ability_manager.h"
#include "ipc_skeleton.h"
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
#include "send_short_message_callback_stub.h"
#include "sms_delivery_callback_gtest.h"
#include "sms_mms_test_helper.h"
#include "sms_send_callback_gtest.h"
#include "sms_service.h"
#include "sms_service_manager_client.h"
#include "sms_service_proxy.h"
#include "string_utils.h"
#include "system_ability_definition.h"
#include "telephony_errors.h"
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
        bool hasSimCard = false;
        if (CoreServiceClient::GetInstance().GetProxy() == nullptr) {
            return hasSimCard;
        }
        CoreServiceClient::GetInstance().HasSimCard(slotId, hasSimCard);
        return hasSimCard;
    }
};

void SmsMmsGtest::TearDownTestCase() {}

void SmsMmsGtest::SetUp() {}

void SmsMmsGtest::TearDown() {}

const int32_t DEFAULT_SIM_SLOT_ID_1 = 1;
const std::string DES_ADDR = "10086";
const std::string TEXT_SMS_CONTENT = "hello world";
const uint8_t DATA_SMS[] = "hello world";
const uint16_t SMS_PORT = 100;

void SmsMmsGtest::SetUpTestCase()
{
    TELEPHONY_LOGI("SetUpTestCase slotId%{public}d", DEFAULT_SIM_SLOT_ID_1);
    g_telephonyService = GetProxy();
    if (g_telephonyService == nullptr) {
        return;
    }
    DelayedSingleton<SmsServiceManagerClient>::GetInstance()->ResetSmsServiceProxy();
    DelayedSingleton<SmsServiceManagerClient>::GetInstance()->InitSmsServiceProxy();
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
HWTEST_F(SmsMmsGtest, OpenCellBroadcast_0001, Function | MediumTest | Level1)
{
    AccessMmsToken token;
    TELEPHONY_LOGI("TelSMSMMSTest::OpenCellBroadcast_0001 -->");
    if (!(SmsMmsGtest::HasSimCard(DEFAULT_SIM_SLOT_ID))) {
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
    EXPECT_FALSE(helper.GetIntResult() == 0);
}

void OpenCellBroadcastTestFuc2(SmsMmsTestHelper &helper)
{
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
HWTEST_F(SmsMmsGtest, OpenCellBroadcast_0002, Function | MediumTest | Level2)
{
    AccessMmsToken token;
    TELEPHONY_LOGI("TelSMSMMSTest::OpenCellBroadcast_0002 -->");
    if (!(SmsMmsGtest::HasSimCard(DEFAULT_SIM_SLOT_ID))) {
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
    EXPECT_FALSE(helper.GetIntResult() == 0);
}

void OpenCellBroadcastTestFuc3(SmsMmsTestHelper &helper)
{
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
HWTEST_F(SmsMmsGtest, OpenCellBroadcast_0003, Function | MediumTest | Level3)
{
    AccessMmsToken token;
    TELEPHONY_LOGI("TelSMSMMSTest::OpenCellBroadcast_0003 -->");
    if (!(SmsMmsGtest::HasSimCard(DEFAULT_SIM_SLOT_ID))) {
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
    EXPECT_FALSE(helper.GetIntResult() == 0);
}

void OpenCellBroadcastTestFuc4(SmsMmsTestHelper &helper)
{
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
HWTEST_F(SmsMmsGtest, OpenCellBroadcast_0004, Function | MediumTest | Level3)
{
    AccessMmsToken token;
    TELEPHONY_LOGI("TelSMSMMSTest::OpenCellBroadcast_0004 -->");
    if (!(SmsMmsGtest::HasSimCard(DEFAULT_SIM_SLOT_ID))) {
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
    ASSERT_TRUE(helper.GetIntResult() == 0);
}

void OpenCellBroadcastTestFuc5(SmsMmsTestHelper &helper)
{
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
HWTEST_F(SmsMmsGtest, OpenCellBroadcast_0005, Function | MediumTest | Level3)
{
    AccessMmsToken token;
    TELEPHONY_LOGI("TelSMSMMSTest::OpenCellBroadcast_0005 -->");
    if (!(SmsMmsGtest::HasSimCard(DEFAULT_SIM_SLOT_ID))) {
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
    ASSERT_TRUE(helper.GetIntResult() == 0);
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

/**
 * @tc.number   Telephony_SmsMmsGtest_OpenCellBroadcast_0006
 * @tc.name     Open cellBroadcast parameter is valid
 * @tc.desc     Function test
 */
HWTEST_F(SmsMmsGtest, OpenCellBroadcast_0006, Function | MediumTest | Level4)
{
    AccessMmsToken token;
    TELEPHONY_LOGI("TelSMSMMSTest::OpenCellBroadcast_0006 -->");
    if (!(SmsMmsGtest::HasSimCard(DEFAULT_SIM_SLOT_ID))) {
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
    TELEPHONY_LOGI("TelSMSMMSTest::OpenCellBroadcast_0006 -->finished");
    ASSERT_TRUE(helper.GetIntResult() == 0);
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
    if (!(SmsMmsGtest::HasSimCard(DEFAULT_SIM_SLOT_ID_1))) {
        TELEPHONY_LOGI("TelephonyTestService has no sim card");
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
    EXPECT_FALSE(helper.GetIntResult() == 0);
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
    if (!(SmsMmsGtest::HasSimCard(DEFAULT_SIM_SLOT_ID_1))) {
        TELEPHONY_LOGI("TelephonyTestService has no sim card");
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
    EXPECT_FALSE(helper.GetIntResult() == 0);
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
    if (!(SmsMmsGtest::HasSimCard(DEFAULT_SIM_SLOT_ID_1))) {
        TELEPHONY_LOGI("TelephonyTestService has no sim card");
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
    ASSERT_TRUE(helper.GetIntResult() == 0);
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
    if (!(SmsMmsGtest::HasSimCard(DEFAULT_SIM_SLOT_ID_1))) {
        TELEPHONY_LOGI("TelephonyTestService has no sim card");
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
    ASSERT_TRUE(helper.GetIntResult() == 0);
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
    if (!(SmsMmsGtest::HasSimCard(DEFAULT_SIM_SLOT_ID_1))) {
        TELEPHONY_LOGI("TelephonyTestService has no sim card");
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
    ASSERT_TRUE(helper.GetIntResult() == 0);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_OpenCellBroadcast_0012
 * @tc.name     Open cellBroadcast parameter is valid
 * @tc.desc     Function test
 */
HWTEST_F(SmsMmsGtest, OpenCellBroadcast_0012, Function | MediumTest | Level3)
{
    TELEPHONY_LOGI("TelSMSMMSTest::OpenCellBroadcast_0012 -->");
    if (!(SmsMmsGtest::HasSimCard(DEFAULT_SIM_SLOT_ID))) {
        TELEPHONY_LOGI("TelephonyTestService has no sim card");
        ASSERT_TRUE(true);
        return;
    }
    SmsMmsTestHelper helper;
    helper.slotId = DEFAULT_SIM_SLOT_ID;
    if (!helper.Run(OpenCellBroadcastTestFuc6, std::ref(helper))) {
        TELEPHONY_LOGI("OpenCellBroadcastTestFuc12 out of time");
        ASSERT_TRUE(false);
    }
    TELEPHONY_LOGI("TelSMSMMSTest::OpenCellBroadcast_0012 -->finished");
    EXPECT_FALSE(helper.GetIntResult() == 0);
}

void CloseCellBroadcastTestFuc(SmsMmsTestHelper &helper)
{
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
HWTEST_F(SmsMmsGtest, CloseCellBroadcast_0001, Function | MediumTest | Level1)
{
    AccessMmsToken token;
    TELEPHONY_LOGI("TelSMSMMSTest::CloseCellBroadcast_0001 -->");
    if (!(SmsMmsGtest::HasSimCard(DEFAULT_SIM_SLOT_ID))) {
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
    EXPECT_FALSE(helper.GetIntResult() == 0);
}

void CloseCellBroadcastTestFuc2(SmsMmsTestHelper &helper)
{
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
HWTEST_F(SmsMmsGtest, CloseCellBroadcast_0002, Function | MediumTest | Level2)
{
    AccessMmsToken token;
    TELEPHONY_LOGI("TelSMSMMSTest::CloseCellBroadcast_0002 -->");
    if (!(SmsMmsGtest::HasSimCard(DEFAULT_SIM_SLOT_ID))) {
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
    EXPECT_FALSE(helper.GetIntResult() == 0);
}

void CloseCellBroadcastTestFuc3(SmsMmsTestHelper &helper)
{
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
HWTEST_F(SmsMmsGtest, CloseCellBroadcast_0003, Function | MediumTest | Level3)
{
    AccessMmsToken token;
    TELEPHONY_LOGI("TelSMSMMSTest::CloseCellBroadcast_0003 -->");
    if (!(SmsMmsGtest::HasSimCard(DEFAULT_SIM_SLOT_ID))) {
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
    EXPECT_FALSE(helper.GetIntResult() == 0);
}

void CloseCellBroadcastTestFuc4(SmsMmsTestHelper &helper)
{
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
HWTEST_F(SmsMmsGtest, CloseCellBroadcast_0004, Function | MediumTest | Level3)
{
    AccessMmsToken token;
    TELEPHONY_LOGI("TelSMSMMSTest::CloseCellBroadcast_0004 -->");
    if (!(SmsMmsGtest::HasSimCard(DEFAULT_SIM_SLOT_ID))) {
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
    ASSERT_TRUE(helper.GetIntResult() == 0);
}

void CloseCellBroadcastTestFuc5(SmsMmsTestHelper &helper)
{
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
HWTEST_F(SmsMmsGtest, CloseCellBroadcast_0005, Function | MediumTest | Level3)
{
    AccessMmsToken token;
    TELEPHONY_LOGI("TelSMSMMSTest::CloseCellBroadcast_0005 -->");
    if (!(SmsMmsGtest::HasSimCard(DEFAULT_SIM_SLOT_ID))) {
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
    ASSERT_TRUE(helper.GetIntResult() == 0);
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

/**
 * @tc.number   Telephony_SmsMmsGtest_CloseCellBroadcast_0006
 * @tc.name     Close cellBroadcast parameter is valid
 * @tc.desc     Function test
 */
HWTEST_F(SmsMmsGtest, CloseCellBroadcast_0006, Function | MediumTest | Level4)
{
    AccessMmsToken token;
    TELEPHONY_LOGI("TelSMSMMSTest::CloseCellBroadcast_0006 -->");
    if (!(SmsMmsGtest::HasSimCard(DEFAULT_SIM_SLOT_ID))) {
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
    TELEPHONY_LOGI("TelSMSMMSTest::CloseCellBroadcast_0006 -->finished");
    ASSERT_TRUE(helper.GetIntResult() == 0);
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
    if (!(SmsMmsGtest::HasSimCard(DEFAULT_SIM_SLOT_ID_1))) {
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
    EXPECT_FALSE(helper.GetIntResult() == 0);
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
    if (!(SmsMmsGtest::HasSimCard(DEFAULT_SIM_SLOT_ID_1))) {
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
    EXPECT_FALSE(helper.GetIntResult() == 0);
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
    if (!(SmsMmsGtest::HasSimCard(DEFAULT_SIM_SLOT_ID_1))) {
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
    ASSERT_TRUE(helper.GetIntResult() == 0);
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
    if (!(SmsMmsGtest::HasSimCard(DEFAULT_SIM_SLOT_ID_1))) {
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
    ASSERT_TRUE(helper.GetIntResult() == 0);
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
    if (!(SmsMmsGtest::HasSimCard(DEFAULT_SIM_SLOT_ID_1))) {
        TELEPHONY_LOGI("TelephonyTestService has no sim card");
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
    ASSERT_TRUE(helper.GetIntResult() == 0);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_CloseCellBroadcast_0012
 * @tc.name     Close cellBroadcast parameter is valid
 * @tc.desc     Function test
 */
HWTEST_F(SmsMmsGtest, CloseCellBroadcast_0012, Function | MediumTest | Level3)
{
    TELEPHONY_LOGI("TelSMSMMSTest::CloseCellBroadcast_0012 -->");
    if (!(SmsMmsGtest::HasSimCard(DEFAULT_SIM_SLOT_ID))) {
        TELEPHONY_LOGI("TelephonyTestService has no sim card");
        ASSERT_TRUE(true);
        return;
    }
    SmsMmsTestHelper helper;
    helper.slotId = DEFAULT_SIM_SLOT_ID;
    if (!helper.Run(CloseCellBroadcastTestFuc6, std::ref(helper))) {
        TELEPHONY_LOGI("CloseCellBroadcastTestFuc12 out of time");
        ASSERT_TRUE(false);
    }
    TELEPHONY_LOGI("TelSMSMMSTest::CloseCellBroadcast_0012 -->finished");
    EXPECT_FALSE(helper.GetIntResult() == 0);
}

void SetDefaultSmsSlotIdTestFuc(SmsMmsTestHelper &helper)
{
    int32_t result = DelayedSingleton<SmsServiceManagerClient>::GetInstance()->SetDefaultSmsSlotId(helper.slotId);
    helper.SetIntResult(result);
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
    if (!(SmsMmsGtest::HasSimCard(DEFAULT_SIM_SLOT_ID))) {
        TELEPHONY_LOGI("TelephonyTestService has no sim card");
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
    ASSERT_TRUE(helper.GetIntResult() == 0);
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
    if (!(SmsMmsGtest::HasSimCard(DEFAULT_SIM_SLOT_ID_1))) {
        TELEPHONY_LOGI("TelephonyTestService has no sim card");
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
    EXPECT_TRUE(helper.GetIntResult() == 0);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_SetDefaultSmsSlotId_0003
 * @tc.name     Set Default Sms SlotId slotId is valid
 * @tc.desc     Function test
 */
HWTEST_F(SmsMmsGtest, SetDefaultSmsSlotId_0003, Function | MediumTest | Level2)
{
    TELEPHONY_LOGI("TelSMSMMSTest::SetDefaultSmsSlotId_0003 -->");
    if (!(SmsMmsGtest::HasSimCard(DEFAULT_SIM_SLOT_ID))) {
        TELEPHONY_LOGI("TelephonyTestService has no sim card");
        ASSERT_TRUE(true);
        return;
    }
    SmsMmsTestHelper helper;
    helper.slotId = DEFAULT_SIM_SLOT_ID;
    if (!helper.Run(SetDefaultSmsSlotIdTestFuc, std::ref(helper))) {
        TELEPHONY_LOGI("SetDefaultSmsSlotIdTestFuc out of time");
        ASSERT_TRUE(false);
    }
    TELEPHONY_LOGI("TelSMSMMSTest::SetDefaultSmsSlotId_0003 -->finished");
    EXPECT_FALSE(helper.GetIntResult() == 0);
}

void GetDefaultSmsSlotIdTestFuc(SmsMmsTestHelper &helper)
{
    int32_t slotId = DelayedSingleton<SmsServiceManagerClient>::GetInstance()->GetDefaultSmsSlotId();
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
    if (!(SmsMmsGtest::HasSimCard(DEFAULT_SIM_SLOT_ID))) {
        TELEPHONY_LOGI("TelephonyTestService has no sim card");
        g_telephonyService = SmsMmsGtest::GetProxy();
        ASSERT_TRUE(true);
        return;
    }
    SmsMmsTestHelper helper;
    if (!helper.Run(GetDefaultSmsSlotIdTestFuc, std::ref(helper))) {
        TELEPHONY_LOGI("GetDefaultSmsSlotIdTestFuc out of time");
        ASSERT_TRUE(false);
    }
    TELEPHONY_LOGI("TelSMSMMSTest::GetDefaultSmsSlotId_0001 -->finished");
    ASSERT_TRUE(helper.GetIntResult() == 0);
}

void SetSmscAddrTestFuc(SmsMmsTestHelper &helper)
{
    // invalid slotID scenario, a invalid smsc addr is OKAY
    std::string scAddr("1234");
    int32_t result = DelayedSingleton<SmsServiceManagerClient>::GetInstance()->SetScAddress(
        helper.slotId, StringUtils::ToUtf16(scAddr));
    helper.SetIntResult(result);
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
    if (!(SmsMmsGtest::HasSimCard(DEFAULT_SIM_SLOT_ID))) {
        TELEPHONY_LOGI("TelephonyTestService has no sim card");
        ASSERT_TRUE(true);
        return;
    }
    SmsMmsTestHelper helper;
    helper.slotId = DEFAULT_SIM_SLOT_ID_REMOVE;
    if (!helper.Run(SetSmscAddrTestFuc, std::ref(helper))) {
        TELEPHONY_LOGI("SetSmscAddrTestFuc out of time");
        ASSERT_TRUE(false);
    }
    TELEPHONY_LOGI("TelSMSMMSTest::SetSmscAddr_0001 -->finished");
    EXPECT_FALSE(helper.GetIntResult() == 0);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_SetSmscAddr_0002
 * @tc.name     Set smsc addr slotId is invalid
 * @tc.desc     Function test
 * @tc.require: issueI5JI0H
 */
HWTEST_F(SmsMmsGtest, SetSmscAddr_0002, Function | MediumTest | Level2)
{
    TELEPHONY_LOGI("TelSMSMMSTest::SetSmscAddr_0002 -->");
    if (!(SmsMmsGtest::HasSimCard(DEFAULT_SIM_SLOT_ID_1))) {
        TELEPHONY_LOGI("TelephonyTestService has no sim card");
        ASSERT_TRUE(true);
        return;
    }
    SmsMmsTestHelper helper;
    helper.slotId = DEFAULT_SIM_SLOT_ID_REMOVE;
    if (!helper.Run(SetSmscAddrTestFuc, std::ref(helper))) {
        TELEPHONY_LOGI("SetSmscAddrTestFuc out of time");
        ASSERT_TRUE(false);
    }
    TELEPHONY_LOGI("TelSMSMMSTest::SetSmscAddr_0002 -->finished");
    EXPECT_FALSE(helper.GetIntResult() == 0);
}

void AddSimMessageTestFuc(SmsMmsTestHelper &helper)
{
    std::u16string smscData(u"");
    std::u16string pduData(u"01000B818176251308F4000007E8B0BCFD76E701");
    uint32_t status = 3;
    int32_t result = DelayedSingleton<SmsServiceManagerClient>::GetInstance()->AddSimMessage(
        helper.slotId, smscData, pduData, static_cast<ISmsServiceInterface::SimMessageStatus>(status));
    helper.SetIntResult(result);
    helper.NotifyAll();
}

void DelSimMessageTestFuc(SmsMmsTestHelper &helper)
{
    uint32_t msgIndex = 0;
    int32_t result = DelayedSingleton<SmsServiceManagerClient>::GetInstance()->DelSimMessage(helper.slotId, msgIndex);
    helper.SetIntResult(result);
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
    if (!(SmsMmsGtest::HasSimCard(DEFAULT_SIM_SLOT_ID))) {
        TELEPHONY_LOGI("TelephonyTestService has no sim card");
        ASSERT_TRUE(true);
        return;
    }
    SmsMmsTestHelper helper;
    helper.slotId = DEFAULT_SIM_SLOT_ID;
    if (!helper.Run(DelSimMessageTestFuc, std::ref(helper))) {
        TELEPHONY_LOGI("AddSimMessageTestFuc DelSimMessageTestFuc out of time");
        ASSERT_TRUE(false);
    }

    if (!helper.Run(AddSimMessageTestFuc, std::ref(helper))) {
        TELEPHONY_LOGI("AddSimMessageTestFuc out of time");
        ASSERT_TRUE(false);
    }
    TELEPHONY_LOGI("TelSMSMMSTest::AddSimMessage_0001 -->finished");
    ASSERT_TRUE(helper.GetIntResult() == 0);
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
    if (!(SmsMmsGtest::HasSimCard(DEFAULT_SIM_SLOT_ID_1))) {
        TELEPHONY_LOGI("TelephonyTestService has no sim card");
        ASSERT_TRUE(true);
        return;
    }
    SmsMmsTestHelper helper;
    helper.slotId = DEFAULT_SIM_SLOT_ID_1;
    if (!helper.Run(DelSimMessageTestFuc, std::ref(helper))) {
        TELEPHONY_LOGI("AddSimMessageTestFuc2 DelSimMessageTestFuc out of time");
        ASSERT_TRUE(false);
    }

    if (!helper.Run(AddSimMessageTestFuc, std::ref(helper))) {
        TELEPHONY_LOGI("AddSimMessageTestFuc2 out of time");
        ASSERT_TRUE(false);
    }
    TELEPHONY_LOGI("TelSMSMMSTest::AddSimMessage_0002 -->finished");
    ASSERT_TRUE(helper.GetIntResult() == 0);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_AddSimMessage_0003
 * @tc.name     Add Sim Message
 * @tc.desc     Function test
 */
HWTEST_F(SmsMmsGtest, AddSimMessage_0003, Function | MediumTest | Level3)
{
    TELEPHONY_LOGI("TelSMSMMSTest::AddSimMessage_0003 -->");
    if (!(SmsMmsGtest::HasSimCard(DEFAULT_SIM_SLOT_ID))) {
        TELEPHONY_LOGI("TelephonyTestService has no sim card");
        ASSERT_TRUE(true);
        return;
    }
    SmsMmsTestHelper helper;
    helper.slotId = DEFAULT_SIM_SLOT_ID;
    if (!helper.Run(DelSimMessageTestFuc, std::ref(helper))) {
        TELEPHONY_LOGI("AddSimMessageTestFuc DelSimMessageTestFuc out of time");
        ASSERT_TRUE(false);
    }

    if (!helper.Run(AddSimMessageTestFuc, std::ref(helper))) {
        TELEPHONY_LOGI("AddSimMessageTestFuc out of time");
        ASSERT_TRUE(false);
    }
    TELEPHONY_LOGI("TelSMSMMSTest::AddSimMessage_0003 -->finished");
    EXPECT_FALSE(helper.GetIntResult() == 0);
}

void GetAllSimMessagesTestFuc(SmsMmsTestHelper &helper)
{
    std::vector<ShortMessage> message;
    DelayedSingleton<SmsServiceManagerClient>::GetInstance()->GetAllSimMessages(helper.slotId, message);
    bool empty = message.empty();
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
    if (!(SmsMmsGtest::HasSimCard(DEFAULT_SIM_SLOT_ID))) {
        TELEPHONY_LOGI("TelephonyTestService has no sim card");
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
    if (!(SmsMmsGtest::HasSimCard(DEFAULT_SIM_SLOT_ID_1))) {
        TELEPHONY_LOGI("TelephonyTestService has no sim card");
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

/**
 * @tc.number   Telephony_SmsMmsGtest_GetAllSimMessages_0003
 * @tc.name     Get All Sim Messages
 * @tc.desc     Function test
 */
HWTEST_F(SmsMmsGtest, GetAllSimMessages_0003, Function | MediumTest | Level3)
{
    TELEPHONY_LOGI("TelSMSMMSTest::GetAllSimMessages_0003 -->");
    if (!(SmsMmsGtest::HasSimCard(DEFAULT_SIM_SLOT_ID))) {
        TELEPHONY_LOGI("TelephonyTestService has no sim card");
        ASSERT_TRUE(true);
        return;
    }
    SmsMmsTestHelper helper;
    helper.slotId = DEFAULT_SIM_SLOT_ID;
    if (!helper.Run(GetAllSimMessagesTestFuc, std::ref(helper))) {
        TELEPHONY_LOGI("GetAllSimMessagesTestFuc out of time");
        ASSERT_TRUE(false);
    }
    TELEPHONY_LOGI("TelSMSMMSTest::GetAllSimMessages_0003 -->finished");
    ASSERT_TRUE(helper.GetBoolResult());
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
    if (!(SmsMmsGtest::HasSimCard(DEFAULT_SIM_SLOT_ID))) {
        TELEPHONY_LOGI("TelephonyTestService has no sim card");
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
    ASSERT_TRUE(helper.GetIntResult() == 0);
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
    if (!(SmsMmsGtest::HasSimCard(DEFAULT_SIM_SLOT_ID_1))) {
        TELEPHONY_LOGI("TelephonyTestService has no sim card");
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
    ASSERT_TRUE(helper.GetIntResult() == 0);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_DelSimMessage_0003
 * @tc.name     Del Sim Message
 * @tc.desc     Function test
 */
HWTEST_F(SmsMmsGtest, DelSimMessage_0003, Function | MediumTest | Level3)
{
    TELEPHONY_LOGI("TelSMSMMSTest::DelSimMessage_0003 -->");
    if (!(SmsMmsGtest::HasSimCard(DEFAULT_SIM_SLOT_ID))) {
        TELEPHONY_LOGI("TelephonyTestService has no sim card");
        ASSERT_TRUE(true);
        return;
    }
    SmsMmsTestHelper helper;
    helper.slotId = DEFAULT_SIM_SLOT_ID;
    if (!helper.Run(DelSimMessageTestFuc, std::ref(helper))) {
        TELEPHONY_LOGI("DelSimMessageTestFuc out of time");
        ASSERT_TRUE(false);
    }
    TELEPHONY_LOGI("TelSMSMMSTest::DelSimMessage_0003 -->finished");
    EXPECT_FALSE(helper.GetIntResult() == 0);
}

void UpdateSimMessageTestFuc(SmsMmsTestHelper &helper)
{
    uint32_t msgIndex = 0;
    std::u16string smscData(u"");
    std::u16string pduData(u"01000B818176251308F4000007E8B0BCFD76E701");
    uint32_t status = 3;
    int32_t result = DelayedSingleton<SmsServiceManagerClient>::GetInstance()->UpdateSimMessage(
        helper.slotId, msgIndex, static_cast<ISmsServiceInterface::SimMessageStatus>(status), pduData, smscData);
    helper.SetIntResult(result);
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
    if (!(SmsMmsGtest::HasSimCard(DEFAULT_SIM_SLOT_ID))) {
        TELEPHONY_LOGI("TelephonyTestService has no sim card");
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
    ASSERT_TRUE(helper.GetIntResult() == 0);
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
    if (!(SmsMmsGtest::HasSimCard(DEFAULT_SIM_SLOT_ID_1))) {
        TELEPHONY_LOGI("TelephonyTestService has no sim card");
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
    ASSERT_TRUE(helper.GetIntResult() == 0);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_UpdateSimMessage_0003
 * @tc.name     Update Sim Message
 * @tc.desc     Function test
 */
HWTEST_F(SmsMmsGtest, UpdateSimMessage_0003, Function | MediumTest | Level3)
{
    TELEPHONY_LOGI("TelSMSMMSTest::UpdateSimMessage_0003 -->");
    if (!(SmsMmsGtest::HasSimCard(DEFAULT_SIM_SLOT_ID))) {
        TELEPHONY_LOGI("TelephonyTestService has no sim card");
        ASSERT_TRUE(true);
        return;
    }
    SmsMmsTestHelper helper;
    helper.slotId = DEFAULT_SIM_SLOT_ID;
    if (!helper.Run(UpdateSimMessageTestFuc, std::ref(helper))) {
        TELEPHONY_LOGI("UpdateSimMessageTestFuc out of time");
        ASSERT_TRUE(false);
    }
    TELEPHONY_LOGI("TelSMSMMSTest::UpdateSimMessage_0003 -->finished");
    EXPECT_FALSE(helper.GetIntResult() == 0);
}

void SetImsSmsConfigTestFuc(SmsMmsTestHelper &helper)
{
    DelayedSingleton<SmsServiceManagerClient>::GetInstance()->SetImsSmsConfig(helper.slotId, 1);
    bool isSupported = false;
    DelayedSingleton<SmsServiceManagerClient>::GetInstance()->IsImsSmsSupported(helper.slotId, isSupported);
    helper.SetBoolResult(isSupported);
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
    if (!(SmsMmsGtest::HasSimCard(DEFAULT_SIM_SLOT_ID))) {
        TELEPHONY_LOGI("TelephonyTestService has no sim card");
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
    bool isSupported = false;
    DelayedSingleton<SmsServiceManagerClient>::GetInstance()->SetImsSmsConfig(helper.slotId, 0);
    DelayedSingleton<SmsServiceManagerClient>::GetInstance()->IsImsSmsSupported(helper.slotId, isSupported);
    helper.SetBoolResult(isSupported);
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
    if (!(SmsMmsGtest::HasSimCard(DEFAULT_SIM_SLOT_ID))) {
        TELEPHONY_LOGI("TelephonyTestService has no sim card");
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
    if (!(SmsMmsGtest::HasSimCard(DEFAULT_SIM_SLOT_ID_1))) {
        TELEPHONY_LOGI("TelephonyTestService has no sim card");
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
    if (!(SmsMmsGtest::HasSimCard(DEFAULT_SIM_SLOT_ID_1))) {
        TELEPHONY_LOGI("TelephonyTestService has no sim card");
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

/**
 * @tc.number   Telephony_SmsMmsGtest_SetImsSmsConfig_0005
 * @tc.name     Enable IMS SMS
 * @tc.desc     Function test
 * @tc.require: issueI5K12U
 */
HWTEST_F(SmsMmsGtest, SetImsSmsConfig_0005, Function | MediumTest | Level2)
{
    TELEPHONY_LOGI("TelSMSMMSTest::SetImsSmsConfig_0005 -->");
    if (!(SmsMmsGtest::HasSimCard(DEFAULT_SIM_SLOT_ID))) {
        TELEPHONY_LOGI("TelephonyTestService has no sim card");
        ASSERT_TRUE(true);
        return;
    }
    SmsMmsTestHelper helper;
    helper.slotId = DEFAULT_SIM_SLOT_ID;
    if (!helper.Run(SetImsSmsConfigTestFuc, std::ref(helper))) {
        TELEPHONY_LOGI("SetImsSmsConfigTestFuc out of time");
        ASSERT_TRUE(false);
    }
    TELEPHONY_LOGI("TelSMSMMSTest::SetImsSmsConfig_0005 -->finished");
    EXPECT_FALSE(helper.GetBoolResult());
}

void SendDataMessageTestFuc(SmsMmsTestHelper &helper)
{
    std::string dest = DES_ADDR;
    std::string sca("");
    OHOS::sptr<SmsSendCallbackGTest> sendCallBackPtr(new SmsSendCallbackGTest(helper));
    OHOS::sptr<SmsDeliveryCallbackGTest> deliveryCallBackPtr(new SmsDeliveryCallbackGTest(helper));
    uint16_t port = SMS_PORT;
    if (sendCallBackPtr == nullptr) {
        TELEPHONY_LOGI("sendCallBackPtr is nullptr");
        helper.NotifyAll();
    }

    if (deliveryCallBackPtr == nullptr) {
        TELEPHONY_LOGI("deliveryCallBackPtr is nullptr");
        helper.NotifyAll();
    }
    sendCallBackPtr->HasDeliveryCallBack(true);
    DelayedSingleton<SmsServiceManagerClient>::GetInstance()->SendMessage(helper.slotId, StringUtils::ToUtf16(dest),
        StringUtils::ToUtf16(sca), port, DATA_SMS, (sizeof(DATA_SMS) / sizeof(DATA_SMS[0]) - 1), sendCallBackPtr,
        deliveryCallBackPtr);
}

void SendDataMessageTestFuc2(SmsMmsTestHelper &helper)
{
    std::string dest = DES_ADDR;
    std::string sca("");
    OHOS::sptr<SmsSendCallbackGTest> sendCallBackPtr(new SmsSendCallbackGTest(helper));
    OHOS::sptr<SmsDeliveryCallbackGTest> deliveryCallBackPtr(new SmsDeliveryCallbackGTest(helper));
    uint16_t port = SMS_PORT;
    if (sendCallBackPtr == nullptr) {
        TELEPHONY_LOGI("sendCallBackPtr is nullptr");
        helper.NotifyAll();
    }

    if (deliveryCallBackPtr == nullptr) {
        TELEPHONY_LOGI("deliveryCallBackPtr is nullptr");
        helper.NotifyAll();
    }
    sendCallBackPtr->HasDeliveryCallBack(false);
    DelayedSingleton<SmsServiceManagerClient>::GetInstance()->SendMessage(helper.slotId, StringUtils::ToUtf16(dest),
        StringUtils::ToUtf16(sca), port, DATA_SMS, (sizeof(DATA_SMS) / sizeof(DATA_SMS[0]) - 1), sendCallBackPtr,
        deliveryCallBackPtr);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_SendDataMessage_0001
 * @tc.name     Send Data Sms
 * @tc.desc     Function test
 */
HWTEST_F(SmsMmsGtest, SendDataMessage_0001, Function | MediumTest | Level2)
{
    AccessMmsToken token;
    TELEPHONY_LOGI("TelSMSMMSTest::SendDataMessage_0001 -->");
    int32_t slotId = DEFAULT_SIM_SLOT_ID;
    if (!(SmsMmsGtest::HasSimCard(slotId))) {
        TELEPHONY_LOGI("TelephonyTestService has no sim card");
        ASSERT_TRUE(true);
        return;
    }
    SmsMmsTestHelper helper;
    helper.slotId = slotId;
    if (!helper.Run(SendDataMessageTestFuc, helper)) {
        TELEPHONY_LOGI("SendDataMessageTestFuc out of time");
        ASSERT_TRUE(false);
    }
    TELEPHONY_LOGI("TelSMSMMSTest::SendDataMessage_0001 -->finished");
    ASSERT_TRUE(helper.GetSendSmsIntResult() == 0 && helper.GetDeliverySmsIntResult() == 0);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_SendDataMessage_0002
 * @tc.name     Send Data Sms
 * @tc.desc     Function test
 */
HWTEST_F(SmsMmsGtest, SendDataMessage_0002, Function | MediumTest | Level2)
{
    AccessMmsToken token;
    TELEPHONY_LOGI("TelSMSMMSTest::SendDataMessage_0002 -->");
    int32_t slotId = DEFAULT_SIM_SLOT_ID_1;
    if (!(SmsMmsGtest::HasSimCard(slotId))) {
        TELEPHONY_LOGI("TelephonyTestService has no sim card");
        ASSERT_TRUE(true);
        return;
    }
    SmsMmsTestHelper helper;
    helper.slotId = slotId;
    if (!helper.Run(SendDataMessageTestFuc, helper)) {
        TELEPHONY_LOGI("SendDataMessageTestFuc out of time");
        ASSERT_TRUE(false);
    }
    TELEPHONY_LOGI("TelSMSMMSTest::SendDataMessage_0002 -->finished");
    ASSERT_TRUE(helper.GetSendSmsIntResult() == 0 && helper.GetDeliverySmsIntResult() == 0);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_SendDataMessage_0003
 * @tc.name     Send Data Sms
 * @tc.desc     Function test
 */
HWTEST_F(SmsMmsGtest, SendDataMessage_0003, Function | MediumTest | Level2)
{
    TELEPHONY_LOGI("TelSMSMMSTest::SendDataMessage_0003 -->");
    int32_t slotId = DEFAULT_SIM_SLOT_ID;
    if (!(SmsMmsGtest::HasSimCard(slotId))) {
        TELEPHONY_LOGI("TelephonyTestService has no sim card");
        ASSERT_TRUE(true);
        return;
    }
    SmsMmsTestHelper helper;
    helper.slotId = slotId;
    if (!helper.Run(SendDataMessageTestFuc2, helper)) {
        TELEPHONY_LOGI("SendDataMessageTestFuc out of time");
        ASSERT_TRUE(false);
    }
    TELEPHONY_LOGI("TelSMSMMSTest::SendDataMessage_0003 -->finished");
    EXPECT_FALSE(helper.GetSendSmsIntResult() == 0);
}

void SendTextMessageTestFuc(SmsMmsTestHelper &helper)
{
    std::string dest = DES_ADDR;
    std::string sca("");
    OHOS::sptr<SmsSendCallbackGTest> sendCallBackPtr(new SmsSendCallbackGTest(helper));
    OHOS::sptr<SmsDeliveryCallbackGTest> deliveryCallBackPtr(new SmsDeliveryCallbackGTest(helper));
    std::string text = TEXT_SMS_CONTENT;
    if (sendCallBackPtr == nullptr) {
        TELEPHONY_LOGI("sendCallBackPtr is nullptr");
        helper.NotifyAll();
    }

    if (deliveryCallBackPtr == nullptr) {
        TELEPHONY_LOGI("deliveryCallBackPtr is nullptr");
        helper.NotifyAll();
    }
    sendCallBackPtr->HasDeliveryCallBack(true);
    DelayedSingleton<SmsServiceManagerClient>::GetInstance()->SendMessage(helper.slotId, StringUtils::ToUtf16(dest),
        StringUtils::ToUtf16(sca), StringUtils::ToUtf16(text), sendCallBackPtr, deliveryCallBackPtr);
}

void SendTextMessageTestFuc2(SmsMmsTestHelper &helper)
{
    std::string dest = DES_ADDR;
    std::string sca("");
    OHOS::sptr<SmsSendCallbackGTest> sendCallBackPtr(new SmsSendCallbackGTest(helper));
    OHOS::sptr<SmsDeliveryCallbackGTest> deliveryCallBackPtr(new SmsDeliveryCallbackGTest(helper));
    std::string text = TEXT_SMS_CONTENT;
    if (sendCallBackPtr == nullptr) {
        TELEPHONY_LOGI("sendCallBackPtr is nullptr");
        helper.NotifyAll();
    }

    if (deliveryCallBackPtr == nullptr) {
        TELEPHONY_LOGI("deliveryCallBackPtr is nullptr");
        helper.NotifyAll();
    }
    sendCallBackPtr->HasDeliveryCallBack(false);
    DelayedSingleton<SmsServiceManagerClient>::GetInstance()->SendMessage(helper.slotId, StringUtils::ToUtf16(dest),
        StringUtils::ToUtf16(sca), StringUtils::ToUtf16(text), sendCallBackPtr, deliveryCallBackPtr);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_SendTextMessage_0001
 * @tc.name     Send Text Sms
 * @tc.desc     Function test
 */
HWTEST_F(SmsMmsGtest, SendTextMessage_0001, Function | MediumTest | Level2)
{
    AccessMmsToken token;
    TELEPHONY_LOGI("TelSMSMMSTest::SendTextMessage_0001 -->");
    int32_t slotId = DEFAULT_SIM_SLOT_ID;
    if (!(SmsMmsGtest::HasSimCard(slotId))) {
        TELEPHONY_LOGI("TelephonyTestService has no sim card");
        ASSERT_TRUE(true);
        return;
    }
    SmsMmsTestHelper helper;
    helper.slotId = slotId;
    if (!helper.Run(SendTextMessageTestFuc, helper)) {
        TELEPHONY_LOGI("SendTextMessageTestFuc out of time");
        ASSERT_TRUE(false);
    }
    TELEPHONY_LOGI("TelSMSMMSTest::SendTextMessage_0001 -->finished");
    ASSERT_TRUE(helper.GetSendSmsIntResult() == 0 && helper.GetDeliverySmsIntResult() == 0);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_SendTextMessage_0002
 * @tc.name     Send Text Sms
 * @tc.desc     Function test
 */
HWTEST_F(SmsMmsGtest, SendTextMessage_0002, Function | MediumTest | Level2)
{
    AccessMmsToken token;
    TELEPHONY_LOGI("TelSMSMMSTest::SendTextMessage_0002 -->");
    int32_t slotId = DEFAULT_SIM_SLOT_ID_1;
    if (!(SmsMmsGtest::HasSimCard(slotId))) {
        TELEPHONY_LOGI("TelephonyTestService has no sim card");
        ASSERT_TRUE(true);
        return;
    }
    SmsMmsTestHelper helper;
    helper.slotId = slotId;
    if (!helper.Run(SendTextMessageTestFuc, helper)) {
        TELEPHONY_LOGI("SendTextMessageTestFuc out of time");
        ASSERT_TRUE(false);
    }
    TELEPHONY_LOGI("TelSMSMMSTest::SendTextMessage_0001 -->finished");
    ASSERT_TRUE(helper.GetSendSmsIntResult() == 0 && helper.GetDeliverySmsIntResult() == 0);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_SendTextMessage_0003
 * @tc.name     Send Text Sms
 * @tc.desc     Function test
 */
HWTEST_F(SmsMmsGtest, SendTextMessage_0003, Function | MediumTest | Level2)
{
    TELEPHONY_LOGI("TelSMSMMSTest::SendTextMessage_0003 -->");
    int32_t slotId = DEFAULT_SIM_SLOT_ID;
    if (!(SmsMmsGtest::HasSimCard(slotId))) {
        TELEPHONY_LOGI("TelephonyTestService has no sim card");
        ASSERT_TRUE(true);
        return;
    }
    SmsMmsTestHelper helper;
    helper.slotId = slotId;
    if (!helper.Run(SendTextMessageTestFuc2, helper)) {
        TELEPHONY_LOGI("SendTextMessageTestFuc out of time");
        ASSERT_TRUE(false);
    }
    TELEPHONY_LOGI("TelSMSMMSTest::SendTextMessage_0003 -->finished");
    EXPECT_FALSE(helper.GetSendSmsIntResult() == 0);
}

void GetSmsSegmentsInfoTestFuc(SmsMmsTestHelper &helper)
{
    std::u16string message = u"";
    bool force7BitCode = false;
    ISmsServiceInterface::SmsSegmentsInfo result;
    DelayedSingleton<SmsServiceManagerClient>::GetInstance()->GetSmsSegmentsInfo(
        helper.slotId, message, force7BitCode, result);
    bool isSupported = false;
    DelayedSingleton<SmsServiceManagerClient>::GetInstance()->IsImsSmsSupported(helper.slotId, isSupported);
    helper.SetBoolResult(isSupported);
    helper.NotifyAll();
}

void GetSmsSegmentsInfoTestFuc2(SmsMmsTestHelper &helper)
{
    std::u16string message = u"message";
    bool force7BitCode = true;
    ISmsServiceInterface::SmsSegmentsInfo result;
    DelayedSingleton<SmsServiceManagerClient>::GetInstance()->GetSmsSegmentsInfo(
        helper.slotId, message, force7BitCode, result);
    bool isSupported = false;
    DelayedSingleton<SmsServiceManagerClient>::GetInstance()->IsImsSmsSupported(helper.slotId, isSupported);
    helper.SetBoolResult(isSupported);
    helper.NotifyAll();
}

/**
 * @tc.number   Telephony_SmsMmsGtest_GetSmsSegmentsInfo_0001
 * @tc.name     Send Text Sms
 * @tc.desc     Function test
 */
HWTEST_F(SmsMmsGtest, GetSmsSegmentsInfo_0001, Function | MediumTest | Level2)
{
    TELEPHONY_LOGI("TelSMSMMSTest::GetSmsSegmentsInfo_0001 -->");
    int32_t slotId = DEFAULT_SIM_SLOT_ID;
    if (!(SmsMmsGtest::HasSimCard(slotId))) {
        TELEPHONY_LOGI("TelephonyTestService has no sim card");
        ASSERT_TRUE(true);
        return;
    }
    SmsMmsTestHelper helper;
    helper.slotId = slotId;
    if (!helper.Run(GetSmsSegmentsInfoTestFuc, helper)) {
        TELEPHONY_LOGI("GetSmsSegmentsInfoTestFuc out of time");
        ASSERT_TRUE(false);
    }
    TELEPHONY_LOGI("TelSMSMMSTest::GetSmsSegmentsInfo_0001 -->finished");
    EXPECT_FALSE(helper.GetBoolResult());
}

/**
 * @tc.number   Telephony_SmsMmsGtest_GetSmsSegmentsInfo_0002
 * @tc.name     Send Text Sms
 * @tc.desc     Function test
 */
HWTEST_F(SmsMmsGtest, GetSmsSegmentsInfo_0002, Function | MediumTest | Level2)
{
    TELEPHONY_LOGI("TelSMSMMSTest::GetSmsSegmentsInfo_0002 -->");
    int32_t slotId = DEFAULT_SIM_SLOT_ID;
    if (!(SmsMmsGtest::HasSimCard(slotId))) {
        TELEPHONY_LOGI("TelephonyTestService has no sim card");
        ASSERT_TRUE(true);
        return;
    }
    SmsMmsTestHelper helper;
    helper.slotId = slotId;
    if (!helper.Run(GetSmsSegmentsInfoTestFuc2, helper)) {
        TELEPHONY_LOGI("GetSmsSegmentsInfoTestFuc2 out of time");
        ASSERT_TRUE(false);
    }
    TELEPHONY_LOGI("TelSMSMMSTest::GetSmsSegmentsInfo_0002 -->finished");
    EXPECT_FALSE(helper.GetBoolResult());
}

/**
 * @tc.number   Telephony_SmsMmsGtest_GetSmsSegmentsInfo_0003
 * @tc.name     Send Text Sms
 * @tc.desc     Function test
 */
HWTEST_F(SmsMmsGtest, GetSmsSegmentsInfo_0003, Function | MediumTest | Level2)
{
    TELEPHONY_LOGI("TelSMSMMSTest::GetSmsSegmentsInfo_0003 -->");
    int32_t slotId = DEFAULT_SIM_SLOT_ID_1;
    if (!(SmsMmsGtest::HasSimCard(slotId))) {
        TELEPHONY_LOGI("TelephonyTestService has no sim card");
        ASSERT_TRUE(true);
        return;
    }
    SmsMmsTestHelper helper;
    helper.slotId = slotId;
    if (!helper.Run(GetSmsSegmentsInfoTestFuc, helper)) {
        TELEPHONY_LOGI("GetSmsSegmentsInfoTestFuc3 out of time");
        ASSERT_TRUE(false);
    }
    TELEPHONY_LOGI("TelSMSMMSTest::GetSmsSegmentsInfo_0003 -->finished");
    EXPECT_FALSE(helper.GetBoolResult());
}

/**
 * @tc.number   Telephony_SmsMmsGtest_MmsAddress_0001
 * @tc.name     Test MmsAddress
 * @tc.desc     Function test
 */
HWTEST_F(SmsMmsGtest, MmsAddress_0001, Function | MediumTest | Level1)
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
HWTEST_F(SmsMmsGtest, MmsAttachment_0001, Function | MediumTest | Level1)
{
    TELEPHONY_LOGI("TelSMSMMSTest::MmsAttachment_0001 -->");
    const std::string pathName = "/data/telephony/enSrc/618C0A89.smil";
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
}

/**
 * @tc.number   Telephony_SmsMmsGtest_MmsAttachment_0002
 * @tc.name     Test MmsAttachment
 * @tc.desc     Function test
 */
HWTEST_F(SmsMmsGtest, MmsAttachment_0002, Function | MediumTest | Level1)
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
    EXPECT_EQ(false, retBool);
    retBool = attachment.SetDataBuffer(std::make_unique<char[]>(len + 1), len + 1);
    EXPECT_EQ(false, retBool);
    retBool = attachment.SetDataBuffer(std::make_unique<char[]>(len - 1), len - 1);
    EXPECT_EQ(true, retBool);
    retBool = attachment.SetDataBuffer(std::make_unique<char[]>(len - 1), len + 1);
    EXPECT_EQ(false, retBool);
    EXPECT_FALSE(attachment.GetDataBuffer(len) == nullptr);
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
HWTEST_F(SmsMmsGtest, MmsBodyPartHeader_0001, Function | MediumTest | Level1)
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
HWTEST_F(SmsMmsGtest, MmsBuffer_0001, Function | MediumTest | Level1)
{
    TELEPHONY_LOGI("TelSMSMMSTest::MmsBuffer_0001 -->");
    MmsBuffer mmsBuffer;
    uint32_t len = 10;
    bool retBool;
    std::string strPathName = "/data/telephony/enSrc/618C0A89.smil";
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
HWTEST_F(SmsMmsGtest, MmsContentParam_0001, Function | MediumTest | Level1)
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
}

/**
 * @tc.number   Telephony_SmsMmsGtest_MmsContentType_0001
 * @tc.name     Test MmsContentType
 * @tc.desc     Function test
 */
HWTEST_F(SmsMmsGtest, MmsContentType_0001, Function | MediumTest | Level1)
{
    TELEPHONY_LOGI("TelSMSMMSTest::MmsContentType_0001 -->");
    MmsContentType mmsContentType;
    MmsDecodeBuffer decodeBuffer;
    MmsContentParam contentParam;
    int32_t len = 10;
    uint8_t type = 10;
    std::string testStr;
    mmsContentType.DumpMmsContentType();
    mmsContentType.DecodeMmsContentType(decodeBuffer, len);
    mmsContentType.DecodeMmsCTGeneralForm(decodeBuffer, len);
    mmsContentType.GetContentTypeFromInt(type);
    mmsContentType.GetContentTypeFromString("");
    mmsContentType.DecodeParameter(decodeBuffer, len);
    mmsContentType.SetContentParam(contentParam);
    mmsContentType.DecodeTextField(decodeBuffer, type, len);
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
HWTEST_F(SmsMmsGtest, MmsDecodeBuffer_0001, Function | MediumTest | Level1)
{
    TELEPHONY_LOGI("TelSMSMMSTest::MmsDecodeBuffer_0001 -->");
    uint8_t byteVar = 1;
    uint32_t intVar = 10;
    uint64_t longVar = 10;
    std::string testStr;
    MmsDecodeBuffer mmsDecodeBuffer;
    mmsDecodeBuffer.PeekOneByte(byteVar);
    mmsDecodeBuffer.GetOneByte(byteVar);
    mmsDecodeBuffer.IncreasePointer(intVar);
    mmsDecodeBuffer.DecreasePointer(intVar);
    mmsDecodeBuffer.DecodeUintvar(intVar, intVar);
    mmsDecodeBuffer.DecodeShortLength(byteVar);
    mmsDecodeBuffer.DecodeValueLengthReturnLen(intVar, intVar);
    mmsDecodeBuffer.DecodeValueLength(intVar);
    mmsDecodeBuffer.DecodeTokenText(testStr, intVar);
    mmsDecodeBuffer.DecodeText(testStr, intVar);
    mmsDecodeBuffer.DecodeQuotedText(testStr, intVar);
    mmsDecodeBuffer.DecodeShortInteger(byteVar);
    mmsDecodeBuffer.DecodeLongInteger(longVar);
    mmsDecodeBuffer.DecodeInteger(longVar);
    mmsDecodeBuffer.DecodeIsShortInt();
    mmsDecodeBuffer.DecodeIsString();
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
HWTEST_F(SmsMmsGtest, MmsEncodeString_0001, Function | MediumTest | Level1)
{
    TELEPHONY_LOGI("TelSMSMMSTest::MmsEncodeString_0001 -->");
    MmsEncodeString mmsEncodeString;
    MmsDecodeBuffer decodeBuffer;
    MmsEncodeBuffer encodeBuffer;
    std::string testStr;
    uint32_t charset = 10;
    MmsAddress addrsss;
    mmsEncodeString.DecodeEncodeString(decodeBuffer);
    mmsEncodeString.EncodeEncodeString(encodeBuffer);
    mmsEncodeString.GetEncodeString(testStr);
    mmsEncodeString.SetAddressString(addrsss);
    MmsEncodeString mmsEncodeString1(mmsEncodeString);
    bool ret = mmsEncodeString1.SetEncodeString(charset, testStr);
    EXPECT_EQ(true, ret);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_MmsHeaderCateg_0001
 * @tc.name     Test MmsHeaderCateg
 * @tc.desc     Function test
 */
HWTEST_F(SmsMmsGtest, MmsHeaderCateg_0001, Function | MediumTest | Level1)
{
    TELEPHONY_LOGI("TelSMSMMSTest::MmsHeaderCateg_0001 -->");
    MmsHeaderCateg mmsHeaderCateg;
    uint8_t fieldId = 0;
    mmsHeaderCateg.FindSendReqOptType(fieldId);
    mmsHeaderCateg.FindSendConfOptType(fieldId);
    bool ret = mmsHeaderCateg.CheckIsValueLen(fieldId);
    EXPECT_EQ(false, ret);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_MmsBase64_0001
 * @tc.name     Test MmsBase64
 * @tc.desc     Function test
 */
HWTEST_F(SmsMmsGtest, MmsBase64_0001, Function | MediumTest | Level1)
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
HWTEST_F(SmsMmsGtest, MmsCharSet_0001, Function | MediumTest | Level1)
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
HWTEST_F(SmsMmsGtest, MmsQuotedPrintable_0001, Function | MediumTest | Level1)
{
    TELEPHONY_LOGI("TelSMSMMSTest::MmsQuotedPrintable_0001 -->");
    MmsQuotedPrintable mmsQuotedPrintable;
    std::string valueStr = "123";
    mmsQuotedPrintable.Encode(valueStr);
    mmsQuotedPrintable.Decode(valueStr, valueStr);
    bool ret = mmsQuotedPrintable.Decode("", valueStr);
    EXPECT_EQ(false, ret);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_CdmaSmsMessage_0001
 * @tc.name     Test CdmaSmsMessage
 * @tc.desc     Function test
 */
HWTEST_F(SmsMmsGtest, CdmaSmsMessage_0001, Function | MediumTest | Level1)
{
    TELEPHONY_LOGI("TelSMSMMSTest::CdmaSmsMessage_0001 -->");
    CdmaSmsMessage cdmaSmsMessage;
    std::string dest = "dest";
    std::string sc = "sc";
    std::string text = "text";
    int32_t port = 10;
    uint8_t *data;
    uint32_t dataLen = 10;
    std::string pdu = "01000B818176251308F4000007E8B0BCFD76E701";
    bool bStatusReport = false;
    SmsCodingScheme codingScheme = SMS_CODING_7BIT;
    cdmaSmsMessage.CreateSubmitTransMsg(dest, sc, text, bStatusReport, codingScheme);
    cdmaSmsMessage.CreateSubmitTransMsg(dest, sc, port, data, dataLen, bStatusReport);
    cdmaSmsMessage.GreateTransMsg();
    cdmaSmsMessage.CovertEncodingType(codingScheme);
    cdmaSmsMessage.CreateMessage(pdu);
    cdmaSmsMessage.PduAnalysis(pdu);
    SmsTransP2PMsg p2pMsg;
    cdmaSmsMessage.AnalysisP2pMsg(p2pMsg);
    cdmaSmsMessage.AnalsisDeliverMwi(p2pMsg);
    bool ret = cdmaSmsMessage.PduAnalysis("");
    EXPECT_EQ(false, ret);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_CdmaSmsPduCodec_0001
 * @tc.name     Test CdmaSmsPduCodec
 * @tc.desc     Function test
 */
HWTEST_F(SmsMmsGtest, CdmaSmsPduCodec_0001, Function | MediumTest | Level1)
{
    TELEPHONY_LOGI("TelSMSMMSTest::CdmaSmsPduCodec_0001 -->");
    CdmaSmsPduCodec cdmaSmsPduCodec;
    unsigned char c = 'a';
    unsigned char *src1 = &c;
    unsigned char *src2 = nullptr;
    unsigned int nBytes = 1;
    unsigned int nShiftBit = 1;
    cdmaSmsPduCodec.ShiftNBit(src1, nBytes, nShiftBit);
    cdmaSmsPduCodec.ShiftNBit(src2, nBytes, nShiftBit);
    cdmaSmsPduCodec.ShiftRNBit(src1, nBytes, nShiftBit);
    cdmaSmsPduCodec.ShiftRNBit(src2, nBytes, nShiftBit);
    cdmaSmsPduCodec.ShiftNBitForDecode(src1, nBytes, nShiftBit);
    cdmaSmsPduCodec.ShiftNBitForDecode(src2, nBytes, nShiftBit);
    unsigned char ret = cdmaSmsPduCodec.DecodeDigitModeNumberPlan(SmsNumberPlanType::SMS_NPI_UNKNOWN);
    EXPECT_EQ(SmsNumberPlanType::SMS_NPI_UNKNOWN, ret);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_CdmaSmsReceiveHandler_0001
 * @tc.name     Test CdmaSmsReceiveHandler
 * @tc.desc     Function test
 */
HWTEST_F(SmsMmsGtest, CdmaSmsReceiveHandler_0001, Function | MediumTest | Level1)
{
    TELEPHONY_LOGI("TelSMSMMSTest::CdmaSmsReceiveHandler_0001 -->");
    int32_t retInt;
    std::shared_ptr<SmsBaseMessage> retPtr;
    std::string pdu = "01000B818176251308F4000007E8B0BCFD76E701";
    std::shared_ptr<AppExecFwk::EventRunner> cdmaSmsReceiveRunner;
    cdmaSmsReceiveRunner = AppExecFwk::EventRunner::Create("cdmaSmsReceiveHandler");
    ASSERT_TRUE(cdmaSmsReceiveRunner != nullptr);
    CdmaSmsReceiveHandler cdmaSmsReceiveHandler(cdmaSmsReceiveRunner, DEFAULT_SIM_SLOT_ID);
    const std::shared_ptr<CdmaSmsMessage> smsCdmaMessage = CdmaSmsMessage::CreateMessage(pdu);
    retInt = cdmaSmsReceiveHandler.HandleSmsByType(nullptr);
    EXPECT_EQ(AckIncomeCause::SMS_ACK_UNKNOWN_ERROR, retInt);
}

/**
 * @tc.number   Sms_TestDump_0100
 * @tc.name    TestDump
 * @tc.desc     Function test
 */
HWTEST_F(SmsMmsGtest, Sms_TestDump_0001, Function | MediumTest | Level3)
{
    std::vector<std::u16string> emptyArgs = {};
    std::vector<std::u16string> args = { u"test", u"test1" };
    EXPECT_EQ(DelayedSingleton<SmsService>::GetInstance()->Dump(-1, args), TELEPHONY_ERR_FAIL);
    EXPECT_EQ(DelayedSingleton<SmsService>::GetInstance()->Dump(0, emptyArgs), 0);
    EXPECT_EQ(DelayedSingleton<SmsService>::GetInstance()->Dump(0, args), 0);
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
