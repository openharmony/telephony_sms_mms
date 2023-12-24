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

#ifndef SATELLITE_SMS_PROXY_H
#define SATELLITE_SMS_PROXY_H

#include "satellite/i_satellite_sms_service.h"

namespace OHOS {
namespace Telephony {
class SatelliteSmsProxy : public IRemoteProxy<ISatelliteSmsService> {
public:
    explicit SatelliteSmsProxy(const sptr<IRemoteObject> &impl) : IRemoteProxy<ISatelliteSmsService>(impl) {}
    virtual ~SatelliteSmsProxy() = default;

    int32_t RegisterSmsNotify(int32_t slotId, int32_t what, const sptr<ISatelliteSmsCallback> &callback) override;
    int32_t UnRegisterSmsNotify(int32_t slotId, int32_t what) override;

    int32_t SendSms(int32_t slotId, int32_t eventId, SatelliteMessage &message) override;
    int32_t SendSmsMoreMode(int32_t slotId, int32_t eventId, SatelliteMessage &message) override;
    int32_t SendSmsAck(int32_t slotId, int32_t eventId, bool success, int32_t cause) override;

private:
    bool WriteInterfaceToken(MessageParcel &data);

private:
    static inline BrokerDelegator<SatelliteSmsProxy> delegator_;
};
} // namespace Telephony
} // namespace OHOS
#endif // SATELLITE_SMS_PROXY_H
