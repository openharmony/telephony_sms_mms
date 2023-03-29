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

#include "sms_interface_stub.h"

#include "sms_interface_manager.h"
#include "string_utils.h"
#include "telephony_errors.h"
#include "telephony_log_wrapper.h"
#include "telephony_types.h"

namespace OHOS {
namespace Telephony {
using namespace std;
SmsInterfaceStub::SmsInterfaceStub()
{
    memberFuncMap_[TEXT_BASED_SMS_DELIVERY] = &SmsInterfaceStub::OnSendSmsTextRequest;
    memberFuncMap_[DATA_BASED_SMS_DELIVERY] = &SmsInterfaceStub::OnSendSmsDataRequest;
    memberFuncMap_[SET_SMSC_ADDRESS] = &SmsInterfaceStub::OnSetSmscAddr;
    memberFuncMap_[GET_SMSC_ADDRESS] = &SmsInterfaceStub::OnGetSmscAddr;
    memberFuncMap_[ADD_SIM_MESSAGE] = &SmsInterfaceStub::OnAddSimMessage;
    memberFuncMap_[DEL_SIM_MESSAGE] = &SmsInterfaceStub::OnDelSimMessage;
    memberFuncMap_[UPDATE_SIM_MESSAGE] = &SmsInterfaceStub::OnUpdateSimMessage;
    memberFuncMap_[GET_ALL_SIM_MESSAGE] = &SmsInterfaceStub::OnGetAllSimMessages;
    memberFuncMap_[SET_CB_CONFIG] = &SmsInterfaceStub::OnSetCBConfig;
    memberFuncMap_[SET_IMS_SMS_CONFIG] = &SmsInterfaceStub::OnSetImsSmsConfig;
    memberFuncMap_[SET_DEFAULT_SMS_SLOT_ID] = &SmsInterfaceStub::OnSetDefaultSmsSlotId;
    memberFuncMap_[GET_DEFAULT_SMS_SLOT_ID] = &SmsInterfaceStub::OnGetDefaultSmsSlotId;
    memberFuncMap_[SPLIT_MESSAGE] = &SmsInterfaceStub::OnSplitMessage;
    memberFuncMap_[GET_SMS_SEGMENTS_INFO] = &SmsInterfaceStub::OnGetSmsSegmentsInfo;
    memberFuncMap_[GET_IMS_SHORT_MESSAGE_FORMAT] = &SmsInterfaceStub::OnGetImsShortMessageFormat;
    memberFuncMap_[IS_IMS_SMS_SUPPORTED] = &SmsInterfaceStub::OnIsImsSmsSupported;
    memberFuncMap_[HAS_SMS_CAPABILITY] = &SmsInterfaceStub::OnHasSmsCapability;
    memberFuncMap_[CREATE_MESSAGE] = &SmsInterfaceStub::OnCreateMessage;
    memberFuncMap_[MMS_BASE64_ENCODE] = &SmsInterfaceStub::OnGetBase64Encode;
    memberFuncMap_[MMS_BASE64_DECODE] = &SmsInterfaceStub::OnGetBase64Decode;
    memberFuncMap_[GET_ENCODE_STRING] = &SmsInterfaceStub::OnGetEncodeStringFunc;
}

SmsInterfaceStub::~SmsInterfaceStub()
{
    slotSmsInterfaceManagerMap_.clear();
    memberFuncMap_.clear();
}

void SmsInterfaceStub::InitModule()
{
    static bool bInitModule = false;
    if (!bInitModule) {
        bInitModule = true;
        std::lock_guard<std::mutex> lock(mutex_);
        for (int32_t slotId = 0; slotId < SIM_SLOT_COUNT; ++slotId) {
            slotSmsInterfaceManagerMap_[slotId] = std::make_shared<SmsInterfaceManager>(slotId);
            if (slotSmsInterfaceManagerMap_[slotId] == nullptr) {
                TELEPHONY_LOGE(
                    "SmsInterfaceStub InitModule slotSmsInterfaceManagerMap_[%{public}d] is nullptr", slotId);
                return;
            }
            slotSmsInterfaceManagerMap_[slotId]->InitInterfaceManager();
            TELEPHONY_LOGI("SmsInterfaceStub InitModule slotId = %{public}d", slotId);
        }
    }
}

std::shared_ptr<SmsInterfaceManager> SmsInterfaceStub::GetSmsInterfaceManager(int32_t slotId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    std::map<uint32_t, std::shared_ptr<SmsInterfaceManager>>::iterator iter =
        slotSmsInterfaceManagerMap_.find(slotId);
    if (iter != slotSmsInterfaceManagerMap_.end()) {
        return iter->second;
    }
    return nullptr;
}

std::shared_ptr<SmsInterfaceManager> SmsInterfaceStub::GetSmsInterfaceManager()
{
    std::lock_guard<std::mutex> lock(mutex_);
    for (const auto &iter : slotSmsInterfaceManagerMap_) {
        if (iter.second != nullptr) {
            return iter.second;
        }
    }
    return nullptr;
}

void SmsInterfaceStub::OnSendSmsTextRequest(MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    sptr<ISendShortMessageCallback> sendCallback = nullptr;
    sptr<IDeliveryShortMessageCallback> deliveryCallback = nullptr;
    int32_t slotId = data.ReadInt32();
    u16string desAddr = data.ReadString16();
    u16string scAddr = data.ReadString16();
    u16string text = data.ReadString16();
    sptr<IRemoteObject> remoteSendCallback = data.ReadRemoteObject();
    sptr<IRemoteObject> remoteDeliveryCallback = data.ReadRemoteObject();
    if (remoteSendCallback != nullptr) {
        sendCallback = iface_cast<ISendShortMessageCallback>(remoteSendCallback);
    }
    if (remoteDeliveryCallback != nullptr) {
        deliveryCallback = iface_cast<IDeliveryShortMessageCallback>(remoteDeliveryCallback);
    }
    TELEPHONY_LOGI("MessageID::TEXT_BASED_SMS_DELIVERY %{public}d", slotId);
    RemoveSpacesInDesAddr(desAddr);
    int32_t result = SendMessage(slotId, desAddr, scAddr, text, sendCallback, deliveryCallback);
    reply.WriteInt32(result);
}

void SmsInterfaceStub::OnSendSmsDataRequest(MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    sptr<ISendShortMessageCallback> sendCallback = nullptr;
    sptr<IDeliveryShortMessageCallback> deliveryCallback = nullptr;
    int32_t slotId = data.ReadInt32();
    u16string desAddr = data.ReadString16();
    u16string scAddr = data.ReadString16();
    int16_t port = data.ReadInt16();
    sptr<IRemoteObject> remoteSendCallback = data.ReadRemoteObject();
    sptr<IRemoteObject> remoteDeliveryCallback = data.ReadRemoteObject();
    if (remoteSendCallback != nullptr) {
        sendCallback = iface_cast<ISendShortMessageCallback>(remoteSendCallback);
    }
    if (remoteDeliveryCallback != nullptr) {
        deliveryCallback = iface_cast<IDeliveryShortMessageCallback>(remoteDeliveryCallback);
    }
    int16_t dataLen = data.ReadInt16();
    const uint8_t *buffer = reinterpret_cast<const uint8_t *>(data.ReadRawData(dataLen));
    RemoveSpacesInDesAddr(desAddr);
    int32_t result = SendMessage(slotId, desAddr, scAddr, port, buffer, dataLen, sendCallback, deliveryCallback);
    reply.WriteInt32(result);
}

void SmsInterfaceStub::RemoveSpacesInDesAddr(std::u16string &desAddr)
{
    // Remove spaces in desAddr
    std::string sourceAddr = StringUtils::ToUtf8(desAddr);
    std::string resultAddr = sourceAddr;
    int32_t count = static_cast<int32_t>(sourceAddr.size());
    int32_t indexDes = 0;
    int32_t indexResult = 0;
    while (indexDes < count) {
        if (desAddr[indexDes] != ' ') {
            resultAddr[indexResult] = desAddr[indexDes];
            indexResult++;
        }
        indexDes++;
    }
    sourceAddr = resultAddr.substr(0, indexResult);
    desAddr = StringUtils::ToUtf16(sourceAddr);
}

void SmsInterfaceStub::OnSetSmscAddr(MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    int32_t slotId = data.ReadInt32();
    std::u16string scAddr = data.ReadString16();
    int32_t result = SetSmscAddr(slotId, scAddr);
    TELEPHONY_LOGI("SetSmscAddr result %{public}d", result);
    reply.WriteInt32(result);
}

void SmsInterfaceStub::OnGetSmscAddr(MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    std::u16string smscAddress;
    int32_t slotId = data.ReadInt32();
    int32_t result = GetSmscAddr(slotId, smscAddress);
    TELEPHONY_LOGI("GetSmscAddr result size %{public}d", result);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("SmsInterfaceStub::OnGetSmscAddr write reply failed.");
        return;
    }
    if (result != TELEPHONY_ERR_SUCCESS) {
        TELEPHONY_LOGE("SmsInterfaceStub::OnGetSmscAddr result is not TELEPHONY_ERR_SUCCESS.");
        return;
    }

