/*
 * Copyright (C) 2023-2024 Huawei Device Co., Ltd.
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

#include "napi_mms_pdu_helper.h"

#include "tel_event_handler.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
constexpr static const int32_t WAIT_TIME_SECOND = 30;

bool NapiMmsPduHelper::Run(void (*func)(NapiMmsPduHelper &), NapiMmsPduHelper &helper)
    __attribute__((no_sanitize("cfi")))
{
    TelFFRTUtils::Submit([&]() { func(helper); });
    TELEPHONY_LOGI("Thread running");
    return WaitForResult(WAIT_TIME_SECOND);
}

void NapiMmsPduHelper::NotifyAll()
{
    std::unique_lock<std::mutex> lock(mtx_);
    cv_.notify_all();
    TELEPHONY_LOGI("Thread NotifyAll");
}

bool NapiMmsPduHelper::WaitForResult(int32_t timeoutSecond) __attribute__((no_sanitize("cfi")))
{
    std::unique_lock<std::mutex> lock(mtx_);
    if (cv_.wait_for(lock, std::chrono::seconds(timeoutSecond)) == std::cv_status::timeout) {
        TELEPHONY_LOGI("Interface overtime");
        return false;
    }
    return true;
}

void NapiMmsPduHelper::SetPduFileName(const std::string &pduFileName)
{
    pduFileName_ = pduFileName;
}

void NapiMmsPduHelper::SetStoreFileName(const std::string &storeFileName)
{
    storeFileName_ = storeFileName;
}

void NapiMmsPduHelper::SetDbUrl(const std::string &dbUrl)
{
    dbUrl_ = dbUrl;
}

void NapiMmsPduHelper::SetDataShareHelper(std::shared_ptr<DataShare::DataShareHelper> &datashareHelper)
{
    datashareHelper_ = datashareHelper;
}

std::string NapiMmsPduHelper::GetPduFileName()
{
    return pduFileName_;
}

std::string NapiMmsPduHelper::GetStoreFileName()
{
    return storeFileName_;
}

std::string NapiMmsPduHelper::GetDbUrl()
{
    return dbUrl_;
}

std::shared_ptr<DataShare::DataShareHelper> NapiMmsPduHelper::GetDataShareHelper()
{
    return datashareHelper_;
}
} // namespace Telephony
} // namespace OHOS
