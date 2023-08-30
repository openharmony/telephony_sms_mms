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
static constexpr uint8_t SUB_PARAMETER_SIZE_MAX = 24;

CdmaSmsTeleserviceMessage::~CdmaSmsTeleserviceMessage()
{
    parameter_.clear();
    parameter_.shrink_to_fit();
}

bool CdmaSmsTeleserviceMessage::Encode(SmsWriteBuffer &pdu)
{
    if (pdu.IsEmpty()) {
        TELEPHONY_LOGE("pdu is empty");
        return false;
    }
    if (parameter_.size() == 0) {
        TELEPHONY_LOGE("parameter is empty");
        return false;
    }

    for (uint8_t i = 0; i < parameter_.size(); i++) {
        if (parameter_[i] == nullptr || !parameter_[i]->Encode(pdu)) {
            TELEPHONY_LOGE("parameter encode error");
            return false;
        }
    }
    return true;
}

bool CdmaSmsTeleserviceMessage::Decode(SmsReadBuffer &pdu)
{
    if (pdu.IsEmpty()) {
        TELEPHONY_LOGE("pdu is empty");
        return false;
    }
    if (parameter_.size() == 0) {
        TELEPHONY_LOGE("parameter is empty");
        return false;
    }

    for (uint8_t i = 0; i < parameter_.size(); i++) {
        if (parameter_[i] == nullptr || !parameter_[i]->Decode(pdu)) {
            TELEPHONY_LOGE("parameter decode error");
            return false;
        }
    }
    return true;
}

uint8_t CdmaSmsTeleserviceMessage::GetMessageType(SmsReadBuffer &pdu)
{
    if (pdu.IsEmpty()) {
        TELEPHONY_LOGE("pdu is empty");
        return RESERVED;
    }

    uint8_t id = CdmaSmsSubParameter::RESERVED;
    uint8_t type = RESERVED;
    uint8_t len = 0;
    while (pdu.ReadByte(id)) {
        if (id == CdmaSmsSubParameter::MESSAGE_IDENTIFIER) {
            SmsTeleSvcMsgId msgId;
            CdmaSmsMessageId messageId(msgId, type);
            pdu.MoveBack();
            if (messageId.Decode(pdu)) {
                type = messageId.GetMessageType();
            }
            return type;
        }
        if (!pdu.ReadByte(len)) {
            TELEPHONY_LOGE("len read error");
            return type;
        }
        pdu.MoveForward(len);
    }
    return type;
}

