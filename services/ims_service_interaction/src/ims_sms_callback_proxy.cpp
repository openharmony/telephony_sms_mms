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
    MessageOption option;
    MessageParcel in;
    MessageParcel out;
    if (!in.WriteInterfaceToken(ImsSmsCallbackProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor token fail!");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!in.WriteInt32(slotId)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!in.WriteRawData((const void *)&result, sizeof(SendSmsResultInfo))) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    int32_t error = Remote()->SendRequest(IMS_SEND_MESSAGE, in, out, option);
    if (error == ERR_NONE) {
        return out.ReadInt32();
    }
    return error;
}

int32_t ImsSmsCallbackProxy::ImsSetSmsConfigResponse(int32_t slotId, const HRilRadioResponseInfo &info)
{
    MessageOption option;
    MessageParcel in;
    MessageParcel out;
    if (!in.WriteInterfaceToken(ImsSmsCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!in.WriteInt32(slotId)) {
        TELEPHONY_LOGE("write slotId fail!");
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!in.WriteRawData((const void *)&info, sizeof(HRilRadioResponseInfo))) {
        TELEPHONY_LOGE("write HRilRadioResponseInfo fail!");
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    int32_t error = Remote()->SendRequest(IMS_SET_SMS_CONFIG, in, out, option);
    if (error == ERR_NONE) {
        return out.ReadInt32();
    }
    return error;
}

int32_t ImsSmsCallbackProxy::ImsGetSmsConfigResponse(int32_t slotId, int32_t imsSmsConfig)
{
    MessageOption option;
    MessageParcel in;
    MessageParcel out;
    if (!in.WriteInterfaceToken(ImsSmsCallbackProxy::GetDescriptor())) {
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!in.WriteInt32(slotId)) {
        TELEPHONY_LOGE("write slotId fail!");
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!in.WriteInt32(imsSmsConfig)) {
        TELEPHONY_LOGE("write imsSmsConfig fail!");
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    int32_t error = Remote()->SendRequest(IMS_GET_SMS_CONFIG, in, out, option);
    if (error == ERR_NONE) {
        return out.ReadInt32();
    }
    return error;
}
} // namespace Telephony
} // namespace OHOS
