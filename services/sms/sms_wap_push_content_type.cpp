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
#include "sms_wap_push_content_type.h"

#include "telephony_log_wrapper.h"
#include "sms_wap_push_buffer.h"

namespace OHOS {
namespace Telephony {
/**
 * @brief Construct a new Sms Wap Push Content Type:: Sms Wap Push Content Type
 * wap-230-wsp-20010705-a
 * Table 42. Character Set Assignment Examples
 */
SmsWapPushContentType::SmsWapPushContentType()
{
    mapCharSet_.emplace("US-ASCII", 0x03);
    mapCharSet_.emplace("UTF-16", 0x03F7);
    mapCharSet_.emplace("CSUNICODE", 0x03E8);
    mapCharSet_.emplace("UTF-8", 0x6A);
    mapCharSet_.emplace("ISO-2022-KR", 0x25);
    mapCharSet_.emplace("KS_C_5601-1987", 0x24);
    mapCharSet_.emplace("EUC-KR", 0x26);
    mapCharSet_.emplace("ISO-2022-JP", 0x27);
    mapCharSet_.emplace("ISO-2022-JP-2", 0x28);
    mapCharSet_.emplace("ISO_8859-1", 0x04);
    mapCharSet_.emplace("ISO_8859-2", 0x05);
    mapCharSet_.emplace("ISO-8859-3", 0x06);
    mapCharSet_.emplace("ISO-8859-4", 0x07);
    mapCharSet_.emplace("ISO-8859-5", 0x08);
    mapCharSet_.emplace("ISO-8859-6", 0x09);
    mapCharSet_.emplace("ISO-8859-7", 0x0A);
    mapCharSet_.emplace("ISO-8859-8", 0x0B);
    mapCharSet_.emplace("ISO-8859-9", 0x0C);
    mapCharSet_.emplace("ISO-8859-10", 0x0D);
    mapCharSet_.emplace("ISO-8859-15", 0x6F);
    mapCharSet_.emplace("SHIFT_JIS", 0x11);
    mapCharSet_.emplace("EUC-JP", 0x13);
    mapCharSet_.emplace("GB2312", 0x07E9);
    mapCharSet_.emplace("BIG5", 0x0d);
    mapCharSet_.emplace("WINDOWS-1251", 0x08CB);
    mapCharSet_.emplace("KOI8-R", 0x0824);
    mapCharSet_.emplace("KOI8-U", 0x0828);
}

/**
 * @brief DecodeContentType
 * wap-230-wsp-20010705-a   section:8.4.2.24 Content type field section:8.4.2.1 Basic rules
 * Content-type-value = Constrained-media | Content-general-form
 * Constrained-media = Constrained-encoding
 * Constrained-encoding = Extension-Media | Short-integer
 * Extension-media = *TEXT End-of-string
 * Content-general-form = Value-length Media-type
 * Media-type = (Well-known-media | Extension-Media) *(Parameter)
 * Well-known-media = Integer-value
 * Parameter = Typed-parameter | Untyped-parameter
 * @param decodeBuffer
 * @param contentLength
 * @return true
 * @return false
 */
bool SmsWapPushContentType::DecodeContentType(SmsWapPushBuffer &decodeBuffer, int32_t &contentLength)
{
    const uint8_t setHighestBitZero = 0x7f;

    if (decodeBuffer.DecodeIsShortInt()) {
        uint8_t oneByte = 0;
        if (!decodeBuffer.GetOneByte(oneByte)) {
            TELEPHONY_LOGE("Wap push decode contentType GetOneByte fail.");
            return false;
        }
        contentType_ = GetContentTypeFromInt(oneByte & setHighestBitZero);
        contentLength = 1;
        return true;
    }

    if (decodeBuffer.DecodeIsString()) {
        std::string sType = "";
        uint32_t len = 0;
        decodeBuffer.DecodeText(sType, len);
        contentLength = static_cast<int32_t>(len + 1);
        contentType_ = sType;
        return true; // 2
    }

    if (!DecodeCTGeneralForm(decodeBuffer, contentLength)) {
        TELEPHONY_LOGE("Decode contentType DecodeMmsCTGeneralForm fail.");
        return false;
    }
    return true;
}

/**
 * @brief DecodeCTGeneralForm
 * wap-230-wsp-20010705-a   section:8.4.2.24 Content type field section:8.4.2.1 Basic rules
 * Content-type-value = Constrained-media | Content-general-form
 * Constrained-media = Constrained-encoding
 * Constrained-encoding = Extension-Media | Short-integer
 * Extension-media = *TEXT End-of-string
 * Content-general-form = Value-length Media-type
 * Media-type = (Well-known-media | Extension-Media) *(Parameter)
 * Well-known-media = Integer-value
 * Parameter = Typed-parameter | Untyped-parameter
 * @param decodeBuffer
 * @param contentLength
 * @return true
 * @return false
 */
bool SmsWapPushContentType::DecodeCTGeneralForm(SmsWapPushBuffer &decodeBuffer, int32_t &contentLength)
{
    const uint8_t setHighestBitZero = 0x7f;

    /** false indicated no more data */
    if (!decodeBuffer.DecodeIsValueLength()) {
        TELEPHONY_LOGE("Wap push decode contentType DecodeIsValueLength fail.");
        return false;
    }

    uint32_t valueLength = 0;
    uint32_t returnLength = 0;
    if (!decodeBuffer.DecodeValueLengthReturnLen(valueLength, returnLength)) {
        TELEPHONY_LOGE("Wap push decode contentType DecodeValueLengthReturnLen fail.");
        return false;
    }
    contentLength = static_cast<int32_t>(valueLength + returnLength);

    uint8_t oneByte = 0;
    if (!decodeBuffer.PeekOneByte(oneByte)) {
        TELEPHONY_LOGE("Wap push decode contentType PeekOneByte fail.");
        return false;
    }
    if (decodeBuffer.DecodeIsShortInt()) {
        contentType_ = GetContentTypeFromInt(oneByte & setHighestBitZero);
        if (!decodeBuffer.IncreasePointer(1)) {
            TELEPHONY_LOGE("Wap push decode contentType IncreasePointer fail.");
            return false;
        }
        if (valueLength == 0) {
            TELEPHONY_LOGI("Wap push decode contentType empty.");
            return false;
        }
        valueLength--;
    } else if (decodeBuffer.DecodeIsString()) {
        std::string sType = "";
        uint32_t len = 0;
        decodeBuffer.DecodeText(sType, len);
        valueLength -= len + 1;
        contentType_ = sType;
    } else {
        TELEPHONY_LOGI("Wap push decode contentType empty.");
        return false;
    }

    if (!DecodeParameter(decodeBuffer, valueLength)) {
        TELEPHONY_LOGE("Wap push decode contentType DecodeParameter fail.");
        return false;
    }
    return true;
}

/**
 * @brief DecodeParameter
 * wap-230-wsp-20010705-a   section:8.4.2.4 Parameter
 * Parameter = Typed-parameter | Untyped-parameter
 * Typed-parameter = Well-known-parameter-token Typed-value
 * Well-known-parameter-token = Integer-value
 * Typed-value = Compact-value | Text-value
 * Compact-value = Integer-value |
 *                 Date-value | Delta-seconds-value | Q-value | Version-value |
 *                 Uri-value
 * Untyped-parameter = Token-text Untyped-value
 * Untyped-value = Integer-value | Text-value
 * Delta-seconds-value = Long-integer
 * Q-value = 1*2 OCTET
 * @param decodeBuffer
 * @param valueLength
 * @return true
 * @return false
 */
bool SmsWapPushContentType::DecodeParameter(SmsWapPushBuffer &decodeBuffer, int32_t valueLength)
{
    uint8_t oneByte = 0;
    uint8_t paramCode = 0;
    while (valueLength > 0) {
        if (!decodeBuffer.GetOneByte(oneByte)) {
            TELEPHONY_LOGE("Wap push DecodeParameter GetOneByte fail.");
            return false;
        }
        paramCode = oneByte;
        valueLength--;
        switch (static_cast<WapContentParam>(paramCode)) {
            case WapContentParam::CT_P_CHARSET: {
                if (!DecodeCharsetField(decodeBuffer, valueLength)) {
                    TELEPHONY_LOGE("Wap push DecodeParameter DecodeCharsetField fail.");
                    return false;
                }
                break;
            }
            case WapContentParam::CT_P_FILENAME:
            case WapContentParam::CT_P_FILENAME_VALUE:
            case WapContentParam::CT_P_START_VALUE:
            case WapContentParam::CT_P_START:
            case WapContentParam::CT_P_NAME:
            case WapContentParam::CT_P_NAME_VALUE:
            case WapContentParam::CT_P_START_INFO:
            case WapContentParam::CT_P_START_INFO_VALUE: {
                if (!DecodeTextField(decodeBuffer, paramCode, valueLength)) {
                    TELEPHONY_LOGE("Wap push DecodeParameter DecodeTextField fail.");
                    return false;
                }
                break;
            }
            case WapContentParam::CT_P_TYPE:
            case WapContentParam::CT_P_TYPE_STRING: {
                if (!DecodeTypeField(decodeBuffer, valueLength)) {
                    TELEPHONY_LOGE("Wap push DecodeParameter DecodeTypeField fail.");
                    return false;
                }
                break;
            }
            default: {
                if (!decodeBuffer.IncreasePointer(valueLength)) {
                    TELEPHONY_LOGE("Wap push DecodeParameter IncreasePointer fail.");
                    return false;
                }
                valueLength = 0;
            }
        }
    }
    return true;
}

std::string SmsWapPushContentType::GetContentType()
{
    return contentType_;
}

bool SmsWapPushContentType::SetContentType(std::string str)
{
    contentType_ = str;
    return true;
}

/**
 * @brief DecodeTextField
 * wap-230-wsp-20010705-a   section:8.4.2.1 Basic rules
 * Text-string = [Quote] *TEXT End-of-string
 * Quote = <Octet 127>
 * End-of-string = <Octet 0>
 * @param decodeBuffer
 * @param field
 * @param valueLength
 * @return true
 * @return false
 */
bool SmsWapPushContentType::DecodeTextField(SmsWapPushBuffer &decodeBuffer, uint8_t field, int32_t &valueLength)
{
    std::string str = "";
    uint32_t len = 0;
    if (!decodeBuffer.DecodeText(str, len)) {
        TELEPHONY_LOGE("Wap push DecodeTextField DecodeText fail.");
        return false;
    }
    textParameterMap_.insert(std::make_pair(field, str));
    valueLength -= static_cast<int32_t>(len);
    valueLength -= 1;
    return true;
}

/**
 * @brief DecodeCharsetField
 * wap-230-wsp-20010705-a   section:8.4.2.8 Accept charset field
 * Well-known-charset = Any-charset | Integer-value
 * Any-charset = <Octet 128>
 * @param decodeBuffer
 * @param valueLength
 * @return true
 * @return false
 */
bool SmsWapPushContentType::DecodeCharsetField(SmsWapPushBuffer &decodeBuffer, int32_t &valueLength)
{
    const uint8_t TEXT_MAX = 0x7F;
    const uint8_t TEXT_MIN = 0x20;
    int32_t charset = 0;
    uint8_t oneByte = 0;
    if (decodeBuffer.PeekOneByte(oneByte) == false) {
        TELEPHONY_LOGE("Wap push DecodeCharsetField PeekOneByte fail.");
        return false;
    }
    if (((oneByte > TEXT_MIN) && (oneByte < TEXT_MAX)) || (oneByte == 0)) {
        std::string sCharset = "";
        uint32_t len = 0;

        if (!decodeBuffer.DecodeText(sCharset, len)) {
            TELEPHONY_LOGE("Wap push DecodeCharsetField DecodeText fail.");
            return false;
        }
        valueLength -= static_cast<int32_t>(len + 1);
        uint32_t tmpCharSet = 0;
        if (!GetCharSetIntFromString(tmpCharSet, sCharset)) {
            TELEPHONY_LOGE("Wap push DecodeCharsetField GetCharSetIntFromString fail.");
            return false;
        }
        charset = static_cast<int32_t>(tmpCharSet);
    } else {
        uint32_t startPosition = decodeBuffer.GetCurPosition();
        uint64_t temp = 0;
        if (!decodeBuffer.DecodeInteger(temp)) {
            TELEPHONY_LOGE("Wap push DecodeCharsetField DecodeInteger fail.");
            return false;
        }
        charset = static_cast<int32_t>(temp);
        uint32_t endPosition = decodeBuffer.GetCurPosition();
        valueLength -= static_cast<int32_t>(endPosition - startPosition);
    }
    charset_ = static_cast<uint32_t>(charset);
    return true;
}

/**
 * @brief DecodeTypeField
 * wap-230-wsp-20010705-a   section:8.4.2.1 Basic rules
 * Constrained-encoding = Extension-Media | Short-integer
 * Extension-media = *TEXT End-of-string
 * @param decodeBuffer
 * @param valueLength
 * @return true
 * @return false
 */
bool SmsWapPushContentType::DecodeTypeField(SmsWapPushBuffer &decodeBuffer, int32_t &valueLength)
{
    const uint8_t TEXT_MAX = 0x7F;
    uint8_t oneByte = 0;
    if (decodeBuffer.GetOneByte(oneByte) == false) {
        TELEPHONY_LOGE("Wap push DecodeTypeField GetOneByte fail.");
        return false;
    }

    if (oneByte > TEXT_MAX) {
        type_ = GetContentTypeFromInt(oneByte & TEXT_MAX);
        valueLength -= 1;
    } else {
        if (!decodeBuffer.DecreasePointer(1)) {
            TELEPHONY_LOGE("Wap push DecodeTypeField DecreasePointer fail.");
            return false;
        }
        std::string sType = "";
        uint32_t len = 0;
        if (!decodeBuffer.DecodeText(sType, len)) {
            TELEPHONY_LOGE("Wap push DecodeTypeField DecodeText fail.");
            return false;
        }
        valueLength -= static_cast<int32_t>(len);
        valueLength -= 1;
        type_ = sType;
    }
    return true;
}

bool SmsWapPushContentType::GetCharSetIntFromString(uint32_t &charSet, const std::string &strCharSet)
{
    auto iterMap = mapCharSet_.find(strCharSet);
    if (iterMap != mapCharSet_.end()) {
        charSet = iterMap->second;
        return true;
    }
    return false;
}

std::string SmsWapPushContentType::GetContentTypeFromInt(uint8_t type)
{
    for (unsigned int i = 0; i < sizeof(wapContentNames) / sizeof(wapContentNames[0]); i++) {
        if (type == static_cast<uint8_t>(wapContentNames[i].key)) {
            return wapContentNames[i].value;
        }
    }
    return "*/*";
}
} // namespace Telephony
} // namespace OHOS
