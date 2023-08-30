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

CdmaSmsAddressParameter::CdmaSmsAddressParameter(TransportAddr &address, uint8_t id) : address_(address)
{
    if (id == ORG_ADDRESS || id == DEST_ADDRESS) {
        id_ = id;
    } else {
        isInvalid_ = true;
        TELEPHONY_LOGE("invalid ID[%{public}d]", id);
    }
}

bool CdmaSmsAddressParameter::Encode(SmsWriteBuffer &pdu)
{
    if (isInvalid_ || pdu.IsEmpty()) {
        TELEPHONY_LOGE("invalid ID or pdu");
        return false;
    }
    if (!pdu.WriteByte(id_)) {
        TELEPHONY_LOGE("id write error");
        return false;
    }
    uint16_t lenIndex = pdu.MoveForward();
    if (!pdu.WriteBits(address_.digitMode ? 0b1 : 0b0) || !pdu.WriteBits(address_.numberMode ? 0b1 : 0b0)) {
        TELEPHONY_LOGE("digit mode or number mode write error");
        return false;
    }
    if (address_.digitMode) {
        if (!pdu.WriteBits(address_.numberType, BIT3)) {
            TELEPHONY_LOGE("number type write error");
            return false;
        }
        if (!address_.numberMode && !pdu.WriteBits(address_.numberPlan, BIT4)) {
            TELEPHONY_LOGE("number plan write error");
            return false;
        }
    }
    if (!EncodeAddress(pdu)) {
        TELEPHONY_LOGE("encode address error");
        return false;
    }
    len_ = pdu.SkipBits() - lenIndex - 1;
    return pdu.InsertByte(len_, lenIndex);
}

bool CdmaSmsAddressParameter::Decode(SmsReadBuffer &pdu)
{
    if (isInvalid_ || IsInvalidPdu(pdu)) {
        TELEPHONY_LOGE("invalid ID or pdu");
        return false;
    }
    if (memset_s(&address_, sizeof(address_), 0x0, sizeof(address_)) != EOK) {
        TELEPHONY_LOGE("memset_s error");
        return false;
    }

    uint8_t v2 = 0;
    uint8_t v1 = 0;
    if (!pdu.ReadBits(v1) || !pdu.ReadBits(v2)) {
        TELEPHONY_LOGE("digit mode or number mode read error");
        return false;
    }
    address_.digitMode = (v1 == 0b1) ? true : false;
    address_.numberMode = (v2 == 0b1) ? true : false;
    if (address_.digitMode) {
        if (!pdu.ReadBits(address_.numberType, BIT3)) {
            TELEPHONY_LOGE("number type read error");
            return false;
        }
        if (!address_.numberMode && !pdu.ReadBits(address_.numberPlan, BIT4)) {
            TELEPHONY_LOGE("number plan read error");
            return false;
        }
    }
    if (!DecodeAddress(pdu)) {
        TELEPHONY_LOGE("decode address error");
        return false;
    }
    pdu.SkipBits();
    return true;
}

bool CdmaSmsAddressParameter::EncodeAddress(SmsWriteBuffer &pdu)
{
    if (!pdu.WriteBits(address_.addrLen, BIT8)) {
        TELEPHONY_LOGE("addlen write error");
        return false;
    }
    if (static_cast<unsigned long>(address_.addrLen) > (sizeof(address_.szData) / sizeof(address_.szData[0]))) {
        TELEPHONY_LOGE("address length error");
        return false;
    }
    if (address_.digitMode) {
        for (uint8_t i = 0; i < address_.addrLen; i++) {
            if (!pdu.WriteBits(address_.szData[i], BIT8)) {
                TELEPHONY_LOGE("address write error");
                return false;
            }
        }
    } else {
        for (uint8_t i = 0; i < address_.addrLen; i++) {
            if (!pdu.WriteBits(SmsCommonUtils::DigitToDtmfChar(address_.szData[i]), BIT4)) {
                TELEPHONY_LOGE("address write error");
                return false;
            }
        }
    }
    return true;
}

