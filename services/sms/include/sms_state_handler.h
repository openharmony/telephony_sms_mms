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

#ifndef SMS_STATE_HANDLER_H
#define SMS_STATE_HANDLER_H

#include "sms_receive_handler.h"
#include "sms_state_observer.h"

namespace OHOS {
namespace Telephony {
class SmsStateHandler {
public:
    SmsStateHandler() = default;
    virtual ~SmsStateHandler() = default;
    void Init();
    void UnInit();
    bool UnRegisterHandler();
    bool RegisterHandler();

private:
    std::shared_ptr<SmsStateObserver> smsStateObserver_ = nullptr;
};
} // namespace Telephony
} // namespace OHOS
#endif // SMS_STATE_HANDLER_H
