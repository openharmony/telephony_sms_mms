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

#ifndef MMS_ATTACHMENT_H
#define MMS_ATTACHMENT_H

#include <cstring>
#include <string>

namespace OHOS {
namespace Telephony {
class MmsAttachment {
public:
    MmsAttachment() = default;
    ~MmsAttachment();
    MmsAttachment(const MmsAttachment &srcAttachment);

    /**
     * @brief Set the Attachment File Path Name
     *
     * @param strPath
     * @return true
     * @return false
     */
    bool SetAttachmentFilePath(std::string strPath, bool isSmil = false);

    /**
     * @brief Get the Attachment File Path Name
     *
     * @return std::string
     */
    std::string GetAttachmentFilePath();

    /**
     * @brief Set the Attachment Content Id
     *
     * @param contentId
     * @return true
     * @return false
     */
    bool SetContentId(std::string contentId);

    /**
     * @brief Get the Attachment Content Id
     *
     * @return std::string
     */
    std::string GetContentId();

    /**
     * @brief Set the Attachment Content Location
     *
     * @param contentLocation
     * @return true
     * @return false
     */
    bool SetContentLocation(std::string contentLocation);

    /**
     * @brief Get the Attachment Content Location
     *
     * @return std::string
     */
    std::string GetContentLocation();

    /**
     * @brief Set the Attachment Content Disposition
     *
     * @param contentDisposition
     * @return true
     * @return false
     */
    bool SetContentDisposition(std::string contentDisposition);

    /**
     * @brief Get Attachment the Content Disposition
     *
     * @return std::string
     */
    std::string GetContentDisposition();

    /**
     * @brief Set the Attachment Content Transfer Encoding
     *
     * @param contentTransferEncoding
     * @return true
     * @return false
     */
    bool SetContentTransferEncoding(std::string contentTransferEncoding);

    /**
     * @brief Get the Attachment Content Transfer Encoding
     *
     * @return std::string
     */
    std::string GetContentTransferEncoding();

    /**
     * @brief Set the Attachment Content Type
     *
     * @param strContentType
     * @return true
     * @return false
     */
    bool SetContentType(std::string strContentType);

    /**
     * @brief Get the Attachment Content Type
     *
     * @return std::string
     */
    std::string GetContentType();

    /**
     * @brief Set the File Name object
     *
     * @param strFileName
     * @return true
     * @return false
     */
    bool SetFileName(std::string strFileName);

    /**
     * @brief Get the File Name object
     *
     * @return std::string
     */
    std::string GetFileName();

    /* @brief Get the Attachment ContentType CharSet
     *
     * @return uint32_t
     */
    uint32_t GetCharSet();
    /**
     * @brief Set the Attachment ContentType CharSet
     *
     * @return void
     */
    void SetCharSet(uint32_t charset);

    /**
     * @brief Check the Attachment Is Smil File
     *
     * @return true
     * @return false
     */
    bool IsSmilFile();

    /**
     * @brief Set the Attachment Is Smil File
     *
     * @param isSmilFile
     */
    void SetIsSmilFile(bool isSmilFile);

    /**
     * @brief Get the Data Buffer The Attachment
     *
     * @param len out put buffer len
     * @return std::unique_ptr<char[]>
     */
    std::unique_ptr<char[]> GetDataBuffer(uint32_t &len);

    /**
     * @brief Set the Data Buffer To Attachment
     *
     * @param inBuff data input buffer pointer
     * @param len data input len
     * @return true
     * @return false
     */
    bool SetDataBuffer(std::unique_ptr<char[]> inBuff, uint32_t len);

private:
    bool isSmilFile_ = false;
    std::string strPathName_;
    std::string contentId_;
    std::string contentLocation_;
    std::string contentDispositon_;
    std::string contenTransferEncoding_;
    std::string contentType_;
    std::string strFileName_;
    uint32_t charset_ = 0;
    uint32_t dataLength_ = 0;
    std::unique_ptr<char[]> pAttachmentBuffer_ = nullptr;
    static constexpr uint32_t MAX_MMS_ATTACHMENT_LEN = 300 * 1024;
};
} // namespace Telephony
} // namespace OHOS
#endif
