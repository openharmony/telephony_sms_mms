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

#include "cdma_sms_parameter_record.h"

namespace OHOS {
namespace Telephony {
bool CdmaSmsParameterRecord::IsInvalidPdu(SmsReadBuffer &pdu)
{
    return false;
}

CdmaSmsTeleserviceId::CdmaSmsTeleserviceId(uint16_t &id) {}

bool CdmaSmsTeleserviceId::Encode(SmsWriteBuffer &pdu)
{
    return false;
}

bool CdmaSmsTeleserviceId::Decode(SmsReadBuffer &pdu)
{
    return false;
}

CdmaSmsServiceCategory::CdmaSmsServiceCategory(uint16_t &cat) {}

bool CdmaSmsServiceCategory::Encode(SmsWriteBuffer &pdu)
{
    return false;
}

bool CdmaSmsServiceCategory::Decode(SmsReadBuffer &pdu)
{
    return false;
}

CdmaSmsBearerReply::CdmaSmsBearerReply(uint8_t &replySeq) {}

bool CdmaSmsBearerReply::Encode(SmsWriteBuffer &pdu)
{
    return false;
}

bool CdmaSmsBearerReply::Decode(SmsReadBuffer &pdu)
{
    return false;
}

CdmaSmsCauseCodes::CdmaSmsCauseCodes(TransportCauseCode &code) {}

bool CdmaSmsCauseCodes::Encode(SmsWriteBuffer &pdu)
{
    return false;
}

bool CdmaSmsCauseCodes::Decode(SmsReadBuffer &pdu)
{
    return false;
}

CdmaSmsAddressParameter::CdmaSmsAddressParameter(TransportAddr &address, uint8_t id) {}

bool CdmaSmsAddressParameter::Encode(SmsWriteBuffer &pdu)
{
    return false;
}

bool CdmaSmsAddressParameter::Decode(SmsReadBuffer &pdu)
{
    return false;
}

CdmaSmsSubaddress::CdmaSmsSubaddress(TransportSubAddr &address, uint8_t id) {}

bool CdmaSmsSubaddress::Encode(SmsWriteBuffer &pdu)
{
    return false;
}

bool CdmaSmsSubaddress::Decode(SmsReadBuffer &pdu)
{
    return false;
}

CdmaSmsBearerData::CdmaSmsBearerData(CdmaTeleserviceMsg &msg) {}

CdmaSmsBearerData::CdmaSmsBearerData(CdmaTeleserviceMsg &msg, SmsReadBuffer &pdu, bool isCMAS) {}

bool CdmaSmsBearerData::Encode(SmsWriteBuffer &pdu)
{
    return false;
}

bool CdmaSmsBearerData::Decode(SmsReadBuffer &pdu)
{
    return false;
}

} // namespace Telephony
} // namespace OHOS
