/*
 * Copyright (C) 2023-2024 Huawei Device Co., Ltd.
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
#include "cdma_sms_receive_handler.h"
#include "cdma_sms_message.h"
#include "cdma_sms_transport_message.h"
#include "core_service_client.h"
#include "gtest/gtest.h"
#include "i_sms_service_interface.h"
#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "radio_event.h"
#include "sms_mms_gtest.h"
#include "sms_mms_test_helper.h"
#include "sms_service.h"
#include "sms_service_manager_client.h"
#include "string_utils.h"
#include "telephony_log_wrapper.h"
#include "sms_cb_data.h"
#undef private
#undef protected

#define INVALID_SLOTID (-1)
namespace OHOS {
namespace Telephony {
namespace {
sptr<ISmsServiceInterface> g_telephonyService = nullptr;
} // namespace
using namespace testing::ext;

class MMSCdmaReceiverHandlerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void MMSCdmaReceiverHandlerTest::TearDownTestCase() {}

void MMSCdmaReceiverHandlerTest::SetUp() {}

void MMSCdmaReceiverHandlerTest::TearDown() {}

void MMSCdmaReceiverHandlerTest::SetUpTestCase() {}

/**
 * @tc.number   CdmaSmsMessage_001
 * @tc.name     Test HandleAck
 * @tc.desc     Function test
 */
HWTEST_F(MMSCdmaReceiverHandlerTest, CdmaSmsMessage_001, Function | MediumTest | Level1)
{
    std::shared_ptr<CdmaSmsReceiveHandler> smsReceiveHandler = std::make_shared<CdmaSmsReceiveHandler>(INVALID_SLOTID);
    smsReceiveHandler->Init();
    std::string pdu = StringUtils::HexToString("16D131D98C56B3DD7039584C36A3D56C375C0E169301");
    std::shared_ptr<SmsBaseMessage> smsBaseMessage = nullptr;

    EXPECT_EQ(AckIncomeCause::SMS_ACK_UNKNOWN_ERROR, smsReceiveHandler->HandleAck(smsBaseMessage));
    smsBaseMessage = GsmSmsMessage::CreateMessage(pdu);
    EXPECT_EQ(nullptr, smsBaseMessage);
}

/**
 * @tc.number   CdmaSmsMessage_002
 * @tc.name     Test HandleRemainDataShare
 * @tc.desc     Function test
 */
HWTEST_F(MMSCdmaReceiverHandlerTest, CdmaSmsMessage_002, Function | MediumTest | Level1)
{
    std::shared_ptr<CdmaSmsReceiveHandler> smsReceiveHandler = std::make_shared<CdmaSmsReceiveHandler>(INVALID_SLOTID);
    smsReceiveHandler->Init();
    std::string pdu = StringUtils::HexToString("16D131D98C56B3DD7039584C36A3D56C375C0E169301");
    std::shared_ptr<SmsBaseMessage> smsBaseMessage = nullptr;
    smsReceiveHandler->HandleRemainDataShare(smsBaseMessage);

    smsBaseMessage = GsmSmsMessage::CreateMessage(pdu);
    EXPECT_EQ(nullptr, smsBaseMessage);
}

/**
 * @tc.number   CdmaSmsMessage_003
 * @tc.name     Test HandleRemainDataShare
 * @tc.desc     Function test
 */
HWTEST_F(MMSCdmaReceiverHandlerTest, CdmaSmsMessage_003, Function | MediumTest | Level1)
{
    std::shared_ptr<CdmaSmsReceiveHandler> smsReceiveHandler = std::make_shared<CdmaSmsReceiveHandler>(INVALID_SLOTID);
    smsReceiveHandler->Init();
    std::string pdu = "PDU";
    std::shared_ptr<SmsBaseMessage> smsBaseMessage = nullptr;
    smsReceiveHandler->HandleRemainDataShare(smsBaseMessage);

    smsBaseMessage = GsmSmsMessage::CreateMessage(pdu);
    EXPECT_EQ(nullptr, smsBaseMessage);
}

/**
 * @tc.number   CdmaSmsMessage_004
 * @tc.name     Test GetDB
 * @tc.desc     Function test
 */
HWTEST_F(MMSCdmaReceiverHandlerTest, CdmaSmsMessage_004, Function | MediumTest | Level1)
{
    std::shared_ptr<CdmaSmsReceiveHandler> smsReceiveHandler = std::make_shared<CdmaSmsReceiveHandler>(INVALID_SLOTID);
    smsReceiveHandler->Init();
    std::string pdu = "PDU";
    const std::shared_ptr<SmsBaseMessage> smsBaseMessage = GsmSmsMessage::CreateMessage(pdu);
    SmsCbData::CbData sendData;
    bool isEmergency = false;
    smsReceiveHandler->GetCBData(smsBaseMessage, sendData, isEmergency);
    EXPECT_FALSE(isEmergency);
}
} // namespace Telephony
} // namespace OHOS