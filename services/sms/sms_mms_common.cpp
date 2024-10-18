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


#include "sms_mms_common.h"

#include "sms_mms_data.h"
#include "common_event_manager.h"
#include "common_event_support.h"
#include "telephony_log_wrapper.h"
#include "telephony_permission.h"
#include "want.h"

namespace OHOS {
namespace Telephony {

SmsMmsCommon::SmsMmsCommon() {}

SmsMmsCommon::~SmsMmsCommon() {}

void SmsMmsCommon::SendBroadcast(uint16_t id, std::string notify, std::string stauts, std::string type)
{
    EventFwk::Want want;
    EventFwk::CommonEventData data;
    EventFwk::CommonEventPublishInfo publishInfo;
    want.SetAction(notify);
    std::vector<std::string> smsPermissions;
    smsPermissions.emplace_back(Permission::SEND_MESSAGES);
    publishInfo.SetSubscriberPermissions(smsPermissions);
    want.SetParam(SmsMmsCommonData::MSG_ID, id);
    want.SetParam(SmsMmsCommonData::MSG_TYPE, type);
    want.SetParam(SmsMmsCommonData::MSG_STATE, stauts);
    data.SetWant(want);
    data.SetData(notify);
    bool result = EventFwk::CommonEventManager::PublishCommonEvent(data, publishInfo, nullptr);
    TELEPHONY_LOGI(
        "SmsMmsCommon::SendBroadcast:%{public}d;notify:%{public}s;stauts:%{public}s;type:%{public}s;result:%{public}d",
        id, notify.c_str(), stauts.c_str(), type.c_str(), result);
}

} // namespace Telephony
} // namespace OHOS