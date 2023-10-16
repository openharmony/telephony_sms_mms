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

#ifndef SMS_BROADCAST_SUBSCRIBER_RECEIVER_H
#define SMS_BROADCAST_SUBSCRIBER_RECEIVER_H

#include <memory>

#include "common_event_manager.h"
#include "common_event_subscriber.h"
#include "common_event_support.h"
#include "sms_receive_reliability_handler.h"

namespace OHOS {
namespace Telephony {

class SmsBroadcastSubscriberReceiver : public EventFwk::CommonEventSubscriber {
public:
    SmsBroadcastSubscriberReceiver(const OHOS::EventFwk::CommonEventSubscribeInfo &subscriberInfo,
        std::shared_ptr<SmsReceiveReliabilityHandler> handler, uint16_t refId, uint16_t dataBaseId,
        const std::string &address);
    ~SmsBroadcastSubscriberReceiver() = default;
    virtual void OnReceiveEvent(const OHOS::EventFwk::CommonEventData &data);

private:
    std::shared_ptr<SmsReceiveReliabilityHandler> handler_ = nullptr;
    uint16_t refId_ = 0;
    uint16_t dataBaseId_ = 0;
    std::string address_;
};
} // namespace Telephony
} // namespace OHOS
#endif