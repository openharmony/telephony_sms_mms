/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
 * Copyright (C) 2014 Samsung Electronics Co., Ltd. All rights reserved
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

#include "cdma_sms_pdu_codec.h"

#include "gsm_sms_udata_codec.h"
#include "securec.h"
#include "sms_common_utils.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
static constexpr uint8_t MSG_UDID_PARAM_LEN = 3;
static constexpr uint8_t BYTE_BITS = 8;
static constexpr uint8_t SHIFT_1BITS = 1;
static constexpr uint8_t SHIFT_2BITS = 2;
static constexpr uint8_t SHIFT_3BITS = 3;
static constexpr uint8_t SHIFT_4BITS = 4;
static constexpr uint8_t SHIFT_5BITS = 5;
static constexpr uint8_t SHIFT_6BITS = 6;
static constexpr uint8_t SHIFT_7BITS = 7;
static constexpr uint8_t SHIFT_8BITS = 8;

static constexpr uint8_t BYTE_STEP = 1;
static constexpr uint8_t MIN_PDU_LEN = 2;
static constexpr uint8_t HEX_BYTE_STEP = 2;
static constexpr uint8_t MAX_MSG_ID_LEN = 5;
static constexpr uint8_t DECIMAL_NUM = 10;
static constexpr uint8_t ENCODE_GSM_BIT = 7;
static constexpr uint8_t ENCODE_BYTE_BIT = 7;
static constexpr uint8_t BYTE_BIT = 8;
static constexpr uint8_t MAX_TPDU_DATA_LEN = 255;
static constexpr uint8_t MIN_PDU_ABS_TIME_LEN = 6;

void CdmaSmsPduCodec::ShiftNBit(unsigned char *src, unsigned int nBytes, unsigned int nShiftBit)
{
    if (src == nullptr) {
        TELEPHONY_LOGE("Source is null!");
        return;
    }

    for (unsigned int index = 1; index < nBytes; index++) {
        if (BYTE_BITS < nShiftBit) {
            TELEPHONY_LOGE("ShiftNBit BYTE_BITS less than nShiftBit");
            return;
        }
        unsigned char temp = src[index] >> (BYTE_BITS - nShiftBit);
        src[index - 1] |= temp;
        src[index] = (unsigned int)src[index] << nShiftBit;
    }
}

void CdmaSmsPduCodec::ShiftRNBit(unsigned char *src, unsigned int nBytes, unsigned int nShiftBit)
{
    unsigned char bit = 0x00;
    if (src == nullptr) {
        TELEPHONY_LOGE("Source is null!");
        return;
    }
    for (unsigned int index = 0; index <= nBytes; index++) {
        unsigned char temp = src[index] >> (nShiftBit);
        temp |= bit;
        if (BYTE_BITS < nShiftBit) {
            TELEPHONY_LOGE("ShiftRNBit BYTE_BITS less than nShiftBit");
            return;
        }
        bit = ((unsigned int)src[index] << (BYTE_BITS - nShiftBit));
        src[index] = temp;
    }
}

void CdmaSmsPduCodec::ShiftNBitForDecode(unsigned char *src, unsigned int nBytes, unsigned int nShiftBit)
{
    if (src == nullptr) {
        TELEPHONY_LOGE("Source is null!");
        return;
    }

    for (unsigned int index = 0; index + 1 < nBytes; index++) {
        (src[index]) <<= nShiftBit;
        if (BYTE_BITS < nShiftBit) {
            TELEPHONY_LOGE("ShiftNBitForDecode BYTE_BITS less than nShiftBit");
            return;
        }
        src[index] |= (src[index + 1] >> (BYTE_BITS - nShiftBit));
    }
}

unsigned char CdmaSmsPduCodec::ConvertToBCD(const unsigned char val)
{
    return static_cast<unsigned char>((val / DECIMAL_NUM) << SHIFT_4BITS) | (val % DECIMAL_NUM);
}

unsigned char CdmaSmsPduCodec::DecodeDigitModeNumberType(const unsigned char val, bool isNumber)
{
    unsigned char temp = (val >> SHIFT_5BITS);
    if (isNumber) {
        if (temp >= SMS_TRANS_DNET_UNKNOWN && temp <= SMS_TRANS_DNET_RESERVED) {
            return temp;
        } else {
            return SMS_TRANS_DNET_RESERVED;
        }
    } else {
        if (temp >= SMS_NUMBER_TYPE_UNKNOWN && temp <= SMS_NUMBER_TYPE_RESERVED_7) {
            return temp;
        }
        return SMS_NUMBER_TYPE_UNKNOWN;
    }
}

unsigned char CdmaSmsPduCodec::DecodeDigitModeNumberPlan(const unsigned char val)
{
    unsigned char numberPlan = 0;
    switch (val) {
        case SMS_NPI_UNKNOWN:
            numberPlan = SMS_NPI_UNKNOWN;
            break;
        case SMS_NPI_ISDN:
            numberPlan = SMS_NPI_ISDN;
            break;
        case SMS_NPI_DATA:
            numberPlan = SMS_NPI_DATA;
            break;
        case SMS_NPI_TELEX:
            numberPlan = SMS_NPI_TELEX;
            break;
        case SMS_NPI_PRIVATE:
            numberPlan = SMS_NPI_PRIVATE;
            break;
        case SMS_NPI_RESERVED:
        default:
            numberPlan = SMS_NPI_RESERVED;
            break;
    }
    return numberPlan;
}

bool CdmaSmsPduCodec::CheckInvalidPDU(const std::vector<unsigned char> &pduStr)
{
    std::size_t offset = 0;
    if (pduStr.size() < MIN_PDU_LEN) {
        TELEPHONY_LOGE("Invalid PDU : pdu size less than 2 is invalid");
        return false;
    }
    if (!(pduStr[offset] == SMS_TRANS_P2P_MSG || pduStr[offset] == SMS_TRANS_BROADCAST_MSG ||
        pduStr[offset] == SMS_TRANS_ACK_MSG)) {
        TELEPHONY_LOGE("Invalid PDU : Message Type [%{public}2x]", pduStr[offset]);
        return false;
    }
    offset++;
    while (offset + 1 < pduStr.size()) {
        if (pduStr[offset] >= 0 && pduStr[offset] <= 0x08) {
            offset += (pduStr[offset + 1] + HEX_BYTE_STEP);
        } else {
            TELEPHONY_LOGE("Invalid PDU : Parameter ID [%{public}2x]", pduStr[offset]);
            return false;
        }
    }
    if (offset != pduStr.size()) {
        TELEPHONY_LOGE("Invalid PDU !");
        return false;
    }
    return true;
}

int CdmaSmsPduCodec::EncodeMsg(const struct SmsTransMsg &transMsg, unsigned char *pduStr, size_t pduStrLen)
{
    int encodeSize = 0;
    switch (transMsg.type) {
        case SMS_TRANS_P2P_MSG:
            encodeSize = EncodeP2PMsg(transMsg.data.p2pMsg, pduStr, pduStrLen);
            break;
        case SMS_TRANS_BROADCAST_MSG:
            encodeSize = EncodeCBMsg(transMsg.data.cbMsg, pduStr, pduStrLen);
            break;
        case SMS_TRANS_ACK_MSG:
            encodeSize = EncodeAckMsg(transMsg.data.ackMsg, pduStr, pduStrLen);
            break;
        default:
            break;
    }
    if (static_cast<size_t>(encodeSize) > pduStrLen) {
        TELEPHONY_LOGE("encodeSize should not more than pduStrLen");
        return 0;
    }

    return encodeSize;
}

int CdmaSmsPduCodec::EncodeP2PMsg(const struct SmsTransP2PMsg &p2pMsg, unsigned char *pduStr, size_t pduStrLen)
{
    int offset = 0;
    int index = 0;
    if (pduStr == nullptr || pduStrLen == 0) {
        TELEPHONY_LOGE("PDU is null!");
        return offset;
    }
    offset += EncodeP2PMsgTeleServiceId(p2pMsg, pduStr, pduStrLen);

    /* 2. Service category */
    offset += EncodeP2PMsgServiceCategory(p2pMsg, &pduStr[offset], pduStrLen);

    /* 3. Address */
    offset += EncodeAddress(p2pMsg.address, &pduStr[offset], pduStrLen);

    /* 4. Sub address (optional) */
    offset += EncodeSubAddress(p2pMsg.subAddress, &pduStr[offset], pduStrLen);

    /* 5. Bearer reply option (optional) */
    offset += EncodeP2PMsgReplyOption(p2pMsg, &pduStr[offset], pduStrLen);

    /* 6. Bearer data */
    if (static_cast<size_t>(offset) >= pduStrLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }
    pduStr[offset++] = SMS_TRANS_PARAM_BEARER_DATA;

    /* PARAMETER_LEN field should be filled at the last part. */
    if (static_cast<size_t>(offset) >= pduStrLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }
    index = offset;
    int encodeSize = EncodeTelesvcMsg(p2pMsg.telesvcMsg, &pduStr[offset], pduStrLen);
    /* PARAMETER_LEN */
    pduStr[index] = encodeSize;
    offset += encodeSize;
    return offset;
}

int CdmaSmsPduCodec::EncodeP2PMsgTeleServiceId(
    const struct SmsTransP2PMsg &p2pMsg, unsigned char *pduStr, size_t pduStrLen)
{
    int offset = 0;
    if (pduStr == nullptr || pduStrLen == 0) {
        TELEPHONY_LOGE("PDU is null!");
        return offset;
    }

    pduStr[offset++] = SMS_TRANS_P2P_MSG;

    /* 1. teleservice id */
    if (static_cast<size_t>(offset) >= pduStrLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }
    pduStr[offset++] = SMS_TRANS_PARAM_TELESVC_IDENTIFIER;

    if (static_cast<size_t>(offset) >= pduStrLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }
    pduStr[offset++] = 0x02;

    if (static_cast<size_t>(offset) >= pduStrLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }
    pduStr[offset++] = (0xff00 & p2pMsg.transTelesvcId) >> SHIFT_8BITS;

    if (static_cast<size_t>(offset) >= pduStrLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }
    pduStr[offset++] = (0x00ff & p2pMsg.transTelesvcId);
    return offset;
}

int CdmaSmsPduCodec::EncodeP2PMsgServiceCategory(
    const struct SmsTransP2PMsg &p2pMsg, unsigned char *pduStr, size_t pduStrLen)
{
    int offset = 0;
    if (pduStr == nullptr || pduStrLen == 0) {
        TELEPHONY_LOGE("PDU is null!");
        return offset;
    }

    if (p2pMsg.transSvcCtg >= SMS_TRANS_SVC_CTG_UNDEFINED) {
        return offset;
    }
    pduStr[offset++] = SMS_TRANS_PARAM_SERVICE_CATEGORY;

    if (static_cast<size_t>(offset) >= pduStrLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }
    pduStr[offset++] = 0x02;

    if (static_cast<size_t>(offset) >= pduStrLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }
    pduStr[offset++] = (0xff00 & p2pMsg.transSvcCtg) >> SHIFT_8BITS;

    if (static_cast<size_t>(offset) >= pduStrLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }
    pduStr[offset++] = (0x00ff & p2pMsg.transSvcCtg);
    return offset;
}

int CdmaSmsPduCodec::EncodeP2PMsgReplyOption(
    const struct SmsTransP2PMsg &p2pMsg, unsigned char *pduStr, size_t pduStrLen)
{
    int offset = 0;
    if (pduStr == nullptr || pduStrLen == 0) {
        TELEPHONY_LOGE("PDU is null!");
        return offset;
    }

    if (p2pMsg.transReplySeq == 0) {
        TELEPHONY_LOGE("transReplySeq error");
        return offset;
    }
    pduStr[offset++] = SMS_TRANS_PARAM_BEARER_REPLY_OPTION;

    if (static_cast<size_t>(offset) >= pduStrLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }
    pduStr[offset++] = 0x01;

    if (static_cast<size_t>(offset) >= pduStrLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }
    pduStr[offset++] = (p2pMsg.transReplySeq << SHIFT_2BITS);
    TELEPHONY_LOGI("Reply sequnce number = [%{public}d]", p2pMsg.transReplySeq);
    return offset;
}

int CdmaSmsPduCodec::EncodeCBMsg(const struct SmsTransBroadCastMsg &cbMsg, unsigned char *pduStr, size_t pduStrLen)
{
    int offset = 0;
    int lenIndex = 0;

    if (pduStr == nullptr || pduStrLen == 0) {
        TELEPHONY_LOGE("PDU is null!");
        return offset;
    }
    /* 1. Service Category(Mandatory) */
    offset += EncodeCBMsgServiceCategory(cbMsg, &pduStr[offset], pduStrLen);

    /* 2. Bearer Data(Optional) */
    if (static_cast<size_t>(offset) >= pduStrLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }
    pduStr[offset++] = SMS_TRANS_PARAM_BEARER_DATA;
    /* PARAMETER_LEN field should be filled at the last part. */

    if (static_cast<size_t>(offset) >= pduStrLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }
    lenIndex = offset++;
    if (static_cast<size_t>(offset) >= pduStrLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }

    int encodeSize = EncodeTelesvcMsg(cbMsg.telesvcMsg, &pduStr[offset], pduStrLen);
    /* PARAMETER_LEN */
    pduStr[lenIndex] = encodeSize;
    offset += encodeSize;
    return offset;
}

int CdmaSmsPduCodec::EncodeCBMsgServiceCategory(
    const struct SmsTransBroadCastMsg &cbMsg, unsigned char *pduStr, size_t pduStrLen)
{
    int offset = 0;
    if (pduStr == nullptr || pduStrLen == 0) {
        TELEPHONY_LOGE("PDU is null!");
        return offset;
    }

    if (static_cast<size_t>(offset) >= pduStrLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }
    pduStr[offset++] = SMS_TRANS_PARAM_SERVICE_CATEGORY;

    if (static_cast<size_t>(offset) >= pduStrLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }
    pduStr[offset++] = 0x02;

    if (static_cast<size_t>(offset) >= pduStrLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }
    pduStr[offset++] = (0xff00 & cbMsg.transSvcCtg) >> SHIFT_8BITS;

    if (static_cast<size_t>(offset) >= pduStrLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }
    pduStr[offset++] = (0x00ff & cbMsg.transSvcCtg);
    return offset;
}

int CdmaSmsPduCodec::EncodeAckMsg(const struct SmsTransAckMsg &ackMsg, unsigned char *pduStr, size_t pduStrLen)
{
    int offset = 0;
    int index = 0;
    if (pduStr == nullptr || pduStrLen == 0) {
        return offset;
    }
    /* 1. Address */
    offset += EncodeAddress(ackMsg.address, &pduStr[offset], pduStrLen);
    /* 2. Sub address */
    offset += EncodeSubAddress(ackMsg.subAddress, &pduStr[offset], pduStrLen);
    /* 3. Cause code */
    offset += EncodeAckMsgCauseCode(ackMsg, &pduStr[offset], pduStrLen, index);

    if (static_cast<size_t>(index) >= pduStrLen) {
        TELEPHONY_LOGE("index over size");
        return offset;
    }
    pduStr[index] = offset - index;
    return offset;
}

int CdmaSmsPduCodec::EncodeAckMsgCauseCode(
    const struct SmsTransAckMsg &ackMsg, unsigned char *pduStr, size_t pduStrLen, int &index)
{
    int offset = 0;
    if (pduStr == nullptr || pduStrLen == 0) {
        return offset;
    }

    pduStr[offset++] = SMS_TRANS_PARAM_CAUSE_CODES;
    if (static_cast<size_t>(offset) >= pduStrLen) {
        TELEPHONY_LOGE("index over size");
        return offset;
    }
    index = offset++;

    if (static_cast<size_t>(offset) >= pduStrLen) {
        TELEPHONY_LOGE("index over size");
        return offset;
    }
    pduStr[offset] |= ackMsg.causeCode.transReplySeq << SHIFT_2BITS;
    pduStr[offset] |= static_cast<unsigned char>(ackMsg.causeCode.errorClass);
    if (ackMsg.causeCode.errorClass != 0x0) {
        if (static_cast<size_t>(offset + 1) >= pduStrLen) {
            TELEPHONY_LOGE("index over size");
            return offset;
        }
        pduStr[++offset] = ackMsg.causeCode.causeCode;
    }
    return offset;
}

int CdmaSmsPduCodec::EncodeTelesvcMsg(const struct SmsTeleSvcMsg &svcMsg, unsigned char *pduStr, size_t pduStrLen)
{
    int encodeSize = 0;
    TELEPHONY_LOGI("Teleservice msg type = [%{public}d]", svcMsg.type);
    switch (svcMsg.type) {
        case SMS_TYPE_SUBMIT:
            encodeSize = EncodeTelesvcSubmitMsg(svcMsg.data.submit, pduStr, pduStrLen);
            break;
        case SMS_TYPE_CANCEL:
            encodeSize = EncodeTelesvcCancelMsg(svcMsg.data.cancel, pduStr, pduStrLen);
            break;
        case SMS_TYPE_DELIVER_REPORT:
            encodeSize = EncodeTelesvcDeliverReportMsg(svcMsg.data.report, pduStr, pduStrLen);
            break;
        case SMS_TYPE_DELIVER:
        case SMS_TYPE_USER_ACK:
        case SMS_TYPE_READ_ACK:
        default:
            TELEPHONY_LOGI("No matching type for [%{public}d]", svcMsg.type);
            break;
    }
    return encodeSize;
}

int CdmaSmsPduCodec::EncodeTelesvcDeliverReportMsg(
    const struct SmsTeleSvcDeliverReport &dRMsg, unsigned char *pduStr, size_t pduStrLen)
{
    int offset = 0;
    if (pduStr == nullptr || pduStrLen == 0) {
        TELEPHONY_LOGE("PDU is null!");
        return offset;
    }
    /* 1. Message Identifier (Mandatory) */
    offset += EncodeMsgId(dRMsg.msgId, SMS_TYPE_DELIVER_REPORT, pduStr, pduStrLen);
    /* 2. TP-Failure Cause (Conditional) */
    if (dRMsg.tpFailCause >= 0x80) {
        if (static_cast<size_t>(offset) >= pduStrLen) {
            TELEPHONY_LOGE("offset over size");
            return offset;
        }
        pduStr[offset++] = SMS_BEARER_TP_FAILURE_CAUSE;

        if (static_cast<size_t>(offset) >= pduStrLen) {
            TELEPHONY_LOGE("offset over size");
            return offset;
        }
        pduStr[offset++] = 0x01;

        if (static_cast<size_t>(offset) >= pduStrLen) {
            TELEPHONY_LOGE("offset over size");
            return offset;
        }
        pduStr[offset++] = dRMsg.tpFailCause;
    }
    /* 3. User Data (Optional) */
    if (static_cast<size_t>(offset) >= pduStrLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }
    offset += EncodeBearerUserData(dRMsg.userData, &pduStr[offset], pduStrLen);
    return offset;
}

int CdmaSmsPduCodec::EncodeTelesvcCancelMsg(
    const struct SmsTeleSvcCancel &cancelMsg, unsigned char *pduStr, size_t pduStrLen)
{
    return EncodeMsgId(cancelMsg.msgId, SMS_TYPE_CANCEL, pduStr, pduStrLen);
}

