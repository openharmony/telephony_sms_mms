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

#ifndef SMS_SERVICE_H
#define SMS_SERVICE_H

#include <memory>

#include "sms_interface_stub.h"
#include "system_ability.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace Telephony {
enum ServiceRunningState { STATE_NOT_START, STATE_RUNNING };

class SmsService : public SystemAbility, public SmsInterfaceStub, public std::enable_shared_from_this<SmsService> {
    DECLARE_DELAYED_SINGLETON(SmsService)
    DECLARE_SYSTEM_ABILITY(SmsService) // necessary
public:
    void OnStart() override;
    void OnStop() override;
    void OnDump() override;
    void SendMessage(int32_t slotId, const std::u16string desAddr, const std::u16string scAddr,
        const std::u16string text, const sptr<ISendShortMessageCallback> &sendCallback,
        const sptr<IDeliveryShortMessageCallback> &deliveryCallback) override;
    void SendMessage(int32_t slotId, const std::u16string desAddr, const std::u16string scAddr, uint16_t port,
        const uint8_t *data, uint16_t dataLen, const sptr<ISendShortMessageCallback> &sendCallback,
        const sptr<IDeliveryShortMessageCallback> &deliveryCallback) override;
    bool SetSmscAddr(int32_t slotId, const std::u16string &scAddr) override;
    std::u16string GetSmscAddr(int32_t slotId) override;
    bool AddSimMessage(
        int32_t slotId, const std::u16string &smsc, const std::u16string &pdu, SimMessageStatus status) override;
    bool DelSimMessage(int32_t slotId, uint32_t msgIndex) override;
    bool UpdateSimMessage(int32_t slotId, uint32_t msgIndex, SimMessageStatus newStatus, const std::u16string &pdu,
        const std::u16string &smsc) override;
    std::vector<ShortMessage> GetAllSimMessages(int32_t slotId) override;
    bool SetCBConfig(int32_t slotId, bool enable, uint32_t fromMsgId, uint32_t toMsgId, uint8_t netType) override;
    bool SetDefaultSmsSlotId(int32_t slotId) override;
    int32_t GetDefaultSmsSlotId() override;
    std::vector<std::u16string> SplitMessage(const std::u16string &message) override;
    std::vector<int32_t> CalculateLength(const std::u16string &message, bool force7BitCode) override;
    std::string GetBindTime();
    int32_t Dump(std::int32_t fd, const std::vector<std::u16string> &args) override;

private:
    constexpr static uint32_t CONNECT_MAX_TRY_COUNT = 20;
    constexpr static uint32_t CONNECT_SERVICE_WAIT_TIME = 2000; // ms

    bool Init();
    bool WaitCoreServiceToInit();

    bool registerToService_ = false;
    ServiceRunningState state_ = ServiceRunningState::STATE_NOT_START;
    int64_t bindTime_ = 0L;
};
} // namespace Telephony
} // namespace OHOS
#endif