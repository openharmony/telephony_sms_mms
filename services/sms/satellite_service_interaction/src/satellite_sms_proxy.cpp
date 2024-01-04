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

#include "satellite_sms_proxy.h"

#include "satellite/satellite_sms_service_ipc_interface_code.h"
#include "telephony_errors.h"
#include "telephony_log_wrapper.h"
#include "telephony_types.h"

namespace OHOS {
namespace Telephony {
bool SatelliteSmsProxy::WriteInterfaceToken(MessageParcel &data)
{
    if (!data.WriteInterfaceToken(SatelliteSmsProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write interface token failed");
        return false;
    }
    return true;
}

int32_t SatelliteSmsProxy::RegisterSmsNotify(int32_t slotId, int32_t what, const sptr<ISatelliteSmsCallback> &callback)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    int32_t ret = TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;

    if (callback == nullptr) {
        TELEPHONY_LOGE("RegisterSmsNotify callback is null");
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!WriteInterfaceToken(data)) {
        TELEPHONY_LOGE("RegisterSmsNotify WriteInterfaceToken is false");
        return ret;
    }
    if (!data.WriteInt32(slotId)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!data.WriteInt32(what)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!data.WriteRemoteObject(callback->AsObject().GetRefPtr())) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }

    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("RegisterSmsNotify Remote is null");
        return ret;
    }

    TELEPHONY_LOGD("Satellite RegisterSmsNotify slotId: %{public}d, what: %{public}d", slotId, what);
    ret = remote->SendRequest(uint32_t(SatelliteSmsServiceInterfaceCode::REGISTER_SMS_NOTIFY), data, reply, option);
    if (ret != ERR_NONE) {
        TELEPHONY_LOGE("RegisterSmsNotify failed, error code is %{public}d ", ret);
        return ret;
    }

    if (!reply.ReadInt32(ret)) {
        TELEPHONY_LOGE("RegisterSmsNotify read reply failed");
    }
    return ret;
}

int32_t SatelliteSmsProxy::UnRegisterSmsNotify(int32_t slotId, int32_t what)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    int32_t ret = TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    if (!WriteInterfaceToken(data)) {
        TELEPHONY_LOGE("UnRegisterSmsNotify WriteInterfaceToken is false");
        return ret;
    }
    if (!data.WriteInt32(slotId)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!data.WriteInt32(what)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }

    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("UnRegisterSmsNotify Remote is null");
        return ret;
    }

    TELEPHONY_LOGD("Satellite UnRegisterSmsNotify slotId: %{public}d, what: %{public}d", slotId, what);
    ret = remote->SendRequest(uint32_t(SatelliteSmsServiceInterfaceCode::UNREGISTER_SMS_NOTIFY), data, reply, option);
    if (ret != ERR_NONE) {
        TELEPHONY_LOGE("UnRegisterSmsNotify failed, error code is %{public}d ", ret);
        return ret;
    }

    if (!reply.ReadInt32(ret)) {
        TELEPHONY_LOGE("UnRegisterSmsNotify read reply failed");
    }
    return ret;
}

int32_t SatelliteSmsProxy::SendSms(int32_t slotId, int32_t eventId, SatelliteMessage &message)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    int32_t ret = TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    if (!WriteInterfaceToken(data)) {
        TELEPHONY_LOGE("SendSms WriteInterfaceToken is false");
        return ret;
    }
    if (!data.WriteInt32(slotId)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!data.WriteInt32(eventId)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }

    if (!data.WriteInt64(message.refId)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!data.WriteString(message.smscPdu)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!data.WriteString(message.pdu)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }

    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("SendSms Remote is null");
        return ret;
    }

    TELEPHONY_LOGD("Satellite SendSms slotId: %{public}d, eventId: %{public}d", slotId, eventId);
    ret = remote->SendRequest(uint32_t(SatelliteSmsServiceInterfaceCode::SEND_SMS), data, reply, option);
    if (ret != ERR_NONE) {
        TELEPHONY_LOGE("SendSms failed, error code is %{public}d ", ret);
        return ret;
    }

    if (!reply.ReadInt32(ret)) {
        TELEPHONY_LOGE("SendSms read reply failed");
    }
    return ret;
}
int32_t SatelliteSmsProxy::SendSmsMoreMode(int32_t slotId, int32_t eventId, SatelliteMessage &message)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    int32_t ret = TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    if (!WriteInterfaceToken(data)) {
        TELEPHONY_LOGE("SendSmsMoreMode WriteInterfaceToken is false");
        return ret;
    }
    if (!data.WriteInt32(slotId)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!data.WriteInt32(eventId)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }

    if (!data.WriteInt64(message.refId)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!data.WriteString(message.smscPdu)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!data.WriteString(message.pdu)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }

    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("SendSmsMoreMode Remote is null");
        return ret;
    }
    TELEPHONY_LOGD("Satellite SendSmsMoreMode slotId: %{public}d, eventId: %{public}d", slotId, eventId);
    ret = remote->SendRequest(uint32_t(SatelliteSmsServiceInterfaceCode::SEND_SMS_MORE_MODE), data, reply, option);
    if (ret != ERR_NONE) {
        TELEPHONY_LOGE("SendSmsMoreMode failed, error code is %{public}d ", ret);
        return ret;
    }

    if (!reply.ReadInt32(ret)) {
        TELEPHONY_LOGE("SendSmsMoreMode read reply failed");
    }
    return ret;
}
int32_t SatelliteSmsProxy::SendSmsAck(int32_t slotId, int32_t eventId, bool success, int32_t cause)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    int32_t ret = TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    if (!WriteInterfaceToken(data)) {
        TELEPHONY_LOGE("SendSmsAck WriteInterfaceToken is false");
        return ret;
    }
    if (!data.WriteInt32(slotId)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!data.WriteInt32(eventId)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }

    if (!data.WriteBool(success)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!data.WriteInt32(cause)) {
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }

    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("SendSmsAck Remote is null");
        return ret;
    }
    TELEPHONY_LOGD("Satellite SendSmsAck slotId: %{public}d, eventId: %{public}d", slotId, eventId);
    ret = remote->SendRequest(uint32_t(SatelliteSmsServiceInterfaceCode::SEND_SMS_ACK), data, reply, option);
    if (ret != ERR_NONE) {
        TELEPHONY_LOGE("SendSmsAck failed, error code is %{public}d ", ret);
        return ret;
    }

    if (!reply.ReadInt32(ret)) {
        TELEPHONY_LOGE("SendSmsAck read reply failed");
    }
    return ret;
}
} // namespace Telephony
} // namespace OHOS
