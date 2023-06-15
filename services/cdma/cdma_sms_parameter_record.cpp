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

#include "cdma_sms_parameter_record.h"

#include "securec.h"
#include "sms_common_utils.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
bool CdmaSmsParameterRecord::IsInvalidPdu(SmsReadBuffer &pdu)
{
    uint8_t id = RESERVED;
    if (pdu.IsEmpty() || !pdu.ReadByte(id) || !pdu.ReadByte(len_) || id != id_) {
        TELEPHONY_LOGE("parameter record: pdu is invalid[%{public}d]: [%{public}d]", id_, id);
        return true;
    }
    return false;
}

CdmaSmsTeleserviceId::CdmaSmsTeleserviceId(uint16_t &id) : teleserviceId_(id)
{
    id_ = TELESERVICE_ID;
    len_ = sizeof(teleserviceId_);
}

bool CdmaSmsTeleserviceId::Encode(SmsWriteBuffer &pdu)
{
    if (pdu.IsEmpty()) {
        TELEPHONY_LOGE("pdu is empty");
        return false;
    }

    if (pdu.WriteByte(id_) && pdu.WriteByte(len_) && pdu.WriteWord(teleserviceId_)) {
        return true;
    }

    TELEPHONY_LOGE("encode error");
    return false;
}

bool CdmaSmsTeleserviceId::Decode(SmsReadBuffer &pdu)
{
    if (IsInvalidPdu(pdu)) {
        TELEPHONY_LOGE("invalid pdu");
        return false;
    }
    return pdu.ReadWord(teleserviceId_);
}

CdmaSmsServiceCategory::CdmaSmsServiceCategory(uint16_t &cat) : serviceCat_(cat)
{
    id_ = SERVICE_CATEGORY;
    len_ = sizeof(serviceCat_);
}

bool CdmaSmsServiceCategory::Encode(SmsWriteBuffer &pdu)
{
    if (pdu.IsEmpty()) {
        TELEPHONY_LOGE("pdu is empty");
        return false;
    }

    if (pdu.WriteByte(id_) && pdu.WriteByte(len_) && pdu.WriteWord(serviceCat_)) {
        return true;
    }

    TELEPHONY_LOGE("encode error");
    return false;
}

bool CdmaSmsServiceCategory::Decode(SmsReadBuffer &pdu)
{
    if (IsInvalidPdu(pdu)) {
        TELEPHONY_LOGE("invalid pdu");
        return false;
    }
    return pdu.ReadWord(serviceCat_);
}

CdmaSmsBearerReply::CdmaSmsBearerReply(uint8_t &replySeq) : replySeq_(replySeq)
{
    id_ = BEARER_REPLY_OPTION;
    len_ = sizeof(replySeq_);
}

bool CdmaSmsBearerReply::Encode(SmsWriteBuffer &pdu)
{
    if (pdu.IsEmpty()) {
        TELEPHONY_LOGE("pdu is empty");
        return false;
    }

    if (pdu.WriteByte(id_) && pdu.WriteByte(len_) && pdu.WriteBits(replySeq_, BIT6)) {
        pdu.SkipBits();
        return true;
    }

    TELEPHONY_LOGE("encode error");
    return false;
}

bool CdmaSmsBearerReply::Decode(SmsReadBuffer &pdu)
{
    if (IsInvalidPdu(pdu)) {
        TELEPHONY_LOGE("invalid pdu");
        return false;
    }
    if (pdu.ReadBits(replySeq_, BIT6)) {
        pdu.SkipBits();
        return true;
    }
    TELEPHONY_LOGE("decode error");
    return false;
}

CdmaSmsCauseCodes::CdmaSmsCauseCodes(TransportCauseCode &code) : code_(code)
{
    id_ = CAUSE_CODES;
}

bool CdmaSmsCauseCodes::Encode(SmsWriteBuffer &pdu)
{
    if (pdu.IsEmpty()) {
        TELEPHONY_LOGE("pdu is empty");
        return false;
    }

    if (!pdu.WriteByte(id_)) {
        TELEPHONY_LOGE("id write error");
        return false;
    }
    uint16_t lenIndex = pdu.MoveForward();
    if (!pdu.WriteBits(code_.transReplySeq, BIT6) || !pdu.WriteBits(static_cast<uint8_t>(code_.errorClass), BIT2)) {
        TELEPHONY_LOGE("replyseq or errorclass write error");
        return false;
    }
    if (code_.errorClass != TransportErrClass::NONE) {
        if (!pdu.WriteBits(static_cast<uint8_t>(code_.causeCode), BIT8)) {
            TELEPHONY_LOGE("cause code write error");
            return false;
        }
    }
    len_ = pdu.SkipBits() - lenIndex - 1;
    return pdu.InsertByte(len_, lenIndex);
}

bool CdmaSmsCauseCodes::Decode(SmsReadBuffer &pdu)
{
    if (IsInvalidPdu(pdu)) {
        TELEPHONY_LOGE("invalid pdu");
        return false;
    }

    if (!pdu.ReadBits(code_.transReplySeq, BIT6)) {
        TELEPHONY_LOGE("replyseq read error");
        return false;
    }
    uint8_t v = 0;
    if (!pdu.ReadBits(v, BIT2)) {
        TELEPHONY_LOGE("error class read error");
        return false;
    }
    if (v == NONE) {
        code_.errorClass = TransportErrClass::NONE;
    } else if (v == TEMPORARY) {
        code_.errorClass = TransportErrClass::TEMPORARY;
    } else if (v == PERMANENT) {
        code_.errorClass = TransportErrClass::PERMANENT;
    }
    if (code_.errorClass != TransportErrClass::NONE) {
        if (!pdu.ReadBits(v, BIT8)) {
            TELEPHONY_LOGE("cause code read error");
            return false;
        }
        code_.causeCode = TransportCauseCodeType(v);
    }
    pdu.SkipBits();
    return true;
}

CdmaSmsAddressParameter::CdmaSmsAddressParameter(TransportAddr &address, uint8_t id) {}

bool CdmaSmsAddressParameter::Encode(SmsWriteBuffer &pdu)
{
    return false;
}

bool CdmaSmsAddressParameter::Decode(SmsReadBuffer &pdu)
{
    return false;
}

CdmaSmsSubaddress::CdmaSmsSubaddress(TransportSubAddr &address, uint8_t id) {}

bool CdmaSmsSubaddress::Encode(SmsWriteBuffer &pdu)
{
    return false;
}

bool CdmaSmsSubaddress::Decode(SmsReadBuffer &pdu)
{
    return false;
}

CdmaSmsBearerData::CdmaSmsBearerData(CdmaTeleserviceMsg &msg) {}

CdmaSmsBearerData::CdmaSmsBearerData(CdmaTeleserviceMsg &msg, SmsReadBuffer &pdu, bool isCMAS) {}

bool CdmaSmsBearerData::Encode(SmsWriteBuffer &pdu)
{
    return false;
}

bool CdmaSmsBearerData::Decode(SmsReadBuffer &pdu)
{
    return false;
}

} // namespace Telephony
} // namespace OHOS
