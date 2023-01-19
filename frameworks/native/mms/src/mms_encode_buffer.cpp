/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#include "mms_encode_buffer.h"

#include "mms_decode_buffer.h"
#include "securec.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
using namespace std;
/**
 * @brief DecodeInteger
 * wap-230-wsp-20010705-a   section:8.4.2.1 Basic rules
 * Uintvar-integer = 1*5 OCTET
 * @param value
 * @return true
 * @return false
 */
bool MmsEncodeBuffer::EncodeUintvar(uint32_t value)
{
    const uint8_t setNotHighestBitZero = 0x80;
    const uint8_t setHighestBitZero = 0x7f;
    const uint8_t shiftUintBits = 7;
    char reversed[8] = {0};
    int len = 1;
    reversed[0] = (char)value & setHighestBitZero;
    value = value >> shiftUintBits;
    while (value > 0) {
        reversed[len] = setNotHighestBitZero | (value & setHighestBitZero);
        len++;
        value = value >> shiftUintBits;
    }

    int j = 0;
    for (j = 0; j < len; j++) {
        if (!WriteByte(reversed[len - j - 1])) {
            TELEPHONY_LOGE("Encode buffer WriteByte fail.");
            return false;
        }
    }
    return true;
}

bool MmsEncodeBuffer::WriteByte(uint8_t value)
{
    if (curPosition_ >= CODE_BUFFER_MAX_SIZE) {
        TELEPHONY_LOGE("Encode buffer current position invalid.");
        return false;
    }
    pduBuffer_[curPosition_++] = value;
    return true;
}

bool MmsEncodeBuffer::WriteBuffer(MmsEncodeBuffer &buff)
{
    uint32_t len = buff.GetCurPosition();
    if (curPosition_ + len >= CODE_BUFFER_MAX_SIZE) {
        TELEPHONY_LOGE("Encode buffer current position invalid.");
        return false;
    }
    if (memcpy_s(pduBuffer_.get() + curPosition_, len, buff.pduBuffer_.get(), len) != EOK) {
        TELEPHONY_LOGE("Encode buffer memcpy_s fail.");
        return false;
    }
    curPosition_ += buff.GetCurPosition();
    return true;
}

bool MmsEncodeBuffer::WriteBuffer(std::unique_ptr<char[]> input, uint32_t length)
{
    if (input == nullptr) {
        TELEPHONY_LOGE("Encode buffer input is null.");
        return false;
    }

    if (curPosition_ + length >= CODE_BUFFER_MAX_SIZE) {
        TELEPHONY_LOGE("Encode buffer current position invalid.");
        return false;
    }
    if (memcpy_s(pduBuffer_.get() + curPosition_, length, input.get(), length) != EOK) {
        TELEPHONY_LOGE("Encode buffer memcpy_s fail.");
        return false;
    }
    curPosition_ += length;
    return true;
}

/**
 * @brief EncodeShortLength
 * wap-230-wsp-20010705-a.pdf   section:8.4.2.2 Length
 * Short-length = <Any octet 0-30>
 * @param value
 * @return true
 * @return false
 */
bool MmsEncodeBuffer::EncodeShortLength(uint8_t value)
{
    const uint8_t minShortLength = 30;
    if (value > minShortLength) {
        TELEPHONY_LOGE("Encode buffer value invalid.");
        return false;
    }
    if (!WriteByte(value)) {
        TELEPHONY_LOGE("Encode buffer WriteByte fail.");
        return false;
    }
    return true;
}

/**
 * @brief EncodeShortInteger
 * wap-230-wsp-20010705-a   section:8.4.2.1 Basic rules
 * Short-integer = OCTET
 * Integers in range 0-127 shall be encoded as a one octet value with
 * the most significant bit set to one (1xxx xxxx) and with the value
 * in the remaining least significant bits
 * @param value
 * @return true
 * @return false
 */
bool MmsEncodeBuffer::EncodeShortInteger(uint8_t value)
{
    const uint8_t setHighestBitOne = 0x80;
    if (!WriteByte(value | setHighestBitOne)) {
        TELEPHONY_LOGE("Encode buffer WriteByte fail.");
        return false;
    }
    return true;
}

/**
 * @brief EncodeOctet
 * wap-230-wsp-20010705-a   section:8.1.2 Variable Length Unsigned Integers
 * octet 8 bits of opaque data
 * @param value
 * @return true
 * @return false
 */
bool MmsEncodeBuffer::EncodeOctet(uint8_t value)
{
    if (!WriteByte(value)) {
        TELEPHONY_LOGE("Encode buffer WriteByte fail.");
        return false;
    }
    return true;
}

/**
 * @brief EncodeValueLength
 * wap-230-wsp-20010705-a   section:8.4.2.2 Length
 * value-length = short-length | (Length-quote Length)
 * short-length = <Any octet 0-30>
 * Length-quote = <Octet 31>
 * (Length-quote Length) = Length-quote + Uintvar-length
 * @param value
 * @return true
 * @return false
 */
