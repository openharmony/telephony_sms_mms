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

#ifndef CDMA_SMS_TELESERVICE_MESSAGE_H
#define CDMA_SMS_TELESERVICE_MESSAGE_H

#include <vector>

#include "cdma_sms_common.h"
#include "cdma_sms_sub_parameter.h"
#include "sms_pdu_buffer.h"

namespace OHOS {
namespace Telephony {

class CdmaSmsTeleserviceMessage {
public:
    virtual ~CdmaSmsTeleserviceMessage();
    virtual bool Encode(SmsWriteBuffer &pdu);
    virtual bool Decode(SmsReadBuffer &pdu);
    static uint8_t GetMessageType(SmsReadBuffer &pdu);

public:
    enum MessageType : uint8_t {
        RESERVED = 0x00,
        DELIVER,
        SUBMIT,
        CANCELLATION,
        DELIVERY_ACK,
        USER_ACK,
        READ_ACK,
        DELIVER_REPORT,
        SUBMIT_REPORT,
    };

protected:
    uint8_t type_ { RESERVED };
    std::vector<std::unique_ptr<CdmaSmsSubParameter>> parameter_;
};

class CdmaSmsSubmitMessage : public CdmaSmsTeleserviceMessage {
public:
    explicit CdmaSmsSubmitMessage(TeleserviceSubmit &msg);
    CdmaSmsSubmitMessage(TeleserviceSubmit &msg, SmsReadBuffer &pdu);
};

class CdmaSmsCancelMessage : public CdmaSmsTeleserviceMessage {
public:
    explicit CdmaSmsCancelMessage(TeleserviceCancel &msg);
};

class CdmaSmsDeliverReport : public CdmaSmsTeleserviceMessage {
public:
    explicit CdmaSmsDeliverReport(TeleserviceDeliverReport &msg);
};

class CdmaSmsDeliverMessage : public CdmaSmsTeleserviceMessage {
public:
    CdmaSmsDeliverMessage(TeleserviceDeliver &msg, SmsReadBuffer &pdu, bool isCMAS = false);
private:
    void PushParameter(uint8_t id, TeleserviceDeliver &msg);
};

class CdmaSmsDeliveryAck : public CdmaSmsTeleserviceMessage {
public:
    CdmaSmsDeliveryAck(TeleserviceDeliverAck &msg, SmsReadBuffer &pdu);
};

class CdmaSmsUserAck : public CdmaSmsTeleserviceMessage {
public:
    CdmaSmsUserAck(TeleserviceUserAck &msg, SmsReadBuffer &pdu);
};

class CdmaSmsReadAck : public CdmaSmsTeleserviceMessage {
public:
    CdmaSmsReadAck(TeleserviceReadAck &msg, SmsReadBuffer &pdu);
};

} // namespace Telephony
} // namespace OHOS
#endif