bool CdmaSmsAddressParameter::DecodeAddress(SmsReadBuffer &pdu)
{
    uint8_t v1 = 0;
    if (!pdu.ReadBits(v1, BIT8)) {
        TELEPHONY_LOGE("addrlen read error");
        return false;
    }
    address_.addrLen = v1;
    if (static_cast<unsigned long>(address_.addrLen) > (sizeof(address_.szData) / sizeof(address_.szData[0]))) {
        TELEPHONY_LOGE("address length error");
        return false;
    }
    if (address_.digitMode) {
        for (uint8_t i = 0; i < address_.addrLen; i++) {
            if (!pdu.ReadBits(v1, BIT8)) {
                TELEPHONY_LOGE("address read error");
                return false;
            }
            address_.szData[i] = v1;
        }
    } else {
        for (uint8_t i = 0; i < address_.addrLen; i++) {
            if (!pdu.ReadBits(v1, BIT4)) {
                TELEPHONY_LOGE("address read error");
                return false;
            }
            address_.szData[i] = SmsCommonUtils::DtmfCharToDigit(v1);
        }
    }
    return true;
}

CdmaSmsSubaddress::CdmaSmsSubaddress(TransportSubAddr &address, uint8_t id) : address_(address)
{
    if (id == ORG_SUB_ADDRESS || id == DEST_SUB_ADDRESS) {
        id_ = id;
    } else {
        isInvalid_ = true;
        TELEPHONY_LOGE("invalid ID[%{public}d]", id);
    }
}

bool CdmaSmsSubaddress::Encode(SmsWriteBuffer &pdu)
{
    if (isInvalid_ || pdu.IsEmpty()) {
        TELEPHONY_LOGE("invalid ID or pdu");
        return false;
    }

    if (!pdu.WriteByte(id_)) {
        TELEPHONY_LOGE("id write error");
        return false;
    }
    uint16_t lenIndex = pdu.MoveForward();
    if (!pdu.WriteBits(static_cast<uint8_t>(address_.type), BIT3) || !pdu.WriteBits(address_.odd ? 0b1 : 0b0) ||
        !pdu.WriteBits(address_.addrLen, BIT8)) {
        TELEPHONY_LOGE("type, odd or addrlen write error");
        return false;
    }

    if (static_cast<unsigned long>(address_.addrLen) > (sizeof(address_.szData) / sizeof(address_.szData[0]))) {
        TELEPHONY_LOGE("address length error");
        return false;
    }

    for (uint8_t i = 0; i < address_.addrLen; i++) {
        if (!pdu.WriteBits(address_.szData[i], BIT8)) {
            TELEPHONY_LOGE("address write error");
            return false;
        }
    }
    len_ = pdu.SkipBits() - lenIndex - 1;
    return pdu.InsertByte(len_, lenIndex);
}

bool CdmaSmsSubaddress::Decode(SmsReadBuffer &pdu)
{
    if (isInvalid_ || IsInvalidPdu(pdu)) {
        TELEPHONY_LOGE("invalid ID or pdu");
        return false;
    }
    if (memset_s(&address_, sizeof(address_), 0x0, sizeof(address_)) != EOK) {
        TELEPHONY_LOGE("memset_s error");
        return false;
    }

    uint8_t v1 = 0;
    uint8_t v2 = 0;
    uint8_t v3 = 0;
    if (!pdu.ReadBits(v1, BIT3) || !pdu.ReadBits(v2) || !pdu.ReadBits(v3, BIT8)) {
        TELEPHONY_LOGE("type, odd or addrlen read error");
        return false;
    }
    if (v1 == NSAP) {
        address_.type = TransportSubAddrType::NSAP;
    } else if (v1 == USER) {
        address_.type = TransportSubAddrType::USER;
    } else {
        address_.type = TransportSubAddrType::RESERVED;
    }
    address_.odd = (v2 == 0b1) ? true : false;
    address_.addrLen = v3;
    if (static_cast<unsigned long>(address_.addrLen) > (sizeof(address_.szData) / sizeof(address_.szData[0]))) {
        TELEPHONY_LOGE("address length error");
        return false;
    }
    for (uint8_t i = 0; i < address_.addrLen; i++) {
        if (!pdu.ReadBits(v1, BIT8)) {
            TELEPHONY_LOGE("address read error");
            return false;
        }
        address_.szData[i] = v1;
    }
    pdu.SkipBits();
    return true;
}

