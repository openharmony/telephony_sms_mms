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

#include "gtest/gtest.h"
#include "if_system_ability_manager.h"
#include "ipc_skeleton.h"
#include "iservice_registry.h"
#include "i_sms_service_interface.h"
#include "sms_service_proxy.h"
#include "system_ability_definition.h"
#include "telephony_types.h"
#include "telephony_log_wrapper.h"

#include "core_service_client.h"
#include "string_utils.h"

namespace OHOS {
namespace Telephony {
namespace {
sptr<ISmsServiceInterface> g_telephonyService = nullptr;
} // namespace
using namespace testing::ext;
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

void SmsMmsGtest::SetUpTestCase()
{
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

/**
 * @tc.number   Telephony_SmsMmsGtest_OpenCellBroadcast_0001
 * @tc.name     Open cellBroadcast slotId is -1
 * @tc.desc     Function test
 */
HWTEST_F(SmsMmsGtest, OpenCellBroadcast_0001, Function | MediumTest | Level1)
{
    bool result = true;
    if (g_telephonyService != nullptr) {
        int32_t slotId = -1;
        bool enable = true;
        uint32_t fromMsgId = 0;
        uint32_t toMsgId = 10;
        uint8_t netType = 1;
        TELEPHONY_LOGI("TelSMSMMSTest::OpenCellBroadcast_0001 -->");
        result = g_telephonyService->SetCBConfig(slotId, enable, fromMsgId, toMsgId, netType);
        TELEPHONY_LOGI("TelSMSMMSTest::OpenCellBroadcast_0001 -->finished");
        EXPECT_FALSE(result);
    }
}

/**
 * @tc.number   Telephony_SmsMmsGtest_OpenCellBroadcast_0002
 * @tc.name     Open cellBroadcast fromMsgId less than toMsgId
 * @tc.desc     Function test
 */
HWTEST_F(SmsMmsGtest, OpenCellBroadcast_0002, Function | MediumTest | Level2)
{
    bool result = true;
    if (g_telephonyService != nullptr) {
        int32_t slotId = DEFAULT_SIM_SLOT_ID;
        bool enable = true;
        uint32_t fromMsgId = 20;
        uint32_t toMsgId = 10;
        uint8_t netType = 1;
        TELEPHONY_LOGI("TelSMSMMSTest::OpenCellBroadcast_0002 -->");
        result = g_telephonyService->SetCBConfig(slotId, enable, fromMsgId, toMsgId, netType);
        TELEPHONY_LOGI("TelSMSMMSTest::OpenCellBroadcast_0002 -->finished");
        EXPECT_FALSE(result);
    }
}

/**
 * @tc.number   Telephony_SmsMmsGtest_OpenCellBroadcast_0003
 * @tc.name     Open cellBroadcast netType is unknown.
 * @tc.desc     Function test
 */
HWTEST_F(SmsMmsGtest, OpenCellBroadcast_0003, Function | MediumTest | Level3)
{
    bool result = true;
    if (g_telephonyService != nullptr) {
        int32_t slotId = DEFAULT_SIM_SLOT_ID;
        bool enable = true;
        uint32_t fromMsgId = 0;
        uint32_t toMsgId = 10;
        uint8_t netType = 3;
        TELEPHONY_LOGI("TelSMSMMSTest::OpenCellBroadcast_0003 -->");
        result = g_telephonyService->SetCBConfig(slotId, enable, fromMsgId, toMsgId, netType);
        TELEPHONY_LOGI("TelSMSMMSTest::OpenCellBroadcast_0003 -->finished");
        EXPECT_FALSE(result);
    }
}

/**
 * @tc.number   Telephony_SmsMmsGtest_OpenCellBroadcast_0004
 * @tc.name     Open cellBroadcast parameter is valid.
 * @tc.desc     Function test
 */
HWTEST_F(SmsMmsGtest, OpenCellBroadcast_0004, Function | MediumTest | Level3)
{
    bool result = false;
    int32_t slotId = DEFAULT_SIM_SLOT_ID;
    if ((g_telephonyService != nullptr) && SmsMmsGtest::HasSimCard(slotId)) {
        bool enable = true;
        uint32_t fromMsgId = 0;
        uint32_t toMsgId = 10;
        uint8_t netType = 1;
        TELEPHONY_LOGI("TelSMSMMSTest::OpenCellBroadcast_0004 -->");
        result = g_telephonyService->SetCBConfig(slotId, enable, fromMsgId, toMsgId, netType);
        TELEPHONY_LOGI("TelSMSMMSTest::OpenCellBroadcast_0004 -->finished");
        EXPECT_TRUE(result);
    }
}

/**
 * @tc.number   Telephony_SmsMmsGtest_OpenCellBroadcast_0005
 * @tc.name     Open cellBroadcast parameter is valid
 * @tc.desc     Function test
 */
HWTEST_F(SmsMmsGtest, OpenCellBroadcast_0005, Function | MediumTest | Level3)
{
    bool result = false;
    int32_t slotId = DEFAULT_SIM_SLOT_ID;
    if ((g_telephonyService != nullptr) && SmsMmsGtest::HasSimCard(slotId)) {
        bool enable = true;
        uint32_t fromMsgId = 0;
        uint32_t toMsgId = 1000;
        uint8_t netType = 1;
        TELEPHONY_LOGI("TelSMSMMSTest::OpenCellBroadcast_0005 -->");
        result = g_telephonyService->SetCBConfig(slotId, enable, fromMsgId, toMsgId, netType);
        TELEPHONY_LOGI("TelSMSMMSTest::OpenCellBroadcast_0005 -->finished");
        EXPECT_TRUE(result);
    }
}

/**
 * @tc.number   Telephony_SmsMmsGtest_OpenCellBroadcast_0006
 * @tc.name     Open cellBroadcast parameter is valid
 * @tc.desc     Function test
 */
HWTEST_F(SmsMmsGtest, OpenCellBroadcast_0006, Function | MediumTest | Level4)
{
    bool result = false;
    int32_t slotId = DEFAULT_SIM_SLOT_ID;
    if ((g_telephonyService != nullptr) && SmsMmsGtest::HasSimCard(slotId)) {
        bool enable = true;
        uint32_t fromMsgId = 0;
        uint32_t toMsgId = 0;
        uint8_t netType = 1;
        TELEPHONY_LOGI("TelSMSMMSTest::OpenCellBroadcast_0006 -->");
        result = g_telephonyService->SetCBConfig(slotId, enable, fromMsgId, toMsgId, netType);
        TELEPHONY_LOGI("TelSMSMMSTest::OpenCellBroadcast_0006 -->finished");
        EXPECT_TRUE(result);
    }
}

/**
 * @tc.number   Telephony_SmsMmsGtest_CloseCellBroadcast_0001
 * @tc.name     Close cellBroadcast slotId is -1
 * @tc.desc     Function test
 */
HWTEST_F(SmsMmsGtest, CloseCellBroadcast_0001, Function | MediumTest | Level1)
{
    bool result = true;
    if (g_telephonyService != nullptr) {
        int32_t slotId = -1;
        bool enable = false;
        uint32_t fromMsgId = 0;
        uint32_t toMsgId = 10;
        uint8_t netType = 1;
        TELEPHONY_LOGI("TelSMSMMSTest::CloseCellBroadcast_0001 -->");
        result = g_telephonyService->SetCBConfig(slotId, enable, fromMsgId, toMsgId, netType);
        TELEPHONY_LOGI("TelSMSMMSTest::CloseCellBroadcast_0001 -->finished");
        EXPECT_FALSE(result);
    }
}

/**
 * @tc.number   Telephony_SmsMmsGtest_CloseCellBroadcast_0002
 * @tc.name     Close cellBroadcast fromMsgId less than toMsgId
 * @tc.desc     Function test
 */
HWTEST_F(SmsMmsGtest, CloseCellBroadcast_0002, Function | MediumTest | Level2)
{
    bool result = true;
    if (g_telephonyService != nullptr) {
        int32_t slotId = DEFAULT_SIM_SLOT_ID;
        bool enable = false;
        uint32_t fromMsgId = 20;
        uint32_t toMsgId = 10;
        uint8_t netType = 1;
        TELEPHONY_LOGI("TelSMSMMSTest::CloseCellBroadcast_0002 -->");
        result = g_telephonyService->SetCBConfig(slotId, enable, fromMsgId, toMsgId, netType);
        TELEPHONY_LOGI("TelSMSMMSTest::CloseCellBroadcast_0002 -->finished");
        EXPECT_FALSE(result);
    }
}

/**
 * @tc.number   Telephony_SmsMmsGtest_CloseCellBroadcast_0003
 * @tc.name     Close cellBroadcast netType is unknown
 * @tc.desc     Function test
 */
HWTEST_F(SmsMmsGtest, CloseCellBroadcast_0003, Function | MediumTest | Level3)
{
    bool result = true;
    if (g_telephonyService != nullptr) {
        int32_t slotId = DEFAULT_SIM_SLOT_ID;
        bool enable = false;
        uint32_t fromMsgId = 0;
        uint32_t toMsgId = 10;
        uint8_t netType = 3;
        TELEPHONY_LOGI("TelSMSMMSTest::CloseCellBroadcast_0003 -->");
        result = g_telephonyService->SetCBConfig(slotId, enable, fromMsgId, toMsgId, netType);
        TELEPHONY_LOGI("TelSMSMMSTest::CloseCellBroadcast_0003 -->finished");
        EXPECT_FALSE(result);
    }
}

/**
 * @tc.number   Telephony_SmsMmsGtest_CloseCellBroadcast_0004
 * @tc.name     Close cellBroadcast parameter is valid
 * @tc.desc     Function test
 */
HWTEST_F(SmsMmsGtest, CloseCellBroadcast_0004, Function | MediumTest | Level3)
{
    bool result = false;
    int32_t slotId = DEFAULT_SIM_SLOT_ID;
    if ((g_telephonyService != nullptr) && SmsMmsGtest::HasSimCard(slotId)) {
        bool enable = false;
        uint32_t fromMsgId = 0;
        uint32_t toMsgId = 10;
        uint8_t netType = 1;
        TELEPHONY_LOGI("TelSMSMMSTest::CloseCellBroadcast_0004 -->");
        result = g_telephonyService->SetCBConfig(slotId, enable, fromMsgId, toMsgId, netType);
        TELEPHONY_LOGI("TelSMSMMSTest::CloseCellBroadcast_0004 -->finished");
        EXPECT_TRUE(result);
    }
}

/**
 * @tc.number   Telephony_SmsMmsGtest_CloseCellBroadcast_0005
 * @tc.name     Close cellBroadcast parameter is valid
 * @tc.desc     Function test
 */
HWTEST_F(SmsMmsGtest, CloseCellBroadcast_0005, Function | MediumTest | Level3)
{
    bool result = false;
    int32_t slotId = DEFAULT_SIM_SLOT_ID;
    if ((g_telephonyService != nullptr) && SmsMmsGtest::HasSimCard(slotId)) {
        bool enable = false;
        uint32_t fromMsgId = 0;
        uint32_t toMsgId = 1000;
        uint8_t netType = 1;
        TELEPHONY_LOGI("TelSMSMMSTest::CloseCellBroadcast_0005 -->");
        result = g_telephonyService->SetCBConfig(slotId, enable, fromMsgId, toMsgId, netType);
        TELEPHONY_LOGI("TelSMSMMSTest::CloseCellBroadcast_0005 -->finished");
        EXPECT_TRUE(result);
    }
}

/**
 * @tc.number   Telephony_SmsMmsGtest_CloseCellBroadcast_0006
 * @tc.name     Close cellBroadcast parameter is valid
 * @tc.desc     Function test
 */
HWTEST_F(SmsMmsGtest, CloseCellBroadcast_0006, Function | MediumTest | Level4)
{
    bool result = false;
    int32_t slotId = DEFAULT_SIM_SLOT_ID;
    if ((g_telephonyService != nullptr) && SmsMmsGtest::HasSimCard(slotId)) {
        bool enable = false;
        uint32_t fromMsgId = 0;
        uint32_t toMsgId = 0;
        uint8_t netType = 1;
        TELEPHONY_LOGI("TelSMSMMSTest::CloseCellBroadcast_0006 -->");
        result = g_telephonyService->SetCBConfig(slotId, enable, fromMsgId, toMsgId, netType);
        TELEPHONY_LOGI("TelSMSMMSTest::CloseCellBroadcast_0006 -->finished");
        EXPECT_TRUE(result);
    }
}

/**
 * @tc.number   Telephony_SmsMmsGtest_SetDefaultSmsSlotId_0001
 * @tc.name     Set Default Sms SlotId slotId is valid
 * @tc.desc     Function test
 */
HWTEST_F(SmsMmsGtest, SetDefaultSmsSlotId_0001, Function | MediumTest | Level3)
{
    bool result = false;
    int32_t slotId = 0;
    if ((g_telephonyService != nullptr) && SmsMmsGtest::HasSimCard(slotId)) {
        TELEPHONY_LOGI("TelSMSMMSTest::SetDefaultSmsSlotId_0001 -->");
        result = g_telephonyService->SetDefaultSmsSlotId(slotId);
        TELEPHONY_LOGI("TelSMSMMSTest::SetDefaultSmsSlotId_0001 -->finished");
        EXPECT_TRUE(result);
    }
}

/**
 * @tc.number   Telephony_SmsMmsGtest_SetDefaultSmsSlotId_0002
 * @tc.name     Set Default Sms SlotId slotId is valid
 * @tc.desc     Function test
 */
HWTEST_F(SmsMmsGtest, SetDefaultSmsSlotId_0002, Function | MediumTest | Level3)
{
    bool result = false;
    int32_t slotId = 1;
    if ((g_telephonyService != nullptr) && SmsMmsGtest::HasSimCard(slotId)) {
        TELEPHONY_LOGI("TelSMSMMSTest::SetDefaultSmsSlotId_0002 -->");
        result = g_telephonyService->SetDefaultSmsSlotId(slotId);
        TELEPHONY_LOGI("TelSMSMMSTest::SetDefaultSmsSlotId_0002 -->finished");
        EXPECT_TRUE(result);
    }
}

/**
 * @tc.number   Telephony_SmsMmsGtest_SetDefaultSmsSlotId_0003
 * @tc.name     Set Default Sms SlotId slotId is 2
 * @tc.desc     Function test
 */
HWTEST_F(SmsMmsGtest, SetDefaultSmsSlotId_0003, Function | MediumTest | Level3)
{
    bool result = false;
    int32_t slotId = 2;
    if ((g_telephonyService != nullptr) && SmsMmsGtest::HasSimCard(slotId)) {
        TELEPHONY_LOGI("TelSMSMMSTest::SetDefaultSmsSlotId_0003 -->");
        result = g_telephonyService->SetDefaultSmsSlotId(slotId);
        TELEPHONY_LOGI("TelSMSMMSTest::SetDefaultSmsSlotId_0003 -->finished");
        EXPECT_TRUE(result);
    }
}

/**
 * @tc.number   Telephony_SmsMmsGtest_SetDefaultSmsSlotId_0004
 * @tc.name     Set Default Sms SlotId slotId is invalid
 * @tc.desc     Function test
 */
HWTEST_F(SmsMmsGtest, SetDefaultSmsSlotId_0004, Function | MediumTest | Level3)
{
    bool result = false;
    int32_t slotId = 3;
    if ((g_telephonyService != nullptr) && SmsMmsGtest::HasSimCard(slotId)) {
        TELEPHONY_LOGI("TelSMSMMSTest::SetDefaultSmsSlotId_0004 -->");
        result = g_telephonyService->SetDefaultSmsSlotId(slotId);
        TELEPHONY_LOGI("TelSMSMMSTest::SetDefaultSmsSlotId_0004 -->finished");
        EXPECT_TRUE(result);
    }
}

/**
 * @tc.number   Telephony_SmsMmsGtest_SetDefaultSmsSlotId_0005
 * @tc.name     Set Default Sms SlotId slotId is invalid
 * @tc.desc     Function test
 */
HWTEST_F(SmsMmsGtest, SetDefaultSmsSlotId_0005, Function | MediumTest | Level3)
{
    bool result = false;
    int32_t slotId = 4;
    if ((g_telephonyService != nullptr) && SmsMmsGtest::HasSimCard(slotId)) {
        TELEPHONY_LOGI("TelSMSMMSTest::SetDefaultSmsSlotId_0005 -->");
        result = g_telephonyService->SetDefaultSmsSlotId(slotId);
        TELEPHONY_LOGI("TelSMSMMSTest::SetDefaultSmsSlotId_0005 -->finished");
        EXPECT_TRUE(result);
    }
}

/**
 * @tc.number   Telephony_SmsMmsGtest_GetDefaultSmsSlotId_0001
 * @tc.name     Get Default Sms SlotId
 * @tc.desc     Function test
 */
HWTEST_F(SmsMmsGtest, GetDefaultSmsSlotId_0001, Function | MediumTest | Level3)
{
    const int32_t error = -1;
    int32_t slotId = DEFAULT_SIM_SLOT_ID;
    if ((g_telephonyService != nullptr) && SmsMmsGtest::HasSimCard(slotId)) {
        TELEPHONY_LOGI("TelSMSMMSTest::GetDefaultSmsSlotId_0001 -->");
        slotId = g_telephonyService->GetDefaultSmsSlotId();
        TELEPHONY_LOGI("TelSMSMMSTest::GetDefaultSmsSlotId_0001 -->finished");
        EXPECT_TRUE(slotId != error);
    }
}

/**
 * @tc.number   Telephony_SmsMmsGtest_SetSmscAddr_0001
 * @tc.name     Set smsc addr slotId is invalid
 * @tc.desc     Function test
 */
HWTEST_F(SmsMmsGtest, SetSmscAddr_0001, Function | MediumTest | Level3)
{
    bool result = true;
    if (g_telephonyService != nullptr) {
        int32_t slotId = -1;
        // invalid slotID scenario, a invalid smsc addr is OKAY
        std::string scAddr("1234");
        TELEPHONY_LOGI("TelSMSMMSTest::SetSmscAddr_0001 -->");
        result = g_telephonyService->SetSmscAddr(slotId, StringUtils::ToUtf16(scAddr));
        TELEPHONY_LOGI("TelSMSMMSTest::SetSmscAddr_0001 -->finished");
        EXPECT_FALSE(result);
    }
}

/**
 * @tc.number   Telephony_SmsMmsGtest_AddSimMessage_0001
 * @tc.name     Add Sim Message
 * @tc.desc     Function test
 */
HWTEST_F(SmsMmsGtest, AddSimMessage_0001, Function | MediumTest | Level3)
{
    bool result = false;
    if (g_telephonyService != nullptr) {
        int32_t slotId = DEFAULT_SIM_SLOT_ID;
        std::u16string smscData(u"");
        std::u16string pduData(u"01000B818176251308F4000007E8B0BCFD76E701");
        uint32_t status = 3;
        TELEPHONY_LOGI("TelSMSMMSTest::AddSimMessage_0001 -->");
        result = g_telephonyService->AddSimMessage(slotId, smscData, pduData,
        static_cast<ISmsServiceInterface::SimMessageStatus>(status));
        TELEPHONY_LOGI("TelSMSMMSTest::AddSimMessage_0001 -->finished");
        EXPECT_TRUE(result);
    }
}

/**
 * @tc.number   Telephony_SmsMmsGtest_GetAllSimMessages_0001
 * @tc.name     Get All Sim Messages
 * @tc.desc     Function test
 */
HWTEST_F(SmsMmsGtest, GetAllSimMessages_0001, Function | MediumTest | Level3)
{
    std::vector<ShortMessage> result;
    if (g_telephonyService != nullptr) {
        int32_t slotId = DEFAULT_SIM_SLOT_ID;
        TELEPHONY_LOGI("TelSMSMMSTest::GetAllSimMessages_0001 -->");
        result = g_telephonyService->GetAllSimMessages(slotId);
        TELEPHONY_LOGI("TelSMSMMSTest::GetAllSimMessages_0001 -->finished");
        EXPECT_TRUE(true);
    }
}

/**
 * @tc.number   Telephony_SmsMmsGtest_DelSimMessage_0001
 * @tc.name     Del Sim Message
 * @tc.desc     Function test
 */
HWTEST_F(SmsMmsGtest, DelSimMessage_0001, Function | MediumTest | Level3)
{
    bool result = false;
    if (g_telephonyService != nullptr) {
        int32_t slotId = DEFAULT_SIM_SLOT_ID;
        uint32_t msgIndex = 0;
        TELEPHONY_LOGI("TelSMSMMSTest::DelSimMessage_0001 -->");
        result = g_telephonyService->DelSimMessage(slotId, msgIndex);
        TELEPHONY_LOGI("TelSMSMMSTest::DelSimMessage_0001 -->finished");
        EXPECT_TRUE(result);
    }
}

/**
 * @tc.number   Telephony_SmsMmsGtest_UpdateSimMessage_0001
 * @tc.name     Update Sim Message
 * @tc.desc     Function test
 */
HWTEST_F(SmsMmsGtest, UpdateSimMessage_0001, Function | MediumTest | Level3)
{
    bool result = false;
    if (g_telephonyService != nullptr) {
        int32_t slotId = DEFAULT_SIM_SLOT_ID;
        uint32_t msgIndex = 0;
        std::u16string smscData(u"");
        std::u16string pduData(u"01000B818176251308F4000007E8B0BCFD76E701");
        uint32_t status = 3;
        TELEPHONY_LOGI("TelSMSMMSTest::UpdateSimMessage_0001 -->");
        result = g_telephonyService->UpdateSimMessage(
        slotId, msgIndex, static_cast<ISmsServiceInterface::SimMessageStatus>(status), pduData, smscData);
        TELEPHONY_LOGI("TelSMSMMSTest::UpdateSimMessage_0001 -->finished");
        EXPECT_TRUE(result);
    }
}

/**
 * @tc.number   Telephony_SmsMmsGtest_SetImsSmsConfig_0001
 * @tc.name     Enable IMS SMS
 * @tc.desc     Function test\
 * @tc.require: issueI5K12U
 */
HWTEST_F(SmsMmsGtest, SetImsSmsConfig_0001, Function | MediumTest | Level3)
{
    bool result = false;
    if (g_telephonyService == nullptr) {
        EXPECT_TRUE(result);
    }
    int32_t slotId = DEFAULT_SIM_SLOT_ID;
    TELEPHONY_LOGI("TelSMSMMSTest::SetImsSmsConfig_0001 -->");
    result = g_telephonyService->SetImsSmsConfig(slotId, 1);
    TELEPHONY_LOGI("TelSMSMMSTest::SetImsSmsConfig_0001 -->finished");
    EXPECT_TRUE(result);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_IsImsSmsSupported_0001
 * @tc.name     Get Ims Sms Config
 * @tc.desc     Function test
 * @tc.require: issueI5K12U
 */
HWTEST_F(SmsMmsGtest, IsImsSmsSupported_0001, Function | MediumTest | Level3)
{
    bool result = false;
    if (g_telephonyService == nullptr) {
        EXPECT_TRUE(result);
    }
    int32_t slotId = DEFAULT_SIM_SLOT_ID;
    TELEPHONY_LOGI("TelSMSMMSTest::IsImsSmsSupported_0001 -->");
    result = g_telephonyService->IsImsSmsSupported(slotId);
    TELEPHONY_LOGI("TelSMSMMSTest::IsImsSmsSupported_0001 -->finished");
    EXPECT_TRUE(result);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_SetImsSmsConfig_0002
 * @tc.name     Disable Ims Sms
 * @tc.desc     Function test
 * @tc.require: issueI5K12U
 */
HWTEST_F(SmsMmsGtest, SetImsSmsConfig_0002, Function | MediumTest | Level3)
{
    bool result = false;
    if (g_telephonyService == nullptr) {
        EXPECT_TRUE(result);
    }
    int32_t slotId = DEFAULT_SIM_SLOT_ID;
    TELEPHONY_LOGI("TelSMSMMSTest::SetImsSmsConfig_0002 -->");
    result = g_telephonyService->SetImsSmsConfig(slotId, 0);
    TELEPHONY_LOGI("TelSMSMMSTest::SetImsSmsConfig_0002 -->finished");
    EXPECT_TRUE(result);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_IsImsSmsSupported_0002
 * @tc.name     Get Ims Sms Config
 * @tc.desc     Function test
 * @tc.require: issueI5K12U
 */
HWTEST_F(SmsMmsGtest, IsImsSmsSupported_0002, Function | MediumTest | Level3)
{
    bool result = false;
    if (g_telephonyService == nullptr) {
        EXPECT_TRUE(result);
    }
    int32_t slotId = DEFAULT_SIM_SLOT_ID;
    TELEPHONY_LOGI("TelSMSMMSTest::IsImsSmsSupported_0002 -->");
    result = g_telephonyService->IsImsSmsSupported(slotId);
    TELEPHONY_LOGI("TelSMSMMSTest::IsImsSmsSupported_0002 -->finished");
    EXPECT_FALSE(result);
}

#else // TEL_TEST_UNSUPPORT
/**
 * @tc.number   Telephony_SmsMms_MockTest_0001
 * @tc.name     Mock test for unsupported platform
 * @tc.desc     Function test
 */
HWTEST_F(SmsMmsGtest, MockTest_0001, Function | MediumTest | Level3)
{
    EXPECT_TRUE(true);
}
#endif // TEL_TEST_UNSUPPORT
} // namespace Telephony
} // namespace OHOS
#endif
