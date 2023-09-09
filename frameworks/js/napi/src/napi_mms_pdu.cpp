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

#include "napi_mms_pdu.h"

#include "ability.h"
#include "datashare_helper.h"
#include "datashare_predicates.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
const std::string SMS_PROFILE_MMS_PDU_URI = "datashare:///com.ohos.smsmmsability/sms_mms/mms_pdu";
static constexpr const char *PDU_CONTENT = "pdu_content";
static constexpr const char *ID = "id";
static constexpr uint8_t SLIDE_STEP = 2;
static constexpr uint8_t HEX_VALUE_F0 = 0xF0;
static constexpr uint8_t HEX_VALUE_0F = 0x0F;

void NAPIMmsPdu::DeleteMmsPdu(NapiMmsPduHelper &pduHelper)
{
    if (GetMmsPdu(pduHelper).empty()) {
        TELEPHONY_LOGE("mmsPdu_ is nullptr");
        return;
    }
    std::shared_ptr<DataShare::DataShareHelper> datashareHelper = pduHelper.GetDataShareHelper();
    if (datashareHelper == nullptr) {
        TELEPHONY_LOGE("datashareHelper is nullptr");
        return;
    }

    Uri uri(SMS_PROFILE_MMS_PDU_URI);

    DataShare::DataSharePredicates predicates;
    predicates.EqualTo(ID, pduHelper.GetDbUrl());
    int32_t result = datashareHelper->Delete(uri, predicates);
    mmsPdu_ = "";
    TELEPHONY_LOGI("result:%{public}d", result);
}

bool NAPIMmsPdu::InsertMmsPdu(NapiMmsPduHelper &pduHelper, const std::string &mmsPdu)
{
    std::shared_ptr<DataShare::DataShareHelper> datashareHelper = pduHelper.GetDataShareHelper();
    if (datashareHelper == nullptr) {
        TELEPHONY_LOGE("datashareHelper is nullptr");
        return false;
    }
    std::string targetMmsPdu;
    for (size_t i = 0; i < mmsPdu.size(); i++) {
        targetMmsPdu += static_cast<char>((mmsPdu[i] & 0x0F) | 0xF0);
        targetMmsPdu += static_cast<char>((mmsPdu[i] & 0xF0) | 0x0F);
    }
    Uri uri(SMS_PROFILE_MMS_PDU_URI);
    DataShare::DataShareValuesBucket bucket;
    bucket.Put(PDU_CONTENT, targetMmsPdu);
    int32_t result = datashareHelper->Insert(uri, bucket);
    std::string dbUrl = std::to_string(result);
    TELEPHONY_LOGI("insert db, dbUrl:%{public}s,pduLen:%{public}d,targetPduLen:%{public}d", dbUrl.c_str(),
        static_cast<uint32_t>(mmsPdu.size()), static_cast<uint32_t>(targetMmsPdu.size()));
    pduHelper.SetDbUrl(dbUrl);
    pduHelper.NotifyAll();
    return result >= 0 ? true : false;
}

std::string NAPIMmsPdu::GetMmsPdu(NapiMmsPduHelper &pduHelper)
{
    if (!QueryMmsPdu(pduHelper)) {
        return "";
    }
    return mmsPdu_;
}

void NAPIMmsPdu::SetMmsPdu(const std::string &mmsPdu)
{
    mmsPdu_ = mmsPdu;
}

bool NAPIMmsPdu::QueryMmsPdu(NapiMmsPduHelper &pduHelper)
{
    std::shared_ptr<DataShare::DataShareHelper> datashareHelper = pduHelper.GetDataShareHelper();
    if (datashareHelper == nullptr) {
        TELEPHONY_LOGE("datashareHelper is nullptr");
        return false;
    }
    Uri uri(SMS_PROFILE_MMS_PDU_URI);
    std::vector<std::string> colume;
    DataShare::DataSharePredicates predicates;
    predicates.EqualTo(ID, pduHelper.GetDbUrl());
    auto resultSet = datashareHelper->Query(uri, predicates, colume);
    if (resultSet == nullptr) {
        TELEPHONY_LOGE("resultSet is nullptr");
        return false;
    }
    int count;
    resultSet->GetRowCount(count);
    if (count <= 0) {
        TELEPHONY_LOGE("pdu count: %{public}d error", count);
        resultSet->Close();
        return false;
    }
    int columnIndex;
    std::vector<uint8_t> blobValue;
    for (int row = 0; row < count; row++) {
        resultSet->GoToRow(row);
        resultSet->GetColumnIndex(PDU_CONTENT, columnIndex);
        resultSet->GetBlob(columnIndex, blobValue);
    }
    resultSet->Close();
    std::string mmsPdu;
    for (size_t i = 0; i + 1 < blobValue.size(); i = i + SLIDE_STEP) {
        char pduChar = (blobValue[i] & HEX_VALUE_0F) | (blobValue[i + 1] & HEX_VALUE_F0);
        mmsPdu += static_cast<char>(pduChar);
    }
    TELEPHONY_LOGI("mmsPdu size:%{public}d", static_cast<uint32_t>(mmsPdu.size()));
    SetMmsPdu(mmsPdu);
    return true;
}
} // namespace Telephony
} // namespace OHOS