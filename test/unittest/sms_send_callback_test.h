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

#ifndef SMS_SEND_CALLBACK_TEST_H
#define SMS_SEND_CALLBACK_TEST_H

#include <cstdint>

#include "iremote_stub.h"

#include "i_send_short_message_callback.h"

namespace OHOS {
namespace Telephony {
class SmsSendCallbackTest : public IRemoteStub<ISendShortMessageCallback> {
public:
    SmsSendCallbackTest() = default;
    virtual ~SmsSendCallbackTest() = default;
    virtual void OnSmsSendResult(const ISendShortMessageCallback::SmsSendResult result) override;
    int OnRemoteRequest(
        uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override final;
};
} // namespace Telephony
} // namespace OHOS
#endif // SMS_SEND_CALLBACK_TEST_H