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

#include "cdma_sms_receive_handler.h"

#include "cdma_sms_message.h"
#include "cdma_sms_sender.h"
#include "cdma_sms_types.h"
#include "string_utils.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
CdmaSmsReceiveHandler::CdmaSmsReceiveHandler(const std::shared_ptr<AppExecFwk::EventRunner> &runner, int32_t slotId)
    : SmsReceiveHandler(runner, slotId)
{
    TELEPHONY_LOGI("%{public}d", slotId_);
}

int32_t CdmaSmsReceiveHandler::HandleSmsByType(const std::shared_ptr<SmsBaseMessage> &smsBaseMessage)
{
    if (smsBaseMessage == nullptr) {
        TELEPHONY_LOGE("SmsBaseMessage is null!");
        return AckIncomeCause::SMS_ACK_UNKNOWN_ERROR;
    }
    CdmaSmsMessage *message = static_cast<CdmaSmsMessage *>(smsBaseMessage.get());
    if (message == nullptr || message->IsBroadcastMsg()) {
        TELEPHONY_LOGE("SmsBaseMessage is null or TransMsgType is 1");
        return AckIncomeCause::SMS_ACK_RESULT_OK;
    }
    int service = message->GetTransTeleService();
    if ((SMS_TRANS_TELESVC_WEMT == service || SMS_TRANS_TELESVC_CMT_95 == service)
        && message->IsStatusReport()) {
        if (HandleStatusReport(smsBaseMessage)) {
            return AckIncomeCause::SMS_ACK_RESULT_OK;
        } else {
            return AckIncomeCause::SMS_ACK_UNKNOWN_ERROR;
        }
    } else if (SMS_TRANS_TELESVC_WAP == service) {
        return AckIncomeCause::SMS_ACK_RESULT_OK;
    } else {
        return AckIncomeCause::SMS_ACK_RESULT_OK;
    }
    std::shared_ptr<SmsReceiveIndexer> indexer = CreateIndexer(smsBaseMessage);
    // add messages to the database
    if (indexer == nullptr || (indexer->GetIsText() && IsRepeatedMessagePart(indexer))) {
        TELEPHONY_LOGE("Make SmsReceiveIndexer or ReceiveIndexer err.");
        return AckIncomeCause::SMS_ACK_UNKNOWN_ERROR;
    }
    if (!AddMsgToDB(indexer)) {
        TELEPHONY_LOGE("Add msg to database fail.");
        return AckIncomeCause::SMS_ACK_UNKNOWN_ERROR;
    }
    CombineMessagePart(indexer);
    return AckIncomeCause::SMS_ACK_RESULT_OK;
}

void CdmaSmsReceiveHandler::ReplySmsToSmsc(int result, const std::shared_ptr<SmsBaseMessage> &response) {}

void CdmaSmsReceiveHandler::SetCdmaSender(const std::weak_ptr<SmsSender> &smsSender)
{
    cdmaSmsSender_ = smsSender;
}

std::shared_ptr<SmsBaseMessage> CdmaSmsReceiveHandler::TransformMessageInfo(
    const std::shared_ptr<SmsMessageInfo> &info)
{
    std::shared_ptr<SmsBaseMessage> baseMessage = nullptr;
    if (info == nullptr) {
        TELEPHONY_LOGE("SmsBaseMessage is null!");
        return baseMessage;
    }
    std::string pdu = StringUtils::StringToHex(info->pdu);
    baseMessage = CdmaSmsMessage::CreateMessage(pdu);
    return baseMessage;
}

CdmaSmsReceiveHandler::~CdmaSmsReceiveHandler()
{
    UnRegisterHandler();
}

void CdmaSmsReceiveHandler::Init()
{
    if (!RegisterHandler()) {
        TELEPHONY_LOGI("GsmSmsSender::Init Register RADIO_SMS_STATUS fail.");
    }
}

