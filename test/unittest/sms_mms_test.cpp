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

#include <memory>
#include <vector>

#include "common_event_support.h"
#include "gsm_sms_sender_test.h"
#include "if_system_ability_manager.h"
#include "ipc_skeleton.h"
#include "iservice_registry.h"
#include "mms_msg_test.h"
#include "short_message_test.h"
#include "sms_broadcast_subscriber.h"
#include "sms_cb_message_test.h"

using namespace OHOS;
using namespace OHOS::Telephony;
using namespace OHOS::EventFwk;

using TestStruct = struct FunStruct {
    std::string funName;
    std::function<void(void)> fun = nullptr;
    FunStruct(const std::string &name, const std::function<void(void)> &function) : funName(name), fun(function) {}
};

using TestParam = struct Param {
    GsmSmsSenderTest gsmSmsSenderTest;
    ShortMessageTest shortMessageTest;
    SmsCbMessageTest smsCbMessageTest;
    MmsMsgTest mmsMsgTest;
};

namespace OHOS {
namespace Telephony {
class SmsMmsTest {
public:
    SmsMmsTest() = default;
    ~SmsMmsTest() = default;

    void TestRecev()
    {
        EventFwk::MatchingSkills matchingSkills;
        matchingSkills.AddEvent(EventFwk::CommonEventSupport::COMMON_EVENT_SMS_RECEIVE_COMPLETED);
        matchingSkills.AddEvent(EventFwk::CommonEventSupport::COMMON_EVENT_SMS_EMERGENCY_CB_RECEIVE_COMPLETED);
        matchingSkills.AddEvent(EventFwk::CommonEventSupport::COMMON_EVENT_SMS_CB_RECEIVE_COMPLETED);
        matchingSkills.AddEvent(EventFwk::CommonEventSupport::COMMON_EVENT_SMS_WAPPUSH_RECEIVE_COMPLETED);
        // make subcriber info
        EventFwk::CommonEventSubscribeInfo subscriberInfo(matchingSkills);
        // make a subcriber object
        std::shared_ptr<SmsBroadcastSubscriber> subscriberTest =
            std::make_shared<SmsBroadcastSubscriber>(subscriberInfo);
        if (subscriberTest == nullptr) {
            return;
        }
        // subscribe a common event
        bool subscribeResult = EventFwk::CommonEventManager::SubscribeCommonEvent(subscriberTest);
        std::cout << "subscribeResult is " << (subscribeResult ? "true" : "false") << std::endl;
    }

