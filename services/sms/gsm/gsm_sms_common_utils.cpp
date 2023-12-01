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

#include "gsm_sms_common_utils.h"

#include <ctime>

#include "gsm_pdu_hex_value.h"
#include "securec.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
static constexpr uint8_t DIGITAL_STEP = 2;
static constexpr uint8_t SMS_ENCODE_GSM_BIT = 7;
static constexpr uint8_t MAX_GSM_7BIT_DATA_LEN = 160;
static constexpr uint8_t SMS_BYTE_BIT = 8;
static constexpr uint8_t MIN_REMAIN_LEN = 2;

bool GsmSmsCommonUtils::Pack7bitChar(SmsWriteBuffer &buffer, const uint8_t *userData, uint8_t dataLen, uint8_t fillBits)
{
    if (userData == nullptr || dataLen > MAX_GSM_7BIT_DATA_LEN) {
        TELEPHONY_LOGE("userData error.");
        return false;
    }
    auto shift = fillBits;
    if (shift > 0) {
        buffer.MoveForward(1);
    }
    uint8_t srcIdx = 0;
    while (srcIdx < dataLen) {
        if (shift == 0) {
            if (!buffer.WriteByte(userData[srcIdx])) {
                TELEPHONY_LOGE("write data error.");
                return false;
            }
            shift = SMS_ENCODE_GSM_BIT;
            srcIdx++;
            if (srcIdx >= dataLen) {
                break;
            }
        }
        if (shift > 1) {
            if (!Pack7bitCharPartData(buffer, userData, srcIdx, shift)) {
                TELEPHONY_LOGE("packet fail.");
                return false;
            }
            shift--;
            srcIdx++;
        } else if (shift == 1) {
            uint8_t oneByte = 0;
            if (!buffer.GetValueFromIndex(buffer.GetIndex() - 1, oneByte)) {
                TELEPHONY_LOGE("get data error.");
                return false;
            }
            oneByte |= (userData[srcIdx] << shift);
            if (!buffer.InsertByte(oneByte, (buffer.GetIndex() - 1))) {
                TELEPHONY_LOGE("write data error.");
                return false;
            }
            srcIdx++;
            shift--;
        }
    }
    return true;
}

bool GsmSmsCommonUtils::Pack7bitCharPartData(
    SmsWriteBuffer &buffer, const uint8_t *userData, uint8_t &srcIdx, uint8_t &shift)
{
    uint8_t oneByte = 0;
    if (!buffer.GetValueFromIndex(buffer.GetIndex() - 1, oneByte)) {
        TELEPHONY_LOGE("get data error.");
        return false;
    }
    oneByte |= (userData[srcIdx] << shift);
    if (!buffer.InsertByte(oneByte, (buffer.GetIndex() - 1))) {
        TELEPHONY_LOGE("write data error.");
        return false;
    }
    if (!buffer.GetTopValue(oneByte)) {
        TELEPHONY_LOGE("get data error.");
        return false;
    }
    uint8_t nextByte = userData[srcIdx] >> (SMS_BYTE_BIT - shift);
    if (!buffer.WriteByte(oneByte | nextByte)) {
        TELEPHONY_LOGE("write data error.");
        return false;
    }
    return true;
}

bool GsmSmsCommonUtils::Unpack7bitChar(SmsReadBuffer &buffer, uint8_t dataLen, uint8_t fillBits, uint8_t *unpackData,
    uint8_t unpackDataLen, uint8_t &dstIdx)
{
    auto shift = fillBits;
    if (unpackData == nullptr || dataLen >= unpackDataLen || fillBits > SMS_BYTE_BIT - 1) {
        TELEPHONY_LOGE("data error.");
        return false;
    }
    if (shift > 0) {
        buffer.MoveForward(1);
    }
    for (; dstIdx < unpackDataLen; dstIdx++) {
        if (shift == 0) {
            uint8_t oneByte = 0;
            if (!buffer.ReadByte(oneByte)) {
                TELEPHONY_LOGI("data unpack finish.");
                return true;
            }
            unpackData[dstIdx] = oneByte & HEX_VALUE_7F;
            shift = SMS_ENCODE_GSM_BIT;
            dstIdx++;
            if (dstIdx >= unpackDataLen) {
                break;
            }
        }

        uint8_t oneByte = 0;
        if (!buffer.PickOneByteFromIndex(buffer.GetIndex() - 1, oneByte)) {
            TELEPHONY_LOGI("data unpack finish.");
            return true;
        }
        uint8_t nextByte = 0;
        if (!buffer.PickOneByte(nextByte)) {
            TELEPHONY_LOGI("data unpack finish.");
            unpackData[dstIdx] = (oneByte >> shift);
            if (unpackData[dstIdx] != 0) {
                dstIdx++;
            }
            return true;
        }
        unpackData[dstIdx] = (oneByte >> shift) + (nextByte << (SMS_BYTE_BIT - shift));
        unpackData[dstIdx] &= HEX_VALUE_7F;
        shift--;
        if (shift > 0) {
            buffer.MoveForward(1);
        }
    }
    return true;
}

bool GsmSmsCommonUtils::DigitToBcd(const char *digit, uint8_t digitLen, uint8_t *bcd, uint8_t bcdLen, uint8_t &len)
{
    if (digit == nullptr || bcd == nullptr || len >= bcdLen) {
        TELEPHONY_LOGE("data error.");
        return false;
    }

    len = 0;
    for (uint8_t i = 0; i < digitLen; i++) {
        uint8_t temp = static_cast<uint8_t>(digit[i] - '0');
        if (len >= bcdLen) {
            TELEPHONY_LOGE("len invalid.");
            return false;
        }
        if ((i % DIGITAL_STEP) == 0) {
            bcd[len] = temp & HEX_VALUE_0F;
        } else {
            bcd[len++] |= ((temp & HEX_VALUE_0F) << HEX_VALUE_04);
        }
    }

    if (len + 1 >= bcdLen) {
        TELEPHONY_LOGE("len invalid.");
        return false;
    }
    if ((digitLen % DIGITAL_STEP) == 1) {
        bcd[len++] |= HEX_VALUE_F0;
    }
    return true;
}

bool GsmSmsCommonUtils::BcdToDigit(const uint8_t *bcd, uint8_t bcdLen, std::string &digit, uint8_t maxDigitLen)
{
    if (bcd == nullptr || bcdLen == 0 || maxDigitLen == 0) {
        TELEPHONY_LOGE("data error.");
        return false;
    }
    for (uint8_t i = 0; i < bcdLen; i++) {
        uint8_t temp = bcd[i] & HEX_VALUE_0F;
        if (digit.size() + MIN_REMAIN_LEN >= maxDigitLen) {
            TELEPHONY_LOGE("digit size over max");
            return false;
        }
        digit.push_back(BcdToChar(temp));
        temp = (bcd[i] & HEX_VALUE_F0) >> HEX_VALUE_04;
        if (temp == HEX_VALUE_0F) {
            return true;
        }
        digit.push_back(BcdToChar(temp));
    }
    return true;
}

char GsmSmsCommonUtils::BcdToChar(const uint8_t c)
{
    char temp = static_cast<char>(c + '0');
    return temp;
}
} // namespace Telephony
} // namespace OHOS