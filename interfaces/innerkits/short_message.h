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

#ifndef SHORT_MESSAGE_H
#define SHORT_MESSAGE_H

#include <codecvt>
#include <locale>
#include <memory>

#include "parcel.h"

namespace OHOS {
namespace Telephony {
class ShortMessage : public Parcelable {
public:
    /**
     * @brief SmsMessageClass
     * Indicates the SMS type.
     * from 3GPP TS 23.038 V4.3.0 5 CBS Data Coding Scheme.
     */
    using SmsMessageClass = enum {
        /**
         * class0 Indicates an instant message, which is displayed immediately after being received.
         */
        SMS_INSTANT_MESSAGE = 0,
        /**
         * class1 Indicates an SMS message that can be stored on the device
         * or SIM card based on the storage status.
         */
        SMS_OPTIONAL_MESSAGE,
        /**
         * class2 Indicates an SMS message containing SIM card information,
         * which is to be stored in a SIM card.
         */
        SMS_SIM_MESSAGE,
        /**
         * class3 Indicates an SMS message to be forwarded to another device.
         */
        SMS_FORWARD_MESSAGE,
        /**
         * Indicates an unknown type.
         */
        SMS_CLASS_UNKNOWN,
    };

    /**
     * @brief SimMessageStatus
     * from 3GPP TS 51.011 V4.0.0 (2001-03) section 10.5.3 Parameter Definitions.
     */
    using SmsSimMessageStatus = enum {
        /**
         * status free space ON SIM.
         */
        SMS_SIM_MESSAGE_STATUS_FREE = 0,
        /**
         * REC READ received unread message.
         */
        SMS_SIM_MESSAGE_STATUS_READ = 1,
        /**
         * REC UNREAD received read message.
         */
        SMS_SIM_MESSAGE_STATUS_UNREAD = 3,
        /**
         * "STO SENT" stored unsent message (only applicable to SMs).
         */
        SMS_SIM_MESSAGE_STATUS_SENT = 5,
        /**
         * "STO UNSENT" stored sent message (only applicable to SMs).
         */
        SMS_SIM_MESSAGE_STATUS_UNSENT = 7,
    };

    /**
     * @brief Obtains the SMS message body.
     *
     * @return std::u16string returns the message body.
     */
    std::u16string GetVisibleMessageBody() const;

    /**
     * @brief Obtains the address of the sender, which is to be displayed on the UI.
     *
     * @return std::u16string returns the raw address.
     */
    std::u16string GetVisibleRawAddress() const;

    /**
     * @brief Obtains the SMS type.
     *
     * @return returns SMS type, {@link SmsMessageClass}.
     */
    SmsMessageClass GetMessageClass() const;

    /**
     * @brief Obtains the short message service center (SMSC) address.
     *
     * @param smscAddress SMS center address
     * @return Interface execution results.
     */
    int32_t GetScAddress(std::u16string &smscAddress) const;

    /**
     * @brief Obtains the SMSC timestamp.
     *
     * @return int64_t returns the SMSC timestamp.
     */
    int64_t GetScTimestamp() const;

    /**
     * @brief Checks whether the received SMS is a "replace short message".
     *
     * @return returns true if the received SMS is a "replace short message", false otherwise.
     */
    bool IsReplaceMessage() const;

    /**
     * @brief Indicates the SMS message status from the SMS-STATUS-REPORT message sent by the
     * Short Message Service Center (SMSC).
     *
     * @return int32_t returns the SMS message status from the SMS-STATUS-REPORT message.
     */
    int32_t GetStatus() const;

    /**
     * @brief Indicates whether the current message is SMS-STATUS-REPORT.
     *
     * @return Returns true if the current message is SMS-STATUS-REPORT, false otherwise.
     */
    bool IsSmsStatusReportMessage() const;

    /**
     * @brief Checks whether the received SMS contains "TP-Reply-Path"
     *
     * @return returns true if the received SMS contains "TP-Reply-Path", false otherwise.
     */
    bool HasReplyPath() const;

    /**
     * @brief Get the Icc Message Status object.
     *
     * @return returns from 3GPP TS 51.011 V4.0.0 (2001-03) section 10.5.3 Parameter Definitions.
     * {@link SmsSimMessageStatus}
     */
    SmsSimMessageStatus GetIccMessageStatus() const;

    /**
     * @brief Get the protocol id used for sending SMS messages.
     *
     * @return int32_t returns protocol id used for sending SMS messages
     */
    int32_t GetProtocolId() const;

    /**
     * @brief Get the protocol data unit.
     *
     * @return returns the pdu code
     */
    std::vector<unsigned char> GetPdu() const;

    /**
     * @brief Get the user data part.
     *
     * @return returns the user data of pdu
     */
    std::string GetRawUserData() const;

    /**
     * @brief Create a Message object
     * Creates an SMS message instance based on the
     * protocol data unit (PDU) and the specified SMS protocol.
     *
     * @param pdu Indicates pdu code.
     * @param specification Indicates 3gpp or 3gpp2.
     * @return Returns {@code 0} if CreateMessage success
     */
    static int32_t CreateMessage(
        std::vector<unsigned char> &pdu, std::u16string specification, ShortMessage &messageObj);

    /**
     * @brief Create a Icc Message object
     * Creates an SMS message instance based on the
     * ICC protocol data unit (PDU) and the specified SMS protocol
     *
     * @param pdu Indicates pdu code.
     * @param specification Indicates 3gpp or 3gpp2.
     * @param index Indicates the message index.
     * @return returns a ShortMessage object
     */
    static ShortMessage CreateIccMessage(std::vector<unsigned char> &pdu, std::string specification, int32_t index);

    /**
     * @brief GetIndexOnSim
     *
     * @return int32_t returns the index
     */
    int32_t GetIndexOnSim() const;

    ~ShortMessage() = default;
    ShortMessage() = default;
    virtual bool Marshalling(Parcel &parcel) const override;
    static ShortMessage UnMarshalling(Parcel &parcel);
    bool ReadFromParcel(Parcel &parcel);

public:
    static constexpr int MIN_ICC_PDU_LEN = 1;
    std::u16string visibleMessageBody_;
    std::u16string visibleRawAddress_;
    SmsMessageClass messageClass_ = SMS_CLASS_UNKNOWN;
    SmsSimMessageStatus simMessageStatus_ = SMS_SIM_MESSAGE_STATUS_FREE;
    std::u16string scAddress_;
    int64_t scTimestamp_ = 0;
    bool isReplaceMessage_ = false;
    int32_t status_ = -1;
    bool isSmsStatusReportMessage_ = false;
    bool hasReplyPath_ = false;
    int32_t protocolId_ = -1;
    std::vector<unsigned char> pdu_;
    std::string rawUserData_;
    int32_t indexOnSim_ = 0;
};
} // namespace Telephony
} // namespace OHOS
#endif