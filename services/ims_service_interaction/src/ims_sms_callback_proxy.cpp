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

#include "ims_sms_callback_proxy.h"

#include "message_option.h"
#include "message_parcel.h"

namespace OHOS {
namespace Telephony {
ImsSmsCallbackProxy::ImsSmsCallbackProxy(const sptr<IRemoteObject> &impl)
    : IRemoteProxy<ImsSmsCallbackInterface>(impl) {}

int32_t ImsSmsCallbackProxy::ImsSendMessageResponse(int32_t slotId, const SendSmsResultInfo &result)
{
    MessageParcel in;
    int32_t ret = WriteCommonInfo(__FUNCTION__, in, slotId);
    if (ret != TELEPHONY_SUCCESS) {
        return ret;
    }
    if (!in.WriteRawData((const void *)&result, sizeof(SendSmsResultInfo))) {
        TELEPHONY_LOGE("[slot%{public}d]Write SendSmsResultInfo fail!", slotId);
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    return SendRequest(in, slotId, IMS_SEND_MESSAGE);
}

int32_t ImsSmsCallbackProxy::ImsSendMessageResponse(int32_t slotId, const HRilRadioResponseInfo &info)
{
    return SendHRilRadioResponseInfo(__FUNCTION__, slotId, IMS_SEND_MESSAGE, info);
}

int32_t ImsSmsCallbackProxy::ImsSetSmsConfigResponse(int32_t slotId, const HRilRadioResponseInfo &info)
{
    return SendHRilRadioResponseInfo(__FUNCTION__, slotId, IMS_SET_SMS_CONFIG, info);
}

int32_t ImsSmsCallbackProxy::ImsGetSmsConfigResponse(int32_t slotId, int32_t imsSmsConfig)
{
    MessageParcel in;
    int32_t ret = WriteCommonInfo(__FUNCTION__, in, slotId);
    if (ret != TELEPHONY_SUCCESS) {
        return ret;
    }
    if (!in.WriteInt32(imsSmsConfig)) {
        TELEPHONY_LOGE("[slot%{public}d]Write imsSmsConfig fail!", slotId);
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    return SendRequest(in, slotId, IMS_GET_SMS_CONFIG);
}

int32_t ImsSmsCallbackProxy::ImsGetSmsConfigResponse(int32_t slotId, const HRilRadioResponseInfo &info)
{
    return SendHRilRadioResponseInfo(__FUNCTION__, slotId, IMS_GET_SMS_CONFIG, info);
}

int32_t ImsSmsCallbackProxy::WriteCommonInfo(std::string funcName, MessageParcel &in, int32_t slotId)
{
    if (!in.WriteInterfaceToken(ImsSmsCallbackProxy::GetDescriptor())) {
        TELEPHONY_LOGE("[slot%{public}d] %{public}s Write descriptor token fail!", slotId, funcName.c_str());
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!in.WriteInt32(slotId)) {
        TELEPHONY_LOGE("[slot%{public}d] %{public}s Write slotId fail!", slotId, funcName.c_str());
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t ImsSmsCallbackProxy::SendHRilRadioResponseInfo(
    std::string funcName, int32_t slotId, int32_t eventId, const HRilRadioResponseInfo &info)
{
    TELEPHONY_LOGI("[slot%{public}d]Send HRilRadioResponseInfo for eventId:%{public}d", slotId, eventId);
    MessageParcel in;
    int32_t ret = WriteCommonInfo(funcName, in, slotId);
    if (ret != TELEPHONY_SUCCESS) {
        return ret;
    }
    if (!in.WriteRawData((const void *)&info, sizeof(HRilRadioResponseInfo))) {
        TELEPHONY_LOGE("[slot%{public}d]Write HRilRadioResponseInfo fail! eventId:%{public}d", slotId, eventId);
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    return SendRequest(in, slotId, eventId);
}

int32_t ImsSmsCallbackProxy::SendRequest(MessageParcel &in, int32_t slotId, int32_t eventId)
{
    MessageParcel out;
    MessageOption option;

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("[slot%{public}d]Remote is null, eventId:%{public}d", slotId, eventId);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }

    int32_t error = remote->SendRequest(eventId, in, out, option);
    if (error == ERR_NONE) {
        return out.ReadInt32();
    }
    TELEPHONY_LOGE("[slot%{public}d]SendRequest fail, eventId:%{public}d, error:%{public}d", slotId, eventId, error);
    return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
}
} // namespace Telephony
} // namespace OHOS
