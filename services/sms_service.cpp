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

#include "sms_service.h"

#include <string>

#include "sms_dump_helper.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
using namespace std;
using namespace AppExecFwk;
using namespace HiviewDFX;
bool g_registerResult = SystemAbility::MakeAndRegisterAbility(DelayedSingleton<SmsService>::GetInstance().get());

SmsService::SmsService() : SystemAbility(TELEPHONY_SMS_MMS_SYS_ABILITY_ID, true) {}

SmsService::~SmsService() {}

void SmsService::OnStart()
{
    if (state_ == ServiceRunningState::STATE_RUNNING) {
        TELEPHONY_LOGE("msService has already started.");
        return;
    }
    if (!Init()) {
        TELEPHONY_LOGE("failed to init SmsService");
        return;
    }
    bindTime_ =
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
            .count();
    state_ = ServiceRunningState::STATE_RUNNING;
    TELEPHONY_LOGI("SmsService::OnStart start service.");
}

bool SmsService::Init()
{
    if (!registerToService_) {
        bool ret = Publish(DelayedSingleton<SmsService>::GetInstance().get());
        if (!ret) {
            TELEPHONY_LOGE("SmsService::Init Publish failed!");
            return false;
        }
        registerToService_ = true;
        WaitCoreServiceToInit();
        InitModule();
    }
    return true;
}

void SmsService::OnStop()
{
    state_ = ServiceRunningState::STATE_NOT_START;
    registerToService_ = false;
    TELEPHONY_LOGI("SmsService::OnStop stop service.");
}

void SmsService::OnDump()
{
    TELEPHONY_LOGI("SmsService OnDump");
}

bool SmsService::WaitCoreServiceToInit()
{
    bool ret = false;
    for (uint32_t i = 0; i < CONNECT_MAX_TRY_COUNT; i++) {
        int slotId = CoreManager::DEFAULT_SLOT_ID;
        std::shared_ptr<Core> core = CoreManager::GetInstance().getCore(slotId);
        TELEPHONY_LOGI("connect core service count: %{public}d", i);
        if (core != nullptr && core->IsInitCore()) {
            ret = true;
            TELEPHONY_LOGI("SmsService Connection successful");
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(CONNECT_SERVICE_WAIT_TIME));
    }
    TELEPHONY_LOGI("SmsService connect core service init ok.");
    return ret;
}

void SmsService::SendMessage(int32_t slotId, const u16string desAddr, const u16string scAddr,
    const u16string text, const sptr<ISendShortMessageCallback> &sendCallback,
    const sptr<IDeliveryShortMessageCallback> &deliveryCallback)
{
    std::shared_ptr<SmsInterfaceManager> interfaceManager = GetSmsInterfaceManager(slotId);
    if (interfaceManager == nullptr) {
        SmsSender::SendResultCallBack(sendCallback, ISendShortMessageCallback::SEND_SMS_FAILURE_UNKNOWN);
        TELEPHONY_LOGE("SmsService::SendMessage interfaceManager== nullptr");
        return;
    }
    interfaceManager->TextBasedSmsDelivery(StringUtils::ToUtf8(desAddr), StringUtils::ToUtf8(scAddr),
        StringUtils::ToUtf8(text), sendCallback, deliveryCallback);
}

void SmsService::SendMessage(int32_t slotId, const u16string desAddr, const u16string scAddr, uint16_t port,
    const uint8_t *data, uint16_t dataLen, const sptr<ISendShortMessageCallback> &sendCallback,
    const sptr<IDeliveryShortMessageCallback> &deliveryCallback)
{
    std::shared_ptr<SmsInterfaceManager> interfaceManager = GetSmsInterfaceManager(slotId);
    if (interfaceManager == nullptr) {
        SmsSender::SendResultCallBack(sendCallback, ISendShortMessageCallback::SEND_SMS_FAILURE_UNKNOWN);
        TELEPHONY_LOGE("SmsService::SendMessage interfaceManager== nullptr");
        return;
    }
    interfaceManager->DataBasedSmsDelivery(StringUtils::ToUtf8(desAddr), StringUtils::ToUtf8(scAddr), port, data,
        dataLen, sendCallback, deliveryCallback);
}

bool SmsService::SetSmscAddr(int32_t slotId, const std::u16string &scAddr)
{
    bool result = false;
    std::shared_ptr<SmsInterfaceManager> interfaceManager = GetSmsInterfaceManager(slotId);
    if (interfaceManager == nullptr) {
        TELEPHONY_LOGE("SmsService::SetSmscAddr interfaceManager== nullptr");
        return result;
    }
    string sca = StringUtils::ToUtf8(scAddr);
    return interfaceManager->SetSmscAddr(sca);
}

std::u16string SmsService::GetSmscAddr(int32_t slotId)
{
    std::u16string result;
    std::shared_ptr<SmsInterfaceManager> interfaceManager = GetSmsInterfaceManager(slotId);
    if (interfaceManager == nullptr) {
        TELEPHONY_LOGE("SmsService::GetSmscAddr interfaceManager== nullptr");
        return result;
    }
    return StringUtils::ToUtf16(interfaceManager->GetSmscAddr());
}

bool SmsService::AddSimMessage(
    int32_t slotId, const std::u16string &smsc, const std::u16string &pdu, SimMessageStatus status)
{
    bool result = false;
    std::shared_ptr<SmsInterfaceManager> interfaceManager = GetSmsInterfaceManager(slotId);
    if (interfaceManager == nullptr) {
        TELEPHONY_LOGE("SmsService::AddSimMessage interfaceManager== nullptr");
        return result;
    }
    std::string smscData = StringUtils::ToUtf8(smsc);
    std::string pduData = StringUtils::ToUtf8(pdu);
    return interfaceManager->AddSimMessage(smscData, pduData, status);
}

