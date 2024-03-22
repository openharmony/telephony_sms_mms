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

#include "gsm_sms_tpdu_decode.h"

#include "gsm_pdu_hex_value.h"
#include "gsm_user_data_pdu.h"
#include "securec.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
using namespace std;
GsmSmsTpduDecode::GsmSmsTpduDecode(std::shared_ptr<GsmUserDataPdu> dataCodec,
    std::shared_ptr<GsmSmsParamCodec> paramCodec, std::shared_ptr<GsmSmsTpduCodec> tpdu)
{
    uDataCodec_ = dataCodec;
    paramCodec_ = paramCodec;
    tpdu_ = tpdu;
}

GsmSmsTpduDecode::~GsmSmsTpduDecode() {}

bool GsmSmsTpduDecode::DecodeSubmit(SmsReadBuffer &buffer, struct SmsSubmit *submit)
{
    if (submit == nullptr || tpdu_ == nullptr) {
        TELEPHONY_LOGE("nullptr error.");
        return false;
    }

    tpdu_->DebugTpdu(buffer, PARSE_SUBMIT_TYPE);
    // TP-RD
    uint8_t oneByte = 0;
    if (!buffer.PickOneByte(oneByte)) {
        TELEPHONY_LOGE("get data error.");
        return false;
    }
    if (oneByte & HEX_VALUE_04) {
        submit->bRejectDup = false;
    } else {
        submit->bRejectDup = true;
    }
    // TP-VPF
    if (!buffer.PickOneByte(oneByte)) {
        TELEPHONY_LOGE("get data error.");
        return false;
    }
    submit->vpf = static_cast<enum SmsVpf>(oneByte & HEX_VALUE_18);
    // TP-SRR
    if (!buffer.PickOneByte(oneByte)) {
        TELEPHONY_LOGE("get data error.");
        return false;
    }
    if (oneByte & HEX_VALUE_20) {
        submit->bStatusReport = true;
    } else {
        submit->bStatusReport = false;
    }
    // TP-UDHI
    if (!buffer.PickOneByte(oneByte)) {
        TELEPHONY_LOGE("get data error.");
        return false;
    }
    if (oneByte & HEX_VALUE_40) {
        submit->bHeaderInd = true;
    } else {
        submit->bHeaderInd = false;
    }
    return DecodeSubmitPartData(buffer, submit);
}

bool GsmSmsTpduDecode::DecodeSubmitPartData(SmsReadBuffer &buffer, struct SmsSubmit *submit)
{
    if (paramCodec_ == nullptr || uDataCodec_ == nullptr) {
        TELEPHONY_LOGE("nullptr error.");
        return false;
    }
    uint8_t oneByte = 0;
    // TP-RP
    if (!buffer.PickOneByte(oneByte)) {
        TELEPHONY_LOGE("get data error.");
        return false;
    }
    if (oneByte & HEX_VALUE_80) {
        submit->bReplyPath = true;
    } else {
        submit->bReplyPath = false;
    }
    buffer.MoveForward(1);

    // TP-MR
    if (!buffer.ReadByte(oneByte)) {
        TELEPHONY_LOGE("get data error.");
        return false;
    }
    submit->msgRef = oneByte;
    // TP-DA
    if (!paramCodec_->DecodeAddressPdu(buffer, &(submit->destAddress))) {
        TELEPHONY_LOGE("DecodeAddressPdu Fail");
        return false;
    }
    // TP-PID
    if (!buffer.ReadByte(oneByte)) {
        TELEPHONY_LOGE("get data error.");
        return false;
    }
    submit->pid = tpdu_->ParsePid(oneByte);
    // TP-DCS
    if (!paramCodec_->DecodeDcsPdu(buffer, &(submit->dcs))) {
        TELEPHONY_LOGE("DecodeDcsPdu Fail");
        return false;
    }
    // TP-VP
    if (submit->vpf != SMS_VPF_NOT_PRESENT) {
        // Decode VP
    }
    // TP-UDL & TP-UD
    bool result =
        uDataCodec_->DecodeUserDataPdu(buffer, submit->bHeaderInd, submit->dcs.codingScheme, &(submit->userData));
    TELEPHONY_LOGI("decode submit result : =%{public}d", result);
    return result;
}

