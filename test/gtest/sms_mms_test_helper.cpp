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

#include "sms_mms_test_helper.h"

#include <thread>

#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
constexpr static const int32_t WAIT_TIME_SECOND = 10;

bool SmsMmsTestHelper::Run(void (*func)(SmsMmsTestHelper &), SmsMmsTestHelper &helper)
{
    std::thread t(func, std::ref(helper));
    t.detach();
    TELEPHONY_LOGI("Thread running");
    return WaitForResult(WAIT_TIME_SECOND);
}

void SmsMmsTestHelper::NotifyAll()
{
    std::unique_lock<std::mutex> lock(mtx_);
    cv_.notify_all();
    TELEPHONY_LOGI("Thread NotifyAll");
}

bool SmsMmsTestHelper::WaitForResult(int32_t timeoutSecond)
{
    std::unique_lock<std::mutex> lock(mtx_);
    if (cv_.wait_for(lock, std::chrono::seconds(timeoutSecond)) == std::cv_status::timeout) {
        TELEPHONY_LOGI("Interface overtime");
        return false;
    }
    return true;
}

void SmsMmsTestHelper::SetBoolResult(bool result)
{
    boolResult_ = result;
    TELEPHONY_LOGI("Set boolResult_ : %{public}d ", boolResult_);
}

void SmsMmsTestHelper::SetSendSmsIntResult(int32_t result)
{
    sendSmsResult_ = result;
    TELEPHONY_LOGI("Set sendSmsResult_ : %{public}d ", sendSmsResult_);
}

void SmsMmsTestHelper::SetDeliverySmsIntResult(int32_t result)
{
    deliverySmsResult_ = result;
    TELEPHONY_LOGI("Set deliverySmsResult_ : %{public}d ", deliverySmsResult_);
}

void SmsMmsTestHelper::SetIntResult(int32_t result)
{
    result_ = result;
}

void SmsMmsTestHelper::SetStringResult(const std::string &str)
{
    strResult_ = str;
}

bool SmsMmsTestHelper::GetBoolResult()
{
    TELEPHONY_LOGI("Get boolResult_ : %{public}d ", boolResult_);
    return boolResult_;
}

int32_t SmsMmsTestHelper::GetSendSmsIntResult()
{
    TELEPHONY_LOGI("Get sendSmsResult_ : %{public}d ", sendSmsResult_);
    return sendSmsResult_;
}

int32_t SmsMmsTestHelper::GetDeliverySmsIntResult()
{
    TELEPHONY_LOGI("Get deliverySmsResult_ : %{public}d ", deliverySmsResult_);
    return deliverySmsResult_;
}

int32_t SmsMmsTestHelper::GetIntResult()
{
    TELEPHONY_LOGI("Get IntResult : %{public}d ", result_);
    return result_;
}

std::string SmsMmsTestHelper::GetStringResult()
{
    return strResult_;
}
} // namespace Telephony
} // namespace OHOS
