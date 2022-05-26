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
     */
    void SendMessage(int32_t slotId, const std::u16string desAddr, const std::u16string scAddr,
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
     * @param deliverCallback [in]
     */
    void SendMessage(int32_t slotId, const std::u16string desAddr, const std::u16string scAddr, uint16_t port,
        const uint8_t *data, uint16_t dataLen, const sptr<ISendShortMessageCallback> &sendCallback,
        const sptr<IDeliveryShortMessageCallback> &deliverCallback) override;

    /**
     * @brief SetSmscAddr
     * Sets the address for the Short Message Service Center (SMSC) based on a specified slot ID.
     * @param slotId [in]
     * @param scAddr [in]
     * @return true
     * @return false
     */
    bool SetSmscAddr(int32_t slotId, const std::u16string &scAddr) override;

    /**
     * @brief GetSmscAddr
     * Obtains the SMSC address based on a specified slot ID.
     * @param slotId [in]
     * @return std::u16string
     */
    std::u16string GetSmscAddr(int32_t slotId) override;

    /**
     * @brief AddSimMessage
     * Add a sms to sim card.
     * @param slotId [in]
     * @param smsc [in]
     * @param pdu [in]
     * @param status [in]
     * @return true
     * @return false
     */
    bool AddSimMessage(
        int32_t slotId, const std::u16string &smsc, const std::u16string &pdu, SimMessageStatus status) override;

    /**
     * @brief DelSimMessage
     * Delete a sms in the sim card.
     * @param slotId [in]
     * @param msgIndex [in]
     * @return true
     * @return false
     */
    bool DelSimMessage(int32_t slotId, uint32_t msgIndex) override;

    /**
     * @brief UpdateSimMessage
     * Update a sms in the sim card.
     * @param slotId [in]
     * @param msgIndex [in]
     * @param newStatus [in]
     * @param pdu [in]
     * @param smsc [in]
     * @return true
     * @return false
     */
    bool UpdateSimMessage(int32_t slotId, uint32_t msgIndex, SimMessageStatus newStatus, const std::u16string &pdu,
        const std::u16string &smsc) override;

    /**
     * @brief GetAllSimMessages
     * Get sim card all the sms.
     * @param slotId [in]
     * @return std::vector<ShortMessage>
     */
    std::vector<ShortMessage> GetAllSimMessages(int32_t slotId) override;

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
    bool SetCBConfig(int32_t slotId, bool enable, uint32_t fromMsgId, uint32_t toMsgId, uint8_t netType) override;

        /**
     * @brief SetImsSmsConfig
     * enable or disable IMS SMS.
     * @param slotId [in]
     * @param enable [in]
     * @return true: enable or disable Ims Sms success,
     * false: enable or disable Ims Sms fail
     */
    bool SetImsSmsConfig(int32_t slotId, int32_t enable) override;

    /**
     * @brief SetDefaultSmsSlotId
     * Set the Default Sms Slot Id To SmsService
     * @param slotId [in]
     * @return true
     * @return false
     */
    bool SetDefaultSmsSlotId(int32_t slotId) override;

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
     * @return std::vector<std::u16string>
     */
    std::vector<std::u16string> SplitMessage(const std::u16string &message) override;

    /**
     * @brief GetSmsSegmentsInfo
     * calculate the Sms Message Segments Info
     * @param slotId [in]
     * @param message [in]
     * @param force7BitCode [in]
     * @param info [out]
     * @return true
     * @return false
     */
    bool GetSmsSegmentsInfo(int32_t slotId, const std::u16string &message, bool force7BitCode,
        ISmsServiceInterface::SmsSegmentsInfo &segInfo) override;

    /**
     * @brief IsImsSmsSupported
     * Check Sms Is supported Ims newtwork
     * Hide this for inner system use
     * @return true
     * @return false
     */
    bool IsImsSmsSupported() override;

    /**
     * @brief GetImsShortMessageFormat
     * Get the Ims Short Message Format 3gpp/3gpp2
     * Hide this for inner system use
     * @return std::u16string
     */
    std::u16string GetImsShortMessageFormat() override;

    /**
     * @brief HasSmsCapability
     * Check whether it is supported Sms Capability
     * @return true
     * @return false
     */
    bool HasSmsCapability() override;

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
