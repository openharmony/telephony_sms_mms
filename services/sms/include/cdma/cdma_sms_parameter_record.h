/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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

#ifndef CDMA_SMS_PARAMETER_RECORD_H
#define CDMA_SMS_PARAMETER_RECORD_H

#include "cdma_sms_common.h"
#include "cdma_sms_teleservice_message.h"
#include "sms_pdu_buffer.h"

namespace OHOS {
namespace Telephony {

class CdmaSmsParameterRecord {
public:
    virtual ~CdmaSmsParameterRecord() = default;
    virtual bool Encode(SmsWriteBuffer &pdu) = 0;
    virtual bool Decode(SmsReadBuffer &pdu) = 0;

protected:
    inline bool IsInvalidPdu(SmsReadBuffer &pdu);

public:
    uint8_t id_ { RESERVED };
    uint8_t len_ { 0 };

    enum ParameterId : uint8_t {
        TELESERVICE_ID = 0x00,
        SERVICE_CATEGORY = 0x01,
        ORG_ADDRESS = 0x02,
        ORG_SUB_ADDRESS = 0x03,
        DEST_ADDRESS = 0x04,
        DEST_SUB_ADDRESS = 0x05,
        BEARER_REPLY_OPTION = 0x06,
        CAUSE_CODES = 0x07,
        BEARER_DATA = 0x08,
        RESERVED
    };
};

class CdmaSmsTeleserviceId : public CdmaSmsParameterRecord {
public:
    explicit CdmaSmsTeleserviceId(uint16_t &id);
    bool Encode(SmsWriteBuffer &pdu) override;
    bool Decode(SmsReadBuffer &pdu) override;

private:
    uint16_t &teleserviceId_;
};

class CdmaSmsServiceCategory : public CdmaSmsParameterRecord {
public:
    explicit CdmaSmsServiceCategory(uint16_t &cat);
    bool Encode(SmsWriteBuffer &pdu) override;
    bool Decode(SmsReadBuffer &pdu) override;

private:
    uint16_t &serviceCat_;
};

class CdmaSmsBearerReply : public CdmaSmsParameterRecord {
public:
    explicit CdmaSmsBearerReply(uint8_t &replySeq);
    bool Encode(SmsWriteBuffer &pdu) override;
    bool Decode(SmsReadBuffer &pdu) override;

private:
    uint8_t &replySeq_;
};

class CdmaSmsCauseCodes : public CdmaSmsParameterRecord {
public:
    explicit CdmaSmsCauseCodes(TransportCauseCode &code);
    bool Encode(SmsWriteBuffer &pdu) override;
    bool Decode(SmsReadBuffer &pdu) override;

private:
    TransportCauseCode &code_;
    enum ErrorClass : uint8_t { NONE = 0b00, TEMPORARY = 0b10, PERMANENT = 0b11 };
};

class CdmaSmsAddressParameter : public CdmaSmsParameterRecord {
public:
    CdmaSmsAddressParameter(TransportAddr &address, uint8_t id);
    bool Encode(SmsWriteBuffer &pdu) override;
    bool Decode(SmsReadBuffer &pdu) override;

private:
    bool EncodeAddress(SmsWriteBuffer &pdu);
    bool DecodeAddress(SmsReadBuffer &pdu);

private:
    TransportAddr &address_;
    bool isInvalid_ { false };
};

class CdmaSmsSubaddress : public CdmaSmsParameterRecord {
public:
    CdmaSmsSubaddress(TransportSubAddr &address, uint8_t id);
    bool Encode(SmsWriteBuffer &pdu) override;
    bool Decode(SmsReadBuffer &pdu) override;

private:
    TransportSubAddr &address_;
    bool isInvalid_ { false };
    enum SubaddressType : uint8_t { NSAP = 0b000, USER = 0b001, RESERVED };
};

class CdmaSmsBearerData : public CdmaSmsParameterRecord {
public:
    explicit CdmaSmsBearerData(CdmaTeleserviceMsg &msg);
    CdmaSmsBearerData(CdmaTeleserviceMsg &msg, SmsReadBuffer &pdu, bool isCMAS = false);
    bool Encode(SmsWriteBuffer &pdu) override;
    bool Decode(SmsReadBuffer &pdu) override;

private:
    std::unique_ptr<CdmaSmsTeleserviceMessage> teleserviceMessage_ { nullptr };
};

} // namespace Telephony
} // namespace OHOS
#endif
