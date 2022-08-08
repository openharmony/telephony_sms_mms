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

#include "gsm_sms_sender_test.h"

#include <iostream>

#include "data_ability_predicates.h"
#include "iservice_registry.h"
#include "values_bucket.h"

#include "ability_context.h"
#include "context_deal.h"
#include "ability_info.h"

#include "sms_delivery_callback_test.h"
#include "sms_send_callback_test.h"
#include "string_utils.h"

namespace OHOS {
namespace Telephony {
void GsmSmsSenderTest::TestGsmSendShortData(const sptr<ISmsServiceInterface> &smsService) const
{
    if (smsService == nullptr) {
        std::cout << "smsService is nullptr." << std::endl;
        return;
    }
    std::string dest;
    std::cout << "Please enter the card id" << std::endl;
    int32_t slotId;
    std::cin >> dest;
    slotId = atoi(dest.c_str());
    dest.clear();
    std::cout << "Please enter the receiver's telephone number" << std::endl;
    std::cin >> dest;
    std::string sca("");
    OHOS::sptr<SmsSendCallbackTest> sendCallBackPtr(new SmsSendCallbackTest());
    OHOS::sptr<SmsDeliveryCallbackTest> deliveryCallBackPtr(new SmsDeliveryCallbackTest());
    uint16_t port = 100;
    uint8_t data[] = "hello world";
    smsService->SendMessage(slotId, StringUtils::ToUtf16(dest), StringUtils::ToUtf16(sca), port, data,
        (sizeof(data) / sizeof(data[0]) - 1), sendCallBackPtr, deliveryCallBackPtr);
    std::cout << "TestGsmSendShortData" << std::endl;
}

void GsmSmsSenderTest::TestGsmSendLongData(const sptr<ISmsServiceInterface> &smsService) const
{
    if (smsService == nullptr) {
        std::cout << "smsService is nullptr." << std::endl;
        return;
    }
    std::string dest;
    std::cout << "Please enter the card id" << std::endl;
    int32_t slotId;
    std::cin >> dest;
    slotId = atoi(dest.c_str());
    dest.clear();
    std::cout << "Please enter the receiver's telephone number" << std::endl;
    std::cin >> dest;
    std::string sca("");
    OHOS::sptr<SmsSendCallbackTest> sendCallBackPtr(new SmsSendCallbackTest());
    OHOS::sptr<SmsDeliveryCallbackTest> deliveryCallBackPtr(new SmsDeliveryCallbackTest());
    uint8_t data[] =
        R"(This is the test data!This is the test data!This is the test data!
        This is the test data!This is the test data!This is the test data!
        This is the test data!This is the test data!This is the test data!
        This is the test data!This is the test data!This is the test data!
        This is the test data!This is the test data!This is the test data!
        This is the test data!This is the test data!This is the test data!)";
    uint16_t port = 100;

