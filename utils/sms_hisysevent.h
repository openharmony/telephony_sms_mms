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

#ifndef SMS_HISYSEVENT_H
#define SMS_HISYSEVENT_H

#include <string>

#include "telephony_hisysevent.h"

namespace OHOS {
namespace Telephony {
enum class SmsMmsMessageType {
    SMS_SHORT_MESSAGE,
    MMS_SHORT_MESSAGE,
    CELL_BROAD_CAST,
    WAP_PUSH,
};

class SmsHiSysEvent : public TelephonyHiSysEvent {
public:
    static void WriteSmsSendBehaviorEvent(const int32_t slotId, const SmsMmsMessageType type);
    static void WriteSmsReceiveBehaviorEvent(const int32_t slotId, const SmsMmsMessageType type);
    static void WriteSmsSendFaultEvent(
        const int32_t slotId, const SmsMmsMessageType type, const SmsMmsErrorCode errorCode, const std::string &desc);
    static void WriteSmsReceiveFaultEvent(
        const int32_t slotId, const SmsMmsMessageType type, const SmsMmsErrorCode errorCode, const std::string &desc);
    void SetSmsBroadcastStartTime();
    void JudgingSmsBroadcastTimeOut(const int32_t slotId, const SmsMmsMessageType type);
    void SetCbBroadcastStartTime();
    void JudgingCbBroadcastTimeOut(const int32_t slotId, const SmsMmsMessageType type);
    void SetWapPushBroadcastStartTime();
    void JudgingWapPushBroadcastTimeOut(const int32_t slotId, const SmsMmsMessageType type);

private:
    int64_t smsBroadcastStartTime_ = 0L;
    int64_t cbBroadcastStartTime_ = 0L;
    int64_t wapPushBroadcastStartTime_ = 0L;
};
} // namespace Telephony
} // namespace OHOS

#endif // SMS_HISYSEVENT_H