bool MmsEncodeBuffer::EncodeValueLength(uint32_t value)
{
    const uint8_t lengthQuote = 0x1f;
    const uint32_t maxShortLength = 30;

    if (value <= maxShortLength) {
        return EncodeShortLength(value);
    }
    if (!WriteByte(lengthQuote)) {
        TELEPHONY_LOGE("Encode buffer WriteByte fail.");
        return false;
    }
    if (!EncodeUintvar(value)) {
        TELEPHONY_LOGE("Encode buffer EncodeUintvar fail.");
        return false;
    }
    return true;
}

bool MmsEncodeBuffer::EncodeInteger(uint32_t value)
{
    const uint32_t maxShortInteger = 127;
    if (value <= maxShortInteger) {
        return EncodeShortInteger(value);
    } else {
        return EncodeLongInteger(value);
    }
    return true;
}

/**
 * @brief EncodeLongInteger
 * wap-230-wsp-20010705-a   section:8.4.2.1 Basic rules
 * Long-integer = [Short-length] [Multi-octet-integer]
 * Short-length = 1 byte
 * Multi-octet-integer = <Short-length> bytes
 * @param value
 * @return true
 * @return false
 */
bool MmsEncodeBuffer::EncodeLongInteger(uint64_t value)
{
    const uint64_t getLast8Bit = 0x000000FF;
    const uint8_t rightMoveBits = 8;

    uint64_t temp = value;
    uint8_t count = 0;
    uint8_t result[8] = {0};
    while (temp > 0) {
        result[count] = temp & getLast8Bit;
        temp = temp >> rightMoveBits;
        count++;
    }

    if (value == 0) {
        count = 1;
    }
    if (!WriteByte(count)) {
        TELEPHONY_LOGE("Encode buffer WriteByte fail.");
        return false;
    }

    for (int16_t i = static_cast<int16_t>(count) - 1; i >= 0; i--) {
        if (!WriteByte(result[i])) {
            TELEPHONY_LOGE("Encode buffer WriteByte fail.");
            return false;
        }
    }
    return true;
}

/**
 * @brief EncodeText
 * wap-230-wsp-20010705-a   section:8.4.2.1 Basic rules
 * Text-string = [Quote] *TEXT End-of-string
 * Quote = <Octet 127>
 * End-of-string = <Octet 0>
 * @param value
 * @return true
 * @return false
 */
bool MmsEncodeBuffer::EncodeText(std::string value)
{
    const uint8_t quoteText = 0x7f;
    if (value.empty()) {
        TELEPHONY_LOGI("Encode buffer value is empty string.");
        return true;
    }

    if (value.at(0) > quoteText) {
        if (!WriteByte(quoteText)) {
            TELEPHONY_LOGE("Encode buffer WriteByte fail.");
            return false;
        }
    }
    for (std::size_t i = 0; i < value.length(); i++) {
        if (!WriteByte(value.at(i))) {
            TELEPHONY_LOGE("Encode buffer WriteByte fail.");
            return false;
        }
    }
    if (!WriteByte(0)) {
        TELEPHONY_LOGE("Encode buffer WriteByte fail.");
        return false;
    }
    return true;
}

/**
 * @brief EncodeQuotedText
 * wap-230-wsp-20010705-a   section:8.4.2.1 Basic rules
 * Quoted-string = <Octet 34> *TEXT End-of-string
 * The TEXT encodes an RFC2616 Quoted-string with the enclosing quotation-marks <"> removed
 * @param value
 * @return true
 * @return false
 */
bool MmsEncodeBuffer::EncodeQuotedText(std::string value)
{
    if (value.empty()) {
        return true;
    }
    if (!WriteByte('\"')) {
        TELEPHONY_LOGE("Encode buffer WriteByte fail.");
        return false;
    }
    for (std::size_t i = 0; i < value.length(); i++) {
        if (!WriteByte(value.at(i))) {
            TELEPHONY_LOGE("Encode buffer WriteByte fail.");
            return false;
        }
    }
    if (!WriteByte(0)) {
        TELEPHONY_LOGE("Encode buffer WriteByte fail.");
        return false;
    }
    return true;
}

/**
 * @brief EncodeTokenText
 * wap-230-wsp-20010705-a   section:8.4.2.2 Length
 * Token-text = Token End-of-string
 * End-of-string = <Octet 0>
 * @param value
 * @return true
 * @return false
 */
bool MmsEncodeBuffer::EncodeTokenText(std::string value)
{
    if (value.empty()) {
        return true;
    }
    for (std::size_t i = 0; i < value.length(); i++) {
        if (!MmsDecodeBuffer::CharIsToken(value.at(i))) {
            TELEPHONY_LOGE("Encode buffer EncodeTokenText fail.");
            return false;
        }
    }
    for (std::size_t i = 0; i < value.length(); i++) {
        if (!WriteByte(value.at(i))) {
            TELEPHONY_LOGE("Encode buffer WriteByte fail.");
            return false;
        }
    }
    if (!WriteByte(0)) {
        TELEPHONY_LOGE("Encode buffer WriteByte fail.");
        return false;
    }
    return true;
}
} // namespace Telephony
} // namespace OHOS
