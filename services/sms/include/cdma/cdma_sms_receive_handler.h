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

#ifndef CDMA_SMS_RECEIVE_HANDLER_H
#define CDMA_SMS_RECEIVE_HANDLER_H

#include "app/fa_context.h"
#include "sms_cb_data.h"
#include "sms_receive_handler.h"
#include "sms_sender.h"

namespace OHOS {
namespace Telephony {
class CdmaSmsReceiveHandler : public SmsReceiveHandler {
public:
    explicit CdmaSmsReceiveHandler(int32_t slotId);
    virtual ~CdmaSmsReceiveHandler() override = default;
    void SetCdmaSender(const std::weak_ptr<SmsSender> &smsSender);
    void Init();
    void UnRegisterHandler();

protected:
    virtual int32_t HandleSmsByType(const std::shared_ptr<SmsBaseMessage> smsBaseMessage) override;
    virtual int32_t HandleAck(const std::shared_ptr<SmsBaseMessage> smsBaseMessage) override;
    virtual void HandleRemainDataShare(const std::shared_ptr<SmsBaseMessage> smsBaseMessage) override;
    virtual bool ReplySmsToSmsc(int result) override;
    virtual std::shared_ptr<SmsBaseMessage> TransformMessageInfo(const std::shared_ptr<SmsMessageInfo> info) override;

private:
    bool RegisterHandler();
    bool SendCBBroadcast(const std::shared_ptr<SmsBaseMessage> smsBaseMessage);
    bool SetCBBroadcastParam(AppExecFwk::Want &want, SmsCbData::CbData &sendData);
    int32_t HandleSmsOtherSvcid(const std::shared_ptr<SmsBaseMessage> smsBaseMessage);
    void GetCBData(
        const std::shared_ptr<SmsBaseMessage> smsBaseMessage, SmsCbData::CbData &sendData, bool &isEmergency);

private:
    std::weak_ptr<SmsSender> cdmaSmsSender_;
    std::u16string plmn_;
};
} // namespace Telephony
} // namespace OHOS
#endif