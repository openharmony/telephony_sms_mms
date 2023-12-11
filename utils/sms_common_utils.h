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

#ifndef SMS_COMMON_UTILS_H
#define SMS_COMMON_UTILS_H

#include <ctime>

#include "sms_pdu_code_type.h"

namespace OHOS {
namespace Telephony {
class SmsCommonUtils {
public:
    static uint16_t Pack7bitChar(
        const uint8_t *userData, uint16_t dataLen, uint8_t fillBits, uint8_t *packData, uint16_t packLen);
    static uint16_t Unpack7bitChar(
        const uint8_t *tpdu, uint16_t dataLen, uint8_t fillBits, uint8_t *unpackData, uint16_t unpackDataLen);
    static uint16_t Unpack7bitCharForCBPdu(
        const uint8_t *tpdu, uint16_t dataLen, uint8_t fillBits, uint8_t *unpackData, uint16_t unpackDataLen);
    static int64_t ConvertTime(const struct SmsTimeAbs &timeAbs);
    static uint8_t DtmfCharToDigit(const uint8_t c);
    static uint8_t DigitToDtmfChar(const uint8_t c);

private:
    static void GetDisplayTime(const time_t &rawtime);
};
} // namespace Telephony
} // namespace OHOS
#endif // SMS_COMMON_UTILS_H
