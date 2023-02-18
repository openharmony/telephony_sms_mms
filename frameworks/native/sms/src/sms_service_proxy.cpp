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
#include "sms_mms_errors.h"
#include "string_utils.h"
#include "telephony_errors.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
const int32_t MAX_LEN = 10000;
SmsServiceProxy::SmsServiceProxy(const sptr<IRemoteObject> &impl) : IRemoteProxy<ISmsServiceInterface>(impl) {}

int32_t SmsServiceProxy::SendMessage(int32_t slotId, const std::u16string desAddr, const std::u16string scAddr,
    const std::u16string text, const sptr<ISendShortMessageCallback> &sendCallback,
    const sptr<IDeliveryShortMessageCallback> &deliverCallback)
{
    TELEPHONY_LOGI("SmsServiceProxy::SendMessage with text slotId : %{public}d", slotId);
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!dataParcel.WriteInterfaceToken(SmsServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("SendMessage with text WriteInterfaceToken is false");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    dataParcel.WriteInt32(slotId);
    dataParcel.WriteString16(desAddr);
    dataParcel.WriteString16(scAddr);
    dataParcel.WriteString16(text);
    if (sendCallback == nullptr) {
        TELEPHONY_LOGE("SendMessage with text sendCallback is nullptr");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    dataParcel.WriteRemoteObject(sendCallback->AsObject().GetRefPtr());
    if (deliverCallback == nullptr) {
        TELEPHONY_LOGE("SendMessage with text deliverCallback is nullptr");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    dataParcel.WriteRemoteObject(deliverCallback->AsObject().GetRefPtr());

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("SendMessage with text Remote is null");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    remote->SendRequest(TEXT_BASED_SMS_DELIVERY, dataParcel, replyParcel, option);
    return replyParcel.ReadInt32();
};

int32_t SmsServiceProxy::SendMessage(int32_t slotId, const std::u16string desAddr, const std::u16string scAddr,
    uint16_t port, const uint8_t *data, uint16_t dataLen, const sptr<ISendShortMessageCallback> &sendCallback,
    const sptr<IDeliveryShortMessageCallback> &deliverCallback)
{
    TELEPHONY_LOGI("SmsServiceProxy::SendMessage with data slotId : %{public}d", slotId);
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!dataParcel.WriteInterfaceToken(SmsServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("SendMessage with data WriteInterfaceToken is false");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    dataParcel.WriteInt32(slotId);
    dataParcel.WriteString16(desAddr);
    dataParcel.WriteString16(scAddr);
    dataParcel.WriteInt16(port);
    if (sendCallback == nullptr) {
        TELEPHONY_LOGE("SendMessage with data sendCallback is nullptr");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    dataParcel.WriteRemoteObject(sendCallback->AsObject().GetRefPtr());
    if (deliverCallback == nullptr) {
        TELEPHONY_LOGE("SendMessage with data deliverCallback is nullptr");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    dataParcel.WriteRemoteObject(deliverCallback->AsObject().GetRefPtr());
    dataParcel.WriteInt16(dataLen);
    dataParcel.WriteRawData(data, dataLen);

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("SendMessage with data Remote is null");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    remote->SendRequest(DATA_BASED_SMS_DELIVERY, dataParcel, replyParcel, option);
    return replyParcel.ReadInt32();
};

int32_t SmsServiceProxy::SetSmscAddr(int32_t slotId, const std::u16string &scAddr)
{
    TELEPHONY_LOGI("SmsServiceProxy::SetSmscAddr slotId : %{public}d", slotId);
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option(MessageOption::TF_SYNC);
    if (!dataParcel.WriteInterfaceToken(SmsServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("SetSmscAddr WriteInterfaceToken is false");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteInt32(slotId);
    dataParcel.WriteString16(scAddr);
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("SetSmscAddr Remote is null");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    remote->SendRequest(SET_SMSC_ADDRESS, dataParcel, replyParcel, option);
    return replyParcel.ReadInt32();
}

int32_t SmsServiceProxy::GetSmscAddr(int32_t slotId, std::u16string &smscAddress)
{
    TELEPHONY_LOGI("SmsServiceProxy::GetSmscAddr slotId : %{public}d", slotId);
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option(MessageOption::TF_SYNC);
    if (!dataParcel.WriteInterfaceToken(SmsServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("GetSmscAddr WriteInterfaceToken is false");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteInt32(slotId);
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("GetSmscAddr Remote is null");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    remote->SendRequest(GET_SMSC_ADDRESS, dataParcel, replyParcel, option);
    int32_t result = replyParcel.ReadInt32();
    if (result == TELEPHONY_ERR_SUCCESS) {
        smscAddress = replyParcel.ReadString16();
    }
    return result;
}

int32_t SmsServiceProxy::AddSimMessage(
    int32_t slotId, const std::u16string &smsc, const std::u16string &pdu, SimMessageStatus status)
{
    TELEPHONY_LOGI("SmsServiceProxy::AddSimMessage slotId : %{public}d", slotId);
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option(MessageOption::TF_SYNC);
    if (!dataParcel.WriteInterfaceToken(SmsServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("AddSimMessage WriteInterfaceToken is false");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteInt32(slotId);
    dataParcel.WriteString16(smsc);
    dataParcel.WriteString16(pdu);
    dataParcel.WriteUint32(status);
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("AddSimMessage Remote is null");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    remote->SendRequest(ADD_SIM_MESSAGE, dataParcel, replyParcel, option);
    return replyParcel.ReadInt32();
}

int32_t SmsServiceProxy::DelSimMessage(int32_t slotId, uint32_t msgIndex)
{
    TELEPHONY_LOGI("SmsServiceProxy::DelSimMessage slotId : %{public}d", slotId);
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option(MessageOption::TF_SYNC);
    if (!dataParcel.WriteInterfaceToken(SmsServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("DelSimMessage WriteInterfaceToken is false");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteInt32(slotId);
    dataParcel.WriteUint32(msgIndex);
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("DelSimMessage Remote is null");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    remote->SendRequest(DEL_SIM_MESSAGE, dataParcel, replyParcel, option);
    return replyParcel.ReadInt32();
}

int32_t SmsServiceProxy::UpdateSimMessage(int32_t slotId, uint32_t msgIndex, SimMessageStatus newStatus,
    const std::u16string &pdu, const std::u16string &smsc)
{
    TELEPHONY_LOGI("SmsServiceProxy::UpdateSimMessage slotId : %{public}d", slotId);
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option(MessageOption::TF_SYNC);
    if (!dataParcel.WriteInterfaceToken(SmsServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("UpdateSimMessage WriteInterfaceToken is false");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteInt32(slotId);
    dataParcel.WriteUint32(msgIndex);
    dataParcel.WriteUint32(newStatus);
    dataParcel.WriteString16(pdu);
    dataParcel.WriteString16(smsc);
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("UpdateSimMessage Remote is null");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    remote->SendRequest(UPDATE_SIM_MESSAGE, dataParcel, replyParcel, option);
    return replyParcel.ReadInt32();
}

int32_t SmsServiceProxy::GetAllSimMessages(int32_t slotId, std::vector<ShortMessage> &message)
{
    TELEPHONY_LOGI("SmsServiceProxy::GetAllSimMessages slotId : %{public}d", slotId);
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option(MessageOption::TF_SYNC);
    if (!dataParcel.WriteInterfaceToken(SmsServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("GetAllSimMessages WriteInterfaceToken is false");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteInt32(slotId);
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("GetAllSimMessages Remote is null");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    remote->SendRequest(GET_ALL_SIM_MESSAGE, dataParcel, replyParcel, option);
    int32_t result = replyParcel.ReadInt32();
    if (result != TELEPHONY_ERR_SUCCESS) {
        TELEPHONY_LOGE("GetAllSimMessages result is not TELEPHONY_ERR_SUCCESS");
        return result;
    }
    int32_t resultLen = replyParcel.ReadInt32();
    if (resultLen >= MAX_LEN) {
        TELEPHONY_LOGE("GetAllSimMessages resultLen over max");
        return SMS_MMS_MESSAGE_LENGTH_OUT_OF_RANGE;
    }
    for (int32_t i = 0; i < resultLen; i++) {
        message.emplace_back(ShortMessage::UnMarshalling(replyParcel));
    }
    return result;
}

int32_t SmsServiceProxy::SetCBConfig(
    int32_t slotId, bool enable, uint32_t fromMsgId, uint32_t toMsgId, uint8_t netType)
{
    TELEPHONY_LOGI("SmsServiceProxy::SetCBConfig slotId : %{public}d", slotId);
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option(MessageOption::TF_SYNC);
    if (!dataParcel.WriteInterfaceToken(SmsServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("SetCBConfig WriteInterfaceToken is false");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteInt32(slotId);
    dataParcel.WriteBool(enable);
    dataParcel.WriteUint32(fromMsgId);
    dataParcel.WriteUint32(toMsgId);
    dataParcel.WriteUint8(netType);
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("SetCBConfig Remote is null");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    remote->SendRequest(SET_CB_CONFIG, dataParcel, replyParcel, option);
    return replyParcel.ReadInt32();
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

int32_t SmsServiceProxy::SetDefaultSmsSlotId(int32_t slotId)
{
    TELEPHONY_LOGI("SmsServiceProxy::SetDefaultSmsSlotId slotId : %{public}d", slotId);
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option(MessageOption::TF_SYNC);
    if (!dataParcel.WriteInterfaceToken(SmsServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("SetDefaultSmsSlotId WriteInterfaceToken is false");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteInt32(slotId);
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("SetDefaultSmsSlotId Remote is null");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    remote->SendRequest(SET_DEFAULT_SMS_SLOT_ID, dataParcel, replyParcel, option);
    return replyParcel.ReadInt32();
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

int32_t SmsServiceProxy::SplitMessage(const std::u16string &message, std::vector<std::u16string> &splitMessage)
{
    TELEPHONY_LOGI("SmsServiceProxy::SplitMessage");
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option(MessageOption::TF_SYNC);
    if (!dataParcel.WriteInterfaceToken(SmsServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("SplitMessage WriteInterfaceToken is false");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteString16(message);
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("SplitMessage Remote is null");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    remote->SendRequest(SPLIT_MESSAGE, dataParcel, replyParcel, option);
    int32_t result = replyParcel.ReadInt32();
    if (result != TELEPHONY_ERR_SUCCESS) {
        TELEPHONY_LOGE("SplitMessage result is not TELEPHONY_ERR_SUCCESS");
        return result;
    }
    int32_t resultLen = replyParcel.ReadInt32();
    if (resultLen >= MAX_LEN) {
        TELEPHONY_LOGE("SplitMessage resultLen over max");
        return SMS_MMS_MESSAGE_LENGTH_OUT_OF_RANGE;
    }
    for (int32_t i = 0; i < resultLen; ++i) {
        splitMessage.emplace_back(replyParcel.ReadString16());
    }
    return result;
}

int32_t SmsServiceProxy::GetSmsSegmentsInfo(
    int32_t slotId, const std::u16string &message, bool force7BitCode, ISmsServiceInterface::SmsSegmentsInfo &segInfo)
{
    TELEPHONY_LOGI("SmsServiceProxy::GetSmsSegmentsInfo slotId : %{public}d", slotId);
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option(MessageOption::TF_SYNC);
    if (!dataParcel.WriteInterfaceToken(SmsServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("GetSmsSegmentsInfo WriteInterfaceToken is false");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteInt32(slotId);
    dataParcel.WriteString16(message);
    dataParcel.WriteBool(force7BitCode);
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("GetSmsSegmentsInfo Remote is null");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    remote->SendRequest(GET_SMS_SEGMENTS_INFO, dataParcel, replyParcel, option);
    int32_t result = replyParcel.ReadInt32();
    if (result != TELEPHONY_ERR_SUCCESS) {
        TELEPHONY_LOGE("GetSmsSegmentsInfo ReadInt32 is null");
        return result;
    }

    segInfo.msgSegCount = replyParcel.ReadInt32();
    segInfo.msgEncodingCount = replyParcel.ReadInt32();
    segInfo.msgRemainCount = replyParcel.ReadInt32();
    int32_t cds = replyParcel.ReadInt32();
    segInfo.msgCodeScheme = static_cast<ISmsServiceInterface::SmsSegmentsInfo::SmsSegmentCodeScheme>(cds);
    return TELEPHONY_ERR_SUCCESS;
}

int32_t SmsServiceProxy::IsImsSmsSupported(int32_t slotId, bool &isSupported)
{
    TELEPHONY_LOGI("SmsServiceProxy::IsImsSmsSupported slotId : %{public}d", slotId);
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option(MessageOption::TF_SYNC);
    if (!dataParcel.WriteInterfaceToken(SmsServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("IsImsSmsSupported WriteInterfaceToken is false");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteInt32(slotId);
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("IsImsSmsSupported Remote is null");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    remote->SendRequest(IS_IMS_SMS_SUPPORTED, dataParcel, replyParcel, option);
    int32_t result = replyParcel.ReadInt32();
    if (result != TELEPHONY_ERR_SUCCESS) {
        TELEPHONY_LOGE("IsImsSmsSupported result:%{public}d", result);
        return result;
    }
    isSupported = replyParcel.ReadBool();
    return result;
}

int32_t SmsServiceProxy::GetImsShortMessageFormat(std::u16string &format)
{
    TELEPHONY_LOGI("SmsServiceProxy::GetImsShortMessageFormat");
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option(MessageOption::TF_SYNC);
    if (!dataParcel.WriteInterfaceToken(SmsServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("GetImsShortMessageFormat WriteInterfaceToken is false");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("GetImsShortMessageFormat Remote is null");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    remote->SendRequest(GET_IMS_SHORT_MESSAGE_FORMAT, dataParcel, replyParcel, option);
    int32_t result = replyParcel.ReadInt32();
    if (result == TELEPHONY_ERR_SUCCESS) {
        format = replyParcel.ReadString16();
    }
    return result;
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

int32_t SmsServiceProxy::CreateMessage(std::string pdu, std::string specification, ShortMessage &message)
{
    if (pdu.empty() || specification.empty()) {
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }

    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option(MessageOption::TF_SYNC);
    if (!dataParcel.WriteInterfaceToken(SmsServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("CreateMessage WriteInterfaceToken is false");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    dataParcel.WriteString(pdu);
    dataParcel.WriteString(specification);

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    remote->SendRequest(CREATE_MESSAGE, dataParcel, replyParcel, option);

    int32_t result = replyParcel.ReadInt32();
    TELEPHONY_LOGI("SmsServiceProxy::CreateMessage result:%{public}d", result);
    if (result != TELEPHONY_ERR_SUCCESS) {
        TELEPHONY_LOGE("CreateMessage result fail");
        return result;
    }

    if (!message.ReadFromParcel(replyParcel)) {
        TELEPHONY_LOGE("SmsServiceProxy::CreateMessage fail");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return result;
}

bool SmsServiceProxy::GetBase64Encode(std::string src, std::string &dest)
{
    bool result = false;
    if (src.empty()) {
        return result;
    }

    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option(MessageOption::TF_SYNC);
    if (!dataParcel.WriteInterfaceToken(SmsServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("GetBase64Encode WriteInterfaceToken is false");
        return result;
    }

    dataParcel.WriteString16(StringUtils::ToUtf16(src));

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        return result;
    }
    remote->SendRequest(MMS_BASE64_ENCODE, dataParcel, replyParcel, option);

    result = replyParcel.ReadBool();
    TELEPHONY_LOGI("SmsServiceProxy::GetBase64Encode result:%{public}d", result);
    if (!result) {
        return result;
    }
    dest = StringUtils::ToUtf8(dataParcel.ReadString16());
    return result;
}

bool SmsServiceProxy::GetBase64Decode(std::string src, std::string &dest)
{
    bool result = false;
    if (src.empty()) {
        return result;
    }

    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option(MessageOption::TF_SYNC);
    if (!dataParcel.WriteInterfaceToken(SmsServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("GetBase64Decode WriteInterfaceToken is false");
        return result;
    }

    dataParcel.WriteString16(StringUtils::ToUtf16(src));

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        return result;
    }
    remote->SendRequest(MMS_BASE64_DECODE, dataParcel, replyParcel, option);
    result = replyParcel.ReadBool();
    TELEPHONY_LOGI("SmsServiceProxy::GetBase64Decode result:%{public}d", result);
    if (!result) {
        return result;
    }
    dest = StringUtils::ToUtf8(dataParcel.ReadString16());
    return result;
}

bool SmsServiceProxy::GetEncodeStringFunc(
    std::string &encodeString, uint32_t charset, uint32_t valLength, std::string strEncodeString)
{
    bool result = false;
    if (strEncodeString.empty()) {
        return result;
    }

    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option(MessageOption::TF_SYNC);
    if (!dataParcel.WriteInterfaceToken(SmsServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("GetEncodeStringFunc WriteInterfaceToken is false");
        return result;
    }

    dataParcel.WriteUint32(charset);
    dataParcel.WriteUint32(valLength);
    dataParcel.WriteString16(StringUtils::ToUtf16(strEncodeString));

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        return result;
    }
    remote->SendRequest(GET_ENCODE_STRING, dataParcel, replyParcel, option);
    result = replyParcel.ReadBool();
    TELEPHONY_LOGI("SmsServiceProxy::GetEncodeStringFunc result:%{public}d", result);
    if (!result) {
        return result;
    }
    encodeString = StringUtils::ToUtf8(dataParcel.ReadString16());
    return result;
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
