/*
 * Copyright (C) 2021-2024 Huawei Device Co., Ltd.
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
#include "parameter.h"
#include "phonenumbers/phonenumber.pb.h"
#include "resource_manager.h"
#include "string_utils.h"
#include "system_ability.h"
#include "system_ability_definition.h"
#include "telephony_errors.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
constexpr static uint8_t SMS_TYPE_CDMA = 2;
const std::string SMS_URI = "datashare:///com.ohos.smsmmsability";
const std::string SMS_SUBSECTION = "datashare:///com.ohos.smsmmsability/sms_mms/sms_subsection";
const std::string SMS_MMS_INFO = "datashare:///com.ohos.smsmmsability/sms_mms/sms_mms_info";
const std::string SMS_SESSION = "datashare:///com.ohos.smsmmsability/sms_mms/session";
const std::string CONTACT_URI = "datashare:///com.ohos.contactsdataability";
const std::string CONTACT_BLOCK = "datashare:///com.ohos.contactsdataability/contacts/contact_blocklist";
const std::string CONTACT_DATA = "datashare:///com.ohos.contactsdataability/contacts/contact_data";
const std::string RAW_CONTACT = "datashare:///com.ohos.contactsdataability/contacts/raw_contact";
const std::string ISO_COUNTRY_CODE = "CN";
const std::string PHONE_NUMBER = "phone_number";
const std::string DETAIL_INFO = "detail_info";
const std::string TYPE_ID = "type_id";
std::string ID = "id";
const std::string RAW_CONTACT_ID = "raw_contact_id";
const std::string CONTACTED_COUNT = "contacted_count";
const std::string LASTEST_CONTACTED_TIME = "lastest_contacted_time";
constexpr static uint8_t TYPE_ID_VALUE = 5;
const std::string PREFIX = "+86";
const std::string NUMBER_START_STR = "192";

SmsPersistHelper::SmsPersistHelper() {}

SmsPersistHelper::~SmsPersistHelper() {}

std::shared_ptr<DataShare::DataShareHelper> SmsPersistHelper::CreateDataShareHelper(const std::string &uri)
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
    return DataShare::DataShareHelper::Creator(remoteObj, uri);
}

bool SmsPersistHelper::Insert(DataShare::DataShareValuesBucket &values, uint16_t &dataBaseId)
{
    std::shared_ptr<DataShare::DataShareHelper> helper = CreateDataShareHelper(SMS_URI);
    if (helper == nullptr) {
        TELEPHONY_LOGE("Create Data Ability Helper nullptr Failed.");
        return false;
    }
    Uri uri(SMS_SUBSECTION);
    int ret = helper->Insert(uri, values);
    helper->Release();
    dataBaseId = ret;
    return ret >= 0 ? true : false;
}

bool SmsPersistHelper::Insert(std::string tableUri, DataShare::DataShareValuesBucket &values)
{
    std::shared_ptr<DataShare::DataShareHelper> helper = CreateDataShareHelper(SMS_URI);
    if (helper == nullptr) {
        TELEPHONY_LOGE("Create Data Ability Helper nullptr Failed.");
        return false;
    }
    Uri uri(tableUri);
    int ret = helper->Insert(uri, values);
    helper->Release();
    return ret >= 0 ? true : false;
}

bool SmsPersistHelper::QuerySession(
    DataShare::DataSharePredicates &predicates, uint16_t &sessionId, uint16_t &messageCount)
{
    std::shared_ptr<DataShare::DataShareHelper> helper = CreateDataShareHelper(SMS_URI);
    if (helper == nullptr) {
        TELEPHONY_LOGE("Create Data Ability Helper nullptr Failed.");
        return false;
    }
    Uri uri(SMS_SESSION);
    std::vector<std::string> columns;
    auto resultSet = helper->Query(uri, predicates, columns);
    if (resultSet == nullptr) {
        TELEPHONY_LOGE("Query Result Set nullptr Failed.");
        helper->Release();
        return false;
    }
    resultSet->GoToFirstRow();
    int32_t columnInt;
    int columnIndex;
    resultSet->GetColumnIndex("id", columnIndex);
    if (resultSet->GetInt(columnIndex, columnInt) == 0) {
        sessionId = columnInt;
    }
    resultSet->GetColumnIndex("message_count", columnIndex);
    if (resultSet->GetInt(columnIndex, columnInt) == 0) {
        messageCount = columnInt;
        resultSet->Close();
        helper->Release();
        return true;
    }
    resultSet->Close();
    helper->Release();
    return false;
}

bool SmsPersistHelper::Update(DataShare::DataSharePredicates &predicates, DataShare::DataShareValuesBucket &values)
{
    std::shared_ptr<DataShare::DataShareHelper> helper = CreateDataShareHelper(SMS_URI);
    if (helper == nullptr) {
        TELEPHONY_LOGE("Create Data Ability Helper nullptr Failed.");
        return false;
    }
    Uri uri(SMS_SESSION);
    int ret = helper->Update(uri, predicates, values);
    helper->Release();
    return ret >= 0 ? true : false;
}

bool SmsPersistHelper::Query(DataShare::DataSharePredicates &predicates, std::vector<SmsReceiveIndexer> &indexers)
{
    std::shared_ptr<DataShare::DataShareHelper> helper = CreateDataShareHelper(SMS_URI);
    if (helper == nullptr) {
        TELEPHONY_LOGE("Create Data Ability Helper nullptr Failed.");
        return false;
    }
    Uri uri(SMS_SUBSECTION);
    std::vector<std::string> columns;
    auto resultSet = helper->Query(uri, predicates, columns);
    if (resultSet == nullptr) {
        TELEPHONY_LOGE("Query Result Set nullptr Failed.");
        helper->Release();
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
    helper->Release();
    return true;
}

bool SmsPersistHelper::QueryMaxGroupId(DataShare::DataSharePredicates &predicates, uint16_t &maxGroupId)
{
    std::shared_ptr<DataShare::DataShareHelper> helper = CreateDataShareHelper(SMS_URI);
    if (helper == nullptr) {
        TELEPHONY_LOGE("Create Data Ability Helper nullptr Failed.");
        return false;
    }
    Uri uri(SMS_MMS_INFO);
    std::vector<std::string> columns;
    auto resultSet = helper->Query(uri, predicates, columns);
    if (resultSet == nullptr) {
        TELEPHONY_LOGE("Query Result Set nullptr Failed.");
        helper->Release();
        return false;
    }

    int32_t columnInt;
    int columnIndex;
    resultSet->GoToLastRow();
    resultSet->GetColumnIndex(SmsMmsInfo::GROUP_ID, columnIndex);
    if (resultSet->GetInt(columnIndex, columnInt) == 0) {
        maxGroupId = columnInt;
    }
    resultSet->Close();
    helper->Release();
    return true;
}

bool SmsPersistHelper::Delete(DataShare::DataSharePredicates &predicates)
{
    std::shared_ptr<DataShare::DataShareHelper> helper = CreateDataShareHelper(SMS_URI);
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
    if (phoneNum.empty()) {
        return result;
    }
    return result;
}

int32_t SmsPersistHelper::FormatSmsNumber(const std::string &num, std::string countryCode,
    const i18n::phonenumbers::PhoneNumberUtil::PhoneNumberFormat formatInfo, std::string &formatNum)
{
    if (num.empty()) {
        TELEPHONY_LOGE("num is nullptr!");
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }
    i18n::phonenumbers::PhoneNumberUtil *phoneUtils = i18n::phonenumbers::PhoneNumberUtil::GetInstance();
    if (phoneUtils == nullptr) {
        TELEPHONY_LOGE("phoneUtils is nullptr");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    transform(countryCode.begin(), countryCode.end(), countryCode.begin(), ::toupper);
    i18n::phonenumbers::PhoneNumber parseResult;
    phoneUtils->Parse(num, countryCode, &parseResult);
    if (phoneUtils->IsValidNumber(parseResult)) {
        phoneUtils->Format(parseResult, formatInfo, &formatNum);
    } else {
        std::string numTemp = "";
        numTemp.assign(num);
        CbnFormat(numTemp, formatInfo, formatNum);
    }
    if (formatNum.empty() || formatNum == "0") {
        TELEPHONY_LOGE("FormatSmsNumber failed!");
        return TELEPHONY_ERROR;
    }
    TrimSpace(formatNum);
    return TELEPHONY_SUCCESS;
}

void SmsPersistHelper::CbnFormat(std::string &numTemp,
    const i18n::phonenumbers::PhoneNumberUtil::PhoneNumberFormat formatInfo, std::string &formatNum)
{
    TELEPHONY_LOGD("into CbnFormat");
    if (numTemp.substr(0, NUMBER_START_STR.size()) == NUMBER_START_STR ||
        numTemp.substr(PREFIX.size(), NUMBER_START_STR.size()) == NUMBER_START_STR) {
        if (formatInfo == i18n::phonenumbers::PhoneNumberUtil::PhoneNumberFormat::NATIONAL) {
            if (numTemp.substr(0, PREFIX.size()) == PREFIX) {
                numTemp.erase(0, PREFIX.size());
                formatNum.assign(numTemp);
                return;
            }
            formatNum.assign(numTemp);
            return;
        }
        if (formatInfo == i18n::phonenumbers::PhoneNumberUtil::PhoneNumberFormat::INTERNATIONAL) {
            if (numTemp.substr(0, PREFIX.size()) == PREFIX) {
                formatNum.assign(numTemp);
                return;
            }
            formatNum.assign(PREFIX + numTemp);
        }
    }
}

void SmsPersistHelper::TrimSpace(std::string &num)
{
    std::string word;
    std::stringstream streamNum(num);
    std::string store;
    while (streamNum >> word) {
        store += word;
    }
    num = store;
}

bool SmsPersistHelper::UpdateContact(const std::string &address)
{
    bool result = false;
    if (address.empty()) {
        TELEPHONY_LOGE("address empty");
        return result;
    }
    int32_t rawCountId = 0;
    int32_t contactedCount = 0;
    bool ret = QueryContactedCount(address, rawCountId, contactedCount);
    if (!ret) {
        TELEPHONY_LOGE("get contacted count fail");
        return result;
    }
    TELEPHONY_LOGI("rawCountId:%{public}d, contactedCount:%{public}d", rawCountId, contactedCount);
    std::shared_ptr<DataShare::DataShareHelper> helper = CreateDataShareHelper(CONTACT_URI);
    if (helper == nullptr) {
        TELEPHONY_LOGE("Create Data Ability Helper nullptr Failed.");
        return false;
    }
    std::time_t timep;
    int64_t currentTime = time(&timep);
    Uri uri(RAW_CONTACT);
    DataShare::DataSharePredicates predicates;
    predicates.EqualTo(ID, rawCountId);
    DataShare::DataShareValuesBucket bucket;
    bucket.Put(CONTACTED_COUNT, contactedCount + 1);
    bucket.Put(LASTEST_CONTACTED_TIME, std::to_string(currentTime));
    auto updateValue = helper->Update(uri, predicates, bucket);
    TELEPHONY_LOGI("updateValue:%{public}d", updateValue);
    helper->Release();
    return updateValue >= 0 ? true : false;
}

bool SmsPersistHelper::QueryContactedCount(const std::string &address, int32_t &rawCountId, int32_t &contactedCount)
{
    bool ret = QueryRawContactId(address, rawCountId);
    if (!ret) {
        TELEPHONY_LOGE("no sms address in contact");
        return ret;
    }
    std::shared_ptr<DataShare::DataShareHelper> helper = CreateDataShareHelper(CONTACT_URI);
    if (helper == nullptr) {
        TELEPHONY_LOGE("Create Data Ability Helper nullptr Failed.");
        return false;
    }
    Uri uri(RAW_CONTACT);
    std::vector<std::string> columns;
    DataShare::DataSharePredicates predicates;
    predicates.EqualTo(ID, rawCountId);
    auto resultSet = helper->Query(uri, predicates, columns);
    bool result = false;
    if (resultSet == nullptr) {
        TELEPHONY_LOGE("result set nullptr.");
        helper->Release();
        return result;
    }
    int32_t count = 0;
    if (resultSet->GetRowCount(count) == 0 && count != 0) {
        resultSet->GoToFirstRow();
        int columnIndex;
        resultSet->GetColumnIndex(CONTACTED_COUNT, columnIndex);
        if (resultSet->GetInt(columnIndex, contactedCount) == 0) {
            result = true;
        }
    }
    resultSet->Close();
    helper->Release();
    return result;
}

bool SmsPersistHelper::QueryRawContactId(const std::string &address, int32_t &rawCountId)
{
    std::shared_ptr<DataShare::DataShareHelper> helper = CreateDataShareHelper(CONTACT_URI);
    if (helper == nullptr) {
        TELEPHONY_LOGE("Create Data Ability Helper nullptr Failed.");
        return false;
    }
    Uri uri(CONTACT_DATA);
    std::vector<std::string> columns;
    DataShare::DataSharePredicates predicates;
    std::string nationalNum;
    std::string internationalNum;
    int32_t ret = FormatSmsNumber(
        address, ISO_COUNTRY_CODE, i18n::phonenumbers::PhoneNumberUtil::PhoneNumberFormat::NATIONAL, nationalNum);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Phone Number format Failed.");
        nationalNum = address;
    }
    ret = FormatSmsNumber(address, ISO_COUNTRY_CODE,
        i18n::phonenumbers::PhoneNumberUtil::PhoneNumberFormat::INTERNATIONAL, internationalNum);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Phone Number format Failed.");
        internationalNum = address;
    }
    predicates.EqualTo(DETAIL_INFO, nationalNum)->Or()->EqualTo(DETAIL_INFO, internationalNum);
    predicates.EqualTo(TYPE_ID, TYPE_ID_VALUE);
    auto resultSet = helper->Query(uri, predicates, columns);
    bool result = false;
    if (resultSet == nullptr) {
        TELEPHONY_LOGE("result set nullptr.");
        helper->Release();
        return result;
    }
    int32_t count = 0;
    if (resultSet->GetRowCount(count) == 0 && count != 0) {
        resultSet->GoToFirstRow();
        int columnIndex;
        resultSet->GetColumnIndex(RAW_CONTACT_ID, columnIndex);
        if (resultSet->GetInt(columnIndex, rawCountId) == 0) {
            result = true;
        }
    }
    resultSet->Close();
    helper->Release();
    return result;
}

bool SmsPersistHelper::QueryParamBoolean(const std::string key, bool defValue)
{
    const int PARAM_SIZE = 64;
    char paramOutBuff[PARAM_SIZE] = { 0 };
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
    SmsReceiveIndexer &info, const std::shared_ptr<DataShare::DataShareResultSet> &resultSet)
{
    int32_t columnInt;
    int columnIndex;
    resultSet->GetColumnIndex(SmsSubsection::FORMAT, columnIndex);
    if (resultSet->GetInt(columnIndex, columnInt) == 0) {
        info.SetIsCdma(columnInt == SMS_TYPE_CDMA);
    }

    resultSet->GetColumnIndex(SmsSubsection::SMS_SUBSECTION_ID, columnIndex);
    if (resultSet->GetInt(columnIndex, columnInt) == 0) {
        info.SetMsgRefId(columnInt);
    }

    resultSet->GetColumnIndex(SmsSubsection::SIZE, columnIndex);
    if (resultSet->GetInt(columnIndex, columnInt) == 0) {
        info.SetMsgCount(columnInt);
    }

    resultSet->GetColumnIndex(SmsSubsection::SUBSECTION_INDEX, columnIndex);
    if (resultSet->GetInt(columnIndex, columnInt) == 0) {
        info.SetMsgSeqId(columnInt);
    }

    resultSet->GetColumnIndex(SmsSubsection::DEST_PORT, columnIndex);
    if (resultSet->GetInt(columnIndex, columnInt) == 0) {
        info.SetDestPort(columnInt);
    }

    resultSet->GetColumnIndex(SmsSubsection::ID, columnIndex);
    if (resultSet->GetInt(columnIndex, columnInt) == 0) {
        info.SetDataBaseId(columnInt);
    }
}

void SmsPersistHelper::ConvertStringToIndexer(
    SmsReceiveIndexer &info, const std::shared_ptr<DataShare::DataShareResultSet> &resultSet)
{
    int columnIndex;
    std::string columnValue;
    resultSet->GetColumnIndex(SmsSubsection::RECEIVER_NUMBER, columnIndex);
    if (resultSet->GetString(columnIndex, columnValue) == 0) {
        info.SetVisibleAddress(columnValue);
    }
    resultSet->GetColumnIndex(SmsSubsection::SENDER_NUMBER, columnIndex);
    if (resultSet->GetString(columnIndex, columnValue) == 0) {
        info.SetOriginatingAddress(columnValue);
    }

    resultSet->GetColumnIndex(SmsSubsection::START_TIME, columnIndex);
    if (resultSet->GetString(columnIndex, columnValue) == 0) {
        info.SetTimestamp(std::stol(columnValue));
    }
    resultSet->GetColumnIndex(SmsSubsection::END_TIME, columnIndex);
    if (resultSet->GetString(columnIndex, columnValue) == 0) {
        info.SetTimestamp(std::stol(columnValue));
    }

    resultSet->GetColumnIndex(SmsSubsection::REW_PUD, columnIndex);
    if (resultSet->GetString(columnIndex, columnValue) == 0) {
        info.SetPdu(StringUtils::HexToByteVector(columnValue));
    }
}

void SmsPersistHelper::ResultSetConvertToIndexer(
    SmsReceiveIndexer &info, const std::shared_ptr<DataShare::DataShareResultSet> &resultSet)
{
    ConvertIntToIndexer(info, resultSet);
    ConvertStringToIndexer(info, resultSet);
}
} // namespace Telephony
} // namespace OHOS
