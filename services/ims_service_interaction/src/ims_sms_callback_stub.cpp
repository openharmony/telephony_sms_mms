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
#include "ims_sms_client.h"

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
    requestFuncMap_[IMS_SET_SMS_CONFIG] = &ImsSmsCallbackStub::OnImsSetSmsConfigResponseInner;
    requestFuncMap_[IMS_GET_SMS_CONFIG] = &ImsSmsCallbackStub::OnImsGetSmsConfigResponseInner;
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
    int32_t slotId = data.ReadInt32();
    auto result = (SendSmsResultInfo *)data.ReadRawData(sizeof(SendSmsResultInfo));
    if (result == nullptr) {
        TELEPHONY_LOGE("OnImsSendMessageResponseInner return, result is nullptr.");
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }
    reply.WriteInt32(ImsSendMessageResponse(slotId, *result));
    return TELEPHONY_SUCCESS;
}

int32_t ImsSmsCallbackStub::OnImsSetSmsConfigResponseInner(MessageParcel &data, MessageParcel &reply)
{
    TELEPHONY_LOGI("ImsSmsCallbackStub::OnImsSetSmsConfigResponseInner entry");
    int32_t slotId = data.ReadInt32();
    auto info = (HRilRadioResponseInfo *)data.ReadRawData(sizeof(HRilRadioResponseInfo));
    if (info == nullptr) {
        TELEPHONY_LOGE("OnImsSetSmsConfigResponseInner return, info is nullptr.");
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }
    reply.WriteInt32(ImsSetSmsConfigResponse(slotId, *info));
    return TELEPHONY_SUCCESS;
}

int32_t ImsSmsCallbackStub::OnImsGetSmsConfigResponseInner(MessageParcel &data, MessageParcel &reply)
{
    TELEPHONY_LOGI("ImsSmsCallbackStub::OnImsGetSmsConfigResponseInner entry");
    int32_t slotId = data.ReadInt32();
    int32_t imsSmsConfig = data.ReadInt32();
    reply.WriteInt32(ImsGetSmsConfigResponse(slotId, imsSmsConfig));
    return TELEPHONY_SUCCESS;
}

int32_t ImsSmsCallbackStub::ImsSendMessageResponse(int32_t slotId, const SendSmsResultInfo &result)
{
    TELEPHONY_LOGI("ImsSmsCallbackStub::ImsSendMessageResponse entry");
    std::unique_ptr<SendSmsResultInfo> sendSmsResultInfo = std::make_unique<SendSmsResultInfo>();
    if (sendSmsResultInfo == nullptr) {
        TELEPHONY_LOGE("make_unique SendSmsResultInfo failed!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    *sendSmsResultInfo = result;
    uint32_t item = RadioEvent::RADIO_SEND_IMS_GSM_SMS;
    DelayedSingleton<ImsSmsClient>::GetInstance()->GetHandler(slotId)->SendEvent(
        item, std::move(sendSmsResultInfo));
    return TELEPHONY_SUCCESS;
}

int32_t ImsSmsCallbackStub::ImsSetSmsConfigResponse(int32_t slotId, const HRilRadioResponseInfo &info)
{
    TELEPHONY_LOGI("ImsSmsCallbackStub::ImsSetSmsConfigResponse error:%{public}d", info.error);
    uint32_t item = RadioEvent::RADIO_SET_IMS_SMS;
    std::unique_ptr<HRilRadioResponseInfo> para = std::make_unique<HRilRadioResponseInfo>();
    if (para == nullptr) {
        TELEPHONY_LOGE("make_unique HRilRadioResponseInfo failed!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    *para = info;
    DelayedSingleton<ImsSmsClient>::GetInstance()->GetHandler(slotId)->SendEvent(item, std::move(para));
    return TELEPHONY_SUCCESS;
}

int32_t ImsSmsCallbackStub::ImsGetSmsConfigResponse(int32_t slotId, int32_t imsSmsConfig)
{
    TELEPHONY_LOGI("ImsSmsCallbackStub::ImsGetSmsConfigResponse entry");
    std::unique_ptr<int32_t> imsSmsCfg = std::make_unique<int32_t>();
    if (imsSmsCfg == nullptr) {
        TELEPHONY_LOGE("make_unique imsSmsConfig failed!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    *imsSmsCfg = imsSmsConfig;
    uint32_t item = RadioEvent::RADIO_GET_IMS_SMS;
    DelayedSingleton<ImsSmsClient>::GetInstance()->GetHandler(slotId)->SendEvent(
        item, std::move(imsSmsCfg));
    return TELEPHONY_SUCCESS;
}
} // namespace Telephony
} // namespace OHOS
