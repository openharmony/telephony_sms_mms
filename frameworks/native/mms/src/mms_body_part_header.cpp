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

#include "mms_body_part_header.h"

#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
MmsBodyPartHeader &MmsBodyPartHeader::operator=(const MmsBodyPartHeader &srcHeader)
{
    if (this != &srcHeader) {
        strContentTransferEncoding_ = srcHeader.strContentTransferEncoding_;
        strFileName_ = srcHeader.strFileName_;
        strContentLocation_ = srcHeader.strContentLocation_;
        strContentID_ = srcHeader.strContentID_;
        strDisposition_ = srcHeader.strDisposition_;
        textMap_ = srcHeader.textMap_;
    }
    return *this;
}

MmsBodyPartHeader::MmsBodyPartHeader(const MmsBodyPartHeader &obj)
{
    *this = obj;
}

void MmsBodyPartHeader::DumpBodyPartHeader() {}

/**
 * @brief DecodeContentLocation
 * OMA-TS-MMS_ENC-V1_3-20110913-A   section:7.3.10 X-Mms-Content-Location Field
 * When used in a PDU other than M-Mbox-Delete.conf and M-Delete.conf:
 * Content-location-value = Uri-value
 * When used in the M-Mbox-Delete.conf and M-Delete.conf PDU:
 * Content-location-Del-value = Value-length Status-count-value Content-location-value
 * Status-count-value = Integer-value
 * Uri-value = Text-string
 * @param decodeBuffer
 * @param Len
 * @return true
 * @return false
 */
bool MmsBodyPartHeader::DecodeContentLocation(MmsDecodeBuffer &decodeBuffer, uint32_t &Len)
{
    std::string sTmp = "";
    Len = 0;
    if (!decodeBuffer.DecodeText(sTmp, Len)) {
        TELEPHONY_LOGE("Body part header decode text fail.");
        return false;
    }
    strContentLocation_ = sTmp;
    return true;
}

/**
 * @brief DecodeContentId
 * wap-230-wsp-20010705-a   section:8.4.2.67 Content-ID field
 * Content-ID-value = Quoted-string
 * @param decodeBuffer
 * @param Len
 * @return true
 * @return false
 */
bool MmsBodyPartHeader::DecodeContentId(MmsDecodeBuffer &decodeBuffer, uint32_t &Len)
{
    std::string sTmp = "";
    Len = 0;
    if (!decodeBuffer.DecodeQuotedText(sTmp, Len)) {
        TELEPHONY_LOGE("Body part header decode quoted text fail.");
        return false;
    }
    strContentID_ = sTmp;
    return true;
}

/**
 * @brief DecodeContentDisposition
 * wap-230-wsp-20010705-a   section:8.4.2.53 Content-disposition field
 * Content-disposition-value = Value-length Disposition *(Parameter)
 * Disposition = Form-data | Attachment | Inline | Token-text
 * Form-data = <Octet 128>
 * Attachment = <Octet 129>
 * Inline = <Octet 130>
 * @param decodeBuffer
 * @param Len
 * @return true
 * @return false
 */
bool MmsBodyPartHeader::DecodeContentDisposition(MmsDecodeBuffer &decodeBuffer, uint32_t &Len)
{
    uint32_t dispositionLength = 0;
    uint32_t count = 0;
    if (!decodeBuffer.DecodeUintvar(dispositionLength, count)) {
        TELEPHONY_LOGE("Body part header decode uintvar fail.");
        return false;
    }

    uint32_t beginPostion = decodeBuffer.GetCurPosition();
    uint8_t oneByte = 0;
    if (!decodeBuffer.GetOneByte(oneByte)) {
        TELEPHONY_LOGE("Body part header decode get one byte fail.");
        return false;
    }

    switch (static_cast<MmsDispositonParam>(oneByte)) {
        case MmsDispositonParam::P_DISPOSITION_FROM_DATA: {
            strDisposition_ = DISPOSITION_FROM_DATA;
            break;
        }
        case MmsDispositonParam::P_DISPOSITION_ATTACHMENT: {
            strDisposition_ = DISPOSITION_ATTACHMENT;
            break;
        }
        case MmsDispositonParam::P_DISPOSITION_INLINE: {
            strDisposition_ = DISPOSITION_INLINE;
            break;
        }
        default: {
            if (!decodeBuffer.DecreasePointer(1)) {
                TELEPHONY_LOGE("Body part header move pointer fail.");
                return false;
            }
            std::string strTmp = "";
            uint32_t tmpLen = 0;
            if (!decodeBuffer.DecodeText(strTmp, tmpLen)) {
                TELEPHONY_LOGE("Body part header decode text fail.");
                return false;
            }
            strDisposition_ = strTmp;
            break;
        }
    }
    TELEPHONY_LOGI("strDisposition_ == %{public}s", strDisposition_.c_str());
    if (!DecodeDispositionParameter(decodeBuffer, dispositionLength, beginPostion)) {
        TELEPHONY_LOGE("Decode Disposition Parameter error.");
        return false;
    }
    Len = dispositionLength + count + 1;
    return true;
}

