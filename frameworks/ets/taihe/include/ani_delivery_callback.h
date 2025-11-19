/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ANI_DELIVERY_CALL_BACK_H
#define ANI_DELIVERY_CALL_BACK_H

#include "delivery_short_message_callback_stub.h"
#include "ani_callback_common.h"

namespace OHOS {
namespace Telephony {

class AniDeliveryCallback : public DeliveryShortMessageCallbackStub {
public:
    AniDeliveryCallback() = default;
    ~AniDeliveryCallback() = default;

    void OnSmsDeliveryResult(const std::u16string &pdu) override;
    bool Init(uintptr_t callbackFunc);

private:
    std::shared_ptr<AniCallbackInfo> cb_ = nullptr;
    void CompleteSmsDeliveryWork(const std::u16string &pdu);
};
} // namespace Telephony
} // namespace OHOS
#endif // ANI_DELIVERY_CALL_BACK_H