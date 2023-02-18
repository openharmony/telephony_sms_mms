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

#ifndef MMS_MSG_H
#define MMS_MSG_H

#include <vector>

#include "mms_header.h"
#include "mms_body.h"
#include "mms_body_part.h"
#include "mms_content_param.h"
#include "mms_decode_buffer.h"
#include "mms_attachment.h"
#include "mms_address.h"

namespace OHOS {
namespace Telephony {
class MmsMsg {
public:
    MmsMsg() = default;
    ~MmsMsg() = default;

    /**
     * @brief Dump the mms Message
     * Printf Mms Message Infomations
     */
    void DumpMms();

    /**
     * @brief Get the Mms Version
     * Get Mms Header Protocol Version
     * @return the Mms Version
     */
    uint16_t GetMmsVersion();

    /**
     * @brief Set the Mms Version
     * Set Mms Header Protocol Version
     * @param value
     * @return true
     * @return false
     */
    bool SetMmsVersion(uint16_t value);

    /**
     * @brief Get the Mms Message Type
     * Get Mms Header Message Type
     * @return uint8_t
     */
    uint8_t GetMmsMessageType();

    /**
     * @brief Set the Mms Message Type
     * Set Mms Header Message Type
     * @param value
     * @return true
     * @return false
     */
    bool SetMmsMessageType(uint8_t value);

    /**
     * @brief Get the Mms Transaction Id From Header
     * A unique identifier for the PDU.
     * @return std::string
     */
    std::string GetMmsTransactionId();

    /**
     * @brief Set the Mms Transaction Id To Header
     * A unique identifier for the PDU.
     * @param value
     * @return true
     * @return false
     */
    bool SetMmsTransactionId(std::string value);

    /**
     * @brief Get the Mms Date From Header
     * Get Mms Message Header Date Infomations
     * @return int64_t
     */
    int64_t GetMmsDate();

    /**
     * @brief Set the Mms Date To Header
     * Set Mms Message Header Date Infomations
     * @return true
     * @return false
     */
    bool SetMmsDate(int64_t value);

    /**
     * @brief Get the Mms Subject From Header
     * Get Mms Message Header SubJect Infomations
     * @return std::string
     */
    std::string GetMmsSubject();

    /**
     * @brief Set the Mms Subject To Header
     * Set Mms Message Header SubJect Infomations
     * @return std::string
     */
    bool SetMmsSubject(std::string value);

    /**
     * @brief Get the Mms From
     * Get Mms Message Header From Address Infomations
     * @return MmsAddress
     */
    MmsAddress GetMmsFrom();

    /**
     * @brief Set the Mms From To Header
     * Set Mms Message Header From Address Infomations
     * @param address
     * @return true
     * @return false
     */
    bool SetMmsFrom(MmsAddress address);

    /**
     * @brief Set the Mms To Address to Header
     * it can support multiple address to header
     * @param address
     * @return true
     * @return false
     */
    bool SetMmsTo(std::vector<MmsAddress>& toAddrs);

    /**
     * @brief Get the Mms Msg to address from the header
     * it can support multiple address to header
     * @param address
     * @return true
     * @return false
     */
    bool GetMmsTo(std::vector<MmsAddress>& toAddrs);

    /**
     * @brief Get Octet Type Value From the Header
     * This is Generic methods to Get Mms Message Header
     * Field Octet Value
     * @param fieldId
     * @return uint8_t
     */
    uint8_t GetHeaderOctetValue(uint8_t fieldId);

    /**
     * @brief Set the Header Octet Value To Header
     * This is Generic methods to Set Mms Message Header
     * Field Octet Value
     * @param fieldId
     * @param value
     * @return true
     * @return false
     */
    bool SetHeaderOctetValue(uint8_t fieldId, uint8_t value);

    /**
     * @brief Get the Header Integer Value From Header
     * This is Generic methods to Get Mms Message Header
     * Field Integer Value
     * @param fieldId
     * @return int32_t
     */
    int32_t GetHeaderIntegerValue(uint8_t fieldId);