void DecodePartData(const uint8_t &pTpdu, struct SmsDeliver &deliver)
{
    /* TP-MMS */
    if (pTpdu & HEX_VALUE_04) {
        deliver.bMoreMsg = false;
    } else {
        deliver.bMoreMsg = true;
    }
    /* TP-SRI */
    if (pTpdu & HEX_VALUE_20) {
        deliver.bStatusReport = true;
    } else {
        deliver.bStatusReport = false;
    }
    /* TP-UDHI */
    if (pTpdu & HEX_VALUE_40) {
        deliver.bHeaderInd = true;
    } else {
        deliver.bHeaderInd = false;
    }
    /* TP-RP */
    if (pTpdu & HEX_VALUE_80) {
        deliver.bReplyPath = true;
    } else {
        deliver.bReplyPath = false;
    }
}

bool GsmSmsTpduDecode::DecodeDeliver(SmsReadBuffer &buffer, struct SmsDeliver *deliver)
{
    if (deliver == nullptr || tpdu_ == nullptr || paramCodec_ == nullptr) {
        TELEPHONY_LOGE("nullptr error.");
        return false;
    }

    uint8_t oneByte = 0;
    if (!buffer.ReadByte(oneByte)) {
        TELEPHONY_LOGE("get data error.");
        return false;
    }
    tpdu_->DebugTpdu(buffer, PARSE_DELIVER_TYPE);
    DecodePartData(oneByte, *deliver);

    /* TP-OA */
    if (!paramCodec_->DecodeAddressPdu(buffer, &(deliver->originAddress))) {
        TELEPHONY_LOGE("DecodeAddressPdu fail");
        return false;
    }

    /* TP-PID */
    if (!buffer.ReadByte(oneByte)) {
        TELEPHONY_LOGE("get data error.");
        return false;
    }
    deliver->pid = tpdu_->ParsePid(oneByte);
    /* TP-DCS */
    if (!paramCodec_->DecodeDcsPdu(buffer, &(deliver->dcs))) {
        TELEPHONY_LOGE("decode dcs fail.");
        return false;
    }
    /* Support KSC5601 :: Coding group bits == 0x84 */
    if (!buffer.PickOneByteFromIndex(buffer.GetIndex() - 1, oneByte)) {
        TELEPHONY_LOGE("get data error.");
        return false;
    }
    if (oneByte == HEX_VALUE_84) {
        deliver->dcs.codingScheme = DATA_CODING_EUCKR;
    }
    return DecodeDeliverPartData(buffer, deliver);
}

bool GsmSmsTpduDecode::DecodeDeliverPartData(SmsReadBuffer &buffer, struct SmsDeliver *deliver)
{
    if (uDataCodec_ == nullptr) {
        TELEPHONY_LOGE("nullptr error.");
        return false;
    }

    if (deliver->pid == HEX_VALUE_20 && deliver->originAddress.ton == TYPE_ALPHA_NUMERIC) {
        int32_t setType = -1;
        int32_t indType = -1;
        bool voicemail = paramCodec_->CheckVoicemail(buffer, &setType, &indType);
        if (voicemail) {
            deliver->dcs.bMWI = true;
            if (setType == 0) {
                deliver->dcs.bIndActive = false;
            } else {
                deliver->dcs.bIndActive = true;
            }
            if (indType == 0) {
                deliver->dcs.indType = SMS_VOICE_INDICATOR;
            } else if (indType == 1) {
                deliver->dcs.indType = SMS_VOICE2_INDICATOR;
            }
        }
    }

    /* TP-SCTS */
    if (!paramCodec_->DecodeTimePdu(buffer, &(deliver->timeStamp))) {
        TELEPHONY_LOGE("decode time fail.");
        return false;
    }
    /* TP-UD */
    bool result = uDataCodec_->DecodeUserDataPdu(
        buffer, deliver->bHeaderInd, deliver->dcs.codingScheme, &(deliver->userData), &(deliver->udData));
    if (!result) {
        TELEPHONY_LOGE("decode delivery fail.");
    }
    return result;
}

