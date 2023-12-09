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

#include "gsm_sms_tpdu_encode.h"

#include "gsm_pdu_hex_value.h"
#include "gsm_user_data_pdu.h"
#include "securec.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
using namespace std;
const std::string CT_SMSC = "10659401";

GsmSmsTpduEncode::GsmSmsTpduEncode(std::shared_ptr<GsmUserDataPdu> dataCodec,
    std::shared_ptr<GsmSmsParamCodec> paramCodec, std::shared_ptr<GsmSmsTpduCodec> tpdu)
{
    uDataCodec_ = dataCodec;
    paramCodec_ = paramCodec;
    tpdu_ = tpdu;
}

GsmSmsTpduEncode::~GsmSmsTpduEncode() {}

bool GsmSmsTpduEncode::EncodeSubmitPdu(SmsWriteBuffer &buffer, const struct SmsSubmit *pSubmit)
{
    if (pSubmit == nullptr || paramCodec_ == nullptr || uDataCodec_ == nullptr) {
        TELEPHONY_LOGE("nullptr error");
        return false;
    }
    EncodeSubmitTpduType(buffer, *pSubmit);

    /* TP-MR */
    if (!buffer.WriteByte(pSubmit->msgRef)) {
        TELEPHONY_LOGE("write data error.");
        return false;
    }

    /* TP-DA */
    std::string address;
    if (!paramCodec_->EncodeAddressPdu(&pSubmit->destAddress, address)) {
        TELEPHONY_LOGE("encode address fail.");
        return false;
    }

    uint8_t position = address.size();
    if (buffer.data_ == nullptr || (buffer.GetIndex() + position) > buffer.GetSize()) {
        TELEPHONY_LOGE("buffer error.");
        return false;
    }
    if (memcpy_s(buffer.data_.get() + buffer.GetIndex(), position, address.data(), position) != EOK) {
        TELEPHONY_LOGE("memcpy_s error.");
        return false;
    }
    buffer.MoveForward(position);
    /* TP-PID */
    if (!buffer.WriteByte(pSubmit->pid)) {
        TELEPHONY_LOGE("write data error.");
        return false;
    }
    return EncodeSubmitPduPartData(buffer, pSubmit);
}

bool GsmSmsTpduEncode::EncodeSubmitPduPartData(SmsWriteBuffer &buffer, const struct SmsSubmit *pSubmit)
{
    /* TP-DCS */
    std::string dcs;
    paramCodec_->EncodeDCS(&pSubmit->dcs, dcs);
    if (buffer.data_ == nullptr || (buffer.GetIndex() + HEX_VALUE_01) > buffer.GetSize()) {
        TELEPHONY_LOGE("buffer error.");
        return false;
    }
    uint16_t bufferLen = buffer.GetSize() - buffer.GetIndex() - HEX_VALUE_01;
    if (memcpy_s(buffer.data_.get() + buffer.GetIndex(), bufferLen, dcs.data(), HEX_VALUE_01) != EOK) {
        TELEPHONY_LOGE("memcpy_s error.");
        return false;
    }
    buffer.MoveForward(HEX_VALUE_01);
    /* TP-VP */
    std::string vpTime;
    if (pSubmit->vpf != SMS_VPF_NOT_PRESENT) {
        paramCodec_->EncodeTimePdu(&pSubmit->validityPeriod, vpTime);
        uint8_t position = vpTime.size();
        if (position > 0) {
            if (buffer.data_ == nullptr || (buffer.GetIndex() + position) > buffer.GetSize()) {
                TELEPHONY_LOGE("buffer error.");
                return false;
            }
            if (memcpy_s(buffer.data_.get() + buffer.GetIndex(), position, vpTime.data(), position) != EOK) {
                TELEPHONY_LOGE("EncodeSubmitPdu vpTime memcpy_s error.");
                return false;
            }
            buffer.MoveForward(position);
        }
    }
    const struct SmsUDPackage *pUserData = &(pSubmit->userData);
    std::string addr = pSubmit->destAddress.address;
    bool ret = uDataCodec_->EncodeUserDataPdu(buffer, pUserData, pSubmit->dcs.codingScheme, addr);
    return ret;
}

