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

#ifndef SMS_SERVICE_MANAGER_CLIENT_H
#define SMS_SERVICE_MANAGER_CLIENT_H

#include <mutex>
#include <string>
#include <vector>

#include "i_sms_service_interface.h"
#include "refbase.h"
#include "singleton.h"

namespace OHOS {
namespace Telephony {
class SmsServiceManagerClient : public std::enable_shared_from_this<SmsServiceManagerClient> {
    DECLARE_DELAYED_SINGLETON(SmsServiceManagerClient)
public:
    /**
     * @brief Init the proxy of SmsService.
     */
    bool InitSmsServiceProxy();

    /**
     * @brief Reset the proxy of SmsService.
     */
    void ResetSmsServiceProxy();

    /**
     * @brief Set the Default Sms Slot Id To SmsService
     *
     * @param slotId [in], indicates the card slot index number,
     * ranging from {@code 0} to the maximum card slot index number supported by the device.
     * @return int32_t, returns {@code 0} if success.
     */
    int32_t SetDefaultSmsSlotId(int32_t slotId);

    /**
     * @brief Get the Default Sms Slot Id From SmsService
     *
     * @return int32_t, returns {@code 0} if success.
     */
    int32_t GetDefaultSmsSlotId();

    /**
     * @brief Get the Default Sms Sim Id From SmsService
     *
     * @param simId [out], indicates the sms sim index number.
     * @return int32_t, returns {@code 0} if success.
     */
    int32_t GetDefaultSmsSimId(int32_t &simId);

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
    int32_t SendMessage(int32_t slotId, const std::u16string desAddr, const std::u16string scAddr,
        const std::u16string text, const sptr<ISendShortMessageCallback> &callback,
        const sptr<IDeliveryShortMessageCallback> &deliveryCallback);

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
    int32_t SendMessage(int32_t slotId, const std::u16string desAddr, const std::u16string scAddr,
        uint16_t port, const uint8_t *data, uint16_t dataLen, const sptr<ISendShortMessageCallback> &callback,
        const sptr<IDeliveryShortMessageCallback> &deliveryCallback);
    /**
     * @brief Sets the address for the Short Message Service Center (SMSC) based on a specified slot ID.
     *
     * @param slotId [in], indicates the card slot index number,
     * ranging from {@code 0} to the maximum card slot index number supported by the device.
     * @param scAddr [in], indicates the sms center address.
     * @return int32_t, returns {@code 0} if success.
     */
    int32_t SetScAddress(int32_t slotId, const std::u16string &scAddr);

    /**
     * @brief Obtains the SMSC address based on a specified slot ID.
     *
     * @param slotId [in], indicates the card slot index number,
     * ranging from {@code 0} to the maximum card slot index number supported by the device.
     * @param smscAddress [out]
     * @return int32_t, returns {@code 0} if success.
     */
    int32_t GetScAddress(int32_t slotId, std::u16string &smscAddress);

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
    int32_t AddSimMessage(int32_t slotId, const std::u16string &smsc, const std::u16string &pdu,
        ISmsServiceInterface::SimMessageStatus status);

    /**
     * @brief Delete a sms in the sim card.
     *
     * @param slotId [in], indicates the card slot index number,
     * ranging from {@code 0} to the maximum card slot index number supported by the device.
     * @param msgIndex [in], indicates the message index.
     * @return int32_t, returns {@code 0} if success.
     */
    int32_t DelSimMessage(int32_t slotId, uint32_t msgIndex);

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
    int32_t UpdateSimMessage(int32_t slotId, uint32_t msgIndex, ISmsServiceInterface::SimMessageStatus newStatus,
        const std::u16string &pdu, const std::u16string &smsc);

    /**
     * @brief Get sim card all the sms.
     *
     * @param slotId [in], indicates the card slot index number,
     * ranging from {@code 0} to the maximum card slot index number supported by the device.
     * @param message [out], indicates all SMS messages of sim card.
     * @return int32_t, returns {@code 0} if success.
     */
    int32_t GetAllSimMessages(int32_t slotId, std::vector<ShortMessage> &message);

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
    int32_t SetCBConfig(int32_t slotId, bool enable, uint32_t startMessageId, uint32_t endMessageId, uint8_t ranType);

    /**
     * @brief SetImsSmsConfig enable or disable IMS SMS.
     *
     * @param slotId Indicates the card slot index number,
     * ranging from {@code 0} to the maximum card slot index number supported by the device.
     * @param enable Indicates enable or disable Ims sms
     * ranging {@code 0} disable Ims sms {@code 1} enable Ims sms
     * @return Returns {@code true} if enable or disable Ims Sms success; returns {@code false} otherwise.
     */
    bool SetImsSmsConfig(int32_t slotId, int32_t enable);

    /**
     * @brief Calculate Sms Message Split Segment count
     *
     * @param message [in], indicates input message.
     * @param splitMessage [out], indicates the split information.
     * @return int32_t, returns {@code 0} if success.
     */
    int32_t SplitMessage(const std::u16string &message, std::vector<std::u16string> &splitMessage);

