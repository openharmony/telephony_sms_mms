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
#include "mms_msg.h"

#include "mms_content_type.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
void MmsMsg::DumpMms()
{
    mmsHeader_.DumpMmsHeader();
    mmsBody_.DumpMmsBody();
}

bool MmsMsg::DecodeMsg(std::string mmsFilePathName)
{
    MmsDecodeBuffer decodeBuffer;
    if (!decodeBuffer.WriteBufferFromFile(mmsFilePathName)) {
        TELEPHONY_LOGE("Read File To Buffer Error.");
        return false;
    }
    if (!mmsHeader_.DecodeMmsHeader(decodeBuffer)) {
        TELEPHONY_LOGE("Decode Mms Header Error.");
        return false;
    }
    if (!mmsBody_.DecodeMmsBody(decodeBuffer, mmsHeader_)) {
        TELEPHONY_LOGE("Decode Mms Body Error.");
        return false;
    }
    return true;
}

bool MmsMsg::DecodeMsg(std::unique_ptr<char[]> inBuff, uint32_t inLen)
{
    MmsDecodeBuffer decodeBuffer;
    if (!decodeBuffer.WriteDataBuffer(std::move(inBuff), inLen)) {
        TELEPHONY_LOGE("Write To Buffer Error.");
        return false;
    }
    if (!mmsHeader_.DecodeMmsHeader(decodeBuffer)) {
        TELEPHONY_LOGE("Decode Mms Header Error.");
        return false;
    }
    if (!mmsBody_.DecodeMmsBody(decodeBuffer, mmsHeader_)) {
        TELEPHONY_LOGE("Decode Mms Body Error.");
        return false;
    }
    return true;
}

std::unique_ptr<char[]> MmsMsg::EncodeMsg(uint32_t &outLen)
{
    MmsEncodeBuffer encodeBuffer;
    if (!mmsHeader_.EncodeMmsHeader(encodeBuffer)) {
        TELEPHONY_LOGE("Encode Mms Header Error.");
        return nullptr;
    }
    if (mmsHeader_.IsHaveBody()) {
        if (!mmsBody_.EncodeMmsHeaderContentType(mmsHeader_, encodeBuffer)) {
            TELEPHONY_LOGE("Encode Mms Header ContentType Error.");
            return nullptr;
        }
        if (!mmsBody_.EncodeMmsBody(encodeBuffer)) {
            TELEPHONY_LOGE("Encode Mms Body Error.");
            return nullptr;
        }
    }
    outLen = encodeBuffer.GetCurPosition();
    return encodeBuffer.ReadDataBuffer(0, outLen);
}

uint16_t MmsMsg::GetMmsVersion()
{
    return GetHeaderLongValue(MMS_MMS_VERSION);
}

bool MmsMsg::SetMmsVersion(uint16_t value)
{
    return SetHeaderLongValue(MMS_MMS_VERSION, value);
}

uint8_t MmsMsg::GetMmsMessageType()
{
    return GetHeaderOctetValue(MMS_MESSAGE_TYPE);
}

bool MmsMsg::SetMmsMessageType(uint8_t value)
{
    return mmsHeader_.SetOctetValue(MMS_MESSAGE_TYPE, value);
}

std::string MmsMsg::GetMmsTransactionId()
{
    return GetHeaderStringValue(MMS_TRANSACTION_ID);
}

bool MmsMsg::SetMmsTransactionId(std::string value)
{
    return mmsHeader_.SetTextValue(MMS_TRANSACTION_ID, value);
}

int64_t MmsMsg::GetMmsDate()
{
    int64_t date = 0;
    mmsHeader_.GetLongValue(MMS_DATE, date);
    return date;
}

bool MmsMsg::SetMmsDate(int64_t value)
{
    return mmsHeader_.SetLongValue(MMS_DATE, value);
}

std::string MmsMsg::GetMmsSubject()
{
    std::string value;
    mmsHeader_.GetStringValue(MMS_SUBJECT, value);
    return value;
}