void GsmSmsTpduEncode::EncodeSubmitTpduType(SmsWriteBuffer &buffer, const struct SmsSubmit &pSubmit)
{
    if (!buffer.InsertByte(HEX_VALUE_01, buffer.GetIndex())) {
        TELEPHONY_LOGE("write data error.");
        return;
    }
    /* TP-RD */
    if (pSubmit.bRejectDup) {
        uint8_t oneByte = 0;
        if (!buffer.GetTopValue(oneByte)) {
            TELEPHONY_LOGE("get data error.");
            return;
        }
        if (!buffer.InsertByte(oneByte | HEX_VALUE_04, buffer.GetIndex())) {
            TELEPHONY_LOGE("write data error.");
            return;
        }
    }
    EncodeSubmitTypePartData(buffer, pSubmit);
}

void GsmSmsTpduEncode::EncodeSubmitTypePartData(SmsWriteBuffer &buffer, const struct SmsSubmit &pSubmit)
{
    uint8_t oneByte = 0;
    /* TP-VPF */
    switch (pSubmit.vpf) {
        case SMS_VPF_NOT_PRESENT:
            break;
        case SMS_VPF_ENHANCED:
            if (!buffer.GetTopValue(oneByte)) {
                TELEPHONY_LOGE("get data error.");
                return;
            }
            if (!buffer.InsertByte(oneByte | HEX_VALUE_08, buffer.GetIndex())) {
                TELEPHONY_LOGE("write data error.");
                return;
            }
            break;
        case SMS_VPF_RELATIVE:
            if (!buffer.GetTopValue(oneByte)) {
                TELEPHONY_LOGE("get data error.");
                return;
            }
            if (!buffer.InsertByte(oneByte | HEX_VALUE_10, buffer.GetIndex())) {
                TELEPHONY_LOGE("write data error.");
                return;
            }
            break;
        case SMS_VPF_ABSOLUTE:
            if (!buffer.GetTopValue(oneByte)) {
                TELEPHONY_LOGE("get data error.");
                return;
            }
            if (!buffer.InsertByte(oneByte | HEX_VALUE_18, buffer.GetIndex())) {
                TELEPHONY_LOGE("write data error.");
                return;
            }
            break;
        default:
            break;
    }
    EncodeSubmitTypeData(buffer, pSubmit);
}

void GsmSmsTpduEncode::EncodeSubmitTypeData(SmsWriteBuffer &buffer, const struct SmsSubmit &pSubmit)
{
    uint8_t oneByte = 0;
    /* TP-SRR */
    if (pSubmit.bStatusReport) {
        if (!buffer.GetTopValue(oneByte)) {
            TELEPHONY_LOGE("get data error.");
            return;
        }
        if (!buffer.InsertByte(oneByte | HEX_VALUE_20, buffer.GetIndex())) {
            TELEPHONY_LOGE("write data error.");
            return;
        }
    }
    /* TP-UDHI */
    if (pSubmit.bHeaderInd && CT_SMSC.compare(pSubmit.destAddress.address) != 0) {
        if (!buffer.GetTopValue(oneByte)) {
            TELEPHONY_LOGE("get data error.");
            return;
        }
        if (!buffer.InsertByte(oneByte | HEX_VALUE_40, buffer.GetIndex())) {
            TELEPHONY_LOGE("write data error.");
            return;
        }
    }
    /* TP-RP */
    if (pSubmit.bReplyPath) {
        if (!buffer.GetTopValue(oneByte)) {
            TELEPHONY_LOGE("get data error.");
            return;
        }
        if (!buffer.InsertByte(oneByte | HEX_VALUE_80, buffer.GetIndex())) {
            TELEPHONY_LOGE("write data error.");
            return;
        }
    }
    buffer.MoveForward(HEX_VALUE_01);
}

bool GsmSmsTpduEncode::EncodeDeliverPdu(SmsWriteBuffer &buffer, const struct SmsDeliver *pDeliver)
{
    if (pDeliver == nullptr) {
        TELEPHONY_LOGE("nullptr error.");
        return false;
    }
    /* TP-MTI : 00 */
    if (!buffer.InsertByte(0x00, buffer.GetIndex())) {
        TELEPHONY_LOGE("write data error.");
        return false;
    }

    /* TP-MMS */
    uint8_t oneByte = 0;
    if (pDeliver->bMoreMsg == false) {
        if (!buffer.GetTopValue(oneByte)) {
            TELEPHONY_LOGE("get data error.");
            return false;
        }
        if (!buffer.InsertByte(oneByte | HEX_VALUE_04, buffer.GetIndex())) {
            TELEPHONY_LOGE("write data error.");
            return false;
        }
    }
    /* TP-SRI */
    if (pDeliver->bStatusReport == true) {
        if (!buffer.GetTopValue(oneByte)) {
            TELEPHONY_LOGE("get data error.");
            return false;
        }
        if (!buffer.InsertByte(oneByte | HEX_VALUE_20, buffer.GetIndex())) {
            TELEPHONY_LOGE("write data error.");
            return false;
        }
    }
    /* TP-UDHI */
    if (pDeliver->bHeaderInd == true) {
        if (!buffer.GetTopValue(oneByte)) {
            TELEPHONY_LOGE("get data error.");
            return false;
        }
        if (!buffer.InsertByte(oneByte | HEX_VALUE_40, buffer.GetIndex())) {
            TELEPHONY_LOGE("write data error.");
            return false;
        }
    }
    return EncodeDeliverPartData(buffer, pDeliver);
}

