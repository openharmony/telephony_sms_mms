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

#ifndef CDMA_SMS_MESSAGE_H
#define CDMA_SMS_MESSAGE_H

#include "sms_base_message.h"

namespace OHOS {
namespace Telephony {
class CdmaSmsMessage : public SmsBaseMessage {
public:
    CdmaSmsMessage() = default;
    virtual ~CdmaSmsMessage() = default;
    virtual int GetProtocolId() const;
    virtual bool IsReplaceMessage();
    virtual bool IsCphsMwi() const;
    virtual bool IsWapPushMsg();
    virtual CdmaTransportMsgType GetTransMsgType() const;
    virtual int GetTransTeleService() const;
    virtual bool IsStatusReport() const;
    virtual int16_t GetDestPort() const;
    virtual bool IsBroadcastMsg() const;
    virtual bool AddUserDataHeader(const struct SmsUDH &header);
    virtual int8_t GetCMASCategory() const;
    virtual int8_t GetCMASResponseType() const;
    virtual int8_t GetCMASSeverity() const;
    virtual int8_t GetCMASUrgency() const;
    virtual int8_t GetCMASCertainty() const;
    virtual int8_t GetCMASMessageClass() const;
    virtual bool IsCMAS() const;
    virtual uint16_t GetMessageId() const;
    virtual int8_t GetFormat() const;
    virtual int8_t GetLanguage() const;
    virtual std::string GetCbInfo() const;
    virtual int8_t GetPriority() const;
    virtual bool IsEmergencyMsg() const;
    virtual uint16_t GetServiceCategoty() const;
    virtual uint8_t GetGeoScope() const;
    int64_t GetReceTime() const;

    virtual std::shared_ptr<SpecialSmsIndication> GetSpecialSmsInd();
    static std::shared_ptr<CdmaSmsMessage> CreateMessage(const std::string &pdu);

    std::unique_ptr<CdmaTransportMsg> CreateSubmitTransMsg(const std::string &dest, const std::string &sc,
        const std::string &text, bool bStatusReport, const DataCodingScheme codingScheme);
    std::unique_ptr<CdmaTransportMsg> CreateSubmitTransMsg(const std::string &dest, const std::string &sc, int32_t port,
        const uint8_t *data, uint32_t dataLen, bool bStatusReport);

private:
    SmsEncodingType CovertEncodingType(const DataCodingScheme &codingScheme);
    bool PduAnalysis(const std::string &pduHex);
    void AnalysisP2pMsg(const CdmaP2PMsg &p2pMsg);
    void AnalysisCbMsg(const CdmaBroadCastMsg &cbMsg);
    void AnalsisAckMsg(const CdmaAckMsg &ackMsg);
    void AnalsisDeliverMwi(const CdmaP2PMsg &p2pMsg);
    void AnalsisDeliverMsg(const TeleserviceDeliver &deliver);
    void AnalsisDeliverAck(const TeleserviceDeliverAck &deliverAck);
    void AnalsisSubmitReport(const TeleserviceDeliverReport &report);
    void AnalsisSubmitMsg(const TeleserviceSubmit &submit);
    void AnalsisUserData(const SmsTeleSvcUserData &userData);
    void AnalsisCMASMsg(const TeleserviceDeliver &deliver);
    void AnalsisHeader(const SmsTeleSvcUserData &userData);
    virtual int DecodeMessage(uint8_t *decodeData, unsigned int length, DataCodingScheme &codingType,
        const std::string &msgText, bool &bAbnormal, MSG_LANGUAGE_ID_T &langId);

private:
    uint16_t destPort_ = -1;
    std::unique_ptr<TransportAddr> address_;
    std::unique_ptr<SmsTeleSvcAddr> callbackNumber_;
    std::vector<struct SmsUDH> userHeaders_;
    int8_t category_ = static_cast<int8_t>(SmsCmaeCategory::RESERVED);
    int8_t responseType_ = static_cast<int8_t>(SmsCmaeResponseType::RESERVED);
    int8_t severity_ = static_cast<int8_t>(SmsCmaeSeverity::RESERVED);
    int8_t urgency_ = static_cast<int8_t>(SmsCmaeUrgency::IMMEDIATE);
    int8_t certainty_ = static_cast<int8_t>(SmsCmaeCertainty::RESERVED);
    int8_t messageClass_ = static_cast<int8_t>(SmsCmaeAlertHandle::RESERVED);
    int8_t priority_ = static_cast<int8_t>(SmsPriorityIndicator::NORMAL);
    bool isCmas_ = false;
    uint16_t messageId_;
    int8_t language_;
    uint16_t serviceCategory_;

    std::unique_ptr<struct CdmaTransportMsg> transMsg_;
    std::unique_ptr<struct CdmaTransportMsg> GreateTransMsg();
};
} // namespace Telephony
} // namespace OHOS
#endif
