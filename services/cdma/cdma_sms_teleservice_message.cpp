/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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

#include "cdma_sms_teleservice_message.h"

#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {

CdmaSmsTeleserviceMessage::~CdmaSmsTeleserviceMessage() {}

bool CdmaSmsTeleserviceMessage::Encode(SmsWriteBuffer &pdu)
{
    return false;
}

bool CdmaSmsTeleserviceMessage::Decode(SmsReadBuffer &pdu)
{
    return false;
}

uint8_t CdmaSmsTeleserviceMessage::GetMessageType(SmsReadBuffer &pdu)
{
    return RESERVED;
}

CdmaSmsSubmitMessage::CdmaSmsSubmitMessage(TeleserviceSubmit &msg)
{
    type_ = SUBMIT;
}

CdmaSmsSubmitMessage::CdmaSmsSubmitMessage(TeleserviceSubmit &msg, SmsReadBuffer &pdu)
{
    if (pdu.IsEmpty()) {
        TELEPHONY_LOGE("pdu is empty");
        return;
    }

    type_ = SUBMIT;
    msg.priority = SmsPriorityIndicator::RESERVED;
    uint8_t id = 0;
    uint8_t len = 0;
    while (pdu.ReadByte(id) && pdu.ReadByte(len)) {
        pdu.MoveForward(len);
    }
}

CdmaSmsCancelMessage::CdmaSmsCancelMessage(TeleserviceCancel &msg)
{
    type_ = CANCELLATION;
}

CdmaSmsDeliverReport::CdmaSmsDeliverReport(TeleserviceDeliverReport &msg)
{
    type_ = DELIVER_REPORT;
}

CdmaSmsDeliverMessage::CdmaSmsDeliverMessage(TeleserviceDeliver &msg, SmsReadBuffer &pdu, bool isCMAS)
{
    if (pdu.IsEmpty()) {
        TELEPHONY_LOGE("pdu is empty");
        return;
    }

    type_ = DELIVER;
    uint8_t id = 0;
    uint8_t len = 0;
    while (pdu.ReadByte(id) && pdu.ReadByte(len)) {
        pdu.MoveForward(len);
    }
}

CdmaSmsDeliveryAck::CdmaSmsDeliveryAck(TeleserviceDeliverAck &msg, SmsReadBuffer &pdu)
{
    if (pdu.IsEmpty()) {
        TELEPHONY_LOGE("pdu is empty");
        return;
    }

    type_ = DELIVERY_ACK;
    uint8_t id = 0;
    uint8_t len = 0;
    while (pdu.ReadByte(id) && pdu.ReadByte(len)) {
        pdu.MoveForward(len);
    }
}

CdmaSmsUserAck::CdmaSmsUserAck(TeleserviceUserAck &msg, SmsReadBuffer &pdu)
{
    if (pdu.IsEmpty()) {
        TELEPHONY_LOGE("pdu is empty");
        return;
    }

    type_ = USER_ACK;
    uint8_t id = 0;
    uint8_t len = 0;
    while (pdu.ReadByte(id) && pdu.ReadByte(len)) {
        pdu.MoveForward(len);
    }
}

CdmaSmsReadAck::CdmaSmsReadAck(TeleserviceReadAck &msg, SmsReadBuffer &pdu)
{
    if (pdu.IsEmpty()) {
        TELEPHONY_LOGE("pdu is empty");
        return;
    }

    type_ = READ_ACK;
    uint8_t id = 0;
    uint8_t len = 0;
    while (pdu.ReadByte(id) && pdu.ReadByte(len)) {
        pdu.MoveForward(len);
    }
}

} // namespace Telephony
} // namespace OHOS