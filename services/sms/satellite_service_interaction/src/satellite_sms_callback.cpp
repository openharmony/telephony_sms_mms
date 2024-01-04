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

#include "satellite_sms_callback.h"

#include "radio_event.h"
#include "telephony_errors.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
SatelliteSmsCallback::SatelliteSmsCallback(const std::shared_ptr<TelEventHandler> &handler) : handler_(handler) {}

SatelliteSmsCallback::~SatelliteSmsCallback() {}

int32_t SatelliteSmsCallback::SendSmsResponse(InnerEvent::Pointer &event)
{
    if (handler_ == nullptr) {
        TELEPHONY_LOGE("SendSmsResponse handler is null!");
        return TELEPHONY_ERROR;
    }
    handler_->SendEvent(event, 0, AppExecFwk::EventQueue::Priority::IMMEDIATE);
    return TELEPHONY_SUCCESS;
}

int32_t SatelliteSmsCallback::SmsStatusReportNotify(InnerEvent::Pointer &event)
{
    if (handler_ == nullptr) {
        TELEPHONY_LOGE("SmsStatusReportNotify handler is null!");
        return TELEPHONY_ERROR;
    }
    handler_->SendEvent(event, 0, AppExecFwk::EventQueue::Priority::IMMEDIATE);
    return TELEPHONY_SUCCESS;
}

int32_t SatelliteSmsCallback::NewSmsNotify(InnerEvent::Pointer &event)
{
    if (handler_ == nullptr) {
        TELEPHONY_LOGE("NewSmsNotify handler is null!");
        return TELEPHONY_ERROR;
    }
    TELEPHONY_LOGI("SatelliteSms NewSms");
    handler_->SendEvent(event, 0, AppExecFwk::EventQueue::Priority::IMMEDIATE);
    return TELEPHONY_SUCCESS;
}
} // namespace Telephony
} // namespace OHOS
