/*
 * Copyright (C) 2024-2024 Huawei Device Co., Ltd.
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
#ifndef __MOCK_IMS_SMS_CALLBACK_INTERFACE_H__
#define __MOCK_IMS_SMS_CALLBACK_INTERFACE_H__
#include "ims_sms_client.h"
#include "ims_sms_proxy.h"
#include "ims_sms_callback_stub.h"

namespace OHOS {
namespace Telephony {

class MockImsSmsCallback : public ImsSmsCallbackInterface {
public:
    int32_t ImsSendMessageResponse(int32_t slotId, const SendSmsResultInfo &result) override
    {
        return retImsSendMessageResponse1;
    }
    int32_t ImsSendMessageResponse(int32_t slotId, const RadioResponseInfo &info) override
    {
        return retImsSendMessageResponse2;
    }
    int32_t ImsSetSmsConfigResponse(int32_t slotId, const RadioResponseInfo &info) override
    {
        return retImsSetSmsConfigResponse;
    }
    int32_t ImsGetSmsConfigResponse(int32_t slotId, int32_t imsSmsConfig) override
    {
        return retImsGetSmsConfigResponse1;
    }
    int32_t ImsGetSmsConfigResponse(int32_t slotId, const RadioResponseInfo &info) override
    {
        return retImsGetSmsConfigResponse2;
    }
    sptr<IRemoteObject> AsObject() override
    {
        return retRemoteObject;
    }
public:
    int32_t retImsSendMessageResponse1;
    int32_t retImsSendMessageResponse2;
    int32_t retImsSetSmsConfigResponse;
    int32_t retImsGetSmsConfigResponse1;
    int32_t retImsGetSmsConfigResponse2;
    IRemoteObject* retRemoteObject;
};
}
}
#endif