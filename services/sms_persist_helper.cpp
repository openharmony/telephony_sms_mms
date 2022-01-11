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

bool SmsPersistHelper::QueryResourceBoolean(const std::string key, bool defValue)
{
    std::unique_ptr<Global::Resource::ResourceManager> resManager(Global::Resource::CreateResourceManager());
    if (resManager == nullptr) {
        TELEPHONY_LOGE("create resourceManager failed!");
        return defValue;
    }

    bool result = resManager->AddResource(RES_PATH.c_str());
    if (!result) {
        TELEPHONY_LOGE("addResource failed!");
        return defValue;
    }

    bool value = defValue;
    if (resManager->GetBooleanByName(key.c_str(), value) != OHOS::Global::Resource::RState::SUCCESS) {
        TELEPHONY_LOGE("get sms config capable fail");
        return defValue;
    }
    return value;
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
    int32_t ret = resultSet->GetColumnIndex(SmsMmsData::FORMAT, columnIndex);
    ret = resultSet->GetInt(columnIndex, columnInt);
    if (ret == 0) {
        info.SetIsCdma(columnInt != 0);
    }

    ret = resultSet->GetColumnIndex(SmsMmsData::SMS_SUBSECTION_ID, columnIndex);
    ret = resultSet->GetInt(columnIndex, columnInt);
    if (ret == 0) {
        info.SetMsgRefId(columnInt);
    }

    ret = resultSet->GetColumnIndex(SmsMmsData::SIZE, columnIndex);
    ret = resultSet->GetInt(columnIndex, columnInt);
    if (ret == 0) {
        info.SetMsgCount(columnInt);
    }

    ret = resultSet->GetColumnIndex(SmsMmsData::SUBSECTION_INDEX, columnIndex);
    ret = resultSet->GetInt(columnIndex, columnInt);
    if (ret == 0) {
        info.SetMsgSeqId(columnInt);
    }

    ret = resultSet->GetColumnIndex(SmsMmsData::DEST_PORT, columnIndex);
    ret = resultSet->GetInt(columnIndex, columnInt);
    if (ret == 0) {
        info.SetDestPort(columnInt);
    }
}

void SmsPersistHelper::ConvertStringToIndexer(
    SmsReceiveIndexer &info, const std::shared_ptr<NativeRdb::AbsSharedResultSet> &resultSet)
{
    int columnIndex;
    std::string columnValue;
    int32_t ret = resultSet->GetColumnIndex(SmsMmsData::RECEIVER_NUMBER, columnIndex);
    ret = resultSet->GetString(columnIndex, columnValue);
    if (ret == 0) {
        info.SetVisibleAddress(columnValue);
    }
    ret = resultSet->GetColumnIndex(SmsMmsData::SENDER_NUMBER, columnIndex);
    ret = resultSet->GetString(columnIndex, columnValue);
    if (ret == 0) {
        info.SetOriginatingAddress(columnValue);
    }

    ret = resultSet->GetColumnIndex(SmsMmsData::START_TIME, columnIndex);
    ret = resultSet->GetString(columnIndex, columnValue);
    if (ret == 0) {
        info.SetTimestamp(std::stol(columnValue));
    }
    ret = resultSet->GetColumnIndex(SmsMmsData::END_TIME, columnIndex);
    ret = resultSet->GetString(columnIndex, columnValue);
    if (ret == 0) {
        info.SetTimestamp(std::stol(columnValue));
    }

    ret = resultSet->GetColumnIndex(SmsMmsData::RAW_PUD, columnIndex);
    ret = resultSet->GetString(columnIndex, columnValue);
    if (ret == 0) {
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
