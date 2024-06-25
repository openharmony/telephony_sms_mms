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

#include "sms_broadcast_subscriber_receiver.h"
#include "sms_persist_helper.h"
#include "telephony_log_wrapper.h"
#include "want.h"
#include "event_handler.h"
#include "event_runner.h"

namespace OHOS {
namespace Telephony {
using namespace EventFwk;

std::queue<std::shared_ptr<SmsBroadcastSubscriberReceiver>> sptrQueue;
std::shared_ptr<AppExecFwk::EventRunner> eventRunner_ = nullptr;
std::shared_ptr<AppExecFwk::EventHandler> eventHandler_ = nullptr;

SmsBroadcastSubscriberReceiver::SmsBroadcastSubscriberReceiver(const CommonEventSubscribeInfo &subscriberInfo,
    std::shared_ptr<SmsReceiveReliabilityHandler> handler, uint16_t refId, uint16_t dataBaseId,
    const std::string &address)
    : CommonEventSubscriber(subscriberInfo)
{
    handler_ = handler;
    refId_ = refId;
    dataBaseId_ = dataBaseId;
    address_ = address;
}

void SmsBroadcastSubscriberReceiver::OnReceiveEvent(const OHOS::EventFwk::CommonEventData &data)
{
    std::string action = data.GetWant().GetAction();
    bool isSms = action == CommonEventSupport::COMMON_EVENT_SMS_RECEIVE_COMPLETED;
    bool isWapPush = action == CommonEventSupport::COMMON_EVENT_SMS_WAPPUSH_RECEIVE_COMPLETED;
    if (isSms || isWapPush) {
        TELEPHONY_LOGI("OnReceiveEvent refId_ =%{public}d, dataBaseId_ =%{public}d", refId_, dataBaseId_);
        if (handler_ == nullptr) {
            TELEPHONY_LOGE("handler_ is nullptr");
            return;
        }
        if (!address_.empty() && isSms) {
            DelayedSingleton<SmsPersistHelper>::GetInstance()->UpdateContact(address_);
        }
        handler_->DeleteMessageFormDb(refId_, dataBaseId_);
        // only save one object of eventrunner and eventhandler because each object will create a new thread.
        if (eventRunner_ == nullptr) {
            eventRunner_ = AppExecFwk::EventRunner::Create("unsubscribe thread");
        }
        if (eventHandler_ == nullptr) {
            eventHandler_ = std::make_shared<AppExecFwk::EventHandler>(eventRunner_);
        }
        if (eventHandler_ == nullptr) {
            TELEPHONY_LOGE("create eventhandler fail");
            return;
        }
        // post a task that be excuted after 10s, and callback is static function in this class.
        bool postTaskResult = eventHandler_->PostTask(UnsubscribeInCes, "unsubscribe event task", 10000);
        if (!postTaskResult) {
            TELEPHONY_LOGE("post unsubscribe task fail");
        }
    }
}

void SmsBroadcastSubscriberReceiver::UnsubscribeInCes()
{
    TELEPHONY_LOGE("UnsubscribeInCes entry");
    if (sptrQueue.empty()) {
        TELEPHONY_LOGE("SmsBroadcastSubscriberReceiver queue is empty");
    } else {
        auto smsBroadcastSubscriberReceiver = sptrQueue.front();
        sptrQueue.pop();
        // call api to unsubscribe
        bool unsubscribeResult = CommonEventManager::UnSubscribeCommonEvent(smsBroadcastSubscriberReceiver);
        if (!unsubscribeResult) {
            TELEPHONY_LOGE("unsubscribe fail");
        }
    }
}
} // namespace Telephony
} // namespace OHOS