bool GsmSmsTpduEncode::EncodeDeliverPartData(SmsWriteBuffer &buffer, const struct SmsDeliver *pDeliver)
{
    if (paramCodec_ == nullptr) {
        TELEPHONY_LOGE("nullptr error.");
        return false;
    }

    /* TP-RP */
    if (pDeliver->bReplyPath == true) {
        uint8_t oneByte = 0;
        if (!buffer.GetTopValue(oneByte)) {
            TELEPHONY_LOGE("get data error.");
            return false;
        }
        if (!buffer.WriteByte(oneByte | HEX_VALUE_80)) {
            TELEPHONY_LOGE("write data error.");
            return false;
        }
    }

    /* TP-OA */

    std::string address;
    if (!paramCodec_->EncodeAddressPdu(&pDeliver->originAddress, address)) {
        TELEPHONY_LOGE("encode addr fail.");
        return false;
    }

    uint8_t length = address.size();
    if (buffer.data_ == nullptr || (buffer.GetIndex() + length) > buffer.GetSize()) {
        TELEPHONY_LOGE("buffer error.");
        return false;
    }
    if (memcpy_s(buffer.data_.get() + buffer.GetIndex(), length, address.data(), length) != EOK) {
        TELEPHONY_LOGE("memcpy_s error.");
        return false;
    }
    buffer.MoveForward(length);
    /* TP-PID */
    if (!buffer.WriteByte(pDeliver->pid)) {
        TELEPHONY_LOGE("write data error.");
        return false;
    }
    return EncodeDeliverData(buffer, pDeliver);
}

bool GsmSmsTpduEncode::EncodeDeliverData(SmsWriteBuffer &buffer, const struct SmsDeliver *pDeliver)
{
    if (uDataCodec_ == nullptr) {
        TELEPHONY_LOGE("nullptr error.");
        return false;
    }
    /* TP-DCS */
    std::string dcs;
    paramCodec_->EncodeDCS(&pDeliver->dcs, dcs);
    if (buffer.data_ == nullptr || (buffer.GetIndex() + HEX_VALUE_01) > buffer.GetSize()) {
        TELEPHONY_LOGE("buffer error.");
        return false;
    }
    uint16_t bufferLen = buffer.GetSize() - buffer.GetIndex() - HEX_VALUE_01;
    if (memcpy_s(buffer.data_.get() + buffer.GetIndex(), bufferLen, dcs.data(), HEX_VALUE_01) != EOK) {
        TELEPHONY_LOGE("memcpy_s error.");
        return false;
    }
    buffer.MoveForward(1);

    /* TP-SCTS */
    std::string scts;
    paramCodec_->EncodeTimePdu(&pDeliver->timeStamp, scts);
    uint8_t length = scts.size();
    if (buffer.data_ == nullptr || (buffer.GetIndex() + length) > buffer.GetSize()) {
        TELEPHONY_LOGE("buffer error.");
        return false;
    }
    if (memcpy_s(buffer.data_.get() + buffer.GetIndex(), length, scts.data(), length) != EOK) {
        TELEPHONY_LOGE("memcpy_s error.");
        return false;
    }
    buffer.MoveForward(length);

    /* TP-UDL & TP-UD */
    const struct SmsUDPackage *pUserData = &(pDeliver->userData);
    std::string addr;
    bool result = uDataCodec_->EncodeUserDataPdu(buffer, pUserData, pDeliver->dcs.codingScheme, addr);
    return result;
}