int CdmaSmsPduCodec::EncodeTelesvcSubmitMsg(
    const struct SmsTeleSvcSubmit &sbMsg, unsigned char *pduStr, size_t pduStrLen)
{
    int offset = 0;
    if (pduStr == nullptr || pduStrLen == 0) {
        TELEPHONY_LOGE("PDU is null!");
        return offset;
    }

    /* 1. Message Identifier */
    offset += EncodeMsgId(sbMsg.msgId, SMS_TYPE_SUBMIT, &pduStr[offset], pduStrLen);
    /* 2. User Data */
    offset += EncodeBearerUserData(sbMsg.userData, &pduStr[offset], pduStrLen);

    /* 3. Deferred Delivery Time */
    offset += EncodeTelesvcSubmitMsgDeliveryTime(sbMsg, &pduStr[offset], pduStrLen);

    /* 5. Priority Indicator */
    offset += EncodeTelesvcSubmitMsgPriorityIndicator(sbMsg, &pduStr[offset], pduStrLen);

    /* 7. Reply Option */
    offset += EncodeTelesvcSubmitMsgPeplyOption(sbMsg, &pduStr[offset], pduStrLen);

    if (sbMsg.callbackNumber.addrLen > 0) {
        std::vector<unsigned char> temp;

        if (static_cast<size_t>(offset) >= pduStrLen) {
            TELEPHONY_LOGE("offset over size");
            return offset;
        }
        pduStr[offset++] = SMS_BEARER_CALLBACK_NUMBER;

        if (static_cast<size_t>(offset) >= pduStrLen) {
            TELEPHONY_LOGE("offset over size");
            return offset;
        }
        pduStr[offset++] = EncodeCbNumber(sbMsg.callbackNumber, temp);

        if (offset + temp.size() >= pduStrLen) {
            TELEPHONY_LOGE("offset over size");
            return offset;
        }
        for (std::size_t i = 0; i < temp.size(); i++) {
            pduStr[offset++] = temp[i];
        }
    }

    return offset;
}

int CdmaSmsPduCodec::EncodeTelesvcSubmitMsgDeliveryTime(
    const struct SmsTeleSvcSubmit &sbMsg, unsigned char *pduStr, size_t pduStrLen)
{
    int offset = 0;
    if (pduStr == nullptr || pduStrLen == 0) {
        TELEPHONY_LOGE("PDU is null!");
        return offset;
    }

    if (sbMsg.deferValPeriod.format == SMS_TIME_RELATIVE) {
        pduStr[offset++] = SMS_BEARER_DEFERRED_DELIVERY_TIME_RELATIVE;

        if (static_cast<size_t>(offset) >= pduStrLen) {
            TELEPHONY_LOGE("offset over size");
            return offset;
        }
        pduStr[offset++] = 0x01;

        if (static_cast<size_t>(offset) >= pduStrLen) {
            TELEPHONY_LOGE("offset over size");
            return offset;
        }
        pduStr[offset++] = sbMsg.deferValPeriod.time.relTime.time;
    } else if (sbMsg.deferValPeriod.format == SMS_TIME_ABSOLUTE) {
        std::vector<unsigned char> temp;

        if (static_cast<size_t>(offset) >= pduStrLen) {
            TELEPHONY_LOGE("offset over size");
            return offset;
        }
        pduStr[offset++] = SMS_BEARER_DEFERRED_DELIVERY_TIME_ABSOLUTE;

        if (static_cast<size_t>(offset) >= pduStrLen) {
            TELEPHONY_LOGE("offset over size");
            return offset;
        }
        pduStr[offset++] = EncodeAbsTime(sbMsg.deferValPeriod.time.absTime, temp, pduStrLen);

        if (offset + temp.size() >= pduStrLen) {
            TELEPHONY_LOGE("offset over size");
            return offset;
        }
        for (std::size_t i = 0; i < temp.size(); i++) {
            pduStr[offset++] = temp[i];
        }
    }
    return offset;
}

int CdmaSmsPduCodec::EncodeTelesvcSubmitMsgPriorityIndicator(
    const struct SmsTeleSvcSubmit &sbMsg, unsigned char *pduStr, size_t pduStrLen)
{
    int offset = 0;
    if (pduStr == nullptr || pduStrLen == 0) {
        TELEPHONY_LOGE("PDU is null!");
        return offset;
    }

    if (sbMsg.priority < SMS_PRIORITY_NORMAL || sbMsg.priority > SMS_PRIORITY_EMERGENCY) {
        TELEPHONY_LOGE("sbMsg.priority error!");
        return offset;
    }
    pduStr[offset++] = SMS_BEARER_PRIORITY_INDICATOR;

    if (static_cast<size_t>(offset) >= pduStrLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }
    pduStr[offset++] = 0x01;

    if (static_cast<size_t>(offset) >= pduStrLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }
    pduStr[offset++] = sbMsg.priority << SHIFT_6BITS;
    return offset;
}

int CdmaSmsPduCodec::EncodeTelesvcSubmitMsgPeplyOption(
    const struct SmsTeleSvcSubmit &sbMsg, unsigned char *pduStr, size_t pduStrLen)
{
    int offset = 0;
    if (pduStr == nullptr || pduStrLen == 0) {
        TELEPHONY_LOGE("PDU is null!");
        return offset;
    }

    if (!sbMsg.replyOpt.userAckReq && !sbMsg.replyOpt.deliverAckReq && !sbMsg.replyOpt.readAckReq &&
        !sbMsg.replyOpt.reportReq) {
        TELEPHONY_LOGE("sbMsg.replyOpt error");
        return offset;
    }
    pduStr[offset++] = SMS_BEARER_REPLY_OPTION;

    if (static_cast<size_t>(offset) >= pduStrLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }
    pduStr[offset++] = 0x01;
    if (static_cast<size_t>(offset) >= pduStrLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }

    pduStr[offset] |= sbMsg.replyOpt.userAckReq << SHIFT_7BITS;
    pduStr[offset] |= sbMsg.replyOpt.deliverAckReq << SHIFT_6BITS;
    pduStr[offset] |= (unsigned int)(sbMsg.replyOpt.readAckReq) << SHIFT_5BITS;

    if (static_cast<size_t>(offset) >= pduStrLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }
    pduStr[offset++] |= sbMsg.replyOpt.reportReq << SHIFT_4BITS;
    return offset;
}

int CdmaSmsPduCodec::EncodeCbNumber(const SmsTeleSvcAddr &cbNumber, std::vector<unsigned char> &pdustr)
{
    unsigned char temp = cbNumber.digitMode ? 0x80 : 0x00;
    if (cbNumber.digitMode == false) {
        temp |= (cbNumber.addrLen & 0xfe) >> SHIFT_1BITS;
        pdustr.push_back(temp);

        int offset = 0;
        unsigned char cbData[SMS_TRANS_ADDRESS_MAX_LEN] = { 0 };
        cbData[offset] |= (cbNumber.addrLen & 0x01) << SHIFT_7BITS;
        int addrLen =
            SmsCommonUtils::ConvertDigitToDTMF(cbNumber.szData, cbNumber.addrLen, SHIFT_1BITS, cbData + offset);
        pdustr.insert(pdustr.end(), cbData, cbData + addrLen);
    } else if (cbNumber.digitMode == true) {
        temp |= cbNumber.numberType << SHIFT_6BITS;
        temp |= cbNumber.numberPlan;
        pdustr.push_back(temp);
        pdustr.push_back(cbNumber.addrLen);
        pdustr.insert(pdustr.end(), cbNumber.szData, cbNumber.szData + cbNumber.addrLen);
    }
    return static_cast<int>(pdustr.size());
}

int CdmaSmsPduCodec::EncodeAbsTime(const SmsTimeAbs &absTime, std::vector<unsigned char> &pdustr, size_t pduStrLen)
{
    int offset = 0;
    if (static_cast<size_t>(offset) + MIN_PDU_ABS_TIME_LEN >= pduStrLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }
    pdustr.push_back(ConvertToBCD(absTime.year));
    pdustr.push_back(ConvertToBCD(absTime.month));
    pdustr.push_back(ConvertToBCD(absTime.day));
    pdustr.push_back(ConvertToBCD(absTime.hour));
    pdustr.push_back(ConvertToBCD(absTime.minute));
    pdustr.push_back(ConvertToBCD(absTime.second));
    return static_cast<int>(pdustr.size());
}

int CdmaSmsPduCodec::EncodeMsgId(
    const SmsTransMsgId &msgId, const SmsMessageType &type, unsigned char *pduStr, size_t pduStrLen)
{
    int offset = 0;
    if (pduStr == nullptr || pduStrLen == 0) {
        return offset;
    }

    pduStr[offset++] = SMS_BEARER_MESSAGE_IDENTIFIER;

    if (static_cast<size_t>(offset) >= pduStrLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }
    pduStr[offset++] = 0x03;

    if (static_cast<size_t>(offset) >= pduStrLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }
    pduStr[offset++] = ((unsigned int)type << SHIFT_4BITS) | ((msgId.msgId & 0xf000) >> (BYTE_BITS + SHIFT_4BITS));

    if (static_cast<size_t>(offset) >= pduStrLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }
    pduStr[offset++] = (msgId.msgId & 0x0ff0) >> (SHIFT_4BITS);

    if (static_cast<size_t>(offset) >= pduStrLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }
    pduStr[offset++] = ((msgId.msgId & 0x000f) << (SHIFT_4BITS)) | (msgId.headerInd ? 0x08 : 0x00);
    return offset;
}

int CdmaSmsPduCodec::EncodeSubAddress(const struct SmsTransSubAddr &address, unsigned char *pduStr, size_t pduStrLen)
{
    unsigned int offset = 0;
    if (pduStr == nullptr || address.addrLen == 0) {
        TELEPHONY_LOGE("PDU is null!");
        return offset;
    }

    pduStr[offset++] = SMS_TRANS_PARAM_ORG_SUB_ADDRESS;

    if (static_cast<size_t>(offset) >= pduStrLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }
    pduStr[offset] = address.addrLen + 0x02;

    unsigned int index = offset++;
    if (static_cast<size_t>(offset) >= pduStrLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }

    pduStr[offset] |= static_cast<unsigned char>(address.type) << SHIFT_5BITS;
    pduStr[offset++] |= (address.odd ? 0x10 : 0x00);

    if (static_cast<size_t>(offset) >= pduStrLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }
    pduStr[offset++] = address.addrLen;

    if (offset + address.addrLen >= pduStrLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }
    if (memcpy_s(pduStr + offset, address.addrLen, address.szData, address.addrLen) != EOK) {
        TELEPHONY_LOGE("EncodeSubAddress memcpy_s err.");
        return offset;
    }
    offset += address.addrLen;
    ShiftNBit(&pduStr[index], offset - index + 1, SHIFT_4BITS);
    return offset;
}

int CdmaSmsPduCodec::EncodeAddress(const SmsTransAddr &address, unsigned char *pduStr, size_t pduStrLen)
{
    unsigned int offset = 0;
    unsigned int lenIndex = 0;
    if (pduStr == nullptr || pduStrLen == 0) {
        TELEPHONY_LOGE("PDU is null!");
        return offset;
    }

    offset += EncodeAddressTransParam(address, &pduStr[offset], pduStrLen, lenIndex);
    offset += EncodeAddressModeParam(address, &pduStr[offset], pduStrLen);

    pduStr[lenIndex] = offset - lenIndex - 1;
    TELEPHONY_LOGI("Address subparam length field = [%{public}d]", pduStr[lenIndex]);
    return offset;
}

int CdmaSmsPduCodec::EncodeAddressTransParam(
    const SmsTransAddr &address, unsigned char *pduStr, size_t pduStrLen, unsigned int &lenIndex)
{
    unsigned int offset = 0;
    if (pduStr == nullptr || pduStrLen == 0) {
        TELEPHONY_LOGE("PDU is null!");
        return offset;
    }

    pduStr[offset++] = SMS_TRANS_PARAM_DEST_ADDRESS; /* Will be set to param length */

    if (static_cast<size_t>(offset) >= pduStrLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }
    lenIndex = offset++; // then lenIndex 1
    if (static_cast<size_t>(offset) >= pduStrLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }

    pduStr[offset] = (address.digitMode ? 0x80 : 0x00);
    pduStr[offset] |= (address.numberMode ? 0x40 : 0x00);
    if (address.digitMode) {
        pduStr[offset] |= address.numberType << SHIFT_3BITS;
    }
    return offset;
}

int CdmaSmsPduCodec::EncodeAddressModeParam(const SmsTransAddr &address, unsigned char *pduStr, size_t pduStrLen)
{
    unsigned int offset = 0;
    if (pduStr == nullptr || pduStrLen == 0) {
        TELEPHONY_LOGE("PDU is null!");
        return offset;
    }

    if (address.digitMode) {
        offset += EncodeAddressDigitModeParam(address, &pduStr[offset], pduStrLen);
    } else {
        unsigned int index = offset++;

        if (static_cast<size_t>(offset) >= pduStrLen) {
            TELEPHONY_LOGE("offset over size");
            return offset;
        }
        pduStr[offset++] = address.addrLen;

        if (offset + address.addrLen >= pduStrLen) {
            TELEPHONY_LOGE("offset over size");
            return offset;
        }
        int addrLen = SmsCommonUtils::ConvertDigitToDTMF(address.szData, address.addrLen, 0, pduStr + offset);
        offset += addrLen;
        ShiftNBit(&pduStr[index], offset - index + 1, SHIFT_6BITS);
    }
    return offset;
}

int CdmaSmsPduCodec::EncodeAddressDigitModeParam(const SmsTransAddr &address, unsigned char *pduStr, size_t pduStrLen)
{
    unsigned int offset = 0;
    if (pduStr == nullptr || pduStrLen == 0) {
        TELEPHONY_LOGE("PDU is null!");
        return offset;
    }

    if (address.numberMode) {
        offset += EncodeAddressNumberModeParam(address, pduStr, pduStrLen);
    } else {
        offset += EncodeAddressOtherNumberModeParam(address, pduStr, pduStrLen);
    }
    return offset;
}

int CdmaSmsPduCodec::EncodeAddressNumberModeParam(const SmsTransAddr &address, unsigned char *pduStr, size_t pduStrLen)
{
    unsigned int offset = 0;
    if (pduStr == nullptr || pduStrLen == 0) {
        TELEPHONY_LOGE("PDU is null!");
        return offset;
    }

    unsigned int index = offset++;

    if (static_cast<size_t>(offset) >= pduStrLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }
    pduStr[offset++] = address.addrLen;

    if (offset + address.addrLen >= pduStrLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }
    if (memcpy_s(pduStr + offset, address.addrLen, address.szData, address.addrLen) != EOK) {
        TELEPHONY_LOGE("EncodeAddress numberMode memcpy_s fail.");
        return offset;
    }
    offset += address.addrLen;
    ShiftNBit(&pduStr[index], offset - index + 1, SHIFT_3BITS);
    return offset;
}

int CdmaSmsPduCodec::EncodeAddressOtherNumberModeParam(
    const SmsTransAddr &address, unsigned char *pduStr, size_t pduStrLen)
{
    unsigned int offset = 0;
    if (pduStr == nullptr || pduStrLen == 0) {
        TELEPHONY_LOGE("PDU is null!");
        return offset;
    }
    pduStr[offset++] |= address.numberPlan >> SHIFT_1BITS;

    if (static_cast<size_t>(offset) >= pduStrLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }
    pduStr[offset++] |= address.numberPlan << SHIFT_7BITS;
    unsigned int index = offset - 1;

    if (static_cast<size_t>(offset) >= pduStrLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }
    pduStr[offset++] = address.addrLen;

    if (offset + address.addrLen >= pduStrLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }
    if (memcpy_s(pduStr + offset, address.addrLen, address.szData, address.addrLen) != EOK) {
        TELEPHONY_LOGE("EncodeAddress mecpy_s fail.");
        return offset;
    }
    offset += address.addrLen;
    ShiftNBit(&pduStr[index], offset - index + 1, SHIFT_7BITS);
    return offset;
}

int CdmaSmsPduCodec::EncodeBearerUserData(
    const struct SmsTeleSvcUserData &userData, unsigned char *pduStr, size_t pduStrLen)
{
    int offset = 0;
    if (pduStr == nullptr || pduStrLen == 0 || userData.userData.length == 0) {
        TELEPHONY_LOGE("PDU is null!");
        return offset;
    }

    size_t lenIndex = 0;
    offset += EncodeBearerUserDataParam(userData, &pduStr[offset], pduStrLen, lenIndex);

    int remainBits = 0;
    offset += EncodeBearerUserDataEncodeType(userData, &pduStr[offset], pduStrLen, remainBits);
    ShiftNBit(&pduStr[lenIndex + 1], offset - lenIndex - 1, SHIFT_3BITS);

    int padding = 0;
    padding = (remainBits > 0) ? (BYTE_BITS - remainBits) : 0;
    if (padding >= SHIFT_3BITS) {
        offset--;
    }
    pduStr[lenIndex] = offset - lenIndex - 1;
    return offset;
}

int CdmaSmsPduCodec::EncodeBearerUserDataParam(
    const struct SmsTeleSvcUserData &userData, unsigned char *pduStr, size_t pduStrLen, size_t &lenIndex)
{
    int offset = 0;
    if (pduStr == nullptr || userData.userData.length == 0) {
        TELEPHONY_LOGE("PDU is null!");
        return offset;
    }

    if (static_cast<size_t>(offset) >= pduStrLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }
    pduStr[offset++] = SMS_BEARER_USER_DATA;
    lenIndex = offset;

    ++offset;
    if (static_cast<size_t>(offset) >= pduStrLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }

    pduStr[offset++] = static_cast<unsigned char>(userData.encodeType) << SHIFT_3BITS;

    if (static_cast<size_t>(offset) >= pduStrLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }
    if (userData.encodeType == SMS_ENCODE_EPM || userData.encodeType == SMS_ENCODE_GSMDCS) {
        pduStr[offset++] = userData.msgType;
    }
    return offset;
}

int CdmaSmsPduCodec::EncodeBearerUserDataEncodeType(
    const struct SmsTeleSvcUserData &userData, unsigned char *pduStr, size_t pduStrLen, int &remainBits)
{
    int offset = 0;
    if (pduStr == nullptr || pduStrLen == 0 || userData.userData.length == 0) {
        TELEPHONY_LOGE("PDU is null!");
        return offset;
    }

    switch (userData.encodeType) {
        case SMS_ENCODE_7BIT_ASCII: {
            int encodeSize = Encode7BitASCIIData(userData.userData, &pduStr[offset], remainBits);
            TELEPHONY_LOGI("Encode7BitASCIIData remainBits %{public}d  offset %{public}d", remainBits, offset);
            offset += encodeSize;
            break;
        }
        case SMS_ENCODE_GSM7BIT: {
            int encodeSize = Encode7BitGSMData(userData.userData, &pduStr[offset], remainBits);
            TELEPHONY_LOGI("Encode7BitGSMData remainBits %{public}d  offset %{public}d", remainBits, offset);
            offset += encodeSize;
            break;
        }
        case SMS_ENCODE_UNICODE: {
            int encodeSize = EncodeUCS2Data(userData.userData, &pduStr[offset], remainBits);
            TELEPHONY_LOGI("EncodeUCS2Data remainBits %{public}d  offset %{public}d", remainBits, offset);
            offset += encodeSize;
            break;
        }
        default: {
            pduStr[offset++] = userData.userData.length;

            if (static_cast<size_t>(offset + userData.userData.length) >= pduStrLen) {
                TELEPHONY_LOGE("offset over size");
                return offset;
            }
            if (memcpy_s(pduStr + offset, userData.userData.length, userData.userData.data, userData.userData.length) ==
                EOK) {
                offset += userData.userData.length;
            }
            break;
        }
    }
    return offset;
}

int CdmaSmsPduCodec::DecodeMsg(const unsigned char *pduStr, int pduLen, struct SmsTransMsg &transMsg)
{
    int decodelen = 0;
    int offset = 0;
    if (pduStr == nullptr || pduLen <= 0) {
        TELEPHONY_LOGE("PDU is null!");
        return decodelen;
    }

    char mti = pduStr[offset++] & 0xff;
    if (pduLen < offset) {
        return decodelen;
    }
    switch (mti) {
        case SMS_TRANS_P2P_MSG:
            transMsg.type = SMS_TRANS_P2P_MSG;
            decodelen = DecodeP2PMsg(&pduStr[offset], pduLen - offset, transMsg.data.p2pMsg);
            break;
        case SMS_TRANS_BROADCAST_MSG:
            transMsg.type = SMS_TRANS_BROADCAST_MSG;
            decodelen = DecodeCBMsg(&pduStr[offset], pduLen - offset, transMsg.data.cbMsg);
            break;
        case SMS_TRANS_ACK_MSG:
            transMsg.type = SMS_TRANS_ACK_MSG;
            decodelen = DecodeAckMsg(&pduStr[offset], pduLen - offset, transMsg.data.ackMsg);
            break;
        default:
            transMsg.type = SMS_TRANS_TYPE_RESERVED;
            break;
    }
    return decodelen + offset;
}

