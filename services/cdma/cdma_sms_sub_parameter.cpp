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

#include "cdma_sms_sub_parameter.h"

#include "gsm_sms_udata_codec.h"
#include "securec.h"
#include "sms_common_utils.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
static constexpr uint8_t DECIMAL_NUM = 10;
static constexpr uint8_t MESSAGE_ID_LEN = 3;
static constexpr uint8_t ABSOLUTE_TIME_LEN = 6;

bool CdmaSmsSubParameter::IsInvalidPdu(SmsReadBuffer &pdu)
{
    uint8_t id = RESERVED;
    if (pdu.IsEmpty() || !pdu.ReadByte(id) || !pdu.ReadByte(len_) || id != id_) {
        TELEPHONY_LOGE("sub parameter: pdu is invalid[%{public}d]: [%{public}d]", id_, id);
        return true;
    }
    return false;
}

CdmaSmsBaseParameter::CdmaSmsBaseParameter(uint8_t id, uint8_t &data) : data_(data)
{
    id_ = id;
    len_ = sizeof(id_);
}

bool CdmaSmsBaseParameter::Encode(SmsWriteBuffer &pdu)
{
    if (pdu.IsEmpty()) {
        TELEPHONY_LOGE("pdu is empty");
        return false;
    }
    if (pdu.WriteByte(id_) && pdu.WriteByte(len_) && pdu.WriteByte(data_)) {
        return true;
    }
    TELEPHONY_LOGE("encode error");
    return false;
}

bool CdmaSmsBaseParameter::Decode(SmsReadBuffer &pdu)
{
    if (IsInvalidPdu(pdu)) {
        TELEPHONY_LOGE("invalid pdu");
        return false;
    }
    return pdu.ReadByte(data_);
}

CdmaSmsReservedParameter::CdmaSmsReservedParameter(uint8_t id)
{
    id_ = id;
}

bool CdmaSmsReservedParameter::Encode(SmsWriteBuffer &pdu)
{
    if (pdu.IsEmpty()) {
        TELEPHONY_LOGE("pdu is empty");
        return false;
    }
    return true;
}

bool CdmaSmsReservedParameter::Decode(SmsReadBuffer &pdu)
{
    if (IsInvalidPdu(pdu)) {
        TELEPHONY_LOGE("invalid pdu");
        return false;
    }
    pdu.MoveForward(len_);
    return true;
}

CdmaSmsMessageId::CdmaSmsMessageId(SmsTeleSvcMsgId &msgId, uint8_t type) : msgId_(msgId)
{
    id_ = MESSAGE_IDENTIFIER;
    type_ = type; // 4 bits
    len_ = MESSAGE_ID_LEN;
}

bool CdmaSmsMessageId::Encode(SmsWriteBuffer &pdu)
{
    if (pdu.IsEmpty()) {
        TELEPHONY_LOGE("pdu is empty");
        return false;
    }

    if (pdu.WriteByte(id_) && pdu.WriteByte(len_)) {
        if (pdu.WriteBits(type_, BIT4) && pdu.WriteBits(msgId_.msgId >> BIT8, BIT8) &&
            pdu.WriteBits(msgId_.msgId, BIT8) && pdu.WriteBits(msgId_.headerInd ? 0b1 : 0b0, BIT1)) {
            pdu.SkipBits();
            return true;
        }
    }

    TELEPHONY_LOGE("encode error");
    return false;
}

bool CdmaSmsMessageId::Decode(SmsReadBuffer &pdu)
{
    if (IsInvalidPdu(pdu)) {
        TELEPHONY_LOGE("invalid pdu");
        return false;
    }

    if (!pdu.ReadBits(type_, BIT4)) {
        TELEPHONY_LOGE("type read error");
        return false;
    }
    uint8_t vh = 0;
    uint8_t v = 0;
    if (!pdu.ReadBits(vh, BIT8) || !pdu.ReadBits(v, BIT8)) {
        TELEPHONY_LOGE("msgid read error");
        return false;
    }
    msgId_.msgId = (vh << BIT8) | v;

    if (!pdu.ReadBits(v, BIT1)) {
        TELEPHONY_LOGE("header ind read error");
        return false;
    }
    msgId_.headerInd = v == 0b1 ? true : false;

    pdu.SkipBits();
    return true;
}

uint8_t CdmaSmsMessageId::GetMessageType()
{
    return type_;
}

CdmaSmsAbsoluteTime::CdmaSmsAbsoluteTime(uint8_t id, SmsTimeAbs &time) : time_(time)
{
    id_ = id;
    len_ = ABSOLUTE_TIME_LEN;
}

bool CdmaSmsAbsoluteTime::Encode(SmsWriteBuffer &pdu)
{
    if (pdu.IsEmpty()) {
        TELEPHONY_LOGE("pdu is empty");
        return false;
    }

    if (pdu.WriteByte(id_) && pdu.WriteByte(len_)) {
        if (pdu.WriteByte(EncodeBCD(time_.year)) && pdu.WriteByte(EncodeBCD(time_.month)) &&
            pdu.WriteByte(EncodeBCD(time_.day)) && pdu.WriteByte(EncodeBCD(time_.hour)) &&
            pdu.WriteByte(EncodeBCD(time_.minute)) && pdu.WriteByte(EncodeBCD(time_.second))) {
            return true;
        }
    }

    TELEPHONY_LOGE("encode error");
    return false;
}