bool GsmSmsTpduDecode::DecodeStatusReport(SmsReadBuffer &buffer, struct SmsStatusReport *statusRep)
{
    /* TP-MMS */
    if (statusRep == nullptr || paramCodec_ == nullptr) {
        TELEPHONY_LOGE("nullptr error.");
        return false;
    }

    uint8_t oneByte = 0;
    if (!buffer.PickOneByte(oneByte)) {
        TELEPHONY_LOGE("get data error.");
        return false;
    }
    statusRep->bMoreMsg = (oneByte & HEX_VALUE_04) ? false : true;
    /* TP-SRQ */

    if (!buffer.PickOneByte(oneByte)) {
        TELEPHONY_LOGE("get data error.");
        return false;
    }
    statusRep->bStatusReport = (oneByte & HEX_VALUE_20) ? true : false;
    /* TP-UDHI */
    if (!buffer.ReadByte(oneByte)) {
        TELEPHONY_LOGE("get data error.");
        return false;
    }
    statusRep->bHeaderInd = (oneByte & HEX_VALUE_40) ? true : false;
    /* TP-MR */
    if (!buffer.ReadByte(oneByte)) {
        TELEPHONY_LOGE("get data error.");
        return false;
    }
    statusRep->msgRef = oneByte;
    /* TP-RA */

    if (!paramCodec_->DecodeAddressPdu(buffer, &(statusRep->recipAddress))) {
        TELEPHONY_LOGE("decode addr fail.");
        return false;
    }

    /* TP-SCTS */
    /* Decode timestamp */
    if (!paramCodec_->DecodeTimePdu(buffer, &(statusRep->timeStamp))) {
        TELEPHONY_LOGE("decode time fail.");
        return false;
    }
    return DecodeStatusReportPartData(buffer, statusRep);
}

bool GsmSmsTpduDecode::DecodeStatusReportPartData(SmsReadBuffer &buffer, struct SmsStatusReport *statusRep)
{
    /* TP-DT */
    /* Decode timestamp */
    if (!paramCodec_->DecodeTimePdu(buffer, &(statusRep->dischargeTime))) {
        TELEPHONY_LOGE("decode time fail.");
        return false;
    }
    /* TP-Status */
    uint8_t oneByte = 0;
    if (!buffer.ReadByte(oneByte)) {
        TELEPHONY_LOGE("get data error.");
        return false;
    }
    statusRep->status = oneByte;
    /* TP-PI */
    if (!buffer.ReadByte(oneByte)) {
        TELEPHONY_LOGE("no pdu data remains.");
        return true;
    }
    statusRep->paramInd = oneByte;
    /* No Parameters */
    if (statusRep->paramInd == 0) {
        statusRep->pid = SMS_NORMAL_PID;
        statusRep->dcs.bCompressed = false;
        statusRep->dcs.bMWI = false;
        statusRep->dcs.bIndActive = false;
        statusRep->dcs.msgClass = SMS_CLASS_UNKNOWN;
        statusRep->dcs.codingScheme = DATA_CODING_7BIT;
        statusRep->dcs.codingGroup = CODING_GENERAL_GROUP;
        statusRep->dcs.indType = SMS_OTHER_INDICATOR;
        statusRep->userData.headerCnt = 0;
        statusRep->userData.length = 0;
        int ret = memset_s(statusRep->userData.data, sizeof(statusRep->userData.data), 0x00, MAX_USER_DATA_LEN + 1);
        if (ret != EOK) {
            TELEPHONY_LOGE("memset_s error.");
            return false;
        }
    }
    return DecodeStatusReportData(buffer, statusRep);
}

bool GsmSmsTpduDecode::DecodeStatusReportData(SmsReadBuffer &buffer, struct SmsStatusReport *statusRep)
{
    if (tpdu_ == nullptr || uDataCodec_ == nullptr) {
        TELEPHONY_LOGE("nullptr error.");
        return false;
    }

    /* TP-PID */
    if (statusRep->paramInd & HEX_VALUE_01) {
        uint8_t oneByte = 0;
        if (!buffer.ReadByte(oneByte)) {
            TELEPHONY_LOGE("get data error.");
            return false;
        }
        statusRep->pid = tpdu_->ParsePid(oneByte);
    }
    /* TP-DCS */
    if (statusRep->paramInd & HEX_VALUE_02) {
        if (!paramCodec_->DecodeDcsPdu(buffer, &(statusRep->dcs))) {
            TELEPHONY_LOGE("decode dcs fail.");
            return false;
        }
    }
    /* TP-UDL & TP-UD */
    bool result = true;
    if (statusRep->paramInd & HEX_VALUE_04) {
        /* Decode User Data */
        result = uDataCodec_->DecodeUserDataPdu(
            buffer, statusRep->bHeaderInd, statusRep->dcs.codingScheme, &(statusRep->userData));
    }
    TELEPHONY_LOGI("decode status result : =%{public}d", result);
    return result;
}
} // namespace Telephony
} // namespace OHOS