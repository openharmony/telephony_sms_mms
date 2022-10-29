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

#include "sendmessagedata_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <string_ex.h>

#include "addsmstoken_fuzzer.h"
#include "delivery_send_call_back_stub.h"
#include "napi/native_api.h"
#include "send_call_back_stub.h"
#include "sms_service_manager_client.h"
#include "system_ability_definition.h"

using namespace OHOS::Telephony;
namespace OHOS {
void DoSomethingInterestingWithMyAPI(const uint8_t *data, size_t size)
{
    if (data == nullptr || size <= 0) {
        return;
    }

    auto smsServerClient = DelayedSingleton<SmsServiceManagerClient>::GetInstance();
    if (!smsServerClient) {
        return;
    }

    std::unique_ptr<SendCallbackStub> sendCallBack = std::make_unique<SendCallbackStub>();

    std::unique_ptr<DeliverySendCallbackStub> deliveryCallBack = std::make_unique<DeliverySendCallbackStub>();
    int32_t slotId = static_cast<int32_t>(size % 2);

    std::string desAddr(reinterpret_cast<const char *>(data), size);
    std::string scAddr(reinterpret_cast<const char *>(data), size);

    smsServerClient->SendMessage(slotId, Str8ToStr16(desAddr), Str8ToStr16(scAddr), size, data, size,
        sendCallBack.release(), deliveryCallBack.release());

    return;
}
}  // namespace OHOS
/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    OHOS::AddSmsTokenFuzzer token;
    /* Run your code on data */
    OHOS::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}
