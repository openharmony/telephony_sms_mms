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
#include "mms_attachment.h"

#include "securec.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
MmsAttachment::~MmsAttachment()
{
    if (pAttachmentBuffer_ != nullptr) {
        pAttachmentBuffer_.reset();
        dataLength_ = 0;
    }
}

bool MmsAttachment::SetAttachmentFilePath(std::string strPath, bool isSmil)
{
    if (strPath.empty()) {
        TELEPHONY_LOGE("Attachment file path is empty!");
        return false;
    }
    strPathName_ = strPath;
    isSmilFile_ = isSmil;
    return true;
}

MmsAttachment::MmsAttachment(const MmsAttachment &srcAttachment)
{
    pAttachmentBuffer_ = std::make_unique<char[]>(srcAttachment.dataLength_);
    if (pAttachmentBuffer_ == nullptr) {
        TELEPHONY_LOGE("make unique attachment buffer nullptr error.");
        return;
    }
    if (memcpy_s(pAttachmentBuffer_.get(), srcAttachment.dataLength_, srcAttachment.pAttachmentBuffer_.get(),
            srcAttachment.dataLength_) != EOK) {
        TELEPHONY_LOGE("memcpy_s attachment buffer error.");
        return;
    }

    dataLength_ = srcAttachment.dataLength_;
    contentId_ = srcAttachment.contentId_;
    contentLocation_ = srcAttachment.contentLocation_;
    contentDispositon_ = srcAttachment.contentDispositon_;
    contentType_ = srcAttachment.contentType_;
    contenTransferEncoding_ = srcAttachment.contenTransferEncoding_;
    strFileName_ = srcAttachment.strFileName_;
    isSmilFile_ = srcAttachment.isSmilFile_;
    charset_ = srcAttachment.charset_;
}

std::string MmsAttachment::GetAttachmentFilePath()
{
    return strPathName_;
}

bool MmsAttachment::SetContentId(std::string contentId)
{
    if (contentId.empty()) {
        TELEPHONY_LOGE("Attachment contentId is empty!");
        return false;
    }
    if (contentId.length() > 1 && contentId.at(0) == '<' && contentId.at(contentId.length() - 1)) {
        contentId_.assign(contentId);
        return true;
    }
    contentId = '<' + contentId + '>';
    contentId_.assign(contentId);
    return true;
}

std::string MmsAttachment::GetContentId()
{
    return contentId_;
}

bool MmsAttachment::SetContentLocation(std::string contentLocation)
{
    if (contentLocation.empty()) {
        TELEPHONY_LOGE("Attachment ContentLocation is empty!");
        return false;
    }
    contentLocation_.assign(contentLocation);
    return true;
}

std::string MmsAttachment::GetContentLocation()
{
    return contentLocation_;
}

bool MmsAttachment::SetContentDisposition(std::string contentDisposition)
{
    if (contentDisposition.empty()) {
        TELEPHONY_LOGE("Attachment contentDisposition is empty!");
        return false;
    }
    contentDispositon_.assign(contentDisposition);
    return true;
}

std::string MmsAttachment::GetContentDisposition()
{
    return contentDispositon_;
}

bool MmsAttachment::SetContentTransferEncoding(std::string contentTransferEncoding)
{
    if (contentTransferEncoding.empty()) {
        TELEPHONY_LOGE("Attachment ContentTransferEncoding is empty!");
        return false;
    }
    contenTransferEncoding_.assign(contentTransferEncoding);
    return true;
}

std::string MmsAttachment::GetContentTransferEncoding()
{
    return contenTransferEncoding_;
}

bool MmsAttachment::SetContentType(std::string strContentType)
{
    if (strContentType.empty()) {
        TELEPHONY_LOGE("Attachment ContentType is empty!");
        return false;
    }
    contentType_.assign(strContentType);
    return true;
}

std::string MmsAttachment::GetContentType()
{
    return contentType_;
}

void MmsAttachment::SetCharSet(uint32_t charset)
{
    charset_ = charset;
}

uint32_t MmsAttachment::GetCharSet()
{
    return charset_;
}

bool MmsAttachment::SetFileName(std::string strFileName)
{
    if (strFileName.empty()) {
        TELEPHONY_LOGE("Attachment file name is empty!");
        return false;
    }
    strFileName_.assign(strFileName);
    return true;
}

std::string MmsAttachment::GetFileName()
{
    if (strFileName_.length() > 0) {
        return strFileName_;
    }
    return "";
}

bool MmsAttachment::IsSmilFile()
{
    return isSmilFile_;
}

void MmsAttachment::SetIsSmilFile(bool isSmilFile)
{
    isSmilFile_ = isSmilFile;
}

std::unique_ptr<char[]> MmsAttachment::GetDataBuffer(uint32_t &len)
{
    len = dataLength_;
    std::unique_ptr<char[]> result = std::make_unique<char[]>(len);
    if (result == nullptr) {
        TELEPHONY_LOGE("make unique buffer result nullptr error.");
        return nullptr;
    }
    if (memcpy_s(result.get(), len, pAttachmentBuffer_.get(), len) != EOK) {
        TELEPHONY_LOGE("memcpy_s from attachment buffer error.");
        return nullptr;
    }
    return result;
}

bool MmsAttachment::SetDataBuffer(std::unique_ptr<char[]> inBuff, uint32_t len)
{
    if (inBuff == nullptr) {
        TELEPHONY_LOGE("input buffer pointer nullptr error.");
        return false;
    }

    if (len > MAX_MMS_ATTACHMENT_LEN) {
        TELEPHONY_LOGE("input buffer over max lenght error.");
        return false;
    }
    pAttachmentBuffer_ = std::make_unique<char[]>(len);
    if (pAttachmentBuffer_ == nullptr) {
        TELEPHONY_LOGE("make unique attachment buffer nullptr error.");
        return false;
    }
    if (memcpy_s(pAttachmentBuffer_.get(), len, inBuff.get(), len) != EOK) {
        TELEPHONY_LOGE("memcpy_s to attachment buffer error.");
        return false;
    }
    dataLength_ = len;
    return true;
}
} // namespace Telephony
} // namespace OHOS
