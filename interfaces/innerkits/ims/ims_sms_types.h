/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#ifndef TELEPHONY_IMS_SMS_TYPES_H
#define TELEPHONY_IMS_SMS_TYPES_H

#include "hril_sms_parcel.h"
#include "ims_core_service_types.h"

namespace OHOS {
namespace Telephony {
typedef enum {
    SMS_RADIO_TECH_3GPP = 1, /* 3GPP Technologies - GSM, WCDMA */
    SMS_RADIO_TECH_3GPP2 = 2 /* 3GPP2 Technologies - CDMA */
} SmsRadioTechnologyFamily;
struct ImsMessageInfo {
    int64_t refId;
    std::string smscPdu;
    std::string pdu;
    SmsRadioTechnologyFamily tech;
};
} // namespace Telephony
} // namespace OHOS

#endif // TELEPHONY_IMS_SMS_TYPES_H
