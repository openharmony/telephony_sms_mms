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

#include "mms_decode_buffer.h"

#include "telephony_log_wrapper.h"

#include "securec.h"

namespace OHOS {
namespace Telephony {
using namespace std;

bool MmsDecodeBuffer::PeekOneByte(uint8_t &oneByte)
{
    if (curPosition_ >= totolLength_) {
        TELEPHONY_LOGE("Decode buffer current position invalid.");
        return false;
    }
    oneByte = pduBuffer_[curPosition_];
    return true;
}

bool MmsDecodeBuffer::GetOneByte(uint8_t &oneByte)
{
    if (curPosition_ >= totolLength_) {
        TELEPHONY_LOGE("Decode buffer current position invalid.");
        return false;
    }
    oneByte = pduBuffer_[curPosition_];
    curPosition_++;
    return true;
}

bool MmsDecodeBuffer::IncreasePointer(uint32_t offset)
{
    if ((offset > totolLength_) || ((curPosition_ + offset) > totolLength_)) {
        TELEPHONY_LOGE("Decode buffer current position invalid.");
        return false;
    }
    curPosition_ += offset;
    return true;
}

bool MmsDecodeBuffer::DecreasePointer(uint32_t offset)
{
    if (offset > curPosition_) {
        TELEPHONY_LOGE("Decode buffer current position invalid.");
        return false;
    }
    curPosition_ -= offset;
    return true;
}

/**
 * @brief DecodeUintvar
 * wap-230-wsp-20010705-a   section:8.4.2.1 Basic rules
 * Uintvar-integer = 1*5 OCTET
 * @param uintVar
 * @param count
 * @return true
 * @return false
 */
bool MmsDecodeBuffer::DecodeUintvar(uint32_t &uintVar, uint32_t &count)
{
    const uint8_t setNotHighestBitZero = 0x80;
    const uint8_t setHighestBitZero = 0x7f;

    uint8_t oneByte = 0;
    count = 0;
    if (!GetOneByte(oneByte)) {
        TELEPHONY_LOGE("Decode buffer GetOneByte fail.");
        return false;
    }
    count++;
    uintVar = 0;
    while ((oneByte & setNotHighestBitZero) != 0) {
        uintVar = uintVar << 0x07;
        uintVar |= oneByte & setHighestBitZero;

        if (!GetOneByte(oneByte)) {
            TELEPHONY_LOGE("Decode buffer GetOneByte fail.");
            return false;
        }
        count++;
    }
    uintVar = uintVar << 0x07;
    uintVar |= oneByte & setHighestBitZero;
    return true;
}

/**
 * @brief DecodeShortLength
 * wap-230-wsp-20010705-a.pdf   section:8.4.2.2 Length
 * Short-length = <Any octet 0-30>
 * @param sValueLength
 * @return true
 * @return false
 */
bool MmsDecodeBuffer::DecodeShortLength(uint8_t &sValueLength)
{
    uint8_t oneByte = 0;
    const unsigned char minShortOctet = 30;
    if (!GetOneByte(oneByte)) {
        TELEPHONY_LOGE("Decode buffer GetOneByte fail.");
        return false;
    }
    if (oneByte <= minShortOctet) {
        sValueLength = oneByte;
        return true;
    }
    return false;
}

/**
 * @brief DecodeValueLengthReturnLen
 * wap-230-wsp-20010705-a.pdf   section:8.4.2.2 Length
 * value-length = short-length | (Length-quote Length)
 * short-length = <Any octet 0-30>
 * Length-quote = <Octet 31>
 * (Length-quote Length) = Length-quote + Uintvar-length
 * @param valueLength
 * @param length
 * @return true
 * @return false
 */
bool MmsDecodeBuffer::DecodeValueLengthReturnLen(uint32_t &valueLength, uint32_t &length)
{
    const uint8_t lengthQuote = 0x1f;
    const uint8_t maxShortLength = 30;
    length = 0;
    uint32_t uintvar = 0;
    uint8_t oneByte = 0;
    valueLength = 0;
    if (!GetOneByte(oneByte)) {
        DecreasePointer(1);
        TELEPHONY_LOGE("Decode buffer GetOneByte fail.");
        return false;
    }

    if (oneByte <= maxShortLength) {
        valueLength = oneByte;
        length = 1;
        return true;
    } else if (oneByte == lengthQuote) {
        /* Length-quote = 0x1F */
        uint32_t count = 0;
        if (!DecodeUintvar(uintvar, count)) {
            TELEPHONY_LOGE("Decode buffer DecodeUintvar fail.");
            return false;
        }
        valueLength = uintvar;
        length = count + 1;
        return true;
    }
    return false;
}

/**
 * @brief DecodeValueLength
 * wap-230-wsp-20010705-a   section:8.4.2.2 Length
 * value-length = short-length | (Length-quote Length)
 * short-length = <Any octet 0-30>
 * Length-quote = <Octet 31>
 * (Length-quote Length) = Length-quote + Uintvar-length
 * @param valueLength
 * @return true
 * @return false
 */
bool MmsDecodeBuffer::DecodeValueLength(uint32_t &valueLength)
{
    const uint8_t lengthQuote = 0x1f;
    const uint8_t maxShortLength = 30;

    uint32_t uintvar = 0;
    uint8_t oneByte = 0;
    valueLength = 0;
    if (!GetOneByte(oneByte)) {
        DecreasePointer(1);
        TELEPHONY_LOGE("Decode buffer GetOneByte fail.");
        return false;
    }

    if (oneByte <= maxShortLength) {
        valueLength = oneByte;
        return true;
    } else if (oneByte == lengthQuote) {
        /* Length-quote = 0x1F */
        uint32_t count = 0;
        if (!DecodeUintvar(uintvar, count)) {
            TELEPHONY_LOGE("Decode buffer DecodeUintvar fail.");
            return false;
        }
        valueLength = uintvar;
        return true;
    }
    return false;
}

/**
 * @brief CharIsToken
 * RFC2616   section:2.2 Basic Rules
 * CHAR           = <any US-ASCII character (octets 0 - 127)>
 * CTL            = <any US-ASCII control character
 *                   (octets 0 - 31) and DEL (127)>
 * separators     = "(" | ")" | "<" | ">" | "@"
 *                     | "," | ";" | ":" | "\" | <">
 *                     | "/" | "[" | "]" | "?" | "="
 *                     | "{" | "}" | SP | HT
 * @param oneByte
 * @return true
 * @return false
 */
bool MmsDecodeBuffer::CharIsToken(uint8_t oneByte)
{
    const uint8_t minToken = 33;
    const uint8_t maxToken = 126;
    if (oneByte < minToken || oneByte > maxToken) {
        return false;
    }
    switch (oneByte) {
        case '"':
        case '(':
        case ')':
        case ',':
        case '/':
        case ':':
        case ';':
        case '<':
        case '=':
        case '>':
        case '?':
        case '@':
        case '[':
        case '\\':
        case ']':
        case '{':
        case '}':
            return false;
        default:
            return true;
    }
}

/**
 * @brief DecodeTokenText
 * wap-230-wsp-20010705-a   section:8.4.2.2 Length
 * Token-text = Token End-of-string
 * End-of-string = <Octet 0>
 * @param str
 * @param len
 * @return true
 * @return false
 */
bool MmsDecodeBuffer::DecodeTokenText(std::string &str, uint32_t &len)
{
    len = 0;
    str.clear();
    uint8_t oneByte = 0;
    const uint8_t minToken = 33;
    const uint8_t maxToken = 126;
    if (!PeekOneByte(oneByte)) {
        TELEPHONY_LOGE("Decode buffer PeekOneByte fail.");
        return false;
    }
    if (oneByte < minToken || oneByte > maxToken) {
        TELEPHONY_LOGE("Decode buffer DecodeTokenText fail.");
        return false;
    }
    while (GetOneByte(oneByte)) {
        if (oneByte != 0) {
            if (CharIsToken(oneByte)) {
                str += oneByte;
                len++;
            }
        } else {
            break;
        }
    }
    return (len > 0) ? true : false;
}

/**
 * @brief DecodeText
 * wap-230-wsp-20010705-a   section:8.4.2.1 Basic rules
 * Text-string = [Quote] *TEXT End-of-string
 * Quote = <Octet 127>
 * End-of-string = <Octet 0>
 * @param str
 * @param len
 * @return true
 * @return false
 */
bool MmsDecodeBuffer::DecodeText(std::string &str, uint32_t &len)
{
    uint8_t oneByte = 0;
    const unsigned char quoteChar = 127;

    len = 0;
    str.clear();
    if (!GetOneByte(oneByte)) {
        TELEPHONY_LOGE("Decode buffer GetOneByte fail.");
        return false;
    }

    // ignore quote
    if (oneByte != quoteChar) {
        this->DecreasePointer(1);
    } else {
        len++;
    }

    while (GetOneByte(oneByte)) {
        if (oneByte != 0) {
            str += oneByte;
            len++;
        } else {
            break;
        }
    }
    return (len > 0) ? true : false;
}

/**
 * @brief DecodeQuotedText
 * wap-230-wsp-20010705-a   section:8.4.2.1 Basic rules
 * Quoted-string = <Octet 34> *TEXT End-of-string
 * The TEXT encodes an RFC2616 Quoted-string with the enclosing quotation-marks <"> removed
 * @param str
 * @param len
 * @return true
 * @return false
 */
bool MmsDecodeBuffer::DecodeQuotedText(std::string &str, uint32_t &len)
{
    const uint8_t quotedFlag = 0x22;
    uint8_t oneByte = 0;
    len = 0;
    str.clear();
    while (GetOneByte(oneByte)) {
        if (oneByte != 0) {
            if (oneByte != quotedFlag) {
                str += oneByte;
            }
            len++;
        } else {
            break;
        }
    }
    return (len > 0) ? true : false;
}

/**
 * @brief DecodeShortInteger
 * wap-230-wsp-20010705-a   section:8.4.2.1 Basic rules
 * Short-integer = OCTET
 * Integers in range 0-127 shall be encoded as a one octet value with
 * the most significant bit set to one (1xxx xxxx) and with the value
 * in the remaining least significant bits
 * @param sInteger
 * @return true
 * @return false
 */
bool MmsDecodeBuffer::DecodeShortInteger(uint8_t &sInteger)
{
    const uint8_t setHighestBitZero = 0x7f;
    const uint8_t maxShortInteger = 0x7f;
    uint8_t oneByte = 0;
    sInteger = 0;
    if (!GetOneByte(oneByte)) {
        TELEPHONY_LOGE("Decode buffer GetOneByte fail.");
        return false;
    }
    if (oneByte > maxShortInteger) {
        sInteger = oneByte & setHighestBitZero;
        return true;
    }
    return false;
}

/**
 * @brief DecodeLongInteger
 * wap-230-wsp-20010705-a   section:8.4.2.1 Basic rules
 * Long-integer = [Short-length] [Multi-octet-integer]
 * Short-length = 1 byte
 * Multi-octet-integer = <Short-length> bytes
 * @param value
 * @return true
 * @return false
 */
bool MmsDecodeBuffer::DecodeLongInteger(uint64_t &value)
{
    unsigned char oneByte = 0;
    uint64_t result = 0;
    uint32_t valuelength = 0;
    if (GetOneByte(oneByte) != true) {
        TELEPHONY_LOGE("Decode buffer GetOneByte fail.");
        return false;
    }
    if (oneByte > LONG_VALUE_LEN_MAX) {
        TELEPHONY_LOGE("Decode buffer DecodeLongInteger fail.");
        return false;
    }
    valuelength = oneByte;

    for (uint32_t i = 0; i < valuelength; i++) {
        if (GetOneByte(oneByte) != true) {
            TELEPHONY_LOGE("Decode buffer GetOneByte fail.");
            return false;
        }
        result = (result << 0x08);
        result |= oneByte;
    }
    value = result;
    return true;
}

/**
 * @brief DecodeInteger
 * wap-230-wsp-20010705-a   section:8.4.2.3 Parameter Values
 * Integer-Value = Short-integer | Long-integer
 * @param iInteger
 * @return true
 * @return false
 */
bool MmsDecodeBuffer::DecodeInteger(uint64_t &iInteger)
{
    iInteger = 0;
    if (DecodeIsShortInt()) {
        uint8_t sInteger = 0;
        if (DecodeShortInteger(sInteger)) {
            iInteger = sInteger;
            return true;
        }
    } else {
        if (DecodeLongInteger(iInteger)) {
            return true;
        }
    }
    return false;
}

/**
 * @brief DecodeIsShortInt
 * wap-230-wsp-20010705-a   section:8.4.1.2 Field values
 * (128 - 255) It is an encoded 7-bit value; this header has no more data
 * @return true
 * @return false
 */
bool MmsDecodeBuffer::DecodeIsShortInt()
{
    const uint8_t maxShortInteger = 0x7f;
    uint8_t oneByte = 0;
    PeekOneByte(oneByte);
    if (oneByte > maxShortInteger) {
        return true;
    }
    return false;
}

/**
 * @brief DecodeIsString
 * wap-230-wsp-20010705-a   section:8.4.1.2 Field values
 * (32 - 127) The value is a text string, terminated by a zero octet (NUL character)
 * @return true
 * @return false
 */
bool MmsDecodeBuffer::DecodeIsString()
{
    const uint8_t minText = 0x20;
    const uint8_t maxText = 0x7f;
    uint8_t oneByte = 0;
    PeekOneByte(oneByte);
    if (minText <= oneByte && maxText >= oneByte) {
        return true;
    }
    return false;
}

/**
 * @brief DecodeIsValueLength
 * wap-230-wsp-20010705-a   section:8.4.1.2 Field values
 * (0 - 30) This octet is followed by the indicated number (0 –30) of data octets
 * 31 This octet is followed by a uintvar, which indicates the number of data octets after it
 * @return true
 * @return false
 */
bool MmsDecodeBuffer::DecodeIsValueLength()
{
    const uint8_t lengthQuote = 0x1f;
    uint8_t oneByte = 0;
    PeekOneByte(oneByte);
    if (oneByte <= lengthQuote) {
        return true;
    }
    return false;
}

void MmsDecodeBuffer::MarkPosition()
{
    savePosition_ = curPosition_;
}

void MmsDecodeBuffer::UnMarkPosition()
{
    curPosition_ = savePosition_;
}
} // namespace Telephony
} // namespace OHOS
