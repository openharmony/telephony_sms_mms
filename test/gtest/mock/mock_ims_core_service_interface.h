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
#ifndef __MOCK_IMS_CORE_SERVICE_INTERFACE_H__
#define __MOCK_IMS_CORE_SERVICE_INTERFACE_H__
#include "ims_sms_client.h"
#include "ims_sms_proxy.h"
#include "ims_sms_callback_stub.h"

namespace OHOS {
namespace Telephony {

class MockImsCoreService : public ImsCoreServiceInterface {
public:
    int32_t GetImsRegistrationStatus(int32_t slotId) override
    {
        return retGetImsRegistrationStatus;
    }
    int32_t RegisterImsCoreServiceCallback(const sptr<ImsCoreServiceCallbackInterface> &callback) override
    {
        return retRegisterImsCoreServiceCallback;
    }
    sptr<IRemoteObject> GetProxyObjectPtr(ImsServiceProxyType proxyType) override
    {
        return retGetProxyObjectPtr;
    }
    int32_t GetPhoneNumberFromIMPU(int32_t slotId, std::string &phoneNumber) override
    {
        return retGetPhoneNumberFromIMPU;
    }
    sptr<IRemoteObject> AsObject() override
    {
        return retRemoteObject;
    }
public:
    int32_t retGetImsRegistrationStatus;
    int32_t retRegisterImsCoreServiceCallback;
    IRemoteObject* retGetProxyObjectPtr;
    int32_t retGetPhoneNumberFromIMPU;
    IRemoteObject* retRemoteObject;
};
}
}
#endif