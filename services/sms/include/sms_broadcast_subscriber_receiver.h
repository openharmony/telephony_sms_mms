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

namespace OHOS {
namespace Telephony {

class SmsBroadcastSubscriberReceiver : public EventFwk::CommonEventSubscriber {
public:
    static constexpr const char *SMS_BROADCAST_DATABASE_ID_KEY = "dataBaseId";
    static constexpr const char *SMS_BROADCAST_MSG_REF_ID_KEY = "msgRefId";
    static constexpr const char *SMS_BROADCAST_ADDRESS_KEY = "addr";
    static constexpr const char *SMS_BROADCAST_MSG_COUNT_KEY = "msgCount";
    SmsBroadcastSubscriberReceiver(const OHOS::EventFwk::CommonEventSubscribeInfo &subscriberInfo);
    ~SmsBroadcastSubscriberReceiver() = default;
    virtual void OnReceiveEvent(const OHOS::EventFwk::CommonEventData &data);
};
} // namespace Telephony
} // namespace OHOS
#endif