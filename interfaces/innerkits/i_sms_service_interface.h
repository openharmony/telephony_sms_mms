/*
 * Copyright (C) 2021-2023 Huawei Device Co., Ltd.
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
    /**
     * @brief SimMessageStatus
     * from 3GPP TS 27.005 V4.1.0 (2001-09) section 3 Parameter Definitions
     */
    using SimMessageStatus = enum {
        /**
         * REC UNREAD received unread message.
         */
        SIM_MESSAGE_STATUS_UNREAD = 0,

        /**
         * REC READ received read message.
         */
        SIM_MESSAGE_STATUS_READ = 1,

        /**
         * "STO UNSENT" stored unsent message (only applicable to SMs).
         */
        SIM_MESSAGE_STATUS_UNSENT = 2,

        /**
         * "STO SENT" stored sent message (only applicable to SMs).
         */
        SIM_MESSAGE_STATUS_SENT = 3,
    };

    /**
     * @brief Indicates the encoding scheme of Sms.
     * from  3GPP TS 23.038 [9] DCS
     */
    enum class SmsEncodingScheme {
        /**
         * Indicates an unknown encoding scheme.
         */
        SMS_ENCODING_UNKNOWN = 0,

        /**
         * Indicates that the encoding scheme is 7-digit.
         */
        SMS_ENCODING_7BIT,

        /**
         * Indicates that the encoding scheme is 8-digit.
         */
        SMS_ENCODING_8BIT,

        /**
         * Indicates that the encoding schemes is 16-digit.
         */
        SMS_ENCODING_16BIT,
    };

    /**
     * @brief Indicates the SMS message segment information.
     */
    struct SmsSegmentsInfo {
        /**
         * Indicates the split count for the SMS message segment information.
         */
        int32_t msgSegCount = 0;
        /**
         * Indicates the encoding count for the SMS message segment information.
         */
        int32_t msgEncodingCount = 0;
        /**
         * Indicates the remaining encoding count for the SMS message segment information.
         */
        int32_t msgRemainCount = 0;

        /**
         * Defines the encoding scheme of sms segment.
         */
        enum class SmsSegmentCodeScheme {
            /**
             * Indicates an unknown encoding scheme.
             */
            SMS_ENCODING_UNKNOWN = 0,
            /**
             * Indicates that the encoding scheme is 7-digit.
             */
            SMS_ENCODING_7BIT,
            /**
             * Indicates that the encoding scheme is 8-digit.
             */
            SMS_ENCODING_8BIT,
            /**
             * Indicates that the encoding scheme is 16-digit.
             */
            SMS_ENCODING_16BIT,
        } msgCodeScheme = SmsSegmentCodeScheme::SMS_ENCODING_UNKNOWN;
    };

    virtual ~ISmsServiceInterface() = default;

    /**
     * @brief Sends a text type SMS message.
     *
     * @param slotId [in], indicates the card slot index number,
     * ranging from {@code 0} to the maximum card slot index number supported by the device.
     * @param desAddr [in], indicates the destination address.
     * @param scAddr [in], indicates the sms center address.
     * @param text [in], indicates sms content.
     * @param sendCallback [in], indicates callback for send out.
     * @param deliverCallback [in], indicates callback for delivery to destination user.
     * @return int32_t, returns {@code 0} if success.
     */
    virtual int32_t SendMessage(int32_t slotId, const std::u16string desAddr, const std::u16string scAddr,
        const std::u16string text, const sptr<ISendShortMessageCallback> &sendCallback,
        const sptr<IDeliveryShortMessageCallback> &deliverCallback) = 0;

    /**
     * @brief Sends a data type SMS message.
     *
     * @param slotId [in], indicates the card slot index number,
     * ranging from {@code 0} to the maximum card slot index number supported by the device.
     * @param desAddr [in], indicates the destination address.
     * @param scAddr [in], indicates the sms center address.
     * @param port [in], indicates the port of data sms.
     * @param data [in], indicates the array of data sms.
     * @param dataLen [in], indicates the array length of data sms.
     * @param sendCallback [in], indicates callback for send out.
     * @param deliverCallback [in], indicates callback for delivery to destination user.
     * @return int32_t, returns {@code 0} if success.
     */
    virtual int32_t SendMessage(int32_t slotId, const std::u16string desAddr, const std::u16string scAddr,
        uint16_t port, const uint8_t *data, uint16_t dataLen, const sptr<ISendShortMessageCallback> &sendCallback,
        const sptr<IDeliveryShortMessageCallback> &deliverCallback) = 0;

    /**
     * @brief Sets the address for the Short Message Service Center (SMSC) based on a specified slot ID.
     *
     * @param slotId [in], indicates the card slot index number,
     * ranging from {@code 0} to the maximum card slot index number supported by the device.
     * @param scAddr [in], indicates the sms center address.
     * @return int32_t, returns {@code 0} if success.
     */
    virtual int32_t SetSmscAddr(int32_t slotId, const std::u16string &scAddr) = 0;

    /**
     * @brief Obtains the SMSC address based on a specified slot ID.
     *
     * @param slotId [in], indicates the card slot index number,
     * ranging from {@code 0} to the maximum card slot index number supported by the device.
     * @param smscAddress [out]
     * @return int32_t, returns {@code 0} if success.
     */
    virtual int32_t GetSmscAddr(int32_t slotId, std::u16string &smscAddress) = 0;

    /**
     * @brief Add a sms to sim card.
     *
     * @param slotId [in], indicates the card slot index number,
     * ranging from {@code 0} to the maximum card slot index number supported by the device.
     * @param smsc [in], indicates the short message service center.
     * @param pdu [in], indicates the protocol data unit of message.
     * @param status [in], indicates the status of sim message.
     * @return int32_t, returns {@code 0} if success.
     */
    virtual int32_t AddSimMessage(
        int32_t slotId, const std::u16string &smsc, const std::u16string &pdu, SimMessageStatus status) = 0;

    /**
     * @brief Delete a sms in the sim card.
     *
     * @param slotId [in], indicates the card slot index number,
     * ranging from {@code 0} to the maximum card slot index number supported by the device.
     * @param msgIndex [in], indicates the message index.
     * @return int32_t, returns {@code 0} if success.
     */
    virtual int32_t DelSimMessage(int32_t slotId, uint32_t msgIndex) = 0;

    /**
     * @brief Update a sms in the sim card.
     *
     * @param slotId [in], indicates the card slot index number,
     * ranging from {@code 0} to the maximum card slot index number supported by the device.
     * @param msgIndex [in], indicates the message index.
     * @param newStatus [in], indicates the new status of the sim message.
     * @param pdu [in], indicates the protocol data unit of message.
     * @param smsc [in], indicates the short message service center.
     * @return int32_t, returns {@code 0} if success.
     */
    virtual int32_t UpdateSimMessage(int32_t slotId, uint32_t msgIndex, SimMessageStatus newStatus,
        const std::u16string &pdu, const std::u16string &smsc) = 0;

    /**
     * @brief Get sim card all the sms.
     *
     * @param slotId [in], indicates the card slot index number,
     * ranging from {@code 0} to the maximum card slot index number supported by the device.
     * @param message [out], indicates all SMS messages of sim card.
     * @return int32_t, returns {@code 0} if success.
     */
    virtual int32_t GetAllSimMessages(int32_t slotId, std::vector<ShortMessage> &message) = 0;

    /**
     * @brief Configure a cell broadcast in a certain band range.
     *
     * @param slotId [in], indicates the card slot index number,
     * ranging from {@code 0} to the maximum card slot index number supported by the device.
     * @param enable [in], indicates whether to enable cell broadcast.
     * @param fromMsgId [in], indicates the start message ID.
     * @param toMsgId [in], indicates the end message ID.
     * @param netType [in], indicates the network type.
     * @return int32_t, returns {@code 0} if success.
     */
    virtual int32_t SetCBConfig(int32_t slotId, bool enable, uint32_t fromMsgId, uint32_t toMsgId, uint8_t netType) = 0;

    /**
     * @brief Enable or disable IMS SMS.
     *
     * @param slotId Indicates the card slot index number,
     * ranging from {@code 0} to the maximum card slot index number supported by the device.
     * @param enable Indicates enable or disable Ims sms
     * ranging {@code 0} disable Ims sms {@code 1} enable Ims sms
     * @return Returns {@code true} if enable or disable Ims Sms success; returns {@code false} otherwise.
     */
    virtual bool SetImsSmsConfig(int32_t slotId, int32_t enable) = 0;

    /**
     * @brief Set the Default Sms Slot Id To SmsService
     *
     * @param slotId [in], indicates the card slot index number,
     * ranging from {@code 0} to the maximum card slot index number supported by the device.
     * @return int32_t, returns {@code 0} if success.
     */
    virtual int32_t SetDefaultSmsSlotId(int32_t slotId) = 0;

    /**
     * @brief Get the Default Sms Slot Id From SmsService
     *
     * @return int32_t, returns {@code 0} if success.
     */
    virtual int32_t GetDefaultSmsSlotId() = 0;

    /**
     * @brief Get the Default Sms Sim Id From SmsService
     *
     * @param simId [out], indicates the sms sim index number.
     * @return int32_t, returns {@code 0} if success.
     */
    virtual int32_t GetDefaultSmsSimId(int32_t &simId) = 0;

    /**
     * @brief Calculate Sms Message Split Segment count
     *
     * @param message [in], indicates input message.
     * @param splitMessage [out], indicates the split information.
     * @return int32_t, returns {@code 0} if success.
     */
    virtual int32_t SplitMessage(const std::u16string &message, std::vector<std::u16string> &splitMessage) = 0;

    /**
     * @brief Calculate the Sms Message Segments Info
     *
     * @param slotId [in], indicates the card slot index number,
     * ranging from {@code 0} to the maximum card slot index number supported by the device.
     * @param message [in], indicates input message.
     * @param force7BitCode [in], indicates sms encode type, 7bit or not.
     * @param info [out], indicates output sms segment.
     * @return int32_t, returns {@code 0} if get sms segments info.
     */
    virtual int32_t GetSmsSegmentsInfo(
        int32_t slotId, const std::u16string &message, bool force7BitCode, SmsSegmentsInfo &info) = 0;

    /**
     * @brief Check Sms Is supported Ims newtwork
     *
     * @param slotId Indicates the card slot index number, ranging from {@code 0} to the maximum card slot index number
     * supported by the device.
     * @param isSupported Whether ims SMS is supported.
     * @return int32_t, returns {@code 0} if success.
     */
    virtual int32_t IsImsSmsSupported(int32_t slotId, bool &isSupported) = 0;

    /**
     * @brief Get the Ims Short Message Format 3gpp/3gpp2
     *
     * @param format Ims short message format
     * @return int32_t, returns {@code 0} if success.
     */
    virtual int32_t GetImsShortMessageFormat(std::u16string &format) = 0;

    /**
     * @brief Check whether it is supported Sms Capability
     *
     * @return true
     * @return false
     */
    virtual bool HasSmsCapability() = 0;

    /**
     * @brief Create a short message
     *
     * @param pdu Indicates pdu code,
     * @param specification Indicates 3gpp or 3gpp2
     * @param message Indicates a short message object
     * @return Returns {@code 0} if CreateMessage success
     */
    virtual int32_t CreateMessage(std::string pdu, std::string specification, ShortMessage &message) = 0;

    /**
     * @brief Mms base64 encode
     *
     * @param src Indicates source string,
     * @param dest Indicates destination string
     * @return Returns {@code true} if encode success; returns {@code false} otherwise
     */
    virtual bool GetBase64Encode(std::string src, std::string &dest) = 0;

    /**
     * @brief Mms base64 decode
     *
     * @param src Indicates source string,
     * @param dest Indicates destination string
     * @return Returns {@code true} if decode success; returns {@code false} otherwise
     */
    virtual bool GetBase64Decode(std::string src, std::string &dest) = 0;

    /**
     * @brief Get Encode String
     *
     * @param encodeString Indicates output string,
     * @param charset Indicates character set,
     * @param valLength Indicates input string length,
     * @param strEncodeString Indicates input string
     * @return Returns {@code true} if decode success; returns {@code false} otherwise
     */
    virtual bool GetEncodeStringFunc(
        std::string &encodeString, uint32_t charset, uint32_t valLength, std::string strEncodeString) = 0;

    /**
     * Send a mms
     * @param slotId Indicates the card slot index number,
     * ranging from {@code 0} to the maximum card slot index number supported by the device
     * @param mmsc Indicates service center of mms
     * @param data Indicates file path of mms pdu
     * @param ua Indicates mms user agent
     * @param uaprof Indicates mms user agent profile
     * @return Returns {@code 0} if send mms success
     */
    virtual int32_t SendMms(int32_t slotId, const std::u16string &mmsc, const std::u16string &data,
        const std::u16string &ua, const std::u16string &uaprof) = 0;

    /**
     * Download a mms
     * @param slotId Indicates the card slot index number,
     * ranging from {@code 0} to the maximum card slot index number supported by the device
     * @param mmsc Indicates service center of mms
     * @param data Indicates file path of mms pdu
     * @param ua Indicates mms user agent
     * @param uaprof Indicates mms user agent profile
     * @return Returns {@code 0} if download mms success
     */
    virtual int32_t DownloadMms(int32_t slotId, const std::u16string &mmsc, std::u16string &data,
        const std::u16string &ua, const std::u16string &uaprof) = 0;

public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.Telephony.ISmsServiceInterface");
};
} // namespace Telephony
} // namespace OHOS
#endif
