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

#include "sms_delivery_callback_gtest.h"

#include <iostream>

#include "string_utils.h"
#include "telephony_errors.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
static constexpr int SMS_DEFAULT_RESULT = 0;
static constexpr int SMS_DEFAULT_ERROR = -1;

SmsDeliveryCallbackGTest::SmsDeliveryCallbackGTest(SmsMmsTestHelper &helper)
{
    callbackHelper_ = &helper;
}

SmsDeliveryCallbackGTest::~SmsDeliveryCallbackGTest() {}

void SmsDeliveryCallbackGTest::OnSmsDeliveryResult(const std::u16string &pdu)
{
    std::string pduStr = StringUtils::ToUtf8(pdu);
    int32_t deliveryPduLen = static_cast<int32_t>(pduStr.size());
    TELEPHONY_LOGI("SmsDeliveryCallbackGTest OnSmsDeliveryResult pduLen =  %{public}d", deliveryPduLen);

    if (callbackHelper_ == nullptr) {
        TELEPHONY_LOGE("SmsDeliveryCallbackGTest callbackHelper_ is nullptr");
        return;
    }
    if (deliveryPduLen > 0) {
        callbackHelper_->SetDeliverySmsIntResult(SMS_DEFAULT_RESULT);
        callbackHelper_->NotifyAll();
    } else {
        callbackHelper_->SetDeliverySmsIntResult(SMS_DEFAULT_ERROR);
        callbackHelper_->NotifyAll();
    }
    callbackHelper_ = nullptr;
}

int SmsDeliveryCallbackGTest::OnRemoteRequest(
    uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    if (data.ReadInterfaceToken() != GetDescriptor()) {
        TELEPHONY_LOGE("descriptor checked fail");
        return TELEPHONY_ERR_DESCRIPTOR_MISMATCH;
    }

    switch (code) {
        case ON_SMS_DELIVERY_RESULT: {
            auto result = data.ReadString16();
            OnSmsDeliveryResult(result);
            return SMS_DEFAULT_RESULT;
        }
        default: {
            OnSmsDeliveryResult(u"");
            return SMS_DEFAULT_ERROR;
        }
    }
}
} // namespace Telephony
} // namespace OHOS