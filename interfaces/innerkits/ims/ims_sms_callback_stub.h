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

#ifndef TELEPHONY_IMS_SMS_CALLBACK_STUB_H
#define TELEPHONY_IMS_SMS_CALLBACK_STUB_H

#include <map>
#include "iremote_stub.h"
#include "ims_sms_callback_interface.h"
#include "event_handler.h"
#include "event_runner.h"

namespace OHOS {
namespace Telephony {
class ImsSmsCallbackStub : public IRemoteStub<ImsSmsCallbackInterface> {
public:
    ImsSmsCallbackStub();
    virtual ~ImsSmsCallbackStub();
    void InitFuncMap();
    void InitSmsBasicFuncMap();
    int OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;

    /****************** sms basic ******************/
    int32_t ImsSendMessageResponse(int32_t slotId, const SendSmsResultInfo &result) override;
    int32_t ImsSetSmsConfigResponse(const ImsResponseInfo &info) override;
    int32_t ImsGetSmsConfigResponse(const ImsResponseInfo &info, int32_t imsSmsConfig) override;
    
private:
    /****************** sms basic ******************/
    int32_t OnImsSendMessageResponseInner(MessageParcel &data, MessageParcel &reply);
    int32_t OnImsSetSmsConfigResponseInner(MessageParcel &data, MessageParcel &reply);
    int32_t OnImsGetSmsConfigResponseInner(MessageParcel &data, MessageParcel &reply);
    
private:
    using RequestFuncType = int32_t (ImsSmsCallbackStub::*)(MessageParcel &data, MessageParcel &reply);
    std::map<uint32_t, RequestFuncType> requestFuncMap_;
    std::shared_ptr<AppExecFwk::EventHandler> handler_[2];
};
} // namespace Telephony
} // namespace OHOS

#endif // TELEPHONY_IMS_SMS_CALLBACK_STUB_H
