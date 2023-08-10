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

#ifndef GSM_SMS_PARAM_ENCODE_H
#define GSM_SMS_PARAM_ENCODE_H

#include <string>

#include "gsm_pdu_code_type.h"

namespace OHOS {
namespace Telephony {
class GsmSmsParamEncode {
public:
    GsmSmsParamEncode() = default;
    virtual ~GsmSmsParamEncode() = default;

    bool EncodeAddressPdu(const struct AddressNumber *num, std::string &resultNum);
    uint8_t EncodeSmscPdu(const char *pAddress, uint8_t *pEncodeAddr);
    uint8_t EncodeSmscPdu(const struct AddressNumber *pAddress, uint8_t *pSMSC, uint8_t smscLen);
    void EncodeDCS(const struct SmsDcs *pDCS, std::string &returnValue);
    void EncodeTimePdu(const struct SmsTimeStamp *pTimeStamp, std::string &resultValue);
};
} // namespace Telephony
} // namespace OHOS
#endif