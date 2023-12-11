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

#ifndef SMS_PERSIST_HELPER_H
#define SMS_PERSIST_HELPER_H

#include "datashare_helper.h"
#include "datashare_predicates.h"
#include "phonenumberutil.h"
#include "singleton.h"
#include "sms_mms_data.h"
#include "sms_receive_indexer.h"

namespace OHOS {
namespace Telephony {
class SmsPersistHelper {
    DECLARE_DELAYED_SINGLETON(SmsPersistHelper)
public:
    bool Insert(DataShare::DataShareValuesBucket &values, uint16_t &dataBaseId);
    bool Insert(std::string tableUri, DataShare::DataShareValuesBucket &values);
    bool Query(DataShare::DataSharePredicates &predicates, std::vector<SmsReceiveIndexer> &indexers);
    bool Delete(DataShare::DataSharePredicates &predicates);
    bool Update(DataShare::DataSharePredicates &predicates, DataShare::DataShareValuesBucket &values);
    bool QueryBlockPhoneNumber(const std::string &phoneNum);
    bool QueryParamBoolean(const std::string key, bool defValue);
    bool QueryMaxGroupId(DataShare::DataSharePredicates &predicates, uint16_t &maxGroupId);
    bool QuerySession(DataShare::DataSharePredicates &predicates, uint16_t &sessionId, uint16_t &messageCount);
    bool UpdateContact(const std::string &address);

    inline static const std::string SMS_CAPABLE_KEY = "sms_config_capable";
    inline static const std::string SMS_ENCODING_KEY = "sms_config_force_7bit_encoding";
    inline static const std::string SMS_CAPABLE_PARAM_KEY = "const.telephony.sms.capable";
    inline static const std::string SMS_ENCODING_PARAM_KEY = "persist.sys.sms.config.7bitforce";

private:
    std::shared_ptr<DataShare::DataShareHelper> CreateDataShareHelper(const std::string &uri);
    void ResultSetConvertToIndexer(
        SmsReceiveIndexer &info, const std::shared_ptr<DataShare::DataShareResultSet> &resultSet);
    void ConvertIntToIndexer(
        SmsReceiveIndexer &info, const std::shared_ptr<DataShare::DataShareResultSet> &resultSet);
    void ConvertStringToIndexer(
        SmsReceiveIndexer &info, const std::shared_ptr<DataShare::DataShareResultSet> &resultSet);
    int32_t FormatSmsNumber(const std::string &num, std::string countryCode,
        const i18n::phonenumbers::PhoneNumberUtil::PhoneNumberFormat formatInfo, std::string &formatNum);
    void TrimSpace(std::string &num);
    bool QueryContactedCount(const std::string &address, int32_t &rawCountId, int32_t &contactedCount);
    bool QueryRawContactId(const std::string &address, int32_t &rawCountId);
    void CbnFormat(std::string &numTemp, const i18n::phonenumbers::PhoneNumberUtil::PhoneNumberFormat formatInfo,
        std::string &formatNum);
};
} // namespace Telephony
} // namespace OHOS
#endif
