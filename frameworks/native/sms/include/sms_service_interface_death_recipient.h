/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#ifndef SMS_SERVICE_INTERFACE_DEATH_RECIPIENT_H
#define SMS_SERVICE_INTERFACE_DEATH_RECIPIENT_H

#include "iremote_object.h"
#include "sms_service_manager_client.h"

namespace OHOS {
namespace Telephony {
class SmsServiceInterfaceDeathRecipient : public IRemoteObject::DeathRecipient {
public:
    explicit SmsServiceInterfaceDeathRecipient(SmsServiceManagerClient &client);
    virtual ~SmsServiceInterfaceDeathRecipient() = default;
    virtual void OnRemoteDied(const wptr<IRemoteObject> &remote) override;

private:
    SmsServiceManagerClient &client_;
};
} // namespace Telephony
} // namespace OHOS

#endif // SMS_SERVICE_DEATH_RECIPIENT_H