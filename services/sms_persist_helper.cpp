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

#include "sms_persist_helper.h"

#include "ability_manager_interface.h"
#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "system_ability.h"
#include "system_ability_definition.h"
#include "resource_manager.h"
#include "parameter.h"

#include "telephony_log_wrapper.h"
#include "string_utils.h"

namespace OHOS {
namespace Telephony {
using namespace OHOS::AppExecFwk;
using namespace OHOS::AAFwk;
using namespace NativeRdb;
class AbsSharedResultSet;
SmsPersistHelper::SmsPersistHelper() {}

SmsPersistHelper::~SmsPersistHelper() {}

std::shared_ptr<AppExecFwk::DataAbilityHelper> SmsPersistHelper::CreateDataAHelper()
{
    auto saManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (saManager == nullptr) {
        TELEPHONY_LOGE(" Get system ability mgr failed.");
        return nullptr;
    }
    auto remoteObj = saManager->GetSystemAbility(TELEPHONY_SMS_MMS_SYS_ABILITY_ID);
    if (remoteObj == nullptr) {
        TELEPHONY_LOGE("GetSystemAbility Service Failed.");
        return nullptr;
    }
    return DataAbilityHelper::Creator(remoteObj);
}

bool SmsPersistHelper::Insert(NativeRdb::ValuesBucket &values)
{
    std::shared_ptr<DataAbilityHelper> helper = CreateDataAHelper();
    if (helper == nullptr) {
        TELEPHONY_LOGE("Create Data Ability Helper nullptr Failed.");
        return false;
    }
    Uri uri(SMS_SUBSECTION);
    int ret = helper->Insert(uri, values);
    helper->Release();
    return ret >= 0 ? true : false;
}

bool SmsPersistHelper::Query(NativeRdb::DataAbilityPredicates &predicates, std::vector<SmsReceiveIndexer> &indexers)
{
    std::shared_ptr<DataAbilityHelper> helper = CreateDataAHelper();
    if (helper == nullptr) {
        TELEPHONY_LOGE("Create Data Ability Helper nullptr Failed.");
        return false;
    }
    Uri uri(SMS_SUBSECTION);
    std::vector<std::string> columns;
    std::shared_ptr<NativeRdb::AbsSharedResultSet> resultSet = helper->Query(uri, columns, predicates);
    helper->Release();
    if (resultSet == nullptr) {
        TELEPHONY_LOGE("Query Result Set nullptr Failed.");
        return false;
    }

    int resultSetNum = resultSet->GoToFirstRow();
    while (resultSetNum == 0) {
        SmsReceiveIndexer indexer;
        ResultSetConvertToIndexer(indexer, resultSet);
        indexers.push_back(indexer);
        resultSetNum = resultSet->GoToNextRow();
    }
    resultSet->Close();
    return true;
}

bool SmsPersistHelper::Delete(NativeRdb::DataAbilityPredicates &predicates)
{
    std::shared_ptr<DataAbilityHelper> helper = CreateDataAHelper();
    if (helper == nullptr) {
        TELEPHONY_LOGE("Create Data Ability Helper nullptr Failed.");
        return false;
    }
    Uri uri(SMS_SUBSECTION);
    int ret = helper->Delete(uri, predicates);
    helper->Release();
    return ret >= 0 ? true : false;
}

bool SmsPersistHelper::QueryBlockPhoneNumber(const std::string &phoneNum)
{
    bool result = false;
    int count = 0;
    const string phoneNumber = "phone_number";
    if (phoneNum.empty()) {
        return result;
    }
    std::shared_ptr<DataAbilityHelper> helper = CreateDataAHelper();
    if (helper == nullptr) {
        TELEPHONY_LOGE("Create Data Ability Helper nullptr Failed.");
        return false;
    }
    Uri uri(CONTACT_BLOCK);
    std::vector<std::string> columns;
    NativeRdb::DataAbilityPredicates predicates;
    predicates.EqualTo(phoneNumber, phoneNum);
    std::shared_ptr<NativeRdb::AbsSharedResultSet> resultSet = helper->Query(uri, columns, predicates);
    helper->Release();
    if (resultSet == nullptr) {
        TELEPHONY_LOGE("Query Result Set nullptr Failed.");
        return result;
    }
    if (resultSet->GetRowCount(count) == 0 && count != 0) {
        result = true;
    }
    resultSet->Close();
    return result;
}

bool SmsPersistHelper::QueryParamBoolean(const std::string key, bool defValue)
{
    const int PARAM_SIZE = 64;
    char paramOutBuff[PARAM_SIZE] = {0};
    std::string defStrValue = defValue ? "1" : "0";
    std::string paramValue(defStrValue);

    int retLen = GetParameter(key.c_str(), defStrValue.c_str(), paramOutBuff, PARAM_SIZE);
    if (retLen > 0) {
        paramOutBuff[retLen] = '\0';
        paramValue.assign(paramOutBuff, retLen);
    }

    TELEPHONY_LOGI("QueryParamBoolean: %{public}zu : %{public}s", paramValue.size(), paramValue.c_str());
    if ((paramValue == std::string("1")) || (paramValue == std::string("y")) || (paramValue == std::string("yes")) ||
        (paramValue == std::string("on")) || (paramValue == std::string("true"))) {
        return true;
    } else if ((paramValue == std::string("0")) || (paramValue == std::string("n")) ||
        (paramValue == std::string("no")) || (paramValue == std::string("off")) ||
        (paramValue == std::string("false"))) {
        return false;
    }
    return defValue;
}

void SmsPersistHelper::ConvertIntToIndexer(
    SmsReceiveIndexer &info, const std::shared_ptr<NativeRdb::AbsSharedResultSet> &resultSet)
{
    int32_t columnInt;
    int columnIndex;
    resultSet->GetColumnIndex(SmsMmsData::FORMAT, columnIndex);
    if (resultSet->GetInt(columnIndex, columnInt) == 0) {
        info.SetIsCdma(columnInt != 0);
    }

    resultSet->GetColumnIndex(SmsMmsData::SMS_SUBSECTION_ID, columnIndex);
    if (resultSet->GetInt(columnIndex, columnInt) == 0) {
        info.SetMsgRefId(columnInt);
    }

    resultSet->GetColumnIndex(SmsMmsData::SIZE, columnIndex);
    if (resultSet->GetInt(columnIndex, columnInt) == 0) {
        info.SetMsgCount(columnInt);
    }

    resultSet->GetColumnIndex(SmsMmsData::SUBSECTION_INDEX, columnIndex);
    if (resultSet->GetInt(columnIndex, columnInt) == 0) {
        info.SetMsgSeqId(columnInt);
    }

    resultSet->GetColumnIndex(SmsMmsData::DEST_PORT, columnIndex);
    if (resultSet->GetInt(columnIndex, columnInt) == 0) {
        info.SetDestPort(columnInt);
    }
}

void SmsPersistHelper::ConvertStringToIndexer(
    SmsReceiveIndexer &info, const std::shared_ptr<NativeRdb::AbsSharedResultSet> &resultSet)
{
    int columnIndex;
    std::string columnValue;
    resultSet->GetColumnIndex(SmsMmsData::RECEIVER_NUMBER, columnIndex);
    if (resultSet->GetString(columnIndex, columnValue) == 0) {
        info.SetVisibleAddress(columnValue);
    }
    resultSet->GetColumnIndex(SmsMmsData::SENDER_NUMBER, columnIndex);
    if (resultSet->GetString(columnIndex, columnValue) == 0) {
        info.SetOriginatingAddress(columnValue);
    }

    resultSet->GetColumnIndex(SmsMmsData::START_TIME, columnIndex);
    if (resultSet->GetString(columnIndex, columnValue) == 0) {
        info.SetTimestamp(std::stol(columnValue));
    }
    resultSet->GetColumnIndex(SmsMmsData::END_TIME, columnIndex);
    if (resultSet->GetString(columnIndex, columnValue) == 0) {
        info.SetTimestamp(std::stol(columnValue));
    }

    resultSet->GetColumnIndex(SmsMmsData::RAW_PUD, columnIndex);
    if (resultSet->GetString(columnIndex, columnValue) == 0) {
        info.SetPdu(StringUtils::HexToByteVector(columnValue));
    }
}

void SmsPersistHelper::ResultSetConvertToIndexer(
    SmsReceiveIndexer &info, const std::shared_ptr<NativeRdb::AbsSharedResultSet> &resultSet)
{
    ConvertIntToIndexer(info, resultSet);
    ConvertStringToIndexer(info, resultSet);
}
} // namespace Telephony
} // namespace OHOS
