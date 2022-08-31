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
static const int32_t NORMAL_SMS_BROADCAST_TIME = 7 * 1000;

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

void SmsHiSysEvent::SetSmsBroadcastStartTime()
{
    smsBroadcastStartTime_ =
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
            .count();
}

void SmsHiSysEvent::JudgingSmsBroadcastTimeOut(const int32_t slotId, const SmsMmsMessageType type)
{
    int64_t smsBroadcastEndTime =
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
            .count();
    if (smsBroadcastEndTime - smsBroadcastStartTime_ > NORMAL_SMS_BROADCAST_TIME) {
        WriteSmsReceiveFaultEvent(slotId, type, SmsMmsErrorCode::SMS_ERROR_BROADCAST_TIME_OUT,
            "smsbroadcast time out " + std::to_string(smsBroadcastEndTime - smsBroadcastStartTime_));
    }
}

void SmsHiSysEvent::SetCbBroadcastStartTime()
{
    cbBroadcastStartTime_ =
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
            .count();
}

void SmsHiSysEvent::JudgingCbBroadcastTimeOut(const int32_t slotId, const SmsMmsMessageType type)
{
    int64_t cbBroadcastEndTime =
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
            .count();
    if (cbBroadcastEndTime - cbBroadcastStartTime_ > NORMAL_SMS_BROADCAST_TIME) {
        WriteSmsReceiveFaultEvent(slotId, type, SmsMmsErrorCode::SMS_ERROR_BROADCAST_TIME_OUT,
            "cbbroadcast time out " + std::to_string(cbBroadcastEndTime - cbBroadcastStartTime_));
    }
}

void SmsHiSysEvent::SetWapPushBroadcastStartTime()
{
    wapPushBroadcastStartTime_ =
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
            .count();
}

void SmsHiSysEvent::JudgingWapPushBroadcastTimeOut(const int32_t slotId, const SmsMmsMessageType type)
{
    int64_t wapPushBroadcastEndTime =
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
            .count();
    if (wapPushBroadcastEndTime - wapPushBroadcastStartTime_ > NORMAL_SMS_BROADCAST_TIME) {
        WriteSmsReceiveFaultEvent(slotId, type, SmsMmsErrorCode::SMS_ERROR_BROADCAST_TIME_OUT,
            "wappushbroadcast time out " + std::to_string(wapPushBroadcastEndTime - wapPushBroadcastStartTime_));
    }
}
} // namespace Telephony
} // namespace OHOS
