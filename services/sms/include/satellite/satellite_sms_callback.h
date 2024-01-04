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

#ifndef SATELLITE_SMS_CALLBACK_H
#define SATELLITE_SMS_CALLBACK_H

#include <map>

#include "iremote_stub.h"
#include "satellite/satellite_sms_callback_ipc_interface_code.h"
#include "satellite_sms_callback_stub.h"
#include "tel_event_handler.h"

namespace OHOS {
namespace Telephony {
class SatelliteSmsCallback : public SatelliteSmsCallbackStub {
public:
    explicit SatelliteSmsCallback(const std::shared_ptr<TelEventHandler> &handler);
    virtual ~SatelliteSmsCallback();

    int32_t SendSmsResponse(InnerEvent::Pointer &event) override;
    int32_t SmsStatusReportNotify(InnerEvent::Pointer &event) override;
    int32_t NewSmsNotify(InnerEvent::Pointer &event) override;

private:
    std::shared_ptr<TelEventHandler> handler_;
};
} // namespace Telephony
} // namespace OHOS

#endif // SATELLITE_SMS_CALLBACK_H
