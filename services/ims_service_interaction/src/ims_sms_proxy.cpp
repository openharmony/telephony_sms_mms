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

namespace OHOS {
namespace Telephony {
int32_t ImsSmsProxy::ImsSendMessage()
{
    MessageOption option;
    MessageParcel in;
    MessageParcel out;
    if (!in.WriteInterfaceToken(ImsSmsProxy::GetDescriptor())) {
        TELEPHONY_LOGE("ImsSmsProxy::ImsSendMessage return, write descriptor token fail!");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    int32_t error = Remote()->SendRequest(IMS_SEND_MESSAGE, in, out, option);
    if (error == ERR_NONE) {
        TELEPHONY_LOGI("ImsSmsProxy::ImsSendMessage return, send request success!");
        return out.ReadInt32();
    }
    return error;
}

int32_t ImsSmsProxy::RegisterImsSmsCallback(const sptr<ImsSmsCallbackInterface> &callback)
{
    if (callback == nullptr) {
        TELEPHONY_LOGE("ImsSmsProxy::RegisterImsSmsCallback return, callback is nullptr");
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }
    MessageOption option;
    MessageParcel in;
    MessageParcel out;
    if (!in.WriteInterfaceToken(ImsSmsProxy::GetDescriptor())) {
        TELEPHONY_LOGE("ImsSmsProxy::RegisterImsSmsCallback return, write descriptor token fail!");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!in.WriteRemoteObject(callback->AsObject().GetRefPtr())) {
        TELEPHONY_LOGE("ImsSmsProxy::RegisterImsSmsCallback return, write data fail!");
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }

    int32_t error = Remote()->SendRequest(IMS_SMS_REGISTER_CALLBACK, in, out, option);
    if (error == ERR_NONE) {
        return out.ReadInt32();
    }
    return error;
}
} // namespace Telephony
} // namespace OHOS