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

#ifndef GSM_SMS_COMMON_UTILS_H
#define GSM_SMS_COMMON_UTILS_H

#include <ctime>

#include "sms_pdu_buffer.h"
#include "sms_pdu_code_type.h"

namespace OHOS {
namespace Telephony {
class GsmSmsCommonUtils {
public:
    GsmSmsCommonUtils() = default;
    ~GsmSmsCommonUtils() = default;
    bool Pack7bitChar(SmsWriteBuffer &buffer, const uint8_t *userData, uint8_t dataLen, uint8_t fillBits);
    bool Unpack7bitChar(SmsReadBuffer &buffer, uint8_t dataLen, uint8_t fillBits, uint8_t *unpackData,
        uint8_t unpackDataLen, uint8_t &dstIdx);
    bool DigitToBcd(const char *digit, uint8_t digitLen, uint8_t *bcd, uint8_t bcdLen, uint8_t &len);
    bool BcdToDigit(const uint8_t *bcd, uint8_t bcdLen, std::string &digit, uint8_t maxDigitLen);

private:
    char BcdToChar(const uint8_t c);
    bool Pack7bitCharPartData(SmsWriteBuffer &buffer, const uint8_t *userData, uint8_t &srcIdx, uint8_t &shift);
};
} // namespace Telephony
} // namespace OHOS
#endif