    if (!reply.WriteString16(smscAddress)) {
        TELEPHONY_LOGE("SmsInterfaceStub::OnGetSmscAddr write reply failed.");
        return;
    }
}

void SmsInterfaceStub::OnAddSimMessage(MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    int32_t slotId = data.ReadInt32();
    std::u16string smsc = data.ReadString16();
    std::u16string pdu = data.ReadString16();
    uint32_t status = data.ReadUint32();
    int32_t result = AddSimMessage(slotId, smsc, pdu, static_cast<SimMessageStatus>(status));
    TELEPHONY_LOGI("AddSimMessage result %{public}d", result);
    reply.WriteInt32(result);
}

void SmsInterfaceStub::OnDelSimMessage(MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    int32_t slotId = data.ReadInt32();
    uint32_t msgIndex = data.ReadUint32();
    int32_t result = DelSimMessage(slotId, msgIndex);
    TELEPHONY_LOGI("DelSimMessage result %{public}d", result);
    reply.WriteInt32(result);
}

void SmsInterfaceStub::OnUpdateSimMessage(MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    int32_t slotId = data.ReadInt32();
    uint32_t msgIndex = data.ReadUint32();
    uint32_t newStatus = data.ReadUint32();
    std::u16string pdu = data.ReadString16();
    std::u16string smsc = data.ReadString16();
    int32_t result = UpdateSimMessage(slotId, msgIndex, static_cast<SimMessageStatus>(newStatus), pdu, smsc);
    TELEPHONY_LOGI("UpdateSimMessage result %{public}d", result);
    reply.WriteInt32(result);
}

void SmsInterfaceStub::OnGetAllSimMessages(MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    std::vector<ShortMessage> message;
    int32_t slotId = data.ReadInt32();
    int32_t result = GetAllSimMessages(slotId, message);
    TELEPHONY_LOGI("GetAllSimMessages result %{public}d size %{public}zu", result, message.size());
    reply.WriteInt32(result);
    if (result != TELEPHONY_ERR_SUCCESS) {
        TELEPHONY_LOGE("SmsInterfaceStub::OnGetSmscAddr result is not TELEPHONY_ERR_SUCCESS.");
        return;
    }
    int32_t resultLen = static_cast<int32_t>(message.size());
    reply.WriteInt32(resultLen);
    for (const auto &v : message) {
        v.Marshalling(reply);
    }
}

void SmsInterfaceStub::OnSetCBConfig(MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    int32_t slotId = data.ReadInt32();
    bool enable = data.ReadBool();
    uint32_t fromMsgId = data.ReadUint32();
    uint32_t toMsgId = data.ReadUint32();
    uint8_t ranType = data.ReadUint8();
    int32_t result = SetCBConfig(slotId, enable, fromMsgId, toMsgId, ranType);
    TELEPHONY_LOGI("OnSetCBConfig result %{public}d", result);
    reply.WriteInt32(result);
}

void SmsInterfaceStub::OnSetImsSmsConfig(MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    bool result = false;
    int32_t slotId = data.ReadInt32();
    int32_t enable = data.ReadInt32();
    result = SetImsSmsConfig(slotId, enable);
    TELEPHONY_LOGI("SetImsSmsConfig result %{public}d", result);
    reply.WriteBool(result);
}

void SmsInterfaceStub::OnSetDefaultSmsSlotId(MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    int32_t slotId = data.ReadInt32();
    int32_t result = SetDefaultSmsSlotId(slotId);
    TELEPHONY_LOGI("SetDefaultSmsSlotId result %{public}d", result);
    reply.WriteInt32(result);
}

void SmsInterfaceStub::OnGetDefaultSmsSlotId(MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    int32_t result = 0;
    result = GetDefaultSmsSlotId();
    TELEPHONY_LOGI("SetDefaultSmsSlotId result %{public}d", result);
    reply.WriteInt32(result);
}

void SmsInterfaceStub::OnSplitMessage(MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    std::vector<std::u16string> splitMessage;
    std::u16string message = data.ReadString16();
    int32_t result = SplitMessage(message, splitMessage);
    reply.WriteInt32(result);
    if (result != TELEPHONY_ERR_SUCCESS) {
        TELEPHONY_LOGE("SmsInterfaceStub::OnSplitMessage result is not TELEPHONY_ERR_SUCCESS.");
        return;
    }
    int32_t resultLen = static_cast<int32_t>(splitMessage.size());
    TELEPHONY_LOGI("SplitMessage size %{public}d", resultLen);
    reply.WriteInt32(resultLen);
    for (const auto &item : splitMessage) {
        reply.WriteString16(item);
    }
}

void SmsInterfaceStub::OnGetSmsSegmentsInfo(MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    int32_t slotId = data.ReadInt32();
    std::u16string message = data.ReadString16();
    bool force7BitCode = data.ReadBool();

    SmsSegmentsInfo segInfo;
    int32_t result = GetSmsSegmentsInfo(slotId, message, force7BitCode, segInfo);
    reply.WriteInt32(result);

    if (result == TELEPHONY_ERR_SUCCESS) {
        reply.WriteInt32(segInfo.msgSegCount);
        reply.WriteInt32(segInfo.msgEncodingCount);
        reply.WriteInt32(segInfo.msgRemainCount);
        reply.WriteInt32(static_cast<int32_t>(segInfo.msgCodeScheme));
    }
}

void SmsInterfaceStub::OnIsImsSmsSupported(MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    int32_t slotId = data.ReadInt32();
    bool isSupported = false;
    int32_t result = IsImsSmsSupported(slotId, isSupported);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("SmsInterfaceStub::OnIsImsSmsSupported write reply failed.");
        return;
    }
    if (result != TELEPHONY_ERR_SUCCESS) {
        TELEPHONY_LOGE("SmsInterfaceStub::OnIsImsSmsSupported result is not TELEPHONY_ERR_SUCCESS.");
        return;
    }
    if (!reply.WriteBool(isSupported)) {
        TELEPHONY_LOGE("SmsInterfaceStub::OnIsImsSmsSupported write reply failed.");
        return;
    }
}