bool CdmaSmsReceiveHandler::RegisterHandler()
{
    bool ret = false;
    std::shared_ptr<Core> core = GetCore();
    if (core != nullptr) {
        TELEPHONY_LOGI("CdmaSmsReceiveHandler::RegisteHandler Register RADIO_GSM_SMS ok.");
        core->RegisterCoreNotify(shared_from_this(), ObserverHandler::RADIO_GSM_SMS, nullptr);
        ret = true;
    }
    return ret;
}

void CdmaSmsReceiveHandler::UnRegisterHandler()
{
    std::shared_ptr<Core> core = GetCore();
    if (core != nullptr) {
        TELEPHONY_LOGI("CdmaSmsReceiveHandler::UnRegisterHandler::slotId= %{public}d", slotId_);
        core->UnRegisterCoreNotify(shared_from_this(), ObserverHandler::RADIO_GSM_SMS);
    }
}

bool CdmaSmsReceiveHandler::AddMsgToDB(const std::shared_ptr<SmsReceiveIndexer> &indexer)
{
    if (indexer == nullptr) {
        TELEPHONY_LOGE("SmsReceiveIndexer is null!");
        return false;
    }
    std::string key = indexer->GetOriginatingAddress() + std::to_string(indexer->GetMsgRefId()) +
        std::to_string(indexer->GetMsgCount());
    indexer->SetEraseRefId(key);
    auto ret = receiveMap_.emplace(key, indexer);
    if (!ret->second) {
        TELEPHONY_LOGE("Add msg to db fail.");
        return false;
    }
    return true;
}

bool CdmaSmsReceiveHandler::HandleStatusReport(const std::shared_ptr<SmsBaseMessage> &smsBaseMessage)
{
    bool result = false;
    if (smsBaseMessage == nullptr) {
        TELEPHONY_LOGE("SmsBaseMessage is null!");
        return result;
    }
    CdmaSmsMessage *message = static_cast<CdmaSmsMessage *>(smsBaseMessage.get());
    if (!cdmaSmsSender_.expired()) {
        std::shared_ptr<SmsSender> smsSender = cdmaSmsSender_.lock();
        CdmaSmsSender *cdmaSend = static_cast<CdmaSmsSender *>(smsSender.get());
        std::shared_ptr<SmsReceiveIndexer> statusInfo = std::make_shared<SmsReceiveIndexer>();
        if (statusInfo == nullptr || cdmaSend == nullptr) {
            TELEPHONY_LOGE("Make SmsReceiveIndexer err.!");
            return result;
        }
        statusInfo->SetMsgRefId(message->GetMsgRef());
        statusInfo->SetPdu(message->GetRawPdu());
        cdmaSend->ReceiveStatusReport(statusInfo);
    }
    return true;
}

std::shared_ptr<SmsReceiveIndexer> CreateIndexer(const std::shared_ptr<SmsBaseMessage> &smsBaseMessage)
{
    std::shared_ptr<SmsReceiveIndexer> result = nullptr;
    if (smsBaseMessage == nullptr) {
        TELEPHONY_LOGE("SmsBaseMessage is null!");
        return result;
    }
    CdmaSmsMessage *message = static_cast<CdmaSmsMessage *>(smsBaseMessage.get());
    if (!message->IsConcatMsg()) {
        result = std::make_shared<SmsReceiveIndexer>(message->GetRawPdu(), message->GetScTimestamp(),
            message->GetDestPort(), true, false, message->GetOriginatingAddress(),
            message->GetVisibleOriginatingAddress(), message->GetVisibleMessageBody());
    } else {
        std::shared_ptr<SmsConcat> smsConcat = message->GetConcatMsg();
        if (smsConcat == nullptr) {
            TELEPHONY_LOGE("SmsConcat is null!");
            return result;
        }
        result = std::make_shared<SmsReceiveIndexer>(message->GetRawPdu(), message->GetScTimestamp(),
            message->GetDestPort(), true, message->GetOriginatingAddress(), message->GetVisibleOriginatingAddress(),
            smsConcat->msgRef, smsConcat->seqNum, smsConcat->totalSeg, false, message->GetVisibleMessageBody());
    }
    return result;
}
} // namespace Telephony
} // namespace OHOS