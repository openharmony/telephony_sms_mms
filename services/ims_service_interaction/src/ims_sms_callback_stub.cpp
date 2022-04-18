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

#include "ims_sms_callback_stub.h"

#include "telephony_log_wrapper.h"
#include "telephony_errors.h"
#include "radio_event.h"

namespace OHOS {
namespace Telephony {
ImsSmsCallbackStub::ImsSmsCallbackStub()
{
    TELEPHONY_LOGI("ImsSmsCallbackStub");
    InitFuncMap();
}

void ImsSmsCallbackStub::InitFuncMap()
{
    InitSmsBasicFuncMap();
}

void ImsSmsCallbackStub::InitSmsBasicFuncMap()
{
    /****************** sms basic ******************/
    requestFuncMap_[IMS_SEND_MESSAGE] = &ImsSmsCallbackStub::OnImsSendMessageResponseInner;
}

ImsSmsCallbackStub::~ImsSmsCallbackStub()
{
    requestFuncMap_.clear();
}

int32_t ImsSmsCallbackStub::OnRemoteRequest(
    uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    std::u16string myDescriptor = ImsSmsCallbackStub::GetDescriptor();
    std::u16string remoteDescriptor = data.ReadInterfaceToken();
    if (myDescriptor != remoteDescriptor) {
        TELEPHONY_LOGE("OnRemoteRequest return, descriptor checked fail");
        return TELEPHONY_ERR_DESCRIPTOR_MISMATCH;
    }
    auto itFunc = requestFuncMap_.find(code);
    if (itFunc != requestFuncMap_.end()) {
        auto requestFunc = itFunc->second;
        if (requestFunc != nullptr) {
            return (this->*requestFunc)(data, reply);
        }
    }
    TELEPHONY_LOGI("ImsSmsCallbackStub::OnRemoteRequest, default case, need check.");
    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}

int32_t ImsSmsCallbackStub::OnImsSendMessageResponseInner(MessageParcel &data, MessageParcel &reply)
{
    TELEPHONY_LOGI("ImsSmsCallbackStub::OnImsSendMessageResponseInner entry");
    auto info = (ImsResponseInfo *)data.ReadRawData(sizeof(ImsResponseInfo));
    if (info == nullptr) {
        TELEPHONY_LOGE("OnImsSendMessageResponseInner return, info is nullptr.");
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }
    reply.WriteInt32(ImsSendMessageResponse(*info));
    return TELEPHONY_SUCCESS;
}

int32_t ImsSmsCallbackStub::ImsSendMessageResponse(const ImsResponseInfo &info)
{
    TELEPHONY_LOGI("ImsSmsCallbackStub::ImsSendMessageResponse entry");
    return TELEPHONY_SUCCESS;
}
} // namespace Telephony
} // namespace OHOS
