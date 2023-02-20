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

#include <regex>

#include "cdma_sms_message.h"
#include "core_manager_inner.h"
#include "ims_sms_client.h"
#include "msg_text_convert.h"
#include "sms_dump_helper.h"
#include "sms_hisysevent.h"
#include "string_utils.h"
#include "telephony_log_wrapper.h"
#include "telephony_permission.h"

namespace OHOS {
namespace Telephony {
using namespace std;
using namespace AppExecFwk;
using namespace HiviewDFX;
constexpr static uint32_t CONNECT_SERVICE_WAIT_TIME = 2000;
constexpr static size_t MIN_LEN = 1;
bool g_registerResult = SystemAbility::MakeAndRegisterAbility(DelayedSingleton<SmsService>::GetInstance().get());

SmsService::SmsService() : SystemAbility(TELEPHONY_SMS_MMS_SYS_ABILITY_ID, true) {}

SmsService::~SmsService() {}

void SmsService::OnStart()
{
    TELEPHONY_LOGI("SmsService::OnStart start service Enter.");
    bindTime_ =
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
            .count();
    if (state_ == ServiceRunningState::STATE_RUNNING) {
        TELEPHONY_LOGE("msService has already started.");
        return;
    }
    if (!Init()) {
        TELEPHONY_LOGE("failed to init SmsService");
        return;
    }
    state_ = ServiceRunningState::STATE_RUNNING;
    endTime_ =
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
            .count();
    TELEPHONY_LOGI("SmsService::OnStart start service Exit.");
}

bool SmsService::Init()
{
    DelayedSingleton<ImsSmsClient>::GetInstance()->Init();
    if (!registerToService_) {
        bool ret = Publish(DelayedSingleton<SmsService>::GetInstance().get());
        if (!ret) {
            TELEPHONY_LOGE("SmsService::Init Publish failed!");
            return false;
        }
        registerToService_ = true;
        WaitCoreServiceToInit();
    }
    DelayedSingleton<SmsStateHandler>::GetInstance()->Init();
    return true;
}

void SmsService::OnStop()
{
    state_ = ServiceRunningState::STATE_NOT_START;
    registerToService_ = false;
    DelayedSingleton<ImsSmsClient>::GetInstance()->UnInit();
    DelayedSingleton<SmsStateHandler>::GetInstance()->UnInit();
    TELEPHONY_LOGI("SmsService::OnStop stop service.");
}

int32_t SmsService::Dump(std::int32_t fd, const std::vector<std::u16string> &args)
{
    if (fd < 0) {
        TELEPHONY_LOGE("dump fd invalid");
        return TELEPHONY_ERR_FAIL;
    }
    std::vector<std::string> argsInStr;
    for (const auto &arg : args) {
        TELEPHONY_LOGI("Dump args: %{public}s", Str16ToStr8(arg).c_str());
        argsInStr.emplace_back(Str16ToStr8(arg));
    }
    std::string result;
    SmsDumpHelper dumpHelper;
    if (dumpHelper.Dump(argsInStr, result)) {
        TELEPHONY_LOGI("%{public}s", result.c_str());
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

void SmsService::WaitCoreServiceToInit()
{
    std::thread connectTask([&]() {
        while (true) {
            if (CoreManagerInner::GetInstance().IsInitFinished()) {
                InitModule();
                TELEPHONY_LOGI("SmsService Connection successful");
                break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(CONNECT_SERVICE_WAIT_TIME));
        }
    });
    connectTask.detach();
}

std::string SmsService::GetBindTime()
{
    return std::to_string(bindTime_);
}

int32_t SmsService::SendMessage(int32_t slotId, const u16string desAddr, const u16string scAddr, const u16string text,
    const sptr<ISendShortMessageCallback> &sendCallback, const sptr<IDeliveryShortMessageCallback> &deliveryCallback)
{
    if (NoPermissionOrParametersCheckFail(slotId, desAddr, sendCallback)) {
        SmsHiSysEvent::WriteSmsSendFaultEvent(slotId, SmsMmsMessageType::SMS_SHORT_MESSAGE,
            SmsMmsErrorCode::SMS_ERROR_PERMISSION_ERROR, Permission::SEND_MESSAGES);
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    std::shared_ptr<SmsInterfaceManager> interfaceManager = GetSmsInterfaceManager(slotId);
    if (interfaceManager == nullptr) {
        SmsSender::SendResultCallBack(sendCallback, ISendShortMessageCallback::SEND_SMS_FAILURE_UNKNOWN);
        SmsHiSysEvent::WriteSmsSendFaultEvent(slotId, SmsMmsMessageType::SMS_SHORT_MESSAGE,
            SmsMmsErrorCode::SMS_ERROR_NULL_POINTER, "text sms interfaceManager is nullptr");
        TELEPHONY_LOGE("SmsService::SendMessage interfaceManager nullptr error.");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    if (!ValidDestinationAddress(StringUtils::ToUtf8(desAddr))) {
        SmsSender::SendResultCallBack(sendCallback, ISendShortMessageCallback::SEND_SMS_FAILURE_UNKNOWN);
        TELEPHONY_LOGE("SmsService::SendMessage desAddr not conform to the regular specification");
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }
    return interfaceManager->TextBasedSmsDelivery(StringUtils::ToUtf8(desAddr), StringUtils::ToUtf8(scAddr),
        StringUtils::ToUtf8(text), sendCallback, deliveryCallback);
}

int32_t SmsService::SendMessage(int32_t slotId, const u16string desAddr, const u16string scAddr, uint16_t port,
    const uint8_t *data, uint16_t dataLen, const sptr<ISendShortMessageCallback> &sendCallback,
    const sptr<IDeliveryShortMessageCallback> &deliveryCallback)
{
    if (NoPermissionOrParametersCheckFail(slotId, desAddr, sendCallback)) {
        SmsHiSysEvent::WriteSmsSendFaultEvent(slotId, SmsMmsMessageType::SMS_SHORT_MESSAGE,
            SmsMmsErrorCode::SMS_ERROR_PERMISSION_ERROR, Permission::SEND_MESSAGES);
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    std::shared_ptr<SmsInterfaceManager> interfaceManager = GetSmsInterfaceManager(slotId);
    if (interfaceManager == nullptr) {
        SmsSender::SendResultCallBack(sendCallback, ISendShortMessageCallback::SEND_SMS_FAILURE_UNKNOWN);
        TELEPHONY_LOGE("SmsService::SendMessage interfaceManager nullptr error.");
        SmsHiSysEvent::WriteSmsSendFaultEvent(slotId, SmsMmsMessageType::SMS_SHORT_MESSAGE,
            SmsMmsErrorCode::SMS_ERROR_NULL_POINTER, "data sms interfaceManager is nullptr");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    if (!ValidDestinationAddress(StringUtils::ToUtf8(desAddr))) {
        SmsSender::SendResultCallBack(sendCallback, ISendShortMessageCallback::SEND_SMS_FAILURE_UNKNOWN);
        TELEPHONY_LOGE("SmsService::SendMessage desAddr not conform to the regular specification");
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }
    return interfaceManager->DataBasedSmsDelivery(
        StringUtils::ToUtf8(desAddr), StringUtils::ToUtf8(scAddr), port, data, dataLen, sendCallback, deliveryCallback);
}

bool SmsService::NoPermissionOrParametersCheckFail(
    int32_t slotId, const u16string desAddr, const sptr<ISendShortMessageCallback> &sendCallback)
{
    if (!TelephonyPermission::CheckPermission(Permission::SEND_MESSAGES)) {
        SmsSender::SendResultCallBack(sendCallback, ISendShortMessageCallback::SEND_SMS_FAILURE_UNKNOWN);
        TELEPHONY_LOGE("Check Permission Failed, No Has Telephony Send Messages Permisson.");
        return true;
    }

    if (desAddr.empty()) {
        SmsSender::SendResultCallBack(sendCallback, ISendShortMessageCallback::SEND_SMS_FAILURE_UNKNOWN);
        TELEPHONY_LOGE("SmsService::SendMessage desAddr invalid.");
        return true;
    }

    return false;
}

int32_t SmsService::IsImsSmsSupported(int32_t slotId, bool &isSupported)
{
    std::shared_ptr<SmsInterfaceManager> interfaceManager = GetSmsInterfaceManager(slotId);
    if (interfaceManager == nullptr) {
        TELEPHONY_LOGE("interfaceManager is nullptr.");
        return TELEPHONY_ERR_SLOTID_INVALID;
    }
    return interfaceManager->IsImsSmsSupported(slotId, isSupported);
}

int32_t SmsService::GetImsShortMessageFormat(std::u16string &format)
{
    int32_t slotId = GetDefaultSmsSlotId();
    std::shared_ptr<SmsInterfaceManager> interfaceManager = GetSmsInterfaceManager(slotId);
    if (interfaceManager == nullptr) {
        TELEPHONY_LOGE("interfaceManager is nullptr error.");
        return TELEPHONY_ERR_SLOTID_INVALID;
    }
    return interfaceManager->GetImsShortMessageFormat(format);
}

bool SmsService::HasSmsCapability()
{
    int32_t slotId = GetDefaultSmsSlotId();
    std::shared_ptr<SmsInterfaceManager> interfaceManager = GetSmsInterfaceManager(slotId);
    if (interfaceManager == nullptr) {
        TELEPHONY_LOGE("Get SmsInterfaceManager is nullptr error.");
        return true;
    }
    return interfaceManager->HasSmsCapability();
}

int32_t SmsService::SetSmscAddr(int32_t slotId, const std::u16string &scAddr)
{
    if (!TelephonyPermission::CheckPermission(Permission::SET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("Check Permission Failed, No Has Telephony Set State Permisson.");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }

    std::shared_ptr<SmsInterfaceManager> interfaceManager = GetSmsInterfaceManager(slotId);
    if (interfaceManager == nullptr) {
        TELEPHONY_LOGE("interfaceManager is nullptr error.");
        return TELEPHONY_ERR_SLOTID_INVALID;
    }
    string sca = StringUtils::ToUtf8(scAddr);
    TrimSmscAddr(sca);
    if (sca.empty() || sca.length() == 0) {
        TELEPHONY_LOGE("sca is empty");
        return TELEPHONY_ERR_ARGUMENT_NULL;
    }
    return interfaceManager->SetSmscAddr(sca);
}

void SmsService::TrimSmscAddr(std::string &sca)
{
    if (sca.length() < MIN_LEN) {
        return;
    }
    while (sca.length() >= MIN_LEN && sca[0] == ' ') {
        sca.erase(0, 1);
    }
    while (sca.length() > MIN_LEN && sca[sca.length() - 1] == ' ') {
        sca.erase(sca.length() - 1, 1);
    }
}

int32_t SmsService::GetSmscAddr(int32_t slotId, std::u16string &smscAddress)
{
    if (!TelephonyPermission::CheckPermission(Permission::GET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("Check Permission Failed, No Has Telephony Get State Permisson.");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }

    std::shared_ptr<SmsInterfaceManager> interfaceManager = GetSmsInterfaceManager(slotId);
    if (interfaceManager == nullptr) {
        TELEPHONY_LOGE("SmsService::GetSmscAddr interfaceManager nullptr error");
        return TELEPHONY_ERR_SLOTID_INVALID;
    }
    return interfaceManager->GetSmscAddr(smscAddress);
}

int32_t SmsService::AddSimMessage(
    int32_t slotId, const std::u16string &smsc, const std::u16string &pdu, SimMessageStatus status)
{
    if (!TelephonyPermission::CheckPermission(Permission::RECEIVE_MESSAGES)) {
        TELEPHONY_LOGE("Check Permission Failed, No Has Telephony Receive Messages Permisson.");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    if (!TelephonyPermission::CheckPermission(Permission::SEND_MESSAGES)) {
        TELEPHONY_LOGE("Check Permission Failed, No Has Telephony Send Messages Permisson.");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }

    std::shared_ptr<SmsInterfaceManager> interfaceManager = GetSmsInterfaceManager(slotId);
    if (interfaceManager == nullptr) {
        TELEPHONY_LOGE("SmsService::AddSimMessage interfaceManager nullptr error");
        return TELEPHONY_ERR_SLOTID_INVALID;
    }
    std::string smscData = StringUtils::ToUtf8(smsc);
    std::string pduData = StringUtils::ToUtf8(pdu);
    return interfaceManager->AddSimMessage(smscData, pduData, status);
}

int32_t SmsService::DelSimMessage(int32_t slotId, uint32_t msgIndex)
{
    if (!TelephonyPermission::CheckPermission(Permission::RECEIVE_MESSAGES)) {
        TELEPHONY_LOGE("Check Permission Failed, No Has Telephony Receive Messages Permisson.");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    if (!TelephonyPermission::CheckPermission(Permission::SEND_MESSAGES)) {
        TELEPHONY_LOGE("Check Permission Failed, No Has Telephony Send Messages Permisson.");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }

    std::shared_ptr<SmsInterfaceManager> interfaceManager = GetSmsInterfaceManager(slotId);
    if (interfaceManager == nullptr) {
        TELEPHONY_LOGE("SmsService::DelSimMessage interfaceManager nullptr error.");
        return TELEPHONY_ERR_SLOTID_INVALID;
    }
    return interfaceManager->DelSimMessage(msgIndex);
}

int32_t SmsService::UpdateSimMessage(int32_t slotId, uint32_t msgIndex, SimMessageStatus newStatus,
    const std::u16string &pdu, const std::u16string &smsc)
{
    if (!TelephonyPermission::CheckPermission(Permission::RECEIVE_MESSAGES)) {
        TELEPHONY_LOGE("Check Permission Failed, No Has Telephony Receive Messages Permisson.");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    if (!TelephonyPermission::CheckPermission(Permission::SEND_MESSAGES)) {
        TELEPHONY_LOGE("Check Permission Failed, No Has Telephony Send Messages Permisson.");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }

    std::shared_ptr<SmsInterfaceManager> interfaceManager = GetSmsInterfaceManager(slotId);
    if (interfaceManager == nullptr) {
        TELEPHONY_LOGE("SmsService::UpdateSimMessage interfaceManager nullptr error.");
        return TELEPHONY_ERR_SLOTID_INVALID;
    }
    std::string pduData = StringUtils::ToUtf8(pdu);
    std::string smscData = StringUtils::ToUtf8(smsc);
    return interfaceManager->UpdateSimMessage(msgIndex, newStatus, pduData, smscData);
}

int32_t SmsService::GetAllSimMessages(int32_t slotId, std::vector<ShortMessage> &message)
{
    if (!TelephonyPermission::CheckPermission(Permission::RECEIVE_MESSAGES)) {
        TELEPHONY_LOGE("Check Permission Failed, No Has Telephony Receive Messages Permisson.");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }

    std::shared_ptr<SmsInterfaceManager> interfaceManager = GetSmsInterfaceManager(slotId);
    if (interfaceManager == nullptr) {
        TELEPHONY_LOGE("SmsService::GetAllSimMessages interfaceManager nullptr error.");
        return TELEPHONY_ERR_SLOTID_INVALID;
    }
    return interfaceManager->GetAllSimMessages(message);
}

int32_t SmsService::SetCBConfig(int32_t slotId, bool enable, uint32_t fromMsgId, uint32_t toMsgId, uint8_t netType)
{
    if (!TelephonyPermission::CheckPermission(Permission::RECEIVE_MESSAGES)) {
        TELEPHONY_LOGE("Check Permission Failed, No Has Telephony Receive Messages Permisson.");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }

    std::shared_ptr<SmsInterfaceManager> interfaceManager = GetSmsInterfaceManager(slotId);
    if (interfaceManager == nullptr) {
        TELEPHONY_LOGE("SmsService::SetCBConfig interfaceManager nullptr error.");
        return TELEPHONY_ERR_SLOTID_INVALID;
    }
    return interfaceManager->SetCBConfig(enable, fromMsgId, toMsgId, netType);
}

bool SmsService::SetImsSmsConfig(int32_t slotId, int32_t enable)
{
    if (!TelephonyPermission::CheckPermission(Permission::SET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("Check permission failed.");
        return false;
    }
    std::shared_ptr<SmsInterfaceManager> interfaceManager = GetSmsInterfaceManager(slotId);
    if (interfaceManager == nullptr) {
        TELEPHONY_LOGE("SmsService::SetImsSmsConfig interfaceManager nullptr error.");
        return false;
    }
    return interfaceManager->SetImsSmsConfig(slotId, enable);
}

int32_t SmsService::SetDefaultSmsSlotId(int32_t slotId)
{
    if (!TelephonyPermission::CheckPermission(Permission::SET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("Check Permission Failed, No Has Telephony Set State Permisson.");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }

    std::shared_ptr<SmsInterfaceManager> interfaceManager = GetSmsInterfaceManager();
    if (interfaceManager == nullptr) {
        TELEPHONY_LOGE("SmsService::SetDefaultSmsSlotId interfaceManager nullptr error.");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return interfaceManager->SetDefaultSmsSlotId(slotId);
}

int32_t SmsService::GetDefaultSmsSlotId()
{
    std::shared_ptr<SmsInterfaceManager> interfaceManager = GetSmsInterfaceManager();
    if (interfaceManager == nullptr) {
        TELEPHONY_LOGE("SmsService::GetDefaultSmsSlotId interfaceManager nullptr error.");
        return TELEPHONY_ERROR;
    }
    return interfaceManager->GetDefaultSmsSlotId();
}

int32_t SmsService::SplitMessage(const std::u16string &message, std::vector<std::u16string> &splitMessage)
{
    if (!TelephonyPermission::CheckPermission(Permission::SEND_MESSAGES)) {
        TELEPHONY_LOGE("Check Permission Failed, No Has Telephony Get State Permisson.");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }

    if (message.empty()) {
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }
    int32_t slotId = GetDefaultSmsSlotId();
    std::shared_ptr<SmsInterfaceManager> interfaceManager = GetSmsInterfaceManager(slotId);
    if (interfaceManager == nullptr) {
        TELEPHONY_LOGE("SmsService::SplitMessage interfaceManager nullptr error.");
        return TELEPHONY_ERR_SLOTID_INVALID;
    }
    std::string messageData = StringUtils::ToUtf8(message);
    return interfaceManager->SplitMessage(messageData, splitMessage);
}

int32_t SmsService::GetSmsSegmentsInfo(
    int32_t slotId, const std::u16string &message, bool force7BitCode, ISmsServiceInterface::SmsSegmentsInfo &info)
{
    if (message.empty()) {
        return TELEPHONY_ERR_ARGUMENT_NULL;
    }
    std::shared_ptr<SmsInterfaceManager> interfaceManager = GetSmsInterfaceManager(slotId);
    if (interfaceManager == nullptr) {
        TELEPHONY_LOGE("GetSmsSegmentsInfo interfaceManager is nullptr error.");
        return TELEPHONY_ERR_SLOTID_INVALID;
    }
    std::string messageData = StringUtils::ToUtf8(message);
    LengthInfo result;
    int32_t ret = interfaceManager->GetSmsSegmentsInfo(messageData, force7BitCode, result);
    if (ret != TELEPHONY_ERR_SUCCESS) {
        return ret;
    }
    info.msgSegCount = static_cast<int32_t>(result.msgSegCount);
    info.msgEncodingCount = static_cast<int32_t>(result.msgEncodeCount);
    info.msgRemainCount = static_cast<int32_t>(result.msgRemainCount);
    SmsSegmentsInfo::SmsSegmentCodeScheme dcs = static_cast<SmsSegmentsInfo::SmsSegmentCodeScheme>(result.dcs);
    switch (dcs) {
        case SmsSegmentsInfo::SmsSegmentCodeScheme::SMS_ENCODING_7BIT:
        case SmsSegmentsInfo::SmsSegmentCodeScheme::SMS_ENCODING_8BIT:
        case SmsSegmentsInfo::SmsSegmentCodeScheme::SMS_ENCODING_16BIT:
            info.msgCodeScheme = dcs;
            break;
        default:
            info.msgCodeScheme = SmsSegmentsInfo::SmsSegmentCodeScheme::SMS_ENCODING_UNKNOWN;
            break;
    }
    return TELEPHONY_ERR_SUCCESS;
}

int32_t SmsService::GetServiceRunningState()
{
    return static_cast<int32_t>(state_);
}

int64_t SmsService::GetEndTime()
{
    return endTime_;
}

bool SmsService::ValidDestinationAddress(std::string desAddr)
{
    // Allow address start with '+' and number, Address length range 3 to 20
    std::regex regexMode("^([0-9_+]{1})([0-9]{2,19})$");
    return std::regex_match(desAddr, regexMode);
}

int64_t SmsService::GetSpendTime()
{
    spendTime_ = endTime_ - bindTime_;
    return spendTime_;
}

int32_t SmsService::CreateMessage(std::string pdu, std::string specification, ShortMessage &message)
{
    std::shared_ptr<SmsBaseMessage> baseMessage;
    if (specification == "3gpp") {
        baseMessage = GsmSmsMessage::CreateMessage(pdu);
    } else if (specification == "3gpp2") {
        baseMessage = CdmaSmsMessage::CreateMessage(pdu);
    }

    if (baseMessage == nullptr) {
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    message.visibleMessageBody_ = StringUtils::ToUtf16(baseMessage->GetVisibleMessageBody());
    message.visibleRawAddress_ = StringUtils::ToUtf16(baseMessage->GetVisibleOriginatingAddress());
    message.messageClass_ = static_cast<ShortMessage::SmsMessageClass>(baseMessage->GetMessageClass());
    message.scAddress_ = StringUtils::ToUtf16(baseMessage->GetSmscAddr());
    message.scTimestamp_ = baseMessage->GetScTimestamp();
    message.isReplaceMessage_ = baseMessage->IsReplaceMessage();
    message.status_ = baseMessage->GetStatus();
    message.isSmsStatusReportMessage_ = baseMessage->IsSmsStatusReportMessage();
    message.hasReplyPath_ = baseMessage->HasReplyPath();
    message.protocolId_ = baseMessage->GetProtocolId();
    message.pdu_ = baseMessage->GetRawPdu();
    return TELEPHONY_ERR_SUCCESS;
}

bool SmsService::GetBase64Encode(std::string src, std::string &dest)
{
    MsgTextConvert *textCvt = MsgTextConvert::Instance();
    if (textCvt == nullptr) {
        return false;
    }

    textCvt->Base64Encode(src, dest);
    return true;
}

bool SmsService::GetBase64Decode(std::string src, std::string &dest)
{
    MsgTextConvert *textCvt = MsgTextConvert::Instance();
    if (textCvt == nullptr) {
        return false;
    }

    textCvt->Base64Decode(src, dest);
    return true;
}

bool SmsService::GetEncodeStringFunc(
    std::string &encodeString, uint32_t charset, uint32_t valLength, std::string strEncodeString)
{
    MsgTextConvert *textCvt = MsgTextConvert::Instance();
    if (textCvt == nullptr) {
        return false;
    }

    textCvt->GetEncodeString(encodeString, charset, valLength, strEncodeString);
    return true;
}
} // namespace Telephony
} // namespace OHOS