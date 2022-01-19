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
#include "mms_body.h"

#include "mms_decode_buffer.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
using namespace std;
MmsBody::MmsBody() : bodyEntrys_(0) {}

void MmsBody::DumpMmsBody()
{
    for (auto bodyPart : mmsBodyParts_) {
        bodyPart.DumpMmsBodyPart();
    }
}

/**
 * @brief DecodeMultipart
 * wap-230-wsp-20010705-a.pdf   section:8.5 Multipart Data
 * The application/vnd.wap.multipart content type consists of a header followed by 0 or more entries.
 * @param decodeBuffer
 * @return true
 * @return false
 */
bool MmsBody::DecodeMultipart(MmsDecodeBuffer &decodeBuffer)
{
    uint32_t nEntries = 0;
    uint32_t length = 0;
    if (!decodeBuffer.DecodeUintvar(nEntries, length)) {
        TELEPHONY_LOGE("Body decode multipart uintvar fail.");
        return false;
    }
    bodyEntrys_ = nEntries;
    while (nEntries) {
        MmsBodyPart bodyPart;
        if (!bodyPart.DecodePart(decodeBuffer)) {
            TELEPHONY_LOGE("Body decode multipart DecodePart fail.");
            return false;
        }
        mmsBodyParts_.push_back(bodyPart);
        nEntries--;
    }
    return true;
}

bool MmsBody::DecodeMmsBody(MmsDecodeBuffer &decodeBuffer, MmsHeader &header)
{
    unsigned char msgType = 0;
    if (!header.GetOctetValue(MMS_MESSAGE_TYPE, msgType)) {
        TELEPHONY_LOGE("Body decode GetOctetValue fail.");
        return false;
    }

    if (msgType != MMS_MSGTYPE_SEND_REQ && msgType != MMS_MSGTYPE_RETRIEVE_CONF) {
        // no body
        TELEPHONY_LOGI("this mms message type no body");
        return true;
    }
    std::string contentType = "";
    header.GetHeaderContentType().GetContentType(contentType);
    switch (static_cast<ContentTypes>(MmsContentType::GetContentTypeFromString(contentType))) {
        case ContentTypes::APPLICATION_VND_WAP_MULTIPART_MIXED:
        case ContentTypes::APPLICATION_VND_WAP_MULTIPART_RELATED:
        case ContentTypes::APPLICATION_VND_WAP_MULTIPART_ALTERNATIVE:
        case ContentTypes::MULTIPART_MIXED:
        case ContentTypes::MULTIPART_ALTERNATIVE:
        case ContentTypes::APPLICATION_VND_OMA_DRM_MESSAGE:
        case ContentTypes::APPLICATION_VND_OMA_DRM_CONTENT:
            return DecodeMultipart(decodeBuffer);
        default:
            break;
    }
    return true;
}

bool MmsBody::EncodeMmsBody(MmsEncodeBuffer &encodeBuffer)
{
    uint32_t partsNum = GetBodyPartCount();
    if (!encodeBuffer.EncodeUintvar(partsNum)) {
        TELEPHONY_LOGE("Body encode uintvar fail.");
        return false;
    }
    for (uint32_t i = 0; i < partsNum; i++) {
        if (!mmsBodyParts_.at(i).EncodeMmsBodyPart(encodeBuffer)) {
            TELEPHONY_LOGE("Body encode EncodeMmsBodyPart fail.");
            return false;
        }
    }
    return true;
}

/**
 * @brief EncodeMmsHeaderContentType
 * wap-230-wsp-20010705-a   section:8.4.2.24 Content type field section:8.4.2.1 Basic rules
 * Content-type-value = Constrained-media | Content-general-form
 * Constrained-media = Constrained-encoding
 * Constrained-encoding = Extension-Media | Short-integer
 * Extension-media = *TEXT End-of-string
 * Content-general-form = Value-length Media-type
 * Media-type = (Well-known-media | Extension-Media) *(Parameter)
 * Well-known-media = Integer-value
 * Parameter = Typed-parameter | Untyped-parameter
 * @param mmsHeader
 * @param encodeBuffer
 * @return true
 * @return false
 */