/**
 * @brief DecodeContentDispositionParameter
 * wap-230-wsp-20010705-a   section:8.4.2.53 Content-disposition field
 * Content-disposition-value = Value-length Disposition *(Parameter)
 * Disposition = Form-data | Attachment | Inline | Token-text
 * Form-data = <Octet 128>
 * Attachment = <Octet 129>
 * Inline = <Octet 130>
 * @param decodeBuffer
 * @param dispLen
 * @param beginPos
 * @return true
 * @return false
 */
bool MmsBodyPartHeader::DecodeDispositionParameter(
    MmsDecodeBuffer &decodeBuffer, uint32_t dispLen, uint32_t beginPos)
{
    const uint8_t pFileNameValue = 0x98;

    uint32_t endPostion = decodeBuffer.GetCurPosition();
    uint8_t oneByte = 0;
    if (dispLen > (endPostion - beginPos)) {
        if (!decodeBuffer.GetOneByte(oneByte)) {
            TELEPHONY_LOGE("Body part header decode get one byte fail.");
            return false;
        }
        if (oneByte == pFileNameValue) {
            std::string strTmp = "";
            uint32_t tmpLen = 0;
            if (!decodeBuffer.DecodeText(strTmp, tmpLen)) {
                TELEPHONY_LOGE("Body part header decode text fail.");
                return false;
            }
            strFileName_ = strTmp;
        }
        endPostion = decodeBuffer.GetCurPosition();
        if (dispLen < (endPostion - beginPos)) {
            TELEPHONY_LOGE("Body part header decode content disposition length err.");
            return false;
        }
        if (!decodeBuffer.IncreasePointer(dispLen - (endPostion - beginPos))) {
            TELEPHONY_LOGE("Body part header decode content disposition move pointer err.");
            return false;
        }
    }
    return true;
}

/**
 * @brief DecodeWellKnownHeader
 * wap-230-wsp-20010705-a   section:8.4.2.6 Header
 * Well-known-header = Well-known-field-name Wap-value
 * Well-known-field-name = Short-integer
 * Wap-value = Accept-value | Accept-charset-value |...
 * @param decodeBuffer
 * @param headerLen
 * @return true
 * @return false
 */
bool MmsBodyPartHeader::DecodeWellKnownHeader(MmsDecodeBuffer &decodeBuffer, uint32_t &headerLen)
{
    const uint8_t endStringZeroLen = 1;
    uint8_t fieldCode = 0xff;
    uint8_t oneByte = 0;
    if (!decodeBuffer.GetOneByte(oneByte)) {
        TELEPHONY_LOGE("Body part header decode get one byte fail.");
        return false;
    }
    headerLen--;
    uint32_t len = 0;
    fieldCode = oneByte & 0x7f;
    switch (static_cast<MmsHeaderParam>(fieldCode)) {
        case MmsHeaderParam::P_CONTENT_LOCATION_V1: /* Content-Location */
        case MmsHeaderParam::P_CONTENT_LOCATION_V2: { /* Content-Location */
            if (!DecodeContentLocation(decodeBuffer, len)) {
                TELEPHONY_LOGE("Body part header decode content location fail.");
                return false;
            }
            break;
        }
        case MmsHeaderParam::P_CONTENT_ID: { /* Content-ID */
            if (!DecodeContentId(decodeBuffer, len)) {
                TELEPHONY_LOGE("Body part header decode contentId fail.");
                return false;
            }
            break;
        }
        case MmsHeaderParam::P_CONTENT_DISPOSITION_V1: /* Content-Disposition */
        case MmsHeaderParam::P_CONTENT_DISPOSITION_V2: {
            if (!DecodeContentDisposition(decodeBuffer, len)) {
                TELEPHONY_LOGE("Body part header decode content disposition fail.");
                return false;
            }
            break;
        }
        default: {
            std::string sTmp = "";
            decodeBuffer.DecodeQuotedText(sTmp, len);
            break;
        }
    }
    if (headerLen > len + endStringZeroLen) {
        headerLen -= len + endStringZeroLen;
    } else {
        headerLen = 0;
    }
    return true;
}

/**
 * @brief DecodeApplicationHeader
 * wap-230-wsp-20010705-a   section:8.4.2.6 Header
 * Application-header		  = Token-text Application-specific-value
 * Application-specific-value = Text-string
 * @param decodeBuffer
 * @param headerLen
 * @return true
 * @return false
 */
