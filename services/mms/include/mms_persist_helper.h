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

#ifndef MMS_PDU_SERVICE_H
#define MMS_PDU_SERVICE_H

#include "datashare_helper.h"
#include "datashare_predicates.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace Telephony {
class MmsPersistHelper {
public:
    MmsPersistHelper() = default;
    ~MmsPersistHelper() = default;
    std::string GetMmsPdu(const std::string &dbUrl);
    void DeleteMmsPdu(const std::string &dbUrl);
    bool InsertMmsPdu(const std::string &mmsPdu, std::string &dbUrl);

private:
    std::shared_ptr<DataShare::DataShareHelper> CreateSmsHelper();
    std::shared_ptr<DataShare::DataShareHelper> CreateDataAHelper(
        int32_t systemAbilityId, const std::string &dataAbilityUri) const;
    bool QueryMmsPdu(const std::string &dbUrl);
    void SetMmsPdu(const std::string &mmsPdu);
    std::vector<std::string> SplitPdu(const std::string &mmsPdu);
    std::vector<std::string> SplitUrl(std::string url);

private:
    std::string mmsPdu_;
    std::shared_ptr<DataShare::DataShareHelper> mmsPduDataShareHelper_ = nullptr;
};
} // namespace Telephony
} // namespace OHOS
#endif