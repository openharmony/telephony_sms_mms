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

#include "sms_dump_helper.h"

#include "core_service_client.h"
#include "sms_service.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
bool SmsDumpHelper::Dump(const std::vector<std::string> &args, std::string &result) const
{
    result.clear();
    ShowHelp(result);
    ShowSmsInfo(result);
    return true;
}

SmsDumpHelper::SmsDumpHelper()
{
    TELEPHONY_LOGI("SmsDumpHelper() entry.");
}

static std::string to_utf8(std::u16string str16)
{
    return std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> {}.to_bytes(str16);
}

bool SmsDumpHelper::WhetherHasSimCard(const int32_t slotId) const
{
    bool hasSimCard = false;
    DelayedRefSingleton<CoreServiceClient>::GetInstance().HasSimCard(slotId, hasSimCard);
    return hasSimCard;
}

void SmsDumpHelper::ShowHelp(std::string &result) const
{
    result.append("Usage:dump <command> [options]\n")
        .append("Description:\n")
        .append("-sms_mms_info          ")
        .append("dump all sms_mms information in the system\n")
        .append("-output_slot        ")
        .append("default_slot_Id\n")
        .append("-output_service_state")
        .append("Output service state information\n");
}

void SmsDumpHelper::ShowSmsInfo(std::string &result) const
{
    result.append("SmsService: \n");
    result.append("BindTime = ");
    result.append(DelayedSingleton<SmsService>::GetInstance()->GetBindTime());
    result.append("\n");
    result.append("SpendTime = ");
    result.append(std::to_string(DelayedSingleton<SmsService>::GetInstance()->GetSpendTime()));
    result.append("\n");
    result.append("EndTime = ");
    result.append(std::to_string(DelayedSingleton<SmsService>::GetInstance()->GetEndTime()));
    result.append("\n");
    result.append("ServiceRunningState: ");
    result.append(std::to_string(DelayedSingleton<SmsService>::GetInstance()->GetServiceRunningState()));
    result.append("\n");
    result.append("DefaultSmsSlotId = ");
    result.append(std::to_string(DelayedSingleton<SmsService>::GetInstance()->GetDefaultSmsSlotId()));
    result.append("\n");
    result.append("HasSmsCapability = ");
    result.append(std::to_string(DelayedSingleton<SmsService>::GetInstance()->HasSmsCapability()));
    result.append("\n");
    for (int32_t i = 0; i < SIM_SLOT_COUNT; i++) {
        if (WhetherHasSimCard(i)) {
            result.append("SlotId = ");
            result.append(std::to_string(i));
            result.append("\n");
            result.append("IsImsSmsSupported = ");
            bool isSupported = false;
            DelayedSingleton<SmsService>::GetInstance()->IsImsSmsSupported(i, isSupported);
            result.append(std::to_string(isSupported));
            result.append("\n");
            result.append("ImsShortMessageFormat = ");
            std::u16string format;
            DelayedSingleton<SmsService>::GetInstance()->GetImsShortMessageFormat(format);
            result.append(to_utf8(format));
            result.append("\n");
        }
    }
}
} // namespace Telephony
} // namespace OHOS
