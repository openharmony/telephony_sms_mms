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

#ifndef GSM_SMS_PARAM_CODEC_H
#define GSM_SMS_PARAM_CODEC_H

#include "gsm_pdu_code_type.h"
#include "sms_pdu_buffer.h"

namespace OHOS {
namespace Telephony {
class GsmSmsParamCodec {
public:
    GsmSmsParamCodec() = default;
    virtual ~GsmSmsParamCodec() = default;
    bool EncodeAddressPdu(const struct AddressNumber *num, std::string &resultNum);
    void EncodeTimePdu(const struct SmsTimeStamp *timeStamp, std::string &resultValue);
    void EncodeDCS(const struct SmsDcs *dcsData, std::string &returnValue);
    uint8_t EncodeSmscPdu(const char *num, uint8_t *resultNum);
    uint8_t EncodeSmscPdu(const struct AddressNumber *num, uint8_t *smscNum, uint8_t smscLen);

    bool DecodeAddressPdu(SmsReadBuffer &buffer, struct AddressNumber *resultNum);
    bool DecodeTimePdu(SmsReadBuffer &bufferr, struct SmsTimeStamp *timeStamp);
    bool DecodeDcsPdu(SmsReadBuffer &buffer, struct SmsDcs *smsDcs);

    void DecodeSmscPdu(uint8_t *pAddress, uint8_t addrLen, enum TypeOfNum ton, std::string &decodeAddr);
    uint8_t DecodeSmscPdu(const uint8_t *pTpdu, uint8_t pduLen, struct AddressNumber &address);
    bool CheckVoicemail(SmsReadBuffer &buffer, int32_t *setType, int32_t *indType);
};
} // namespace Telephony
} // namespace OHOS
#endif