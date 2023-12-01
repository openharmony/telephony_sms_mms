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

#include "cdmasmsencode_fuzzer.h"

#include "addsmstoken_fuzzer.h"
#include "cdma_sms_sub_parameter.h"
#include "cdma_sms_transport_message.h"
#include "securec.h"

using namespace OHOS::Telephony;
namespace OHOS {
bool g_flag = false;
constexpr int32_t BOOL_NUM = 2;

void TeleserviceIdEncode(const uint8_t *data, size_t size)
{
    uint16_t value;
    if (memcpy_s(&value, sizeof(value), data, size) != EOK) {
        return;
    }
    auto parameter = std::make_unique<CdmaSmsTeleserviceId>(value);
    if (parameter == nullptr) {
        return;
    }
    SmsWriteBuffer pdu;
    parameter->Encode(pdu);
    auto pduBuffer = pdu.GetPduBuffer();
}

void ServiceCategoryEncode(const uint8_t *data, size_t size)
{
    uint16_t value;
    if (memcpy_s(&value, sizeof(value), data, size) != EOK) {
        return;
    }
    auto parameter = std::make_unique<CdmaSmsServiceCategory>(value);
    if (parameter == nullptr) {
        return;
    }
    SmsWriteBuffer pdu;
    parameter->Encode(pdu);
    auto pduBuffer = pdu.GetPduBuffer();
}

void BearerReplyEncode(const uint8_t *data, size_t size)
{
    uint8_t value;
    if (memcpy_s(&value, sizeof(value), data, size) != EOK) {
        return;
    }
    auto parameter = std::make_unique<CdmaSmsBearerReply>(value);
    if (parameter == nullptr) {
        return;
    }
    SmsWriteBuffer pdu;
    parameter->Encode(pdu);
    auto pduBuffer = pdu.GetPduBuffer();
}

void CauseCodesEncode(const uint8_t *data, size_t size)
{
    TransportCauseCode value;
    if (memcpy_s(&value, sizeof(value), data, size) != EOK) {
        return;
    }
    auto parameter = std::make_unique<CdmaSmsCauseCodes>(value);
    if (parameter == nullptr) {
        return;
    }
    SmsWriteBuffer pdu;
    parameter->Encode(pdu);
    auto pduBuffer = pdu.GetPduBuffer();
}

void AddressParameterEncode(const uint8_t *data, size_t size)
{
    TransportAddr value;
    if (memcpy_s(&value, sizeof(value), data, size) != EOK) {
        return;
    }
    value.digitMode = static_cast<bool>(size % BOOL_NUM);
    value.numberMode = static_cast<bool>(size % BOOL_NUM);
    auto parameter = std::make_unique<CdmaSmsAddressParameter>(value, CdmaSmsParameterRecord::ORG_ADDRESS);
    if (parameter == nullptr) {
        return;
    }
    SmsWriteBuffer pdu;
    parameter->Encode(pdu);
    auto pduBuffer = pdu.GetPduBuffer();
}

void SubaddressEncode(const uint8_t *data, size_t size)
{
    TransportSubAddr value;
    if (memcpy_s(&value, sizeof(value), data, size) != EOK) {
        return;
    }
    value.odd = static_cast<bool>(size % BOOL_NUM);
    auto parameter = std::make_unique<CdmaSmsSubaddress>(value, CdmaSmsParameterRecord::ORG_SUB_ADDRESS);
    if (parameter == nullptr) {
        return;
    }
    SmsWriteBuffer pdu;
    parameter->Encode(pdu);
    auto pduBuffer = pdu.GetPduBuffer();
}

void BearerDataEncode(const uint8_t *data, size_t size)
{
    CdmaTeleserviceMsg value;
    if (memcpy_s(&value, sizeof(value), data, size) != EOK) {
        return;
    }
    auto parameter = std::make_unique<CdmaSmsBearerData>(value);
    if (parameter == nullptr) {
        return;
    }
    SmsWriteBuffer pdu;
    parameter->Encode(pdu);
    auto pduBuffer = pdu.GetPduBuffer();
}

void BaseParameterEncode(const uint8_t *data, size_t size)
{
    uint8_t value;
    if (memcpy_s(&value, sizeof(value), data, size) != EOK) {
        return;
    }
    auto parameter = std::make_unique<CdmaSmsBaseParameter>(CdmaSmsSubParameter::MESSAGE_IDENTIFIER, value);
    if (parameter == nullptr) {
        return;
    }
    SmsWriteBuffer pdu;
    parameter->Encode(pdu);
    auto pduBuffer = pdu.GetPduBuffer();
}

void ReservedParameterEncode(const uint8_t *data, size_t size)
{
    auto parameter = std::make_unique<CdmaSmsReservedParameter>(CdmaSmsSubParameter::MESSAGE_IDENTIFIER);
    if (parameter == nullptr) {
        return;
    }
    SmsWriteBuffer pdu;
    parameter->Encode(pdu);
    auto pduBuffer = pdu.GetPduBuffer();
}

void MessageIdEncode(const uint8_t *data, size_t size)
{
    SmsTeleSvcMsgId value;
    if (memcpy_s(&value, sizeof(value), data, size) != EOK) {
        return;
    }
    value.headerInd = static_cast<bool>(size % BOOL_NUM);
    auto parameter = std::make_unique<CdmaSmsMessageId>(value, CdmaSmsTeleserviceMessage::DELIVER);
    if (parameter == nullptr) {
        return;
    }
    SmsWriteBuffer pdu;
    parameter->Encode(pdu);
    auto pduBuffer = pdu.GetPduBuffer();
}

void AbsoluteTimeEncode(const uint8_t *data, size_t size)
{
    SmsTimeAbs value;
    if (memcpy_s(&value, sizeof(value), data, size) != EOK) {
        return;
    }
    auto parameter = std::make_unique<CdmaSmsAbsoluteTime>(CdmaSmsSubParameter::VALIDITY_PERIOD_ABSOLUTE, value);
    if (parameter == nullptr) {
        return;
    }
    SmsWriteBuffer pdu;
    parameter->Encode(pdu);
    auto pduBuffer = pdu.GetPduBuffer();
}

void PriorityIndEncode(const uint8_t *data, size_t size)
{
    SmsPriorityIndicator value;
    if (memcpy_s(&value, sizeof(value), data, size) != EOK) {
        return;
    }
    auto parameter = std::make_unique<CdmaSmsPriorityInd>(value);
    if (parameter == nullptr) {
        return;
    }
    SmsWriteBuffer pdu;
    parameter->Encode(pdu);
    auto pduBuffer = pdu.GetPduBuffer();
}

void PrivacyIndEncode(const uint8_t *data, size_t size)
{
    SmsPrivacyIndicator value;
    if (memcpy_s(&value, sizeof(value), data, size) != EOK) {
        return;
    }
    auto parameter = std::make_unique<CdmaSmsPrivacyInd>(value);
    if (parameter == nullptr) {
        return;
    }
    SmsWriteBuffer pdu;
    parameter->Encode(pdu);
    auto pduBuffer = pdu.GetPduBuffer();
}

void ReplyOptionEncode(const uint8_t *data, size_t size)
{
    SmsReplyOption value;
    if (memcpy_s(&value, sizeof(value), data, size) != EOK) {
        return;
    }
    auto parameter = std::make_unique<CdmaSmsReplyOption>(value);
    if (parameter == nullptr) {
        return;
    }
    SmsWriteBuffer pdu;
    parameter->Encode(pdu);
    auto pduBuffer = pdu.GetPduBuffer();
}

void UserDataEncode(const uint8_t *data, size_t size)
{
    SmsTeleSvcUserData value;
    if (memcpy_s(&value, sizeof(value), data, size) != EOK) {
        return;
    }
    bool headerInd = false;
    auto parameter = std::make_unique<CdmaSmsUserData>(value, headerInd);
    if (parameter == nullptr) {
        return;
    }
    SmsWriteBuffer pdu;
    parameter->Encode(pdu);
    auto pduBuffer = pdu.GetPduBuffer();
}

void CmasDataEncode(const uint8_t *data, size_t size)
{
    SmsTeleSvcCmasData value;
    if (memcpy_s(&value, sizeof(value), data, size) != EOK) {
        return;
    }
    auto parameter = std::make_unique<CdmaSmsCmasData>(value);
    if (parameter == nullptr) {
        return;
    }
    SmsWriteBuffer pdu;
    parameter->Encode(pdu);
    auto pduBuffer = pdu.GetPduBuffer();
}

void AlertPriorityEncode(const uint8_t *data, size_t size)
{
    SmsAlertPriority value;
    if (memcpy_s(&value, sizeof(value), data, size) != EOK) {
        return;
    }
    auto parameter = std::make_unique<CdmaSmsAlertPriority>(value);
    if (parameter == nullptr) {
        return;
    }
    SmsWriteBuffer pdu;
    parameter->Encode(pdu);
    auto pduBuffer = pdu.GetPduBuffer();
}
void LanguageIndEncode(const uint8_t *data, size_t size)
{
    SmsLanguageType value;
    if (memcpy_s(&value, sizeof(value), data, size) != EOK) {
        return;
    }
    auto parameter = std::make_unique<CdmaSmsLanguageInd>(value);
    if (parameter == nullptr) {
        return;
    }
    SmsWriteBuffer pdu;
    parameter->Encode(pdu);
    auto pduBuffer = pdu.GetPduBuffer();
}
void CallbackNumberEncode(const uint8_t *data, size_t size)
{
    SmsTeleSvcAddr value;
    if (memcpy_s(&value, sizeof(value), data, size) != EOK) {
        return;
    }
    value.digitMode = static_cast<bool>(size % BOOL_NUM);
    auto parameter = std::make_unique<CdmaSmsCallbackNumber>(value);
    if (parameter == nullptr) {
        return;
    }
    SmsWriteBuffer pdu;
    parameter->Encode(pdu);
    auto pduBuffer = pdu.GetPduBuffer();
}
void DepositIndexEncode(const uint8_t *data, size_t size)
{
    uint16_t value;
    if (memcpy_s(&value, sizeof(value), data, size) != EOK) {
        return;
    }
    auto parameter = std::make_unique<CdmaSmsDepositIndex>(value);
    if (parameter == nullptr) {
        return;
    }
    SmsWriteBuffer pdu;
    parameter->Encode(pdu);
    auto pduBuffer = pdu.GetPduBuffer();
}
void DisplayModeEncode(const uint8_t *data, size_t size)
{
    SmsDisplayMode value;
    if (memcpy_s(&value, sizeof(value), data, size) != EOK) {
        return;
    }
    auto parameter = std::make_unique<CdmaSmsDisplayMode>(value);
    if (parameter == nullptr) {
        return;
    }
    SmsWriteBuffer pdu;
    parameter->Encode(pdu);
    auto pduBuffer = pdu.GetPduBuffer();
}
void MessageStatusEncode(const uint8_t *data, size_t size)
{
    SmsStatusCode value;
    if (memcpy_s(&value, sizeof(value), data, size) != EOK) {
        return;
    }
    auto parameter = std::make_unique<CdmaSmsMessageStatus>(value);
    if (parameter == nullptr) {
        return;
    }
    SmsWriteBuffer pdu;
    parameter->Encode(pdu);
    auto pduBuffer = pdu.GetPduBuffer();
}
void NumberMessagesEncode(const uint8_t *data, size_t size)
{
    uint32_t value;
    if (memcpy_s(&value, sizeof(value), data, size) != EOK) {
        return;
    }
    auto parameter = std::make_unique<CdmaSmsNumberMessages>(value);
    if (parameter == nullptr) {
        return;
    }
    SmsWriteBuffer pdu;
    parameter->Encode(pdu);
    auto pduBuffer = pdu.GetPduBuffer();
}
void EnhancedVmnEncode(const uint8_t *data, size_t size)
{
    SmsEnhancedVmn value;
    if (memcpy_s(&value, sizeof(value), data, size) != EOK) {
        return;
    }
    auto parameter = std::make_unique<CdmaSmsEnhancedVmn>(value);
    if (parameter == nullptr) {
        return;
    }
    SmsWriteBuffer pdu;
    parameter->Encode(pdu);
    auto pduBuffer = pdu.GetPduBuffer();
}
void EnhancedVmnAckEncode(const uint8_t *data, size_t size)
{
    SmsEnhancedVmnAck value;
    if (memcpy_s(&value, sizeof(value), data, size) != EOK) {
        return;
    }
    auto parameter = std::make_unique<CdmaSmsEnhancedVmnAck>(value);
    if (parameter == nullptr) {
        return;
    }
    SmsWriteBuffer pdu;
    parameter->Encode(pdu);
    auto pduBuffer = pdu.GetPduBuffer();
}

void TransportMessageEncode(const uint8_t *data, size_t size)
{
    SmsWriteBuffer pdu;
    CdmaTransportMsg msg;
    if (memcpy_s(&msg, sizeof(msg), data, size) != EOK) {
        return;
    }
    auto message = CdmaSmsTransportMessage::CreateTransportMessage(msg);
    if (message == nullptr) {
        return;
    }
    message->Encode(pdu);
    auto pduBuffer = pdu.GetPduBuffer();

    SmsWriteBuffer wBuffer1;
    msg.type = CdmaTransportMsgType::P2P;
    auto message1 = CdmaSmsTransportMessage::CreateTransportMessage(msg);
    if (message1 == nullptr) {
        return;
    }
    message1->Encode(wBuffer1);
    auto pduBuffer1 = wBuffer1.GetPduBuffer();

    SmsWriteBuffer wBuffer2;
    msg.type = CdmaTransportMsgType::BROADCAST;
    auto message2 = CdmaSmsTransportMessage::CreateTransportMessage(msg);
    if (message2 == nullptr) {
        return;
    }
    message2->Encode(wBuffer2);
    auto pduBuffer2 = wBuffer2.GetPduBuffer();

    SmsWriteBuffer wBuffer3;
    msg.type = CdmaTransportMsgType::ACK;
    auto message3 = CdmaSmsTransportMessage::CreateTransportMessage(msg);
    if (message3 == nullptr) {
        return;
    }
    message3->Encode(wBuffer3);
    auto pduBuffer3 = wBuffer3.GetPduBuffer();
}

void ParameterRecordEncode(const uint8_t *data, size_t size)
{
    TeleserviceIdEncode(data, size);
    ServiceCategoryEncode(data, size);
    BearerReplyEncode(data, size);
    CauseCodesEncode(data, size);
    AddressParameterEncode(data, size);
    SubaddressEncode(data, size);
    BearerDataEncode(data, size);
}

void SubParameterEncode(const uint8_t *data, size_t size)
{
    BaseParameterEncode(data, size);
    ReservedParameterEncode(data, size);
    MessageIdEncode(data, size);
    AbsoluteTimeEncode(data, size);
    PriorityIndEncode(data, size);
    PrivacyIndEncode(data, size);
    ReplyOptionEncode(data, size);
    UserDataEncode(data, size);
    CmasDataEncode(data, size);
    AlertPriorityEncode(data, size);
    LanguageIndEncode(data, size);
    CallbackNumberEncode(data, size);
    DepositIndexEncode(data, size);
    DisplayModeEncode(data, size);
    MessageStatusEncode(data, size);
    NumberMessagesEncode(data, size);
    EnhancedVmnEncode(data, size);
    EnhancedVmnAckEncode(data, size);
}

void DoSomethingInterestingWithMyAPI(const uint8_t *data, size_t size)
{
    if (data == nullptr || size == 0) {
        return;
    }

    if (g_flag) {
        return;
    }
    g_flag = true;

    TransportMessageEncode(data, size);
    ParameterRecordEncode(data, size);
    SubParameterEncode(data, size);
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
