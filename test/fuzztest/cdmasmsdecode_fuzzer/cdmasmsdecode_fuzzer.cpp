/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "cdmasmsdecode_fuzzer.h"

#include <sstream>

#include "addsmstoken_fuzzer.h"
#include "cdma_sms_sub_parameter.h"
#include "cdma_sms_transport_message.h"
#include "securec.h"

using namespace OHOS::Telephony;
namespace OHOS {
static constexpr uint16_t PDU_BUFFER_MAX_SIZE = 0xFF;
bool g_flag = false;

void TeleserviceIdDecode(const uint8_t *data, size_t size)
{
    uint16_t value;
    auto parameter = std::make_unique<CdmaSmsTeleserviceId>(value);
    if (parameter == nullptr) {
        return;
    }

    std::string strValue(reinterpret_cast<const char *>(data), size);
    SmsReadBuffer pdu(strValue);
    parameter->Decode(pdu);

    std::stringstream ss1;
    ss1 << static_cast<uint8_t>(CdmaSmsParameterRecord::TELESERVICE_ID);
    ss1 << static_cast<uint8_t>(size);
    for (uint8_t i = 0; i < size; i++) {
        ss1 << data[i];
    }
    SmsReadBuffer rBuffer1(ss1.str());
    parameter->Decode(rBuffer1);
}

void ServiceCategoryDecode(const uint8_t *data, size_t size)
{
    uint16_t value;
    auto parameter = std::make_unique<CdmaSmsServiceCategory>(value);
    if (parameter == nullptr) {
        return;
    }

    std::string strValue(reinterpret_cast<const char *>(data), size);
    SmsReadBuffer pdu(strValue);
    parameter->Decode(pdu);

    std::stringstream ss1;
    ss1 << static_cast<uint8_t>(CdmaSmsParameterRecord::SERVICE_CATEGORY);
    ss1 << static_cast<uint8_t>(size);
    for (uint8_t i = 0; i < size; i++) {
        ss1 << data[i];
    }
    SmsReadBuffer rBuffer1(ss1.str());
    parameter->Decode(rBuffer1);
}

void BearerReplyDecode(const uint8_t *data, size_t size)
{
    uint8_t value;
    auto parameter = std::make_unique<CdmaSmsBearerReply>(value);
    if (parameter == nullptr) {
        return;
    }

    std::string strValue(reinterpret_cast<const char *>(data), size);
    SmsReadBuffer pdu(strValue);
    parameter->Decode(pdu);

    std::stringstream ss1;
    ss1 << static_cast<uint8_t>(CdmaSmsParameterRecord::BEARER_REPLY_OPTION);
    ss1 << static_cast<uint8_t>(size);
    for (uint8_t i = 0; i < size; i++) {
        ss1 << data[i];
    }
    SmsReadBuffer rBuffer1(ss1.str());
    parameter->Decode(rBuffer1);
}

void CauseCodesDecode(const uint8_t *data, size_t size)
{
    TransportCauseCode value;
    auto parameter = std::make_unique<CdmaSmsCauseCodes>(value);
    if (parameter == nullptr) {
        return;
    }

    std::string strValue(reinterpret_cast<const char *>(data), size);
    SmsReadBuffer pdu(strValue);
    parameter->Decode(pdu);

    std::stringstream ss1;
    ss1 << static_cast<uint8_t>(CdmaSmsParameterRecord::CAUSE_CODES);
    ss1 << static_cast<uint8_t>(size);
    for (uint8_t i = 0; i < size; i++) {
        ss1 << data[i];
    }
    SmsReadBuffer rBuffer1(ss1.str());
    parameter->Decode(rBuffer1);
}

void AddressParameterDecode(const uint8_t *data, size_t size)
{
    TransportAddr value;
    auto parameter = std::make_unique<CdmaSmsAddressParameter>(value, CdmaSmsParameterRecord::ORG_ADDRESS);
    if (parameter == nullptr) {
        return;
    }

    std::string strValue(reinterpret_cast<const char *>(data), size);
    SmsReadBuffer pdu(strValue);
    parameter->Decode(pdu);

    std::stringstream ss1;
    ss1 << static_cast<uint8_t>(CdmaSmsParameterRecord::ORG_ADDRESS);
    ss1 << static_cast<uint8_t>(size);
    for (uint8_t i = 0; i < size; i++) {
        ss1 << data[i];
    }
    SmsReadBuffer rBuffer1(ss1.str());
    parameter->Decode(rBuffer1);
}

void SubaddressDecode(const uint8_t *data, size_t size)
{
    TransportSubAddr value;
    auto parameter = std::make_unique<CdmaSmsSubaddress>(value, CdmaSmsParameterRecord::ORG_SUB_ADDRESS);
    if (parameter == nullptr) {
        return;
    }

    std::string strValue(reinterpret_cast<const char *>(data), size);
    SmsReadBuffer pdu(strValue);
    parameter->Decode(pdu);

    std::stringstream ss1;
    ss1 << static_cast<uint8_t>(CdmaSmsParameterRecord::ORG_SUB_ADDRESS);
    ss1 << static_cast<uint8_t>(size);
    for (uint8_t i = 0; i < size; i++) {
        ss1 << data[i];
    }
    SmsReadBuffer rBuffer1(ss1.str());
    parameter->Decode(rBuffer1);
}

void BearerDataDecode(const uint8_t *data, size_t size)
{
    std::string strValue(reinterpret_cast<const char *>(data), size);
    SmsReadBuffer pdu(strValue);
    pdu.MoveForward();
    pdu.MoveForward();
    CdmaTeleserviceMsg value;
    auto parameter = std::make_unique<CdmaSmsBearerData>(value, pdu);
    if (parameter == nullptr) {
        return;
    }
    pdu.SetIndex(0);
    parameter->Decode(pdu);

    std::stringstream ss1;
    ss1 << static_cast<uint8_t>(CdmaSmsParameterRecord::BEARER_DATA);
    ss1 << static_cast<uint8_t>(size);
    for (uint8_t i = 0; i < size; i++) {
        ss1 << data[i];
    }
    SmsReadBuffer rBuffer1(ss1.str());
    rBuffer1.MoveForward();
    rBuffer1.MoveForward();
    auto parameter1 = std::make_unique<CdmaSmsBearerData>(value, rBuffer1);
    rBuffer1.SetIndex(0);
    parameter1->Decode(rBuffer1);

    rBuffer1.SetIndex(0);
    rBuffer1.MoveForward();
    rBuffer1.MoveForward();
    auto parameter2 = std::make_unique<CdmaSmsBearerData>(value, rBuffer1, true);
    if (parameter2 == nullptr) {
        return;
    }
    rBuffer1.SetIndex(0);
    parameter2->Decode(rBuffer1);
}

void BaseParameterDecode(const uint8_t *data, size_t size)
{
    uint8_t value;
    auto parameter = std::make_unique<CdmaSmsBaseParameter>(CdmaSmsSubParameter::MESSAGE_IDENTIFIER, value);
    if (parameter == nullptr) {
        return;
    }

    std::string strValue(reinterpret_cast<const char *>(data), size);
    SmsReadBuffer pdu(strValue);
    parameter->Decode(pdu);

    std::stringstream ss1;
    ss1 << static_cast<uint8_t>(CdmaSmsSubParameter::MESSAGE_IDENTIFIER);
    ss1 << static_cast<uint8_t>(size);
    for (uint8_t i = 0; i < size; i++) {
        ss1 << data[i];
    }
    SmsReadBuffer rBuffer1(ss1.str());
    parameter->Decode(rBuffer1);
}

void ReservedParameterDecode(const uint8_t *data, size_t size)
{
    auto parameter = std::make_unique<CdmaSmsReservedParameter>(CdmaSmsSubParameter::MESSAGE_IDENTIFIER);
    if (parameter == nullptr) {
        return;
    }

    std::string strValue(reinterpret_cast<const char *>(data), size);
    SmsReadBuffer pdu(strValue);
    parameter->Decode(pdu);

    std::stringstream ss1;
    ss1 << static_cast<uint8_t>(CdmaSmsSubParameter::MESSAGE_IDENTIFIER);
    ss1 << static_cast<uint8_t>(size);
    for (uint8_t i = 0; i < size; i++) {
        ss1 << data[i];
    }
    SmsReadBuffer rBuffer1(ss1.str());
    parameter->Decode(rBuffer1);
}

void MessageIdDecode(const uint8_t *data, size_t size)
{
    SmsTeleSvcMsgId value;
    auto parameter = std::make_unique<CdmaSmsMessageId>(value, CdmaSmsTeleserviceMessage::DELIVER);
    if (parameter == nullptr) {
        return;
    }

    std::string strValue(reinterpret_cast<const char *>(data), size);
    SmsReadBuffer pdu(strValue);
    parameter->Decode(pdu);

    std::stringstream ss1;
    ss1 << static_cast<uint8_t>(CdmaSmsSubParameter::MESSAGE_IDENTIFIER);
    ss1 << static_cast<uint8_t>(size);
    for (uint8_t i = 0; i < size; i++) {
        ss1 << data[i];
    }
    SmsReadBuffer rBuffer1(ss1.str());
    parameter->Decode(rBuffer1);
}

void AbsoluteTimeDecode(const uint8_t *data, size_t size)
{
    SmsTimeAbs value;
    auto parameter = std::make_unique<CdmaSmsAbsoluteTime>(CdmaSmsSubParameter::VALIDITY_PERIOD_ABSOLUTE, value);
    if (parameter == nullptr) {
        return;
    }

    std::string strValue(reinterpret_cast<const char *>(data), size);
    SmsReadBuffer pdu(strValue);
    parameter->Decode(pdu);

    std::stringstream ss1;
    ss1 << static_cast<uint8_t>(CdmaSmsSubParameter::VALIDITY_PERIOD_ABSOLUTE);
    ss1 << static_cast<uint8_t>(size);
    for (uint8_t i = 0; i < size; i++) {
        ss1 << data[i];
    }
    SmsReadBuffer rBuffer1(ss1.str());
    parameter->Decode(rBuffer1);
}

void PriorityIndDecode(const uint8_t *data, size_t size)
{
    SmsPriorityIndicator value;
    auto parameter = std::make_unique<CdmaSmsPriorityInd>(value);
    if (parameter == nullptr) {
        return;
    }

    std::string strValue(reinterpret_cast<const char *>(data), size);
    SmsReadBuffer pdu(strValue);
    parameter->Decode(pdu);

    std::stringstream ss1;
    ss1 << static_cast<uint8_t>(CdmaSmsSubParameter::PRIORITY_INDICATOR);
    ss1 << static_cast<uint8_t>(size);
    for (uint8_t i = 0; i < size; i++) {
        ss1 << data[i];
    }
    SmsReadBuffer rBuffer1(ss1.str());
    parameter->Decode(rBuffer1);
}

void PrivacyIndDecode(const uint8_t *data, size_t size)
{
    SmsPrivacyIndicator value;
    auto parameter = std::make_unique<CdmaSmsPrivacyInd>(value);
    if (parameter == nullptr) {
        return;
    }

    std::string strValue(reinterpret_cast<const char *>(data), size);
    SmsReadBuffer pdu(strValue);
    parameter->Decode(pdu);

    std::stringstream ss1;
    ss1 << static_cast<uint8_t>(CdmaSmsSubParameter::PRIVACY_INDICATOR);
    ss1 << static_cast<uint8_t>(size);
    for (uint8_t i = 0; i < size; i++) {
        ss1 << data[i];
    }
    SmsReadBuffer rBuffer1(ss1.str());
    parameter->Decode(rBuffer1);
}

void ReplyOptionDecode(const uint8_t *data, size_t size)
{
    SmsReplyOption value;
    auto parameter = std::make_unique<CdmaSmsReplyOption>(value);
    if (parameter == nullptr) {
        return;
    }

    std::string strValue(reinterpret_cast<const char *>(data), size);
    SmsReadBuffer pdu(strValue);
    parameter->Decode(pdu);

    std::stringstream ss1;
    ss1 << static_cast<uint8_t>(CdmaSmsSubParameter::REPLY_OPTION);
    ss1 << static_cast<uint8_t>(size);
    for (uint8_t i = 0; i < size; i++) {
        ss1 << data[i];
    }
    SmsReadBuffer rBuffer1(ss1.str());
    parameter->Decode(rBuffer1);
}

void UserDataDecode(const uint8_t *data, size_t size)
{
    SmsTeleSvcUserData value;
    bool headerInd = false;
    auto parameter = std::make_unique<CdmaSmsUserData>(value, headerInd);
    if (parameter == nullptr) {
        return;
    }

    std::string strValue(reinterpret_cast<const char *>(data), size);
    SmsReadBuffer pdu(strValue);
    parameter->Decode(pdu);

    std::stringstream ss1;
    ss1 << static_cast<uint8_t>(CdmaSmsSubParameter::USER_DATA);
    ss1 << static_cast<uint8_t>(size);
    for (uint8_t i = 0; i < size; i++) {
        ss1 << data[i];
    }
    SmsReadBuffer rBuffer1(ss1.str());
    parameter->Decode(rBuffer1);
}

void CmasDataDecode(const uint8_t *data, size_t size)
{
    SmsTeleSvcCmasData value;
    auto parameter = std::make_unique<CdmaSmsCmasData>(value);
    if (parameter == nullptr) {
        return;
    }

    std::string strValue(reinterpret_cast<const char *>(data), size);
    SmsReadBuffer pdu(strValue);
    parameter->Decode(pdu);

    std::stringstream ss1;
    ss1 << static_cast<uint8_t>(CdmaSmsSubParameter::USER_DATA);
    ss1 << static_cast<uint8_t>(size);
    for (uint8_t i = 0; i < size; i++) {
        ss1 << data[i];
    }
    SmsReadBuffer rBuffer1(ss1.str());
    parameter->Decode(rBuffer1);
}

void AlertPriorityDecode(const uint8_t *data, size_t size)
{
    SmsAlertPriority value;
    auto parameter = std::make_unique<CdmaSmsAlertPriority>(value);
    if (parameter == nullptr) {
        return;
    }

    std::string strValue(reinterpret_cast<const char *>(data), size);
    SmsReadBuffer pdu(strValue);
    parameter->Decode(pdu);

    std::stringstream ss1;
    ss1 << static_cast<uint8_t>(CdmaSmsSubParameter::ALERT_ON_MSG_DELIVERY);
    ss1 << static_cast<uint8_t>(size);
    for (uint8_t i = 0; i < size; i++) {
        ss1 << data[i];
    }
    SmsReadBuffer rBuffer1(ss1.str());
    parameter->Decode(rBuffer1);
}
void LanguageIndDecode(const uint8_t *data, size_t size)
{
    SmsLanguageType value;
    auto parameter = std::make_unique<CdmaSmsLanguageInd>(value);
    if (parameter == nullptr) {
        return;
    }

    std::string strValue(reinterpret_cast<const char *>(data), size);
    SmsReadBuffer pdu(strValue);
    parameter->Decode(pdu);

    std::stringstream ss1;
    ss1 << static_cast<uint8_t>(CdmaSmsSubParameter::LANGUAGE_INDICATOR);
    ss1 << static_cast<uint8_t>(size);
    for (uint8_t i = 0; i < size; i++) {
        ss1 << data[i];
    }
    SmsReadBuffer rBuffer1(ss1.str());
    parameter->Decode(rBuffer1);
}
void CallbackNumberDecode(const uint8_t *data, size_t size)
{
    SmsTeleSvcAddr value;
    auto parameter = std::make_unique<CdmaSmsCallbackNumber>(value);
    if (parameter == nullptr) {
        return;
    }

    std::string strValue(reinterpret_cast<const char *>(data), size);
    SmsReadBuffer pdu(strValue);
    parameter->Decode(pdu);

    std::stringstream ss1;
    ss1 << static_cast<uint8_t>(CdmaSmsSubParameter::CALLBACK_NUMBER);
    ss1 << static_cast<uint8_t>(size);
    for (uint8_t i = 0; i < size; i++) {
        ss1 << data[i];
    }
    SmsReadBuffer rBuffer1(ss1.str());
    parameter->Decode(rBuffer1);
}
void DepositIndexDecode(const uint8_t *data, size_t size)
{
    uint16_t value;
    auto parameter = std::make_unique<CdmaSmsDepositIndex>(value);
    if (parameter == nullptr) {
        return;
    }

    std::string strValue(reinterpret_cast<const char *>(data), size);
    SmsReadBuffer pdu(strValue);
    parameter->Decode(pdu);

    std::stringstream ss1;
    ss1 << static_cast<uint8_t>(CdmaSmsSubParameter::MESSAGE_DEPOSIT_INDEX);
    ss1 << static_cast<uint8_t>(size);
    for (uint8_t i = 0; i < size; i++) {
        ss1 << data[i];
    }
    SmsReadBuffer rBuffer1(ss1.str());
    parameter->Decode(rBuffer1);
}
void DisplayModeDecode(const uint8_t *data, size_t size)
{
    SmsDisplayMode value;
    auto parameter = std::make_unique<CdmaSmsDisplayMode>(value);
    if (parameter == nullptr) {
        return;
    }

    std::string strValue(reinterpret_cast<const char *>(data), size);
    SmsReadBuffer pdu(strValue);
    parameter->Decode(pdu);

    std::stringstream ss1;
    ss1 << static_cast<uint8_t>(CdmaSmsSubParameter::MESSAGE_DISPLAY_MODE);
    ss1 << static_cast<uint8_t>(size);
    for (uint8_t i = 0; i < size; i++) {
        ss1 << data[i];
    }
    SmsReadBuffer rBuffer1(ss1.str());
    parameter->Decode(rBuffer1);
}
void MessageStatusDecode(const uint8_t *data, size_t size)
{
    SmsStatusCode value;
    auto parameter = std::make_unique<CdmaSmsMessageStatus>(value);
    if (parameter == nullptr) {
        return;
    }

    std::string strValue(reinterpret_cast<const char *>(data), size);
    SmsReadBuffer pdu(strValue);
    parameter->Decode(pdu);

    std::stringstream ss1;
    ss1 << static_cast<uint8_t>(CdmaSmsSubParameter::MESSAGE_STATUS);
    ss1 << static_cast<uint8_t>(size);
    for (uint8_t i = 0; i < size; i++) {
        ss1 << data[i];
    }
    SmsReadBuffer rBuffer1(ss1.str());
    parameter->Decode(rBuffer1);
}
void NumberMessagesDecode(const uint8_t *data, size_t size)
{
    uint32_t value;
    auto parameter = std::make_unique<CdmaSmsNumberMessages>(value);
    if (parameter == nullptr) {
        return;
    }

    std::string strValue(reinterpret_cast<const char *>(data), size);
    SmsReadBuffer pdu(strValue);
    parameter->Decode(pdu);

    std::stringstream ss1;
    ss1 << static_cast<uint8_t>(CdmaSmsSubParameter::NUMBER_OF_MESSAGES);
    ss1 << static_cast<uint8_t>(size);
    for (uint8_t i = 0; i < size; i++) {
        ss1 << data[i];
    }
    SmsReadBuffer rBuffer1(ss1.str());
    parameter->Decode(rBuffer1);
}
void EnhancedVmnDecode(const uint8_t *data, size_t size)
{
    SmsEnhancedVmn value;
    auto parameter = std::make_unique<CdmaSmsEnhancedVmn>(value);
    if (parameter == nullptr) {
        return;
    }

    std::string strValue(reinterpret_cast<const char *>(data), size);
    SmsReadBuffer pdu(strValue);
    parameter->Decode(pdu);

    std::stringstream ss1;
    ss1 << static_cast<uint8_t>(CdmaSmsSubParameter::ENHANCED_VMN);
    ss1 << static_cast<uint8_t>(size);
    for (uint8_t i = 0; i < size; i++) {
        ss1 << data[i];
    }
    SmsReadBuffer rBuffer1(ss1.str());
    parameter->Decode(rBuffer1);
}
void EnhancedVmnAckDecode(const uint8_t *data, size_t size)
{
    SmsEnhancedVmnAck value;
    auto parameter = std::make_unique<CdmaSmsEnhancedVmnAck>(value);
    if (parameter == nullptr) {
        return;
    }

    std::string strValue(reinterpret_cast<const char *>(data), size);
    SmsReadBuffer pdu(strValue);
    parameter->Decode(pdu);

    std::stringstream ss1;
    ss1 << static_cast<uint8_t>(CdmaSmsSubParameter::ENHANCED_VMN_ACK);
    ss1 << static_cast<uint8_t>(size);
    for (uint8_t i = 0; i < size; i++) {
        ss1 << data[i];
    }
    SmsReadBuffer rBuffer1(ss1.str());
    parameter->Decode(rBuffer1);
}

void TransportMessageDecode(const uint8_t *data, size_t size)
{
    std::string strValue(reinterpret_cast<const char *>(data), size);
    SmsReadBuffer pdu(strValue);
    CdmaTransportMsg msg;
    auto message = CdmaSmsTransportMessage::CreateTransportMessage(msg, pdu);
    if (message == nullptr) {
        return;
    }
    message->Decode(pdu);

    std::stringstream ss1;
    ss1 << static_cast<uint8_t>(CdmaTransportMsgType::P2P);
    for (uint8_t i = 0; i < size; i++) {
        ss1 << data[i];
    }
    SmsReadBuffer rBuffer1(ss1.str());
    memset_s(&msg, sizeof(msg), 0, sizeof(msg));
    auto message1 = CdmaSmsTransportMessage::CreateTransportMessage(msg, rBuffer1);
    if (message1 == nullptr) {
        return;
    }
    message1->Decode(rBuffer1);

    std::stringstream ss2;
    ss2 << static_cast<uint8_t>(CdmaTransportMsgType::BROADCAST);
    for (uint8_t i = 0; i < size; i++) {
        ss2 << data[i];
    }
    SmsReadBuffer rBuffer2(ss2.str());
    memset_s(&msg, sizeof(msg), 0, sizeof(msg));
    auto message2 = CdmaSmsTransportMessage::CreateTransportMessage(msg, rBuffer2);
    if (message2 == nullptr) {
        return;
    }
    message2->Decode(rBuffer2);

    std::stringstream ss3;
    ss3 << static_cast<uint8_t>(CdmaTransportMsgType::ACK);
    for (uint8_t i = 0; i < size; i++) {
        ss3 << data[i];
    }
    SmsReadBuffer rBuffer3(ss3.str());
    memset_s(&msg, sizeof(msg), 0, sizeof(msg));
    auto message3 = CdmaSmsTransportMessage::CreateTransportMessage(msg, rBuffer3);
    if (message3 == nullptr) {
        return;
    }
    message3->Decode(rBuffer3);
}

void ParameterRecordDecode(const uint8_t *data, size_t size)
{
    TeleserviceIdDecode(data, size);
    ServiceCategoryDecode(data, size);
    BearerReplyDecode(data, size);
    CauseCodesDecode(data, size);
    AddressParameterDecode(data, size);
    SubaddressDecode(data, size);
    BearerDataDecode(data, size);
}

void SubParameterDecode(const uint8_t *data, size_t size)
{
    BaseParameterDecode(data, size);
    ReservedParameterDecode(data, size);
    MessageIdDecode(data, size);
    AbsoluteTimeDecode(data, size);
    PriorityIndDecode(data, size);
    PrivacyIndDecode(data, size);
    ReplyOptionDecode(data, size);
    UserDataDecode(data, size);
    CmasDataDecode(data, size);
    AlertPriorityDecode(data, size);
    LanguageIndDecode(data, size);
    CallbackNumberDecode(data, size);
    DepositIndexDecode(data, size);
    DisplayModeDecode(data, size);
    MessageStatusDecode(data, size);
    NumberMessagesDecode(data, size);
    EnhancedVmnDecode(data, size);
    EnhancedVmnAckDecode(data, size);
}

void DoSomethingInterestingWithMyAPI(const uint8_t *data, size_t size)
{
    if (data == nullptr || size == 0 || size > PDU_BUFFER_MAX_SIZE) {
        return;
    }

    if (g_flag) {
        return;
    }
    g_flag = true;

    TransportMessageDecode(data, size);
    ParameterRecordDecode(data, size);
    SubParameterDecode(data, size);
}

} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    /* Run your code on data */
    OHOS::AddSmsTokenFuzzer token;
    OHOS::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}
