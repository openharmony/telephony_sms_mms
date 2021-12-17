/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#ifndef SMS_DATA_BASE_HELPER_H
#define SMS_DATA_BASE_HELPER_H

#include "singleton.h"

#include "data_ability_helper.h"
#include "data_ability_predicates.h"
#include "values_bucket.h"
#include "abs_shared_result_set.h"

#include "sms_receive_indexer.h"
#include "sms_mms_data.h"

namespace OHOS {
namespace Telephony {
class SmsDataBaseHelper {
    DECLARE_DELAYED_SINGLETON(SmsDataBaseHelper)
public:
    bool Insert(NativeRdb::ValuesBucket &values);
    bool Query(NativeRdb::DataAbilityPredicates &predicates, std::vector<SmsReceiveIndexer> &indexers);
    bool Delete(NativeRdb::DataAbilityPredicates &predicates);

private:
    const std::string SMS_SUBSECTION = "dataability:///com.ohos.smsmmsability/sms_mms/sms_subsection";

    std::shared_ptr<AppExecFwk::DataAbilityHelper> CreateDataAHelper();
    void ResultSetConvertToIndexer(
        SmsReceiveIndexer &info, const std::shared_ptr<NativeRdb::AbsSharedResultSet> &resultSet);
    void ConvertIntToIndexer(
        SmsReceiveIndexer &info, const std::shared_ptr<NativeRdb::AbsSharedResultSet> &resultSet);
    void ConvertStringToIndexer(
        SmsReceiveIndexer &info, const std::shared_ptr<NativeRdb::AbsSharedResultSet> &resultSet);
};
} // namespace Telephony
} // namespace OHOS
#endif
