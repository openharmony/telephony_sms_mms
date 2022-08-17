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

#include "sms_hisysevent.h"

namespace OHOS {
namespace Telephony {
// EVENT
static constexpr const char *SMS_RECEIVE_FAILED_EVENT = "SMS_RECEIVE_FAILED";
static constexpr const char *SMS_SEND_FAILED_EVENT = "SMS_SEND_FAILED";
static constexpr const char *SMS_SEND_EVENT = "SMS_SEND";
static constexpr const char *SMS_RECEIVE_EVENT = "SMS_RECEIVE";

// KEY
static constexpr const char *MODULE_KEY = "MODULE";
static constexpr const char *SLOT_ID_KEY = "SLOT_ID";
static constexpr const char *MSG_TYPE_KEY = "MSG_TYPE";
static constexpr const char *ERROR_TYPE_KEY = "ERROR_TYPE";
static constexpr const char *ERROR_MSG_KEY = "ERROR_MSG";

// VALUE
static constexpr const char *SMS_MMS_MODULE = "SMS_MMS";

void SmsHiSysEvent::WriteSmsSendBehaviorEvent(const int32_t slotId, const SmsMmsMessageType type)
{
    HiWriteBehaviorEvent(SMS_SEND_EVENT, SLOT_ID_KEY, slotId, MSG_TYPE_KEY, static_cast<int32_t>(type));
}

void SmsHiSysEvent::WriteSmsReceiveBehaviorEvent(const int32_t slotId, const SmsMmsMessageType type)
{
    HiWriteBehaviorEvent(SMS_RECEIVE_EVENT, SLOT_ID_KEY, slotId, MSG_TYPE_KEY, static_cast<int32_t>(type));
}

void SmsHiSysEvent::WriteSmsSendFaultEvent(
    const int32_t slotId, const SmsMmsMessageType type, const SmsMmsErrorCode errorCode, const std::string &desc)
{
    HiWriteFaultEvent(SMS_SEND_FAILED_EVENT, MODULE_KEY, SMS_MMS_MODULE, SLOT_ID_KEY, slotId, MSG_TYPE_KEY,
        static_cast<int32_t>(type), ERROR_TYPE_KEY, static_cast<int32_t>(errorCode), ERROR_MSG_KEY, desc);
}

void SmsHiSysEvent::WriteSmsReceiveFaultEvent(
    const int32_t slotId, const SmsMmsMessageType type, const SmsMmsErrorCode errorCode, const std::string &desc)
{
    HiWriteFaultEvent(SMS_RECEIVE_FAILED_EVENT, MODULE_KEY, SMS_MMS_MODULE, SLOT_ID_KEY, slotId, MSG_TYPE_KEY,
        static_cast<int32_t>(type), ERROR_TYPE_KEY, static_cast<int32_t>(errorCode), ERROR_MSG_KEY, desc);
}
} // namespace Telephony
} // namespace OHOS
