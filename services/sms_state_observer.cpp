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

#include "sms_state_observer.h"

#include "common_event.h"
#include "common_event_manager.h"
#include "common_event_support.h"
#include "iservice_registry.h"
#include "singleton.h"
#include "sms_hisysevent.h"
#include "system_ability_definition.h"
#include "telephony_log_wrapper.h"
#include "want.h"

namespace OHOS {
namespace Telephony {
using namespace OHOS::EventFwk;
static const int32_t DEFAULT_VALUE = -1;

void SmsStateObserver::StartEventSubscriber()
{
    MatchingSkills matchingSkills;
    matchingSkills.AddEvent(CommonEventSupport::COMMON_EVENT_SMS_EMERGENCY_CB_RECEIVE_COMPLETED);
    matchingSkills.AddEvent(CommonEventSupport::COMMON_EVENT_SMS_CB_RECEIVE_COMPLETED);
    matchingSkills.AddEvent(CommonEventSupport::COMMON_EVENT_SMS_RECEIVE_COMPLETED);
    matchingSkills.AddEvent(CommonEventSupport::COMMON_EVENT_SMS_WAPPUSH_RECEIVE_COMPLETED);
    CommonEventSubscribeInfo subscriberInfo(matchingSkills);
    smsSubscriber_ = std::make_shared<SmsStateEventSubscriber>(subscriberInfo);
    smsSubscriber_->InitEventMap();
    auto samgrProxy = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (samgrProxy == nullptr) {
        TELEPHONY_LOGE("StartEventSubscriber samgrProxy is nullptr");
        return;
    }
    smsStatusListener_ = new (std::nothrow) SystemAbilityStatusChangeListener(smsSubscriber_);
    if (smsStatusListener_ == nullptr) {
        TELEPHONY_LOGE("StartEventSubscriber smsStatusListener_ is nullptr");
        return;
    }
    int32_t ret = samgrProxy->SubscribeSystemAbility(COMMON_EVENT_SERVICE_ID, smsStatusListener_);
    TELEPHONY_LOGI("StartEventSubscriber SubscribeSystemAbility result:%{public}d", ret);
}

void SmsStateObserver::StopEventSubscriber()
{
    if (smsSubscriber_ == nullptr) {
        TELEPHONY_LOGE("SmsStateObserver::StopEventSubscriber subscriber_ is nullptr");
        return;
    }
    bool unSubscribeResult = CommonEventManager::UnSubscribeCommonEvent(smsSubscriber_);
    smsSubscriber_ = nullptr;
    TELEPHONY_LOGI("SmsStateObserver::StopEventSubscriber unSubscribeResult = %{public}d", unSubscribeResult);
}

void SmsStateEventSubscriber::OnReceiveEvent(const OHOS::EventFwk::CommonEventData &data)
{
    OHOS::EventFwk::Want want = data.GetWant();
    std::string action = data.GetWant().GetAction();
    TELEPHONY_LOGI("SmsStateEventSubscriber::OnReceiveEvent: action = %{public}s", action.c_str());
    int32_t slotId = want.GetIntParam("slotId", DEFAULT_VALUE);
    switch (GetSmsStateEventIntValue(action)) {
        case COMMON_EVENT_SMS_EMERGENCY_CB_RECEIVE_COMPLETED:
        case COMMON_EVENT_SMS_CB_RECEIVE_COMPLETED:
            DelayedSingleton<SmsHiSysEvent>::GetInstance()->JudgingCbBroadcastTimeOut(
                slotId, SmsMmsMessageType::CELL_BROAD_CAST);
            SmsHiSysEvent::WriteSmsReceiveBehaviorEvent(slotId, SmsMmsMessageType::CELL_BROAD_CAST);
            break;
        case COMMON_EVENT_SMS_RECEIVE_COMPLETED:
            DelayedSingleton<SmsHiSysEvent>::GetInstance()->JudgingSmsBroadcastTimeOut(
                slotId, SmsMmsMessageType::SMS_SHORT_MESSAGE);
            SmsHiSysEvent::WriteSmsReceiveBehaviorEvent(slotId, SmsMmsMessageType::SMS_SHORT_MESSAGE);
            break;
        case COMMON_EVENT_SMS_WAPPUSH_RECEIVE_COMPLETED:
            DelayedSingleton<SmsHiSysEvent>::GetInstance()->JudgingWapPushBroadcastTimeOut(
                slotId, SmsMmsMessageType::WAP_PUSH);
            SmsHiSysEvent::WriteSmsReceiveBehaviorEvent(slotId, SmsMmsMessageType::WAP_PUSH);
            break;
        default:
            break;
    }
}

SmsStateEventIntValue SmsStateEventSubscriber::GetSmsStateEventIntValue(std::string &event) const
{
    auto iter = smsStateEvenMapIntValues_.find(event);
    if (iter == smsStateEvenMapIntValues_.end()) {
        return COMMON_EVENT_UNKNOWN;
    }
    return iter->second;
}

void SmsStateEventSubscriber::InitEventMap()
{
    smsStateEvenMapIntValues_ = {
        { CommonEventSupport::COMMON_EVENT_SMS_EMERGENCY_CB_RECEIVE_COMPLETED,
            COMMON_EVENT_SMS_EMERGENCY_CB_RECEIVE_COMPLETED },
        { CommonEventSupport::COMMON_EVENT_SMS_CB_RECEIVE_COMPLETED, COMMON_EVENT_SMS_CB_RECEIVE_COMPLETED },
        { CommonEventSupport::COMMON_EVENT_SMS_RECEIVE_COMPLETED, COMMON_EVENT_SMS_RECEIVE_COMPLETED },
        { CommonEventSupport::COMMON_EVENT_SMS_WAPPUSH_RECEIVE_COMPLETED, COMMON_EVENT_SMS_WAPPUSH_RECEIVE_COMPLETED },
    };
}

SmsStateObserver::SystemAbilityStatusChangeListener::SystemAbilityStatusChangeListener(
    std::shared_ptr<SmsStateEventSubscriber> &sub)
    : sub_(sub)
{}

void SmsStateObserver::SystemAbilityStatusChangeListener::OnAddSystemAbility(
    int32_t systemAbilityId, const std::string &deviceId)
{
    if (systemAbilityId != COMMON_EVENT_SERVICE_ID) {
        TELEPHONY_LOGE("systemAbilityId is not COMMON_EVENT_SERVICE_ID");
        return;
    }
    if (sub_ == nullptr) {
        TELEPHONY_LOGE("OnAddSystemAbility COMMON_EVENT_SERVICE_ID sub_ is nullptr");
        return;
    }
    bool subscribeResult = EventFwk::CommonEventManager::SubscribeCommonEvent(sub_);
    TELEPHONY_LOGI("SmsStateObserver::OnAddSystemAbility subscribeResult = %{public}d", subscribeResult);
}

void SmsStateObserver::SystemAbilityStatusChangeListener::OnRemoveSystemAbility(
    int32_t systemAbilityId, const std::string &deviceId)
{
    if (systemAbilityId != COMMON_EVENT_SERVICE_ID) {
        TELEPHONY_LOGE("systemAbilityId is not COMMON_EVENT_SERVICE_ID");
        return;
    }
    if (sub_ == nullptr) {
        TELEPHONY_LOGE("OnRemoveSystemAbility COMMON_EVENT_SERVICE_ID opName_ is nullptr");
        return;
    }
    bool subscribeResult = CommonEventManager::UnSubscribeCommonEvent(sub_);
    TELEPHONY_LOGI("DeviceStateObserver::OnRemoveSystemAbility subscribeResult = %{public}d", subscribeResult);
}
} // namespace Telephony
} // namespace OHOS