CdmaSmsSubmitMessage::CdmaSmsSubmitMessage(TeleserviceSubmit &msg)
{
    type_ = SUBMIT;
    // Message Identifier
    parameter_.push_back(std::make_unique<CdmaSmsMessageId>(msg.msgId, type_));
    // User Data
    parameter_.push_back(std::make_unique<CdmaSmsUserData>(msg.userData, msg.msgId.headerInd));
    // Deferred Delivery Time
    if (msg.deferValPeriod.format == SMS_TIME_RELATIVE) {
        parameter_.push_back(std::make_unique<CdmaSmsBaseParameter>(
            CdmaSmsSubParameter::DEFERRED_DELIVERY_TIME_RELATIVE, msg.deferValPeriod.time.relTime.time));
    } else if (msg.deferValPeriod.format == SMS_TIME_ABSOLUTE) {
        parameter_.push_back(std::make_unique<CdmaSmsAbsoluteTime>(
            CdmaSmsSubParameter::DEFERRED_DELIVERY_TIME_ABSOLUTE, msg.deferValPeriod.time.absTime));
    }
    // Priority Indicator
    if (msg.priority <= SmsPriorityIndicator::EMERGENCY) {
        parameter_.push_back(std::make_unique<CdmaSmsPriorityInd>(msg.priority));
    }
    // Reply Option
    if (msg.replyOpt.userAck || msg.replyOpt.dak || msg.replyOpt.readAck || msg.replyOpt.report) {
        parameter_.push_back(std::make_unique<CdmaSmsReplyOption>(msg.replyOpt));
    }
    // Call-Back Number
    if (msg.callbackNumber.addrLen > 0) {
        parameter_.push_back(std::make_unique<CdmaSmsCallbackNumber>(msg.callbackNumber));
    }
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
    while ((parameter_.size() < SUB_PARAMETER_SIZE_MAX) && pdu.ReadByte(id) && pdu.ReadByte(len)) {
        if (id == CdmaSmsSubParameter::MESSAGE_IDENTIFIER) {
            parameter_.push_back(std::make_unique<CdmaSmsMessageId>(msg.msgId, type_));
        } else if (id == CdmaSmsSubParameter::USER_DATA) {
            parameter_.push_back(std::make_unique<CdmaSmsUserData>(msg.userData, msg.msgId.headerInd));
        } else if (id == CdmaSmsSubParameter::VALIDITY_PERIOD_ABSOLUTE) {
            msg.valPeriod.format = SMS_TIME_ABSOLUTE;
            parameter_.push_back(std::make_unique<CdmaSmsAbsoluteTime>(id, msg.valPeriod.time.absTime));
        } else if (id == CdmaSmsSubParameter::VALIDITY_PERIOD_RELATIVE) {
            msg.valPeriod.format = SMS_TIME_RELATIVE;
            parameter_.push_back(std::make_unique<CdmaSmsBaseParameter>(id, msg.valPeriod.time.relTime.time));
        } else if (id == CdmaSmsSubParameter::DEFERRED_DELIVERY_TIME_ABSOLUTE) {
            msg.deferValPeriod.format = SMS_TIME_ABSOLUTE;
            parameter_.push_back(std::make_unique<CdmaSmsAbsoluteTime>(id, msg.deferValPeriod.time.absTime));
        } else if (id == CdmaSmsSubParameter::DEFERRED_DELIVERY_TIME_RELATIVE) {
            msg.deferValPeriod.format = SMS_TIME_RELATIVE;
            parameter_.push_back(std::make_unique<CdmaSmsBaseParameter>(id, msg.deferValPeriod.time.relTime.time));
        } else if (id == CdmaSmsSubParameter::PRIORITY_INDICATOR) {
            parameter_.push_back(std::make_unique<CdmaSmsPriorityInd>(msg.priority));
        } else if (id == CdmaSmsSubParameter::PRIVACY_INDICATOR) {
            parameter_.push_back(std::make_unique<CdmaSmsPrivacyInd>(msg.privacy));
        } else if (id == CdmaSmsSubParameter::REPLY_OPTION) {
            parameter_.push_back(std::make_unique<CdmaSmsReplyOption>(msg.replyOpt));
        } else if (id == CdmaSmsSubParameter::ALERT_ON_MSG_DELIVERY) {
            parameter_.push_back(std::make_unique<CdmaSmsAlertPriority>(msg.alertPriority));
        } else if (id == CdmaSmsSubParameter::LANGUAGE_INDICATOR) {
            parameter_.push_back(std::make_unique<CdmaSmsLanguageInd>(msg.language));
        } else if (id == CdmaSmsSubParameter::CALLBACK_NUMBER) {
            parameter_.push_back(std::make_unique<CdmaSmsCallbackNumber>(msg.callbackNumber));
        } else if (id == CdmaSmsSubParameter::MESSAGE_DEPOSIT_INDEX) {
            parameter_.push_back(std::make_unique<CdmaSmsDepositIndex>(msg.depositId));
        } else {
            TELEPHONY_LOGW("sub parameter [%{public}d] not support", id);
            parameter_.push_back(std::make_unique<CdmaSmsReservedParameter>(id));
        }
        pdu.MoveForward(len);
    }
}

CdmaSmsCancelMessage::CdmaSmsCancelMessage(TeleserviceCancel &msg)
{
    type_ = CANCELLATION;
    // Message Identifier
    parameter_.push_back(std::make_unique<CdmaSmsMessageId>(msg.msgId, type_));
}

