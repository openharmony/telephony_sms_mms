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

#ifndef CDMA_SMS_TRANSPORT_MESSAGE_H
#define CDMA_SMS_TRANSPORT_MESSAGE_H

#include <memory>
#include <vector>

#include "cdma_sms_common.h"
#include "cdma_sms_parameter_record.h"
#include "sms_pdu_buffer.h"

namespace OHOS {
namespace Telephony {

class CdmaSmsTransportMessage {
public:
    virtual ~CdmaSmsTransportMessage();
    virtual bool Encode(SmsWriteBuffer &pdu);
    virtual bool Decode(SmsReadBuffer &pdu);
    virtual bool IsEmpty();

    static std::unique_ptr<CdmaSmsTransportMessage> CreateTransportMessage(CdmaTransportMsg &msg);
    static std::unique_ptr<CdmaSmsTransportMessage> CreateTransportMessage(CdmaTransportMsg &msg, SmsReadBuffer &pdu);

protected:
    enum MessageType : uint8_t { P2P = 0x00, BROADCAST = 0x01, ACK = 0x02, RESERVED };
    uint8_t type_ { RESERVED };
    std::vector<std::unique_ptr<CdmaSmsParameterRecord>> parameter_;
};

class CdmaSmsP2pMessage : public CdmaSmsTransportMessage {
public:
    explicit CdmaSmsP2pMessage(CdmaP2PMsg &msg);
    CdmaSmsP2pMessage(CdmaP2PMsg &msg, SmsReadBuffer &pdu);
};

class CdmaSmsBroadcastMessage : public CdmaSmsTransportMessage {
public:
    explicit CdmaSmsBroadcastMessage(CdmaBroadCastMsg &msg);
    CdmaSmsBroadcastMessage(CdmaBroadCastMsg &msg, SmsReadBuffer &pdu);
};

class CdmaSmsAckMessage : public CdmaSmsTransportMessage {
public:
    explicit CdmaSmsAckMessage(CdmaAckMsg &msg);
    CdmaSmsAckMessage(CdmaAckMsg &msg, SmsReadBuffer &pdu);
};

} // namespace Telephony
} // namespace OHOS
#endif
