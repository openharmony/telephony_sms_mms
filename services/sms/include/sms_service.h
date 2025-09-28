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

#ifndef SMS_SERVICE_H
#define SMS_SERVICE_H

#include <memory>

#include "sms_interface_stub.h"
#include "sms_state_handler.h"
#include "system_ability.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace Telephony {
enum ServiceRunningState { STATE_NOT_START, STATE_RUNNING };

class SmsService : public SystemAbility, public SmsInterfaceStub, public std::enable_shared_from_this<SmsService> {
    DECLARE_DELAYED_SINGLETON(SmsService)
    DECLARE_SYSTEM_ABILITY(SmsService) // necessary
public:
    void OnStart() override;
    void OnStop() override;
    int32_t Dump(std::int32_t fd, const std::vector<std::u16string> &args) override;
    std::string GetBindTime();
    void InsertSessionAndDetail(int32_t slotId, const std::string &telephone, const std::string &text,
        int32_t &dataBaseId);

    /**
     * Sends a text Type SMS message.
     * @param slotId Indicates the card slot index number,
     * ranging from {@code 0} to the maximum card slot index number supported by
     * the device
     * @param desAddr Indicates the destination address
     * @param scAddr Indicates the sms center address
     * @param text Indicates sms content
     * @param sendCallback Indicates callback for send out
     * @param deliverCallback Indicates callback for delivery to destination user
     * @return Returns {@code 0} if send message success.
     */
    int32_t SendMessage(int32_t slotId, const std::u16string desAddr, const std::u16string scAddr,
        const std::u16string text, const sptr<ISendShortMessageCallback> &sendCallback,
        const sptr<IDeliveryShortMessageCallback> &deliveryCallback, bool isMmsApp = true) override;

    /**
     * Sends a text Type SMS message withot save to database.
     * @param slotId Indicates the card slot index number,
     * ranging from {@code 0} to the maximum card slot index number supported by
     * the device
     * @param desAddr Indicates the destination address
     * @param scAddr Indicates the sms center address
     * @param text Indicates sms content
     * @param sendCallback Indicates callback for send out
     * @param deliverCallback Indicates callback for delivery to destination user
     * @return Returns {@code 0} if send message success.
     */
    int32_t SendMessageWithoutSave(int32_t slotId, const std::u16string desAddr, const std::u16string scAddr,
        const std::u16string text, const sptr<ISendShortMessageCallback> &sendCallback,
        const sptr<IDeliveryShortMessageCallback> &deliveryCallback) override;

    /**
     * Sends a data Type SMS message.
     * @param slotId Indicates the card slot index number,
     * ranging from {@code 0} to the maximum card slot index number supported by
     * the device
     * @param desAddr Indicates the destination address
     * @param scAddr Indicates the sms center address
     * @param port Indicates the port of data sms
     * @param data Indicates the array of data sms
     * @param dataLen Indicates the array length of data sms
     * @param sendCallback Indicates callback for send out
     * @param deliverCallback Indicates callback for delivery to destination user
     * @return Returns {@code 0} if send message success.
     */
    int32_t SendMessage(int32_t slotId, const std::u16string desAddr, const std::u16string scAddr, uint16_t port,
        const uint8_t *data, uint16_t dataLen, const sptr<ISendShortMessageCallback> &sendCallback,
        const sptr<IDeliveryShortMessageCallback> &deliveryCallback) override;

    /**
     * Sets the address for the Short Message Service Center (SMSC) based on a
     * specified slot ID
     * @param slotId Indicates the card slot index number,
     * ranging from {@code 0} to the maximum card slot index number supported by
     * the device
     * @param scAddr Indicates the sms center address
     * @return Returns {@code 0} if set smsc success
     */
    int32_t SetSmscAddr(int32_t slotId, const std::u16string &scAddr) override;

    /**
     * Obtains the SMSC address based on a specified slot ID.
     * @param slotId Indicates the card slot index number,
     * ranging from {@code 0} to the maximum card slot index number supported by
     * the device
     * @param smscAddress [out]
     * @return Returns {@code 0} if get smsc success.
     */
    int32_t GetSmscAddr(int32_t slotId, std::u16string &smscAddress) override;

    /**
     * Add a sms to sim card.
     * @param slotId Indicates the card slot index number,
     * ranging from {@code 0} to the maximum card slot index number supported by
     * the device
     * @param smsc Indicates the sms center address
     * @param pdu Indicates the sms pdu data
     * @param status Indicates the sms status, read or not
     * @return Returns {@code true} if add sim success; returns {@code false}
     */
    int32_t AddSimMessage(
        int32_t slotId, const std::u16string &smsc, const std::u16string &pdu, SimMessageStatus status) override;

