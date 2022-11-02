/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#ifndef TELEPHONY_IMS_SMS_CALLBACK_PROXY_H
#define TELEPHONY_IMS_SMS_CALLBACK_PROXY_H

#include "ims_sms_callback_interface.h"
#include "iremote_proxy.h"
#include "telephony_errors.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
class ImsSmsCallbackProxy : public IRemoteProxy<ImsSmsCallbackInterface> {
public:
    explicit ImsSmsCallbackProxy(const sptr<IRemoteObject> &impl);
    virtual ~ImsSmsCallbackProxy() = default;

    /****************** sms basic ******************/
    int32_t ImsSendMessageResponse(int32_t slotId, const SendSmsResultInfo &result) override;
    int32_t ImsSendMessageResponse(int32_t slotId, const HRilRadioResponseInfo &info) override;
    int32_t ImsSetSmsConfigResponse(int32_t slotId, const HRilRadioResponseInfo &info) override;
    int32_t ImsGetSmsConfigResponse(int32_t slotId, int32_t imsSmsConfig) override;
    int32_t ImsGetSmsConfigResponse(int32_t slotId, const HRilRadioResponseInfo &info) override;

private:
    int32_t WriteCommonInfo(std::string funcName, MessageParcel &in, int32_t slotId);
    int32_t SendHRilRadioResponseInfo(
        std::string funcName, int32_t slotId, int32_t eventId, const HRilRadioResponseInfo &info);
    int32_t SendRequest(MessageParcel &in, int32_t slotId, int32_t eventId);

private:
    static inline BrokerDelegator<ImsSmsCallbackProxy> delegator_;
};
} // namespace Telephony
} // namespace OHOS

#endif // TELEPHONY_IMS_SMS_CALLBACK_PROXY_H
