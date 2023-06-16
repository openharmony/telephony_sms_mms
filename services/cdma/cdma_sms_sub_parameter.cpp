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

#include "cdma_sms_sub_parameter.h"

#include "gsm_sms_udata_codec.h"
#include "securec.h"
#include "sms_common_utils.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {

bool CdmaSmsSubParameter::IsInvalidPdu(SmsReadBuffer &pdu)
{
    uint8_t id = RESERVED;
    if (pdu.IsEmpty() || !pdu.ReadByte(id) || !pdu.ReadByte(len_) || id != id_) {
        TELEPHONY_LOGE("sub parameter: pdu is invalid[%{public}d]: [%{public}d]", id_, id);
        return true;
    }
    return false;
}

CdmaSmsBaseParameter::CdmaSmsBaseParameter(uint8_t id, uint8_t &data) : data_(data)
{
    id_ = id;
    len_ = sizeof(id_);
}

bool CdmaSmsBaseParameter::Encode(SmsWriteBuffer &pdu)
{
    if (pdu.IsEmpty()) {
        TELEPHONY_LOGE("pdu is empty");
        return false;
    }
    if (pdu.WriteByte(id_) && pdu.WriteByte(len_) && pdu.WriteByte(data_)) {
        return true;
    }
    TELEPHONY_LOGE("encode error");
    return false;
}

bool CdmaSmsBaseParameter::Decode(SmsReadBuffer &pdu)
{
    if (IsInvalidPdu(pdu)) {
        TELEPHONY_LOGE("invalid pdu");
        return false;
    }
    return pdu.ReadByte(data_);
}

CdmaSmsReservedParameter::CdmaSmsReservedParameter(uint8_t id)
{
    id_ = id;
}

bool CdmaSmsReservedParameter::Encode(SmsWriteBuffer &pdu)
{
    if (pdu.IsEmpty()) {
        TELEPHONY_LOGE("pdu is empty");
        return false;
    }
    return true;
}

bool CdmaSmsReservedParameter::Decode(SmsReadBuffer &pdu)
{
    if (IsInvalidPdu(pdu)) {
        TELEPHONY_LOGE("invalid pdu");
        return false;
    }
    pdu.MoveForward(len_);
    return true;
}

} // namespace Telephony
} // namespace OHOS