int CdmaSmsPduCodec::DecodeP2PMsg(const unsigned char *pduStr, int pduLen, struct SmsTransP2PMsg &p2pMsg)
{
    int offset = 0;
    if (pduStr == nullptr || pduLen <= 0) {
        TELEPHONY_LOGE("PDU is null!");
        return offset;
    }
    while (offset < pduLen) {
        switch (pduStr[offset]) {
            case SMS_TRANS_PARAM_TELESVC_IDENTIFIER:
                offset += DecodeTeleId(&pduStr[offset], pduLen - offset, p2pMsg.transTelesvcId);
                break;
            case SMS_TRANS_PARAM_SERVICE_CATEGORY:
                offset += DecodeSvcCtg(&pduStr[offset], pduLen - offset, p2pMsg.transSvcCtg);
                break;
            case SMS_TRANS_PARAM_ORG_ADDRESS:
            case SMS_TRANS_PARAM_DEST_ADDRESS:
                offset += DecodeAddress(&pduStr[offset], pduLen - offset, p2pMsg.address);
                break;
            case SMS_TRANS_PARAM_ORG_SUB_ADDRESS:
            case SMS_TRANS_PARAM_DEST_SUB_ADDRESS:
                offset += DecodeSubAddress(&pduStr[offset], pduLen - offset, p2pMsg.subAddress);
                break;
            case SMS_TRANS_PARAM_BEARER_REPLY_OPTION:
                offset += DecodeP2PMsgBearerReply(&pduStr[offset], pduLen - offset, p2pMsg);
                break;
            case SMS_TRANS_PARAM_BEARER_DATA:
                offset += DecodeP2PMsgBearerData(&pduStr[offset], pduLen - offset, p2pMsg);
                break;
            default:
                offset++;
                break;
        }
    }
    return offset;
}

int CdmaSmsPduCodec::DecodeP2PMsgBearerReply(const unsigned char *pduStr, int pduLen, struct SmsTransP2PMsg &p2pMsg)
{
    int offset = 0;
    if (pduStr == nullptr || pduLen <= 0) {
        TELEPHONY_LOGE("PDU is null!");
        return offset;
    }

    offset += HEX_BYTE_STEP;
    if (offset >= pduLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }
    p2pMsg.transReplySeq = (pduStr[offset] >> SHIFT_2BITS);
    offset++;
    return offset;
}

int CdmaSmsPduCodec::DecodeP2PMsgBearerData(const unsigned char *pduStr, int pduLen, struct SmsTransP2PMsg &p2pMsg)
{
    int offset = 0;
    if (pduStr == nullptr || pduLen <= 0) {
        TELEPHONY_LOGE("PDU is null!");
        return offset;
    }

    if (++offset >= pduLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }
    int tmpLen = pduStr[offset++];
    if (offset + tmpLen >= pduLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }

    DecodeP2PTelesvcMsg(&pduStr[offset], tmpLen, p2pMsg.telesvcMsg);
    offset += (tmpLen + 1);
    return offset;
}

int CdmaSmsPduCodec::DecodeCBMsg(const unsigned char *pduStr, int pduLen, struct SmsTransBroadCastMsg &cbMsg)
{
    int offset = 0;
    if (pduStr == nullptr || pduLen <= 0) {
        TELEPHONY_LOGE("PDU is null!");
        return offset;
    }

    while (offset < pduLen) {
        switch (pduStr[offset]) {
            case SMS_TRANS_PARAM_SERVICE_CATEGORY:
                offset += DecodeTeleId(pduStr + offset, pduLen - offset, cbMsg.transSvcCtg);
                break;
            case SMS_TRANS_PARAM_BEARER_DATA:
                offset += DecodeCBMsgParamBearerData(&pduStr[offset], pduLen, cbMsg);
                break;
            default:
                offset++;
        }
    }
    return offset;
}

int CdmaSmsPduCodec::DecodeCBMsgParamBearerData(
    const unsigned char *pduStr, int pduLen, struct SmsTransBroadCastMsg &cbMsg)
{
    int offset = 0;
    int tempLen = 0;
    if (pduStr == nullptr || pduLen <= 0) {
        TELEPHONY_LOGE("PDU is null!");
        return offset;
    }

    if (++offset >= pduLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }
    tempLen = pduStr[offset];

    if (++offset >= pduLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }
    if (cbMsg.transSvcCtg >= SMS_TRANS_SVC_CTG_CMAS_PRESIDENTIAL && cbMsg.transSvcCtg <= SMS_TRANS_SVC_CTG_CMAS_TEST) {
        if (offset + tempLen >= pduLen) {
            TELEPHONY_LOGE("offset over size");
            return offset;
        }
        DecodeCBBearerData(pduStr + offset, tempLen, cbMsg.telesvcMsg, true);
    } else {
        if (offset + tempLen >= pduLen) {
            TELEPHONY_LOGE("offset over size");
            return offset;
        }
        DecodeCBBearerData(pduStr + offset, tempLen, cbMsg.telesvcMsg, false);
    }
    offset += (tempLen + 1);
    return offset;
}

int CdmaSmsPduCodec::DecodeAckMsg(const unsigned char *pduStr, int pduLen, struct SmsTransAckMsg &ackMsg)
{
    int offset = 0;
    if (pduStr == nullptr || pduLen <= 0) {
        TELEPHONY_LOGE("PDU is null!");
        return offset;
    }

    while (offset < pduLen) {
        switch (pduStr[offset]) {
            case SMS_TRANS_PARAM_DEST_ADDRESS:
                offset += DecodeAddress(pduStr + offset, pduLen, ackMsg.address);
                break;
            case SMS_TRANS_PARAM_DEST_SUB_ADDRESS:
                offset += DecodeSubAddress(pduStr + offset, pduLen, ackMsg.subAddress);
                break;
            case SMS_TRANS_PARAM_CAUSE_CODES:
                offset += HEX_BYTE_STEP;
                if (offset >= pduLen) {
                    TELEPHONY_LOGE("offset over size");
                    return offset;
                }
                ackMsg.causeCode.transReplySeq = pduStr[offset] >> SHIFT_2BITS;

                if (offset >= pduLen) {
                    TELEPHONY_LOGE("offset over size");
                    return offset;
                }
                if ((pduStr[offset++] & 0x03) == 0x00) {
                    ackMsg.causeCode.errorClass = SMS_TRANS_ERR_CLASS_NONE;
                } else {
                    ackMsg.causeCode.causeCode = static_cast<enum SmsTransCauseCodeType>(pduStr[offset++]);
                }
                break;
            default:
                offset++;
                break;
        }
    }
    return offset;
}

void CdmaSmsPduCodec::DecodeP2PTelesvcMsg(const unsigned char *pduStr, int pduLen, struct SmsTeleSvcMsg &svcMsg)
{
    if (pduStr == nullptr || pduLen <= 0) {
        TELEPHONY_LOGE("PDU is null!");
        return;
    }

    std::vector<unsigned char> tempPdu(pduStr, pduStr + pduLen);
    svcMsg.type = FindMsgType(tempPdu);
    TELEPHONY_LOGI("Msg Type = [%{public}d]", svcMsg.type);
    switch (svcMsg.type) {
        case SMS_TYPE_DELIVER:
            DecodeP2PDeliverMsg(pduStr, pduLen, svcMsg.data.deliver);
            break;
        case SMS_TYPE_SUBMIT:
            DecodeP2PSubmitMsg(pduStr, pduLen, svcMsg.data.submit);
            break;
        case SMS_TYPE_DELIVERY_ACK:
            DecodeP2PDeliveryAckMsg(pduStr, pduLen, svcMsg.data.deliveryAck);
            break;
        case SMS_TYPE_USER_ACK:
            DecodeP2PUserAckMsg(pduStr, pduLen, svcMsg.data.userAck);
            break;
        case SMS_TYPE_READ_ACK:
            DecodeP2PReadAckMsg(pduStr, pduLen, svcMsg.data.readAck);
            break;
        case SMS_TYPE_SUBMIT_REPORT:
        default:
            break;
    }
}

void CdmaSmsPduCodec::DecodeP2PDeliverMsg(const unsigned char *pduStr, int pduLen, struct SmsTeleSvcDeliver &delMsg)
{
    int offset = 0;
    if (pduStr == nullptr || pduLen <= 0) {
        TELEPHONY_LOGE("PDU is null!");
        return;
    }

    while (offset < pduLen) {
        TELEPHONY_LOGI("current offset = [%{public}d] [%{public}x]", offset, pduStr[offset]);
        offset += DecodeP2PDeliverMsgBearerParam(&pduStr[offset], pduLen, delMsg);
    }
}

int CdmaSmsPduCodec::DecodeP2PDeliverMsgBearerParam(
    const unsigned char *pduStr, int pduLen, struct SmsTeleSvcDeliver &delMsg)
{
    int offset = 0;
    if (pduStr == nullptr || pduLen <= 0) {
        TELEPHONY_LOGE("PDU is null!");
        return offset;
    }

    switch (pduStr[offset]) {
        case SMS_BEARER_MESSAGE_IDENTIFIER:
            offset += DecodeP2PDeliverMsgBearerMsgIdentifier(&pduStr[offset], pduLen, delMsg);
            break;
        case SMS_BEARER_USER_DATA:
            offset += DecodeP2PDeliverMsgBearerUserData(&pduStr[offset], pduLen, delMsg);
            break;
        case SMS_BEARER_MSG_CENTER_TIME_STAMP:
            offset += DecodeP2PDeliverMsgBearerTimeStamp(&pduStr[offset], pduLen, delMsg);
            break;
        case SMS_BEARER_VALIDITY_PERIOD_ABSOLUTE:
            offset += DecodeP2PDeliverMsgBearerPeriodAbsolute(&pduStr[offset], pduLen, delMsg);
            break;
        case SMS_BEARER_VALIDITY_PERIOD_RELATIVE:
            offset += DecodeP2PDeliverMsgBearerPeriodRelative(&pduStr[offset], pduLen, delMsg);
            break;
        case SMS_BEARER_PRIORITY_INDICATOR:
            offset += DecodeP2PDeliverMsgBearerPriorityIndicator(&pduStr[offset], pduLen, delMsg);
            break;
        case SMS_BEARER_PRIVACY_INDICATOR:
            offset += DecodeP2PDeliverMsgBearerPrivacyIndicator(&pduStr[offset], pduLen, delMsg);
            break;
        case SMS_BEARER_REPLY_OPTION:
            offset += DecodeP2PDeliverMsgBearerReplyOption(&pduStr[offset], pduLen, delMsg);
            break;
        case SMS_BEARER_NUMBER_OF_MESSAGES:
            offset += DecodeP2PDeliverMsgBearerNumberMesssages(&pduStr[offset], pduLen, delMsg);
            break;
        default: /* skip unrecognized sub parameters */
            offset += DecodeP2PDeliverMsgOthersParam(&pduStr[offset], pduLen, delMsg);
            break;
    }
    return offset;
}

int CdmaSmsPduCodec::DecodeP2PDeliverMsgOthersParam(
    const unsigned char *pduStr, int pduLen, struct SmsTeleSvcDeliver &delMsg)
{
    int offset = 0;
    int tempLen;
    if (pduStr == nullptr || pduLen <= 0) {
        TELEPHONY_LOGE("PDU is null!");
        return offset;
    }

    switch (pduStr[offset]) {
        case SMS_BEARER_ALERT_ON_MSG_DELIVERY:
            offset += DecodeP2PDeliverMsgBearerAlertDelivery(&pduStr[offset], pduLen, delMsg);
            break;
        case SMS_BEARER_LANGUAGE_INDICATOR:
            offset += DecodeP2PDeliverMsgBearerLanguageIndicator(&pduStr[offset], pduLen, delMsg);
            break;
        case SMS_BEARER_CALLBACK_NUMBER:
            offset += DecodeP2PDeliverMsgBearerCallbackNumber(&pduStr[offset], pduLen, delMsg);
            break;
        case SMS_BEARER_MSG_DISPLAY_MODE:
            offset += DecodeP2PDeliverMsgBearerDisplayMode(&pduStr[offset], pduLen, delMsg);
            break;
        case SMS_BEARER_MULTI_ENCODING_USER_DATA:
            offset += DecodeP2PDeliverMsgBearerMultiEncoding(&pduStr[offset], pduLen, delMsg);
            break;
        case SMS_BEARER_MSG_DEPOSIT_INDEX:
            offset += DecodeP2PDeliverMsgBearerMessageIndex(&pduStr[offset], pduLen, delMsg);
            break;
        case SMS_BEARER_ENHANCED_VMN:
            DecodeP2PEnhancedVmn(pduStr, pduLen, delMsg.enhancedVmn);
            offset += HEX_BYTE_STEP;
            break;
        case SMS_BEARER_ENHANCED_VMN_ACK:
            DecodeP2PDeliverVmnAck(pduStr, pduLen, delMsg.enhancedVmnAck);
            offset += HEX_BYTE_STEP;
            break;
        default: /* skip unrecognized sub parameters */
            offset++;
            if (offset >= pduLen) {
                TELEPHONY_LOGE("offset over size");
                return offset;
            }
            tempLen = pduStr[offset++];
            offset += tempLen;
            break;
    }
    return offset;
}

int CdmaSmsPduCodec::DecodeP2PDeliverMsgBearerMsgIdentifier(
    const unsigned char *pduStr, int pduLen, struct SmsTeleSvcDeliver &delMsg)
{
    int offset = 0;
    if (pduStr == nullptr || pduLen <= 0) {
        TELEPHONY_LOGE("PDU is null!");
        return offset;
    }

    offset += MAX_MSG_ID_LEN;
    if (offset >= pduLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }
    offset += DecodeMsgId(pduStr + offset, MAX_MSG_ID_LEN, delMsg.msgId);
    return offset;
}

int CdmaSmsPduCodec::DecodeP2PDeliverMsgBearerUserData(
    const unsigned char *pduStr, int pduLen, struct SmsTeleSvcDeliver &delMsg)
{
    int offset = 0;
    int tempLen;
    int error = EOK;
    unsigned char tempStr[pduLen + 1];
    if (pduStr == nullptr || pduLen <= 0) {
        TELEPHONY_LOGE("PDU is null!");
        return offset;
    }

    offset += HEX_BYTE_STEP;
    if (offset - 1 >= pduLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }
    tempLen = pduStr[offset - 1];
    if (tempLen + offset >= pduLen) {
        TELEPHONY_LOGE("length err tempLen:%{public}d, pduLen:%{public}d.", tempLen, pduLen);
        return offset;
    }
    error = memset_s(tempStr, sizeof(tempStr), 0x00, sizeof(tempStr));
    if (error != EOK) {
        TELEPHONY_LOGE("SMS_BEARER_USER_DATA memset_s err.");
        return offset;
    }
    if (memcpy_s(tempStr, sizeof(tempStr), pduStr + offset, tempLen) == EOK) {
        bool headerInd = delMsg.msgId.headerInd;
        DecodeUserData(tempStr, tempLen, delMsg.userData, headerInd);
        offset += tempLen;
    }
    return offset;
}

int CdmaSmsPduCodec::DecodeP2PDeliverMsgBearerTimeStamp(
    const unsigned char *pduStr, int pduLen, struct SmsTeleSvcDeliver &delMsg)
{
    int offset = 0;
    if (pduStr == nullptr || pduLen <= 0) {
        TELEPHONY_LOGE("PDU is null!");
        return offset;
    }

    offset += HEX_BYTE_STEP;
    offset += DecodeAbsTime(pduStr + offset, pduLen, delMsg.timeStamp);
    return offset;
}

int CdmaSmsPduCodec::DecodeP2PDeliverMsgBearerPeriodAbsolute(
    const unsigned char *pduStr, int pduLen, struct SmsTeleSvcDeliver &delMsg)
{
    int offset = 0;
    if (pduStr == nullptr || pduLen <= 0) {
        TELEPHONY_LOGE("PDU is null!");
        return offset;
    }

    offset += HEX_BYTE_STEP;
    delMsg.valPeriod.format = SMS_TIME_ABSOLUTE;
    offset += DecodeAbsTime(pduStr + offset, pduLen, delMsg.valPeriod.time.absTime);
    return offset;
}

int CdmaSmsPduCodec::DecodeP2PDeliverMsgBearerPeriodRelative(
    const unsigned char *pduStr, int pduLen, struct SmsTeleSvcDeliver &delMsg)
{
    int offset = 0;
    if (pduStr == nullptr || pduLen <= 0) {
        TELEPHONY_LOGE("PDU is null!");
        return offset;
    }

    offset += HEX_BYTE_STEP;
    delMsg.valPeriod.format = SMS_TIME_RELATIVE;
    if (offset >= pduLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }
    delMsg.valPeriod.time.relTime.time = static_cast<enum SmsRelativeTime>(pduStr[offset++]);
    return offset;
}

int CdmaSmsPduCodec::DecodeP2PDeliverMsgBearerPriorityIndicator(
    const unsigned char *pduStr, int pduLen, struct SmsTeleSvcDeliver &delMsg)
{
    int offset = 0;
    if (pduStr == nullptr || pduLen <= 0) {
        TELEPHONY_LOGE("PDU is null!");
        return offset;
    }

    offset += HEX_BYTE_STEP;
    if (offset >= pduLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }
    delMsg.priority = static_cast<enum SmsPriorityIndicator>((pduStr[offset++] & 0xc0) >> SHIFT_6BITS);
    return offset;
}

int CdmaSmsPduCodec::DecodeP2PDeliverMsgBearerPrivacyIndicator(
    const unsigned char *pduStr, int pduLen, struct SmsTeleSvcDeliver &delMsg)
{
    int offset = 0;
    if (pduStr == nullptr || pduLen <= 0) {
        TELEPHONY_LOGE("PDU is null!");
        return offset;
    }

    offset += HEX_BYTE_STEP;
    if (offset >= pduLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }
    delMsg.privacy = static_cast<enum SmsPrivacyIndicator>((pduStr[offset++] & 0xc0) >> SHIFT_6BITS);
    return offset;
}

int CdmaSmsPduCodec::DecodeP2PDeliverMsgBearerReplyOption(
    const unsigned char *pduStr, int pduLen, struct SmsTeleSvcDeliver &delMsg)
{
    int offset = 0;
    if (pduStr == nullptr || pduLen <= 0) {
        TELEPHONY_LOGE("PDU is null!");
        return offset;
    }

    offset += HEX_BYTE_STEP;
    if (offset >= pduLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }
    delMsg.replyOpt.userAckReq = (pduStr[offset] & 0x80) ? true : false;
    delMsg.replyOpt.deliverAckReq = (pduStr[offset] & 0x40) ? true : false;
    delMsg.replyOpt.readAckReq = (pduStr[offset] & 0x20) ? true : false;
    delMsg.replyOpt.reportReq = (pduStr[offset] & 0x10) ? true : false;
    offset++;
    return offset;
}

int CdmaSmsPduCodec::DecodeP2PDeliverMsgBearerNumberMesssages(
    const unsigned char *pduStr, int pduLen, struct SmsTeleSvcDeliver &delMsg)
{
    int offset = 0;
    if (pduStr == nullptr || pduLen <= 0) {
        TELEPHONY_LOGE("PDU is null!");
        return offset;
    }

    offset += HEX_BYTE_STEP;
    if (offset >= pduLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }
    delMsg.numMsg = (((pduStr[offset] & 0xf0) >> SHIFT_4BITS) * DECIMAL_NUM) + (pduStr[offset] & 0x0f);
    offset++;
    return offset;
}