    /**
     * @brief Calculate the Sms Message Segments Info
     *
     * @param slotId [in], indicates the card slot index number,
     * ranging from {@code 0} to the maximum card slot index number supported by the device.
     * @param message [in], indicates input message.
     * @param force7BitCode [in], indicates sms encode type, 7bit or not.
     * @param segInfo [out], indicates output sms segment.
     * @return int32_t, returns {@code 0} if get sms segments info.
     */
    int32_t GetSmsSegmentsInfo(int32_t slotId, const std::u16string &message, bool force7BitCode,
        ISmsServiceInterface::SmsSegmentsInfo &segInfo);

    /**
     * @brief Check Sms Is supported Ims newtwork
     *
     * @param slotId Indicates the card slot index number, ranging from {@code 0} to the maximum card
     * slot index number supported by the device.
     * @param isSupported Whether ims SMS is supported.
     * @return int32_t, returns {@code 0} if success.
     */
    int32_t IsImsSmsSupported(int32_t slotId, bool &isSupported);

    /**
     * @brief Get the Ims Short Message Format 3gpp/3gpp2
     *
     * @param format Ims short message format
     * @return int32_t, returns {@code 0} if success.
     */
    int32_t GetImsShortMessageFormat(std::u16string &format);

    /**
     * @brief Check whether it is supported Sms Capability
     *
     * @return {@code true} if UE has sms capability; returns {@code false} otherwise.
     */
    bool HasSmsCapability();

    /**
     * @brief Create a short message
     *
     * @param pdu Indicates pdu code,
     * @param specification Indicates 3gpp or 3gpp2
     * @param message Indicates a short message object
     * @return Returns {@code 0} if CreateMessage success
     */
    int32_t CreateMessage(std::string pdu, std::string specification, ShortMessage &message);

    /**
     * @brief Mms base64 encode
     *
     * @param src Indicates source string,
     * @param dest Indicates destination string
     * @return Returns {@code true} if encode success; returns {@code false} otherwise
     */
    bool GetBase64Encode(std::string src, std::string &dest);

    /**
     * @brief Mms base64 decode
     *
     * @param src Indicates source string,
     * @param dest Indicates destination string
     * @return Returns {@code true} if decode success; returns {@code false} otherwise
     */
    bool GetBase64Decode(std::string src, std::string &dest);

    /**
     * @brief Get Encode String
     *
     * @param encodeString Indicates output string,
     * @param charset Indicates character set,
     * @param valLength Indicates input string length,
     * @param strEncodeString Indicates input string
     * @return Returns {@code true} if decode success; returns {@code false} otherwise
     */
    bool GetEncodeStringFunc(
        std::string &encodeString, uint32_t charset, uint32_t valLength, std::string strEncodeString);

    /**
     * Send a Mms.
     * @param slotId Indicates the card slot index number,
     * ranging from {@code 0} to the maximum card slot index number supported by
     * the device
     * @param mmsc Indicates service center of mms
     * @param data Indicates mms pdu byte array
     * @param ua Indicates mms user agent
     * @param uaprof Indicates mms user agent profile
     * @return Returns {@code 0} if send mms success; returns {@code false} otherwise
     */
    int32_t SendMms(int32_t slotId, const std::u16string &mmsc, const std::u16string &data, const std::u16string &ua,
        const std::u16string &uaprof);

    /**
     * Download a Mms.
     * @param slotId Indicates the card slot index number,
     * ranging from {@code 0} to the maximum card slot index number supported by
     * the device
     * @param mmsc Indicates service center of mms
     * @param data Indicates mms pdu byte array
     * @param ua Indicates mms user agent
     * @param uaprof Indicates mms user agent profile
     * @return Returns {@code 0} if download mms success; returns {@code false} otherwise
     */
    int32_t DownloadMms(int32_t slotId, const std::u16string &mmsc, std::u16string &data, const std::u16string &ua,
        const std::u16string &uaprof);

private:
    std::mutex mutex_;
    sptr<ISmsServiceInterface> smsServiceInterface_;
    sptr<IRemoteObject::DeathRecipient> recipient_;
};

/**
 * @brief Enumerates the result of sending SMS.
 */
enum class SmsSendResult {
    /**
     * Indicates that the SMS message is successfully sent.
     */
    SEND_SMS_SUCCESS = 0,
    /**
     * Indicates that sending the SMS message fails due to an unknown reason.
     */
    SEND_SMS_FAILURE_UNKNOWN = 1,
    /**
     * Indicates that sending the SMS fails because the modem is powered off.
     */
    SEND_SMS_FAILURE_RADIO_OFF = 2,
    /**
     * Indicates that sending the SMS message fails because the network is unavailable
     * or does not support sending or reception of SMS messages.
     */
    SEND_SMS_FAILURE_SERVICE_UNAVAILABLE = 3
};
} // namespace Telephony
} // namespace OHOS
#endif // SMS_SERVICE_MANAGER_CLIENT_H