    smsService->SendMessage(slotId, StringUtils::ToUtf16(dest), StringUtils::ToUtf16(sca), port, data,
        sizeof(data) / sizeof(data[0]), sendCallBackPtr, deliveryCallBackPtr);
    std::cout << "TestGsmSendLongData" << std::endl;
}

void GsmSmsSenderTest::TestSendShortText(const sptr<ISmsServiceInterface> &smsService) const
{
    if (!RequestPermissions()) {
        std::cout << "RequestPermissions failed." << std::endl;
        return;
    }
    if (smsService == nullptr) {
        std::cout << "smsService is nullptr." << std::endl;
        return;
    }
    std::string dest;
    std::cout << "Please enter the card id" << std::endl;
    int32_t slotId;
    std::cin >> dest;
    slotId = atoi(dest.c_str());
    dest.clear();
    std::string text;
    std::cin.ignore();
    std::cout << "Please enter the receiver's telephone number" << std::endl;
    getline(std::cin, dest);
    std::cout << "Please enter text" << std::endl;
    getline(std::cin, text);
    std::string sca("");
    OHOS::sptr<SmsSendCallbackTest> sendCallBackPtr(new SmsSendCallbackTest());
    OHOS::sptr<SmsDeliveryCallbackTest> deliveryCallBackPtr(new SmsDeliveryCallbackTest());
    smsService->SendMessage(slotId, StringUtils::ToUtf16(dest), StringUtils::ToUtf16(sca),
        StringUtils::ToUtf16(text), sendCallBackPtr, deliveryCallBackPtr);
    std::cout << "TestGsmSendShortText" << std::endl;
}

void GsmSmsSenderTest::TestSendLongText(const sptr<ISmsServiceInterface> &smsService) const
{
    if (smsService == nullptr) {
        std::cout << "smsService is nullptr." << std::endl;
        return;
    }
    std::string dest;
    std::cout << "Please enter the card id" << std::endl;
    int32_t slotId;
    std::cin >> dest;
    slotId = atoi(dest.c_str());
    dest.clear();
    std::cout << "Please enter the receiver's telephone number" << std::endl;
    std::cin >> dest;
    std::string sca("");
    std::string text(
        R"(This is the test data!This is the test data!This is the test data!
        This is the test data!This is the test data!This is the test data!
        This is the test data!This is the test data!This is the test data!
        This is the test data!This is the test data!This is the test data!
        This is the test data!This is the test data!This is the test data!
        This is the test data!This is the test data!This is the test data!)");
    OHOS::sptr<SmsSendCallbackTest> sendCallBackPtr(new SmsSendCallbackTest());
    OHOS::sptr<SmsDeliveryCallbackTest> deliveryCallBackPtr(new SmsDeliveryCallbackTest());
    smsService->SendMessage(slotId, StringUtils::ToUtf16(dest), StringUtils::ToUtf16(sca),
        StringUtils::ToUtf16(text), sendCallBackPtr, deliveryCallBackPtr);
    std::cout << "TestGsmSendLongText" << std::endl;
}

void GsmSmsSenderTest::TestSetSmscAddr(const sptr<ISmsServiceInterface> &smsService) const
{
    bool result = false;
    if (smsService == nullptr) {
        std::cout << "smsService is nullptr." << std::endl;
        return;
    }
    std::string dest;
    std::cout << "Please enter the card id" << std::endl;
    int32_t slotId;
    std::cin >> dest;
    slotId = atoi(dest.c_str());
    dest.clear();
    std::string sca;
    std::cout << "Please enter smsc" << std::endl;
    std::cin >> sca;
    result = smsService->SetSmscAddr(slotId, StringUtils::ToUtf16(sca));
    if (result) {
        std::cout << "TestSetSmscAddr OK!" << std::endl;
    } else {
        std::cout << "TestSetSmscAddr failed!" << std::endl;
    }
}

void GsmSmsSenderTest::TestGetSmscAddr(const sptr<ISmsServiceInterface> &smsService) const
{
    std::string result;
    if (smsService == nullptr) {
        std::cout << "smsService is nullptr." << std::endl;
        return;
    }
    std::string dest;
    std::cout << "Please enter the card id" << std::endl;
    int32_t slotId;
    std::cin >> dest;
    slotId = atoi(dest.c_str());
    dest.clear();
    result = StringUtils::ToUtf8(smsService->GetSmscAddr(slotId));
    std::cout << "TestGetSmscAddr:" << result << std::endl;
}

void GsmSmsSenderTest::TestAddSimMessage(const sptr<ISmsServiceInterface> &smsService) const
{
    bool result = false;
    if (smsService == nullptr) {
        std::cout << "smsService is nullptr." << std::endl;
        return;
    }
    std::string dest;
    std::cout << "Please enter the card id" << std::endl;
    int32_t slotId;
    std::cin >> dest;
    slotId = atoi(dest.c_str());
    dest.clear();
    std::string smsc;
    std::cout << "Please enter smsc" << std::endl;
    std::cin >> smsc;
    std::string pdu;
    std::cout << "Please enter pdu" << std::endl;
    std::cin >> pdu;
    std::string input;
    std::cout << "Please enter status" << std::endl;
    std::cin >> input;
    uint32_t status = std::atoi(input.c_str());
    result = smsService->AddSimMessage(slotId, StringUtils::ToUtf16(smsc), StringUtils::ToUtf16(pdu),
        static_cast<ISmsServiceInterface::SimMessageStatus>(status));
    if (result) {
        std::cout << "TestAddSimMessage OK!" << std::endl;
    } else {
        std::cout << "TestAddSimMessage failed!" << std::endl;
    }
}

void GsmSmsSenderTest::TestDelSimMessage(const sptr<ISmsServiceInterface> &smsService) const
{
    bool result = false;
    if (smsService == nullptr) {
        std::cout << "smsService is nullptr." << std::endl;
        return;
    }
    std::string dest;
    std::cout << "Please enter the card id" << std::endl;
    int32_t slotId;
    std::cin >> dest;
    slotId = atoi(dest.c_str());
    dest.clear();
    std::string input;
    std::cout << "Please enter msgIndex" << std::endl;
    std::cin >> input;
    uint32_t msgIndex = std::atoi(input.c_str());
    result = smsService->DelSimMessage(slotId, msgIndex);
    if (result) {
        std::cout << "TestDelSimMessage OK!" << std::endl;
    } else {
        std::cout << "TestDelSimMessage failed!" << std::endl;
    }
}

void GsmSmsSenderTest::TestUpdateSimMessage(const sptr<ISmsServiceInterface> &smsService) const
{
    bool result = false;
    if (smsService == nullptr) {
        std::cout << "smsService is nullptr." << std::endl;
        return;
    }
    std::string dest;
    std::cout << "Please enter the card id" << std::endl;
    int32_t slotId;
    std::cin >> dest;
    slotId = atoi(dest.c_str());
    dest.clear();
    std::string input;
    std::cout << "Please enter msgIndex" << std::endl;
    std::cin >> input;
    uint32_t msgIndex = std::atoi(input.c_str());
    std::string pdu;
    std::cout << "Please enter pdu" << std::endl;
    std::cin >> pdu;
    std::string smsc;
    std::cout << "Please enter smsc" << std::endl;
    std::cin >> smsc;
    int32_t status;
    std::cout << "Please enter status" << std::endl;
    std::cin >> status;
    std::u16string pduData = StringUtils::ToUtf16(pdu);
    std::u16string smscData = StringUtils::ToUtf16(smsc);
    result = smsService->UpdateSimMessage(
        slotId, msgIndex, static_cast<ISmsServiceInterface::SimMessageStatus>(status), pduData, smscData);
    if (result) {
        std::cout << "TestUpdateSimMessage OK!" << std::endl;
    } else {
        std::cout << "TestUpdateSimMessage failed!" << std::endl;
    }
}

void GsmSmsSenderTest::TestGetAllSimMessages(const sptr<ISmsServiceInterface> &smsService) const
{
    std::vector<ShortMessage> result;
    if (smsService == nullptr) {
        std::cout << "smsService is nullptr." << std::endl;
        return;
    }
    std::string dest;
    std::cout << "Please enter the card id" << std::endl;
    int32_t slotId;
    std::cin >> dest;
    slotId = atoi(dest.c_str());
    dest.clear();
    result = smsService->GetAllSimMessages(slotId);
    std::cout << "TestGetAllSimMessages Begin:" << std::endl;
    for (auto &item : result) {
        std::cout << "[" << item.GetIndexOnSim() << "] " << StringUtils::StringToHex(item.GetPdu()) << std::endl;
        std::cout << "status:" << item.GetIccMessageStatus()
                  << " message:" << StringUtils::ToUtf8(item.GetVisibleMessageBody()) << std::endl;
    }
    std::cout << "TestGetAllSimMessages End!" << std::endl;
}

void GsmSmsSenderTest::TestEnableCBRangeConfig(const sptr<ISmsServiceInterface> &smsService) const
{
    bool result = false;
    if (smsService == nullptr) {
        std::cout << "smsService is nullptr." << std::endl;
        return;
    }
    std::string dest;
    std::cout << "Please enter the card id" << std::endl;
    int32_t slotId;
    std::cin >> dest;
    slotId = atoi(dest.c_str());
    dest.clear();
    bool enable = true;
    uint32_t startMessageId = 0;
    uint32_t endMessageId = 0;
    uint8_t ranType = 1;
    std::string input;
    std::cout << "Please enter startMessageId" << std::endl;
    std::cin >> input;
    startMessageId = std::atoi(input.c_str());
    std::cout << "Please enter endMessageId" << std::endl;
    std::cin >> input;
    endMessageId = std::atoi(input.c_str());
    result = smsService->SetCBConfig(slotId, enable, startMessageId, endMessageId, ranType);
    std::string ret = result ? "true" : "false";
    std::cout << "TestEnableCBRangeConfig:" << ret << std::endl;
}

void GsmSmsSenderTest::TestDisableCBRangeConfig(const sptr<ISmsServiceInterface> &smsService) const
{
    bool result = false;
    if (smsService == nullptr) {
        std::cout << "smsService is nullptr." << std::endl;
        return;
    }
    std::string dest;
    std::cout << "Please enter the card id" << std::endl;
    int32_t slotId;
    std::cin >> dest;
    slotId = atoi(dest.c_str());
    dest.clear();
    bool enable = false;
    uint32_t startMessageId = 0;
    uint32_t endMessageId = 0;
    uint8_t ranType = 1;
    std::string input;
    std::cout << "Please enter startMessageId" << std::endl;
    std::cin >> input;
    startMessageId = std::atoi(input.c_str());
    std::cout << "Please enter endMessageId" << std::endl;
    std::cin >> input;
    endMessageId = std::atoi(input.c_str());
    result = smsService->SetCBConfig(slotId, enable, startMessageId, endMessageId, ranType);
    std::string ret = result ? "true" : "false";
    std::cout << "TestDisableCBRangeConfig:" << ret << std::endl;
}

void GsmSmsSenderTest::TestEnableCBConfig(const sptr<ISmsServiceInterface> &smsService) const
{
    bool result = false;
    if (smsService == nullptr) {
        std::cout << "smsService is nullptr." << std::endl;
        return;
    }
    std::string dest;
    std::cout << "Please enter the card id" << std::endl;
    int32_t slotId;
    std::cin >> dest;
    slotId = atoi(dest.c_str());
    dest.clear();
    bool enable = true;
    uint32_t identifier = 0;
    uint8_t ranType = 1;
    std::string input;
    std::cout << "Please enter identifier" << std::endl;
    std::cin >> input;
    identifier = std::atoi(input.c_str());
    result = smsService->SetCBConfig(slotId, enable, identifier, identifier, ranType);
    std::string ret = result ? "true" : "false";
    std::cout << "TestEnableCBConfig:" << ret << std::endl;
}

void GsmSmsSenderTest::TestDisableCBConfig(const sptr<ISmsServiceInterface> &smsService) const
{
    bool result = false;
    if (smsService == nullptr) {
        std::cout << "smsService is nullptr." << std::endl;
        return;
    }
    std::string dest;
    std::cout << "Please enter the card id" << std::endl;
    int32_t slotId;
    std::cin >> dest;
    slotId = atoi(dest.c_str());
    dest.clear();
    bool enable = false;
    uint32_t identifier = 0;
    uint8_t ranType = 1;
    std::string input;
    std::cout << "Please enter identifier" << std::endl;
    std::cin >> input;
    identifier = std::atoi(input.c_str());
    result = smsService->SetCBConfig(slotId, enable, identifier, identifier, ranType);
    std::string ret = result ? "true" : "false";
    std::cout << "TestDisableCBConfig:" << ret << std::endl;
}

void GsmSmsSenderTest::TestSetDefaultSmsSlotId(const sptr<ISmsServiceInterface> &smsService) const
{
    bool result = false;
    if (smsService == nullptr) {
        std::cout << "smsService is nullptr." << std::endl;
        return;
    }
    int32_t slotId;
    std::string input;
    std::cout << "Please enter Slot Id" << std::endl;
    std::cin >> input;
    slotId = std::atoi(input.c_str());
    result = smsService->SetDefaultSmsSlotId(slotId);
    std::string ret = result ? "true" : "false";
    std::cout << "TestSetDefaultSmsSlotId:" << ret << std::endl;
}

void GsmSmsSenderTest::TestGetDefaultSmsSlotId(const sptr<ISmsServiceInterface> &smsService) const
{
    int32_t result;
    if (smsService == nullptr) {
        std::cout << "smsService is nullptr." << std::endl;
        return;
    }
    result = smsService->GetDefaultSmsSlotId();
    std::cout << "TestGetDefaultSmsSlotId:" << result << std::endl;
}

void GsmSmsSenderTest::TestSplitMessage(const sptr<ISmsServiceInterface> &smsService) const
{
    std::vector<std::u16string> result;
    if (smsService == nullptr) {
        std::cout << "smsService is nullptr." << std::endl;
        return;
    }
    std::string input;
    std::cout << "Please enter message" << std::endl;
    std::getline(std::cin, input);
    result = smsService->SplitMessage(StringUtils::ToUtf16(input));
    std::cout << "TestSplitMessage size:" << result.size() << std::endl;
    for (auto &item : result) {
        std::cout << StringUtils::ToUtf8(item) << std::endl;
    }
}

void GsmSmsSenderTest::TestGetSmsSegmentsInfo(const sptr<ISmsServiceInterface> &smsService) const
{
    if (smsService == nullptr) {
        std::cout << "smsService is nullptr." << std::endl;
        return;
    }
    int32_t slotId;
    std::string input;
    std::cout << "Please enter Slot Id" << std::endl;
    std::getline(std::cin, input);
    slotId = std::atoi(input.c_str());
    input.clear();
    std::cout << "Please enter message" << std::endl;
    std::getline(std::cin, input);
    ISmsServiceInterface::SmsSegmentsInfo result;
    if (!smsService->GetSmsSegmentsInfo(slotId, StringUtils::ToUtf16(input), false, result)) {
        std::cout << "Get Sms SegmentsInfo Fail." << std::endl;
        return;
    }

    int32_t codeScheme = static_cast<int32_t>(result.msgCodeScheme);
    std::cout << "msgSegCount:" << result.msgSegCount << " msgEncodingCount:" << result.msgEncodingCount << std::endl;
    std::cout << "msgRemainCount:" << result.msgRemainCount << " msgCodeScheme:" << codeScheme << std::endl;
}

void GsmSmsSenderTest::TestIsImsSmsSupported(const sptr<ISmsServiceInterface> &smsService) const
{
    if (smsService == nullptr) {
        std::cout << "smsService is nullptr." << std::endl;
        return;
    }
    int32_t slotId;
    std::string input;
    std::cout << "Please enter Slot Id" << std::endl;
    std::getline(std::cin, input);
    slotId = std::atoi(input.c_str());
    std::string res = smsService->IsImsSmsSupported(slotId) ? "true" : "false";
    std::cout << "IsImsSmsSupported:" << res << std::endl;
}

void GsmSmsSenderTest::TestSetImsSmsConfig(const sptr<ISmsServiceInterface> &smsService) const
{
    if (smsService == nullptr) {
        std::cout << "smsService is nullptr." << std::endl;
        return;
    }
    int32_t slotId;
    std::string input;
    std::cout << "Please enter Slot Id" << std::endl;
    std::getline(std::cin, input);
    slotId = std::atoi(input.c_str());
    int32_t enable;
    std::cout << "Please enter enable" << std::endl;
    std::getline(std::cin, input);
    enable = std::atoi(input.c_str());
    std::string res = smsService->SetImsSmsConfig(slotId, enable) ? "true" : "false";
    std::cout << "SetImsSmsConfig:" << res << std::endl;
}

void GsmSmsSenderTest::TestGetImsShortMessageFormat(const sptr<ISmsServiceInterface> &smsService) const
{
    if (smsService == nullptr) {
        std::cout << "smsService is nullptr." << std::endl;
        return;
    }
    std::cout << "GetImsShortMessageFormat:" << StringUtils::ToUtf8(smsService->GetImsShortMessageFormat())
              << std::endl;
}

void GsmSmsSenderTest::TestAddBlockPhone() const
{
    std::shared_ptr<AppExecFwk::DataAbilityHelper> helper = CreateDataAHelper();
    if (helper == nullptr) {
        std::cout << "Creator helper nullptr error." << std::endl;
        return;
    }
    std::string input;
    std::cout << "Please enter block phone number" << std::endl;
    std::getline(std::cin, input);

    Uri uri("dataability:///com.ohos.contactsdataability/contacts/contact_blocklist");
    NativeRdb::ValuesBucket value;
    value.PutString("phone_number", input);
    int ret = helper->Insert(uri, value);
    helper->Release();
    std::cout << "add block:" << input << ((ret >= 0) ? " success" : " error") << std::endl;
}

void GsmSmsSenderTest::TestRemoveBlockPhone() const
{
    std::shared_ptr<AppExecFwk::DataAbilityHelper> helper = CreateDataAHelper();
    if (helper == nullptr) {
        std::cout << "Creator helper nullptr error." << std::endl;
        return;
    }
    std::string input;
    std::cout << "Please enter Remove phone number" << std::endl;
    std::getline(std::cin, input);
    Uri uri("dataability:///com.ohos.contactsdataability/contacts/contact_blocklist");
    NativeRdb::DataAbilityPredicates predicates;
    predicates.EqualTo("phone_number", input);
    int ret = helper->Delete(uri, predicates);
    helper->Release();
    std::cout << "remove block:" << input << ((ret >= 0) ? " success" : " error") << std::endl;
}

void GsmSmsSenderTest::TestHasSmsCapability(const sptr<ISmsServiceInterface> &smsService) const
{
    if (smsService == nullptr) {
        std::cout << "GsmSmsSenderTest smsService is nullptr error." << std::endl;
        return;
    }
    std::string res = smsService->HasSmsCapability() ? "true" : "false";
    std::cout << "HasSmsCapability:" << res << std::endl;
}

std::shared_ptr<AppExecFwk::DataAbilityHelper> GsmSmsSenderTest::CreateDataAHelper() const
{
    auto saManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (saManager == nullptr) {
        std::cout << "Get system ability mgr failed." << std::endl;
        return nullptr;
    }
    auto remoteObj = saManager->GetSystemAbility(TELEPHONY_SMS_MMS_SYS_ABILITY_ID);
    if (remoteObj == nullptr) {
        std::cout << "GetSystemAbility Service Failed." << std::endl;
        return nullptr;
    }
    const std::string uriContact("dataability:///com.ohos.contactsdataability");
    std::shared_ptr<Uri> dataAbilityUri = std::make_shared<Uri>(uriContact);
    if (dataAbilityUri == nullptr) {
        std::cout << "make dataAbilityUri Error." << std::endl;
        return nullptr;
    }
    return AppExecFwk::DataAbilityHelper::Creator(remoteObj, dataAbilityUri);
}

bool GsmSmsSenderTest::RequestPermissions() const
{
    std::shared_ptr<AppExecFwk::AbilityContext> context = std::make_shared<AppExecFwk::AbilityContext>();
    if (context == nullptr) {
        std::cout << "make AbilityContext Error." << std::endl;
        return false;
    }

    std::shared_ptr<AppExecFwk::ApplicationInfo> appInfo = std::make_shared<AppExecFwk::ApplicationInfo>();
    std::shared_ptr<AppExecFwk::ContextDeal> deal = std::make_shared<AppExecFwk::ContextDeal>();
    if (appInfo == nullptr || deal == nullptr) {
        std::cout << "make ApplicationInfo ContextDeal Error." << std::endl;
        return false;
    }

    const int32_t requestCodes = 10048;
    std::string name = "sms_mms_test";
    appInfo->bundleName = name;
    std::vector<std::string> permissions;
    permissions.emplace_back("ohos.permission.SET_TELEPHONY_STATE");
    permissions.emplace_back("ohos.permission.GET_TELEPHONY_STATE");
    permissions.emplace_back("ohos.permission.SEND_MESSAGES");
    permissions.emplace_back("ohos.permission.RECEIVE_SMS");
    std::vector<int> permissionsState(permissions.size(), -1);
    deal->SetApplicationInfo(appInfo);
    context->AttachBaseContext(deal);
    context->RequestPermissionsFromUser(permissions, permissionsState, requestCodes);
    std::cout << "RequestPermissions Ok." << std::endl;
    return true;
}
} // namespace Telephony
} // namespace OHOS
