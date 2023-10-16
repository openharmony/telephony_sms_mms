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

namespace OHOS {
namespace Telephony {
using namespace EventFwk;

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
    }
}
} // namespace Telephony
} // namespace OHOS