bool MmsMsg::SetMmsSubject(std::string value)
{
    return mmsHeader_.SetEncodeStringValue(MMS_SUBJECT, 0x6A, value);
}

MmsAddress MmsMsg::GetMmsFrom()
{
    std::vector<MmsAddress> address;
    GetHeaderAllAddressValue(MMS_FROM, address);
    MmsAddress fromAddress;
    if (address.size() == 1) {
        fromAddress = address[0];
    }
    return fromAddress;
}

bool MmsMsg::SetMmsFrom(MmsAddress address)
{
    return AddHeaderAddressValue(MMS_FROM, address);
}

bool MmsMsg::SetMmsTo(std::vector<MmsAddress> &toAddrs)
{
    bool ret = false;
    for (auto it : toAddrs) {
        ret = AddHeaderAddressValue(MMS_TO, it);
        if (!ret) {
            return ret;
        }
    }
    return ret;
}

bool MmsMsg::GetMmsTo(std::vector<MmsAddress> &toAddrs)
{
    return GetHeaderAllAddressValue(MMS_TO, toAddrs);
}

uint8_t MmsMsg::GetHeaderOctetValue(uint8_t fieldId)
{
    uint8_t value = 0;
    mmsHeader_.GetOctetValue(fieldId, value);
    return value;
}

bool MmsMsg::SetHeaderOctetValue(uint8_t fieldId, uint8_t value)
{
    return mmsHeader_.SetOctetValue(fieldId, value);
}

int32_t MmsMsg::GetHeaderIntegerValue(uint8_t fieldId)
{
    int64_t value = 0;
    mmsHeader_.GetLongValue(fieldId, value);
    return static_cast<int32_t>(value);
}

bool MmsMsg::SetHeaderIntegerValue(uint8_t fieldId, int32_t value)
{
    return mmsHeader_.SetLongValue(fieldId, static_cast<int64_t>(value));
}

int64_t MmsMsg::GetHeaderLongValue(uint8_t fieldId)
{
    int64_t value = 0;
    mmsHeader_.GetLongValue(fieldId, value);
    return value;
}

bool MmsMsg::SetHeaderLongValue(uint8_t fieldId, int64_t value)
{
    return mmsHeader_.SetLongValue(fieldId, value);
}

std::string MmsMsg::GetHeaderStringValue(uint8_t fieldId)
{
    std::string value;
    if (mmsHeader_.GetTextValue(fieldId, value)) {
        return value;
    }

    value.clear();
    MmsEncodeString eValue;
    if (!mmsHeader_.GetEncodeStringValue(fieldId, eValue)) {
        return value;
    }
    eValue.GetEncodeString(value);
    return value;
}

// this fun must combit tow type string
bool MmsMsg::SetHeaderStringValue(uint8_t fieldId, std::string &value)
{
    return mmsHeader_.SetTextValue(fieldId, value);
}

bool MmsMsg::SetHeaderEncodedStringValue(uint8_t fieldId, const std::string &value, uint32_t charset)
{
    return mmsHeader_.SetEncodeStringValue(fieldId, charset, value);
}

bool MmsMsg::AddHeaderAddressValue(uint8_t fieldId, MmsAddress &address)
{
    return mmsHeader_.AddHeaderAddressValue(fieldId, address);
}

bool MmsMsg::GetHeaderAllAddressValue(uint8_t fieldId, std::vector<MmsAddress> &addressValue)
{
    return mmsHeader_.GetHeaderAllAddressValue(fieldId, addressValue);
}

bool MmsMsg::SetHeaderContentType(std::string strType)
{
    return mmsHeader_.GetHeaderContentType().SetContentType(strType);
}

std::string MmsMsg::GetHeaderContentType()
{
    std::string contentType = "";
    mmsHeader_.GetHeaderContentType().GetContentType(contentType);
    return contentType;
}

