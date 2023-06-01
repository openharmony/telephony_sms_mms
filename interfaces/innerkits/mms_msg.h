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
     * @brief Dump the mms Message, printf Mms Message Infomations.
     */
    void DumpMms();

    /**
     * @brief Get Mms Header Protocol Version.
     *
     * @return returns Mms Header Protocol Version.
     */
    uint16_t GetMmsVersion();

    /**
     * @brief Set Mms Header Protocol Version.
     *
     * @param value the Mms Header Protocol Version.
     * @return true if set successfully, false otherwise.
     */
    bool SetMmsVersion(uint16_t value);

    /**
     * @brief Get the Mms Message Type.
     *
     * @return uint8_t returns Mms Header Message Type.
     */
    uint8_t GetMmsMessageType();

    /**
     * @brief Set the Mms Message Type.
     *
     * @param value Mms Header Message Type.
     * @return true if set successfully, false otherwise.
     */
    bool SetMmsMessageType(uint8_t value);

    /**
     * @brief Get the Mms Transaction Id From Header, a unique identifier for the PDU.
     *
     * @return std::string returns the Mms Transaction Id.
     */
    std::string GetMmsTransactionId();

    /**
     * @brief Set the Mms Transaction Id To Header, a unique identifier for the PDU.
     *
     * @param value the Mms Transaction Id.
     * @return true if set successfully, false otherwise.
     */
    bool SetMmsTransactionId(std::string value);

    /**
     * @brief Get the Mms Date From Header.
     *
     * @return int64_t return the Mms Message Header Date Infomations.
     */
    int64_t GetMmsDate();

    /**
     * @brief Set the Mms Date To Header.
     *
     * @param value the Mms Message Header Date Infomations.
     * @return true if set successfully, false otherwise.
     */
    bool SetMmsDate(int64_t value);

    /**
     * @brief Get the Mms Subject From Header.
     *
     * @return std::string Mms Message Header SubJect Infomations.
     */
    std::string GetMmsSubject();

    /**
     * @brief Set Mms Message Header Subject Infomations.
     *
     * @param value Mms Message Header Subject Infomations.
     * @return true if set successfully, false otherwise.
     */
    bool SetMmsSubject(std::string value);

    /**
     * @brief Get Mms Message Header From Address Infomations.
     *
     * @return returns a MmsAddress object, contains the address, address type .etc
     */
    MmsAddress GetMmsFrom();

    /**
     * @brief Set Mms Message Header From Address Infomations.
     *
     * @param address the MmsAddress object, contains the address, address type .etc
     * @return true if set successfully, false otherwise.
     */
    bool SetMmsFrom(MmsAddress address);

    /**
     * @brief Set the Mms To Address in Header, support multiple address.
     *
     * @param toAddrs a list of MmsAddress objects.
     * @return true if set successfully, false otherwise.
     */
    bool SetMmsTo(std::vector<MmsAddress>& toAddrs);

    /**
     * @brief Get the Mms Msg to address from the header, support multiple address.
     *
     * @param toAddrs a list of MmsAddress objects.
     * @return true if get successfully, false otherwise.
     */
    bool GetMmsTo(std::vector<MmsAddress>& toAddrs);

    /**
     * @brief Get Octet Type Value From the Header.
     * This is Generic methods to Get Mms Message Header.
     *
     * @param fieldId the Header Field Assigned Number {@link MmsFieldCode}.
     * @return uint8_t returns the Octet Type Value.
     */
    uint8_t GetHeaderOctetValue(uint8_t fieldId);

    /**
     * @brief Set the Header Octet Value To Header.
     * This is Generic methods to Set Mms Message Header.
     *
     * @param fieldId the Header Field Assigned Number {@link MmsFieldCode}.
     * @param value the Header Octet Value.
     * @return true if set successfully, false otherwise.
     */
    bool SetHeaderOctetValue(uint8_t fieldId, uint8_t value);

    /**
     * @brief Get the Header Integer Value From Header.
     * This is Generic methods to Get Mms Message Header.
     *
     * @param fieldId the Header Field Assigned Number {@link MmsFieldCode}.
     * @return int32_t return the Header Octet Value.
     */
    int32_t GetHeaderIntegerValue(uint8_t fieldId);

    /**
     * @brief Set the Header Integer Value To Header.
     * This is Generic methods to Set Mms Message Header.
     *
     * @param fieldId the Header Field Assigned Number {@link MmsFieldCode}.
     * @param value the Header Integer Value.
     * @return true if set successfully, false otherwise.
     */
    bool SetHeaderIntegerValue(uint8_t fieldId, int32_t value);

    /**
     * @brief Get Long Type Value From the Header.
     * This is Generic methods to Get Mms Message Header.
     *
     * @param fieldId the Header Field Assigned Number {@link MmsFieldCode}.
     * @return int64_t return the Long Type Value.
     */
    int64_t GetHeaderLongValue(uint8_t fieldId);

    /**
     * @brief Set the Header Long Value To Header
     * This is Generic methods to Set Mms Message Header.
     *
     * @param fieldId the Header Field Assigned Number {@link MmsFieldCode}
     * @param value the Field Long Value
     * @return true if set successfully, false otherwise
     */
    bool SetHeaderLongValue(uint8_t fieldId, int64_t value);

    /**
     * @brief Get String or Encode String Type Value From the Header.
     * This is Generic methods to Get Mms Message Header.
     *
     * @param fieldId the Header Field Assigned Number {@link MmsFieldCode}.
     * @return std::string return Field Text Value Or EncondeString with UTF_8.
     */
    std::string GetHeaderStringValue(uint8_t fieldId);

    /**
     * @brief Set the Header String Value To Header.
     * This is Generic methods to Set Mms Message Header.
     *
     * @param fieldId the Header Field Assigned Number {@link MmsFieldCode}.
     * @param value Field Text Value.
     * @return true if set successfully, false otherwise.
     */
    bool SetHeaderStringValue(uint8_t fieldId, std::string &value);

    /**
     * @brief Set the Header Encoded String Value From Header.
     * This is Generic methods to Set Mms Message Header.
     *
     * @param fieldId the Header Field Assigned Number {@link MmsFieldCode}.
     * @param value the Field EncondeString Value.
     * @param charset the charset type.
     * @return true if set successfully, false otherwise.
     */
    bool SetHeaderEncodedStringValue(uint8_t fieldId, const std::string &value, uint32_t charset);

    /**
     * @brief AddHeaderAddressValue, for example: To / Cc / Bcc.
     *
     * @param fieldId the Header Field Assigned Number {@link MmsFieldCode}.
     * @param address the MmsAddress object, contains the address, address type .etc
     * @return true if add successfully, false otherwise.
     */
    bool AddHeaderAddressValue(uint8_t fieldId, MmsAddress &address);

    /**
     * @brief Get the Header All Address Value From Header,
     * for example: To / Cc / Bcc.
     *
     * @param fieldId the Header Field Assigned Number {@link MmsFieldCode}.
     * @param addressValue a list of MmsAddress Objects.
     * @return true if get successfully, false otherwise.
     */
    bool GetHeaderAllAddressValue(uint8_t fieldId, std::vector<MmsAddress> &addressValue);

    /**
     * @brief Set the Content Type.
     *
     * @param strType the content type.
     * @return true if set successfully, false otherwise.
     */
    bool SetHeaderContentType(std::string strType);

    /**
     * @brief Get the Content Type.
     *
     * @return std::string return the header content type.
     */
    std::string GetHeaderContentType();

    /**
     * @brief Get the Content Type start parameter.
     *
     * @return std::string return the Content Type start parameter.
     */
    std::string GetHeaderContentTypeStart();

    /**
     * @brief add attachment to mms message if attachment is smil file must to add first.
     *
     * @param attachment the MmsAttachment object, indicates the attachment
     * detail information for the MMS message.
     * @return returns true if add successfully, false otherwise.
     */
    bool AddAttachment(MmsAttachment &attachment);

    /**
     * @brief Get the All Attachments from mms message.
     *
     * @param attachments a list of MmsAttachment objects, indicates the attachment
     * detail information for the MMS message.
     * @return returns true if get successfully, false otherwise.
     */
    bool GetAllAttachment(std::vector<MmsAttachment> &attachments);

    /**
     * @brief DecodeMsg from a mms file.
     *
     * @param mmsFilePathName the path of mms file.
     * @return true if decode successfully, false otherwise.
     */
    bool DecodeMsg(std::string mmsFilePathName);

    /**
     * @brief DecodeMsg from a mms from buff.
     *
     * @param inBuff the mms buffer.
     * @param inLen the mms buffer length.
     * @return true if decode successfully, false otherwise.
     */
    bool DecodeMsg(std::unique_ptr<char[]> inBuff, uint32_t inLen);

    /**
     * @brief EncodeMsg form MmsMsg.
     *
     * @param outLen the result message length limit.
     * @return return eoncode MmsMsg result data buff.
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
