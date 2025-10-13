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

#include "ani.h"
#include "delivery_short_message_callback_stub.h"

namespace OHOS {
namespace Telephony {
struct DeliveryCallbackContext {
    ani_env *env = nullptr;
    ani_ref callbackRef = nullptr;
    std::string pduStr = "";
};
class AniDeliveryCallback : public DeliveryShortMessageCallbackStub {
public:
    AniDeliveryCallback(bool hasCallback, ani_env *env, ani_ref callbackRef);
    ~AniDeliveryCallback();
    void OnSmsDeliveryResult(const std::u16string &pdu) override;

private:
    bool hasCallback_;
    ani_env *env_;
    ani_ref callbackRef_;
};
} // namespace Telephony
} // namespace OHOS

#endif // ANI_DELIVERY_CALL_BACK_H