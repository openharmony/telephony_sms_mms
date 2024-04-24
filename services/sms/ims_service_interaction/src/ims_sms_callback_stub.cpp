/*
 * Copyright (C) 2022-2023 Huawei Device Co., Ltd.
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

#include "ims_sms_client.h"
#include "radio_event.h"
#include "telephony_errors.h"
#include "telephony_log_wrapper.h"

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
    requestFuncMap_[static_cast<uint32_t>(ImsSmsCallbackInterfaceCode::IMS_SEND_MESSAGE)] =
        &ImsSmsCallbackStub::OnImsSendMessageResponseInner;
    requestFuncMap_[static_cast<uint32_t>(ImsSmsCallbackInterfaceCode::IMS_SET_SMS_CONFIG)] =
        &ImsSmsCallbackStub::OnImsSetSmsConfigResponseInner;
    requestFuncMap_[static_cast<uint32_t>(ImsSmsCallbackInterfaceCode::IMS_GET_SMS_CONFIG)] =
        &ImsSmsCallbackStub::OnImsGetSmsConfigResponseInner;
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
        TELEPHONY_LOGE("Descriptor check failed, return");
        return TELEPHONY_ERR_DESCRIPTOR_MISMATCH;
    }
    auto itFunc = requestFuncMap_.find(code);
    if (itFunc != requestFuncMap_.end()) {
        auto requestFunc = itFunc->second;
        if (requestFunc != nullptr) {
            return (this->*requestFunc)(data, reply);
        }
    }
    TELEPHONY_LOGE("Do not found the requestFunc of code=%{public}d, need to check", code);
    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}

int32_t ImsSmsCallbackStub::OnImsSendMessageResponseInner(MessageParcel &data, MessageParcel &reply)
{
    int32_t slotId = data.ReadInt32();
    RadioResponseInfo hRilRadioResponseInfo;
    SendSmsResultInfo sendSmsResultInfo;
    if (data.GetRawDataSize() == sizeof(RadioResponseInfo)) {
        hRilRadioResponseInfo.flag = data.ReadInt32();
        hRilRadioResponseInfo.serial = data.ReadInt32();
        hRilRadioResponseInfo.error = static_cast<const ErrType>(data.ReadInt32());
        hRilRadioResponseInfo.type = static_cast<const ResponseTypes>(data.ReadInt32());
        reply.WriteInt32(ImsSendMessageResponse(slotId, hRilRadioResponseInfo));
        return TELEPHONY_SUCCESS;
    } else {
        sendSmsResultInfo.msgRef = data.ReadInt32();
        sendSmsResultInfo.pdu = data.ReadString();
        sendSmsResultInfo.errCode = data.ReadInt32();
        sendSmsResultInfo.flag = data.ReadInt64();
        reply.WriteInt32(ImsSendMessageResponse(slotId, sendSmsResultInfo));
        return TELEPHONY_SUCCESS;
    }
}

int32_t ImsSmsCallbackStub::OnImsSetSmsConfigResponseInner(MessageParcel &data, MessageParcel &reply)
{
    int32_t slotId = data.ReadInt32();
    RadioResponseInfo hRilRadioResponseInfo;
    hRilRadioResponseInfo.flag = data.ReadInt32();
    hRilRadioResponseInfo.serial = data.ReadInt32();
    hRilRadioResponseInfo.error = static_cast<const ErrType>(data.ReadInt32());
    hRilRadioResponseInfo.type = static_cast<const ResponseTypes>(data.ReadInt32());
    reply.WriteInt32(ImsSetSmsConfigResponse(slotId, hRilRadioResponseInfo));
    return TELEPHONY_SUCCESS;
}

int32_t ImsSmsCallbackStub::OnImsGetSmsConfigResponseInner(MessageParcel &data, MessageParcel &reply)
{
    int32_t slotId = data.ReadInt32();
    int32_t imsSmsConfig = data.ReadInt32();
    reply.WriteInt32(ImsGetSmsConfigResponse(slotId, imsSmsConfig));
    return TELEPHONY_SUCCESS;
}

int32_t ImsSmsCallbackStub::ImsSendMessageResponse(int32_t slotId, const SendSmsResultInfo &result)
{
    TELEPHONY_LOGI("[slot%{public}d]Entry with SendSmsResultInfo", slotId);
    std::shared_ptr<SendSmsResultInfo> sendSmsResultInfo = std::make_shared<SendSmsResultInfo>();
    if (sendSmsResultInfo == nullptr) {
        TELEPHONY_LOGE("[slot%{public}d]make_shared SendSmsResultInfo failed", slotId);
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    *sendSmsResultInfo = result;
    uint32_t item = RadioEvent::RADIO_SEND_IMS_GSM_SMS;
    auto eventHandler = DelayedSingleton<ImsSmsClient>::GetInstance()->GetHandler(slotId);
    if (eventHandler == nullptr) {
        TELEPHONY_LOGE("eventHandler is nullptr");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    TelEventHandler::SendTelEvent(eventHandler, item, sendSmsResultInfo);
    return TELEPHONY_SUCCESS;
}

int32_t ImsSmsCallbackStub::ImsSendMessageResponse(int32_t slotId, const RadioResponseInfo &info)
{
    return SendHRilRadioResponseInfo(slotId, static_cast<uint32_t>(RadioEvent::RADIO_SEND_IMS_GSM_SMS), info);
}

int32_t ImsSmsCallbackStub::ImsSetSmsConfigResponse(int32_t slotId, const RadioResponseInfo &info)
{
    return SendHRilRadioResponseInfo(slotId, static_cast<uint32_t>(RadioEvent::RADIO_SET_IMS_SMS), info);
}

int32_t ImsSmsCallbackStub::ImsGetSmsConfigResponse(int32_t slotId, int32_t imsSmsConfig)
{
    TELEPHONY_LOGI("[slot%{public}d]Entry", slotId);
    std::shared_ptr<int32_t> imsSmsCfg = std::make_shared<int32_t>();
    if (imsSmsCfg == nullptr) {
        TELEPHONY_LOGE("[slot%{public}d]make_shared imsSmsConfig failed", slotId);
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    *imsSmsCfg = imsSmsConfig;
    uint32_t item = RadioEvent::RADIO_GET_IMS_SMS;
    auto eventHandler = DelayedSingleton<ImsSmsClient>::GetInstance()->GetHandler(slotId);
    if (eventHandler == nullptr) {
        TELEPHONY_LOGE("eventHandler is nullptr");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    TelEventHandler::SendTelEvent(eventHandler, item, imsSmsCfg);
    return TELEPHONY_SUCCESS;
}

int32_t ImsSmsCallbackStub::ImsGetSmsConfigResponse(int32_t slotId, const RadioResponseInfo &info)
{
    return SendHRilRadioResponseInfo(slotId, static_cast<uint32_t>(RadioEvent::RADIO_GET_IMS_SMS), info);
}

int32_t ImsSmsCallbackStub::SendHRilRadioResponseInfo(
    int32_t slotId, uint32_t eventId, const RadioResponseInfo &info)
{
    TELEPHONY_LOGI("[slot%{public}d]eventId=%{public}d response error:%{public}d", slotId, eventId, info.error);
    std::shared_ptr<RadioResponseInfo> hRilRadioResponseInfo = std::make_shared<RadioResponseInfo>();
    if (hRilRadioResponseInfo == nullptr) {
        TELEPHONY_LOGE("[slot%{public}d]make_shared RadioResponseInfo failed", slotId);
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    *hRilRadioResponseInfo = info;
    auto eventHandler = DelayedSingleton<ImsSmsClient>::GetInstance()->GetHandler(slotId);
    if (eventHandler == nullptr) {
        TELEPHONY_LOGE("eventHandler is nullptr");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    TelEventHandler::SendTelEvent(eventHandler, eventId, hRilRadioResponseInfo);
    return TELEPHONY_SUCCESS;
}
} // namespace Telephony
} // namespace OHOS