    void InitGsmFun(const std::unique_ptr<std::vector<TestStruct>> &funArray, const TestParam &param,
        const sptr<ISmsServiceInterface> &smsService)
    {
        funArray->emplace_back(
            "TestSendShortText", std::bind(&GsmSmsSenderTest::TestSendShortText, param.gsmSmsSenderTest, smsService));
        funArray->emplace_back("TestGsmSendShortData",
            std::bind(&GsmSmsSenderTest::TestGsmSendShortData, param.gsmSmsSenderTest, smsService));
        funArray->emplace_back(
            "TestSendLongText", std::bind(&GsmSmsSenderTest::TestSendLongText, param.gsmSmsSenderTest, smsService));
        funArray->emplace_back("TestGsmSendLongData",
            std::bind(&GsmSmsSenderTest::TestGsmSendLongData, param.gsmSmsSenderTest, smsService));
        funArray->emplace_back(
            "TestSetSmscAddr", std::bind(&GsmSmsSenderTest::TestSetSmscAddr, param.gsmSmsSenderTest, smsService));
        funArray->emplace_back(
            "TestGetSmscAddr", std::bind(&GsmSmsSenderTest::TestGetSmscAddr, param.gsmSmsSenderTest, smsService));
        funArray->emplace_back("TestCbMessage", std::bind(&SmsCbMessageTest::ProcessTest, param.smsCbMessageTest));
        funArray->emplace_back("TestEnableCBRangeConfig",
            std::bind(&GsmSmsSenderTest::TestEnableCBRangeConfig, param.gsmSmsSenderTest, smsService));
        funArray->emplace_back("TestDisableCBRangeConfig",
            std::bind(&GsmSmsSenderTest::TestDisableCBRangeConfig, param.gsmSmsSenderTest, smsService));
        funArray->emplace_back(
            "TestEnableCBConfig", std::bind(&GsmSmsSenderTest::TestEnableCBConfig, param.gsmSmsSenderTest, smsService));
        funArray->emplace_back("TestDisableCBConfig",
            std::bind(&GsmSmsSenderTest::TestDisableCBConfig, param.gsmSmsSenderTest, smsService));
        funArray->emplace_back("TestSetDefaultSmsSlotId",
            std::bind(&GsmSmsSenderTest::TestSetDefaultSmsSlotId, param.gsmSmsSenderTest, smsService));
        funArray->emplace_back("TestGetDefaultSmsSlotId",
            std::bind(&GsmSmsSenderTest::TestGetDefaultSmsSlotId, param.gsmSmsSenderTest, smsService));
        funArray->emplace_back(
            "TestSplitMessage", std::bind(&GsmSmsSenderTest::TestSplitMessage, param.gsmSmsSenderTest, smsService));
        funArray->emplace_back("TestGetSmsSegmentsInfo",
            std::bind(&GsmSmsSenderTest::TestGetSmsSegmentsInfo, param.gsmSmsSenderTest, smsService));
        funArray->emplace_back("TestIsImsSmsSupported",
            std::bind(&GsmSmsSenderTest::TestIsImsSmsSupported, param.gsmSmsSenderTest, smsService));
        funArray->emplace_back("TestSetImsSmsConfig",
            std::bind(&GsmSmsSenderTest::TestSetImsSmsConfig, param.gsmSmsSenderTest, smsService));
        funArray->emplace_back("TestGetImsShortMessageFormat",
            std::bind(&GsmSmsSenderTest::TestGetImsShortMessageFormat, param.gsmSmsSenderTest, smsService));
        funArray->emplace_back(
            "TestAddBlockPhone", std::bind(&GsmSmsSenderTest::TestAddBlockPhone, param.gsmSmsSenderTest));
        funArray->emplace_back(
            "TestRemoveBlockPhone", std::bind(&GsmSmsSenderTest::TestRemoveBlockPhone, param.gsmSmsSenderTest));
        funArray->emplace_back("TestHasSmsCapability",
            std::bind(&GsmSmsSenderTest::TestHasSmsCapability, param.gsmSmsSenderTest, smsService));
    }

    void InitSimMessageFun(const std::unique_ptr<std::vector<TestStruct>> &funArray, const TestParam &param,
        const sptr<ISmsServiceInterface> &smsService)
    {
        funArray->emplace_back(
            "TestAddSimMessage", std::bind(&GsmSmsSenderTest::TestAddSimMessage, param.gsmSmsSenderTest, smsService));
        funArray->emplace_back(
            "TestDelSimMessage", std::bind(&GsmSmsSenderTest::TestDelSimMessage, param.gsmSmsSenderTest, smsService));
        funArray->emplace_back("TestUpdateSimMessage",
            std::bind(&GsmSmsSenderTest::TestUpdateSimMessage, param.gsmSmsSenderTest, smsService));
        funArray->emplace_back("TestGetAllSimMessages",
            std::bind(&GsmSmsSenderTest::TestGetAllSimMessages, param.gsmSmsSenderTest, smsService));
    }

