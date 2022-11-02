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

#include "core_manager_inner.h"
#include "gsm_sms_message.h"
#include "radio_event.h"
#include "sms_base_message.h"
#include "sms_hisysevent.h"
#include "sms_receive_indexer.h"
#include "string_utils.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
using namespace std;
GsmSmsReceiveHandler::GsmSmsReceiveHandler(const shared_ptr<AppExecFwk::EventRunner> &runner, int32_t slotId)
    : SmsReceiveHandler(runner, slotId)
{}

GsmSmsReceiveHandler::~GsmSmsReceiveHandler()
{
    if (smsCbHandler_ != nullptr) {
        smsCbHandler_->UnRegisterHandler();
    }
}

void GsmSmsReceiveHandler::Init()
{
    if (!RegisterHandler()) {
        TELEPHONY_LOGI("GsmSmsSender::Init Register RADIO_SMS_STATUS fail.");
    }
    smsCbRunner_ = AppExecFwk::EventRunner::Create("GsmSmsCbHandler" + to_string(slotId_));
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
    TELEPHONY_LOGI("GsmSmsReceiveHandler::RegisteHandler Register RADIO_GSM_SMS ok.");
    CoreManagerInner::GetInstance().RegisterCoreNotify(
        slotId_, shared_from_this(), RadioEvent::RADIO_GSM_SMS, nullptr);
    return true;
}

void GsmSmsReceiveHandler::UnRegisterHandler()
{
    TELEPHONY_LOGI("SmsReceiveHandler::UnRegisterHandler::slotId= %{public}d", slotId_);
    CoreManagerInner::GetInstance().UnRegisterCoreNotify(
        slotId_, shared_from_this(), RadioEvent::RADIO_GSM_SMS);
    if (smsCbHandler_ != nullptr) {
        smsCbRunner_->Stop();
    }
}

int32_t GsmSmsReceiveHandler::HandleSmsByType(const shared_ptr<SmsBaseMessage> &smsBaseMessage)
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
    if (!CheckSmsCapable()) {
        TELEPHONY_LOGI("sms receive capable unSupport");
        SmsHiSysEvent::WriteSmsReceiveFaultEvent(slotId_, SmsMmsMessageType::SMS_SHORT_MESSAGE,
            SmsMmsErrorCode::SMS_ERROR_EMPTY_INPUT_PARAMETER, "sms receive capable unsupported");
        return AckIncomeCause::SMS_ACK_PROCESSED;
    }

    shared_ptr<SmsReceiveIndexer> indexer;
    if (!message->IsConcatMsg()) {
        indexer = make_shared<SmsReceiveIndexer>(message->GetRawPdu(), message->GetScTimestamp(),
            message->GetDestPort(), !message->GetGsm(), false, message->GetOriginatingAddress(),
            message->GetVisibleOriginatingAddress(), message->GetVisibleMessageBody());
    } else {
        std::shared_ptr<SmsConcat> smsConcat = message->GetConcatMsg();
        if (smsConcat == nullptr) {
            TELEPHONY_LOGE("Concat is null.");
            return AckIncomeCause::SMS_ACK_UNKNOWN_ERROR;
        }
        indexer = make_shared<SmsReceiveIndexer>(message->GetRawPdu(), message->GetScTimestamp(),
            message->GetDestPort(), !message->GetGsm(), message->GetOriginatingAddress(),
            message->GetVisibleOriginatingAddress(), smsConcat->msgRef, smsConcat->seqNum, smsConcat->totalSeg,
            false, message->GetVisibleMessageBody());
    }
    if (indexer == nullptr) {
        TELEPHONY_LOGE("indexer is null.");
        return AckIncomeCause::SMS_ACK_UNKNOWN_ERROR;
    }
    indexer->SetRawUserData(message->GetRawUserData());
    if (indexer->GetIsText() && IsRepeatedMessagePart(indexer)) {
        TELEPHONY_LOGE("Ack repeated error.");
        SmsHiSysEvent::WriteSmsReceiveFaultEvent(slotId_, SmsMmsMessageType::SMS_SHORT_MESSAGE,
            SmsMmsErrorCode::SMS_ERROR_REPEATED_ERROR, "gsm message repeated error");
        return AckIncomeCause::SMS_ACK_REPEATED_ERROR;
    }
    if (!AddMsgToDB(indexer)) {
        return AckIncomeCause::SMS_ACK_UNKNOWN_ERROR;
    }
    CombineMessagePart(indexer);
    return AckIncomeCause::SMS_ACK_RESULT_OK;
}

void GsmSmsReceiveHandler::ReplySmsToSmsc(int result, const shared_ptr<SmsBaseMessage> &response)
{
    TELEPHONY_LOGI("GsmSmsReceiveHandler::ReplySmsToSmsc ackResult %{public}d", result);
    CoreManagerInner::GetInstance().SendSmsAck(
        slotId_, SMS_EVENT_NEW_SMS_REPLY, result == AckIncomeCause::SMS_ACK_RESULT_OK, result, shared_from_this());
}

shared_ptr<SmsBaseMessage> GsmSmsReceiveHandler::TransformMessageInfo(const shared_ptr<SmsMessageInfo> &info)
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