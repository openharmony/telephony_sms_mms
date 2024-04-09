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

#include "satellite_sms_callback_stub.h"

#include "tel_ril_sms_parcel.h"
#include "radio_event.h"
#include "telephony_errors.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
SatelliteSmsCallbackStub::SatelliteSmsCallbackStub()
{
    TELEPHONY_LOGD("SatelliteSmsCallbackStub");
    InitFuncMap();
}

void SatelliteSmsCallbackStub::InitFuncMap()
{
    requestFuncMap_[static_cast<uint32_t>(SatelliteSmsCallbackInterfaceCode::SEND_SMS_RESPONSE)] =
        &SatelliteSmsCallbackStub::OnSendSmsResponse;
    requestFuncMap_[static_cast<uint32_t>(SatelliteSmsCallbackInterfaceCode::SMS_STATUS_REPORT_NOTIFY)] =
        &SatelliteSmsCallbackStub::OnSmsStatusReportNotify;
    requestFuncMap_[static_cast<uint32_t>(SatelliteSmsCallbackInterfaceCode::NEW_SMS_NOTIFY)] =
        &SatelliteSmsCallbackStub::OnNewSmsNotify;
}

SatelliteSmsCallbackStub::~SatelliteSmsCallbackStub()
{
    requestFuncMap_.clear();
}

int32_t SatelliteSmsCallbackStub::OnRemoteRequest(
    uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    std::u16string myDescriptor = SatelliteSmsCallbackStub::GetDescriptor();
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
    TELEPHONY_LOGD("Do not found the requestFunc of code=%{public}d, need to check", code);
    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}

int32_t SatelliteSmsCallbackStub::OnSendSmsResponse(MessageParcel &data, MessageParcel &reply)
{
    int32_t eventCode = data.ReadInt32();
    int32_t smsCallbackType = data.ReadInt32();
    if (smsCallbackType == static_cast<int32_t>(SatelliteSmsResultType::HRIL_RADIO_RESPONSE)) {
        int32_t flag = data.ReadInt32();
        int32_t serial = data.ReadInt32();
        int32_t error = data.ReadInt32();
        int32_t type = data.ReadInt32();
        auto info = std::make_shared<RadioResponseInfo>();
        if (info == nullptr) {
            TELEPHONY_LOGE("info is null!");
            return TELEPHONY_ERR_LOCAL_PTR_NULL;
        }
        info->flag = flag;
        info->serial = serial;
        info->error = static_cast<ErrType>(error);
        info->type = static_cast<ResponseTypes>(type);
        AppExecFwk::InnerEvent::Pointer response = AppExecFwk::InnerEvent::Get(eventCode, info);
        if (response == nullptr) {
            TELEPHONY_LOGE("response is null!");
            return TELEPHONY_ERR_LOCAL_PTR_NULL;
        }
        reply.WriteInt32(SendSmsResponse(response));
        return TELEPHONY_SUCCESS;
    }

    if (smsCallbackType == static_cast<int32_t>(SatelliteSmsResultType::SEND_SMS_RESULT)) {
        int32_t msgRef = data.ReadInt32();
        std::string pdu = data.ReadString();
        int32_t errCode = data.ReadInt32();
        int64_t flag = data.ReadInt64();
        auto info = std::make_shared<SendSmsResultInfo>();
        if (info == nullptr) {
            TELEPHONY_LOGE("info is null!");
            return TELEPHONY_ERR_LOCAL_PTR_NULL;
        }
        info->msgRef = msgRef;
        info->pdu = pdu;
        info->errCode = errCode;
        info->flag = flag;
        AppExecFwk::InnerEvent::Pointer response = AppExecFwk::InnerEvent::Get(eventCode, info);
        if (response == nullptr) {
            TELEPHONY_LOGE("response is null!");
            return TELEPHONY_ERR_LOCAL_PTR_NULL;
        }
        reply.WriteInt32(SendSmsResponse(response));
        return TELEPHONY_SUCCESS;
    }

    TELEPHONY_LOGE("SatelliteSmsCallbackStub: sms response is null!");
    return TELEPHONY_ERR_READ_DATA_FAIL;
}

int32_t SatelliteSmsCallbackStub::OnSmsStatusReportNotify(MessageParcel &data, MessageParcel &reply)
{
    int32_t eventCode = data.ReadInt32();

    auto info = std::make_shared<SmsMessageInfo>();
    if (info == nullptr) {
        TELEPHONY_LOGE("info is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    int32_t type = 0;
    if (!data.ReadInt32(type)) {
        TELEPHONY_LOGE("SmsMessageInfo is null!");
        return TELEPHONY_ERR_READ_DATA_FAIL;
    }
    info->indicationType = type;
    info->size = data.ReadInt32();
    data.ReadUInt8Vector(&(info->pdu));
    AppExecFwk::InnerEvent::Pointer response = AppExecFwk::InnerEvent::Get(eventCode, info);
    if (response == nullptr) {
        TELEPHONY_LOGE("response is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    reply.WriteInt32(SmsStatusReportNotify(response));
    return TELEPHONY_SUCCESS;
}

int32_t SatelliteSmsCallbackStub::OnNewSmsNotify(MessageParcel &data, MessageParcel &reply)
{
    int32_t eventCode = data.ReadInt32();

    auto info = std::make_shared<SmsMessageInfo>();
    if (info == nullptr) {
        TELEPHONY_LOGE("info is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    int32_t type = 0;
    if (!data.ReadInt32(type)) {
        TELEPHONY_LOGE("SmsMessageInfo is null!");
        return TELEPHONY_ERR_READ_DATA_FAIL;
    }
    info->indicationType = type;
    info->size = data.ReadInt32();
    data.ReadUInt8Vector(&(info->pdu));
    AppExecFwk::InnerEvent::Pointer response = AppExecFwk::InnerEvent::Get(eventCode, info);
    if (response == nullptr) {
        TELEPHONY_LOGE("response is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    reply.WriteInt32(NewSmsNotify(response));
    return TELEPHONY_SUCCESS;
}
} // namespace Telephony
} // namespace OHOS
