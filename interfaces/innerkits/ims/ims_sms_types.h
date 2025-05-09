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

#include "tel_ril_sms_parcel.h"
#include "ims_core_service_types.h"

namespace OHOS {
namespace Telephony {

/**
 * @brief Enumerates the format of SMS over IMS.
 */
typedef enum {
    /**
     * 3GPP Technologies - GSM, WCDMA.
     */
    SMS_RADIO_TECH_3GPP = 1,

    /**
     * 3GPP2 Technologies - CDMA.
     */
    SMS_RADIO_TECH_3GPP2 = 2
} SmsRadioTechnologyFamily;

/**
 * @brief Used to save the information of IMS message.
 */
struct ImsMessageInfo {
    /**
     * Indicates the ID of IMS message.
     */
    int64_t refId = 0;

    /**
     * Indicates the SMSC address in PDU form.
     */
    std::string smscPdu = "";

    /**
     * Indicates the protocol data unit of IMS message.
     */
    std::string pdu = "";

    /**
     * Indicates the SMS over IMS format, 3GPP or 3GPP2.
     */
    SmsRadioTechnologyFamily tech = SmsRadioTechnologyFamily::SMS_RADIO_TECH_3GPP;
};
} // namespace Telephony
} // namespace OHOS

#endif // TELEPHONY_IMS_SMS_TYPES_H