bool GsmSmsTpduEncode::EncodeDeliverReportPdu(SmsWriteBuffer &buffer, const struct SmsDeliverReport *pDeliverRep)
{
    if (pDeliverRep == nullptr) {
        TELEPHONY_LOGE("nullptr error.");
        return false;
    }

    /* TP-MTI : 00 */
    if (!buffer.InsertByte(0x00, buffer.GetIndex())) {
        TELEPHONY_LOGE("write data error.");
        return false;
    }
    /* TP-UDHI */
    if (pDeliverRep->bHeaderInd == true) {
        uint8_t oneByte = 0;
        if (!buffer.GetTopValue(oneByte)) {
            TELEPHONY_LOGE("get data error.");
            return false;
        }
        if (!buffer.InsertByte(oneByte | HEX_VALUE_40, buffer.GetIndex())) {
            TELEPHONY_LOGE("write data error.");
            return false;
        }
    }
    buffer.MoveForward(1);
    /* TP-FCS */
    if (pDeliverRep->reportType == SMS_REPORT_NEGATIVE) {
        if (!buffer.WriteByte(pDeliverRep->failCause)) {
            TELEPHONY_LOGE("write data error.");
            return false;
        }
        TELEPHONY_LOGI("Delivery report : fail cause = [%{public}02x]", pDeliverRep->failCause);
    }
    /* TP-PI */
    if (!buffer.WriteByte(pDeliverRep->paramInd)) {
        TELEPHONY_LOGE("write data error.");
        return false;
    }
    /* TP-PID */
    if (pDeliverRep->paramInd & HEX_VALUE_01) {
        if (!buffer.WriteByte(pDeliverRep->pid)) {
            TELEPHONY_LOGE("write data error.");
            return false;
        }
    }
    return EncodeDeliverReportPartData(buffer, pDeliverRep);
}

bool GsmSmsTpduEncode::EncodeDeliverReportPartData(SmsWriteBuffer &buffer, const struct SmsDeliverReport *pDeliverRep)
{
    if (paramCodec_ == nullptr || uDataCodec_ == nullptr) {
        TELEPHONY_LOGE("nullptr error.");
        return false;
    }
    /* TP-DCS */
    if (pDeliverRep->paramInd & HEX_VALUE_02) {
        std::string dcs;
        paramCodec_->EncodeDCS(&pDeliverRep->dcs, dcs);
        if (buffer.data_ == nullptr || (buffer.GetIndex() + HEX_VALUE_01) > buffer.GetSize()) {
            TELEPHONY_LOGE("buffer error.");
            return false;
        }
        uint16_t bufferLen = buffer.GetSize() - buffer.GetIndex() - HEX_VALUE_01;
        if (memcpy_s(buffer.data_.get() + buffer.GetIndex(), bufferLen, dcs.data(), HEX_VALUE_01) != EOK) {
            TELEPHONY_LOGE("memcpy_s error.");
            return false;
        }
        buffer.MoveForward(1);
    }
    bool result = false;
    /* TP-UDL & TP-UD */
    if (pDeliverRep->paramInd & HEX_VALUE_04) {
        const struct SmsUDPackage *pUserData = &(pDeliverRep->userData);
        std::string addr;
        result = uDataCodec_->EncodeUserDataPdu(buffer, pUserData, pDeliverRep->dcs.codingScheme, addr);
    }
    return result;
}

bool GsmSmsTpduEncode::EncodeStatusReportPdu(SmsWriteBuffer &buffer, const struct SmsStatusReport *pStatusRep)
{
    if (pStatusRep == nullptr) {
        TELEPHONY_LOGE("nullptr error.");
        return false;
    }
    /* TP-MTI : 10 */
    if (!buffer.InsertByte(HEX_VALUE_02, buffer.GetIndex())) {
        TELEPHONY_LOGE("write data error.");
        return false;
    }
    /* TP-MMS */
    uint8_t oneByte = 0;
    if (pStatusRep->bMoreMsg == true) {
        if (!buffer.GetTopValue(oneByte)) {
            TELEPHONY_LOGE("get data error.");
            return false;
        }
        if (!buffer.InsertByte(oneByte | HEX_VALUE_04, buffer.GetIndex())) {
            TELEPHONY_LOGE("write data error.");
            return false;
        }
    }
    /* TP-SRQ */
    if (pStatusRep->bStatusReport == true) {
        if (!buffer.GetTopValue(oneByte)) {
            TELEPHONY_LOGE("get data error.");
            return false;
        }
        if (!buffer.InsertByte(oneByte | HEX_VALUE_20, buffer.GetIndex())) {
            TELEPHONY_LOGE("write data error.");
            return false;
        }
    }
    /* TP-UDHI */
    if (pStatusRep->bHeaderInd == true) {
        if (!buffer.GetTopValue(oneByte)) {
            TELEPHONY_LOGE("get data error.");
            return false;
        }
        if (!buffer.WriteByte(oneByte | HEX_VALUE_40)) {
            TELEPHONY_LOGE("write data error.");
            return false;
        }
    }

    /* TP-MR */
    if (!buffer.WriteByte(pStatusRep->msgRef)) {
        TELEPHONY_LOGE("write data error.");
        return false;
    }
    return EncodeStatusReportPartData(buffer, pStatusRep);
}

