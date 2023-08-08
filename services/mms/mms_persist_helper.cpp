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

#include "mms_persist_helper.h"

#include "gsm_pdu_hex_value.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
const std::string SMS_PROFILE_URI = "datashare:///com.ohos.smsmmsability";
const std::string SMS_PROFILE_MMS_PDU_URI = "datashare:///com.ohos.smsmmsability/sms_mms/mms_pdu";
static constexpr const char *PDU_CONTENT = "pdu_content";
static constexpr const char *ID = "id";
static constexpr uint8_t SLIDE_STEP = 2;

std::shared_ptr<DataShare::DataShareHelper> MmsPersistHelper::CreateSmsHelper()
{
    if (mmsPduDataAbilityHelper_ == nullptr) {
        mmsPduDataAbilityHelper_ = CreateDataAHelper(TELEPHONY_SMS_MMS_SYS_ABILITY_ID, SMS_PROFILE_URI);
    }
    return mmsPduDataAbilityHelper_;
}

std::shared_ptr<DataShare::DataShareHelper> MmsPersistHelper::CreateDataAHelper(
    int32_t systemAbilityId, const std::string &dataAbilityUri) const
{
    auto saManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (saManager == nullptr) {
        TELEPHONY_LOGE("Get system ability mgr failed");
        return nullptr;
    }
    auto remoteObj = saManager->GetSystemAbility(systemAbilityId);
    if (remoteObj == nullptr) {
        TELEPHONY_LOGE("GetSystemAbility Service Failed");
        return nullptr;
    }
    return DataShare::DataShareHelper::Creator(remoteObj, dataAbilityUri);
}

void MmsPersistHelper::DeleteMmsPdu(const std::string &dbUrl)
{
    std::shared_ptr<DataShare::DataShareHelper> helper = CreateSmsHelper();
    if (helper == nullptr) {
        TELEPHONY_LOGE("helper is nullptr");
        return;
    }

    Uri uri(SMS_PROFILE_MMS_PDU_URI);

    DataShare::DataSharePredicates predicates;
    predicates.EqualTo("id", dbUrl);
    int32_t result = helper->Delete(uri, predicates);
    helper->Release();
    mmsPdu_ = "";
    TELEPHONY_LOGI("result:%{public}d", result);
}

bool MmsPersistHelper::UpdateMmsPdu(const std::string &mmsPdu, const std::string &dbUrl)
{
    std::shared_ptr<DataShare::DataShareHelper> helper = CreateSmsHelper();
    if (helper == nullptr) {
        TELEPHONY_LOGE("helper is nullptr");
        return false;
    }

    Uri uri(SMS_PROFILE_MMS_PDU_URI);
    DataShare::DataShareValuesBucket bucket;
    std::string targetMmsPdu;
    for (size_t i = 0; i < mmsPdu.size(); i++) {
        targetMmsPdu += static_cast<char>((mmsPdu[i] & 0x0F) | 0xF0);
        targetMmsPdu += static_cast<char>((mmsPdu[i] & 0xF0) | 0x0F);
    }
    bucket.Put(PDU_CONTENT, targetMmsPdu);
    DataShare::DataSharePredicates predicates;
    predicates.EqualTo(ID, dbUrl);
    int32_t result = helper->Update(uri, predicates, bucket);
    helper->Release();
    TELEPHONY_LOGI("result:%{public}d", result);
    return result >= 0 ? true : false;
}

std::string MmsPersistHelper::GetMmsPdu(const std::string &dbUrl)
{
    if (!QueryMmsPdu(dbUrl)) {
        return "";
    }
    return mmsPdu_;
}

void MmsPersistHelper::SetMmsPdu(const std::string &mmsPdu)
{
    mmsPdu_ = mmsPdu;
}

bool MmsPersistHelper::QueryMmsPdu(const std::string &dbUrl)
{
    std::shared_ptr<DataShare::DataShareHelper> helper = CreateSmsHelper();
    if (helper == nullptr) {
        TELEPHONY_LOGE("helper is nullptr");
        return false;
    }
    Uri uri(SMS_PROFILE_MMS_PDU_URI);
    std::vector<std::string> colume;
    DataShare::DataSharePredicates predicates;
    predicates.EqualTo(ID, dbUrl);
    auto resultSet = helper->Query(uri, predicates, colume);
    if (resultSet == nullptr) {
        TELEPHONY_LOGE("Query Result Set nullptr Failed.");
        helper->Release();
        return false;
    }
    int count = 0;
    resultSet->GetRowCount(count);
    if (count <= 0) {
        TELEPHONY_LOGE("MmsPdu count: %{public}d error", count);
        resultSet->Close();
        helper->Release();
        return false;
    }
    int columnIndex;
    std::vector<uint8_t> blobValue;
    resultSet->GoToFirstRow();
    resultSet->GetColumnIndex(PDU_CONTENT, columnIndex);
    resultSet->GetBlob(columnIndex, blobValue);
    resultSet->Close();
    helper->Release();
    std::string mmsPdu;
    char pduChar = 0x00;
    for (size_t i = 0; i + 1 < blobValue.size(); i = i + SLIDE_STEP) {
        pduChar = (blobValue[i] & HEX_VALUE_0F) | (blobValue[i + 1] & HEX_VALUE_F0);
        mmsPdu += static_cast<char>(pduChar);
    }
    TELEPHONY_LOGI("blob len:%{public}d, mmsPdu len:%{public}d", static_cast<uint32_t>(blobValue.size()),
        static_cast<uint32_t>(mmsPdu.size()));
    SetMmsPdu(mmsPdu);
    return true;
}
} // namespace Telephony
} // namespace OHOS