/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#include "sms_service_proxy.h"

#include "parcel.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
const int32_t MAX_LEN = 10000;
SmsServiceProxy::SmsServiceProxy(const sptr<IRemoteObject> &impl) : IRemoteProxy<ISmsServiceInterface>(impl) {}

void SmsServiceProxy::SendMessage(int32_t slotId, const std::u16string desAddr, const std::u16string scAddr,
    const std::u16string text, const sptr<ISendShortMessageCallback> &sendCallback,
    const sptr<IDeliveryShortMessageCallback> &deliverCallback)
{
    TELEPHONY_LOGI("SmsServiceProxy::SendMessage with text slotId : %{public}d", slotId);
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!dataParcel.WriteInterfaceToken(SmsServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("SendMessage with text WriteInterfaceToken is false");
        return;
    }

    dataParcel.WriteInt32(slotId);
    dataParcel.WriteString16(desAddr);
    dataParcel.WriteString16(scAddr);
    dataParcel.WriteString16(text);
    if (sendCallback != nullptr) {
        dataParcel.WriteRemoteObject(sendCallback->AsObject().GetRefPtr());
    }
    if (deliverCallback != nullptr) {
        dataParcel.WriteRemoteObject(deliverCallback->AsObject().GetRefPtr());
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("SendMessage with text Remote is null");
        return;
    }
    remote->SendRequest(TEXT_BASED_SMS_DELIVERY, dataParcel, replyParcel, option);
};

void SmsServiceProxy::SendMessage(int32_t slotId, const std::u16string desAddr, const std::u16string scAddr,
    uint16_t port, const uint8_t *data, uint16_t dataLen, const sptr<ISendShortMessageCallback> &sendCallback,
    const sptr<IDeliveryShortMessageCallback> &deliverCallback)
{
    TELEPHONY_LOGI("SmsServiceProxy::SendMessage with data slotId : %{public}d", slotId);
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!dataParcel.WriteInterfaceToken(SmsServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("SendMessage with data WriteInterfaceToken is false");
        return;
    }

    dataParcel.WriteInt32(slotId);
    dataParcel.WriteString16(desAddr);
    dataParcel.WriteString16(scAddr);
    dataParcel.WriteInt16(port);
    if (sendCallback != nullptr) {
        dataParcel.WriteRemoteObject(sendCallback->AsObject().GetRefPtr());
    }
    if (deliverCallback != nullptr) {
        dataParcel.WriteRemoteObject(deliverCallback->AsObject().GetRefPtr());
    }
    dataParcel.WriteInt16(dataLen);
    dataParcel.WriteRawData(data, dataLen);

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("SendMessage with data Remote is null");
        return;
    }
    remote->SendRequest(DATA_BASED_SMS_DELIVERY, dataParcel, replyParcel, option);
};

bool SmsServiceProxy::SetSmscAddr(int32_t slotId, const std::u16string &scAddr)
{
    TELEPHONY_LOGI("SmsServiceProxy::SetSmscAddr slotId : %{public}d", slotId);
    bool result = false;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option(MessageOption::TF_SYNC);
    if (!dataParcel.WriteInterfaceToken(SmsServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("SetSmscAddr WriteInterfaceToken is false");
        return result;
    }
    dataParcel.WriteInt32(slotId);
    dataParcel.WriteString16(scAddr);
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("SetSmscAddr Remote is null");
        return result;
    }
    remote->SendRequest(SET_SMSC_ADDRESS, dataParcel, replyParcel, option);
    return replyParcel.ReadBool();
}

std::u16string SmsServiceProxy::GetSmscAddr(int32_t slotId)
{
    TELEPHONY_LOGI("SmsServiceProxy::GetSmscAddr slotId : %{public}d", slotId);
    std::u16string result;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option(MessageOption::TF_SYNC);
    if (!dataParcel.WriteInterfaceToken(SmsServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("GetSmscAddr WriteInterfaceToken is false");
        return result;
    }
    dataParcel.WriteInt32(slotId);
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("GetSmscAddr Remote is null");
        return result;
    }
    remote->SendRequest(GET_SMSC_ADDRESS, dataParcel, replyParcel, option);
    return replyParcel.ReadString16();
}

