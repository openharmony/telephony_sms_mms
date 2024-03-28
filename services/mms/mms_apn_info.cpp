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

#include "mms_apn_info.h"

#include "core_manager_inner.h"
#include "pdp_profile_data.h"
#include "string_utils.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
const std::string PDP_PROFILE_NET_URI = "datashare:///com.ohos.pdpprofileability/net/pdp_profile?simId=";
const std::string MMS_APN_TYPE = "mms";
const std::string ALL_APN_TYPE = "*";

MmsApnInfo::MmsApnInfo(int32_t slotId) : slotId_(slotId)
{
    getMmsApn();
}

MmsApnInfo::~MmsApnInfo() {}

void MmsApnInfo::getMmsApn()
{
    std::shared_ptr<DataShare::DataShareHelper> helper = CreatePdpProfileHelper();
    if (helper == nullptr) {
        TELEPHONY_LOGE("getMmsApn helper is nullptr");
        return;
    }
    PdpProfileSelect(helper);
}

std::shared_ptr<DataShare::DataShareHelper> MmsApnInfo::CreatePdpProfileHelper()
{
    if (mmsPdpProfileDataAbilityHelper == nullptr) {
        mmsPdpProfileDataAbilityHelper = CreateDataAHelper(TELEPHONY_SMS_MMS_SYS_ABILITY_ID, PDP_PROFILE_URI);
    }
    return mmsPdpProfileDataAbilityHelper;
}

std::shared_ptr<DataShare::DataShareHelper> MmsApnInfo::CreateDataAHelper(
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

bool MmsApnInfo::SplitAndMatchApnTypes(std::string apn)
{
    std::vector<std::string> apns;
    size_t pos = 0;
    size_t found = 0;
    while ((found = apn.find(',', pos)) != std::string::npos) {
        apns.push_back(apn.substr(pos, found - pos));
        pos = found + 1;
    }
    apns.push_back(apn.substr(pos));

    for (size_t i = 0; i < apns.size(); i++) {
        if (apns[i] == MMS_APN_TYPE || (apns[i] == ALL_APN_TYPE && apns.size() == 1)) {
            TELEPHONY_LOGI("ApnType match success");
            return true;
        }
    }
    return false;
}

bool MmsApnInfo::GetMmsApnValue(
    std::shared_ptr<DataShare::ResultSet> resultSet, int count, std::string &homeUrlVal, std::string &mmsIPAddressVal)
{
    int columnIndex;
    std::string apn;
    for (int row = 0; row < count; row++) {
        resultSet->GoToRow(row);
        resultSet->GetColumnIndex(PdpProfileData::APN_TYPES, columnIndex);
        resultSet->GetString(columnIndex, apn);
        if (SplitAndMatchApnTypes(apn)) {
            resultSet->GetColumnIndex(PdpProfileData::HOME_URL, columnIndex);
            resultSet->GetString(columnIndex, homeUrlVal);
            resultSet->GetColumnIndex(PdpProfileData::MMS_IP_ADDRESS, columnIndex);
            resultSet->GetString(columnIndex, mmsIPAddressVal);
            return true;
        }
    }
    return false;
}

void MmsApnInfo::PdpProfileSelect(const std::shared_ptr<DataShare::DataShareHelper> &helper)
{
    int32_t simId = CoreManagerInner::GetInstance().GetSimId(slotId_);
    Uri uri(static_cast<std::string>(PDP_PROFILE_NET_URI) + std::to_string(simId));
    std::vector<std::string> colume;
    DataShare::DataSharePredicates predicates;
    std::u16string operatorNumeric;
    CoreManagerInner::GetInstance().GetSimOperatorNumeric(slotId_, operatorNumeric);
    std::string mccmnc = StringUtils::ToUtf8(operatorNumeric);
    if (mccmnc.empty()) {
        TELEPHONY_LOGE("mccmnc is empty");
        return;
    }
    TELEPHONY_LOGI("query mms apn data base");
    predicates.EqualTo(PdpProfileData::MCCMNC, mccmnc);
    auto resultSet = helper->Query(uri, predicates, colume);
    if (resultSet == nullptr) {
        TELEPHONY_LOGE("resultSet nullptr");
        helper->Release();
        return;
    }
    int count;
    resultSet->GetRowCount(count);
    if (count <= 0) {
        TELEPHONY_LOGE("count: %{public}d null return", count);
        resultSet->Close();
        helper->Release();
        return;
    }
    std::string homeUrlVal;
    std::string mmsIPAddressVal;
    if (GetMmsApnValue(resultSet, count, homeUrlVal, mmsIPAddressVal)) {
        setMmscUrl(homeUrlVal);
        setMmsProxyAddressAndProxyPort(mmsIPAddressVal);
    } else {
        TELEPHONY_LOGI("homeUrlVal and mmsIPAddressVal not matched");
    }
    resultSet->Close();
    helper->Release();
}

std::string MmsApnInfo::getMmscUrl()
{
    return mmscUrl_;
}

void MmsApnInfo::setMmscUrl(std::string mmscUrl)
{
    mmscUrl_ = mmscUrl;
}

std::string MmsApnInfo::getMmsProxyAddressAndProxyPort()
{
    return mmsProxyAddressAndProxyPort_;
}

void MmsApnInfo::setMmsProxyAddressAndProxyPort(std::string mmsProxyAddressAndProxyPort)
{
    mmsProxyAddressAndProxyPort_ = mmsProxyAddressAndProxyPort;
}
} // namespace Telephony
} // namespace OHOS