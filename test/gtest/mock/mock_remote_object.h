/*
 * Copyright (C) 2024-2024 Huawei Device Co., Ltd.
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
#ifndef __MOCK_REMOTE_OBJECT_H__
#define __MOCK_REMOTE_OBJECT_H__
#include "iremote_object.h"
#include "message_parcel.h"

namespace OHOS {
namespace Telephony {

class MockRemoteObject final : public IRemoteObject {
public:
    MockRemoteObject() : IRemoteObject(u"")
    {
    }
    int32_t GetObjectRefCount() override
    {
        return retGetObjectRefCount;
    }
    int SendRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override
    {
        return retSendRequest;
    }
    bool AddDeathRecipient(const sptr<DeathRecipient> &recipient) override
    {
        return retAddDeathRecipient;
    }
    bool RemoveDeathRecipient(const sptr<DeathRecipient> &recipient) override
    {
        return retRemoveDeathRecipient;
    }
    int Dump(int fd, const std::vector<std::u16string> &args) override
    {
        return retDump;
    }
public:
    int32_t retGetObjectRefCount;
    int     retSendRequest;
    bool    retAddDeathRecipient;
    bool    retRemoveDeathRecipient;
    int     retDump;
};
}
}
#endif