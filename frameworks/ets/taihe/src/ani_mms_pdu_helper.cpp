/*
 * Copyright (C) 2023-2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "ani_mms_pdu_helper.h"

#include "tel_event_handler.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
constexpr static const int32_t WAIT_TIME_SECOND = 30;

bool AniMmsPduHelper::Run(void (*func)(AniMmsPduHelper &), AniMmsPduHelper &helper) __attribute__((no_sanitize("cfi")))
{
    TelFFRTUtils::Submit([&]() { func(helper); });
    TELEPHONY_LOGI("Thread running");
    return WaitForResult(WAIT_TIME_SECOND);
}

void AniMmsPduHelper::NotifyAll()
{
    std::unique_lock<std::mutex> lock(mtx_);
    cv_.notify_all();
    TELEPHONY_LOGI("Thread NotifyAll");
}

bool AniMmsPduHelper::WaitForResult(int32_t timeoutSecond) __attribute__((no_sanitize("cfi")))
{
    std::unique_lock<std::mutex> lock(mtx_);
    if (cv_.wait_for(lock, std::chrono::seconds(timeoutSecond)) == std::cv_status::timeout) {
        TELEPHONY_LOGI("Interface overtime");
        return false;
    }
    return true;
}

void AniMmsPduHelper::SetPduFileName(const std::string &pduFileName)
{
    pduFileName_ = pduFileName;
}

void AniMmsPduHelper::SetStoreFileName(const std::string &storeFileName)
{
    storeFileName_ = storeFileName;
}

void AniMmsPduHelper::SetDbUrl(const std::string &dbUrl)
{
    dbUrl_ = dbUrl;
}

void AniMmsPduHelper::SetDataShareHelper(std::shared_ptr<DataShare::DataShareHelper> &datashareHelper)
{
    datashareHelper_ = datashareHelper;
}

std::string AniMmsPduHelper::GetPduFileName()
{
    return pduFileName_;
}

std::string AniMmsPduHelper::GetStoreFileName()
{
    return storeFileName_;
}

std::string AniMmsPduHelper::GetDbUrl()
{
    return dbUrl_;
}

std::shared_ptr<DataShare::DataShareHelper> AniMmsPduHelper::GetDataShareHelper()
{
    return datashareHelper_;
}
} // namespace Telephony
} // namespace OHOS