int CdmaSmsPduCodec::DecodeP2PDeliverMsgBearerAlertDelivery(
    const unsigned char *pduStr, int pduLen, struct SmsTeleSvcDeliver &delMsg)
{
    int offset = 0;
    int tempLen;
    if (pduStr == nullptr || pduLen <= 0) {
        TELEPHONY_LOGE("PDU is null!");
        return offset;
    }

    offset += HEX_BYTE_STEP;
    if (offset >= pduLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }
    tempLen = pduStr[offset - 1];
    if (tempLen > 0) {
        delMsg.alertPriority = static_cast<enum SmsAlertPriority>((pduStr[offset] & 0xc0) >> SHIFT_6BITS);
    }
    offset += tempLen;
    return offset;
}

int CdmaSmsPduCodec::DecodeP2PDeliverMsgBearerLanguageIndicator(
    const unsigned char *pduStr, int pduLen, struct SmsTeleSvcDeliver &delMsg)
{
    int offset = 0;
    if (pduStr == nullptr || pduLen <= 0) {
        TELEPHONY_LOGE("PDU is null!");
        return offset;
    }

    offset += HEX_BYTE_STEP;
    if (offset >= pduLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }
    delMsg.language = static_cast<enum SmsLanguageType>(pduStr[offset++]);
    return offset;
}

int CdmaSmsPduCodec::DecodeP2PDeliverMsgBearerCallbackNumber(
    const unsigned char *pduStr, int pduLen, struct SmsTeleSvcDeliver &delMsg)
{
    int offset = 0;
    int tempLen;
    if (pduStr == nullptr || pduLen <= 0) {
        TELEPHONY_LOGE("PDU is null!");
        return offset;
    }

    offset += HEX_BYTE_STEP;
    if (offset >= pduLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }
    tempLen = pduStr[offset - 1];
    DecodeCallBackNum(&pduStr[offset], tempLen, delMsg.callbackNumber);
    offset += tempLen;
    return offset;
}

int CdmaSmsPduCodec::DecodeP2PDeliverMsgBearerDisplayMode(
    const unsigned char *pduStr, int pduLen, struct SmsTeleSvcDeliver &delMsg)
{
    int offset = 0;
    if (pduStr == nullptr || pduLen <= 0) {
        TELEPHONY_LOGE("PDU is null!");
        return offset;
    }

    offset += HEX_BYTE_STEP;
    if (offset >= pduLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }
    delMsg.displayMode = static_cast<enum SmsDisplayMode>((pduStr[offset++] & 0xc0) >> SHIFT_6BITS);
    return offset;
}

int CdmaSmsPduCodec::DecodeP2PDeliverMsgBearerMultiEncoding(
    const unsigned char *pduStr, int pduLen, struct SmsTeleSvcDeliver &delMsg)
{
    int offset = 0;
    int tempLen;
    if (pduStr == nullptr || pduLen <= 0) {
        TELEPHONY_LOGE("PDU is null!");
        return offset;
    }

    offset += HEX_BYTE_STEP;
    if (offset - 1 >= pduLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }
    tempLen = pduStr[offset - 1];
    offset += tempLen;
    return offset;
}

int CdmaSmsPduCodec::DecodeP2PDeliverMsgBearerMessageIndex(
    const unsigned char *pduStr, int pduLen, struct SmsTeleSvcDeliver &delMsg)
{
    int offset = 0;
    if (pduStr == nullptr || pduLen <= 0) {
        TELEPHONY_LOGE("PDU is null!");
        return offset;
    }

    offset += HEX_BYTE_STEP;
    if (offset >= pduLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }
    delMsg.depositId = pduStr[offset++];
    if (offset >= pduLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }

    delMsg.depositId = (delMsg.depositId << SHIFT_8BITS) + pduStr[offset++];
    return offset;
}

void CdmaSmsPduCodec::DecodeP2PEnhancedVmn(
    const unsigned char *pduStr, int pduLen, struct SmsEnhancedVmn &enhancedVmn)
{
    int ret = 0;
    int offset = 1;
    int tempOff = 0;
    if (++offset >= pduLen) {
        TELEPHONY_LOGE("offset over size");
        return;
    }
    unsigned char tempStr[pduLen + 1];
    if (++offset >= pduLen) {
        TELEPHONY_LOGE("offset over size");
        return;
    }
    int tempLen = pduStr[offset++];
    int error = memset_s(tempStr, sizeof(tempStr), 0x00, sizeof(tempStr));
    if (error != EOK) {
        TELEPHONY_LOGE("DecodeP2PEnhancedVmn memset_s err.");
        return;
    }
    if (tempLen + offset >= pduLen) {
        TELEPHONY_LOGE("data length invalid tempLen:%{public}d, pduLen:%{public}d.", tempLen, pduLen);
        return;
    }
    ret = memcpy_s(tempStr, sizeof(tempStr), pduStr + offset, tempLen);
    if (ret != EOK) {
        TELEPHONY_LOGE("DecodeP2PEnhancedVmn memcpy_s");
        return;
    }
    enhancedVmn.priority = (enum SmsPriorityIndicator)(pduStr[offset] >> SHIFT_6BITS);
    enhancedVmn.passwordReq = (pduStr[offset] & 0x20) ? true : false;
    enhancedVmn.setupReq = (pduStr[offset] & 0x10) ? true : false;
    enhancedVmn.pwChangeReq = (pduStr[offset] & 0x08) ? true : false;

    ShiftNBitForDecode(tempStr, tempLen, SHIFT_5BITS);
    if (enhancedVmn.setupReq || enhancedVmn.pwChangeReq) {
        enhancedVmn.minPwLen = tempStr[tempOff] >> SHIFT_4BITS;
        enhancedVmn.maxPwLen = tempStr[tempOff++] & 0x0f;
    }
    DecodeP2PEnhancedVmnParam(tempStr, tempLen, enhancedVmn);
}

void CdmaSmsPduCodec::DecodeP2PEnhancedVmnParam(unsigned char *tempStr, int tempLen, struct SmsEnhancedVmn &enhancedVmn)
{
    int tempOff = 0;
    if (tempOff + 1 >= tempLen) {
        TELEPHONY_LOGE("tempOff over size");
        return;
    }
    enhancedVmn.vmNumUnheardMsg = tempStr[tempOff++];

    enhancedVmn.vmMailboxAlmFull = (tempStr[tempOff] & 0x80) ? true : false;
    enhancedVmn.vmMailboxFull = (tempStr[tempOff] & 0x40) ? true : false;
    enhancedVmn.replyAllowed = (tempStr[tempOff] & 0x20) ? true : false;
    enhancedVmn.faxIncluded = (tempStr[tempOff] & 0x10) ? true : false;

    if (tempOff + 1 >= tempLen) {
        TELEPHONY_LOGE("tempOff over size");
        return;
    }
    enhancedVmn.vmLen = (tempStr[tempOff] << SHIFT_8BITS) | tempStr[tempOff + 1];
    tempOff += HEX_BYTE_STEP;

    if (tempOff >= tempLen) {
        TELEPHONY_LOGE("tempOff over size");
        return;
    }
    enhancedVmn.vmRetDay = tempStr[tempOff] >> SHIFT_1BITS;
    ShiftNBitForDecode(tempStr, tempLen, SHIFT_7BITS);

    if (tempOff + 1 >= tempLen) {
        TELEPHONY_LOGE("tempOff over size");
        return;
    }
    enhancedVmn.vmMsgId = (tempStr[tempOff] << SHIFT_8BITS) | tempStr[tempOff + 1];
    tempOff += HEX_BYTE_STEP;

    if (tempOff + 1 >= tempLen) {
        TELEPHONY_LOGE("tempOff over size");
        return;
    }
    enhancedVmn.vmMailboxId = (tempStr[tempOff] << SHIFT_8BITS) | tempStr[tempOff + 1];
    tempOff += HEX_BYTE_STEP;

    if (tempOff >= tempLen) {
        TELEPHONY_LOGE("tempOff over size");
        return;
    }
    enhancedVmn.anDigitMode = (tempStr[tempOff] & 0x80) ? true : false;
    enhancedVmn.anNumberType = (tempStr[tempOff] & 0x70) >> SHIFT_4BITS;

    DecodeP2PEnhancedVmnAnDigitMode(tempStr, tempLen, enhancedVmn);
    DecodeP2PEnhancedVmnCliDigitMode(tempStr, tempLen, enhancedVmn);
}

void CdmaSmsPduCodec::DecodeP2PEnhancedVmnAnDigitMode(
    unsigned char *tempStr, int tempLen, struct SmsEnhancedVmn &enhancedVmn)
{
    int tempOff = 0;

    if (enhancedVmn.anDigitMode) {
        if (tempOff + 1 >= tempLen) {
            TELEPHONY_LOGE("tempOff over size");
            return;
        }
        enhancedVmn.anNumberPlan = tempStr[tempOff++] & 0x0f;
        enhancedVmn.anNumField = tempStr[tempOff++];

        int bcdLen = enhancedVmn.anNumField / HEX_BYTE_STEP;
        bcdLen = (enhancedVmn.anNumField % HEX_BYTE_STEP == 0) ? bcdLen : bcdLen + 1;
        if (tempOff >= tempLen || bcdLen > SMS_MAX_USER_DATA_LEN) {
            TELEPHONY_LOGE("DecodeP2PEnhancedVmn or tempOff over size");
            return;
        }
        SmsCommonUtils::BcdToDigitCdma(&(tempStr[tempOff]), bcdLen, reinterpret_cast<char *>(enhancedVmn.anChar));
        enhancedVmn.anChar[enhancedVmn.anNumField] = '\0';
    } else {
        ShiftNBitForDecode(tempStr, tempLen, SHIFT_4BITS);
        if (tempOff >= tempLen) {
            TELEPHONY_LOGE("tempOff over size");
            return;
        }
        enhancedVmn.anNumField = tempStr[tempOff++];

        if (memset_s(enhancedVmn.anChar, sizeof(enhancedVmn.anChar), 0x00, sizeof(enhancedVmn.anChar)) != EOK) {
            TELEPHONY_LOGE("enhancedVmn memset_s err.");
            return;
        }
        if (static_cast<unsigned long>(enhancedVmn.anNumField) > sizeof(enhancedVmn.anChar)) {
            TELEPHONY_LOGE("enhancedVmn memcpy_s data length invalid.");
            return;
        }
        if (static_cast<unsigned long>(tempOff) + static_cast<unsigned long>(enhancedVmn.anNumField) >
            static_cast<unsigned long>(tempLen)) {
            TELEPHONY_LOGE("enhancedVmn memcpy_s data length invalid.");
            return;
        }
        int ret = memcpy_s(enhancedVmn.anChar, sizeof(enhancedVmn.anChar), tempStr + tempOff, enhancedVmn.anNumField);
        if (ret != EOK) {
            TELEPHONY_LOGE("enhancedVmn memcpy_s err.");
            return;
        }
        tempOff += enhancedVmn.anNumField;
    }
}

void CdmaSmsPduCodec::DecodeP2PEnhancedVmnCliDigitMode(
    unsigned char *tempStr, int tempLen, struct SmsEnhancedVmn &enhancedVmn)
{
    int tempOff = 0;
    if (tempOff >= tempLen) {
        TELEPHONY_LOGE("tempOff over size");
        return;
    }

    enhancedVmn.cliDigitMode = (tempStr[tempOff] & 0x80) ? true : false;
    enhancedVmn.cliNumberType = (tempStr[tempOff] & 0x70) >> SHIFT_4BITS;
    if (enhancedVmn.cliDigitMode) {
        if (tempOff + 1 >= tempLen) {
            TELEPHONY_LOGE("tempOff over size");
            return;
        }
        enhancedVmn.cliNumberPlan = tempStr[tempOff++] & 0x0f;
        enhancedVmn.cliNumField = tempStr[tempOff++];

        if (tempOff >= tempLen) {
            TELEPHONY_LOGE("tempOff over size");
            return;
        }
        int bcdLen = tempStr[tempOff++] / HEX_BYTE_STEP;
        bcdLen = (enhancedVmn.cliNumField % HEX_BYTE_STEP == 0) ? bcdLen : bcdLen + 1;
        if (tempOff >= tempLen || bcdLen > SMS_MAX_USER_DATA_LEN) {
            TELEPHONY_LOGE("DecodeP2PEnhancedVmn or tempOff over size");
            return;
        }
        enhancedVmn.cliNumField =
            SmsCommonUtils::BcdToDigitCdma(&(tempStr[tempOff]), bcdLen, reinterpret_cast<char *>(enhancedVmn.anChar));
        enhancedVmn.anChar[enhancedVmn.cliNumField] = '\0';
    } else {
        ShiftNBitForDecode(tempStr, tempLen, SHIFT_4BITS);

        if (tempOff >= tempLen) {
            TELEPHONY_LOGE("tempOff over size");
            return;
        }
        enhancedVmn.cliNumField = tempStr[tempOff++];
        (void)memset_s(enhancedVmn.cliChar, sizeof(enhancedVmn.cliChar), 0x00, sizeof(enhancedVmn.cliChar));
        if (static_cast<unsigned long>(enhancedVmn.cliNumField) > sizeof(enhancedVmn.cliChar)) {
            TELEPHONY_LOGE("enhancedVmn memcpy_s data length invalid.");
            return;
        }
        if (static_cast<unsigned long>(tempOff) + static_cast<unsigned long>(enhancedVmn.cliNumField) >
            static_cast<unsigned long>(tempLen)) {
            TELEPHONY_LOGE("enhancedVmn memcpy_s data length invalid.");
            return;
        }
        if (memcpy_s(enhancedVmn.cliChar, sizeof(enhancedVmn.cliChar), tempStr + tempOff, enhancedVmn.cliNumField) !=
            EOK) {
            TELEPHONY_LOGE("enhancedVmn cliChar memcpy_s fail.");
        }
    }
}

void CdmaSmsPduCodec::DecodeP2PDeliverVmnAck(
    const unsigned char *pduStr, int pduLen, struct SmsEnhancedVmnAck &enhancedVmnAck)
{
    int offset = 1;
    if (pduStr == nullptr || pduLen <= 0) {
        return;
    }

    int tempLen = pduStr[offset++];
    unsigned char tempStr[pduLen + 1];
    int error = memset_s(tempStr, sizeof(tempStr), 0x00, sizeof(tempStr));
    if (error != EOK) {
        TELEPHONY_LOGE("DeliverVmnAck memset_s err.");
        return;
    }
    if (tempLen + offset >= pduLen) {
        TELEPHONY_LOGE("length err tempLen:%{public}d, pduLen:%{public}d.", tempLen, pduLen);
        return;
    }
    if (memcpy_s(tempStr, sizeof(tempStr), pduStr + offset, tempLen) != EOK) {
        TELEPHONY_LOGE("DecodeP2PDeliverVmnAck memcpy_s error.");
        return;
    }
    enhancedVmnAck.vmMailboxId = (tempStr[offset] << SHIFT_8BITS) | tempStr[offset + 1];
    offset += HEX_BYTE_STEP;
    if (offset >= pduLen) {
        TELEPHONY_LOGE("offset over size");
        return;
    }
    enhancedVmnAck.vmNumUnheardMsg = tempStr[offset++];
    enhancedVmnAck.numDeleteAck = tempStr[offset] >> SHIFT_5BITS;
    enhancedVmnAck.numPlayAck = (tempStr[offset] & 0x1c) >> SHIFT_2BITS;
    ShiftNBitForDecode(tempStr, tempLen, SHIFT_6BITS);
    for (int i = 0; i < enhancedVmnAck.numDeleteAck; i++) {
        if (offset + 1 >= pduLen) {
            TELEPHONY_LOGE("offset over size");
            return;
        }
        enhancedVmnAck.daVmMsgId[i] = (tempStr[offset] << SHIFT_8BITS) | tempStr[offset + 1];
        offset += HEX_BYTE_STEP;
    }
    for (int i = 0; i < enhancedVmnAck.numPlayAck; i++) {
        if (offset + 1 >= pduLen) {
            TELEPHONY_LOGE("offset over size");
            return;
        }
        enhancedVmnAck.paVmMsgId[i] = (tempStr[offset] << SHIFT_8BITS) | tempStr[offset + 1];
        offset += HEX_BYTE_STEP;
    }
}

void CdmaSmsPduCodec::DecodeP2PSubmitMsg(const unsigned char *pduStr, int pduLen, struct SmsTeleSvcSubmit &subMsg)
{
    int offset = 0;
    if (pduStr == nullptr || pduLen <= 0) {
        TELEPHONY_LOGE("PDU is null!");
        return;
    }

    while (offset < pduLen) {
        offset += DecodeP2PSubmitMsgBearerParam(&pduStr[offset], pduLen, subMsg);
    }
}

int CdmaSmsPduCodec::DecodeP2PSubmitMsgBearerParam(
    const unsigned char *pduStr, int pduLen, struct SmsTeleSvcSubmit &subMsg)
{
    int offset = 0;
    if (pduStr == nullptr || pduLen <= 0) {
        TELEPHONY_LOGE("PDU is null!");
        return offset;
    }

    switch (pduStr[offset]) {
        case SMS_BEARER_MESSAGE_IDENTIFIER:
            offset += DecodeP2PSubmitMsgBearerParamIdentifier(&pduStr[offset], pduLen, subMsg);
            break;
        case SMS_BEARER_USER_DATA:
            offset += DecodeP2PSubmitMsgBearerParamUserData(&pduStr[offset], pduLen, subMsg);
            break;
        case SMS_BEARER_VALIDITY_PERIOD_ABSOLUTE:
            offset += DecodeP2PSubmitMsgBearerParamPeriodAbsolute(&pduStr[offset], pduLen, subMsg);
            break;
        case SMS_BEARER_VALIDITY_PERIOD_RELATIVE:
            offset += DecodeP2PSubmitMsgBearerParamPeriodRelative(&pduStr[offset], pduLen, subMsg);
            break;
        case SMS_BEARER_DEFERRED_DELIVERY_TIME_ABSOLUTE:
            offset += DecodeP2PSubmitMsgBearerParamTimeAbsolute(&pduStr[offset], pduLen, subMsg);
            break;
        case SMS_BEARER_DEFERRED_DELIVERY_TIME_RELATIVE:
            offset += DecodeP2PSubmitMsgBearerParamTimeRelative(&pduStr[offset], pduLen, subMsg);
            break;
        case SMS_BEARER_PRIORITY_INDICATOR:
            offset += DecodeP2PSubmitMsgBearerParamPriorityIndicator(&pduStr[offset], pduLen, subMsg);
            break;
        default:
            offset += DecodeP2PSubmitMsgOthersParam(&pduStr[offset], pduLen, subMsg);
            break;
    }
    return offset;
}

int CdmaSmsPduCodec::DecodeP2PSubmitMsgOthersParam(
    const unsigned char *pduStr, int pduLen, struct SmsTeleSvcSubmit &subMsg)
{
    int offset = 0;
    if (pduStr == nullptr || pduLen <= 0) {
        TELEPHONY_LOGE("PDU is null!");
        return offset;
    }

    switch (pduStr[offset]) {
        case SMS_BEARER_PRIVACY_INDICATOR:
            offset += DecodeP2PSubmitMsgBearerParamPrivacyIndicator(&pduStr[offset], pduLen, subMsg);
            break;
        case SMS_BEARER_REPLY_OPTION:
            offset += DecodeP2PSubmitMsgBearerParamReplyOption(&pduStr[offset], pduLen, subMsg);
            break;
        case SMS_BEARER_ALERT_ON_MSG_DELIVERY:
            offset += DecodeP2PSubmitMsgBearerParamMsgDelivery(&pduStr[offset], pduLen, subMsg);
            break;
        case SMS_BEARER_LANGUAGE_INDICATOR:
            offset += DecodeP2PSubmitMsgBearerParamLangIndicator(&pduStr[offset], pduLen, subMsg);
            break;
        case SMS_BEARER_CALLBACK_NUMBER:
            offset += DecodeP2PSubmitMsgBearerParamCallbackNumber(&pduStr[offset], pduLen, subMsg);
            break;
        case SMS_BEARER_MULTI_ENCODING_USER_DATA:
            offset += DecodeP2PSubmitMsgBearerParamMultiEncoding(&pduStr[offset], pduLen, subMsg);
            break;
        case SMS_BEARER_MSG_DEPOSIT_INDEX:
            offset += DecodeP2PSubmitMsgBearerParamDepositIndex(&pduStr[offset], pduLen, subMsg);
            break;
        default:
            offset++;
            break;
    }
    return offset;
}