bool SmsServiceProxy::AddSimMessage(
    int32_t slotId, const std::u16string &smsc, const std::u16string &pdu, SimMessageStatus status)
{
    TELEPHONY_LOGI("SmsServiceProxy::AddSimMessage slotId : %{public}d", slotId);
    bool result = false;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option(MessageOption::TF_SYNC);
    if (!dataParcel.WriteInterfaceToken(SmsServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("AddSimMessage WriteInterfaceToken is false");
        return result;
    }
    dataParcel.WriteInt32(slotId);
    dataParcel.WriteString16(smsc);
    dataParcel.WriteString16(pdu);
    dataParcel.WriteUint32(status);
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("AddSimMessage Remote is null");
        return result;
    }
    remote->SendRequest(ADD_SIM_MESSAGE, dataParcel, replyParcel, option);
    return replyParcel.ReadBool();
}

bool SmsServiceProxy::DelSimMessage(int32_t slotId, uint32_t msgIndex)
{
    TELEPHONY_LOGI("SmsServiceProxy::DelSimMessage slotId : %{public}d", slotId);
    bool result = false;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option(MessageOption::TF_SYNC);
    if (!dataParcel.WriteInterfaceToken(SmsServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("DelSimMessage WriteInterfaceToken is false");
        return result;
    }
    dataParcel.WriteInt32(slotId);
    dataParcel.WriteUint32(msgIndex);
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("DelSimMessage Remote is null");
        return result;
    }
    remote->SendRequest(DEL_SIM_MESSAGE, dataParcel, replyParcel, option);
    return replyParcel.ReadBool();
}

bool SmsServiceProxy::UpdateSimMessage(int32_t slotId, uint32_t msgIndex, SimMessageStatus newStatus,
    const std::u16string &pdu, const std::u16string &smsc)
{
    TELEPHONY_LOGI("SmsServiceProxy::UpdateSimMessage slotId : %{public}d", slotId);
    bool result = false;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option(MessageOption::TF_SYNC);
    if (!dataParcel.WriteInterfaceToken(SmsServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("UpdateSimMessage WriteInterfaceToken is false");
        return result;
    }
    dataParcel.WriteInt32(slotId);
    dataParcel.WriteUint32(msgIndex);
    dataParcel.WriteUint32(newStatus);
    dataParcel.WriteString16(pdu);
    dataParcel.WriteString16(smsc);
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("UpdateSimMessage Remote is null");
        return result;
    }
    remote->SendRequest(UPDATE_SIM_MESSAGE, dataParcel, replyParcel, option);
    return replyParcel.ReadBool();
}

std::vector<ShortMessage> SmsServiceProxy::GetAllSimMessages(int32_t slotId)
{
    TELEPHONY_LOGI("SmsServiceProxy::GetAllSimMessages slotId : %{public}d", slotId);
    std::vector<ShortMessage> result;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option(MessageOption::TF_SYNC);
    if (!dataParcel.WriteInterfaceToken(SmsServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("GetAllSimMessages WriteInterfaceToken is false");
        return result;
    }
    dataParcel.WriteInt32(slotId);
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("GetAllSimMessages Remote is null");
        return result;
    }
    remote->SendRequest(GET_ALL_SIM_MESSAGE, dataParcel, replyParcel, option);

    int32_t resultLen = replyParcel.ReadInt32();
    if (resultLen >= MAX_LEN) {
        TELEPHONY_LOGE("GetAllSimMessages resultLen over max");
        return result;
    }
    for (int32_t i = 0; i < resultLen; i++) {
        result.emplace_back(ShortMessage::UnMarshalling(replyParcel));
    }
    return result;
}