CdmaSmsBearerData::CdmaSmsBearerData(CdmaTeleserviceMsg &msg)
{
    id_ = BEARER_DATA;
    if (msg.type == TeleserviceMsgType::SUBMIT) {
        teleserviceMessage_ = std::make_unique<CdmaSmsSubmitMessage>(msg.data.submit);
    } else if (msg.type == TeleserviceMsgType::CANCEL) {
        teleserviceMessage_ = std::make_unique<CdmaSmsCancelMessage>(msg.data.cancel);
    } else if (msg.type == TeleserviceMsgType::DELIVER_REPORT) {
        teleserviceMessage_ = std::make_unique<CdmaSmsDeliverReport>(msg.data.report);
    } else {
        TELEPHONY_LOGE("no matching type[%{public}d]", static_cast<uint8_t>(msg.type));
    }
}

CdmaSmsBearerData::CdmaSmsBearerData(CdmaTeleserviceMsg &msg, SmsReadBuffer &pdu, bool isCMAS)
{
    id_ = BEARER_DATA;
    // teleservice pdu, does not include id and length
    uint8_t index = pdu.GetIndex();
    uint8_t type = CdmaSmsTeleserviceMessage::GetMessageType(pdu);
    pdu.SetIndex(index);
    if (type == CdmaSmsTeleserviceMessage::DELIVER) {
        msg.type = TeleserviceMsgType::DELIVER;
        teleserviceMessage_ = std::make_unique<CdmaSmsDeliverMessage>(msg.data.deliver, pdu, isCMAS);
    } else if (type == CdmaSmsTeleserviceMessage::SUBMIT) {
        msg.type = TeleserviceMsgType::SUBMIT;
        teleserviceMessage_ = std::make_unique<CdmaSmsSubmitMessage>(msg.data.submit, pdu);
    } else if (type == CdmaSmsTeleserviceMessage::DELIVERY_ACK) {
        msg.type = TeleserviceMsgType::DELIVERY_ACK;
        teleserviceMessage_ = std::make_unique<CdmaSmsDeliveryAck>(msg.data.deliveryAck, pdu);
    } else if (type == CdmaSmsTeleserviceMessage::USER_ACK) {
        msg.type = TeleserviceMsgType::USER_ACK;
        teleserviceMessage_ = std::make_unique<CdmaSmsUserAck>(msg.data.userAck, pdu);
    } else if (type == CdmaSmsTeleserviceMessage::READ_ACK) {
        msg.type = TeleserviceMsgType::READ_ACK;
        teleserviceMessage_ = std::make_unique<CdmaSmsReadAck>(msg.data.readAck, pdu);
    } else {
        msg.type = TeleserviceMsgType::RESERVED;
        TELEPHONY_LOGE("no matching type[%{public}d]", type);
    }
}

bool CdmaSmsBearerData::Encode(SmsWriteBuffer &pdu)
{
    if (pdu.IsEmpty()) {
        TELEPHONY_LOGE("pdu is empty");
        return false;
    }
    if (teleserviceMessage_ == nullptr) {
        TELEPHONY_LOGE("teleservice message is null");
        return false;
    }

    if (!pdu.WriteByte(id_)) {
        TELEPHONY_LOGE("id write error");
        return false;
    }
    uint16_t lenIndex = pdu.MoveForward();
    if (!teleserviceMessage_->Encode(pdu)) {
        TELEPHONY_LOGE("teleservice message encode error");
        return false;
    }
    len_ = pdu.GetIndex() - lenIndex - 1;
    return pdu.InsertByte(len_, lenIndex);
}

bool CdmaSmsBearerData::Decode(SmsReadBuffer &pdu)
{
    if (teleserviceMessage_ == nullptr) {
        TELEPHONY_LOGE("Teleservice message is null");
        return false;
    }
    if (IsInvalidPdu(pdu)) {
        TELEPHONY_LOGE("invalid pdu");
        return false;
    }

    return teleserviceMessage_->Decode(pdu);
}

} // namespace Telephony
} // namespace OHOS
