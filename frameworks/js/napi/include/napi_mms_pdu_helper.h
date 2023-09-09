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
#ifndef NAPI_MMS_PDU_HELPER_H
#define NAPI_MMS_PDU_HELPER_H

#include <condition_variable>
#include <mutex>

#include "datashare_helper.h"
#include "datashare_predicates.h"

namespace OHOS {
namespace Telephony {
class NapiMmsPduHelper {
public:
    bool Run(void (*func)(NapiMmsPduHelper &), NapiMmsPduHelper &helper);
    void NotifyAll();
    bool WaitForResult(int32_t timeoutSecond);
    void SetPduFileName(const std::string &pduFileName);
    void SetStoreFileName(const std::string &storeFileName);
    void SetDbUrl(const std::string &dbUrl);
    void SetDataShareHelper(std::shared_ptr<DataShare::DataShareHelper> &datashareHelper);

    std::string GetPduFileName();
    std::string GetStoreFileName();
    std::string GetDbUrl();
    std::shared_ptr<DataShare::DataShareHelper> GetDataShareHelper();

private:
    std::string pduFileName_;
    std::mutex mtx_;
    std::condition_variable cv_;
    std::shared_ptr<DataShare::DataShareHelper> datashareHelper_;
    std::string dbUrl_;
    std::string storeFileName_;
};
} // namespace Telephony
} // namespace OHOS

#endif // NAPI_MMS_PDU_HELPER_H