int CdmaSmsPduCodec::DecodeP2PSubmitMsgBearerParamIdentifier(
    const unsigned char *pduStr, int pduLen, struct SmsTeleSvcSubmit &subMsg)
{
    int offset = 0;
    if (pduStr == nullptr || pduLen <= 0) {
        TELEPHONY_LOGE("PDU is null!");
        return offset;
    }

    offset += MAX_MSG_ID_LEN;
    if (offset >= pduLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }
    offset += DecodeMsgId(pduStr + offset, MAX_MSG_ID_LEN, subMsg.msgId);
    return offset;
}

int CdmaSmsPduCodec::DecodeP2PSubmitMsgBearerParamUserData(
    const unsigned char *pduStr, int pduLen, struct SmsTeleSvcSubmit &subMsg)
{
    int offset = 0;
    int tempLen;
    unsigned char tempStr[pduLen + 1];
    if (pduStr == nullptr || pduLen <= 0) {
        TELEPHONY_LOGE("PDU is null!");
        return offset;
    }

    offset += HEX_BYTE_STEP;
    if (offset - 1 >= pduLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }

    tempLen = pduStr[offset - 1];
    if (tempLen + offset >= pduLen) {
        TELEPHONY_LOGE("length err tempLen:%{public}d, pduLen:%{public}d.", tempLen, pduLen);
        return offset;
    }
    (void)memset_s(tempStr, sizeof(tempStr), 0x00, sizeof(tempStr));
    if (memcpy_s(tempStr, sizeof(tempStr), pduStr + offset, tempLen) == EOK) {
        bool headerInd = subMsg.msgId.headerInd;
        DecodeUserData(tempStr, tempLen, subMsg.userData, headerInd);
        offset += tempLen;
    }
    return offset;
}

int CdmaSmsPduCodec::DecodeP2PSubmitMsgBearerParamPeriodAbsolute(
    const unsigned char *pduStr, int pduLen, struct SmsTeleSvcSubmit &subMsg)
{
    int offset = 0;
    if (pduStr == nullptr || pduLen <= 0) {
        TELEPHONY_LOGE("PDU is null!");
        return offset;
    }

    offset += HEX_BYTE_STEP;
    if (offset >= pduLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }
    subMsg.valPeriod.format = SMS_TIME_ABSOLUTE;
    offset += DecodeAbsTime(pduStr + offset, pduLen, subMsg.valPeriod.time.absTime);
    return offset;
}

int CdmaSmsPduCodec::DecodeP2PSubmitMsgBearerParamPeriodRelative(
    const unsigned char *pduStr, int pduLen, struct SmsTeleSvcSubmit &subMsg)
{
    int offset = 0;
    if (pduStr == nullptr || pduLen <= 0) {
        TELEPHONY_LOGE("PDU is null!");
        return offset;
    }

    offset += HEX_BYTE_STEP;
    if (offset >= pduLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }
    subMsg.valPeriod.format = SMS_TIME_RELATIVE;
    subMsg.valPeriod.time.relTime.time = static_cast<enum SmsRelativeTime>(pduStr[offset++]);
    return offset;
}

int CdmaSmsPduCodec::DecodeP2PSubmitMsgBearerParamTimeAbsolute(
    const unsigned char *pduStr, int pduLen, struct SmsTeleSvcSubmit &subMsg)
{
    int offset = 0;
    if (pduStr == nullptr || pduLen <= 0) {
        TELEPHONY_LOGE("PDU is null!");
        return offset;
    }

    offset += HEX_BYTE_STEP;
    if (offset >= pduLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }

    subMsg.deferValPeriod.format = SMS_TIME_ABSOLUTE;
    offset += DecodeAbsTime(pduStr + offset, pduLen, subMsg.deferValPeriod.time.absTime);
    return offset;
}

int CdmaSmsPduCodec::DecodeP2PSubmitMsgBearerParamTimeRelative(
    const unsigned char *pduStr, int pduLen, struct SmsTeleSvcSubmit &subMsg)
{
    int offset = 0;
    if (pduStr == nullptr || pduLen <= 0) {
        TELEPHONY_LOGE("PDU is null!");
        return offset;
    }

    offset += HEX_BYTE_STEP;
    if (offset >= pduLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }
    subMsg.deferValPeriod.format = SMS_TIME_RELATIVE;
    subMsg.deferValPeriod.time.relTime.time = static_cast<enum SmsRelativeTime>(pduStr[offset++]);
    return offset;
}

int CdmaSmsPduCodec::DecodeP2PSubmitMsgBearerParamPriorityIndicator(
    const unsigned char *pduStr, int pduLen, struct SmsTeleSvcSubmit &subMsg)
{
    int offset = 0;
    if (pduStr == nullptr || pduLen <= 0) {
        TELEPHONY_LOGE("PDU is null!");
        return offset;
    }

    offset += HEX_BYTE_STEP;
    if (offset >= pduLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }
    subMsg.priority = static_cast<enum SmsPriorityIndicator>((pduStr[offset++] & 0xc0) >> SHIFT_6BITS);
    return offset;
}

int CdmaSmsPduCodec::DecodeP2PSubmitMsgBearerParamPrivacyIndicator(
    const unsigned char *pduStr, int pduLen, struct SmsTeleSvcSubmit &subMsg)
{
    int offset = 0;
    if (pduStr == nullptr || pduLen <= 0) {
        TELEPHONY_LOGE("PDU is null!");
        return offset;
    }

    offset += HEX_BYTE_STEP;
    if (offset >= pduLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }
    subMsg.privacy = static_cast<enum SmsPrivacyIndicator>((pduStr[offset++] & 0xc0) >> SHIFT_6BITS);
    return offset;
}

int CdmaSmsPduCodec::DecodeP2PSubmitMsgBearerParamReplyOption(
    const unsigned char *pduStr, int pduLen, struct SmsTeleSvcSubmit &subMsg)
{
    int offset = 0;
    if (pduStr == nullptr || pduLen <= 0) {
        TELEPHONY_LOGE("PDU is null!");
        return offset;
    }

    offset += HEX_BYTE_STEP;
    if (offset >= pduLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }

    subMsg.replyOpt.userAckReq = (pduStr[offset] & 0x80) ? true : false;
    subMsg.replyOpt.deliverAckReq = (pduStr[offset] & 0x40) ? true : false;
    subMsg.replyOpt.readAckReq = (pduStr[offset] & 0x20) ? true : false;
    subMsg.replyOpt.reportReq = (pduStr[offset] & 0x10) ? true : false;
    offset++;
    return offset;
}

int CdmaSmsPduCodec::DecodeP2PSubmitMsgBearerParamMsgDelivery(
    const unsigned char *pduStr, int pduLen, struct SmsTeleSvcSubmit &subMsg)
{
    int offset = 0;
    if (pduStr == nullptr || pduLen <= 0) {
        TELEPHONY_LOGE("PDU is null!");
        return offset;
    }

    offset += HEX_BYTE_STEP;
    if (offset >= pduLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }
    subMsg.alertPriority = static_cast<enum SmsAlertPriority>((pduStr[offset++] & 0xc0) >> SHIFT_6BITS);
    return offset;
}

int CdmaSmsPduCodec::DecodeP2PSubmitMsgBearerParamLangIndicator(
    const unsigned char *pduStr, int pduLen, struct SmsTeleSvcSubmit &subMsg)
{
    int offset = 0;
    if (pduStr == nullptr || pduLen <= 0) {
        TELEPHONY_LOGE("PDU is null!");
        return offset;
    }

    offset += HEX_BYTE_STEP;
    if (offset >= pduLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }
    subMsg.language = static_cast<enum SmsLanguageType>(pduStr[offset++]);
    return offset;
}

int CdmaSmsPduCodec::DecodeP2PSubmitMsgBearerParamCallbackNumber(
    const unsigned char *pduStr, int pduLen, struct SmsTeleSvcSubmit &subMsg)
{
    int offset = 0;
    int tempLen;
    if (pduStr == nullptr || pduLen <= 0) {
        TELEPHONY_LOGE("PDU is null!");
        return offset;
    }

    offset += HEX_BYTE_STEP;
    if (offset - 1 >= pduLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }
    tempLen = pduStr[offset - 1];
    DecodeCallBackNum(&pduStr[offset], tempLen, subMsg.callbackNumber);
    offset += tempLen;
    return offset;
}

int CdmaSmsPduCodec::DecodeP2PSubmitMsgBearerParamMultiEncoding(
    const unsigned char *pduStr, int pduLen, struct SmsTeleSvcSubmit &subMsg)
{
    int offset = 0;
    int tempLen;
    if (pduStr == nullptr || pduLen <= 0) {
        TELEPHONY_LOGE("PDU is null!");
        return offset;
    }

    offset += HEX_BYTE_STEP;
    if (offset - 1 >= pduLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }
    tempLen = pduStr[offset - 1];
    offset += tempLen;
    return offset;
}

int CdmaSmsPduCodec::DecodeP2PSubmitMsgBearerParamDepositIndex(
    const unsigned char *pduStr, int pduLen, struct SmsTeleSvcSubmit &subMsg)
{
    int offset = 0;
    if (pduStr == nullptr || pduLen <= 0) {
        TELEPHONY_LOGE("PDU is null!");
        return offset;
    }

    offset += HEX_BYTE_STEP;
    if (offset >= pduLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }
    subMsg.depositId = pduStr[offset++];
    if (offset >= pduLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }

    subMsg.depositId = (subMsg.depositId << SHIFT_8BITS) + pduStr[offset++];
    return offset;
}

void CdmaSmsPduCodec::DecodeP2PUserAckMsg(const unsigned char *pduStr, int pduLen, struct SmsTeleSvcUserAck &userAck)
{
    int offset = 0;
    if (pduStr == nullptr || pduLen <= 0) {
        TELEPHONY_LOGE("PDU is null!");
        return;
    }

    while (offset < pduLen) {
        offset += DecodeP2PUserAckMsgBearerParam(&pduStr[offset], pduLen, userAck);
    }
}

int CdmaSmsPduCodec::DecodeP2PUserAckMsgBearerParam(
    const unsigned char *pduStr, int pduLen, struct SmsTeleSvcUserAck &userAck)
{
    int offset = 0;
    if (pduStr == nullptr || pduLen <= 0) {
        TELEPHONY_LOGE("PDU is null!");
        return offset;
    }

    switch (pduStr[offset]) {
        case SMS_BEARER_MESSAGE_IDENTIFIER:
            offset += DecodeP2PUserAckMsgBearerParamIdentifier(&pduStr[offset], pduLen, userAck);
            break;
        case SMS_BEARER_USER_DATA:
            offset += DecodeP2PUserAckMsgBearerParamUserData(&pduStr[offset], pduLen, userAck);
            break;
        case SMS_BEARER_USER_RESPONSE_CODE:
            offset += DecodeP2PUserAckMsgBearerParamResponseCode(&pduStr[offset], pduLen, userAck);
            break;
        case SMS_BEARER_MSG_CENTER_TIME_STAMP:
            offset += DecodeP2PUserAckMsgBearerParamTimeStamp(&pduStr[offset], pduLen, userAck);
            break;
        case SMS_BEARER_MULTI_ENCODING_USER_DATA:
            offset += DecodeP2PUserAckMsgBearerParamMultiEncoding(&pduStr[offset], pduLen, userAck);
            break;
        case SMS_BEARER_MSG_DEPOSIT_INDEX:
            offset += DecodeP2PUserAckMsgBearerParamDepositIndex(&pduStr[offset], pduLen, userAck);
            break;
        default:
            offset++;
            break;
    }
    return offset;
}

int CdmaSmsPduCodec::DecodeP2PUserAckMsgBearerParamIdentifier(
    const unsigned char *pduStr, int pduLen, struct SmsTeleSvcUserAck &userAck)
{
    int offset = 0;
    if (pduStr == nullptr || pduLen <= 0) {
        TELEPHONY_LOGE("PDU is null!");
        return offset;
    }

    offset += MAX_MSG_ID_LEN;
    if (offset >= pduLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }
    offset += DecodeMsgId(pduStr + offset, MAX_MSG_ID_LEN, userAck.msgId);
    return offset;
}

int CdmaSmsPduCodec::DecodeP2PUserAckMsgBearerParamUserData(
    const unsigned char *pduStr, int pduLen, struct SmsTeleSvcUserAck &userAck)
{
    int offset = 0;
    int tempLen;
    int error = EOK;
    unsigned char tempStr[pduLen + 1];
    if (pduStr == nullptr || pduLen <= 0) {
        TELEPHONY_LOGE("PDU is null!");
        return offset;
    }

    offset += HEX_BYTE_STEP;
    if (offset - 1 >= pduLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }
    tempLen = pduStr[offset - 1];
    if (tempLen + offset >= pduLen) {
        TELEPHONY_LOGE("length err tempLen:%{public}d, pduLen:%{public}d.", tempLen, pduLen);
        return offset;
    }
    error = memset_s(tempStr, sizeof(tempStr), 0x00, sizeof(tempStr));
    if (error != EOK) {
        TELEPHONY_LOGE("UserAckMsg memset_s err.");
        return offset;
    }
    if (memcpy_s(tempStr, sizeof(tempStr), pduStr + offset, tempLen) == EOK) {
        bool headerInd = userAck.msgId.headerInd;
        DecodeUserData(tempStr, tempLen, userAck.userData, headerInd);
        offset += tempLen;
    }
    return offset;
}

int CdmaSmsPduCodec::DecodeP2PUserAckMsgBearerParamResponseCode(
    const unsigned char *pduStr, int pduLen, struct SmsTeleSvcUserAck &userAck)
{
    int offset = 0;
    if (pduStr == nullptr || pduLen <= 0) {
        TELEPHONY_LOGE("PDU is null!");
        return offset;
    }

    offset += HEX_BYTE_STEP;
    if (offset >= pduLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }
    userAck.respCode = pduStr[offset++];
    return offset;
}

int CdmaSmsPduCodec::DecodeP2PUserAckMsgBearerParamTimeStamp(
    const unsigned char *pduStr, int pduLen, struct SmsTeleSvcUserAck &userAck)
{
    int offset = 0;
    if (pduStr == nullptr || pduLen <= 0) {
        TELEPHONY_LOGE("PDU is null!");
        return offset;
    }

    offset += HEX_BYTE_STEP;
    if (offset >= pduLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }
    offset += DecodeAbsTime(pduStr + offset, pduLen, userAck.timeStamp);
    return offset;
}

int CdmaSmsPduCodec::DecodeP2PUserAckMsgBearerParamMultiEncoding(
    const unsigned char *pduStr, int pduLen, struct SmsTeleSvcUserAck &userAck)
{
    int offset = 0;
    int tempLen;
    if (pduStr == nullptr || pduLen <= 0) {
        TELEPHONY_LOGE("PDU is null!");
        return offset;
    }

    offset += HEX_BYTE_STEP;
    if (offset - 1 >= pduLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }
    tempLen = pduStr[offset - 1];
    offset += tempLen;
    return offset;
}

int CdmaSmsPduCodec::DecodeP2PUserAckMsgBearerParamDepositIndex(
    const unsigned char *pduStr, int pduLen, struct SmsTeleSvcUserAck &userAck)
{
    int offset = 0;
    if (pduStr == nullptr || pduLen <= 0) {
        TELEPHONY_LOGE("PDU is null!");
        return offset;
    }

    offset += HEX_BYTE_STEP;
    if (offset + 1 >= pduLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }
    userAck.depositId = pduStr[++offset]; // the hight byte
    offset++;
    return offset;
}

void CdmaSmsPduCodec::DecodeP2PReadAckMsg(
    const unsigned char *pduStr, int pduLen, struct SmsTeleSvcReadAck &readAck)
{
    int offset = 0;
    if (pduStr == nullptr || pduLen <= 0) {
        TELEPHONY_LOGE("PDU is null!");
        return;
    }

    while (offset < pduLen) {
        offset += DecodeP2PReadAckMsgBearerParam(&pduStr[offset], pduLen, readAck);
    }
}

int CdmaSmsPduCodec::DecodeP2PReadAckMsgBearerParam(
    const unsigned char *pduStr, int pduLen, struct SmsTeleSvcReadAck &readAck)
{
    int offset = 0;
    if (pduStr == nullptr || pduLen <= 0) {
        TELEPHONY_LOGE("PDU is null!");
        return offset;
    }

    switch (pduStr[offset]) {
        case SMS_BEARER_MESSAGE_IDENTIFIER:
            offset += DecodeP2PReadAckMsgBearerParamIdentifier(&pduStr[offset], pduLen, readAck);
            break;
        case SMS_BEARER_USER_DATA:
            offset += DecodeP2PReadAckMsgBearerParamUserData(&pduStr[offset], pduLen, readAck);
            break;
        case SMS_BEARER_MSG_CENTER_TIME_STAMP:
            offset += DecodeP2PReadAckMsgBearerParamTimeStamp(&pduStr[offset], pduLen, readAck);

            break;
        case SMS_BEARER_MULTI_ENCODING_USER_DATA:
            offset += DecodeP2PReadAckMsgBearerParamMultiEncoding(&pduStr[offset], pduLen, readAck);
            break;
        case SMS_BEARER_MSG_DEPOSIT_INDEX:
            offset += DecodeP2PReadAckMsgBearerParamDepositIndex(&pduStr[offset], pduLen, readAck);
            break;
        default:
            offset++;
            break;
    }
    return offset;
}

int CdmaSmsPduCodec::DecodeP2PReadAckMsgBearerParamIdentifier(
    const unsigned char *pduStr, int pduLen, struct SmsTeleSvcReadAck &readAck)
{
    int offset = 0;
    if (pduStr == nullptr || pduLen <= 0) {
        TELEPHONY_LOGE("PDU is null!");
        return offset;
    }

    offset += MAX_MSG_ID_LEN;
    if (offset >= pduLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }
    offset += DecodeMsgId(pduStr + offset, MAX_MSG_ID_LEN, readAck.msgId);
    return offset;
}

int CdmaSmsPduCodec::DecodeP2PReadAckMsgBearerParamUserData(
    const unsigned char *pduStr, int pduLen, struct SmsTeleSvcReadAck &readAck)
{
    int offset = 0;
    int tempLen;
    unsigned char tempStr[pduLen + 1];
    int error = EOK;
    if (pduStr == nullptr || pduLen <= 0) {
        TELEPHONY_LOGE("PDU is null!");
        return offset;
    }

    offset += HEX_BYTE_STEP;
    if (offset - 1 >= pduLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }
    tempLen = pduStr[offset - 1];
    if (tempLen + offset >= pduLen) {
        TELEPHONY_LOGE("length err tempLen:%{public}d, pduLen:%{public}d.", tempLen, pduLen);
        return offset;
    }
    error = memset_s(tempStr, sizeof(tempStr), 0x00, sizeof(tempStr));
    if (error != EOK) {
        TELEPHONY_LOGE("ReadAckMsg memset_s err.");
        return offset;
    }
    if (memcpy_s(tempStr, sizeof(tempStr), pduStr + offset, tempLen) == EOK) {
        bool headerInd = readAck.msgId.headerInd;
        DecodeUserData(tempStr, tempLen, readAck.userData, headerInd);
        offset += tempLen;
    }
    return offset;
}

int CdmaSmsPduCodec::DecodeP2PReadAckMsgBearerParamTimeStamp(
    const unsigned char *pduStr, int pduLen, struct SmsTeleSvcReadAck &readAck)
{
    int offset = 0;
    if (pduStr == nullptr || pduLen <= 0) {
        TELEPHONY_LOGE("PDU is null!");
        return offset;
    }

    offset += HEX_BYTE_STEP;
    if (offset >= pduLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }
    offset += DecodeAbsTime(pduStr + offset, pduLen, readAck.timeStamp);
    return offset;
}

