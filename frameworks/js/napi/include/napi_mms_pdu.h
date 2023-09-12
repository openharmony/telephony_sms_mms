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

#ifndef NAPI_MMS_PDU_H
#define NAPI_MMS_PDU_H

#include "iservice_registry.h"
#include "napi_mms_pdu_helper.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace Telephony {
class NAPIMmsPdu {
public:
    NAPIMmsPdu() = default;
    ~NAPIMmsPdu() = default;
    void DeleteMmsPdu(NapiMmsPduHelper &helper);
    bool InsertMmsPdu(NapiMmsPduHelper &helper, const std::string &mmsPdu);
    std::string GetMmsPdu(NapiMmsPduHelper &helper);

private:
    bool QueryMmsPdu(NapiMmsPduHelper &helper);
    void SetMmsPdu(const std::string &mmsPdu);
    std::vector<std::string> SplitPdu(const std::string &mmsPdu);
    std::vector<std::string> SplitUrl(std::string url);

private:
    std::string mmsPdu_;
};
} // namespace Telephony
} // namespace OHOS
#endif
