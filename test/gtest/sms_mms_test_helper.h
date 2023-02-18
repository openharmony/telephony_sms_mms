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
#ifndef SMS_MMS_TEST_HELPER_H
#define SMS_MMS_TEST_HELPER_H

#include <mutex>

namespace OHOS {
namespace Telephony {
class SmsMmsTestHelper {
public:
    int32_t slotId = 0;
    bool Run(void (*func)(SmsMmsTestHelper &), SmsMmsTestHelper &helper);
    void NotifyAll();
    bool WaitForResult(int32_t timeoutSecond);
    void SetBoolResult(bool result);
    void SetSendSmsIntResult(int32_t result);
    void SetDeliverySmsIntResult(int32_t result);
    void SetIntResult(int32_t result);
    void SetStringResult(const std::string &str);
    bool GetBoolResult();
    int32_t GetSendSmsIntResult();
    int32_t GetDeliverySmsIntResult();
    int32_t GetIntResult();
    std::string GetStringResult();

private:
    int32_t result_ = 0;
    bool boolResult_ = false;
    int32_t sendSmsResult_ = -1;
    int32_t deliverySmsResult_ = -1;
    std::string strResult_ = "";
    std::mutex mtx_;
    std::condition_variable cv_;
};
} // namespace Telephony
} // namespace OHOS

#endif // SMS_MMS_TEST_HELPER_H
