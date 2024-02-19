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
static constexpr uint32_t SPLIT_PDU_LENGTH = 195 * 1024;

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
    std::vector<std::string> dbUrls = SplitUrl(pduHelper.GetDbUrl());
    int32_t result = -1;
    for (std::string url : dbUrls) {
        predicates.EqualTo(ID, url);
        result = datashareHelper->Delete(uri, predicates);
        if (result < 0) {
            TELEPHONY_LOGE("delete mms pdu fail");
            mmsPdu_ = "";
            return;
        }
    }
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

    Uri uri(SMS_PROFILE_MMS_PDU_URI);

    std::vector<std::string> mmsPdus = SplitPdu(mmsPdu);

    std::string dbUrl;
    for (std::string mmsPdu : mmsPdus) {
        DataShare::DataShareValuesBucket bucket;
        bucket.Put(PDU_CONTENT, mmsPdu);
        int32_t result = datashareHelper->Insert(uri, bucket);
        if (result < 0) {
            TELEPHONY_LOGE("mms pdu insert fail");
            return false;
        }
        dbUrl += std::to_string(result) + ',';
    }

    TELEPHONY_LOGI("insert db, dbUrl:%{public}s", dbUrl.c_str());
    pduHelper.SetDbUrl(dbUrl);
    pduHelper.NotifyAll();
    return dbUrl.empty() ? false : true;
}

std::vector<std::string> NAPIMmsPdu::SplitPdu(const std::string &mmsPdu)
{
    std::vector<std::string> mmsPdus;
    std::string targetMmsPdu;
    for (size_t i = 0; i < mmsPdu.size(); i++) {
        targetMmsPdu += static_cast<char>((mmsPdu[i] & HEX_VALUE_0F) | HEX_VALUE_F0);
        targetMmsPdu += static_cast<char>((mmsPdu[i] & HEX_VALUE_F0) | HEX_VALUE_0F);
    }
    std::string mmsPduData;
    for (uint32_t locate = 0; locate * SPLIT_PDU_LENGTH < targetMmsPdu.size(); locate++) {
        if ((locate + 1) * SPLIT_PDU_LENGTH < targetMmsPdu.size()) {
            mmsPduData = targetMmsPdu.substr(locate * SPLIT_PDU_LENGTH, SPLIT_PDU_LENGTH);
            mmsPdus.push_back(mmsPduData);
        } else {
            mmsPduData = targetMmsPdu.substr(locate * SPLIT_PDU_LENGTH);
            mmsPdus.push_back(mmsPduData);
            break;
        }
    }
    TELEPHONY_LOGI("pduLen:%{public}zu,targetPduLen:%{public}zu", mmsPdu.size(), targetMmsPdu.size());
    return mmsPdus;
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

std::vector<std::string> NAPIMmsPdu::SplitUrl(std::string url)
{
    std::vector<std::string> dbUrls;
    while (url.size() > 0) {
        size_t locate = url.find_first_of(',');
        if (locate == 0 || locate == std::string::npos) {
            break;
        }
        dbUrls.push_back(url.substr(0, locate));
        url = url.substr(locate + 1);
    }
    return dbUrls;
}

bool NAPIMmsPdu::QueryMmsPdu(NapiMmsPduHelper &pduHelper)
{
    std::shared_ptr<DataShare::DataShareHelper> datashareHelper = pduHelper.GetDataShareHelper();
    if (datashareHelper == nullptr) {
        TELEPHONY_LOGE("datashareHelper is nullptr");
        return false;
    }
    std::vector<std::string> dbUrls = SplitUrl(pduHelper.GetDbUrl());
    std::string mmsPdu;
    std::string urlData;
    for (std::string url : dbUrls) {
        urlData.append(url + ',');
        Uri uri(SMS_PROFILE_MMS_PDU_URI);
        std::vector<std::string> colume;
        DataShare::DataSharePredicates predicates;
        predicates.EqualTo(ID, url);
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
        blobValue.pop_back();
        for (size_t i = 0; i + 1 < blobValue.size(); i = i + SLIDE_STEP) {
            char pduChar = (blobValue[i] & HEX_VALUE_0F) | (blobValue[i + 1] & HEX_VALUE_F0);
            mmsPdu += static_cast<char>(pduChar);
        }
    }
    TELEPHONY_LOGI("mmsPdu size:%{public}zu, urlData:%{public}s", mmsPdu.size(), urlData.c_str());
    SetMmsPdu(mmsPdu);
    return true;
}
} // namespace Telephony
} // namespace OHOS