    /**
     * @brief Set the Header Integer Value To Header
     * This is Generic methods to Set Mms Message Header
     * Field Integer Value
     * @param fieldId
     * @param value
     * @return true
     * @return false
     */
    bool SetHeaderIntegerValue(uint8_t fieldId, int32_t value);

    /**
     * @brief Get Long Type Value From the Header
     * This is Generic methods to Get Mms Message Header
     * Field Long Value
     * @param fieldId
     * @return int64_t
     */
    int64_t GetHeaderLongValue(uint8_t fieldId);

    /**
     * @brief Set the Header Long Value To Header
     * This is Generic methods to Set Mms Message Header
     * Field Long Value
     * @param fieldId
     * @param value
     * @return true
     * @return false
     */
    bool SetHeaderLongValue(uint8_t fieldId, int64_t value);

    /**
     * @brief Get String or Encode String Type Value From the Header
     * This is Generic methods to Get Mms Message Header
     * Field Text Value Or EncondeString with UTF_8
     * @param fieldId
     * @return std::string
     */
    std::string GetHeaderStringValue(uint8_t fieldId);

    /**
     * @brief Set the Header String Value To Header
     * This is Generic methods to Set Mms Message Header
     * Field Text Value
     * @param fieldId
     * @param value
     * @return true
     * @return false
     */
    bool SetHeaderStringValue(uint8_t fieldId, std::string &value);

    /**
     * @brief Set the Header Encoded String Value From Header
     * This is Generic methods to Set Mms Message Header
     * Field EncondeString Value
     * @param fieldId
     * @param value
     * @param charset
     * @return true
     * @return false
     */
    bool SetHeaderEncodedStringValue(uint8_t fieldId, const std::string &value, uint32_t charset);

    /**
     * @brief AddHeaderAddressValue
     * for example: To / Cc / Bcc
     * @param fieldId
     * @param address
     * @return true
     * @return false
     */
    bool AddHeaderAddressValue(uint8_t fieldId, MmsAddress &address);

    /**
     * @brief Get the Header All Address Value From Header
     * for example: To / Cc / Bcc
     * @param fieldId
     * @param addressValue
     * @return true
     * @return false
     */
    bool GetHeaderAllAddressValue(uint8_t fieldId, std::vector<MmsAddress> &addressValue);

    /**
     * @brief Set the Content Type
     *
     * @param strType
     * @return true
     * @return false
     */
    bool SetHeaderContentType(std::string strType);

    /**
     * @brief Get the Content Type
     *
     * @return std::string
     */
    std::string GetHeaderContentType();

    /**
     * @brief Get the Content Type start parameter
     *
     * @return std::string
     */
    std::string GetHeaderContentTypeStart();

    /**
     * @brief add attachment to mms message
     * if attachment is smil file must to add first
     * @param attachment
     * @return true
     * @return false
     */
    bool AddAttachment(MmsAttachment &attachment);

    /**
     * @brief Get the All Attachment from mms message
     * if has smil attachments defalut place frist
     * @param attachments
     * @return true
     * @return false
     */
    bool GetAllAttachment(std::vector<MmsAttachment> &attachments);

    /**
     * @brief DecodeMsg from a mms file
     *
     * @param mmsFilePathName
     * @return true
     * @return false
     */
    bool DecodeMsg(std::string mmsFilePathName);

    /**
     * @brief DecodeMsg from a mms from buff
     *
     * @param inBuff
     * @param inLen
     * @return true
     * @return false
     */
    bool DecodeMsg(std::unique_ptr<char[]> inBuff, uint32_t inLen);

    /**
     * @brief EncodeMsg form MmsMsg
     * return eoncode MmsMsg result data buff
     * @param outLen
     * @return std::unique_ptr<char[]>
     */
    std::unique_ptr<char[]> EncodeMsg(uint32_t &outLen);

    MmsHeader &GetHeader();

private:
    MmsHeader mmsHeader_;
    MmsBody mmsBody_;
};
} // namespace Telephony
} // namespace OHOS
#endif
