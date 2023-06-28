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

#include "gsm_sms_param_codec.h"

#include "gsm_pdu_hex_value.h"
#include "gsm_sms_param_decode.h"
#include "gsm_sms_param_encode.h"
#include "telephony_log_wrapper.h"
#include "text_coder.h"

namespace OHOS {
namespace Telephony {
bool GsmSmsParamCodec::EncodeAddressPdu(const struct AddressNumber *num, std::string &resultNum)
{
    GsmSmsParamEncode codec;
    return codec.EncodeAddressPdu(num, resultNum);
}

uint8_t GsmSmsParamCodec::EncodeSmscPdu(const char *num, uint8_t *resultNum)
{
    GsmSmsParamEncode codec;
    return codec.EncodeSmscPdu(num, resultNum);
}

uint8_t GsmSmsParamCodec::EncodeSmscPdu(const struct AddressNumber *num, uint8_t *smscNum, uint8_t smscLen)
{
    GsmSmsParamEncode codec;
    return codec.EncodeSmscPdu(num, smscNum, smscLen);
}

void GsmSmsParamCodec::EncodeTimePdu(const struct SmsTimeStamp *timeStamp, std::string &resultValue)
{
    GsmSmsParamEncode codec;
    codec.EncodeTimePdu(timeStamp, resultValue);
}

void GsmSmsParamCodec::EncodeDCS(const struct SmsDcs *dcsData, std::string &returnValue)
{
    GsmSmsParamEncode codec;
    codec.EncodeDCS(dcsData, returnValue);
}

bool GsmSmsParamCodec::DecodeAddressPdu(SmsReadBuffer &buffer, struct AddressNumber *resultNum)
{
    GsmSmsParamDecode codec;
    return codec.DecodeAddressPdu(buffer, resultNum);
}

bool GsmSmsParamCodec::DecodeTimePdu(SmsReadBuffer &buffer, struct SmsTimeStamp *timeStamp)
{
    GsmSmsParamDecode codec;
    return codec.DecodeTimePdu(buffer, timeStamp);
}

bool GsmSmsParamCodec::DecodeDcsPdu(SmsReadBuffer &buffer, struct SmsDcs *smsDcs)
{
    GsmSmsParamDecode codec;
    return codec.DecodeDcsPdu(buffer, smsDcs);
}

void GsmSmsParamCodec::DecodeSmscPdu(uint8_t *pAddress, uint8_t addrLen, enum TypeOfNum ton, std::string &decodeAddr)
{
    GsmSmsParamDecode codec;
    codec.DecodeSmscPdu(pAddress, addrLen, ton, decodeAddr);
}

uint8_t GsmSmsParamCodec::DecodeSmscPdu(const uint8_t *pTpdu, uint8_t pduLen, struct AddressNumber &address)
{
    GsmSmsParamDecode codec;
    return codec.DecodeSmscPdu(pTpdu, pduLen, address);
}

bool GsmSmsParamCodec::CheckVoicemail(SmsReadBuffer &buffer, int32_t *setType, int32_t *indType)
{
    uint8_t oneByte = 0;
    if (!buffer.ReadByte(oneByte)) {
        TELEPHONY_LOGE("get data error.");
        return false;
    }

    if (oneByte != HEX_VALUE_04) {
        TELEPHONY_LOGE("data error.");
        return false;
    }

    if (!buffer.ReadByte(oneByte)) {
        TELEPHONY_LOGE("get data error.");
        return false;
    }
    if (oneByte != HEX_VALUE_D0) {
        TELEPHONY_LOGE("data error.");
        return false;
    }

    if (!buffer.PickOneByte(oneByte)) {
        TELEPHONY_LOGE("get data error.");
        return false;
    }
    uint8_t nextByte = 0;
    if (!buffer.PickOneByte(nextByte)) {
        TELEPHONY_LOGE("get data error.");
        return false;
    }

    if (oneByte == HEX_VALUE_11 || nextByte == HEX_VALUE_10) {
        TELEPHONY_LOGI("####### VMI msg ######");
        if (!buffer.ReadByte(oneByte)) {
            TELEPHONY_LOGE("get data error.");
            return false;
        }
        *setType = static_cast<int32_t>(oneByte & HEX_VALUE_01); /* 0 : clear, 1 : set */
        if (!buffer.PickOneByte(oneByte)) {
            TELEPHONY_LOGE("get data error.");
            return false;
        }
        *indType = static_cast<int32_t>(oneByte & HEX_VALUE_01); /* 0 : indicator 1, 1 : indicator 2 */
        return true;
    }
    return false;
}
} // namespace Telephony
} // namespace OHOS
