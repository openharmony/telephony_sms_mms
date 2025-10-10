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

#ifndef ANI_SEND_CALL_BACK_H
#define ANI_SEND_CALL_BACK_H

#include "ani.h"
#include "ani_sms.h"
#include "send_short_message_callback_stub.h"

namespace OHOS {
namespace Telephony {
struct SendCallbackContext {
    ani_env *env = nullptr;
    ani_ref callbackRef = nullptr;
    SendSmsResult result = SendSmsResult::SEND_SMS_FAILURE_UNKNOWN;
};
class AniSendCallback : public SendShortMessageCallbackStub {
public:
    AniSendCallback(bool hasCallback, ani_env *env, ani_ref callbackRef);
    ~AniSendCallback();
    void OnSmsSendResult(const ISendShortMessageCallback::SmsSendResult result) override;

private:
    bool hasCallback_;
    ani_env *env_;
    ani_ref callbackRef_;
    SendSmsResult WrapSmsSendResult(const ISendShortMessageCallback::SmsSendResult result);
};
} // namespace Telephony
} // namespace OHOS
#endif // ANI_SEND_CALL_BACK_H