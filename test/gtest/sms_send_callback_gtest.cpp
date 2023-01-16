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

#include "sms_send_callback_gtest.h"

#include <iostream>

#include "telephony_errors.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
static constexpr int SMS_DEFAULT_RESULT = 0;
static constexpr int SMS_DEFAULT_ERROR = -1;

SmsSendCallbackGTest::SmsSendCallbackGTest(SmsMmsTestHelper &helper)
{
    callbackHelper_ = &helper;
}

SmsSendCallbackGTest::~SmsSendCallbackGTest() {}

void SmsSendCallbackGTest::HasDeliveryCallBack(bool hasCallback)
{
    hasDeliveryCallback_ = hasCallback;
}

void SmsSendCallbackGTest::OnSmsSendResult(const ISendShortMessageCallback::SmsSendResult result)
{
    TELEPHONY_LOGI("SmsSendCallbackGTest OnSmsSendResult = %{public}d", result);
    if (callbackHelper_ == nullptr) {
        TELEPHONY_LOGE("SmsSendCallbackGTest callbackHelper_ is nullptr");
        return;
    }

    callbackHelper_->SetSendSmsIntResult(result);
    if (!hasDeliveryCallback_) {
        callbackHelper_->NotifyAll();
    }
    callbackHelper_ = nullptr;
}

int SmsSendCallbackGTest::OnRemoteRequest(
    uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    if (data.ReadInterfaceToken() != GetDescriptor()) {
        TELEPHONY_LOGE("descriptor checked fail");
        return TELEPHONY_ERR_DESCRIPTOR_MISMATCH;
    }

    switch (code) {
        case ON_SMS_SEND_RESULT: {
            int32_t result = data.ReadInt32();
            OnSmsSendResult(static_cast<ISendShortMessageCallback::SmsSendResult>(result));
            return SMS_DEFAULT_RESULT;
        }
        default: {
            return SMS_DEFAULT_ERROR;
        }
    }
}
} // namespace Telephony
} // namespace OHOS