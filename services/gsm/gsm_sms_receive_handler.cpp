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

#include "gsm_sms_receive_handler.h"

#include "gsm_sms_message.h"
#include "sms_base_message.h"
#include "sms_receive_indexer.h"
#include "string_utils.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
GsmSmsReceiveHandler::GsmSmsReceiveHandler(const std::shared_ptr<AppExecFwk::EventRunner> &runner, int32_t slotId)
    : SmsReceiveHandler(runner, slotId)
{}

GsmSmsReceiveHandler::~GsmSmsReceiveHandler()
{
    UnRegisterHandler();
}

void GsmSmsReceiveHandler::Init()
{
    if (!RegisterHandler()) {
        TELEPHONY_LOGI("GsmSmsSender::Init Register RADIO_SMS_STATUS fail.");
    }
    smsCbRunner_ = AppExecFwk::EventRunner::Create("GsmSmsCbHandler" + std::to_string(slotId_));
    if (smsCbRunner_ == nullptr) {
        TELEPHONY_LOGE("failed to create GsmSmsCbHandler");
        return;
    }
    smsCbHandler_ = std::make_shared<GsmSmsCbHandler>(smsCbRunner_, slotId_);
    if (smsCbHandler_ == nullptr) {
        TELEPHONY_LOGE("failed to create GsmSmsCbHandler");
        return;
    }
    smsCbHandler_->Init();
    smsCbRunner_->Run();
    TELEPHONY_LOGI("smsCbHandler_->Run().");
}

bool GsmSmsReceiveHandler::RegisterHandler()
{
    bool ret = false;
    std::shared_ptr<Core> core = GetCore();
    if (core != nullptr) {
        TELEPHONY_LOGI("GsmSmsReceiveHandler::RegisteHandler Register RADIO_GSM_SMS ok.");
        core->RegisterCoreNotify(shared_from_this(), ObserverHandler::RADIO_GSM_SMS, nullptr);
        ret = true;
    }
    return ret;
}

void GsmSmsReceiveHandler::UnRegisterHandler()
{
    std::shared_ptr<Core> core = GetCore();
    if (core != nullptr) {
        TELEPHONY_LOGI("SmsReceiveHandler::UnRegisterHandler::slotId= %{public}d", slotId_);
        core->UnRegisterCoreNotify(shared_from_this(), ObserverHandler::RADIO_GSM_SMS);
    }
    if (smsCbHandler_ != nullptr) {
        smsCbRunner_->Stop();
    }
}

int32_t GsmSmsReceiveHandler::HandleSmsByType(const std::shared_ptr<SmsBaseMessage> &smsBaseMessage)
{
    TELEPHONY_LOGI("GsmSmsReceiveHandler:: HandleSmsByType");
    if (smsBaseMessage == nullptr) {
        TELEPHONY_LOGE("BaseMessage is null.");
        return AckIncomeCause::SMS_ACK_UNKNOWN_ERROR;
    }
    GsmSmsMessage *message = (GsmSmsMessage *)smsBaseMessage.get();
    if (message->IsSpecialMessage()) {
        TELEPHONY_LOGI("GsmSmsReceiveHandler:: IsSpecialMessage");
        return AckIncomeCause::SMS_ACK_RESULT_OK;
    }
    std::shared_ptr<SmsReceiveIndexer> indexer;
    if (!message->IsConcatMsg()) {
        indexer = std::make_shared<SmsReceiveIndexer>(message->GetRawPdu(), message->GetScTimestamp(),
            message->GetDestPort(), !message->GetGsm(), false, message->GetOriginatingAddress(),
            message->GetVisibleOriginatingAddress(), message->GetVisibleMessageBody());
    } else {
        std::shared_ptr<SmsConcat> smsConcat = message->GetConcatMsg();
        if (smsConcat == nullptr) {
            TELEPHONY_LOGE("Concat is null.");
            return AckIncomeCause::SMS_ACK_UNKNOWN_ERROR;
        }
        indexer = std::make_shared<SmsReceiveIndexer>(message->GetRawPdu(), message->GetScTimestamp(),
            message->GetDestPort(), !message->GetGsm(), message->GetOriginatingAddress(),
            message->GetVisibleOriginatingAddress(), smsConcat->msgRef, smsConcat->seqNum, smsConcat->totalSeg,
            false, message->GetVisibleMessageBody());
    }
    if (indexer == nullptr) {
        TELEPHONY_LOGE("indexer is null.");
        return AckIncomeCause::SMS_ACK_UNKNOWN_ERROR;
    }

    if (indexer->GetIsText() && IsRepeatedMessagePart(indexer)) {
        TELEPHONY_LOGE("Ack repeated error.");
        return AckIncomeCause::SMS_ACK_REPEATED_ERROR;
    }

    bool result = AddMessageToDb(indexer);
    if (result == false) {
        TELEPHONY_LOGE("HandleSmsByType insert fail.");
        return AckIncomeCause::SMS_ACK_UNKNOWN_ERROR;
    }
    CombineMessagePart(indexer);
    return AckIncomeCause::SMS_ACK_RESULT_OK;
}

void GsmSmsReceiveHandler::ReplySmsToSmsc(int result, const std::shared_ptr<SmsBaseMessage> &response)
{
    std::shared_ptr<Core> core = GetCore();
    if (core != nullptr) {
        auto reply = AppExecFwk::InnerEvent::Get(SMS_EVENT_NEW_SMS_REPLY, response);
        reply->SetOwner(shared_from_this());
        TELEPHONY_LOGI("GsmSmsReceiveHandler::ReplySmsToSmsc ackResult %{public}d", result);
        core->SendSmsAck(result == AckIncomeCause::SMS_ACK_RESULT_OK, result, reply);
    }
}

std::shared_ptr<SmsBaseMessage> GsmSmsReceiveHandler::TransformMessageInfo(const std::shared_ptr<SmsMessageInfo> &info)
{
    std::shared_ptr<SmsBaseMessage> baseMessage = nullptr;
    if (info == nullptr) {
        TELEPHONY_LOGE("MessageInfo is null.");
        return baseMessage;
    }
    std::string pdu = StringUtils::StringToHex(info->pdu);
    baseMessage = GsmSmsMessage::CreateMessage(pdu);
    return baseMessage;
}
} // namespace Telephony
} // namespace OHOS