bool SmsServiceProxy::SetCBConfig(
    int32_t slotId, bool enable, uint32_t fromMsgId, uint32_t toMsgId, uint8_t netType)
{
    TELEPHONY_LOGI("SmsServiceProxy::SetCBConfig slotId : %{public}d", slotId);
    bool result = false;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option(MessageOption::TF_SYNC);
    if (!dataParcel.WriteInterfaceToken(SmsServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("SetCBConfig WriteInterfaceToken is false");
        return result;
    }
    dataParcel.WriteInt32(slotId);
    dataParcel.WriteBool(enable);
    dataParcel.WriteUint32(fromMsgId);
    dataParcel.WriteUint32(toMsgId);
    dataParcel.WriteUint8(netType);
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("SetCBConfig Remote is null");
        return result;
    }
    remote->SendRequest(SET_CB_CONFIG, dataParcel, replyParcel, option);
    return replyParcel.ReadBool();
}

bool SmsServiceProxy::SetImsSmsConfig(
    int32_t slotId, int32_t enable)
{
    TELEPHONY_LOGI("SmsServiceProxy::SetImsSmsConfig slotId : %{public}d", slotId);
    bool result = false;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option(MessageOption::TF_SYNC);
    if (!dataParcel.WriteInterfaceToken(SmsServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("SetImsSmsConfig WriteInterfaceToken is false");
        return result;
    }
    dataParcel.WriteInt32(slotId);
    dataParcel.WriteInt32(enable);
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("SetImsSmsConfig Remote is null");
        return result;
    }
    remote->SendRequest(SET_IMS_SMS_CONFIG, dataParcel, replyParcel, option);
    return replyParcel.ReadBool();
}

bool SmsServiceProxy::SetDefaultSmsSlotId(int32_t slotId)
{
    TELEPHONY_LOGI("SmsServiceProxy::SetDefaultSmsSlotId slotId : %{public}d", slotId);
    bool result = false;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option(MessageOption::TF_SYNC);
    if (!dataParcel.WriteInterfaceToken(SmsServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("SetDefaultSmsSlotId WriteInterfaceToken is false");
        return result;
    }
    dataParcel.WriteInt32(slotId);
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("SetDefaultSmsSlotId Remote is null");
        return result;
    }
    remote->SendRequest(SET_DEFAULT_SMS_SLOT_ID, dataParcel, replyParcel, option);
    return replyParcel.ReadBool();
}