CdmaSmsDeliverReport::CdmaSmsDeliverReport(TeleserviceDeliverReport &msg)
{
    type_ = DELIVER_REPORT;
    // Message Identifier
    parameter_.push_back(std::make_unique<CdmaSmsMessageId>(msg.msgId, type_));
    // TP-Failure Cause
    if (msg.tpFailCause >= 0x80) {
        parameter_.push_back(
            std::make_unique<CdmaSmsBaseParameter>(CdmaSmsSubParameter::TP_FAILURE_CAUSE, msg.tpFailCause));
    }
    // User Data
    parameter_.push_back(std::make_unique<CdmaSmsUserData>(msg.userData, msg.msgId.headerInd));
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
    while ((parameter_.size() < SUB_PARAMETER_SIZE_MAX) && pdu.ReadByte(id) && pdu.ReadByte(len)) {
        if (isCMAS && id == CdmaSmsSubParameter::USER_DATA) {
            parameter_.push_back(std::make_unique<CdmaSmsCmasData>(msg.cmasData));
        } else {
            PushParameter(id, msg);
        }
        pdu.MoveForward(len);
    }
}

void CdmaSmsDeliverMessage::PushParameter(uint8_t id, TeleserviceDeliver &msg)
{
    if (id == CdmaSmsSubParameter::MESSAGE_IDENTIFIER) {
        parameter_.push_back(std::make_unique<CdmaSmsMessageId>(msg.msgId, type_));
    } else if (id == CdmaSmsSubParameter::USER_DATA) {
        parameter_.push_back(std::make_unique<CdmaSmsUserData>(msg.userData, msg.msgId.headerInd));
    } else if (id == CdmaSmsSubParameter::MESSAGE_CENTER_TIME_STAMP) {
        parameter_.push_back(std::make_unique<CdmaSmsAbsoluteTime>(id, msg.timeStamp));
    } else if (id == CdmaSmsSubParameter::VALIDITY_PERIOD_ABSOLUTE) {
        msg.valPeriod.format = SMS_TIME_ABSOLUTE;
        parameter_.push_back(std::make_unique<CdmaSmsAbsoluteTime>(id, msg.valPeriod.time.absTime));
    } else if (id == CdmaSmsSubParameter::VALIDITY_PERIOD_RELATIVE) {
        msg.valPeriod.format = SMS_TIME_RELATIVE;
        parameter_.push_back(std::make_unique<CdmaSmsBaseParameter>(id, msg.valPeriod.time.relTime.time));
    } else if (id == CdmaSmsSubParameter::PRIORITY_INDICATOR) {
        parameter_.push_back(std::make_unique<CdmaSmsPriorityInd>(msg.priority));
    } else if (id == CdmaSmsSubParameter::PRIVACY_INDICATOR) {
        parameter_.push_back(std::make_unique<CdmaSmsPrivacyInd>(msg.privacy));
    } else if (id == CdmaSmsSubParameter::REPLY_OPTION) {
        parameter_.push_back(std::make_unique<CdmaSmsReplyOption>(msg.replyOpt));
    } else if (id == CdmaSmsSubParameter::NUMBER_OF_MESSAGES) {
        parameter_.push_back(std::make_unique<CdmaSmsNumberMessages>(msg.numMsg));
    } else if (id == CdmaSmsSubParameter::ALERT_ON_MSG_DELIVERY) {
        parameter_.push_back(std::make_unique<CdmaSmsAlertPriority>(msg.alertPriority));
    } else if (id == CdmaSmsSubParameter::LANGUAGE_INDICATOR) {
        parameter_.push_back(std::make_unique<CdmaSmsLanguageInd>(msg.language));
    } else if (id == CdmaSmsSubParameter::CALLBACK_NUMBER) {
        parameter_.push_back(std::make_unique<CdmaSmsCallbackNumber>(msg.callbackNumber));
    } else if (id == CdmaSmsSubParameter::MESSAGE_DISPLAY_MODE) {
        parameter_.push_back(std::make_unique<CdmaSmsDisplayMode>(msg.displayMode));
    } else if (id == CdmaSmsSubParameter::MESSAGE_DEPOSIT_INDEX) {
        parameter_.push_back(std::make_unique<CdmaSmsDepositIndex>(msg.depositId));
    } else if (id == CdmaSmsSubParameter::ENHANCED_VMN) {
        parameter_.push_back(std::make_unique<CdmaSmsEnhancedVmn>(msg.vmn));
    } else if (id == CdmaSmsSubParameter::ENHANCED_VMN_ACK) {
        parameter_.push_back(std::make_unique<CdmaSmsEnhancedVmnAck>(msg.vmnAck));
    } else {
        parameter_.push_back(std::make_unique<CdmaSmsReservedParameter>(id));
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
    while ((parameter_.size() < SUB_PARAMETER_SIZE_MAX) && pdu.ReadByte(id) && pdu.ReadByte(len)) {
        if (id == CdmaSmsSubParameter::MESSAGE_IDENTIFIER) {
            parameter_.push_back(std::make_unique<CdmaSmsMessageId>(msg.msgId, type_));
        } else if (id == CdmaSmsSubParameter::USER_DATA) {
            parameter_.push_back(std::make_unique<CdmaSmsUserData>(msg.userData, msg.msgId.headerInd));
        } else if (id == CdmaSmsSubParameter::MESSAGE_CENTER_TIME_STAMP) {
            parameter_.push_back(std::make_unique<CdmaSmsAbsoluteTime>(id, msg.timeStamp));
        } else if (id == CdmaSmsSubParameter::MESSAGE_STATUS) {
            parameter_.push_back(std::make_unique<CdmaSmsMessageStatus>(msg.msgStatus));
        } else {
            TELEPHONY_LOGW("sub parameter [%{public}d] not support", id);
            parameter_.push_back(std::make_unique<CdmaSmsReservedParameter>(id));
        }
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
    while ((parameter_.size() < SUB_PARAMETER_SIZE_MAX) && pdu.ReadByte(id) && pdu.ReadByte(len)) {
        if (id == CdmaSmsSubParameter::MESSAGE_IDENTIFIER) {
            parameter_.push_back(std::make_unique<CdmaSmsMessageId>(msg.msgId, type_));
        } else if (id == CdmaSmsSubParameter::USER_DATA) {
            parameter_.push_back(std::make_unique<CdmaSmsUserData>(msg.userData, msg.msgId.headerInd));
        } else if (id == CdmaSmsSubParameter::USER_RESPONSE_CODE) {
            parameter_.push_back(std::make_unique<CdmaSmsBaseParameter>(id, msg.respCode));
        } else if (id == CdmaSmsSubParameter::MESSAGE_CENTER_TIME_STAMP) {
            parameter_.push_back(std::make_unique<CdmaSmsAbsoluteTime>(id, msg.timeStamp));
        } else if (id == CdmaSmsSubParameter::MESSAGE_DEPOSIT_INDEX) {
            parameter_.push_back(std::make_unique<CdmaSmsDepositIndex>(msg.depositId));
        } else {
            TELEPHONY_LOGW("sub parameter [%{public}d] not support", id);
            parameter_.push_back(std::make_unique<CdmaSmsReservedParameter>(id));
        }
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
    while ((parameter_.size() < SUB_PARAMETER_SIZE_MAX) && pdu.ReadByte(id) && pdu.ReadByte(len)) {
        if (id == CdmaSmsSubParameter::MESSAGE_IDENTIFIER) {
            parameter_.push_back(std::make_unique<CdmaSmsMessageId>(msg.msgId, type_));
        } else if (id == CdmaSmsSubParameter::USER_DATA) {
            parameter_.push_back(std::make_unique<CdmaSmsUserData>(msg.userData, msg.msgId.headerInd));
        } else if (id == CdmaSmsSubParameter::MESSAGE_CENTER_TIME_STAMP) {
            parameter_.push_back(std::make_unique<CdmaSmsAbsoluteTime>(id, msg.timeStamp));
        } else if (id == CdmaSmsSubParameter::MESSAGE_DEPOSIT_INDEX) {
            parameter_.push_back(std::make_unique<CdmaSmsDepositIndex>(msg.depositId));
        } else {
            TELEPHONY_LOGW("Sub Parameter ID[%{public}d] not support", id);
            parameter_.push_back(std::make_unique<CdmaSmsReservedParameter>(id));
        }
        pdu.MoveForward(len);
    }
}

} // namespace Telephony
} // namespace OHOS