    /**
     * Delete a sms in the sim card.
     * @param slotId Indicates the card slot index number,
     * ranging from {@code 0} to the maximum card slot index number supported by
     * the device
     * @param msgIndex Indicates the sim sms index in sim card
     * @return Returns {@code true} if delete sim success; returns {@code false}
     */
    int32_t DelSimMessage(int32_t slotId, uint32_t msgIndex) override;

    /**
     * Update a sms in the sim card.
     * @param slotId Indicates the card slot index number,
     * ranging from {@code 0} to the maximum card slot index number supported by
     * the device
     * @param msgIndex Indicates the sim sms index in sim card
     * @param newStatusIndicates the sms status, read or not
     * @param pdu Indicates the sms pdu data
     * @param smsc Indicates the sms center address
     * @return Returns {@code 0} if update sim success
     */
    int32_t UpdateSimMessage(int32_t slotId, uint32_t msgIndex, SimMessageStatus newStatus, const std::u16string &pdu,
        const std::u16string &smsc) override;

    /**
     * Get sim card all the sms.
     * @param slotId Indicates the card slot index number,
     * ranging from {@code 0} to the maximum card slot index number supported by
     * the device
     * @param message Indicates all SMS messages of sim card
     * @return Returns {@code 0} if get all sim messages success
     */
    int32_t GetAllSimMessages(int32_t slotId, std::vector<ShortMessage> &message) override;

    /**
     * Configure a cell broadcast in a certain band range.
     * @param slotId [in]
     * @param enable [in]
     * @param fromMsgId [in]
     * @param toMsgId [in]
     * @param netType [in]
     * @return Returns {@code 0} if set CB config success
     */
    int32_t SetCBConfig(int32_t slotId, bool enable, uint32_t fromMsgId, uint32_t toMsgId, uint8_t netType) override;

    /**
     * Configure cell broadcast list in some certain band range.
     * @param slotId [in]
     * @param messageIds [in]
     * @param ranType [in]
     * @return Returns {@code 0} if set CB config list success
     */
    int32_t SetCBConfigList(int32_t slotId, const std::vector<int32_t>& messageIds, int32_t ranType) override;

    /**
     * enable or disable IMS SMS.
     * @param slotId Indicates the card slot index number,
     * ranging from {@code 0} to the maximum card slot index number supported by
     * the device
     * @param enable Indicates enable or disable Ims sms
     * ranging {@code 0} disable Ims sms {@code 1} enable Ims sms
     * @return Returns {@code true} if enable or disable Ims Sms success; returns
     * {@code false} otherwise
     */
    bool SetImsSmsConfig(int32_t slotId, int32_t enable) override;

    /**
     * Set the Default Sms Slot Id To SmsService
     * @param slotId Indicates the card slot index number,
     * ranging from {@code 0} to the maximum card slot index number supported by
     * the device
     * @return Returns {@code 0} if set default sms slot id success
     */
    int32_t SetDefaultSmsSlotId(int32_t slotId) override;

    /**
     * Get the Default Sms Slot Id From SmsService
     * @return Returns default sms slot id
     */
    int32_t GetDefaultSmsSlotId() override;

    /**
     * @brief GetDefaultSmsSlotId
     * Get the Default Sms Sim Id From SmsService
     * @return int32_t
     */
    int32_t GetDefaultSmsSimId(int32_t &simId) override;

    /**
     * @brief SplitMessage
     * calculate Sms Message Split Segment count
     * @param Indicates input message
     * @param splitMessage Indicates the split information
     * @return Returns {@code 0} if split message success
     */
    int32_t SplitMessage(const std::u16string &message, std::vector<std::u16string> &splitMessage) override;

    /**
     * calculate the Sms Message Segments Info
     * @param slotId Indicates the card slot index number,
     * ranging from {@code 0} to the maximum card slot index number supported by
     * the device
     * @param message Indicates input message
     * @param force7BitCode Indicates sms encode type, 7bit or not
     * @param info Indicates output sms segment
     * @return Returns {@code 0} if get sms segments info
     */
    int32_t GetSmsSegmentsInfo(int32_t slotId, const std::u16string &message, bool force7BitCode,
        ISmsServiceInterface::SmsSegmentsInfo &info) override;

