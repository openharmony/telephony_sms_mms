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

#ifndef SMS_SERVICE_PROXY_H
#define SMS_SERVICE_PROXY_H

#include "i_sms_service_interface.h"
#include "iremote_object.h"
#include "iremote_proxy.h"

namespace OHOS {
namespace Telephony {
class SmsServiceProxy : public IRemoteProxy<ISmsServiceInterface> {
public:
    explicit SmsServiceProxy(const sptr<IRemoteObject> &impl);
    virtual ~SmsServiceProxy() = default;

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
    int32_t SendMessage(int32_t slotId, const std::u16string desAddr, const std::u16string scAddr,
        const std::u16string text, const sptr<ISendShortMessageCallback> &sendCallback,
        const sptr<IDeliveryShortMessageCallback> &deliverCallback) override;

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
     * @param deliverCallback [in].
     * @return int32_t
     */
    int32_t SendMessage(int32_t slotId, const std::u16string desAddr, const std::u16string scAddr, uint16_t port,
        const uint8_t *data, uint16_t dataLen, const sptr<ISendShortMessageCallback> &sendCallback,
        const sptr<IDeliveryShortMessageCallback> &deliverCallback) override;

    /**
     * @brief SetSmscAddr
     * Sets the address for the Short Message Service Center (SMSC) based on a specified slot ID.
     * @param slotId [in]
     * @param scAddr [in]
     * @return int32_t
     */
    int32_t SetSmscAddr(int32_t slotId, const std::u16string &scAddr) override;

    /**
     * @brief GetSmscAddr
     * Obtains the SMSC address based on a specified slot ID.
     * @param slotId [in]
     * @param smscAddress [out]
     * @return Interface execution results.
     */
    int32_t GetSmscAddr(int32_t slotId, std::u16string &smscAddress) override;

    /**
     * @brief AddSimMessage
     * Add a sms to sim card.
     * @param slotId [in]
     * @param smsc [in]
     * @param pdu [in]
     * @param status [in]
     * @return int32_t
     */
    int32_t AddSimMessage(
        int32_t slotId, const std::u16string &smsc, const std::u16string &pdu, SimMessageStatus status) override;

    /**
     * @brief DelSimMessage
     * Delete a sms in the sim card.
     * @param slotId [in]
     * @param msgIndex [in]
     * @return int32_t
     */
    int32_t DelSimMessage(int32_t slotId, uint32_t msgIndex) override;

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
    int32_t UpdateSimMessage(int32_t slotId, uint32_t msgIndex, SimMessageStatus newStatus, const std::u16string &pdu,
        const std::u16string &smsc) override;

    /**
     * @brief GetAllSimMessages
     * Get sim card all the sms.
     * @param slotId [in]
     * @return int32_t
     */
    int32_t GetAllSimMessages(int32_t slotId, std::vector<ShortMessage> &message) override;

    /**
     * @brief SetCBConfig
     * Configure a cell broadcast in a certain band range.
     * @param slotId [in]
     * @param enable [in]
     * @param fromMsgId [in]
     * @param toMsgId [in]
     * @param netType [in]
     * @return true
     * @return false
     */
    int32_t SetCBConfig(int32_t slotId, bool enable, uint32_t fromMsgId, uint32_t toMsgId, uint8_t netType) override;

    /**
     * @brief SetImsSmsConfig enable or disable IMS SMS.
     * @param slotId Indicates the card slot index number,
     * ranging from {@code 0} to the maximum card slot index number supported by the device.
     * @param enable Indicates enable or disable Ims sms
     * ranging {@code 0} disable Ims sms {@code 1} enable Ims sms
     * @return Returns {@code true} if enable or disable Ims Sms success; returns {@code false} otherwise.
     */
    bool SetImsSmsConfig(int32_t slotId, int32_t enable) override;

    /**
     * @brief SetDefaultSmsSlotId
     * Set the Default Sms Slot Id To SmsService
     * @param slotId [in]
     * @return int32_t
     */
    int32_t SetDefaultSmsSlotId(int32_t slotId) override;

    /**
     * @brief GetDefaultSmsSlotId
     * Get the Default Sms Slot Id From SmsService
     * @return int32_t
     */
    int32_t GetDefaultSmsSlotId() override;

    /**
     * @brief SplitMessage
     * calculate Sms Message Split Segment count
     * @param message [in]
     * @return int32_t
     */
    int32_t SplitMessage(const std::u16string &message, std::vector<std::u16string> &splitMessage) override;

    /**
     * @brief GetSmsSegmentsInfo
     * calculate the Sms Message Segments Info
     * @param slotId [in]
     * @param message [in]
     * @param force7BitCode [in]
     * @param info [out]
     * @return Returns {@code 0} if get sms segments info
     */
    int32_t GetSmsSegmentsInfo(int32_t slotId, const std::u16string &message, bool force7BitCode,
        ISmsServiceInterface::SmsSegmentsInfo &segInfo) override;

    /**
     * Check Sms Is supported Ims newtwork
     * Hide this for inner system use
     * @param slotId Indicates the card slot index number, ranging from {@code 0} to the maximum card slot index number
     * supported by the device.
     * @param isSupported Whether ims SMS is supported.
     * @return Interface execution results.
     */
    int32_t IsImsSmsSupported(int32_t slotId, bool &isSupported) override;

    /**
     * @brief GetImsShortMessageFormat
     * Get the Ims Short Message Format 3gpp/3gpp2
     * Hide this for inner system use
     * @return int32_t
     */
    int32_t GetImsShortMessageFormat(std::u16string &format) override;

    /**
     * @brief HasSmsCapability
     * Check whether it is supported Sms Capability
     * @return true
     * @return false
     */
    bool HasSmsCapability() override;

    /**
     * create a short message
     * @param pdu Indicates pdu code,
     * @param specification Indicates 3gpp or 3gpp2
     * @param ShortMessage Indicates a short message object
     * @return Returns {@code 0} if CreateMessage success
     */
    int32_t CreateMessage(std::string pdu, std::string specification, ShortMessage &message) override;

    /**
     * mms base64 encode
     * @param src Indicates source string,
     * @param dest Indicates destination string
     * @return Returns {@code true} if encode success; returns {@code false} otherwise
     */
    bool GetBase64Encode(std::string src, std::string &dest) override;

    /**
     * mms base64 decode
     * @param src Indicates source string,
     * @param dest Indicates destination string
     * @return Returns {@code true} if decode success; returns {@code false} otherwise
     */
    bool GetBase64Decode(std::string src, std::string &dest) override;

    /**
     * Get Encode String
     * @param encodeString Indicates output string,
     * @param charset Indicates character set,
     * @param valLength Indicates input string length,
     * @param strEncodeString Indicates input string
     * @return Returns {@code true} if decode success; returns {@code false} otherwise
     */
    bool GetEncodeStringFunc(
        std::string &encodeString, uint32_t charset, uint32_t valLength, std::string strEncodeString) override;

private:
    static inline BrokerDelegator<SmsServiceProxy> delegator_;
};

class SmsServiceDeathRecipient : public IRemoteObject::DeathRecipient {
public:
    virtual void OnRemoteDied(const wptr<IRemoteObject> &remote);
    SmsServiceDeathRecipient();
    virtual ~SmsServiceDeathRecipient();
    static bool GotDeathRecipient();
    static bool gotDeathRecipient_;
};
} // namespace Telephony
} // namespace OHOS
#endif // SMS_SERVICE_PROXY_H