    void InitShortMessageFun(const std::unique_ptr<std::vector<TestStruct>> &funArray, const TestParam &param,
        const sptr<ISmsServiceInterface> &smsService)
    {
        funArray->emplace_back(
            "TestCreateMessage", std::bind(&ShortMessageTest::TestCreateMessage, param.shortMessageTest));
        funArray->emplace_back("TestGetVisibleMessageBody",
            std::bind(&ShortMessageTest::TestGetVisibleMessageBody, param.shortMessageTest));
        funArray->emplace_back(
            "TestShowShortMessage", std::bind(&ShortMessageTest::TestShowShortMessage, param.shortMessageTest));
        funArray->emplace_back(
            "TestCreate3Gpp2Message", std::bind(&ShortMessageTest::Test3Gpp2CreateMessage, param.shortMessageTest));
    }

    void InitMmsFun(const std::unique_ptr<std::vector<TestStruct>> &funArray, const TestParam &param,
        const sptr<ISmsServiceInterface> &smsService)
    {
        funArray->emplace_back("TestMmsMsgFunction", std::bind(&MmsMsgTest::ProcessTest, param.mmsMsgTest));
    }

    void InitFunArray(const std::unique_ptr<std::vector<TestStruct>> &funArray, const TestParam &param,
        const sptr<ISmsServiceInterface> &smsService)
    {
        InitGsmFun(funArray, param, smsService);
        InitSimMessageFun(funArray, param, smsService);
        InitShortMessageFun(funArray, param, smsService);
        InitMmsFun(funArray, param, smsService);
    }

    std::unique_ptr<std::vector<TestStruct>> GetFunArray(const sptr<ISmsServiceInterface> &smsService)
    {
        static TestParam param;
        param.shortMessageTest.TestCreateMessage();
        std::unique_ptr<std::vector<TestStruct>> funArray = std::make_unique<std::vector<TestStruct>>();
        if (smsService == nullptr || funArray == nullptr) {
            return funArray;
        }
        InitFunArray(funArray, param, smsService);
        return funArray;
    }
};
} // namespace Telephony
} // namespace OHOS

int main()
{
    OHOS::Telephony::SmsMmsTest smsMmsTest;
    smsMmsTest.TestRecev();
    sptr<ISystemAbilityManager> systemAbilityMgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    sptr<IRemoteObject> remote = nullptr;
    sptr<ISmsServiceInterface> smsService = nullptr;
    if ((systemAbilityMgr == nullptr) ||
        ((remote = systemAbilityMgr->CheckSystemAbility(TELEPHONY_SMS_MMS_SYS_ABILITY_ID)) == nullptr) ||
        ((smsService = iface_cast<ISmsServiceInterface>(remote)) == nullptr)) {
        std::cout << "connect to sms service failed." << std::endl;
        return 0;
    }
    std::unique_ptr<std::vector<TestStruct>> testFunArray = smsMmsTest.GetFunArray(smsService);
    int8_t caseCount = 0;
    if (testFunArray == nullptr || ((caseCount = testFunArray->size()) <= 0)) {
        std::cout << "Failed to get testFunArray data!\n";
        return 0;
    }
    std::string hint = "[-1]:Exit\n";
    for (int index = 0; index < caseCount; ++index) {
        hint += "[" + std::to_string(index) + "]:" + (*testFunArray)[index].funName + "\n";
    }
    while (smsService != nullptr) {
        std::cout << hint;
        std::cout << "Please input test case number!" << std::endl;
        std::string input;
        int caseNumber = 0;
        std::cin >> input;
        caseNumber = std::atoi(input.c_str());
        std::cin.clear();
        std::cin.ignore();
        std::cin.sync();
        if (caseNumber < -1 || caseNumber >= caseCount) {
            std::cout << "test case is not exist!" << std::endl;
            continue;
        }
        if (caseNumber == -1) {
            break;
        }
        std::cout << "Enter the " << (*testFunArray)[caseNumber].funName << " case!" << std::endl;
        if ((*testFunArray)[caseNumber].fun != nullptr) {
            (*testFunArray)[caseNumber].fun();
        }
    }
    return 0;
}