std::string MmsMsg::GetHeaderContentTypeStart()
{
    std::string start = "";
    uint8_t fieldCode = static_cast<uint8_t>(ContentParam::CT_P_START);
    mmsHeader_.GetHeaderContentType().GetContentParam().GetNormalField(fieldCode, start);
    if (start.empty()) {
        fieldCode = static_cast<uint8_t>(ContentParam::CT_P_START_VALUE);
        mmsHeader_.GetHeaderContentType().GetContentParam().GetNormalField(fieldCode, start);
        return start;
    }
    return start;
}

bool MmsMsg::AddAttachment(MmsAttachment &attachment)
{
    MmsBodyPart bodyPart;
    if (!bodyPart.SetAttachment(attachment)) {
        TELEPHONY_LOGE("BodyPart Set AttachmentPath Error!");
        return false;
    }
    /** Check Body Part for duplication **/
    if (mmsBody_.IsBodyPartExist(bodyPart)) {
        TELEPHONY_LOGE("MmsBody BodyPart Has Exist Error!");
        return false;
    }
    /** Add Body Part To Mms Body **/
    if (!mmsBody_.AddMmsBodyPart(bodyPart)) {
        TELEPHONY_LOGE("MmsBody Add BodyPart error!");
        return false;
    }
    /** Check Body Part is Smil Part **/
    if (bodyPart.IsSmilFile()) {
        std::string contentId;
        std::string contentType;
        bodyPart.GetContentId(contentId);
        bodyPart.GetContentType(contentType);

        mmsHeader_.GetHeaderContentType().GetContentParam().SetStart(contentId);
        mmsHeader_.GetHeaderContentType().GetContentParam().SetType(contentType);
    }
    return true;
}

bool MmsMsg::GetAllAttachment(std::vector<MmsAttachment> &attachments)
{
    attachments.clear();
    if (mmsBody_.GetBodyPartCount() <= 0) {
        TELEPHONY_LOGE("MmsBody Not Has BodyPart Error.");
        return false;
    }
    std::string smilFileName;
    std::vector<MmsBodyPart> bodyParts;
    mmsHeader_.GetSmilFileName(smilFileName);
    mmsBody_.GetMmsBodyPart(bodyParts);

    for (auto part : bodyParts) {
        MmsAttachment attachment;
        std::string contentId = "";
        part.GetPartHeader().GetContentId(contentId);
        if (!contentId.empty()) {
            attachment.SetContentId(contentId);
            bool isSmil = (smilFileName == contentId) ? true : false;
            attachment.SetIsSmilFile(isSmil);
        }
        std::string fileName = "";
        fileName = part.GetPartFileName();
        if (fileName.empty()) {
            TELEPHONY_LOGE("Body Part File Name Empty Error.");
            return false;
        }
        attachment.SetFileName(fileName);

        std::string contentType = "";
        part.GetContentType().GetContentType(contentType);
        attachment.SetContentType(contentType);
        attachment.SetCharSet(part.GetContentType().GetContentParam().GetCharSet());

        std::string contentLocation = "";
        part.GetPartHeader().GetContentLocation(contentLocation);
        attachment.SetContentLocation(contentLocation);

        std::string contentDispositon = "";
        part.GetPartHeader().GetContentDisposition(contentDispositon);
        attachment.SetContentDisposition(contentDispositon);

        std::string strContentTransferEncoding = "";
        part.GetPartHeader().GetContentTransferEncoding(strContentTransferEncoding);
        attachment.SetContentTransferEncoding(strContentTransferEncoding);

        uint32_t partBodyLen = 0;
        std::unique_ptr<char[]> partBodyBuffer = part.ReadBodyPartBuffer(partBodyLen);
        attachment.SetDataBuffer(std::move(partBodyBuffer), partBodyLen);
        attachments.push_back(attachment);
    }
    return true;
}

MmsHeader& MmsMsg::GetHeader()
{
    return mmsHeader_;
}
} // namespace Telephony
} // namespace OHOS
