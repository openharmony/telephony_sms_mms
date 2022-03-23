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
#include "mms_content_type.h"

#include "telephony_log_wrapper.h"
#include "mms_decode_buffer.h"
#include "mms_charset.h"

namespace OHOS {
namespace Telephony {
void MmsContentType::DumpMmsContentType()
{
    TELEPHONY_LOGI("******** Start DumpMmsContentType **********");
    TELEPHONY_LOGI("contentType : %{public}s", contentType_.c_str());
    msgContentParm_.DumpContentParam();
    TELEPHONY_LOGI("******** End DumpMmsContentType ************");
}

MmsContentType::MmsContentType(const MmsContentType &srcContentType)
{
    contentType_ = srcContentType.contentType_;
    msgContentParm_ = srcContentType.msgContentParm_;
}

MmsContentType &MmsContentType::operator=(const MmsContentType &srcContentType)
{
    if (this != &srcContentType) {
        contentType_ = srcContentType.contentType_;
        msgContentParm_ = srcContentType.msgContentParm_;
    }
    return *this;
}

/**
 * @brief DecodeMmsContentType
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
bool MmsContentType::DecodeMmsContentType(MmsDecodeBuffer &decodeBuffer, int32_t &contentLength)
{
    const uint8_t setHighestBitZero = 0x7f;
    uint8_t oneByte = 0;
    if (decodeBuffer.DecodeIsShortInt()) {
        if (!decodeBuffer.GetOneByte(oneByte)) {
            TELEPHONY_LOGE("Decode contentType GetOneByte fail.");
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

    if (!DecodeMmsCTGeneralForm(decodeBuffer, contentLength)) {
        TELEPHONY_LOGE("Decode contentType DecodeMmsCTGeneralForm fail.");
        return false;
    }
    return true;
}

/**
 * @brief DecodeMmsCTGeneralForm
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
bool MmsContentType::DecodeMmsCTGeneralForm(MmsDecodeBuffer &decodeBuffer, int32_t &contentLength)
{
    const uint8_t setHighestBitZero = 0x7f;

   /** false indicated no more data */
    if (!decodeBuffer.DecodeIsValueLength()) {
        TELEPHONY_LOGE("Decode contentType DecodeIsValueLength fail.");
        return false;
    }

    uint32_t valueLength = 0;
    uint32_t returnLength = 0;
    if (!decodeBuffer.DecodeValueLengthReturnLen(valueLength, returnLength)) {
        TELEPHONY_LOGE("Decode contentType DecodeValueLengthReturnLen fail.");
        return false;
    }
    contentLength = static_cast<int32_t>(valueLength + returnLength);