bool MmsBodyPartHeader::DecodeApplicationHeader(MmsDecodeBuffer &decodeBuffer, uint32_t &headerLen)
{
    const uint32_t endStringZeroLen = 2;
    headerLen = 0;
    std::string sField = "";
    uint32_t fieldLen = 0;
    if (!decodeBuffer.DecodeTokenText(sField, fieldLen)) {
        TELEPHONY_LOGE("Body part header decode token text fail.");
        return false;
    }

    std::string sValue = "";
    uint32_t valueLen = 0;
    if (!decodeBuffer.DecodeText(sValue, valueLen)) {
        TELEPHONY_LOGE("Body part header decode text fail.");
        return false;
    }
    if (sField == "Content-Transfer-Encoding") {
        strContentTransferEncoding_ = sValue;
    }
    if (headerLen > fieldLen + valueLen + endStringZeroLen) {
        headerLen -= fieldLen + valueLen + endStringZeroLen;
    } else {
        headerLen = 0;
    }
    return true;
}

bool MmsBodyPartHeader::GetContentId(std::string &contentId)
{
    contentId.clear();
    contentId = strContentID_;
    return true;
}

bool MmsBodyPartHeader::SetContentId(std::string contentId)
{
    strContentID_ = contentId;
    return true;
}

bool MmsBodyPartHeader::GetContentTransferEncoding(std::string &contentTransferEncoding)
{
    contentTransferEncoding.clear();
    contentTransferEncoding = strContentTransferEncoding_;
    return true;
}

bool MmsBodyPartHeader::SetContentTransferEncoding(std::string contentTransferEncoding)
{
    strContentTransferEncoding_ = contentTransferEncoding;
    return true;
}

bool MmsBodyPartHeader::GetContentLocation(std::string &contentLocation)
{
    contentLocation.clear();
    contentLocation.assign(strContentLocation_);
    return true;
}

bool MmsBodyPartHeader::SetContentLocation(std::string contentLocation)
{
    strContentLocation_ = contentLocation;
    return true;
}

bool MmsBodyPartHeader::GetContentDisposition(std::string &contentDisposition)
{
    contentDisposition.clear();
    contentDisposition.assign(strDisposition_);
    return true;
}

bool MmsBodyPartHeader::SetContentDisposition(std::string contentDisposition)
{
    strDisposition_ = contentDisposition;
    return true;
}

/**
 * @brief EncodeContentLocation
 * OMA-TS-MMS_ENC-V1_3-20110913-A   section:7.3.10 X-Mms-Content-Location Field
 * When used in a PDU other than M-Mbox-Delete.conf and M-Delete.conf:
 * Content-location-value = Uri-value
 * When used in the M-Mbox-Delete.conf and M-Delete.conf PDU:
 * Content-location-Del-value = Value-length Status-count-value Content-location-value
 * Status-count-value = Integer-value
 * Uri-value = Text-string
 * @param encodeBuffer
 * @return true
 * @return false
 */
bool MmsBodyPartHeader::EncodeContentLocation(MmsEncodeBuffer &encodeBuffer)
{
    const uint8_t setHighestBitOne = 0x80;
    if (strContentLocation_.empty()) {
        TELEPHONY_LOGI("Body part header encode content location is empty.");
        return true;
    }

    if (!encodeBuffer.WriteByte(static_cast<uint8_t>(MmsHeaderParam::P_CONTENT_LOCATION_V2) | setHighestBitOne)) {
        TELEPHONY_LOGE("Body part header encode content location write byte fail.");
        return false;
    }
    if (!encodeBuffer.EncodeText(strContentLocation_)) {
        TELEPHONY_LOGE("Body part header encode content location fail.");
        return false;
    }
    return true;
}

/**
 * @brief EncodeContentId
 * wap-230-wsp-20010705-a   section:8.4.2.67 Content-ID field
 * Content-ID-value = Quoted-string
 * @param encodeBuffer
 * @return true
 * @return false
 */
bool MmsBodyPartHeader::EncodeContentId(MmsEncodeBuffer &encodeBuffer)
{
    const uint8_t setHighestBitOne = 0x80;
    if (strContentID_.empty()) {
        TELEPHONY_LOGI("Body part header encode content ID is empty.");
        return true;
    }

    if (!encodeBuffer.WriteByte(static_cast<uint8_t>(MmsHeaderParam::P_CONTENT_ID) | setHighestBitOne)) {
        TELEPHONY_LOGE("Body part header encode content ID write byte fail.");
        return false;
    }
    if (!encodeBuffer.EncodeQuotedText(strContentID_)) {
        TELEPHONY_LOGE("Body part header encode content ID fail.");
        return false;
    }
    return true;
}