int CdmaSmsPduCodec::DecodeP2PReadAckMsgBearerParamMultiEncoding(
    const unsigned char *pduStr, int pduLen, struct SmsTeleSvcReadAck &readAck)
{
    int offset = 0;
    int tempLen;
    if (pduStr == nullptr || pduLen <= 0) {
        TELEPHONY_LOGE("PDU is null!");
        return offset;
    }

    offset += HEX_BYTE_STEP;
    if (offset - 1 >= pduLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }
    tempLen = pduStr[offset - 1];
    offset += tempLen;
    return offset;
}

int CdmaSmsPduCodec::DecodeP2PReadAckMsgBearerParamDepositIndex(
    const unsigned char *pduStr, int pduLen, struct SmsTeleSvcReadAck &readAck)
{
    int offset = 0;
    if (pduStr == nullptr || pduLen <= 0) {
        TELEPHONY_LOGE("PDU is null!");
        return offset;
    }

    /* Message Deposit Index */
    offset += HEX_BYTE_STEP;
    if (offset + 1 >= pduLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }
    readAck.depositId = pduStr[++offset]; // the hight byte
    offset++;
    return offset;
}

void CdmaSmsPduCodec::DecodeP2PSubmitReportMsg(
    const unsigned char *pduStr, int pduLen, struct SmsTeleSvcDeliverReport &subReport)
{
    int offset = 0;
    if (pduStr == nullptr || pduLen <= 0) {
        TELEPHONY_LOGE("PDU is null!");
        return;
    }

    while (offset < pduLen) {
        offset += DecodeP2PSubmitReportMsgBearerParam(&pduStr[offset], pduLen, subReport);
    }
}

int CdmaSmsPduCodec::DecodeP2PSubmitReportMsgBearerParam(
    const unsigned char *pduStr, int pduLen, struct SmsTeleSvcDeliverReport &subReport)
{
    int offset = 0;
    if (pduStr == nullptr || pduLen <= 0) {
        TELEPHONY_LOGE("PDU is null!");
        return offset;
    }

    switch (pduStr[offset]) {
        case SMS_BEARER_MESSAGE_IDENTIFIER:
            offset += DecodeP2PSubmitReportMsgBearerParamIdentifier(&pduStr[offset], pduLen, subReport);
            break;
        case SMS_BEARER_USER_DATA:
            offset += DecodeP2PSubmitReportMsgBearerUserData(&pduStr[offset], pduLen, subReport);
            break;
        case SMS_BEARER_LANGUAGE_INDICATOR:
            offset += DecodeP2PSubmitReportMsgBearerLanguageIndicator(&pduStr[offset], pduLen, subReport);
            break;
        case SMS_BEARER_MULTI_ENCODING_USER_DATA:
            offset += DecodeP2PSubmitReportMsgBearerParamMultiEncoding(&pduStr[offset], pduLen, subReport);
            break;
        case SMS_BEARER_TP_FAILURE_CAUSE:
            offset += DecodeP2PSubmitReportMsgBearerParamTPFailCause(&pduStr[offset], pduLen, subReport);
            break;
        default:
            offset++;
            break;
    }
    return offset;
}

int CdmaSmsPduCodec::DecodeP2PSubmitReportMsgBearerParamIdentifier(
    const unsigned char *pduStr, int pduLen, struct SmsTeleSvcDeliverReport &subReport)
{
    int offset = 0;
    if (pduStr == nullptr || pduLen <= 0) {
        TELEPHONY_LOGE("PDU is null!");
        return offset;
    }

    offset += MAX_MSG_ID_LEN;
    if (offset >= pduLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }
    offset += DecodeMsgId(&pduStr[offset], MAX_MSG_ID_LEN, subReport.msgId);
    return offset;
}

int CdmaSmsPduCodec::DecodeP2PSubmitReportMsgBearerUserData(
    const unsigned char *pduStr, int pduLen, struct SmsTeleSvcDeliverReport &subReport)
{
    int offset = 0;
    int tempLen;
    int error = EOK;
    unsigned char tempStr[pduLen + 1];
    if (pduStr == nullptr || pduLen <= 0) {
        TELEPHONY_LOGE("PDU is null!");
        return offset;
    }

    offset += HEX_BYTE_STEP;
    if (offset - 1 >= pduLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }
    tempLen = pduStr[offset - 1];
    if (tempLen + offset >= pduLen) {
        TELEPHONY_LOGE("length err tempLen:%{public}d, pduLen:%{public}d.", tempLen, pduLen);
        return offset;
    }

    error = memset_s(tempStr, sizeof(tempStr), 0x00, sizeof(tempStr));
    if (error != EOK) {
        TELEPHONY_LOGE("SubmitReportMsg memset_s err.");
        return offset;
    }
    if (memcpy_s(tempStr, sizeof(tempStr), pduStr + offset, tempLen) == EOK) {
        bool headerInd = subReport.msgId.headerInd;
        DecodeUserData(tempStr, tempLen, subReport.userData, headerInd);
        offset += tempLen;
    }
    return offset;
}

int CdmaSmsPduCodec::DecodeP2PSubmitReportMsgBearerLanguageIndicator(
    const unsigned char *pduStr, int pduLen, struct SmsTeleSvcDeliverReport &subReport)
{
    int offset = 0;
    if (pduStr == nullptr || pduLen <= 0) {
        TELEPHONY_LOGE("PDU is null!");
        return offset;
    }

    offset += HEX_BYTE_STEP;
    if (offset >= pduLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }
    subReport.language = static_cast<enum SmsLanguageType>(pduStr[offset++]);
    return offset;
}

int CdmaSmsPduCodec::DecodeP2PSubmitReportMsgBearerParamMultiEncoding(
    const unsigned char *pduStr, int pduLen, struct SmsTeleSvcDeliverReport &subReport)
{
    int offset = 0;
    int tempLen;
    if (pduStr == nullptr || pduLen <= 0) {
        TELEPHONY_LOGE("PDU is null!");
        return offset;
    }

    offset += HEX_BYTE_STEP;
    if (offset - 1 >= pduLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }
    tempLen = pduStr[offset - 1];
    offset += tempLen;
    return offset;
}

int CdmaSmsPduCodec::DecodeP2PSubmitReportMsgBearerParamTPFailCause(
    const unsigned char *pduStr, int pduLen, struct SmsTeleSvcDeliverReport &subReport)
{
    int offset = 0;
    if (pduStr == nullptr || pduLen <= 0) {
        TELEPHONY_LOGE("PDU is null!");
        return offset;
    }

    offset += HEX_BYTE_STEP;
    if (offset >= pduLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }
    subReport.tpFailCause = pduStr[offset++];
    return offset;
}

void CdmaSmsPduCodec::DecodeP2PDeliveryAckMsg(
    const unsigned char *pduStr, int pduLen, struct SmsTeleSvcDeliverAck &delAckMsg)
{
    int offset = 0;
    if (pduStr == nullptr || pduLen <= 0) {
        TELEPHONY_LOGE("PDU is null!");
        return;
    }

    while (offset < pduLen) {
        switch (pduStr[offset]) {
            case SMS_BEARER_MESSAGE_IDENTIFIER:
                offset += DecodeP2PDeliveryAckMsgIdentifier(&pduStr[offset], pduLen, delAckMsg);
                break;
            case SMS_BEARER_USER_DATA:
                offset += DecodeP2PDeliveryAckMsgUserData(&pduStr[offset], pduLen, delAckMsg);
                break;
            case SMS_BEARER_MSG_CENTER_TIME_STAMP:
                offset += DecodeP2PDeliveryAckMsgTimeStamp(&pduStr[offset], pduLen, delAckMsg);

                break;
            case SMS_BEARER_MULTI_ENCODING_USER_DATA:
                offset += DecodeP2PDeliveryAckMsgMultiEncoding(&pduStr[offset], pduLen, delAckMsg);
                break;
            case SMS_BEARER_MESSAGE_STATUS:
                offset += DecodeP2PDeliveryAckMsgStatus(&pduStr[offset], pduLen, delAckMsg);
                break;
            default:
                offset++;
                break;
        }
    }
}

int CdmaSmsPduCodec::DecodeP2PDeliveryAckMsgIdentifier(
    const unsigned char *pduStr, int pduLen, struct SmsTeleSvcDeliverAck &delAckMsg)
{
    int offset = 0;
    if (pduStr == nullptr || pduLen <= 0) {
        TELEPHONY_LOGE("PDU is null!");
        return offset;
    }

    offset += MAX_MSG_ID_LEN;
    if (offset >= pduLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }
    offset += DecodeMsgId(pduStr + offset, MAX_MSG_ID_LEN, delAckMsg.msgId);
    return offset;
}

int CdmaSmsPduCodec::DecodeP2PDeliveryAckMsgUserData(
    const unsigned char *pduStr, int pduLen, struct SmsTeleSvcDeliverAck &delAckMsg)
{
    int offset = 0;
    int tmpLen;
    int error = EOK;
    unsigned char tempStr[pduLen + 1];
    if (pduStr == nullptr || pduLen <= 0) {
        TELEPHONY_LOGE("PDU is null!");
        return offset;
    }

    offset += HEX_BYTE_STEP;
    if (offset - 1 >= pduLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }
    tmpLen = pduStr[offset - 1];
    if (tmpLen + offset >= pduLen) {
        TELEPHONY_LOGE("length err tmpLen:%{public}d, pduLen:%{public}d.", tmpLen, pduLen);
        return offset;
    }
    error = memset_s(tempStr, sizeof(tempStr), 0x00, sizeof(tempStr));
    if (error != EOK) {
        TELEPHONY_LOGE("DeliveryAckMsg memset_s err.");
        return offset;
    }
    if (memcpy_s(tempStr, sizeof(tempStr), pduStr + offset, tmpLen) == EOK) {
        bool headerInd = delAckMsg.msgId.headerInd;
        DecodeUserData(tempStr, tmpLen, delAckMsg.userData, headerInd);
        offset += tmpLen;
    }
    return offset;
}

int CdmaSmsPduCodec::DecodeP2PDeliveryAckMsgTimeStamp(
    const unsigned char *pduStr, int pduLen, struct SmsTeleSvcDeliverAck &delAckMsg)
{
    int offset = 0;
    if (pduStr == nullptr || pduLen <= 0) {
        TELEPHONY_LOGE("PDU is null!");
        return offset;
    }

    offset += HEX_BYTE_STEP;
    if (offset >= pduLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }
    offset += DecodeAbsTime(pduStr + offset, pduLen, delAckMsg.timeStamp);
    return offset;
}

int CdmaSmsPduCodec::DecodeP2PDeliveryAckMsgMultiEncoding(
    const unsigned char *pduStr, int pduLen, struct SmsTeleSvcDeliverAck &delAckMsg)
{
    int offset = 0;
    int tmpLen;
    if (pduStr == nullptr || pduLen <= 0) {
        TELEPHONY_LOGE("PDU is null!");
        return offset;
    }

    offset += HEX_BYTE_STEP;
    if (offset - 1 >= pduLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }
    tmpLen = pduStr[offset - 1];
    offset += tmpLen;
    return offset;
}

int CdmaSmsPduCodec::DecodeP2PDeliveryAckMsgStatus(
    const unsigned char *pduStr, int pduLen, struct SmsTeleSvcDeliverAck &delAckMsg)
{
    int offset = 0;
    if (pduStr == nullptr || pduLen <= 0) {
        TELEPHONY_LOGE("PDU is null!");
        return offset;
    }

    offset += HEX_BYTE_STEP;
    if (offset >= pduLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }

    delAckMsg.msgStatus = static_cast<enum SmsStatusCode>(pduStr[offset++]);
    return offset;
}

void CdmaSmsPduCodec::DecodeCBBearerData(
    const unsigned char *pduStr, int pduLen, struct SmsTeleSvcMsg &telesvc, bool isCMAS)
{
    int offset = 0;
    if (pduStr == nullptr || pduLen <= 0) {
        TELEPHONY_LOGE("PDU is null!");
        return;
    }

    while (offset < pduLen) {
        offset += DecodeCBBearerDataBearerParam(&pduStr[offset], pduLen, telesvc, isCMAS);
    }
}

int CdmaSmsPduCodec::DecodeCBBearerDataBearerParam(
    const unsigned char *pduStr, int pduLen, struct SmsTeleSvcMsg &telesvc, bool isCMAS)
{
    int offset = 0;
    if (pduStr == nullptr || pduLen <= 0) {
        TELEPHONY_LOGE("PDU is null!");
        return offset;
    }

    switch (pduStr[offset]) {
        case SMS_BEARER_MESSAGE_IDENTIFIER:
            offset += DecodeCBBearerDataBearerParamIdentifier(&pduStr[offset], pduLen, telesvc, isCMAS);
            break;
        case SMS_BEARER_USER_DATA:
            offset += DecodeCBBearerDataBearerParamUserData(&pduStr[offset], pduLen, telesvc, isCMAS);
            break;
        case SMS_BEARER_MSG_CENTER_TIME_STAMP:
            offset += DecodeCBBearerDataBearerParamTimeStamp(&pduStr[offset], pduLen, telesvc, isCMAS);
            break;
        case SMS_BEARER_VALIDITY_PERIOD_ABSOLUTE:
            offset += DecodeCBBearerDataBearerParamPeriodAbsolute(&pduStr[offset], pduLen, telesvc, isCMAS);
            break;
        case SMS_BEARER_VALIDITY_PERIOD_RELATIVE:
            offset += DecodeCBBearerDataBearerParamPeriodRelative(&pduStr[offset], pduLen, telesvc, isCMAS);
            break;
        case SMS_BEARER_PRIORITY_INDICATOR:
            offset += DecodeCBBearerDataBearerParamPriorityIndicator(&pduStr[offset], pduLen, telesvc, isCMAS);
            break;
        case SMS_BEARER_ALERT_ON_MSG_DELIVERY:
            offset += DecodeCBBearerDataBearerParamAlertDelivery(&pduStr[offset], pduLen, telesvc, isCMAS);
            break;
        case SMS_BEARER_LANGUAGE_INDICATOR:
            offset += DecodeCBBearerDataBearerParamLang_indicator(&pduStr[offset], pduLen, telesvc, isCMAS);
            break;
        case SMS_BEARER_CALLBACK_NUMBER:
            offset += DecodeCBBearerDataBearerParamCallbackNumber(&pduStr[offset], pduLen, telesvc, isCMAS);
            break;
        case SMS_BEARER_MSG_DISPLAY_MODE:
            offset += DecodeCBBearerDataBearerParamDisplayMode(&pduStr[offset], pduLen, telesvc, isCMAS);

            break;
        case SMS_BEARER_MULTI_ENCODING_USER_DATA:
            offset += DecodeCBBearerDataBearerParamMultiEncoding(&pduStr[offset], pduLen, telesvc, isCMAS);
            break;
        default:
            offset++;
            break;
    }
    return offset;
}

int CdmaSmsPduCodec::DecodeCBBearerDataBearerParamIdentifier(
    const unsigned char *pduStr, int pduLen, struct SmsTeleSvcMsg &telesvc, bool isCMAS)
{
    int offset = 0;
    if (pduStr == nullptr || pduLen <= 0) {
        TELEPHONY_LOGE("PDU is null!");
        return offset;
    }

    telesvc.type = SMS_TYPE_DELIVER;
    offset += MAX_MSG_ID_LEN;
    if (offset >= pduLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }
    offset += DecodeMsgId(pduStr + offset, MAX_MSG_ID_LEN, telesvc.data.deliver.msgId);
    return offset;
}

int CdmaSmsPduCodec::DecodeCBBearerDataBearerParamUserData(
    const unsigned char *pduStr, int pduLen, struct SmsTeleSvcMsg &telesvc, bool isCMAS)
{
    int offset = 0;
    int tempLen;
    int error = EOK;
    unsigned char tempStr[pduLen + 1];
    if (pduStr == nullptr || pduLen <= 0) {
        TELEPHONY_LOGE("PDU is null!");
        return offset;
    }

    offset += HEX_BYTE_STEP;
    if (offset - 1 >= pduLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }
    tempLen = pduStr[offset - 1];
    if (tempLen + offset >= pduLen) {
        TELEPHONY_LOGE("length err tempLen:%{public}d, pduLen:%{public}d.", tempLen, pduLen);
        return offset;
    }
    error = memset_s(tempStr, sizeof(tempStr), 0x00, sizeof(tempStr));
    if (error != EOK) {
        TELEPHONY_LOGE("BearerData memset_s err.");
        return offset;
    }
    if (memcpy_s(tempStr, sizeof(tempStr), pduStr + offset, tempLen) != EOK) {
        TELEPHONY_LOGE("DecodeCBBearerData memcpy_s fail.");
        return offset;
    }
    if (isCMAS) {
        DecodeCMASData(tempStr, tempLen, telesvc.data.deliver.cmasData);
    } else {
        bool headerInd = telesvc.data.deliver.msgId.headerInd;
        DecodeUserData(tempStr, tempLen, telesvc.data.deliver.userData, headerInd);
    }
    offset += tempLen;
    return offset;
}

int CdmaSmsPduCodec::DecodeCBBearerDataBearerParamTimeStamp(
    const unsigned char *pduStr, int pduLen, struct SmsTeleSvcMsg &telesvc, bool isCMAS)
{
    int offset = 0;
    if (pduStr == nullptr || pduLen <= 0) {
        TELEPHONY_LOGE("PDU is null!");
        return offset;
    }

    offset += HEX_BYTE_STEP;
    offset += DecodeAbsTime(pduStr + offset, pduLen, telesvc.data.deliver.timeStamp);
    return offset;
}

int CdmaSmsPduCodec::DecodeCBBearerDataBearerParamPeriodAbsolute(
    const unsigned char *pduStr, int pduLen, struct SmsTeleSvcMsg &telesvc, bool isCMAS)
{
    int offset = 0;
    if (pduStr == nullptr || pduLen <= 0) {
        TELEPHONY_LOGE("PDU is null!");
        return offset;
    }

    offset += HEX_BYTE_STEP;
    telesvc.data.deliver.valPeriod.format = SMS_TIME_ABSOLUTE;
    offset += DecodeAbsTime(pduStr + offset, pduLen, telesvc.data.deliver.valPeriod.time.absTime);
    return offset;
}

int CdmaSmsPduCodec::DecodeCBBearerDataBearerParamPeriodRelative(
    const unsigned char *pduStr, int pduLen, struct SmsTeleSvcMsg &telesvc, bool isCMAS)
{
    int offset = 0;
    if (pduStr == nullptr || pduLen <= 0) {
        TELEPHONY_LOGE("PDU is null!");
        return offset;
    }

    offset += HEX_BYTE_STEP;
    telesvc.data.deliver.valPeriod.format = SMS_TIME_RELATIVE;
    if (offset >= pduLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }
    telesvc.data.deliver.valPeriod.time.relTime.time = static_cast<enum SmsRelativeTime>(pduStr[offset++]);
    return offset;
}

int CdmaSmsPduCodec::DecodeCBBearerDataBearerParamPriorityIndicator(
    const unsigned char *pduStr, int pduLen, struct SmsTeleSvcMsg &telesvc, bool isCMAS)
{
    int offset = 0;
    if (pduStr == nullptr || pduLen <= 0) {
        TELEPHONY_LOGE("PDU is null!");
        return offset;
    }

    offset += HEX_BYTE_STEP;
    if (offset >= pduLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }

    telesvc.data.deliver.priority = static_cast<enum SmsPriorityIndicator>((pduStr[offset++] & 0xc0) >> SHIFT_6BITS);
    return offset;
}

int CdmaSmsPduCodec::DecodeCBBearerDataBearerParamAlertDelivery(
    const unsigned char *pduStr, int pduLen, struct SmsTeleSvcMsg &telesvc, bool isCMAS)
{
    int offset = 0;
    if (pduStr == nullptr || pduLen <= 0) {
        TELEPHONY_LOGE("PDU is null!");
        return offset;
    }

    offset += HEX_BYTE_STEP;
    if (offset >= pduLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }

    telesvc.data.deliver.alertPriority = static_cast<enum SmsAlertPriority>((pduStr[offset++] & 0xc0) >> SHIFT_6BITS);
    return offset;
}