bool GsmSmsTpduEncode::EncodeStatusReportPartData(SmsWriteBuffer &buffer, const struct SmsStatusReport *pStatusRep)
{
    if (paramCodec_ == nullptr) {
        TELEPHONY_LOGE("nullptr error.");
        return false;
    }

    /* TP-RA */
    std::string address;
    if (!paramCodec_->EncodeAddressPdu(&pStatusRep->recipAddress, address)) {
        TELEPHONY_LOGE("encode addr fail.");
        return false;
    }

    uint8_t length = address.size();
    if (buffer.data_ == nullptr || (buffer.GetIndex() + length) > buffer.GetSize()) {
        TELEPHONY_LOGE("buffer error.");
        return false;
    }
    if (memcpy_s(buffer.data_.get() + buffer.GetIndex(), length, address.data(), length) != EOK) {
        TELEPHONY_LOGE("memcpy_s error.");
        return false;
    }
    buffer.MoveForward(length);

    /* TP-SCTS */
    std::string scts;

    paramCodec_->EncodeTimePdu(&pStatusRep->timeStamp, scts);
    length = scts.size();
    if (buffer.data_ == nullptr || (buffer.GetIndex() + length) > buffer.GetSize()) {
        TELEPHONY_LOGE("buffer error.");
        return false;
    }
    if (memcpy_s(buffer.data_.get() + buffer.GetIndex(), length, scts.data(), length) != EOK) {
        TELEPHONY_LOGE("memcpy_s error.");
        return false;
    }
    buffer.MoveForward(length);

    /* TP-DT */
    std::string dt;
    paramCodec_->EncodeTimePdu(&pStatusRep->dischargeTime, dt);
    length = scts.size();
    if (buffer.data_ == nullptr || (buffer.GetIndex() + length) > buffer.GetSize()) {
        TELEPHONY_LOGE("buffer error.");
        return false;
    }
    if (memcpy_s(buffer.data_.get() + buffer.GetIndex(), length, dt.data(), length) != EOK) {
        TELEPHONY_LOGE("memcpy_s error.");
        return false;
    }
    buffer.MoveForward(length);
    return EncodeStatusReportData(buffer, pStatusRep, length);
}

bool GsmSmsTpduEncode::EncodeStatusReportData(
    SmsWriteBuffer &buffer, const struct SmsStatusReport *pStatusRep, uint8_t length)
{
    if (uDataCodec_ == nullptr) {
        TELEPHONY_LOGE("nullptr error.");
        return false;
    }

    /* TP-Status */
    if (!buffer.WriteByte(pStatusRep->status)) {
        TELEPHONY_LOGE("write data error.");
        return false;
    }
    /* TP-PI */
    if (!buffer.WriteByte(pStatusRep->paramInd)) {
        TELEPHONY_LOGE("write data error.");
        return false;
    }
    /* TP-PID */
    if (pStatusRep->paramInd & HEX_VALUE_01) {
        if (!buffer.WriteByte(pStatusRep->pid)) {
            TELEPHONY_LOGE("write data error.");
            return false;
        }
    }
    /* TP-DCS */
    if (pStatusRep->paramInd & HEX_VALUE_02) {
        std::string dcs;
        paramCodec_->EncodeDCS(&pStatusRep->dcs, dcs);
        if (buffer.data_ == nullptr || (buffer.GetIndex() + length) > buffer.GetSize()) {
            TELEPHONY_LOGE("buffer error.");
            return false;
        }
        if (memcpy_s(buffer.data_.get() + buffer.GetIndex(), length, dcs.data(), length) != EOK) {
            TELEPHONY_LOGE("memcpy_s error.");
            return false;
        }
        buffer.MoveForward(length);
    }
    /* TP-UDL & TP-UD */
    bool result = false;
    if (pStatusRep->paramInd & HEX_VALUE_04) {
        const struct SmsUDPackage *pUserData = &(pStatusRep->userData);
        std::string addr;
        result = uDataCodec_->EncodeUserDataPdu(buffer, pUserData, pStatusRep->dcs.codingScheme, addr);
    }
    return result;
}
} // namespace Telephony
} // namespace OHOS