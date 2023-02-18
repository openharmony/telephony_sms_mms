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

#ifndef SMS_INTERFACE_STUB_H
#define SMS_INTERFACE_STUB_H

#include <list>
#include <memory>
#include <mutex>
#include <string>

#include "iremote_stub.h"

#include "i_sms_service_interface.h"
#include "sms_interface_manager.h"
#include "sms_receive_manager.h"
#include "sms_send_manager.h"

namespace OHOS {
namespace Telephony {
class SmsInterfaceStub : public IRemoteStub<ISmsServiceInterface> {
public:
    SmsInterfaceStub();
    ~SmsInterfaceStub();
    virtual int OnRemoteRequest(
        uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;
    std::shared_ptr<SmsInterfaceManager> GetSmsInterfaceManager(int32_t slotId);
    std::shared_ptr<SmsInterfaceManager> GetSmsInterfaceManager();

protected:
    void InitModule();

private:
    void OnSendSmsTextRequest(MessageParcel &data, MessageParcel &reply, MessageOption &option);
    void OnSendSmsDataRequest(MessageParcel &data, MessageParcel &reply, MessageOption &option);
    void OnSetSmscAddr(MessageParcel &data, MessageParcel &reply, MessageOption &option);
    void OnGetSmscAddr(MessageParcel &data, MessageParcel &reply, MessageOption &option);
    void OnAddSimMessage(MessageParcel &data, MessageParcel &reply, MessageOption &option);
    void OnDelSimMessage(MessageParcel &data, MessageParcel &reply, MessageOption &option);
    void OnUpdateSimMessage(MessageParcel &data, MessageParcel &reply, MessageOption &option);
    void OnGetAllSimMessages(MessageParcel &data, MessageParcel &reply, MessageOption &option);
    void OnSetCBRangeConfig(MessageParcel &data, MessageParcel &reply, MessageOption &option);
    void OnSetCBConfig(MessageParcel &data, MessageParcel &reply, MessageOption &option);
    void OnSetImsSmsConfig(MessageParcel &data, MessageParcel &reply, MessageOption &option);
    void OnSetDefaultSmsSlotId(MessageParcel &data, MessageParcel &reply, MessageOption &option);
    void OnGetDefaultSmsSlotId(MessageParcel &data, MessageParcel &reply, MessageOption &option);
    void OnSplitMessage(MessageParcel &data, MessageParcel &reply, MessageOption &option);
    void OnGetSmsSegmentsInfo(MessageParcel &data, MessageParcel &reply, MessageOption &option);
    void OnIsImsSmsSupported(MessageParcel &data, MessageParcel &reply, MessageOption &option);
    void OnGetImsShortMessageFormat(MessageParcel &data, MessageParcel &reply, MessageOption &option);
    void OnHasSmsCapability(MessageParcel &data, MessageParcel &reply, MessageOption &option);
    void OnCreateMessage(MessageParcel &data, MessageParcel &reply, MessageOption &option);
    void OnGetBase64Encode(MessageParcel &data, MessageParcel &reply, MessageOption &option);
    void OnGetBase64Decode(MessageParcel &data, MessageParcel &reply, MessageOption &option);
    void OnGetEncodeStringFunc(MessageParcel &data, MessageParcel &reply, MessageOption &option);
    void RemoveSpacesInDesAddr(std::u16string &desAddr);

    std::map<uint32_t, std::shared_ptr<SmsInterfaceManager>> slotSmsInterfaceManagerMap_;
    using SmsServiceFunc = void (SmsInterfaceStub::*)(MessageParcel &data, MessageParcel &reply, MessageOption &option);
    std::map<uint32_t, SmsServiceFunc> memberFuncMap_;
    std::mutex mutex_;
};
} // namespace Telephony
} // namespace OHOS
#endif