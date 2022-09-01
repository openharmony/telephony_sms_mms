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

#include "sms_state_handler.h"

#include "sms_state_observer.h"
#include "string_utils.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
bool SmsStateHandler::UnRegisterHandler()
{
    TELEPHONY_LOGI("SmsStateHandler::UnRegisterHandler entry");
    if (smsStateObserver_ == nullptr) {
        TELEPHONY_LOGE("SmsStateHandler::UnRegisterHandler smsStateObserver_ is null.");
        return false;
    }
    smsStateObserver_->StopEventSubscriber();
    return true;
}

bool SmsStateHandler::RegisterHandler()
{
    smsStateObserver_ = std::make_shared<SmsStateObserver>();
    if (smsStateObserver_ == nullptr) {
        TELEPHONY_LOGE("SmsStateHandler::RegisteHandler smsStateObserver_ is null.");
        return false;
    }
    smsStateObserver_->StartEventSubscriber();
    return true;
}

void SmsStateHandler::Init()
{
    TELEPHONY_LOGI("SmsStateHandler init");
    if (!RegisterHandler()) {
        TELEPHONY_LOGE("SmsStateHandler::Init register handle fail.");
    }
}

void SmsStateHandler::UnInit()
{
    TELEPHONY_LOGI("SmsStateHandler UnInit");
    if (!UnRegisterHandler()) {
        TELEPHONY_LOGE("SmsStateHandler::UnInit unregister handle fail.");
    }
}
} // namespace Telephony
} // namespace OHOS