bool SmsService::DelSimMessage(int32_t slotId, uint32_t msgIndex)
{
    bool result = false;
    std::shared_ptr<SmsInterfaceManager> interfaceManager = GetSmsInterfaceManager(slotId);
    if (interfaceManager == nullptr) {
        TELEPHONY_LOGE("SmsService::DelSimMessage interfaceManager== nullptr");
        return result;
    }
    return interfaceManager->DelSimMessage(msgIndex);
}

bool SmsService::UpdateSimMessage(int32_t slotId, uint32_t msgIndex, SimMessageStatus newStatus,
    const std::u16string &pdu, const std::u16string &smsc)
{
    bool result = false;
    std::shared_ptr<SmsInterfaceManager> interfaceManager = GetSmsInterfaceManager(slotId);
    if (interfaceManager == nullptr) {
        TELEPHONY_LOGE("SmsService::UpdateSimMessage interfaceManager== nullptr");
        return result;
    }
    std::string pduData = StringUtils::ToUtf8(pdu);
    std::string smscData = StringUtils::ToUtf8(smsc);
    return interfaceManager->UpdateSimMessage(msgIndex, newStatus, pduData, smscData);
}

std::vector<ShortMessage> SmsService::GetAllSimMessages(int32_t slotId)
{
    std::vector<ShortMessage> result;
    std::shared_ptr<SmsInterfaceManager> interfaceManager = GetSmsInterfaceManager(slotId);
    if (interfaceManager == nullptr) {
        TELEPHONY_LOGE("SmsService::GetAllSimMessages interfaceManager== nullptr");
        return result;
    }
    return interfaceManager->GetAllSimMessages();
}

bool SmsService::SetCBConfig(
    int32_t slotId, bool enable, uint32_t fromMsgId, uint32_t toMsgId, uint8_t netType)
{
    bool result = false;
    std::shared_ptr<SmsInterfaceManager> interfaceManager = GetSmsInterfaceManager(slotId);
    if (interfaceManager == nullptr) {
        TELEPHONY_LOGE("SmsService::SetCBConfig interfaceManager== nullptr");
        return result;
    }
    return interfaceManager->SetCBConfig(enable, fromMsgId, toMsgId, netType);
}

bool SmsService::SetDefaultSmsSlotId(int32_t slotId)
{
    bool result = false;
    std::shared_ptr<SmsInterfaceManager> interfaceManager = GetSmsInterfaceManager();
    if (interfaceManager == nullptr) {
        TELEPHONY_LOGE("SmsService::SetCBConfig interfaceManager== nullptr");
        return result;
    }
    return interfaceManager->SetDefaultSmsSlotId(slotId);
}

int32_t SmsService::GetDefaultSmsSlotId()
{
    int32_t result = -1;
    std::shared_ptr<SmsInterfaceManager> interfaceManager = GetSmsInterfaceManager();
    if (interfaceManager == nullptr) {
        TELEPHONY_LOGE("SmsService::SetCBConfig interfaceManager== nullptr");
        return result;
    }
    return interfaceManager->GetDefaultSmsSlotId();
}

std::vector<std::u16string> SmsService::SplitMessage(const std::u16string &message)
{
    std::vector<std::string> temp;
    std::vector<std::u16string> result;
    if (message.empty()) {
        return result;
    }
    int32_t slotId = GetDefaultSmsSlotId();
    std::shared_ptr<SmsInterfaceManager> interfaceManager = GetSmsInterfaceManager(slotId);
    if (interfaceManager == nullptr) {
        TELEPHONY_LOGE("SmsService::SplitMessage interfaceManager== nullptr");
        return result;
    }
    std::string messageData = StringUtils::ToUtf8(message);
    temp = interfaceManager->SplitMessage(messageData);
    for (auto &item : temp) {
        result.emplace_back(StringUtils::ToUtf16(item));
    }
    return result;
}

std::vector<int32_t> SmsService::CalculateLength(const std::u16string &message, bool force7BitCode)
{
    std::vector<int32_t> result;
    if (message.empty()) {
        return result;
    }
    int32_t slotId = GetDefaultSmsSlotId();
    std::shared_ptr<SmsInterfaceManager> interfaceManager = GetSmsInterfaceManager(slotId);
    if (interfaceManager == nullptr) {
        TELEPHONY_LOGE("CalculateLength interfaceManager is nullptr");
        return result;
    }
    std::string messageData = StringUtils::ToUtf8(message);
    return interfaceManager->CalculateLength(messageData, force7BitCode);
}

int32_t SmsService::Dump(std::int32_t fd, const std::vector<std::u16string> &args)
{
    if (fd < 0) {
        TELEPHONY_LOGE("dump fd invalid");
        return TELEPHONY_ERR_FAIL;
    }
    std::vector<std::string> argsInStr;
    for (const auto &arg : args) {
        TELEPHONY_LOGI("Dump args: %s", Str16ToStr8(arg).c_str());
        argsInStr.emplace_back(Str16ToStr8(arg));
    }
    std::string result;
    SmsDumpHelper dumpHelper;
    if (dumpHelper.Dump(argsInStr, result)) {
        TELEPHONY_LOGI("%s", result.c_str());
        std::int32_t ret = dprintf(fd, "%s", result.c_str());
        if (ret < 0) {
            TELEPHONY_LOGE("dprintf to dump fd failed");
            return TELEPHONY_ERR_FAIL;
        }
        return TELEPHONY_SUCCESS;
    }
    TELEPHONY_LOGW("dumpHelper failed");
    return TELEPHONY_ERR_FAIL;
}

std::string SmsService::GetBindTime()
{
    return std::to_string(bindTime_);
}
} // namespace Telephony
} // namespace OHOS