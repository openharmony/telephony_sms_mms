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

#ifndef SMS_SERVICE_INTERFACE_H
#define SMS_SERVICE_INTERFACE_H

#include "i_delivery_short_message_callback.h"
#include "i_send_short_message_callback.h"
#include "iremote_broker.h"
#include "short_message.h"

namespace OHOS {
namespace Telephony {
class ISmsServiceInterface : public IRemoteBroker {
public:
    using MessageID = enum {
        TEXT_BASED_SMS_DELIVERY = 0,
        SEND_SMS_TEXT_WITHOUT_SAVE,
        DATA_BASED_SMS_DELIVERY,
        SET_SMSC_ADDRESS,
        GET_SMSC_ADDRESS,
        ADD_SIM_MESSAGE,
        DEL_SIM_MESSAGE,
        UPDATE_SIM_MESSAGE,
        GET_ALL_SIM_MESSAGE,
        SET_CB_CONFIG,
        SET_IMS_SMS_CONFIG,
        SET_DEFAULT_SMS_SLOT_ID,
        GET_DEFAULT_SMS_SLOT_ID,
        SPLIT_MESSAGE,
        GET_SMS_SEGMENTS_INFO,
        GET_IMS_SHORT_MESSAGE_FORMAT,
        IS_IMS_SMS_SUPPORTED,
        HAS_SMS_CAPABILITY,
        SEND_MMS,
        DOWNLOAD_MMS,
        CREATE_MESSAGE,
        MMS_BASE64_ENCODE,
        MMS_BASE64_DECODE,
        GET_ENCODE_STRING,
    };

    /**
     * @brief SimMessageStatus
     * from 3GPP TS 27.005 V4.1.0 (2001-09) section 3 Parameter Definitions
     */
    using SimMessageStatus = enum {
        SIM_MESSAGE_STATUS_UNREAD = 0, // 0 REC UNREAD received unread message
        SIM_MESSAGE_STATUS_READ = 1, // 1 REC READ received read message
        SIM_MESSAGE_STATUS_UNSENT = 2, // 2 "STO UNSENT" stored unsent message (only applicable to SMs)
        SIM_MESSAGE_STATUS_SENT = 3, // 3 "STO SENT" stored sent message (only applicable to SMs)
    };

    /**
     * @brief SmsEncodingScheme
     * from  3GPP TS 23.038 [9] DCS
     */
    enum class SmsEncodingScheme {
        SMS_ENCODING_UNKNOWN = 0,
        SMS_ENCODING_7BIT,
        SMS_ENCODING_8BIT,
        SMS_ENCODING_16BIT,
    };

    struct SmsSegmentsInfo {
        int32_t msgSegCount = 0;
        int32_t msgEncodingCount = 0;
        int32_t msgRemainCount = 0;
        enum class SmsSegmentCodeScheme {
            SMS_ENCODING_UNKNOWN = 0,
            SMS_ENCODING_7BIT,
            SMS_ENCODING_8BIT,
            SMS_ENCODING_16BIT,
        } msgCodeScheme = SmsSegmentCodeScheme::SMS_ENCODING_UNKNOWN;
    };

    virtual ~ISmsServiceInterface() = default;

    /**
     * @brief SendMessage
     * Sends a text or data SMS message.
     * @param slotId [in]
     * @param desAddr [in]
     * @param scAddr [in]
     * @param text [in]
     * @param sendCallback [in]
     * @param deliverCallback [in]
     * @return int32_t
     */
    virtual int32_t SendMessage(int32_t slotId, const std::u16string desAddr, const std::u16string scAddr,
        const std::u16string text, const sptr<ISendShortMessageCallback> &sendCallback,
        const sptr<IDeliveryShortMessageCallback> &deliverCallback) = 0;

    /**
     * @brief SendMessage
     * Sends a text or data SMS message.
     * @param slotId [in]
     * @param desAddr [in]
     * @param scAddr [in]
     * @param port [in]
     * @param data [in]
     * @param dataLen [in]
     * @param sendCallback [in]
     * @param deliverCallback [in]
     * @return int32_t
     */
    virtual int32_t SendMessage(int32_t slotId, const std::u16string desAddr, const std::u16string scAddr,
        uint16_t port, const uint8_t *data, uint16_t dataLen, const sptr<ISendShortMessageCallback> &sendCallback,
        const sptr<IDeliveryShortMessageCallback> &deliverCallback) = 0;

    /**
     * @brief SetSmscAddr
     * Sets the address for the Short Message Service Center (SMSC) based on a specified slot ID.
     * @param slotId [in]
     * @param scAddr [in]
     * @return int32_t
     */
    virtual int32_t SetSmscAddr(int32_t slotId, const std::u16string &scAddr) = 0;

    /**
     * @brief GetSmscAddr
     * Obtains the SMSC address based on a specified slot ID.
     * @param slotId [in]
     * @param smscAddress [out]
     * @return int32_t.
     */
    virtual int32_t GetSmscAddr(int32_t slotId, std::u16string &smscAddress) = 0;

    /**
     * @brief AddSimMessage
     * Add a sms to sim card.
     * @param slotId [in]
     * @param smsc [in]
     * @param pdu [in]
     * @param status [in]
     * @return int32_t
     */
    virtual int32_t AddSimMessage(
        int32_t slotId, const std::u16string &smsc, const std::u16string &pdu, SimMessageStatus status) = 0;

