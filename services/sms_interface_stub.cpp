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
    for (auto &iter : slotSmsInterfaceManagerMap_) {
        if (iter.second != nullptr) {
            return iter.second;
        }
    }
    return nullptr;
}

void SmsInterfaceStub::OnSendSmsTextRequest(MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    int32_t result = 0;
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
    SendMessage(slotId, desAddr, scAddr, text, sendCallback, deliveryCallback);
    reply.WriteInt32(result);
}

void SmsInterfaceStub::OnSendSmsDataRequest(MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    int32_t result = 0;
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
    SendMessage(slotId, desAddr, scAddr, port, buffer, dataLen, sendCallback, deliveryCallback);
    reply.WriteInt32(result);
}

void SmsInterfaceStub::OnSetSmscAddr(MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    bool result = false;
    int32_t slotId = data.ReadInt32();
    std::u16string scAddr = data.ReadString16();
    result = SetSmscAddr(slotId, scAddr);
    TELEPHONY_LOGI("SetSmscAddr result %{public}d", result);
    reply.WriteBool(result);
}

void SmsInterfaceStub::OnGetSmscAddr(MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    std::u16string result;
    int32_t slotId = data.ReadInt32();
    result = GetSmscAddr(slotId);
    TELEPHONY_LOGI("GetSmscAddr result size %{public}zu", result.size());
    reply.WriteString16(result);
}

void SmsInterfaceStub::OnAddSimMessage(MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    bool result = false;
    int32_t slotId = data.ReadInt32();
    std::u16string smsc = data.ReadString16();
    std::u16string pdu = data.ReadString16();
    uint32_t status = data.ReadUint32();
    result = AddSimMessage(slotId, smsc, pdu, static_cast<SimMessageStatus>(status));
    TELEPHONY_LOGI("AddSimMessage result %{public}d", result);
    reply.WriteBool(result);
}

void SmsInterfaceStub::OnDelSimMessage(MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    bool result = false;
    int32_t slotId = data.ReadInt32();
    uint32_t msgIndex = data.ReadUint32();
    result = DelSimMessage(slotId, msgIndex);
    TELEPHONY_LOGI("DelSimMessage result %{public}d", result);
    reply.WriteBool(result);
}

void SmsInterfaceStub::OnUpdateSimMessage(MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    bool result = false;
    int32_t slotId = data.ReadInt32();
    uint32_t msgIndex = data.ReadUint32();
    uint32_t newStatus = data.ReadUint32();
    std::u16string pdu = data.ReadString16();
    std::u16string smsc = data.ReadString16();
    result = UpdateSimMessage(slotId, msgIndex, static_cast<SimMessageStatus>(newStatus), pdu, smsc);
    TELEPHONY_LOGI("UpdateSimMessage result %{public}d", result);
    reply.WriteBool(result);
}

void SmsInterfaceStub::OnGetAllSimMessages(MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    std::vector<ShortMessage> result;
    int32_t slotId = data.ReadInt32();
    result = GetAllSimMessages(slotId);
    TELEPHONY_LOGI("GetAllSimMessages size %{public}zu", result.size());

    int32_t resultLen = static_cast<int32_t>(result.size());
    reply.WriteInt32(resultLen);
    for (const auto &v : result) {
        v.Marshalling(reply);
    }
}

void SmsInterfaceStub::OnSetCBConfig(MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    bool result = false;
    int32_t slotId = data.ReadInt32();
    bool enable = data.ReadBool();
    uint32_t fromMsgId = data.ReadUint32();
    uint32_t toMsgId = data.ReadUint32();
    uint8_t ranType = data.ReadUint8();
    result = SetCBConfig(slotId, enable, fromMsgId, toMsgId, ranType);
    TELEPHONY_LOGI("OnSetCBConfig result %{public}d", result);
    reply.WriteBool(result);
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
    bool result = false;
    int32_t slotId = data.ReadInt32();
    result = SetDefaultSmsSlotId(slotId);
    TELEPHONY_LOGI("SetDefaultSmsSlotId result %{public}d", result);
    reply.WriteBool(result);
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
    std::vector<std::u16string> result;
    std::u16string message = data.ReadString16();
    result = SplitMessage(message);
    int32_t resultLen = static_cast<int32_t>(result.size());
    TELEPHONY_LOGI("SplitMessage size %{public}d", resultLen);
    reply.WriteInt32(resultLen);
    for (const auto &item : result) {
        reply.WriteString16(item);
    }
}

void SmsInterfaceStub::OnGetSmsSegmentsInfo(MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    int32_t slotId = data.ReadInt32();
    std::u16string message = data.ReadString16();
    bool force7BitCode = data.ReadBool();

    SmsSegmentsInfo segInfo;
    bool result = GetSmsSegmentsInfo(slotId, message, force7BitCode, segInfo);
    reply.WriteBool(result);

    if (result) {
        reply.WriteInt32(segInfo.msgSegCount);
        reply.WriteInt32(segInfo.msgEncodingCount);
        reply.WriteInt32(segInfo.msgRemainCount);
        reply.WriteInt32(static_cast<int32_t>(segInfo.msgCodeScheme));
    }
}

void SmsInterfaceStub::OnIsImsSmsSupported(MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    int32_t slotId = data.ReadInt32();
    reply.WriteBool(IsImsSmsSupported(slotId));
}

void SmsInterfaceStub::OnGetImsShortMessageFormat(MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    std::u16string format = GetImsShortMessageFormat();
    reply.WriteString16(format);
}

void SmsInterfaceStub::OnHasSmsCapability(MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    reply.WriteBool(HasSmsCapability());
}

int SmsInterfaceStub::OnRemoteRequest(
    uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    int32_t result = 0;
    TELEPHONY_LOGI("###################################### OnRemoteRequest");
    std::u16string myDescripter = SmsInterfaceStub::GetDescriptor();
    std::u16string remoteDescripter = data.ReadInterfaceToken();
    if (myDescripter == remoteDescripter) {
        auto itFunc = memberFuncMap_.find(code);
        if (itFunc != memberFuncMap_.end()) {
            auto memberFunc = itFunc->second;
            if (memberFunc != nullptr) {
                (this->*memberFunc)(data, reply, option);
            } else {
                TELEPHONY_LOGE("memberFunc is nullptr");
            }
        } else {
            TELEPHONY_LOGE("itFunc was not found");
        }
    } else {
        TELEPHONY_LOGE("descriptor checked fail");
        return result;
    }
    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}
} // namespace Telephony
} // namespace OHOS