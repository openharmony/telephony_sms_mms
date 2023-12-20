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

#ifndef APN_INFO_H
#define APN_INFO_H

#include "datashare_helper.h"
#include "datashare_predicates.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace Telephony {
class MmsApnInfo {
public:
    explicit MmsApnInfo(int32_t slotId);
    ~MmsApnInfo();
    void getMmsApn();
    std::shared_ptr<DataShare::DataShareHelper> CreatePdpProfileHelper();
    void PdpProfileSelect(const std::shared_ptr<DataShare::DataShareHelper> &helper);
    std::string getMmscUrl();
    void setMmscUrl(std::string mmscUrl);
    std::string getMmsProxyAddressAndProxyPort();
    void setMmsProxyAddressAndProxyPort(std::string mmsProxyAddressAndProxyPort);
    bool SplitAndMatchApnTypes(std::string apn);
    bool GetMmsApnValue(std::shared_ptr<DataShare::ResultSet> resultSet, int count, std::string &homeUrlVal,
        std::string &mmsIPAddressVal);

private:
    std::shared_ptr<DataShare::DataShareHelper> CreateDataAHelper(
        int32_t systemAbilityId, const std::string &dataAbilityUri) const;

private:
    int32_t slotId_ = -1;
    std::string mmscUrl_;
    std::string mmsProxyAddressAndProxyPort_;
    std::shared_ptr<DataShare::DataShareHelper> mmsPdpProfileDataAbilityHelper = nullptr;
};
} // namespace Telephony
} // namespace OHOS
#endif