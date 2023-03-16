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

#include "common_event_support.h"
#include "sms_broadcast_subscriber_gtest.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
using namespace OHOS::EventFwk;
SmsBroadcastSubscriberGtest::SmsBroadcastSubscriberGtest(
    const OHOS::EventFwk::CommonEventSubscribeInfo &subscriberInfo, SmsMmsTestHelper &helper)
    : CommonEventSubscriber(subscriberInfo)
{
    helper_ = &helper;
}

void SmsBroadcastSubscriberGtest::OnReceiveEvent(const OHOS::EventFwk::CommonEventData &data)
{
    std::string action = data.GetWant().GetAction();
    if (action == CommonEventSupport::COMMON_EVENT_SMS_EMERGENCY_CB_RECEIVE_COMPLETED ||
        action == CommonEventSupport::COMMON_EVENT_SMS_CB_RECEIVE_COMPLETED) {
        TELEPHONY_LOGI("receive a Cell Broadcast");
        if (helper_ == nullptr) {
            return;
        }
        helper_->SetBoolResult(true);
        helper_->NotifyAll();
        helper_ = nullptr;
    } else if (action == CommonEventSupport::COMMON_EVENT_SMS_RECEIVE_COMPLETED) {
        TELEPHONY_LOGI("receive a Normal Sms");
        if (helper_ == nullptr) {
            return;
        }
        helper_->SetBoolResult(true);
        helper_->NotifyAll();
        helper_ = nullptr;
    } else if (action == CommonEventSupport::COMMON_EVENT_SMS_WAPPUSH_RECEIVE_COMPLETED) {
        TELEPHONY_LOGI("receive a Wap Push");
        if (helper_ == nullptr) {
            return;
        }
        helper_->SetBoolResult(true);
        helper_->NotifyAll();
        helper_ = nullptr;
    }
}
} // namespace Telephony
} // namespace OHOS
