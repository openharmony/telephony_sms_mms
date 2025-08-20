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

#ifndef SMS_STATE_OBSERVER_H
#define SMS_STATE_OBSERVER_H

#include "common_event_manager.h"
#include "common_event_subscriber.h"
#include "common_event_support.h"
#include "system_ability_status_change_stub.h"

namespace OHOS {
namespace Telephony {
using CommonEventSubscribeInfo = OHOS::EventFwk::CommonEventSubscribeInfo;
using CommonEventSubscriber = OHOS::EventFwk::CommonEventSubscriber;

enum SmsStateEventIntValue {
    COMMON_EVENT_SMS_EMERGENCY_CB_RECEIVE_COMPLETED,
    COMMON_EVENT_SMS_CB_RECEIVE_COMPLETED,
    COMMON_EVENT_SMS_RECEIVE_COMPLETED,
    COMMON_EVENT_SMS_WAPPUSH_RECEIVE_COMPLETED,
#ifdef BASE_POWER_IMPROVEMENT_FEATURE
    COMMON_EVENT_EXIT_STR_TELEPHONY_NOTIFY,
#endif
    COMMON_EVENT_UNKNOWN,
};

class SmsStateObserver;
class SmsStateEventSubscriber : public CommonEventSubscriber {
public:
    SmsStateEventSubscriber(const CommonEventSubscribeInfo &info, SmsStateObserver &observer) :
        CommonEventSubscriber(info), observer_(observer) {}
    ~SmsStateEventSubscriber() = default;
    void OnReceiveEvent(const OHOS::EventFwk::CommonEventData &data) override;
    void InitEventMap();

private:
    SmsStateEventIntValue GetSmsStateEventIntValue(std::string &event) const;

private:
    std::map<std::string, SmsStateEventIntValue> smsStateEvenMapIntValues_;
    SmsStateObserver &observer_;
};

class SmsStateObserver {
public:
    SmsStateObserver() = default;
    ~SmsStateObserver() = default;
    void StartEventSubscriber();
    void StopEventSubscriber();
#ifdef BASE_POWER_IMPROVEMENT_FEATURE
    void SetAsyncCommonEvent(const std::shared_ptr<EventFwk::AsyncCommonEventResult> &result);
    void ProcessStrExitFinishEvent();
#endif

private:

    std::shared_ptr<SmsStateEventSubscriber> SubscribeToEvents(const std::vector<std::string>& events,
        int priority = 0, const std::string& permission = "");
    void StopEventsSubscriber(std::shared_ptr<SmsStateEventSubscriber> &subscriber);
#ifdef BASE_POWER_IMPROVEMENT_FEATURE
    void StartOrderEventSubscriber();
#endif
    void StartDisorderEventSubscriber();

private:
    std::shared_ptr<SmsStateEventSubscriber> smsSubscriber_;
#ifdef BASE_POWER_IMPROVEMENT_FEATURE
    std::shared_ptr<SmsStateEventSubscriber> strEnterSubscriber_ = nullptr;
    std::shared_ptr<EventFwk::AsyncCommonEventResult> strEnterEventResult_ = nullptr;
#endif

private:
    class SystemAbilityStatusChangeListener : public SystemAbilityStatusChangeStub {
    public:
        explicit SystemAbilityStatusChangeListener(std::shared_ptr<SmsStateEventSubscriber> &subscriber);
        ~SystemAbilityStatusChangeListener() = default;
        virtual void OnAddSystemAbility(int32_t systemAbilityId, const std::string &deviceId) override;
        virtual void OnRemoveSystemAbility(int32_t systemAbilityId, const std::string &deviceId) override;

    private:
        std::shared_ptr<SmsStateEventSubscriber> sub_ = nullptr;
    };
};
} // namespace Telephony
} // namespace OHOS
#endif // SMS_STATE_OBSERVER_H