    /**
     * Check Sms Is supported Ims newtwork
     * Hide this for inner system use
     * @param slotId Indicates the card slot index number,
     * ranging from {@code 0} to the maximum card slot index number supported by
     * the device
     * @param isSupported Whether ims SMS is supported
     * @return Returns {@code 0} if successful
     */
    int32_t IsImsSmsSupported(int32_t slotId, bool &isSupported) override;

    /**
     * Get the Ims Short Message Format 3gpp/3gpp2
     * Hide this for inner system use
     * @return int32_t
     */
    int32_t GetImsShortMessageFormat(std::u16string &format) override;

    /**
     * Check whether it is supported Sms Capability
     * @return Returns {@code true} if support sms; returns {@code false}
     */
    bool HasSmsCapability() override;

    /**
     * @brief GetServiceRunningState
     * Get service running state
     * @return ServiceRunningState
     */
    int32_t GetServiceRunningState();

    /**
     * @brief GetSpendTime
     * Get service start spend time
     * @return Spend time
     */
    int64_t GetSpendTime();

    /**
     * @brief GetEndTime
     * Get service start finish time
     * @return Spend time
     */
    int64_t GetEndTime();

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
        const std::u16string &uaprof, int64_t &time, bool isMmsApp = false) override;

    /**
     * Check send mms permissions.
     * @return Returns {@code 0} if send mms success; returns {@code false} otherwise
     */
    int32_t CheckMmsPermissions();

    /**
     * Query send mms database id.
     * @param slotId Indicates the card slot index number,
     * ranging from {@code 0} to the maximum card slot index number supported by
     * the device
     * @param time Indicates tiemstamp of hap call NAPI function
     * @return Returns id of mms in sqlite
     */
    int32_t QueryMmsDatabaseId(int32_t slotId, int64_t time);

    /**
     * Service after Send Mms.
     * @param slotId Indicates the card slot index number,
     * ranging from {@code 0} to the maximum card slot index number supported by
     * the device
     * @param time Indicates tiemstamp of hap call NAPI function
     * @param dataBaseId id of mms in sqlite
     * @param sessionBucket  object of DataShareValuesBucket to upate sqlite
     * @param sendStatus status of mms sent
     * @return Returns {@code 0} if send mms success; returns {@code false} otherwise
     */
    void ServiceAfterSendMmsComplete(int32_t slotId, int64_t &time, int32_t &dataBaseId,
        DataShare::DataShareValuesBucket &sessionBucket, std::string  &sendStatus);

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
    int32_t DownloadMms(int32_t slotId, const std::u16string &mmsc, std::u16string &data,
        const std::u16string &ua, const std::u16string &uaprof) override;
    int32_t OnRilAdapterHostDied(int32_t slotId);

private:
    bool Init();
    void WaitCoreServiceToInit();
    bool CheckSmsPermission(const sptr<ISendShortMessageCallback> &sendCallback);
    bool ValidDestinationAddress(std::string desAddr);
    void TrimSmscAddr(std::string &sca);
    bool CheckSimMessageIndexValid(int32_t slotId, uint32_t msgIndex);
    void InsertSmsMmsInfo(int32_t slotId, uint16_t sessionId, const std::string &number,
        const std::string &text, int32_t &dataBaseId);
    bool InsertSession(bool isNewSession, uint16_t messageCount, const std::string &number, const std::string &text);
    bool QuerySessionByTelephone(const std::string &telephone, uint16_t &sessionId, uint16_t &messageCount);
    void UpdateSmsContact(const std::string &address);
    bool IsInfoMsg(const std::string &telephone);
    void UpdatePredicatesByPhoneNum(DataShare::DataSharePredicates &predicates, const std::string &phoneNum);
    bool IsValidCBRangeList(const std::vector<int32_t>& messageIds);

private:
    int64_t bindTime_ = 0;
    int64_t endTime_ = 0;
    int64_t spendTime_ = 0;
    bool registerToService_ = false;
    static constexpr const char *SMS_MMS_INFO_MSG_STATE_SENDING = "1";
    ServiceRunningState state_ = ServiceRunningState::STATE_NOT_START;
    std::shared_ptr<SmsStateHandler> smsStateHandler_;
    const std::string SMS_MMS_INFO = "datashare:///com.ohos.smsmmsability/sms_mms/sms_mms_info";
    const std::string SMS_SESSION = "datashare:///com.ohos.smsmmsability/sms_mms/session";
};
} // namespace Telephony
} // namespace OHOS
#endif