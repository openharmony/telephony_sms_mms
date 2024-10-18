/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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


#ifndef SMS_MMS_COMMON_H
#define SMS_MMS_COMMON_H

#include "singleton.h"
namespace OHOS {
namespace Telephony {
namespace SmsMmsCommonData {
    static constexpr const char *SMS_MMS_INFO_MSG_STATE_SUCCEED = "0";
    static constexpr const char *SMS_MMS_INFO_MSG_STATE_FAILED = "2";
    static constexpr const char *SMS_MMS_INFO_SMS_TYPE = "0";
    static constexpr const char *SMS_MMS_INFO_MMS_TYPE = "1";
    static constexpr const char *SMS_MMS_SENT_RESULT_NOTIFY = "SMS_MMS_SENT_RESULT_NOTIFY";
    static constexpr const char *MESSAGE_STATUS_CHANGE_NOTIFY = "MESSAGE_STATUS_CHANGE_NOTIFY";
    static constexpr const char *MSG_ID = "msgId";
    static constexpr const char *MSG_TYPE = "msgType";
    static constexpr const char *MSG_STATE = "msgState";
    static constexpr uint16_t SMS_MSM_STATUS_SENDING = 1;
    static constexpr  uint16_t SMS_MSM_STATUS_FAILED = 2;
    static constexpr  uint16_t SMS_MSM_STATUS_SUCCEED = 0;
    constexpr static uint16_t SMS_MSM_TYPE_SMS = 0;
    constexpr static uint16_t SMS_MSM_TYPE_MMS = 1;
} // namespace of SmsMmsCommonData

class SmsMmsCommon {
    DECLARE_DELAYED_SINGLETON(SmsMmsCommon)

public:
    void SendBroadcast(uint16_t id, std::string notify, std::string stauts, std::string type);
};
} // namespace Telephony
} // namespace OHOS
#endif // SMS_MMS_COMMON_H