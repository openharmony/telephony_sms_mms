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

#include "ims_sms_proxy.h"

#include "message_option.h"
#include "message_parcel.h"
#include "telephony_errors.h"
#include "telephony_permission.h"

namespace OHOS {
namespace Telephony {
int32_t ImsSmsProxy::ImsSendMessage(int32_t slotId, const ImsMessageInfo &imsMessageInfo)
{
    if (!TelephonyPermission::CheckPermission(Permission::SEND_MESSAGES)) {
        TELEPHONY_LOGE("[slot%{public}d]Permission denied!", slotId);
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    MessageParcel in;
    int32_t ret = WriteCommonInfo(__FUNCTION__, in, slotId);
    if (ret != TELEPHONY_SUCCESS) {
        return ret;
    }
    if (!in.WriteRawData((const void *)&imsMessageInfo, sizeof(imsMessageInfo))) {
        TELEPHONY_LOGE("[slot%{public}d]Write imsMessageInfo fail!", slotId);
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    return SendRequest(in, slotId, IMS_GET_SMS_CONFIG);
}

int32_t ImsSmsProxy::ImsSetSmsConfig(int32_t slotId, int32_t imsSmsConfig)
{
    if (!TelephonyPermission::CheckPermission(Permission::SET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("[slot%{public}d]Permission denied!", slotId);
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
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

int32_t ImsSmsProxy::ImsGetSmsConfig(int32_t slotId)
{
    MessageParcel in;
    int32_t ret = WriteCommonInfo(__FUNCTION__, in, slotId);
    if (ret != TELEPHONY_SUCCESS) {
        return ret;
    }
    return SendRequest(in, slotId, IMS_GET_SMS_CONFIG);
}

int32_t ImsSmsProxy::RegisterImsSmsCallback(const sptr<ImsSmsCallbackInterface> &callback)
{
    if (!TelephonyPermission::CheckPermission(Permission::SET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("Permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    if (callback == nullptr) {
        TELEPHONY_LOGE("callback is nullptr!");
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }
    MessageOption option;
    MessageParcel in;
    MessageParcel out;
    if (!in.WriteInterfaceToken(ImsSmsProxy::GetDescriptor())) {
        TELEPHONY_LOGE("Write descriptor token fail!");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!in.WriteRemoteObject(callback->AsObject().GetRefPtr())) {
        TELEPHONY_LOGE("Write ImsSmsCallbackInterface fail!");
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("Remote is null");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }

    int32_t error = remote->SendRequest(IMS_SMS_REGISTER_CALLBACK, in, out, option);
    if (error == ERR_NONE) {
        return out.ReadInt32();
    }
    TELEPHONY_LOGE("SendRequest fail, error:%{public}d", error);
    return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
}

int32_t ImsSmsProxy::WriteCommonInfo(std::string funcName, MessageParcel &in, int32_t slotId)
{
    if (!in.WriteInterfaceToken(ImsSmsProxy::GetDescriptor())) {
        TELEPHONY_LOGE("[slot%{public}d] %{public}s Write descriptor token fail!", slotId, funcName.c_str());
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!in.WriteInt32(slotId)) {
        TELEPHONY_LOGE("[slot%{public}d] %{public}s Write slotId fail!", slotId, funcName.c_str());
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t ImsSmsProxy::SendRequest(MessageParcel &in, int32_t slotId, int32_t eventId)
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