bool MmsBody::EncodeMmsHeaderContentType(MmsHeader &mmsHeader, MmsEncodeBuffer &encodeBuffer)
{
    if (!encodeBuffer.WriteByte((int8_t)(MMS_CONTENT_TYPE))) {
        TELEPHONY_LOGE("Body encode WriteByte fail.");
        return false;
    }
    std::string strContentType = "";
    mmsHeader.GetHeaderContentType().GetContentType(strContentType);
    if (!strContentType.length()) {
        TELEPHONY_LOGE("Body encode ContentType length is zero.");
        return false;
    }
    if (!mmsHeader.IsHaveBody() || GetBodyPartCount() == 0) {
        if (!encodeBuffer.EncodeText(strContentType)) {
            TELEPHONY_LOGE("Body encode ContentType EncodeText fail.");
            return false;
        }
        return true;
    }

    MmsEncodeBuffer tmpEncodeBuffer;
    if (!tmpEncodeBuffer.EncodeText(strContentType)) {
        TELEPHONY_LOGE("Body encode ContentType EncodeText fail.");
        return false;
    }
    // parameter
    mmsHeader.GetHeaderContentType().EncodeTextField(tmpEncodeBuffer);
    mmsHeader.GetHeaderContentType().EncodeTypeField(tmpEncodeBuffer);
    mmsHeader.GetHeaderContentType().EncodeCharsetField(tmpEncodeBuffer);

    if (!encodeBuffer.EncodeValueLength(tmpEncodeBuffer.GetCurPosition())) {
        TELEPHONY_LOGE("Body encode ContentType EncodeValueLength fail.");
        return false;
    }
    if (!encodeBuffer.WriteBuffer(tmpEncodeBuffer)) {
        TELEPHONY_LOGE("Body encode ContentType WriteBuffer fail.");
        return false;
    }
    return true;
}

uint32_t MmsBody::GetBodyPartCount()
{
    return mmsBodyParts_.size();
}

bool MmsBody::IsContentLocationPartExist(std::string contentLocation)
{
    for (auto item : mmsBodyParts_) {
        std::string strlocation = "";
        item.GetPartHeader().GetContentLocation(strlocation);
        if (contentLocation == strlocation) {
            return true;
        }
    }
    return false;
}

bool MmsBody::IsContentIdPartExist(std::string contentId)
{
    for (auto item : mmsBodyParts_) {
        std::string strId = "";
        item.GetPartHeader().GetContentId(strId);
        if (contentId == strId) {
            return true;
        }
    }
    return false;
}

bool MmsBody::IsBodyPartExist(MmsBodyPart &bodyPart)
{
    std::string contentId;
    bodyPart.GetContentId(contentId);
    if (IsContentIdPartExist(contentId)) {
        return true;
    }

    std::string contentLocation;
    bodyPart.GetContentLocation(contentLocation);
    if (IsContentLocationPartExist(contentLocation)) {
        return true;
    }
    return false;
}

void MmsBody::GetMmsBodyPart(std::vector<MmsBodyPart> &parts)
{
    parts.clear();
    parts.assign(mmsBodyParts_.begin(), mmsBodyParts_.end());
}

bool MmsBody::AddMmsBodyPart(MmsBodyPart &bodyPart)
{
    if (bodyPart.IsSmilFile() && bHaveSmilPart_) {
        TELEPHONY_LOGE("Not allowed to add more than two SmilPart File error.");
        return false;
    }

    if (bodyPart.IsSmilFile()) {
        mmsBodyParts_.insert(mmsBodyParts_.begin(), bodyPart);
        bHaveSmilPart_ = true;
    } else {
        mmsBodyParts_.push_back(bodyPart);
    }
    return true;
}
} // namespace Telephony
} // namespace OHOS
