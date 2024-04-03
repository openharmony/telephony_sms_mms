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

#include "gsm_pdu_hex_value.h"
#include "gsm_sms_param_codec.h"
#include "gsm_sms_tpdu_decode.h"
#include "gsm_sms_tpdu_encode.h"
#include "securec.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
using namespace std;
static constexpr uint8_t SLIDE_DATA_STEP = 2;
static constexpr uint16_t MAX_DECODE_LEN = 520;
static constexpr uint16_t MAX_ENCODE_LEN = 255;

GsmSmsTpduCodec::GsmSmsTpduCodec()
{
    uDataCodec_ = std::make_shared<GsmUserDataPdu>();
    paramCodec_ = std::make_shared<GsmSmsParamCodec>();
}

GsmSmsTpduCodec::~GsmSmsTpduCodec() {}

bool GsmSmsTpduCodec::EncodeSmsPdu(std::shared_ptr<SmsTpdu> sourceData, char *pdu, uint16_t pduLen, uint16_t &bufLen)
{
    if (pduLen == 0 || pduLen > MAX_ENCODE_LEN) {
        TELEPHONY_LOGE("pduLen Error.");
        return false;
    }
    if (sourceData == nullptr) {
        TELEPHONY_LOGE("nullptr error.");
        return false;
    }

    auto tpduEncode = std::make_shared<GsmSmsTpduEncode>(uDataCodec_, paramCodec_, shared_from_this());
    if (tpduEncode == nullptr) {
        TELEPHONY_LOGE("nullptr Error.");
        return false;
    }

    bool encodeRet = 0;
    SmsWriteBuffer pduBuffer;
    switch (sourceData->tpduType) {
        case SMS_TPDU_SUBMIT:
            encodeRet = tpduEncode->EncodeSubmitPdu(pduBuffer, &(sourceData->data.submit));
            break;
        case SMS_TPDU_DELIVER:
            encodeRet = tpduEncode->EncodeDeliverPdu(pduBuffer, &(sourceData->data.deliver));
            break;
        case SMS_TPDU_DELIVER_REP:
            encodeRet = tpduEncode->EncodeDeliverReportPdu(pduBuffer, &(sourceData->data.deliverRep));
            break;
        case SMS_TPDU_STATUS_REP:
            encodeRet = tpduEncode->EncodeStatusReportPdu(pduBuffer, &(sourceData->data.statusRep));
            break;
        default:
            break;
    }
    bufLen = pduBuffer.GetIndex();
    if (!encodeRet || bufLen > pduLen) {
        TELEPHONY_LOGE("encode sms fail");
        return false;
    }
    auto bufferRet = pduBuffer.GetPduBuffer();
    if (bufferRet == nullptr) {
        TELEPHONY_LOGE("bufferRet nullptr");
        return false;
    }
    std::vector<uint8_t> buf = *(bufferRet);
    for (uint16_t locate = 0; locate < bufLen; locate++) {
        pdu[locate] = static_cast<char>(buf[locate]);
    }
    TELEPHONY_LOGI("encode sms success");
    return true;
}

bool GsmSmsTpduCodec::DecodeSmsPdu(const uint8_t *pTpdu, uint16_t TpduLen, struct SmsTpdu *pSmsTpdu)
{
    if (pTpdu == nullptr || pSmsTpdu == nullptr || TpduLen == 0 || TpduLen > MAX_DECODE_LEN) {
        TELEPHONY_LOGE("nullptr error. or TpduLen Error.");
        return false;
    }

    string pduData(TpduLen, '\0');
    for (int locate = 0; locate < TpduLen; locate++) {
        pduData[locate] = static_cast<char>(pTpdu[locate]);
    }
    TELEPHONY_LOGI("TpduLen:%{public}d", TpduLen);

    SmsReadBuffer buffer(pduData);
    std::shared_ptr<GsmSmsTpduDecode> tpduDecode =
        std::make_shared<GsmSmsTpduDecode>(uDataCodec_, paramCodec_, shared_from_this());
    if (tpduDecode == nullptr) {
        TELEPHONY_LOGE("nullptr error.");
        return false;
    }

    const char mti = pTpdu[0] & HEX_VALUE_03;
    bool decodeResult = false;
    switch (mti) {
        case TYPE_INDICATOR_DELIVER:
            pSmsTpdu->tpduType = SMS_TPDU_DELIVER;
            decodeResult = tpduDecode->DecodeDeliver(buffer, &(pSmsTpdu->data.deliver));
            break;
        case TYPE_INDICATOR_SUBMIT:
            pSmsTpdu->tpduType = SMS_TPDU_SUBMIT;
            decodeResult = tpduDecode->DecodeSubmit(buffer, &(pSmsTpdu->data.submit));
            break;
        case TYPE_INDICATOR_STATUS_REP:
            pSmsTpdu->tpduType = SMS_TPDU_STATUS_REP;
            decodeResult = tpduDecode->DecodeStatusReport(buffer, &(pSmsTpdu->data.statusRep));
            break;
        default:
            break;
    }
    TELEPHONY_LOGD("buffer index:%{public}d", buffer.GetIndex());
    if (decodeResult && buffer.GetIndex() <= (TpduLen + 1)) {
        TELEPHONY_LOGD("decode sms success");
        return true;
    }
    TELEPHONY_LOGE("decode sms fail");
    return false;
}

enum SmsPid GsmSmsTpduCodec::ParsePid(const uint8_t pid)
{
    return (enum SmsPid)pid;
}

void GsmSmsTpduCodec::DebugTpdu(SmsReadBuffer &buffer, const enum SmsParseType type)
{
    char tpduTmp[(MAX_ENCODE_LEN * SLIDE_DATA_STEP) + 1];
    if (memset_s(tpduTmp, sizeof(tpduTmp), 0x00, sizeof(tpduTmp)) != EOK) {
        TELEPHONY_LOGE("memset_s error.");
        return;
    }
    uint8_t oneByte = 0;
    uint8_t step = SLIDE_DATA_STEP;
    uint16_t tpduLen = buffer.GetSize();
    for (uint16_t i = 0; i < tpduLen; i++) {
        if (sizeof(tpduTmp) <= (i * step)) {
            TELEPHONY_LOGE("data error.");
            return;
        }
        const uint16_t len = sizeof(tpduTmp) - (i * step);
        if (!buffer.PickOneByteFromIndex(i, oneByte)) {
            TELEPHONY_LOGE("buffer error.");
            return;
        }
        if (snprintf_s(tpduTmp + (i * step), len - 1, len - 1, "%02X", oneByte) < 0) {
            TELEPHONY_LOGE("DebugTpdu snprintf_s error");
            return;
        }
    }
}
} // namespace Telephony
} // namespace OHOS