int CdmaSmsPduCodec::DecodeCBBearerDataBearerParamLang_indicator(
    const unsigned char *pduStr, int pduLen, struct SmsTeleSvcMsg &telesvc, bool isCMAS)
{
    int offset = 0;
    if (pduStr == nullptr || pduLen <= 0) {
        TELEPHONY_LOGE("PDU is null!");
        return offset;
    }

    offset += HEX_BYTE_STEP;
    if (offset >= pduLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }

    telesvc.data.deliver.language = static_cast<enum SmsLanguageType>(pduStr[offset++]);
    return offset;
}

int CdmaSmsPduCodec::DecodeCBBearerDataBearerParamCallbackNumber(
    const unsigned char *pduStr, int pduLen, struct SmsTeleSvcMsg &telesvc, bool isCMAS)
{
    int offset = 0;
    int tempLen;
    if (pduStr == nullptr || pduLen <= 0) {
        TELEPHONY_LOGE("PDU is null!");
        return offset;
    }

    offset += HEX_BYTE_STEP;
    if (offset >= pduLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }

    tempLen = pduStr[offset - 1];
    DecodeCallBackNum(&pduStr[offset], tempLen, telesvc.data.deliver.callbackNumber);
    offset += tempLen;
    return offset;
}

int CdmaSmsPduCodec::DecodeCBBearerDataBearerParamDisplayMode(
    const unsigned char *pduStr, int pduLen, struct SmsTeleSvcMsg &telesvc, bool isCMAS)
{
    int offset = 0;
    if (pduStr == nullptr || pduLen <= 0) {
        TELEPHONY_LOGE("PDU is null!");
        return offset;
    }

    offset += HEX_BYTE_STEP;
    if (offset >= pduLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }

    telesvc.data.deliver.displayMode = static_cast<enum SmsDisplayMode>((pduStr[offset++] & 0xc0) >> SHIFT_6BITS);
    return offset;
}

int CdmaSmsPduCodec::DecodeCBBearerDataBearerParamMultiEncoding(
    const unsigned char *pduStr, int pduLen, struct SmsTeleSvcMsg &telesvc, bool isCMAS)
{
    int offset = 0;
    int tempLen;
    if (pduStr == nullptr || pduLen <= 0) {
        TELEPHONY_LOGE("PDU is null!");
        return offset;
    }

    offset += HEX_BYTE_STEP;
    if (offset - 1 >= pduLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }
    tempLen = pduStr[offset - 1];
    offset += tempLen;
    return offset;
}

int CdmaSmsPduCodec::DecodeTeleId(const unsigned char *pduStr, int pduLen, unsigned short &teleSvcId)
{
    int offset = 0;
    unsigned short tempParam;
    offset += HEX_BYTE_STEP;
    if (offset + 1 >= pduLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }
    tempParam = pduStr[offset++];
    tempParam = (tempParam << SHIFT_8BITS) + pduStr[offset++];
    switch (tempParam) {
        case SMS_TRANS_TELESVC_CMT_91:
            teleSvcId = SMS_TRANS_TELESVC_CMT_91;
            break;
        case SMS_TRANS_TELESVC_CPT_95:
            teleSvcId = SMS_TRANS_TELESVC_CPT_95;
            break;
        case SMS_TRANS_TELESVC_CMT_95:
            teleSvcId = SMS_TRANS_TELESVC_CMT_95;
            break;
        case SMS_TRANS_TELESVC_VMN_95:
            teleSvcId = SMS_TRANS_TELESVC_VMN_95;
            break;
        case SMS_TRANS_TELESVC_WAP:
            teleSvcId = SMS_TRANS_TELESVC_WAP;
            break;
        case SMS_TRANS_TELESVC_WEMT:
            teleSvcId = SMS_TRANS_TELESVC_WEMT;
            break;
        case SMS_TRANS_TELESVC_SCPT:
            teleSvcId = SMS_TRANS_TELESVC_SCPT;
            break;
        case SMS_TRANS_TELESVC_CATPT:
            teleSvcId = SMS_TRANS_TELESVC_CATPT;
            break;
        default:
            teleSvcId = SMS_TRANS_TELESVC_RESERVED;
            break;
    }

    return offset;
}

int CdmaSmsPduCodec::DecodeSvcCtg(const unsigned char *pduStr, int pduLen, unsigned short &transSvcCtg)
{
    int offset = 0;
    unsigned short tempParam;
    offset += HEX_BYTE_STEP;
    if (offset + 1 >= pduLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }
    tempParam = pduStr[offset++];
    tempParam = (tempParam << SHIFT_8BITS) + pduStr[offset++];
    if ((tempParam >= SMS_TRANS_SVC_CTG_UNKNOWN && tempParam <= SMS_TRANS_SVC_CTG_KDDI_CORP_MAX1) ||
        (tempParam >= SMS_TRANS_SVC_CTG_KDDI_CORP_MIN2 && tempParam <= SMS_TRANS_SVC_CTG_KDDI_CORP_MAX2) ||
        (tempParam >= SMS_TRANS_SVC_CTG_KDDI_CORP_MIN3 && tempParam <= SMS_TRANS_SVC_CTG_KDDI_CORP_MAX3)) {
        transSvcCtg = tempParam;
    } else {
        transSvcCtg = SMS_TRANS_SVC_CTG_RESERVED;
    }
    return offset;
}

int CdmaSmsPduCodec::DecodeAddress(const unsigned char *pduStr, int pduLen, struct SmsTransAddr &transAddr)
{
    int offset = 1;
    unsigned char tempStr[pduLen + 1];
    int tempLen = pduStr[offset++];
    int error = memset_s(tempStr, sizeof(tempStr), 0x00, sizeof(tempStr));
    if (error != EOK) {
        TELEPHONY_LOGE("DecodeAddress memset_s err.");
        return offset;
    }
    if (tempLen + offset >= pduLen) {
        TELEPHONY_LOGE("data length invalid tempLen:%{public}d, pduLen:%{public}d.", tempLen, pduLen);
        return offset;
    }
    if (memcpy_s(tempStr, sizeof(tempStr), pduStr + offset, tempLen) != EOK) {
        TELEPHONY_LOGE("DecodeAddress pduStr memcpy_s err.");
        return offset;
    }

    if (memset_s(&transAddr, sizeof(SmsTransAddr), 0x00, sizeof(SmsTransAddr)) != EOK) {
        TELEPHONY_LOGE("SmsTransAddr memcpy_s err.");
        return offset;
    }

    offset += tempLen;
    transAddr.digitMode = (tempStr[0] & 0x80) ? true : false;
    transAddr.numberMode = (tempStr[0] & 0x40) ? true : false;
    TELEPHONY_LOGI("digitMode %{public}d  numberMode %{public}d", transAddr.digitMode, transAddr.numberMode);
    ShiftNBitForDecode(tempStr, tempLen, SHIFT_2BITS);
    DecodeAddressDigitMode(tempStr, tempLen, transAddr);

    return offset;
}

void CdmaSmsPduCodec::DecodeAddressDigitMode(unsigned char *tempStr, int tempLen, struct SmsTransAddr &transAddr)
{
    if (tempStr == nullptr) {
        TELEPHONY_LOGE("PDU is null!");
        return;
    }

    if (transAddr.digitMode) {
        if (transAddr.numberMode) {
            /* digit mode 1, number mode 1 */
            transAddr.numberType = DecodeDigitModeNumberType(tempStr[0] & 0xe0, true);
            ShiftNBitForDecode(tempStr, tempLen, SHIFT_3BITS);
        } else {
            /* digit mode 1, number mode 0 */
            transAddr.numberType = DecodeDigitModeNumberType(tempStr[0] & 0xe0, false);
            ShiftNBitForDecode(tempStr, tempLen, SHIFT_3BITS);
            transAddr.numberPlan = DecodeDigitModeNumberPlan((tempStr[0] >> SHIFT_4BITS) & 0x0f);
            ShiftNBitForDecode(tempStr, tempLen, SHIFT_4BITS);
        }
        transAddr.addrLen = tempStr[0];
        if (transAddr.addrLen > sizeof(transAddr.szData) ||
            static_cast<unsigned long>(transAddr.addrLen) + 1 > static_cast<unsigned long>(tempLen)) {
            TELEPHONY_LOGE("data length invalid.");
        }
        if (memcpy_s(transAddr.szData, sizeof(transAddr.szData), &tempStr[1], transAddr.addrLen) != EOK) {
            TELEPHONY_LOGE("DecodeAddress memcpy_s error.");
        }
    } else {
        transAddr.addrLen = tempStr[0];
        int bcdLen = (transAddr.addrLen % HEX_BYTE_STEP == 0) ? transAddr.addrLen : transAddr.addrLen + 1;
        if (bcdLen > SMS_TRANS_ADDRESS_MAX_LEN) {
            TELEPHONY_LOGE("DecodeAddress data length invalid.");
            return;
        }
        SmsCommonUtils::BcdToDigitCdma(&(tempStr[1]), bcdLen, transAddr.szData);
        transAddr.szData[transAddr.addrLen] = '\0';
    }
}

int CdmaSmsPduCodec::DecodeSubAddress(const unsigned char *pduStr, int pduLen, struct SmsTransSubAddr &subAddr)
{
    int offset = 0;
    int tempLen = 0;
    unsigned char tempStr[pduLen + 1];
    if (pduStr == nullptr || pduLen <= 0) {
        TELEPHONY_LOGE("PDU is null!");
        return offset;
    }

    offset++;
    tempLen = pduStr[offset++];
    int error = memset_s(tempStr, sizeof(tempStr), 0x00, sizeof(tempStr));
    if (error != EOK) {
        TELEPHONY_LOGE("DecodeSubAddress memset_s err.");
        return offset;
    }
    if (tempLen + offset >= pduLen) {
        TELEPHONY_LOGE("data length invalid tempLen:%{public}d, pduLen:%{public}d.", tempLen, pduLen);
        return offset;
    }
    if (offset + tempLen >= pduLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }
    if (memcpy_s(tempStr, sizeof(tempStr), &pduStr[offset], tempLen) != EOK) {
        TELEPHONY_LOGE("DecodeSubAddress memcpy_s err.");
        return offset;
    }

    offset += tempLen;
    DecodeSubAddressSZData(tempStr, tempLen, subAddr);
    return offset;
}

void CdmaSmsPduCodec::DecodeSubAddressSZData(unsigned char *tempStr, int tempLen, struct SmsTransSubAddr &subAddr)
{
    int tempOffset = 0;
    if (tempStr == nullptr) {
        TELEPHONY_LOGE("PDU is null!");
        return;
    }
    if (tempOffset >= tempLen) {
        TELEPHONY_LOGE("tempOffset over size");
        return;
    }

    switch (tempStr[tempOffset] & 0xe0) {
        case 0x00:
            subAddr.type = SMS_TRANS_SUB_ADDR_NSAP;
            break;
        case 0x20:
            subAddr.type = SMS_TRANS_SUB_ADDR_USER;
            break;
        default:
            subAddr.type = SMS_TRANS_SUB_ADDR_RESERVED;
            break;
    }
    subAddr.odd = (tempStr[tempOffset] & 0x10) ? true : false;
    ShiftNBitForDecode(tempStr, tempLen, SHIFT_4BITS);

    int numfeilds = tempStr[tempOffset++];
    (void)memset_s(subAddr.szData, sizeof(subAddr.szData), 0x00, sizeof(subAddr.szData));
    if (static_cast<unsigned long>(numfeilds) > sizeof(subAddr.szData) || tempOffset + numfeilds >= tempLen) {
        TELEPHONY_LOGE("data length invalid.");
    }
    if (memcpy_s(subAddr.szData, sizeof(subAddr.szData), tempStr + tempOffset, numfeilds) != EOK) {
        TELEPHONY_LOGE("DecodeSubAddress memcpy_s szdata error.");
    }
}

int CdmaSmsPduCodec::DecodeMsgId(const unsigned char *pduStr, int pduLen, struct SmsTransMsgId &smgId)
{
    int ret;
    int offset = 0;
    unsigned char tempStr[pduLen + 1];

    ret = memset_s(tempStr, sizeof(tempStr), 0x00, sizeof(tempStr));
    if (ret != EOK) {
        TELEPHONY_LOGE("DecodeMsgId memset_s fail.");
        return offset;
    }
    if (MSG_UDID_PARAM_LEN > static_cast<uint8_t>(pduLen)) {
        TELEPHONY_LOGE("data length invalid.");
        return offset;
    }
    if (static_cast<uint8_t>(offset) + HEX_BYTE_STEP + MSG_UDID_PARAM_LEN > static_cast<uint8_t>(pduLen)) {
        TELEPHONY_LOGE("data length invalid.");
        return offset;
    }
    ret = memcpy_s(tempStr, sizeof(tempStr), &pduStr[offset + HEX_BYTE_STEP], MSG_UDID_PARAM_LEN);
    if (ret != EOK) {
        TELEPHONY_LOGE("DecodeMsgId memcpy_s fail.");
        return offset;
    }

    ShiftNBitForDecode(tempStr, MSG_UDID_PARAM_LEN, SHIFT_4BITS);
    if (offset >= pduLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }
    smgId.msgId = tempStr[offset++];

    if (offset >= pduLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }
    smgId.msgId = (smgId.msgId << SHIFT_8BITS) + tempStr[offset++];

    if (offset >= pduLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }
    smgId.headerInd = (tempStr[offset++] & 0x80) ? true : false;
    offset += HEX_BYTE_STEP;
    return offset;
}

void CdmaSmsPduCodec::DecodeCallBackNum(const unsigned char *pduStr, int pduLen, struct SmsTeleSvcAddr &svcAddr)
{
    int offset = 0;
    unsigned char tempStr[pduLen + 1];
    (void)memset_s(&svcAddr, sizeof(SmsTeleSvcAddr), 0x00, sizeof(SmsTeleSvcAddr));

    if (pduStr[offset] & 0x80) {
        svcAddr.digitMode = true;
        svcAddr.numberType = DecodeDigitModeNumberType((pduStr[offset] << SHIFT_1BITS) & 0x70, false);
        svcAddr.numberPlan = DecodeDigitModeNumberPlan(pduStr[offset++] & 0x0f);
        svcAddr.addrLen = pduStr[offset++];
        if (svcAddr.addrLen > sizeof(svcAddr.szData) ||
            static_cast<unsigned long>(offset) + static_cast<unsigned long>(svcAddr.addrLen) >
                static_cast<unsigned long>(pduLen)) {
            TELEPHONY_LOGE("DecodeCallBackNum data length invalid.");
            return;
        }
        if (svcAddr.numberType == SMS_NUMBER_TYPE_INTERNATIONAL) {
            if (memcpy_s(&(svcAddr.szData[1]), sizeof(svcAddr.szData) - 1, pduStr + offset, svcAddr.addrLen) != EOK) {
                TELEPHONY_LOGE("DecodeCallBackNum pduStr memcpy_s err.");
                return;
            }

            if (svcAddr.szData[1] != '\0') {
                svcAddr.szData[0] = '+';
            }
        } else {
            if (memcpy_s(svcAddr.szData, sizeof(svcAddr.szData), pduStr + offset, svcAddr.addrLen) != EOK) {
                TELEPHONY_LOGE("DecodeCallBackNum memcpy_s fail.");
            }
        }
    } else {
        svcAddr.digitMode = false;
        int error = memset_s(tempStr, sizeof(tempStr), 0x00, sizeof(tempStr));
        if (error != EOK) {
            TELEPHONY_LOGE("DecodeCallBackNum memset_s err.");
            return;
        }
        if (memcpy_s(tempStr, sizeof(tempStr), pduStr + offset, pduLen) == EOK) {
            ShiftNBitForDecode(tempStr, pduLen, SHIFT_1BITS);
            svcAddr.addrLen = tempStr[0];
            int bcdLen = svcAddr.addrLen / HEX_BYTE_STEP;
            bcdLen = (svcAddr.addrLen % HEX_BYTE_STEP == 0) ? bcdLen : bcdLen + 1;
            if (bcdLen > SMS_TRANS_ADDRESS_MAX_LEN) {
                TELEPHONY_LOGE("DecodeCallBackNum data length invalid.");
                return;
            }
            SmsCommonUtils::BcdToDigitCdma(&(tempStr[1]), bcdLen, svcAddr.szData);
            svcAddr.szData[svcAddr.addrLen] = '\0';
        }
    }
}

int CdmaSmsPduCodec::DecodeAbsTime(const unsigned char *pduStr, int pduLen, struct SmsTimeAbs &timeAbs)
{
    int offset = 0;
    if (pduStr == nullptr || pduLen <= 0) {
        TELEPHONY_LOGE("PDU is null!");
        return offset;
    }
    if (offset + MIN_PDU_ABS_TIME_LEN >= pduLen) {
        TELEPHONY_LOGE("offset over size");
        return offset;
    }
    timeAbs.year = (((pduStr[offset] & 0xf0) >> SHIFT_4BITS) * DECIMAL_NUM) + (pduStr[offset] & 0x0f);
    offset++;
    timeAbs.month = (((pduStr[offset] & 0xf0) >> SHIFT_4BITS) * DECIMAL_NUM) + (pduStr[offset] & 0x0f);
    offset++;
    timeAbs.day = (((pduStr[offset] & 0xf0) >> SHIFT_4BITS) * DECIMAL_NUM) + (pduStr[offset] & 0x0f);
    offset++;
    timeAbs.hour = (((pduStr[offset] & 0xf0) >> SHIFT_4BITS) * DECIMAL_NUM) + (pduStr[offset] & 0x0f);
    offset++;
    timeAbs.minute = (((pduStr[offset] & 0xf0) >> SHIFT_4BITS) * DECIMAL_NUM) + (pduStr[offset] & 0x0f);
    offset++;
    timeAbs.second = (((pduStr[offset] & 0xf0) >> SHIFT_4BITS) * DECIMAL_NUM) + (pduStr[offset] & 0x0f);
    offset++;
    return offset;
}

int CdmaSmsPduCodec::Encode7BitASCIIData(const struct SmsUserData &userData, unsigned char *dest, int &remainBits)
{
    int shift = 0;
    int offset = 0;
    int fillBits = 0;
    unsigned char udhl = 0x00;
    offset = (userData.headerCnt > 0) ? HEX_BYTE_STEP : 1;
    for (int i = 0; i < userData.headerCnt; i++) {
        int headerLen = GsmSmsUDataCodec::EncodeHeader(userData.header[i], (char *)(&(dest[offset])));
        TELEPHONY_LOGI("headerLen [%{public}d]", headerLen);
        udhl += headerLen;
        offset += headerLen;
    }
    if (udhl > 0) {
        fillBits = ((udhl + 1) * BYTE_BITS) % ENCODE_BYTE_BIT; /* + UDHL */
    }
    if (fillBits > 0) {
        fillBits = ENCODE_BYTE_BIT - fillBits;
        remainBits = BYTE_BIT - fillBits;
    }
    TELEPHONY_LOGI("fillBits [%{public}d] udhl [%{public}d]", fillBits, udhl);
    /* Set UDL, UDHL */
    dest[0] = (udhl > 0) ? (udhl + 1 + userData.length) : userData.length;

    unsigned char *temp = static_cast<unsigned char *>(calloc(BYTE_STEP, userData.length + 0x01));
    if (temp == nullptr) {
        TELEPHONY_LOGE("SmsUserData is null!");
        return shift;
    }
    for (int i = 0; i < userData.length; i++) {
        temp[i] = static_cast<unsigned char>(userData.data[i]) << SHIFT_1BITS;
    }

    int j = 0;
    for (int i = 0; i < userData.length; i++) {
        shift = j % ENCODE_GSM_BIT;
        dest[offset + j] = (temp[i] << shift) + (temp[i + 0x01] >> (ENCODE_GSM_BIT - shift));
        j++;
        if (shift == 0x06) {
            i++;
        }
    }
    if (temp) {
        free(temp);
        temp = nullptr;
    }

    if (fillBits > 0) {
        ShiftRNBit(&dest[offset], j, fillBits);
        ++j;
    }
    return j + offset;
}

