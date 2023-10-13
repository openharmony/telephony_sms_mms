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

#include "ability_context.h"
#include "ability_info.h"
#include "access_mms_token.h"
#include "context_deal.h"
#include "iservice_registry.h"
#include "sms_delivery_callback_test.h"
#include "sms_send_callback_test.h"
#include "string_utils.h"
#include "telephony_errors.h"

namespace OHOS {
namespace Telephony {
namespace {
const std::string CU_MMSC = "http://mmsc.myuni.com.cn";
const std::string SEND_MMS_FILE_URL = "/data/app/deSrc/SendReq.mms";
} // namespace

void GsmSmsSenderTest::TestGsmSendShortData(const sptr<ISmsServiceInterface> &smsService) const
{
    AccessMmsToken token;
    if (smsService == nullptr) {
        std::cout << "smsService is nullptr." << std::endl;
        return;
    }
    std::string dest;
    std::cout << "Please enter the card id" << std::endl;
    int32_t slotIdTesta;
    std::cin >> dest;
    slotIdTesta = atoi(dest.c_str());
    std::cout << "Please enter the receiver's telephone number" << std::endl;
    dest.clear();
    std::cin >> dest;
    std::string sca("");
    OHOS::sptr<SmsSendCallbackTest> sendCallBackPtr(new SmsSendCallbackTest());
    OHOS::sptr<SmsDeliveryCallbackTest> deliveryCallBackPtr(new SmsDeliveryCallbackTest());
    uint16_t port = 100;
    uint8_t data[] = "hello world";
    smsService->SendMessage(slotIdTesta, StringUtils::ToUtf16(dest), StringUtils::ToUtf16(sca), port, data,
        (sizeof(data) / sizeof(data[0]) - 1), sendCallBackPtr, deliveryCallBackPtr);
    std::cout << "TestGsmSendShortData" << std::endl;
}

void GsmSmsSenderTest::TestGsmSendLongData(const sptr<ISmsServiceInterface> &smsService) const
{
    AccessMmsToken token;
    if (smsService == nullptr) {
        std::cout << "smsService is nullptr." << std::endl;
        return;
    }
    int32_t slotIdTestb;
    std::string dest;
    std::cout << "Please enter the card id" << std::endl;
    std::cin >> dest;
    slotIdTestb = atoi(dest.c_str());
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

    smsService->SendMessage(slotIdTestb, StringUtils::ToUtf16(dest), StringUtils::ToUtf16(sca), port, data,
        sizeof(data) / sizeof(data[0]), sendCallBackPtr, deliveryCallBackPtr);
    std::cout << "TestGsmSendLongData" << std::endl;
}

void GsmSmsSenderTest::TestSendShortText(const sptr<ISmsServiceInterface> &smsService) const
{
    AccessMmsToken token;
    if (smsService == nullptr) {
        std::cout << "smsService is nullptr." << std::endl;
        return;
    }
    std::string dest;
    std::cout << "Please enter the card id" << std::endl;
    int32_t slotIdTestc;
    std::cin >> dest;
    slotIdTestc = atoi(dest.c_str());
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
    smsService->SendMessage(slotIdTestc, StringUtils::ToUtf16(dest), StringUtils::ToUtf16(sca),
        StringUtils::ToUtf16(text), sendCallBackPtr, deliveryCallBackPtr);
    std::cout << "TestGsmSendShortText" << std::endl;
}

void GsmSmsSenderTest::TestSendMms(const sptr<ISmsServiceInterface> &smsService) const
{
    AccessMmsToken token;
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
    std::u16string mmsc(StringUtils::ToUtf16(CU_MMSC));
    std::u16string data(StringUtils::ToUtf16(SEND_MMS_FILE_URL));
    std::u16string ua(u"");
    std::u16string uaprof(u"");
    int32_t result = smsService->SendMms(slotId, mmsc, data, ua, uaprof);
    if (result == 0) {
        std::cout << "send mms success" << std::endl;
    } else {
        std::cout << "send mms fail" << std::endl;
    }
}

void GsmSmsSenderTest::TestSendLongText(const sptr<ISmsServiceInterface> &smsService) const
{
    AccessMmsToken token;
    if (smsService == nullptr) {
        std::cout << "smsService is nullptr." << std::endl;
        return;
    }
    int32_t slotIdTestd;
    std::string dest;
    std::cout << "Please enter the card id" << std::endl;
    std::cin >> dest;
    slotIdTestd = atoi(dest.c_str());
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
    smsService->SendMessage(slotIdTestd, StringUtils::ToUtf16(dest), StringUtils::ToUtf16(sca),
        StringUtils::ToUtf16(text), sendCallBackPtr, deliveryCallBackPtr);
    std::cout << "TestGsmSendLongText" << std::endl;
}

void GsmSmsSenderTest::TestSetSmscAddr(const sptr<ISmsServiceInterface> &smsService) const
{
    AccessMmsToken token;
    bool result = false;
    if (smsService == nullptr) {
        std::cout << "smsService is nullptr." << std::endl;
        return;
    }
    std::string dest;
    std::cout << "Please enter the card id" << std::endl;
    int32_t slotIdTeste;
    std::cin >> dest;
    slotIdTeste = atoi(dest.c_str());
    dest.clear();
    std::string sca;
    std::cout << "Please enter smsc" << std::endl;
    std::cin >> sca;
    result = smsService->SetSmscAddr(slotIdTeste, StringUtils::ToUtf16(sca));
    if (result) {
        std::cout << "TestSetSmscAddr OK!" << std::endl;
    } else {
        std::cout << "TestSetSmscAddr failed!" << std::endl;
    }
}

void GsmSmsSenderTest::TestGetSmscAddr(const sptr<ISmsServiceInterface> &smsService) const
{
    AccessMmsToken token;
    std::u16string result;
    if (smsService == nullptr) {
        std::cout << "smsService is nullptr." << std::endl;
        return;
    }
    std::string dest;
    std::cout << "Please enter the card id" << std::endl;
    int32_t slotIdTestf;
    std::cin >> dest;
    slotIdTestf = atoi(dest.c_str());
    dest.clear();
    smsService->GetSmscAddr(slotIdTestf, result);
    std::cout << "TestGetSmscAddr:" << StringUtils::ToUtf8(result) << std::endl;
}

void GsmSmsSenderTest::TestAddSimMessage(const sptr<ISmsServiceInterface> &smsService) const
{
    AccessMmsToken token;
    bool result = false;
    if (smsService == nullptr) {
        std::cout << "smsService is nullptr." << std::endl;
        return;
    }
    std::string dest;
    std::cout << "Please enter the card id" << std::endl;
    int32_t slotIdTestg;
    std::cin >> dest;
    slotIdTestg = atoi(dest.c_str());
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
    result = smsService->AddSimMessage(slotIdTestg, StringUtils::ToUtf16(smsc), StringUtils::ToUtf16(pdu),
        static_cast<ISmsServiceInterface::SimMessageStatus>(status));
    if (result == 0) {
        std::cout << "TestAddSimMessage OK!" << std::endl;
    } else {
        std::cout << "TestAddSimMessage failed!" << std::endl;
    }
}

void GsmSmsSenderTest::TestDelSimMessage(const sptr<ISmsServiceInterface> &smsService) const
{
    AccessMmsToken token;
    if (smsService == nullptr) {
        std::cout << "smsService is nullptr." << std::endl;
        return;
    }
    bool result = false;
    int32_t slotIdTesth;
    std::string dest;
    std::cout << "Please enter the card id" << std::endl;
    std::cin >> dest;
    slotIdTesth = atoi(dest.c_str());
    dest.clear();
    std::string input;
    std::cout << "Please enter msgIndex" << std::endl;
    std::cin >> input;
    uint32_t msgIndex = std::atoi(input.c_str());
    result = smsService->DelSimMessage(slotIdTesth, msgIndex);
    if (result == 0) {
        std::cout << "TestDelSimMessage OK!" << std::endl;
    } else {
        std::cout << "TestDelSimMessage failed!" << std::endl;
    }
}

void GsmSmsSenderTest::TestUpdateSimMessage(const sptr<ISmsServiceInterface> &smsService) const
{
    AccessMmsToken token;
    if (smsService == nullptr) {
        std::cout << "smsService is nullptr." << std::endl;
        return;
    }
    bool result = false;
    std::string dest;
    std::cout << "Please enter the card id" << std::endl;
    int32_t slotIdTesti;
    std::cin >> dest;
    slotIdTesti = atoi(dest.c_str());
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
        slotIdTesti, msgIndex, static_cast<ISmsServiceInterface::SimMessageStatus>(status), pduData, smscData);
    if (result) {
        std::cout << "TestUpdateSimMessage OK!" << std::endl;
    } else {
        std::cout << "TestUpdateSimMessage failed!" << std::endl;
    }
}

void GsmSmsSenderTest::TestGetAllSimMessages(const sptr<ISmsServiceInterface> &smsService) const
{
    AccessMmsToken token;
    std::vector<ShortMessage> result;
    if (smsService == nullptr) {
        std::cout << "smsService is nullptr." << std::endl;
        return;
    }
    std::string dest;
    std::cout << "Please enter the card id" << std::endl;
    int32_t slotIdTestj;
    std::cin >> dest;
    slotIdTestj = atoi(dest.c_str());
    dest.clear();
    smsService->GetAllSimMessages(slotIdTestj, result);
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
    AccessMmsToken token;
    if (smsService == nullptr) {
        std::cout << "smsService is nullptr." << std::endl;
        return;
    }
    std::string dest;
    std::cout << "Please enter the card id" << std::endl;
    int32_t slotIdTestk;
    std::cin >> dest;
    bool enable = true;
    slotIdTestk = atoi(dest.c_str());
    dest.clear();
    uint8_t ranType = 1;
    uint32_t startMessageId = 0;
    uint32_t endMessageId = 0;
    std::cout << "Please enter startMessageId" << std::endl;
    std::string input;
    std::cin >> input;
    startMessageId = std::atoi(input.c_str());
    std::cout << "Please enter endMessageId" << std::endl;
    std::cin >> input;
    endMessageId = std::atoi(input.c_str());
    int32_t result = smsService->SetCBConfig(slotIdTestk, enable, startMessageId, endMessageId, ranType);
    std::cout << "TestEnableCBRangeConfig:" << result << std::endl;
}

void GsmSmsSenderTest::TestDisableCBRangeConfig(const sptr<ISmsServiceInterface> &smsService) const
{
    AccessMmsToken token;
    if (smsService == nullptr) {
        std::cout << "smsService is nullptr." << std::endl;
        return;
    }
    std::string dest;
    std::cout << "Please enter the card id" << std::endl;
    std::cin >> dest;
    int32_t slotIdTestl = atoi(dest.c_str());
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
    int32_t result = smsService->SetCBConfig(slotIdTestl, enable, startMessageId, endMessageId, ranType);
    std::cout << "TestDisableCBRangeConfig:" << result << std::endl;
}

void GsmSmsSenderTest::TestEnableCBConfig(const sptr<ISmsServiceInterface> &smsService) const
{
    AccessMmsToken token;
    if (smsService == nullptr) {
        std::cout << "smsService is nullptr." << std::endl;
        return;
    }
    std::string dest;
    std::cout << "Please enter the card id" << std::endl;
    int32_t slotIdTestm;
    std::cin >> dest;
    slotIdTestm = atoi(dest.c_str());
    dest.clear();
    bool enable = true;
    uint32_t identifier = 0;
    uint8_t ranType = 1;
    std::string input;
    std::cout << "Please enter identifier" << std::endl;
    std::cin >> input;
    identifier = std::atoi(input.c_str());
    int32_t result = smsService->SetCBConfig(slotIdTestm, enable, identifier, identifier, ranType);
    std::cout << "TestEnableCBConfig:" << result << std::endl;
}

void GsmSmsSenderTest::TestDisableCBConfig(const sptr<ISmsServiceInterface> &smsService) const
{
    AccessMmsToken token;
    if (smsService == nullptr) {
        std::cout << "smsService is nullptr." << std::endl;
        return;
    }
    std::string dest;
    std::cout << "Please enter the card id" << std::endl;
    int32_t slotIdTestn;
    std::cin >> dest;
    slotIdTestn = atoi(dest.c_str());
    dest.clear();
    bool enable = false;
    uint32_t identifier = 0;
    uint8_t ranType = 1;
    std::string input;
    std::cout << "Please enter identifier" << std::endl;
    std::cin >> input;
    identifier = std::atoi(input.c_str());
    int32_t result = smsService->SetCBConfig(slotIdTestn, enable, identifier, identifier, ranType);
    std::cout << "TestDisableCBConfig:" << result << std::endl;
}

void GsmSmsSenderTest::TestSetDefaultSmsSlotId(const sptr<ISmsServiceInterface> &smsService) const
{
    AccessMmsToken token;
    if (smsService == nullptr) {
        std::cout << "smsService is nullptr." << std::endl;
        return;
    }
    int32_t slotId;
    std::string input;
    std::cout << "Please enter Slot Id" << std::endl;
    std::cin >> input;
    slotId = std::atoi(input.c_str());
    int32_t result = smsService->SetDefaultSmsSlotId(slotId);
    std::cout << "TestSetDefaultSmsSlotId:" << result << std::endl;
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
    AccessMmsToken token;
    std::vector<std::u16string> result;
    if (smsService == nullptr) {
        std::cout << "smsService is nullptr." << std::endl;
        return;
    }
    std::string input;
    std::cout << "Please enter message" << std::endl;
    std::getline(std::cin, input);
    smsService->SplitMessage(StringUtils::ToUtf16(input), result);
    std::cout << "TestSplitMessage size:" << result.size() << std::endl;
    for (auto &item : result) {
        std::cout << StringUtils::ToUtf8(item) << std::endl;
    }
}

void GsmSmsSenderTest::TestGetSmsSegmentsInfo(const sptr<ISmsServiceInterface> &smsService) const
{
    if (smsService == nullptr) {
        std::cout << "TestGetSmsSegmentsInfo smsService is nullptr." << std::endl;
        return;
    }
    int32_t slotId;
    std::string input;
    std::cout << "TestGetSmsSegmentsInfo Please enter Slot Id" << std::endl;
    std::getline(std::cin, input);
    slotId = std::atoi(input.c_str());
    input.clear();
    std::cout << "Please enter message" << std::endl;
    std::getline(std::cin, input);
    ISmsServiceInterface::SmsSegmentsInfo result;
    if (smsService->GetSmsSegmentsInfo(slotId, StringUtils::ToUtf16(input), false, result) != TELEPHONY_ERR_SUCCESS) {
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
        std::cout << "TestIsImsSmsSupported smsService is nullptr." << std::endl;
        return;
    }
    int32_t slotId;
    std::string input;
    std::cout << "TestIsImsSmsSupported Please enter Slot Id" << std::endl;
    std::getline(std::cin, input);
    slotId = std::atoi(input.c_str());
    bool result = false;
    smsService->IsImsSmsSupported(slotId, result);
    std::string res = result ? "true" : "false";
    std::cout << "IsImsSmsSupported:" << res << std::endl;
}

void GsmSmsSenderTest::TestSetImsSmsConfig(const sptr<ISmsServiceInterface> &smsService) const
{
    AccessMmsToken token;
    if (smsService == nullptr) {
        std::cout << "TestSetImsSmsConfig smsService is nullptr." << std::endl;
        return;
    }
    int32_t slotId;
    std::string input;
    std::cout << "TestSetImsSmsConfig Please enter Slot Id" << std::endl;
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
    std::u16string format;
    smsService->GetImsShortMessageFormat(format);
    std::cout << "GetImsShortMessageFormat:" << StringUtils::ToUtf8(format) << std::endl;
}

void GsmSmsSenderTest::TestAddBlockPhone() const
{
    std::shared_ptr<DataShare::DataShareHelper> helper = CreateDataAHelper();
    if (helper == nullptr) {
        std::cout << "Creator helper nullptr error." << std::endl;
        return;
    }
    std::string input;
    std::cout << "Please enter block phone number" << std::endl;
    std::getline(std::cin, input);

    Uri uri("datashare:///com.ohos.contactsdataability/contacts/contact_blocklist");
    DataShare::DataShareValuesBucket value;
    value.Put("phone_number", input);
    int ret = helper->Insert(uri, value);
    helper->Release();
    std::cout << "add block:" << input << ((ret >= 0) ? " success" : " error") << std::endl;
}

void GsmSmsSenderTest::TestRemoveBlockPhone() const
{
    std::shared_ptr<DataShare::DataShareHelper> helper = CreateDataAHelper();
    if (helper == nullptr) {
        std::cout << "Creator helper nullptr error." << std::endl;
        return;
    }
    std::string input;
    std::cout << "Please enter Remove phone number" << std::endl;
    std::getline(std::cin, input);
    Uri uri("datashare:///com.ohos.contactsdataability/contacts/contact_blocklist");
    DataShare::DataSharePredicates predicates;
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

std::shared_ptr<DataShare::DataShareHelper> GsmSmsSenderTest::CreateDataAHelper() const
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
    const std::string uriContact("datashare:///com.ohos.contactsdataability");
    return DataShare::DataShareHelper::Creator(remoteObj, uriContact);
}
} // namespace Telephony
} // namespace OHOS
