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

#ifndef GSM_SMS_RECEIVE_HANDLER_H
#define GSM_SMS_RECEIVE_HANDLER_H

#include "gsm_sms_cb_handler.h"
#ifdef SMS_MMS_SATELLITE
#include "satellite_sms_callback.h"
#endif // SMS_MMS_SATELLITE
#include "sms_receive_handler.h"

namespace OHOS {
namespace Telephony {
class GsmSmsReceiveHandler : public SmsReceiveHandler {
public:
    explicit GsmSmsReceiveHandler(int32_t slotId);
    ~GsmSmsReceiveHandler() override;
    void Init();
    void UnRegisterHandler();
#ifdef SMS_MMS_SATELLITE
    void RegisterSatelliteCallback();
    void UnregisterSatelliteCallback();
#endif // SMS_MMS_SATELLITE

protected:
    virtual int32_t HandleSmsByType(const std::shared_ptr<SmsBaseMessage> smsBaseMessage) override;
    virtual int32_t HandleAck(const std::shared_ptr<SmsBaseMessage> smsBaseMessage) override;
    virtual void HandleRemainDataShare(const std::shared_ptr<SmsBaseMessage> smsBaseMessage) override;
    virtual bool ReplySmsToSmsc(int result) override;
    virtual std::shared_ptr<SmsBaseMessage> TransformMessageInfo(const std::shared_ptr<SmsMessageInfo> info) override;

private:
    bool RegisterHandler();
    int32_t CheckSmsSupport();
    int32_t HandleNormalSmsByType(const std::shared_ptr<SmsBaseMessage> smsBaseMessage);

private:
    std::shared_ptr<GsmSmsCbHandler> smsCbHandler_;
#ifdef SMS_MMS_SATELLITE
    sptr<ISatelliteSmsCallback> satelliteCallback_ = nullptr;
#endif // SMS_MMS_SATELLITE
};
} // namespace Telephony
} // namespace OHOS
#endif