int CdmaSmsPduCodec::Encode7BitGSMData(const struct SmsUserData &userData, unsigned char *dest, int &remainBits)
{
    int offset = 0;
    int fillBits = 0;
    int packSize = 0;
    int encodeLen = 0;

    unsigned char udhl = 0x00;
    offset = (userData.headerCnt > 0) ? HEX_BYTE_STEP : 1;

    for (int i = 0; i < userData.headerCnt; i++) {
        int headerLen = GsmSmsUDataCodec::EncodeHeader(userData.header[i], (char *)&(dest[offset]));
        TELEPHONY_LOGI("headerLen [%{public}d]", headerLen);
        udhl += headerLen;
        offset += headerLen;
    }
    TELEPHONY_LOGI("udhl [%{public}u]", udhl);
    if (udhl > 0) {
        fillBits = ((udhl + 1) * BYTE_BIT) % ENCODE_BYTE_BIT; /* + UDHL */
    }
    if (fillBits > 0) {
        fillBits = ENCODE_BYTE_BIT - fillBits;
        remainBits = BYTE_BIT - fillBits;
    }
    TELEPHONY_LOGI("fillBits [%{public}d] dataLen [%{public}d]", fillBits, userData.length);
    /* Set UDL, UDHL */
    if (udhl > 0) {
        dest[0] = udhl + 1 + userData.length;
        dest[1] = udhl;
    } else {
        dest[0] = userData.length;
    }

    packSize = SmsCommonUtils::Pack7bitChar(
        reinterpret_cast<const unsigned char *>(userData.data), userData.length, fillBits, &dest[offset]);
    encodeLen = offset + packSize;
    TELEPHONY_LOGI("packSize [%{public}d] encodeLen [%{public}d]", packSize, encodeLen);
    if (fillBits > 0) {
        int offsetIndex = 0;
        ShiftRNBit(&dest[offsetIndex], packSize, fillBits);
        ++encodeLen;
    }
    return encodeLen;
}

int CdmaSmsPduCodec::EncodeUCS2Data(const struct SmsUserData &userData, unsigned char *dest, int &remainBits)
{
    int offset = 0;
    int encodeLen = 0;

    unsigned char udhl = 0x00;
    offset = (userData.headerCnt > 0) ? HEX_BYTE_STEP : 1;

    for (int i = 0; i < userData.headerCnt; i++) {
        int headerLen = GsmSmsUDataCodec::EncodeHeader(userData.header[i], (char *)&(dest[offset]));
        TELEPHONY_LOGI("headerLen [%{public}d]", headerLen);
        udhl += headerLen;
    }
    TELEPHONY_LOGI("udhl [%{public}u]", udhl);
    /* Set UDL, UDHL */
    if (udhl > 0) {
        unsigned char udhBytes = udhl + 1;
        unsigned char udhCodeUnits = (udhBytes + 1) / HEX_BYTE_STEP;
        unsigned char udCodeUnits = userData.length / HEX_BYTE_STEP;
        dest[0] = udhCodeUnits + udCodeUnits;
        dest[1] = udhl;
        offset += udhCodeUnits * HEX_BYTE_STEP - 1;
    } else {
        dest[0] = (char)userData.length / HEX_BYTE_STEP;
    }

    if (userData.length > static_cast<int>(MAX_TPDU_DATA_LEN - offset)) {
        TELEPHONY_LOGE("data length invalid.");
        return encodeLen;
    }
    if (memcpy_s(&dest[offset], MAX_TPDU_DATA_LEN - offset, userData.data, userData.length) != EOK) {
        TELEPHONY_LOGE("EncodeUCS2Data memcpy_s error");
        return encodeLen;
    }
    remainBits = 0;
    encodeLen = offset + userData.length;
    return encodeLen;
}

void CdmaSmsPduCodec::DecodeCMASData(unsigned char *pduStr, int pduLen, struct SmsTeleSvcCmasData &cmasData)
{
    int offset = 0;
    if (memset_s(&cmasData, sizeof(SmsTeleSvcCmasData), 0x00, sizeof(SmsTeleSvcCmasData)) != EOK) {
        TELEPHONY_LOGE("DecodeCMASData memset_s fail.");
        return;
    }
    if ((pduStr[offset] & 0xf8) != 0x00) {
        TELEPHONY_LOGE("Wrong Encode Type = [%{public}d]!! must be 0", (pduStr[offset] & 0xf8) >> SHIFT_3BITS);
        return;
    }
    ShiftNBitForDecode(pduStr, pduLen, SHIFT_5BITS);
    offset++;
    if (pduStr[offset++] != 0x00) {
        TELEPHONY_LOGE("Wrong protocol version = [%{public}d]!! must be 0", pduStr[offset - 1]);
        cmasData.isWrongRecodeType = true;
        return;
    }
    while (offset < pduLen - 1) {
        if (pduStr[offset] == 0x00) {
            offset++;
            offset += DecodeCMASType0Data(&pduStr[offset], pduLen - offset, cmasData);
        } else if (pduStr[offset] == 0x01) {
            offset += HEX_BYTE_STEP;
            if (offset >= pduLen) {
                TELEPHONY_LOGE("offset over size");
                return;
            }
            TELEPHONY_LOGI("Type 1 length = [%{public}d]", pduStr[offset - 1]);
            cmasData.category = static_cast<enum SmsCmaeCategory>(pduStr[offset++]);
            cmasData.responseType = static_cast<enum SmsCmaeResponseType>(pduStr[offset++]);
            cmasData.severity = static_cast<enum SmsCmaeSeverity>(pduStr[offset] >> SHIFT_4BITS);
            cmasData.urgency = static_cast<enum SmsCmaeUrgency>(pduStr[offset++] & 0x0f);
            cmasData.certainty = static_cast<enum SmsCmaeCertainty>(pduStr[offset++] >> SHIFT_4BITS);
        } else if (pduStr[offset] == 0x02) {
            offset += HEX_BYTE_STEP;
            if (offset >= pduLen) {
                TELEPHONY_LOGE("offset over size");
                return;
            }
            TELEPHONY_LOGI("Type 2 length = [%{public}d]", pduStr[offset - 1]);
            cmasData.id = pduStr[offset++];
            cmasData.id = (cmasData.id << SHIFT_8BITS) + pduStr[offset++];
            cmasData.alertHandle = static_cast<enum SmsCmaeAlertHandle>(pduStr[offset++]);
            if (offset + MIN_PDU_ABS_TIME_LEN >= pduLen) {
                TELEPHONY_LOGE("offset over size");
                return;
            }
            offset += DecodeAbsTime(pduStr + offset, pduLen, cmasData.expires);
            cmasData.language = static_cast<enum SmsLanguageType>(pduStr[offset++]);
        } else {
            offset++;
        }
        TELEPHONY_LOGI("offset = [%{public}d], pduLen = [%{public}d]", offset, pduLen);
    }
}

int CdmaSmsPduCodec::DecodeCMASType0Data(unsigned char *pduStr, int pduLen, struct SmsTeleSvcCmasData &cmasData)
{
    int offset = 0;
    size_t tempLen = 0;
    if (pduStr == nullptr || pduLen <= 0) {
        TELEPHONY_LOGE("PDU is null!");
        return offset;
    }
    unsigned char tempStr[pduLen + 1];
    tempLen = pduStr[offset++];
    TELEPHONY_LOGI("Type 0 length = [%{public}zu]", tempLen);
    int error = memset_s(tempStr, sizeof(tempStr), 0x00, sizeof(tempStr));
    if (error != EOK) {
        TELEPHONY_LOGE("DecodeCMASType0Data memset_s err.");
        return offset;
    }
    if (tempLen > static_cast<size_t>(pduLen + 1) ||
        static_cast<size_t>(offset) + tempLen > static_cast<size_t>(pduLen)) {
        TELEPHONY_LOGE("data length invalid tempLen:%{public}zu, pduLen:%{public}d.", tempLen, pduLen);
        return offset;
    }
    if (memcpy_s(tempStr, sizeof(tempStr), pduStr + offset, tempLen) != EOK) {
        TELEPHONY_LOGE("DecodeCMASType0Data memset_s err.");
        return offset;
    }

    cmasData.encodeType = FindMsgEncodeType(tempStr[0] & 0xf8);
    ShiftNBitForDecode(tempStr, tempLen, SHIFT_5BITS);

    DecodeCMASType0TempData(tempStr, tempLen, cmasData);
    offset += tempLen;
    return offset;
}

int CdmaSmsPduCodec::DecodeCMASType0TempData(unsigned char *tempStr, int tempLen, struct SmsTeleSvcCmasData &cmasData)
{
    int offset = 0;
    if (tempStr == nullptr) {
        TELEPHONY_LOGE("PDU is null!");
        return offset;
    }
    switch (cmasData.encodeType) {
        case SMS_ENCODE_7BIT_ASCII:
        case SMS_ENCODE_IA5:
        case SMS_ENCODE_GSM7BIT:
            cmasData.dataLen = (tempLen * BYTE_BITS - SHIFT_5BITS) / ENCODE_GSM_BIT;
            for (unsigned int i = 0; i < cmasData.dataLen; i++) {
                cmasData.alertText[i] = tempStr[0] >> SHIFT_1BITS;
                ShiftNBitForDecode(tempStr, tempLen, SHIFT_7BITS);
            }
            break;
        case SMS_ENCODE_EPM:
        case SMS_ENCODE_GSMDCS:
            break;
        default:
            cmasData.dataLen = (tempLen == 0) ? 0 : (tempLen - 1);
            if (static_cast<size_t>(tempLen - 1) > sizeof(cmasData.alertText) ||
                static_cast<size_t>(offset) > static_cast<size_t>(tempLen)) {
                TELEPHONY_LOGE("data length invalid.");
                return offset;
            }
            if (memcpy_s(cmasData.alertText, sizeof(cmasData.alertText), tempStr + offset, tempLen - 1) != EOK) {
                TELEPHONY_LOGE("cmasData alertText memcpy_s fail.");
            }
            break;
    }
    return offset;
}

void CdmaSmsPduCodec::DecodeUserData(
    unsigned char *pduStr, int pduLen, struct SmsTeleSvcUserData &userData, bool headerInd)
{
    int offset = 0;
    if (pduStr == nullptr || pduLen <= 0) {
        TELEPHONY_LOGE("PDU is null!");
        return;
    }

    userData.encodeType = FindMsgEncodeType(pduStr[offset] & 0xf8);
    ShiftNBitForDecode(pduStr, pduLen, SHIFT_5BITS);
    if (userData.encodeType == SMS_ENCODE_EPM || userData.encodeType == SMS_ENCODE_GSMDCS) {
        userData.msgType = pduStr[offset];
        ShiftNBitForDecode(pduStr, pduLen, SHIFT_8BITS);
    }

    if (memset_s(userData.userData.data, sizeof(userData.userData.data), 0x00, sizeof(userData.userData.data)) !=
        EOK) {
        TELEPHONY_LOGE("DecodeUserData memset_s err.");
        return;
    }

    unsigned char numFields = pduStr[offset++];
    unsigned char udhlBytes = headerInd ? pduStr[offset++] : 0;
    TELEPHONY_LOGI("numFields %{public}d  udhlBytes %{public}d", numFields, udhlBytes);
    Decode7BitHeader(&pduStr[offset], pduLen, udhlBytes, userData.userData);
    offset += udhlBytes;
    DecodeUserDataEncodeType(&pduStr[offset], pduLen, userData, headerInd, numFields, udhlBytes);
}

void CdmaSmsPduCodec::DecodeUserDataEncodeType(unsigned char *pduStr, int pduLen, struct SmsTeleSvcUserData &userData,
    bool headerInd, unsigned char numFields, unsigned char udhlBytes)
{
    int offset = 0;
    if (pduStr == nullptr || pduLen <= 0) {
        TELEPHONY_LOGE("PDU is null!");
        return;
    }

    switch (userData.encodeType) {
        case SMS_ENCODE_7BIT_ASCII:
        case SMS_ENCODE_IA5:
            DecodeUserDataEncodeASCIIType(&pduStr[offset], pduLen, userData, headerInd, numFields, udhlBytes);
            break;
        case SMS_ENCODE_GSM7BIT:
            DecodeUserDataEncodeGSM7Type(&pduStr[offset], pduLen, userData, headerInd, numFields, udhlBytes);
            break;
        case SMS_ENCODE_EPM:
        case SMS_ENCODE_GSMDCS:
            break;
        case SMS_ENCODE_UNICODE:
            DecodeUserDataEncodeUnicodeType(&pduStr[offset], pduLen, userData, headerInd, numFields, udhlBytes);
            break;
        default:
            DecodeUserDataEncodeDefaultType(&pduStr[offset], pduLen, userData, headerInd);
            break;
    }
}

void CdmaSmsPduCodec::DecodeUserDataEncodeASCIIType(unsigned char *pduStr, int pduLen,
    struct SmsTeleSvcUserData &userData, bool headerInd, unsigned char numFields, unsigned char udhlBytes)
{
    int offset = 0;
    if (pduStr == nullptr || pduLen <= 0) {
        TELEPHONY_LOGE("PDU is null!");
        return;
    }
    unsigned char fillBits;

    fillBits = headerInd ? ((udhlBytes + 1) * BYTE_BIT) % ENCODE_BYTE_BIT : 0;
    fillBits = fillBits > 0 ? ENCODE_BYTE_BIT - fillBits : 0;
    if (offset >= pduLen) {
        TELEPHONY_LOGE("data length error, pduLen %{public}d  offset %{public}d", pduLen, offset);
        return;
    }
    ShiftNBitForDecode(&pduStr[offset], (unsigned int)(pduLen - offset), fillBits);
    if (numFields - udhlBytes - 1 <= 0) {
        TELEPHONY_LOGE("userData.userData.length error");
        return;
    }
    userData.userData.length = headerInd ? (numFields - udhlBytes - 1) : numFields;
    for (int i = 0; i < userData.userData.length; i++) {
        userData.userData.data[i] = pduStr[offset] >> SHIFT_1BITS;
        ShiftNBitForDecode(&pduStr[offset], pduLen, SHIFT_7BITS);
    }
}

void CdmaSmsPduCodec::DecodeUserDataEncodeGSM7Type(unsigned char *pduStr, int pduLen,
    struct SmsTeleSvcUserData &userData, bool headerInd, unsigned char numFields, unsigned char udhlBytes)
{
    int offset = 0;
    if (pduStr == nullptr || pduLen <= 0) {
        TELEPHONY_LOGE("PDU is null!");
        return;
    }
    unsigned char fillBits;

    fillBits = headerInd ? ((udhlBytes + 1) * BYTE_BIT) % ENCODE_BYTE_BIT : 0;
    fillBits = fillBits > 0 ? ENCODE_BYTE_BIT - fillBits : 0;
    if (pduLen < offset) {
        TELEPHONY_LOGE("data length error, pduLen %{public}d  offset %{public}d", pduLen, offset);
        return;
    }
    ShiftNBitForDecode(&pduStr[offset], (unsigned int)(pduLen - offset), fillBits);

    if (numFields - udhlBytes - 1 <= 0) {
        TELEPHONY_LOGE("userData.userData.length error");
        return;
    }
    userData.userData.length = headerInd ? (numFields - udhlBytes - 1) : numFields;
    SmsCommonUtils::Unpack7bitChar(&(pduStr[offset]), userData.userData.length, 0x00,
        reinterpret_cast<unsigned char *>(userData.userData.data), MAX_USER_DATA_LEN + 1);
}

void CdmaSmsPduCodec::DecodeUserDataEncodeUnicodeType(unsigned char *pduStr, int pduLen,
    struct SmsTeleSvcUserData &userData, bool headerInd, unsigned char numFields, unsigned char udhlBytes)
{
    int offset = 0;
    if (pduStr == nullptr || pduLen <= 0) {
        TELEPHONY_LOGE("PDU is null!");
        return;
    }
    unsigned char fillBits;

    if (headerInd) {
        fillBits = ((udhlBytes + 1) % HEX_BYTE_STEP == 0) ? 0 : 0x08;
        offset += (fillBits > 0) ? 1 : 0;
        userData.userData.length = numFields * HEX_BYTE_STEP - (udhlBytes + 1);
    } else {
        userData.userData.length = numFields * HEX_BYTE_STEP;
    }
    if (static_cast<unsigned long>(userData.userData.length) + static_cast<unsigned long>(offset) >
            sizeof(userData.userData.data) |
        static_cast<unsigned long>(userData.userData.length) + static_cast<unsigned long>(offset) >
            static_cast<size_t>(pduLen)) {
        TELEPHONY_LOGE("data length error.");
        return;
    }
    if (memcpy_s(userData.userData.data, sizeof(userData.userData.data), pduStr + offset, userData.userData.length) !=
        EOK) {
        TELEPHONY_LOGE("SMS_ENCODE_UNICODE memcpy_s err.");
    }
}

void CdmaSmsPduCodec::DecodeUserDataEncodeDefaultType(
    unsigned char *pduStr, int pduLen, struct SmsTeleSvcUserData &userData, bool headerInd)
{
    int offset = 0;
    if (pduStr == nullptr || pduLen <= 0) {
        TELEPHONY_LOGE("PDU is null!");
        return;
    }

    if (static_cast<unsigned long>(userData.userData.length) + static_cast<unsigned long>(offset) >
            sizeof(userData.userData.data) |
        static_cast<unsigned long>(userData.userData.length) + static_cast<unsigned long>(offset) >
            static_cast<size_t>(pduLen)) {
        TELEPHONY_LOGE("data length error.");
        return;
    }
    if (memcpy_s(userData.userData.data, sizeof(userData.userData.data), pduStr + offset, userData.userData.length) !=
        EOK) {
        TELEPHONY_LOGE("unkown encodeType memcpy_s err.");
    }
}

/**
 * @brief Decode7BitHeader
 * Decode User Data Header
 * @param pduStr
 * @param udhlBytes
 * @param userData
 */
void CdmaSmsPduCodec::Decode7BitHeader(
    const unsigned char *pduStr, int pduLen, unsigned char udhlBytes, struct SmsUserData &userData)
{
    if (udhlBytes > 0) {
        int offset = 0;
        userData.headerCnt = 0;
        for (int i = 0; offset < udhlBytes; i++) {
            int headerLen = GsmSmsUDataCodec::DecodeHeader(&(pduStr[offset]), pduLen, &(userData.header[i]));
            if (headerLen <= 0) {
                TELEPHONY_LOGI("Error to Header. headerLen [%{public}d]", headerLen);
                GsmSmsUDataCodec::ResetUserData(userData);
                return;
            }
            offset += headerLen;
            if (offset > (udhlBytes + HEX_BYTE_STEP)) {
                TELEPHONY_LOGI("Error to Header. offset [%{public}d] > (udhl [%{public}d] + 2)", offset, udhlBytes);
                GsmSmsUDataCodec::ResetUserData(userData);
                return;
            }
            userData.headerCnt++;
        }
    } else {
        userData.headerCnt = 0;
    }
}

enum SmsMessageType CdmaSmsPduCodec::FindMsgType(const std::vector<unsigned char> &pduStr)
{
    std::size_t offset = 0;
    while (offset + 1 < pduStr.size()) {
        if (pduStr[offset] == 0x00 && offset + HEX_BYTE_STEP < pduStr.size()) {
            return static_cast<enum SmsMessageType>((pduStr[offset + HEX_BYTE_STEP] & 0xf0) >> SHIFT_4BITS);
        }
        offset += (pduStr[offset + 1] + HEX_BYTE_STEP);
    }
    return SMS_TYPE_MAX_VALUE;
}

enum SmsEncodingType CdmaSmsPduCodec::FindMsgEncodeType(const unsigned char value)
{
    enum SmsEncodingType encodeType = SMS_ENCODE_RESERVED;
    unsigned char temp = (value >> SHIFT_3BITS);
    if (temp >= SMS_ENCODE_OCTET && temp <= SMS_ENCODE_GSMDCS) {
        encodeType = static_cast<SmsEncodingType>(temp);
    }
    if (temp == SMS_ENCODE_EUCKR) {
        return SMS_ENCODE_EUCKR;
    }
    return encodeType;
}
} // namespace Telephony
} // namespace OHOS