void SmsInterfaceStub::OnGetImsShortMessageFormat(MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    std::u16string format;
    int32_t result = GetImsShortMessageFormat(format);
    reply.WriteInt32(result);
    if (result != TELEPHONY_ERR_SUCCESS) {
        TELEPHONY_LOGE("SmsInterfaceStub::OnGetImsShortMessageFormat result is not TELEPHONY_ERR_SUCCESS.");
        return;
    }
    reply.WriteString16(format);
}

void SmsInterfaceStub::OnHasSmsCapability(MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    reply.WriteBool(HasSmsCapability());
}

void SmsInterfaceStub::OnCreateMessage(MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    std::string pdu = data.ReadString();
    std::string specification = data.ReadString();
    ShortMessage message;
    int32_t result = CreateMessage(pdu, specification, message);

    reply.WriteInt32(result);
    if (result != TELEPHONY_ERR_SUCCESS) {
        return;
    }
    if (!message.Marshalling(reply)) {
        TELEPHONY_LOGE("SmsInterfaceStub::OnCreateMessage fail");
    }
}

void SmsInterfaceStub::OnGetBase64Encode(MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    bool result = false;

    u16string src = data.ReadString16();
    std::string dest;
    result = GetBase64Encode(StringUtils::ToUtf8(src), dest);
    reply.WriteBool(result);
    if (!result) {
        return;
    }
    reply.WriteString16(StringUtils::ToUtf16(dest));
}