    /**
     * @brief DelSimMessage
     * Delete a sms in the sim card.
     * @param slotId [in]
     * @param msgIndex [in]
     * @return int32_t
     */
    virtual int32_t DelSimMessage(int32_t slotId, uint32_t msgIndex) = 0;

    /**
     * @brief UpdateSimMessage
     * Update a sms in the sim card.
     * @param slotId [in]
     * @param msgIndex [in]
     * @param newStatus [in]
     * @param pdu [in]
     * @param smsc [in]
     * @return int32_t
     */
    virtual int32_t UpdateSimMessage(int32_t slotId, uint32_t msgIndex, SimMessageStatus newStatus,
        const std::u16string &pdu, const std::u16string &smsc) = 0;

    /**
     * @brief GetAllSimMessages
     * Get sim card all the sms.
     * @param slotId [in]
     * @param message [out]
     * @return int32_t
     */
    virtual int32_t GetAllSimMessages(int32_t slotId, std::vector<ShortMessage> &message) = 0;

    /**
     * @brief SetCBConfig
     * Configure a cell broadcast in a certain band range.
     * @param slotId [in]
     * @param enable [in]
     * @param fromMsgId [in]
     * @param toMsgId [in]
     * @param netType [in]
     * @return int32_t
     */
    virtual int32_t SetCBConfig(
        int32_t slotId, bool enable, uint32_t fromMsgId, uint32_t toMsgId, uint8_t netType) = 0;

    /**
     * @brief SetImsSmsConfig enable or disable IMS SMS.
     * @param slotId Indicates the card slot index number,
     * ranging from {@code 0} to the maximum card slot index number supported by the device.
     * @param enable Indicates enable or disable Ims sms
     * ranging {@code 0} disable Ims sms {@code 1} enable Ims sms
     * @return Returns {@code true} if enable or disable Ims Sms success; returns {@code false} otherwise.
     */
    virtual bool SetImsSmsConfig(int32_t slotId, int32_t enable) = 0;

    /**
     * @brief SetDefaultSmsSlotId
     * Set the Default Sms Slot Id To SmsService
     * @param slotId [in]
     * @return int32_t
     */
    virtual int32_t SetDefaultSmsSlotId(int32_t slotId) = 0;

    /**
     * @brief GetDefaultSmsSlotId
     * Get the Default Sms Slot Id From SmsService
     * @return int32_t
     */
    virtual int32_t GetDefaultSmsSlotId() = 0;

    /**
     * @brief SplitMessage
     * calculate Sms Message Split Segment count
     * @param message [in]
     * @param splitMessage [out]
     * @return int32_t
     */
    virtual int32_t SplitMessage(const std::u16string &message, std::vector<std::u16string> &splitMessage) = 0;

    /**
     * @brief GetSmsSegmentsInfo
     * calculate the Sms Message Segments Info
     * @param slotId [in]
     * @param message [in]
     * @param force7BitCode [in]
     * @param info [out]
     * @return int32_t
     */
    virtual int32_t GetSmsSegmentsInfo(
        int32_t slotId, const std::u16string &message, bool force7BitCode, SmsSegmentsInfo &info) = 0;

    /**
     * Check Sms Is supported Ims newtwork
     * @param slotId Indicates the card slot index number, ranging from {@code 0} to the maximum card slot index number
     * supported by the device.
     * @param isSupported Whether ims SMS is supported.
     * @return nterface execution results.
     */
    virtual int32_t IsImsSmsSupported(int32_t slotId, bool &isSupported) = 0;

    /**
     * @brief GetImsShortMessageFormat
     * Get the Ims Short Message Format 3gpp/3gpp2
     * Hide this for inner system use
     * @param format Ims short message format
     * @return int32_t
     */
    virtual int32_t GetImsShortMessageFormat(std::u16string &format) = 0;

    /**
     * @brief HasSmsCapability
     * Check whether it is supported Sms Capability
     * Hide this for inner system use
     * @return true
     * @return false
     */
    virtual bool HasSmsCapability() = 0;

    /**
     * create a short message
     * @param pdu Indicates pdu code,
     * @param specification Indicates 3gpp or 3gpp2
     * @param ShortMessage Indicates a short message object
     * @return Returns {@code 0} if CreateMessage success
     */
    virtual int32_t CreateMessage(std::string pdu, std::string specification, ShortMessage &message) = 0;

    /**
     * mms base64 encode
     * @param src Indicates source string,
     * @param dest Indicates destination string
     * @return Returns {@code true} if encode success; returns {@code false} otherwise
     */
    virtual bool GetBase64Encode(std::string src, std::string &dest) = 0;

    /**
     * mms base64 decode
     * @param src Indicates source string,
     * @param dest Indicates destination string
     * @return Returns {@code true} if decode success; returns {@code false} otherwise
     */
    virtual bool GetBase64Decode(std::string src, std::string &dest) = 0;

    /**
     * Get Encode String
     * @param encodeString Indicates output string,
     * @param charset Indicates character set,
     * @param valLength Indicates input string length,
     * @param strEncodeString Indicates input string
     * @return Returns {@code true} if decode success; returns {@code false} otherwise
     */
    virtual bool GetEncodeStringFunc(
        std::string &encodeString, uint32_t charset, uint32_t valLength, std::string strEncodeString) = 0;

public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.Telephony.ISmsServiceInterface");
};
} // namespace Telephony
} // namespace OHOS
#endif
