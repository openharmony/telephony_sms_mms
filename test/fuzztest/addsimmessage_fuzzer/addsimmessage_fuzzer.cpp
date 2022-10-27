/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "addsimmessage_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <string_ex.h>

#include "addsmstoken_fuzzer.h"
#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "napi_util.h"
#include "sms_service_interface_death_recipient.h"
#include "sms_service_manager_client.h"
#include "system_ability_definition.h"
#include "telephony_log_wrapper.h"

using namespace OHOS::Telephony;
namespace OHOS {
bool DoSomethingInterestingWithMyAPI(const uint8_t *data, size_t size)
{
    if (data == nullptr || size <= 0) {
        return false;
    }

    auto smsServerClient = DelayedSingleton<SmsServiceManagerClient>::GetInstance();
    if (!smsServerClient) {
        return false;
    }

    std::string smsc(reinterpret_cast<const char *>(data), size);
    std::string pdu(reinterpret_cast<const char *>(data), size);
    auto smscU16 = Str8ToStr16(smsc);
    auto pduU16 = Str8ToStr16(pdu);

    int32_t soltId = static_cast<int32_t>(size % 2);
    ISmsServiceInterface::SimMessageStatus status = static_cast<ISmsServiceInterface::SimMessageStatus>(size % 4);
    bool result = smsServerClient->AddSimMessage(soltId, smscU16, pduU16, status);

    return result;
}
}  // namespace OHOS
/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::AddSmsTokenFuzzer token;
    OHOS::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}