/**
 * @brief EncodeContentDisposition
 * wap-230-wsp-20010705-a   section:8.4.2.53 Content-disposition field
 * Content-disposition-value = Value-length Disposition *(Parameter)
 * Disposition = Form-data | Attachment | Inline | Token-text
 * Form-data = <Octet 128>
 * Attachment = <Octet 129>
 * Inline = <Octet 130>
 * @param encodeBuffer
 * @return true
 * @return false
 */
bool MmsBodyPartHeader::EncodeContentDisposition(MmsEncodeBuffer &encodeBuffer)
{
    const uint8_t setHighestBitOne = 0x80;
    std::vector<std::string> dispVec {DISPOSITION_FROM_DATA, DISPOSITION_ATTACHMENT, DISPOSITION_INLINE};
    auto it = std::find(dispVec.begin(), dispVec.end(), strDisposition_);
    if (it == dispVec.end()) {
        return true;
    }

    TELEPHONY_LOGI("strDisposition = %{public}s", strDisposition_.c_str());
    if (!encodeBuffer.WriteByte(static_cast<uint8_t>(MmsHeaderParam::P_CONTENT_DISPOSITION_V1) | setHighestBitOne)) {
        TELEPHONY_LOGE("Body part header encode content disposition write byte fail.");
        return false;
    }
    if (!encodeBuffer.EncodeUintvar(0x01)) {
        TELEPHONY_LOGE("EncodeContentDisposition EncodeUintvar Error.");
        return false;
    }
    if (strDisposition_ == DISPOSITION_FROM_DATA) {
        if (!encodeBuffer.WriteByte(static_cast<uint8_t>(MmsDispositonParam::P_DISPOSITION_FROM_DATA))) {
            TELEPHONY_LOGE("Body part header encode content disposition write byte fail.");
            return false;
        }
        return true;
    }
    if (strDisposition_ == DISPOSITION_ATTACHMENT) {
        if (!encodeBuffer.WriteByte(static_cast<uint8_t>(MmsDispositonParam::P_DISPOSITION_ATTACHMENT))) {
            TELEPHONY_LOGE("Body part header encode content disposition write byte fail.");
            return false;
        }
        return true;
    }
    if (strDisposition_ == DISPOSITION_INLINE) {
        if (!encodeBuffer.WriteByte(static_cast<uint8_t>(MmsDispositonParam::P_DISPOSITION_INLINE))) {
            TELEPHONY_LOGE("Body part header encode content disposition write byte fail.");
            return false;
        }
        return true;
    }
    return true;
}

/**
 * @brief EncodeContentDisposition
 * wap-230-wsp-20010705-a   section:8.4.2.46 Transfer encoding field
 * Transfer-encoding-values = Chunked | Token-text
 * Chunked = <Octet 128>
 * @param encodeBuffer
 * @return true
 * @return false
 */
bool MmsBodyPartHeader::EncodeContentTransferEncoding(MmsEncodeBuffer &encodeBuffer)
{
    if (strContentTransferEncoding_.empty()) {
        TELEPHONY_LOGI("Body part header encode content transfer encoding is empty.");
        return true;
    }

    if (!encodeBuffer.EncodeText("Content-Transfer-Encoding")) {
        TELEPHONY_LOGE("Body part header encode content transfer encoding encode text fail.");
        return false;
    }
    if (!encodeBuffer.EncodeText(strContentTransferEncoding_)) {
        TELEPHONY_LOGE("Body part header encode content transfer encoding encode text fail.");
        return false;
    }
    return true;
}

/**
 * @brief EncodeMmsBodyPartHeader
 * wap-230-wsp-20010705-a   section:8.4.2.6 Header
 * Well-known-header = Well-known-field-name Wap-value
 * Well-known-field-name = Short-integer
 * Wap-value = Accept-value | Accept-charset-value |...
 * @param encodeBuffer
 * @return true
 * @return false
 */
bool MmsBodyPartHeader::EncodeMmsBodyPartHeader(MmsEncodeBuffer &encodeBuffer)
{
    if (!EncodeContentId(encodeBuffer)) {
        TELEPHONY_LOGE("Body part header encode ContentId fail.");
        return false;
    }
    if (!EncodeContentLocation(encodeBuffer)) {
        TELEPHONY_LOGE("Body part header encode ContentLocation fail.");
        return false;
    }
    if (!EncodeContentDisposition(encodeBuffer)) {
        TELEPHONY_LOGE("Body part header encode ContentDisposition fail.");
        return false;
    }
    if (!EncodeContentTransferEncoding(encodeBuffer)) {
        TELEPHONY_LOGE("Body part header encode ContentTransferEncoding fail.");
        return false;
    }
    return true;
}
} // namespace Telephony
} // namespace OHOS
