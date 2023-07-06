/*
 * Copyright (C) 2021-2023 Huawei Device Co., Ltd.
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

#ifndef I_DELIVERY_SHORT_MESSAGE_CALLBACK_H
#define I_DELIVERY_SHORT_MESSAGE_CALLBACK_H

#include "delivery_short_message_callback_ipc_interface_code.h"
#include "iremote_broker.h"

namespace OHOS {
namespace Telephony {
class IDeliveryShortMessageCallback : public IRemoteBroker {
public:
    virtual ~IDeliveryShortMessageCallback() = default;

    /**
     * @brief OnSmsDeliveryResult
     * Callback to indicate the status of the sent SMS on the recipient.
     *
     * @param pdu [in], pdu of message.
     */
    virtual void OnSmsDeliveryResult(const std::u16string &pdu) = 0;

public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.Telephony.IDeliveryShortMessageCallback");
};
} // namespace Telephony
} // namespace OHOS
#endif