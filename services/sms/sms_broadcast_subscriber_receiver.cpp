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
#include "sms_receive_reliability_handler.h"
#include "want.h"
#include "event_handler.h"
#include "event_runner.h"

namespace OHOS {
namespace Telephony {
using namespace EventFwk;

SmsBroadcastSubscriberReceiver::SmsBroadcastSubscriberReceiver(const CommonEventSubscribeInfo &subscriberInfo)
    : CommonEventSubscriber(subscriberInfo) {}

void SmsBroadcastSubscriberReceiver::OnReceiveEvent(const OHOS::EventFwk::CommonEventData &data)
{
    auto want = data.GetWant();
    std::string action = want.GetAction();
    bool isSms = action == CommonEventSupport::COMMON_EVENT_SMS_RECEIVE_COMPLETED;
    bool isWapPush = action == CommonEventSupport::COMMON_EVENT_SMS_WAPPUSH_RECEIVE_COMPLETED;
    if (isSms || isWapPush) {
        int refId = want.GetIntParam(SMS_BROADCAST_MSG_REF_ID_KEY, 0);
        int dataBaseId = want.GetIntParam(SMS_BROADCAST_DATABASE_ID_KEY, 0);
        TELEPHONY_LOGI("OnReceiveEvent refId =%{public}d, dataBaseId =%{public}d", refId, dataBaseId);
        istd::string address = want.GetStringParam(SMS_BROADCAST_ADDRESS_KEY);
        if (!address.empty() && isSms) {
            DelayedSingleton<SmsPersistHelper>::GetInstance()->UpdateContact(address);
        }
        std::make_shared<SmsReceiveReliabilityHandler>(0)->DeleteMessageFormDb(refId, dataBaseId);
    }
}
} // namespace Telephony
} // namespace OHOS