    uint8_t oneByte = 0;
    if (!decodeBuffer.PeekOneByte(oneByte)) {
        TELEPHONY_LOGE("Decode contentType PeekOneByte fail.");
        return false;
    }
    if (decodeBuffer.DecodeIsShortInt()) {
        contentType_ = GetContentTypeFromInt(oneByte & setHighestBitZero);
        if (!decodeBuffer.IncreasePointer(1)) {
            TELEPHONY_LOGE("Decode contentType IncreasePointer fail.");
            return false;
        }
        if (valueLength == 0) {
            TELEPHONY_LOGE("Decode contentType valueLength empty.");
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
        TELEPHONY_LOGE("Decode contentType DecodeMmsContentType fail.");
        return false;
    }

    if (!DecodeParameter(decodeBuffer, valueLength)) {
        TELEPHONY_LOGE("Decode contentType DecodeParameter fail.");
        return false;
    }
    return true;
}

std::string MmsContentType::GetContentTypeFromInt(uint8_t type)
{
    for (unsigned int i = 0; i < sizeof(mmsContentNames) / sizeof(mmsContentNames[0]); i++) {
        if (type == static_cast<uint8_t>(mmsContentNames[i].key)) {
            return mmsContentNames[i].value;
        }
    }
    return "*/*";
}

int8_t MmsContentType::GetContentTypeFromString(std::string str)
{
    for (unsigned int i = 0; i < sizeof(mmsContentNames) / sizeof(mmsContentNames[0]); i++) {
        if (str == std::string(mmsContentNames[i].value)) {
            return i;
        }
    }
    return -1;
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
bool MmsContentType::DecodeParameter(MmsDecodeBuffer &decodeBuffer, int32_t valueLength)
{
    uint8_t oneByte = 0;
    uint8_t paramCode = 0;
    while (valueLength > 0) {
        if (!decodeBuffer.GetOneByte(oneByte)) {
            TELEPHONY_LOGE("Decode contentType GetOneByte fail.");
            return false;
        }
        paramCode = oneByte;
        valueLength--;
        switch (static_cast<ContentParam>(paramCode)) {
            case ContentParam::CT_P_CHARSET: {
                /* charset */
                if (!DecodeCharsetField(decodeBuffer, valueLength)) {
                    TELEPHONY_LOGE("Decode contentType DecodeCharsetField fail.");
                    return false;
                }
                break;
            }
            case ContentParam::CT_P_FILENAME:
            case ContentParam::CT_P_FILENAME_VALUE:
            case ContentParam::CT_P_START_VALUE:
            case ContentParam::CT_P_START:
            case ContentParam::CT_P_NAME:
            case ContentParam::CT_P_NAME_VALUE:
            case ContentParam::CT_P_START_INFO:
            case ContentParam::CT_P_START_INFO_VALUE: {
                if (!DecodeTextField(decodeBuffer, paramCode, valueLength)) {
                    TELEPHONY_LOGE("Decode contentType DecodeTextField fail.");
                    return false;
                }
                break;
            }
            case ContentParam::CT_P_TYPE:
            case ContentParam::CT_P_TYPE_STRING: {
                if (!DecodeTypeField(decodeBuffer, valueLength)) {
                    TELEPHONY_LOGE("Decode contentType DecodeTypeField fail.");
                    return false;
                }
                break;
            }
            default: {
                if (!decodeBuffer.IncreasePointer(valueLength)) {
                    TELEPHONY_LOGE("Decode contentType IncreasePointer fail.");
                    return false;
                }
                valueLength = 0;
            }
        }
    }
    return true;
}

bool MmsContentType::GetContentType(std::string &str)
{
    str = contentType_;
    return true;
}

bool MmsContentType::SetContentType(std::string str)
{
    contentType_ = str;
    return true;
}

bool MmsContentType::SetContentParam(MmsContentParam &contentParam)
{
    msgContentParm_ = contentParam;
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
bool MmsContentType::DecodeTextField(MmsDecodeBuffer &decodeBuffer, uint8_t field, int32_t &valueLength)
{
    std::string str = "";
    uint32_t len = 0;
    if (!decodeBuffer.DecodeText(str, len)) {
        TELEPHONY_LOGE("Decode contentType DecodeText fail.");
        return false;
    }
    msgContentParm_.GetParamMap().insert(std::make_pair(field, str));
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
bool MmsContentType::DecodeCharsetField(MmsDecodeBuffer &decodeBuffer, int32_t &valueLength)
{
    int32_t charset = 0;
    uint8_t oneByte = 0;
    const uint8_t textMinValue = 32;
    const uint8_t textMaxValue = 127;
    if (decodeBuffer.PeekOneByte(oneByte) == false) {
        TELEPHONY_LOGE("Decode contentType PeekOneByte fail.");
        return false;
    }
    if (((oneByte > textMinValue) && (oneByte < textMaxValue)) || (oneByte == 0)) {
        std::string sCharset = "";
        uint32_t len = 0;
        if (!decodeBuffer.DecodeText(sCharset, len)) {
            TELEPHONY_LOGE("Decode contentType DecodeText fail.");
            return false;
        }
        valueLength -= static_cast<int32_t>(len + 1);
        uint32_t tmpCharSet = 0;
        auto charSetInstance = DelayedSingleton<MmsCharSet>::GetInstance();
        if (charSetInstance == nullptr || (!charSetInstance->GetCharSetIntFromString(tmpCharSet, sCharset))) {
            TELEPHONY_LOGE("Decode contentType GetInstance or GetCharSetIntFromString fail.");
            return false;
        }
        charset = static_cast<int32_t>(tmpCharSet);
    } else {
        uint32_t startPosition = decodeBuffer.GetCurPosition();
        uint64_t tmp = 0;
        if (!decodeBuffer.DecodeInteger(tmp)) {
            TELEPHONY_LOGE("Decode contentType DecodeInteger fail.");
            return false;
        }
        charset = (int32_t)tmp;
        uint32_t endPosition = decodeBuffer.GetCurPosition();
        valueLength -= static_cast<int32_t>(endPosition - startPosition);
    }
    msgContentParm_.SetCharSet(charset);
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
bool MmsContentType::DecodeTypeField(MmsDecodeBuffer &decodeBuffer, int32_t &valueLength)
{
    uint8_t oneByte = 0;
    if (decodeBuffer.GetOneByte(oneByte) == false) {
        TELEPHONY_LOGE("Decode contentType GetOneByte fail.");
        return false;
    }

    if (oneByte > 0x7f) {
        msgContentParm_.SetType(GetContentTypeFromInt(oneByte & 0x7f));
        valueLength -= 1;
    } else {
        if (!decodeBuffer.DecreasePointer(1)) {
            TELEPHONY_LOGE("Decode contentType DecreasePointer fail.");
            return false;
        }

        std::string sType = "";
        uint32_t len = 0;
        if (!decodeBuffer.DecodeText(sType, len)) {
            TELEPHONY_LOGE("Decode contentType DecodeText fail.");
            return false;
        }
        valueLength -= static_cast<int32_t>(len);
        valueLength -= 1;
        msgContentParm_.SetType(sType);
    }
    return true;
}

/**
 * @brief EncodeTextField
 * wap-230-wsp-20010705-a   section:8.4.2.1 Basic rules
 * Text-string = [Quote] *TEXT End-of-string
 * Quote = <Octet 127>
 * End-of-string = <Octet 0>
 * @param encodeBuffer
 * @return true
 * @return false
 */
bool MmsContentType::EncodeTextField(MmsEncodeBuffer &encodeBuffer)
{
    const uint8_t textParamLen = 8;
    uint8_t fields[textParamLen] = {static_cast<uint8_t>(ContentParam::CT_P_FILENAME),
        static_cast<uint8_t>(ContentParam::CT_P_FILENAME_VALUE),
        static_cast<uint8_t>(ContentParam::CT_P_START_VALUE), static_cast<uint8_t>(ContentParam::CT_P_START),
        static_cast<uint8_t>(ContentParam::CT_P_NAME), static_cast<uint8_t>(ContentParam::CT_P_NAME_VALUE),
        static_cast<uint8_t>(ContentParam::CT_P_START_INFO),
        static_cast<uint8_t>(ContentParam::CT_P_START_INFO_VALUE)};

    for (size_t i = 0; i < sizeof(fields); i++) {
        if (msgContentParm_.GetParamMap().find(fields[i]) != msgContentParm_.GetParamMap().end()) {
            if (!encodeBuffer.WriteByte(fields[i])) {
                TELEPHONY_LOGE("Encode contentType WriteByte fail.");
                return false;
            }
            if (!encodeBuffer.EncodeText(msgContentParm_.GetParamMap()[fields[i]])) {
                TELEPHONY_LOGE("Encode contentType EncodeText fail.");
                return false;
            }
        }
    }
    return true;
}

/**
 * @brief EncodeCharsetField
 * wap-230-wsp-20010705-a   section:8.4.2.8 Accept charset field
 * Well-known-charset = Any-charset | Integer-value
 * Any-charset = <Octet 128>
 * @param encodeBuffer
 * @return true
 * @return false
 */
bool MmsContentType::EncodeCharsetField(MmsEncodeBuffer &encodeBuffer)
{
    if (msgContentParm_.GetCharSet() == 0) {
        return true;
    }
    if (!encodeBuffer.WriteByte(static_cast<uint8_t>(ContentParam::CT_P_CHARSET))) {
        TELEPHONY_LOGE("Encode contentType WriteByte fail.");
        return false;
    }
    if (!encodeBuffer.EncodeLongInteger(msgContentParm_.GetCharSet())) {
        TELEPHONY_LOGE("Encode contentType EncodeLongInteger fail.");
        return false;
    }
    return true;
}

/**
 * @brief EncodeTypeField
 * wap-230-wsp-20010705-a   section:8.4.2.1 Basic rules
 * Constrained-encoding = Extension-Media | Short-integer
 * Extension-media = *TEXT End-of-string
 * @param encodeBuffer
 * @return true
 * @return false
 */
bool MmsContentType::EncodeTypeField(MmsEncodeBuffer &encodeBuffer)
{
    if (msgContentParm_.GetType().empty()) {
        return true;
    }
    if (!encodeBuffer.WriteByte(static_cast<uint8_t>(ContentParam::CT_P_TYPE))) {
        TELEPHONY_LOGE("Encode contentType WriteByte fail.");
        return false;
    }
    if (!encodeBuffer.EncodeText(msgContentParm_.GetType())) {
        TELEPHONY_LOGE("Encode contentType EncodeText fail.");
        return false;
    }
    return true;
}

/**
 * @brief EncodeMmsBodyPartContentParam
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
 * @param encodeBuffer
 * @return true
 * @return false
 */
bool MmsContentType::EncodeMmsBodyPartContentParam(MmsEncodeBuffer &encodeBuffer)
{
    if (!EncodeTextField(encodeBuffer)) {
        TELEPHONY_LOGE("Encode contentType EncodeTextField fail.");
        return false;
    }
    if (!EncodeCharsetField(encodeBuffer)) {
        TELEPHONY_LOGE("Encode contentType EncodeCharsetField fail.");
        return false;
    }
    if (!EncodeTypeField(encodeBuffer)) {
        TELEPHONY_LOGE("Encode contentType EncodeTypeField fail.");
        return false;
    }
    return true;
}

/**
 * @brief EncodeMmsBodyPartContentType
 * wap-230-wsp-20010705-a   section:8.4.2.24 Content type field section:8.4.2.1 Basic rules
 * Content-type-value = Constrained-media | Content-general-form
 * Constrained-media = Constrained-encoding
 * Constrained-encoding = Extension-Media | Short-integer
 * Extension-media = *TEXT End-of-string
 * Content-general-form = Value-length Media-type
 * Media-type = (Well-known-media | Extension-Media) *(Parameter)
 * Well-known-media = Integer-value
 * Parameter = Typed-parameter | Untyped-parameter
 * @param encodeBuffer
 * @return true
 * @return false
 */
bool MmsContentType::EncodeMmsBodyPartContentType(MmsEncodeBuffer &encodeBuffer)
{
    MmsEncodeBuffer tmpEncodeBuffer;
    int8_t u8ContentType = 0;
    u8ContentType = GetContentTypeFromString(contentType_);
    if (u8ContentType < 0) {
        if (!tmpEncodeBuffer.EncodeText(contentType_)) {
            TELEPHONY_LOGE("Encode contentType EncodeText fail.");
            return false;
        }
    } else {
        if (!tmpEncodeBuffer.WriteByte(static_cast<uint8_t>(u8ContentType) | 0x80)) {
            TELEPHONY_LOGE("Encode contentType WriteByte fail.");
            return false;
        }
    }
    if (!EncodeMmsBodyPartContentParam(tmpEncodeBuffer)) {
        TELEPHONY_LOGE("Encode contentType EncodeMmsBodyPartContentParam fail.");
        return false;
    }
    if (!encodeBuffer.EncodeValueLength(tmpEncodeBuffer.GetCurPosition())) {
        TELEPHONY_LOGE("Encode contentType EncodeValueLength fail.");
        return false;
    }
    if (!encodeBuffer.WriteBuffer(tmpEncodeBuffer)) {
        TELEPHONY_LOGE("Encode contentType WriteBuffer fail.");
        return false;
    }
    return true;
}

MmsContentParam& MmsContentType::GetContentParam()
{
    return msgContentParm_;
}
} // namespace Telephony
} // namespace OHOS
