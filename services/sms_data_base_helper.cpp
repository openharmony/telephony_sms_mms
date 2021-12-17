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

#include "sms_data_base_helper.h"

#include "ability_manager_interface.h"
#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "system_ability.h"
#include "system_ability_definition.h"

#include "telephony_log_wrapper.h"
#include "string_utils.h"

namespace OHOS {
namespace Telephony {
using namespace OHOS::AppExecFwk;
using namespace OHOS::AAFwk;
using namespace NativeRdb;
class AbsSharedResultSet;
SmsDataBaseHelper::SmsDataBaseHelper() {}

SmsDataBaseHelper::~SmsDataBaseHelper() {}

std::shared_ptr<AppExecFwk::DataAbilityHelper> SmsDataBaseHelper::CreateDataAHelper()
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

bool SmsDataBaseHelper::Insert(NativeRdb::ValuesBucket &values)
{
    std::shared_ptr<DataAbilityHelper> helper = CreateDataAHelper();
    if (helper == nullptr) {
        TELEPHONY_LOGE("helper is nullptr.");
        return false;
    }
    Uri uri(SMS_SUBSECTION);
    int ret = helper->Insert(uri, values);
    helper->Release();
    return ret >= 0 ? true : false;
}

bool SmsDataBaseHelper::Query(
    NativeRdb::DataAbilityPredicates &predicates, std::vector<SmsReceiveIndexer> &indexers)
{
    std::shared_ptr<DataAbilityHelper> helper = CreateDataAHelper();
    if (helper == nullptr) {
        TELEPHONY_LOGE("helper is nullptr.");
        return false;
    }
    Uri uri(SMS_SUBSECTION);
    std::vector<std::string> columns;
    std::shared_ptr<NativeRdb::AbsSharedResultSet> resultSet = helper->Query(uri, columns, predicates);
    helper->Release();

    if (resultSet == nullptr) {
        TELEPHONY_LOGE("resultSet is nullptr.");
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

bool SmsDataBaseHelper::Delete(NativeRdb::DataAbilityPredicates &predicates)
{
    std::shared_ptr<DataAbilityHelper> helper = CreateDataAHelper();
    if (helper == nullptr) {
        TELEPHONY_LOGE("helper is nullptr.");
        return false;
    }
    Uri uri(SMS_SUBSECTION);
    int ret = helper->Delete(uri, predicates);
    helper->Release();
    return ret >= 0 ? true : false;
}

void SmsDataBaseHelper::ConvertIntToIndexer(
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

void SmsDataBaseHelper::ConvertStringToIndexer(
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

    ret = resultSet->GetColumnIndex(SmsMmsData::REW_PUD, columnIndex);
    ret = resultSet->GetString(columnIndex, columnValue);
    if (ret == 0) {
        info.SetPdu(StringUtils::HexToByteVector(columnValue));
    }
}

void SmsDataBaseHelper::ResultSetConvertToIndexer(
    SmsReceiveIndexer &info, const std::shared_ptr<NativeRdb::AbsSharedResultSet> &resultSet)
{
    ConvertIntToIndexer(info, resultSet);
    ConvertStringToIndexer(info, resultSet);
}
} // namespace Telephony
} // namespace OHOS
