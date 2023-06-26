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

#ifndef GSM_SMS_PARAM_DECODE_H
#define GSM_SMS_PARAM_DECODE_H

#include "gsm_pdu_code_type.h"
#include "sms_pdu_buffer.h"

namespace OHOS {
namespace Telephony {
class GsmSmsParamDecode {
public:
    GsmSmsParamDecode() = default;
    virtual ~GsmSmsParamDecode() = default;
    bool DecodeAddressPdu(SmsReadBuffer &buffer, struct AddressNumber *resultNum);
    void DecodeSmscPdu(uint8_t *srcAddr, uint8_t addrLen, enum TypeOfNum ton, std::string &desAddr);
    uint8_t DecodeSmscPdu(const uint8_t *pTpdu, uint8_t pduLen, struct AddressNumber &desAddrObj);
    bool DecodeDcsPdu(SmsReadBuffer &buffer, struct SmsDcs *pDCS);

    enum SmsMessageClass GetMsgClass(uint8_t dcs);
    enum DataCodingScheme GetMsgCodingScheme(uint8_t dcs);
    enum SmsIndicatorType GetMsgIndicatorType(const uint8_t dcs);
    void GetMwiType(const uint8_t dcs, struct SmsDcs &smsDcs);
    bool DecodeTimePdu(SmsReadBuffer &buffer, struct SmsTimeStamp *timeStamp);

private:
    bool DecodeAddressAlphaNum(SmsReadBuffer &buffer, struct AddressNumber *resultNum, uint8_t bcdLen, uint8_t addrLen);
    bool DecodeAddressInternationalNum(SmsReadBuffer &buffer, struct AddressNumber *resultNum, uint8_t bcdLen);
    bool DecodeAddressDefaultNum(SmsReadBuffer &buffer, struct AddressNumber *resultNum, uint8_t bcdLen);
    void DecodeDcsGeneralGroupPdu(uint8_t dcs, struct SmsDcs *smsDcs);
    void DecodeDcsClassGroupPdu(uint8_t dcs, struct SmsDcs *smsDcs);
    void DecodeDcsDeleteGroupPdu(uint8_t dcs, struct SmsDcs *smsDcs);
    void DecodeDcsDiscardGroupPdu(uint8_t dcs, struct SmsDcs *smsDcs);
    void DecodeDcsStoreGsmGroupPdu(uint8_t dcs, struct SmsDcs *smsDcs);
    void DecodeDcsStoreUCS2GroupPdu(uint8_t dcs, struct SmsDcs *smsDcs);
    void DecodeDcsUnknownGroupPdu(uint8_t dcs, struct SmsDcs *smsDcs);
    bool DecodeTimePduPartData(SmsReadBuffer &buffer, struct SmsTimeStamp *pTimeStamp);
    bool DecodeTimePduData(SmsReadBuffer &buffer, struct SmsTimeStamp *pTimeStamp);
};
} // namespace Telephony
} // namespace OHOS
#endif