int32_t SmsServiceProxy::GetDefaultSmsSlotId()
{
    TELEPHONY_LOGI("SmsServiceProxy::GetDefaultSmsSlotId");
    int32_t result = -1;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option(MessageOption::TF_SYNC);
    if (!dataParcel.WriteInterfaceToken(SmsServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("GetDefaultSmsSlotId WriteInterfaceToken is false");
        return result;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("GetDefaultSmsSlotId Remote is null");
        return result;
    }
    remote->SendRequest(GET_DEFAULT_SMS_SLOT_ID, dataParcel, replyParcel, option);
    return replyParcel.ReadInt32();
}

std::vector<std::u16string> SmsServiceProxy::SplitMessage(const std::u16string &message)
{
    TELEPHONY_LOGI("SmsServiceProxy::SplitMessage");
    std::vector<std::u16string> result;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option(MessageOption::TF_SYNC);
    if (!dataParcel.WriteInterfaceToken(SmsServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("SplitMessage WriteInterfaceToken is false");
        return result;
    }
    dataParcel.WriteString16(message);
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("SplitMessage Remote is null");
        return result;
    }
    remote->SendRequest(SPLIT_MESSAGE, dataParcel, replyParcel, option);
    int32_t resultLen = replyParcel.ReadInt32();
    if (resultLen >= MAX_LEN) {
        TELEPHONY_LOGE("SplitMessage resultLen over max");
        return result;
    }
    for (int32_t i = 0; i < resultLen; ++i) {
        result.emplace_back(replyParcel.ReadString16());
    }
    return result;
}

bool SmsServiceProxy::GetSmsSegmentsInfo(int32_t slotId, const std::u16string &message, bool force7BitCode,
    ISmsServiceInterface::SmsSegmentsInfo &segInfo)
{
    TELEPHONY_LOGI("SmsServiceProxy::GetSmsSegmentsInfo slotId : %{public}d", slotId);
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option(MessageOption::TF_SYNC);
    if (!dataParcel.WriteInterfaceToken(SmsServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("GetSmsSegmentsInfo WriteInterfaceToken is false");
        return false;
    }
    dataParcel.WriteInt32(slotId);
    dataParcel.WriteString16(message);
    dataParcel.WriteBool(force7BitCode);
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("GetSmsSegmentsInfo Remote is null");
        return false;
    }
    remote->SendRequest(GET_SMS_SEGMENTS_INFO, dataParcel, replyParcel, option);
    if (!replyParcel.ReadBool()) {
        TELEPHONY_LOGE("GetSmsSegmentsInfo ReadBool is null");
        return false;
    }

    segInfo.msgSegCount = replyParcel.ReadInt32();
    segInfo.msgEncodingCount = replyParcel.ReadInt32();
    segInfo.msgRemainCount = replyParcel.ReadInt32();
    int32_t cds = replyParcel.ReadInt32();
    segInfo.msgCodeScheme = static_cast<ISmsServiceInterface::SmsSegmentsInfo::SmsSegmentCodeScheme>(cds);
    return true;
}

bool SmsServiceProxy::IsImsSmsSupported(int32_t slotId)
{
    TELEPHONY_LOGI("SmsServiceProxy::IsImsSmsSupported slotId : %{public}d", slotId);
    bool result = false;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option(MessageOption::TF_SYNC);
    if (!dataParcel.WriteInterfaceToken(SmsServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("IsImsSmsSupported WriteInterfaceToken is false");
        return result;
    }
    dataParcel.WriteInt32(slotId);
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("IsImsSmsSupported Remote is null");
        return result;
    }
    remote->SendRequest(IS_IMS_SMS_SUPPORTED, dataParcel, replyParcel, option);
    return replyParcel.ReadBool();
}

std::u16string SmsServiceProxy::GetImsShortMessageFormat()
{
    TELEPHONY_LOGI("SmsServiceProxy::GetImsShortMessageFormat");
    std::u16string result;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option(MessageOption::TF_SYNC);
    if (!dataParcel.WriteInterfaceToken(SmsServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("GetImsShortMessageFormat WriteInterfaceToken is false");
        return result;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("GetImsShortMessageFormat Remote is null");
        return result;
    }
    remote->SendRequest(GET_IMS_SHORT_MESSAGE_FORMAT, dataParcel, replyParcel, option);
    return replyParcel.ReadString16();
}

bool SmsServiceProxy::HasSmsCapability()
{
    TELEPHONY_LOGI("SmsServiceProxy::HasSmsCapability");
    bool result = false;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option(MessageOption::TF_SYNC);
    if (!dataParcel.WriteInterfaceToken(SmsServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("HasSmsCapability WriteInterfaceToken is false");
        return result;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("HasSmsCapability Remote is null");
        return result;
    }
    remote->SendRequest(HAS_SMS_CAPABILITY, dataParcel, replyParcel, option);
    return replyParcel.ReadBool();
}

bool SmsServiceDeathRecipient::gotDeathRecipient_ = false;

bool SmsServiceDeathRecipient::GotDeathRecipient()
{
    return gotDeathRecipient_;
}

void SmsServiceDeathRecipient::OnRemoteDied(const wptr<IRemoteObject> &remote)
{
    gotDeathRecipient_ = true;
}

SmsServiceDeathRecipient::SmsServiceDeathRecipient() {}

SmsServiceDeathRecipient::~SmsServiceDeathRecipient() {}
} // namespace Telephony
} // namespace OHOS
