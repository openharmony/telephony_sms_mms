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
    g_telephonyService = SmsMmsGtest::GetProxy();
    EXPECT_FALSE(g_telephonyService == nullptr);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_OpenCellBroadcast_0001
 * @tc.name     Open cellBroadcast slotId is -1
 * @tc.desc     Function test
 */
HWTEST_F(SmsMmsGtest, OpenCellBroadcast_0001, Function | MediumTest | Level1)
{
    bool result = false;
    if (g_telephonyService != nullptr) {
        int32_t slotId = -1;
        bool enable = true;
        uint32_t fromMsgId = 0;
        uint32_t toMsgId = 10;
        uint8_t netType = 1;
        result = g_telephonyService->SetCBConfig(slotId, enable, fromMsgId, toMsgId, netType);
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
    bool result = false;
    if (g_telephonyService != nullptr) {
        int32_t slotId = DEFAULT_SIM_SLOT_ID;
        bool enable = true;
        uint32_t fromMsgId = 20;
        uint32_t toMsgId = 10;
        uint8_t netType = 1;
        result = g_telephonyService->SetCBConfig(slotId, enable, fromMsgId, toMsgId, netType);
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
    bool result = false;
    if (g_telephonyService != nullptr) {
        int32_t slotId = DEFAULT_SIM_SLOT_ID;
        bool enable = true;
        uint32_t fromMsgId = 0;
        uint32_t toMsgId = 10;
        uint8_t netType = 3;
        result = g_telephonyService->SetCBConfig(slotId, enable, fromMsgId, toMsgId, netType);
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
        result = g_telephonyService->SetCBConfig(slotId, enable, fromMsgId, toMsgId, netType);
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
        result = g_telephonyService->SetCBConfig(slotId, enable, fromMsgId, toMsgId, netType);
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
        result = g_telephonyService->SetCBConfig(slotId, enable, fromMsgId, toMsgId, netType);
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
    bool result = false;
    if (g_telephonyService != nullptr) {
        int32_t slotId = -1;
        bool enable = false;
        uint32_t fromMsgId = 0;
        uint32_t toMsgId = 10;
        uint8_t netType = 1;
        result = g_telephonyService->SetCBConfig(slotId, enable, fromMsgId, toMsgId, netType);
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
    bool result = false;
    if (g_telephonyService != nullptr) {
        int32_t slotId = DEFAULT_SIM_SLOT_ID;
        bool enable = false;
        uint32_t fromMsgId = 20;
        uint32_t toMsgId = 10;
        uint8_t netType = 1;
        result = g_telephonyService->SetCBConfig(slotId, enable, fromMsgId, toMsgId, netType);
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
    bool result = false;
    if (g_telephonyService != nullptr) {
        int32_t slotId = DEFAULT_SIM_SLOT_ID;
        bool enable = false;
        uint32_t fromMsgId = 0;
        uint32_t toMsgId = 10;
        uint8_t netType = 3;
        result = g_telephonyService->SetCBConfig(slotId, enable, fromMsgId, toMsgId, netType);
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
        result = g_telephonyService->SetCBConfig(slotId, enable, fromMsgId, toMsgId, netType);
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
        result = g_telephonyService->SetCBConfig(slotId, enable, fromMsgId, toMsgId, netType);
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
        result = g_telephonyService->SetCBConfig(slotId, enable, fromMsgId, toMsgId, netType);
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
        result = g_telephonyService->SetDefaultSmsSlotId(slotId);
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
        result = g_telephonyService->SetDefaultSmsSlotId(slotId);
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
        result = g_telephonyService->SetDefaultSmsSlotId(slotId);
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
        result = g_telephonyService->SetDefaultSmsSlotId(slotId);
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
        result = g_telephonyService->SetDefaultSmsSlotId(slotId);
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
        slotId = g_telephonyService->GetDefaultSmsSlotId();
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
    bool result = false;
    if (g_telephonyService != nullptr) {
        int32_t slotId = -1;
        // invalid slotID scenario, a invalid smsc addr is OKAY
        std::string scAddr("1234");
        result = g_telephonyService->SetSmscAddr(slotId, StringUtils::ToUtf16(scAddr));
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
        result = g_telephonyService->AddSimMessage(slotId, smscData, pduData,
        static_cast<ISmsServiceInterface::SimMessageStatus>(status));
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
        result = g_telephonyService->GetAllSimMessages(slotId);
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
        result = g_telephonyService->DelSimMessage(slotId, msgIndex);
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
        result = g_telephonyService->UpdateSimMessage(
        slotId, msgIndex, static_cast<ISmsServiceInterface::SimMessageStatus>(status), pduData, smscData);
        EXPECT_TRUE(result);
    }
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
