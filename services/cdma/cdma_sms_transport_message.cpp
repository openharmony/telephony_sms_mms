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

#include "cdma_sms_transport_message.h"

#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {

CdmaSmsTransportMessage::~CdmaSmsTransportMessage() {}

bool CdmaSmsTransportMessage::Encode(SmsWriteBuffer &pdu)
{
    TELEPHONY_LOGW("encode not support");
    return false;
}

bool CdmaSmsTransportMessage::Decode(SmsReadBuffer &pdu)
{
    TELEPHONY_LOGW("decode not support");
    return false;
}

bool CdmaSmsTransportMessage::IsEmpty()
{
    return true;
}

std::unique_ptr<CdmaSmsTransportMessage> CdmaSmsTransportMessage::CreateTransportMessage(CdmaTransportMsg &msg)
{
    TELEPHONY_LOGW("create not support");
    return nullptr;
}

std::unique_ptr<CdmaSmsTransportMessage> CdmaSmsTransportMessage::CreateTransportMessage(
    CdmaTransportMsg &msg, SmsReadBuffer &pdu)
{
    TELEPHONY_LOGW("create not support");
    return nullptr;
}

} // namespace Telephony
} // namespace OHOS
