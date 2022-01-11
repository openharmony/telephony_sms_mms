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

#include "telephony_log_wrapper.h"
#include "sms_service.h"

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
    result.append("Ohos sms_mms service: ")
        .append(",    Ohos sms_mms bind time:  ")
        .append(DelayedSingleton<SmsService>::GetInstance()->GetBindTime())
        .append("\n");
}
} // namespace Telephony
} // namespace OHOS