void SmsInterfaceStub::OnGetBase64Decode(MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    bool result = false;
    u16string src = data.ReadString16();
    std::string dest;
    result = GetBase64Decode(StringUtils::ToUtf8(src), dest);
    reply.WriteBool(result);
    if (!result) {
        return;
    }
    reply.WriteString16(StringUtils::ToUtf16(dest));
}

void SmsInterfaceStub::OnGetEncodeStringFunc(MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    bool result = false;
    uint32_t charset = data.ReadUint32();
    uint32_t valLength = data.ReadUint32();
    u16string strEncodeString = data.ReadString16();
    std::string str = StringUtils::ToUtf8(strEncodeString);
    std::string encodeString;

    if (valLength != str.length()) {
        TELEPHONY_LOGE("invalid valLength!");
        return;
    }

    result = GetEncodeStringFunc(encodeString, charset, valLength, str);
    reply.WriteBool(result);
    if (!result) {
        return;
    }
    reply.WriteString16(StringUtils::ToUtf16(encodeString));
}

int SmsInterfaceStub::OnRemoteRequest(
    uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    std::u16string myDescripter = SmsInterfaceStub::GetDescriptor();
    std::u16string remoteDescripter = data.ReadInterfaceToken();
    if (myDescripter != remoteDescripter) {
        TELEPHONY_LOGE("descriptor checked fail");
        return TELEPHONY_ERR_DESCRIPTOR_MISMATCH;
    }

    auto itFunc = memberFuncMap_.find(code);
    if (itFunc != memberFuncMap_.end()) {
        auto memberFunc = itFunc->second;
        if (memberFunc != nullptr) {
            (this->*memberFunc)(data, reply, option);
            return TELEPHONY_ERR_SUCCESS;
        }
    }
    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}
} // namespace Telephony
} // namespace OHOS
