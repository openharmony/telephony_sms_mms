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

#include "cdma_sms_types.h"
#include "sms_pdu_buffer.h"

namespace OHOS {
namespace Telephony {

class CdmaSmsTransportMessage {
public:
    virtual ~CdmaSmsTransportMessage();
    virtual bool Encode(SmsWriteBuffer &pdu);
    virtual bool Decode(SmsReadBuffer &pdu);
    virtual bool IsEmpty();

    static std::unique_ptr<CdmaSmsTransportMessage> CreateTransportMessage(SmsTransMsg &msg);
    static std::unique_ptr<CdmaSmsTransportMessage> CreateTransportMessage(SmsTransMsg &msg, SmsReadBuffer &pdu);
};

} // namespace Telephony
} // namespace OHOS
#endif