bool CdmaSmsAbsoluteTime::Decode(SmsReadBuffer &pdu)
{
    if (IsInvalidPdu(pdu)) {
        TELEPHONY_LOGE("invalid pdu");
        return false;
    }
    uint8_t v1 = 0;
    uint8_t v2 = 0;
    uint8_t v3 = 0;
    uint8_t v4 = 0;
    uint8_t v5 = 0;
    uint8_t v6 = 0;
    if (!pdu.ReadByte(v1) || !pdu.ReadByte(v2) || !pdu.ReadByte(v3) || !pdu.ReadByte(v4) || !pdu.ReadByte(v5) ||
        !pdu.ReadByte(v6)) {
        TELEPHONY_LOGE("read error");
        return false;
    }
    time_.year = DecodeBCD(v1);
    time_.month = DecodeBCD(v2);
    time_.day = DecodeBCD(v3);
    time_.hour = DecodeBCD(v4);
    time_.minute = DecodeBCD(v5);
    time_.second = DecodeBCD(v6);
    return true;
}

uint8_t CdmaSmsAbsoluteTime::EncodeBCD(const uint8_t v)
{
    return ((v / DECIMAL_NUM) << BIT4) | (v % DECIMAL_NUM);
}

uint8_t CdmaSmsAbsoluteTime::DecodeBCD(const uint8_t v)
{
    return ((v & 0xf0) >> BIT4) * DECIMAL_NUM + (v & 0x0f);
}

CdmaSmsPriorityInd::CdmaSmsPriorityInd(SmsPriorityIndicator &priority) : priority_(priority)
{
    id_ = PRIORITY_INDICATOR;
    len_ = sizeof(uint8_t);
}

bool CdmaSmsPriorityInd::Encode(SmsWriteBuffer &pdu)
{
    if (pdu.IsEmpty()) {
        TELEPHONY_LOGE("pdu is empty");
        return false;
    }

    if (pdu.WriteByte(id_) && pdu.WriteByte(len_)) {
        if (pdu.WriteBits(static_cast<uint8_t>(priority_), BIT2)) {
            pdu.SkipBits();
            return true;
        }
    }

    TELEPHONY_LOGE("encode error");
    return false;
}

bool CdmaSmsPriorityInd::Decode(SmsReadBuffer &pdu)
{
    if (IsInvalidPdu(pdu)) {
        TELEPHONY_LOGE("invalid pdu");
        return false;
    }

    uint8_t v = 0;
    if (!pdu.ReadBits(v, BIT2)) {
        TELEPHONY_LOGE("read error");
        return false;
    }
    pdu.SkipBits();
    priority_ = SmsPriorityIndicator(v);
    return true;
}

CdmaSmsPrivacyInd::CdmaSmsPrivacyInd(SmsPrivacyIndicator &privacy) : privacy_(privacy)
{
    id_ = PRIVACY_INDICATOR;
    len_ = sizeof(uint8_t);
}

bool CdmaSmsPrivacyInd::Encode(SmsWriteBuffer &pdu)
{
    if (pdu.IsEmpty()) {
        TELEPHONY_LOGE("pdu is empty");
        return false;
    }

    if (pdu.WriteByte(id_) && pdu.WriteByte(len_)) {
        if (pdu.WriteBits(static_cast<uint8_t>(privacy_), BIT2)) {
            pdu.SkipBits();
            return true;
        }
    }

    TELEPHONY_LOGE("encode error");
    return false;
}

bool CdmaSmsPrivacyInd::Decode(SmsReadBuffer &pdu)
{
    if (IsInvalidPdu(pdu)) {
        TELEPHONY_LOGE("invalid pdu");
        return false;
    }

    uint8_t v = 0;
    if (!pdu.ReadBits(v, BIT2)) {
        TELEPHONY_LOGE("read error");
        return false;
    }
    pdu.SkipBits();
    privacy_ = SmsPrivacyIndicator(v);
    return true;
}

CdmaSmsReplyOption::CdmaSmsReplyOption(SmsReplyOption &replyOpt) : replyOpt_(replyOpt)
{
    id_ = REPLY_OPTION;
    len_ = sizeof(uint8_t);
}

bool CdmaSmsReplyOption::Encode(SmsWriteBuffer &pdu)
{
    if (pdu.IsEmpty()) {
        TELEPHONY_LOGE("pdu is empty");
        return false;
    }

    if (pdu.WriteByte(id_) && pdu.WriteByte(len_)) {
        if (pdu.WriteBits(replyOpt_.userAck ? 0b1 : 0b0, BIT1) && pdu.WriteBits(replyOpt_.dak ? 0b1 : 0b0, BIT1) &&
            pdu.WriteBits(replyOpt_.readAck ? 0b1 : 0b0, BIT1) && pdu.WriteBits(replyOpt_.report ? 0b1 : 0b0, BIT1)) {
            pdu.SkipBits();
            return true;
        }
    }

    TELEPHONY_LOGE("encode error");
    return false;
}

bool CdmaSmsReplyOption::Decode(SmsReadBuffer &pdu)
{
    if (IsInvalidPdu(pdu)) {
        TELEPHONY_LOGE("invalid pdu");
        return false;
    }

    uint8_t v1 = 0;
    uint8_t v2 = 0;
    uint8_t v3 = 0;
    uint8_t v4 = 0;
    if (!pdu.ReadBits(v1, BIT1) || !pdu.ReadBits(v2, BIT1) || !pdu.ReadBits(v3, BIT1) || !pdu.ReadBits(v4, BIT1)) {
        TELEPHONY_LOGE("read error");
        return false;
    }
    replyOpt_.userAck = (v1 == 0b1) ? true : false;
    replyOpt_.dak = (v2 == 0b1) ? true : false;
    replyOpt_.readAck = (v3 == 0b1) ? true : false;
    replyOpt_.report = (v4 == 0b1) ? true : false;
    pdu.SkipBits();
    return true;
}

} // namespace Telephony
} // namespace OHOS
