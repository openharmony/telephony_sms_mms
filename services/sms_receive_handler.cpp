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

#include "sms_receive_handler.h"

#include "common_event.h"
#include "common_event_manager.h"
#include "common_event_support.h"
#include "want.h"

#include "string_utils.h"
#include "gsm_sms_message.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
using namespace std;
using namespace EventFwk;
SmsReceiveHandler::SmsReceiveHandler(const std::shared_ptr<AppExecFwk::EventRunner> &runner, int32_t slotId)
    : AppExecFwk::EventHandler(runner), slotId_(slotId)
{}

SmsReceiveHandler::~SmsReceiveHandler() {}

void SmsReceiveHandler::ProcessEvent(const AppExecFwk::InnerEvent::Pointer &event)
{
    if (event == nullptr) {
        TELEPHONY_LOGE("SmsReceiveHandler::ProcessEvent event == nullptr");
        return;
    }

    int eventId = 0;
    eventId = event->GetInnerEventId();
    TELEPHONY_LOGI("SmsReceiveHandler::ProcessEvent eventId = %{public}d", eventId);
    switch (eventId) {
        case ObserverHandler::RADIO_GSM_SMS: {
            std::shared_ptr<SmsBaseMessage> message = nullptr;
            message = TransformMessageInfo(event->GetSharedObject<SmsMessageInfo>());
            if (message != nullptr) {
                TELEPHONY_LOGI("[raw pdu size] = %{public}zu", StringUtils::StringToHex(message->GetRawPdu()).size());
            }
            HandleReceivedSms(message);
            break;
        }
        default:
            TELEPHONY_LOGE("SmsReceiveHandler::ProcessEvent Unknown eventId %{public}d", eventId);
            break;
    }
}

void SmsReceiveHandler::HandleReceivedSms(const std::shared_ptr<SmsBaseMessage> &smsBaseMessage)
{
    if (smsBaseMessage == nullptr) {
        TELEPHONY_LOGE("smsBaseMessage is nullptr");
        return;
    }
    ReplySmsToSmsc(HandleSmsByType(smsBaseMessage), nullptr);
}

void SmsReceiveHandler::CombineMessagePart(const std::shared_ptr<SmsReceiveIndexer> &indexer)
{
    std::shared_ptr<vector<string>> pdus = make_shared<vector<string>>();
    if ((indexer == nullptr) || (pdus == nullptr)) {
        TELEPHONY_LOGE("indexer or pdus is nullptr");
        return;
    }
    std::string messagBody;
    if (indexer->IsSingleMsg()) {
        string pdu = StringUtils::StringToHex(indexer->GetPdu());
        messagBody.append(indexer->GetVisibleMessageBody());
        pdus->push_back(pdu);
    } else {
        pdus->assign(MAX_SEGMENT_NUM, "");
        int msgSeg = indexer->GetMsgCount();
        int8_t count = 0;
        int8_t notNullPart = msgSeg;

        std::vector<SmsReceiveIndexer> dbIndexers;
        NativeRdb::DataAbilityPredicates predicates;
        predicates.EqualTo(SmsMmsData::SENDER_NUMBER, indexer->GetOriginatingAddress())->And()
                    ->EqualTo(SmsMmsData::SMS_SUBSECTION_ID, std::to_string(indexer->GetMsgRefId()))->And()
                    ->EqualTo(SmsMmsData::SIZE, std::to_string(indexer->GetMsgCount()));
        DelayedSingleton<SmsDataBaseHelper>::GetInstance()->Query(predicates, dbIndexers);

        for (const auto &v : dbIndexers) {
            ++ count;
            string pdu = StringUtils::StringToHex(v.GetPdu());
            if ((v.GetMsgSeqId() - PDU_POS_OFFSET >= MAX_SEGMENT_NUM) ||
                (v.GetMsgSeqId() - PDU_POS_OFFSET < 0)) {
                DeleteMessageFromDb(indexer);
                return;
            }
            pdus->at(v.GetMsgSeqId() - PDU_POS_OFFSET) = pdu;
            if (v.GetPdu().size() == 0) {
                --notNullPart;
            }
        }
        if ((count != msgSeg) || (pdus->empty()) || (notNullPart != msgSeg)) {
            return;
        }
    }
    if (indexer->GetIsWapPushMsg()) {
        DeleteMessageFromDb(indexer);
        return;
    }
    DeleteMessageFromDb(indexer);
    indexer->SetVisibleMessageBody(messagBody);
    SendBroadcast(indexer, pdus);
}

bool SmsReceiveHandler::AddMessageToDb(const std::shared_ptr<SmsReceiveIndexer> &smsIndexer)
{
    const uint8_t gsm = 1;
    const uint8_t cdma = 2;
    bool result = false;
    if (smsIndexer == nullptr) {
        TELEPHONY_LOGE("smsIndexer is nullptr");
        return result;
    }
    NativeRdb::ValuesBucket bucket;
    bucket.PutString(SmsMmsData::RECEIVER_NUMBER, smsIndexer->GetOriginatingAddress());
    bucket.PutString(SmsMmsData::SENDER_NUMBER, smsIndexer->GetOriginatingAddress());
    bucket.PutString(SmsMmsData::START_TIME, std::to_string(smsIndexer->GetTimestamp()));
    bucket.PutString(SmsMmsData::END_TIME, std::to_string(smsIndexer->GetTimestamp()));
    bucket.PutString(SmsMmsData::REW_PUD, StringUtils::StringToHex(smsIndexer->GetPdu()));

    bucket.PutInt(SmsMmsData::FORMAT, smsIndexer->GetIsCdma() ? cdma : gsm);
    bucket.PutInt(SmsMmsData::DEST_PORT, smsIndexer->GetDestPort());
    bucket.PutInt(SmsMmsData::SMS_SUBSECTION_ID, smsIndexer->GetMsgRefId());
    bucket.PutInt(SmsMmsData::SIZE, smsIndexer->GetMsgCount());
    bucket.PutInt(SmsMmsData::SUBSECTION_INDEX, smsIndexer->GetMsgSeqId());
    return DelayedSingleton<SmsDataBaseHelper>::GetInstance()->Insert(bucket);
}

void SmsReceiveHandler::DeleteMessageFromDb(const std::shared_ptr<SmsReceiveIndexer> &smsIndexer)
{
    if (smsIndexer == nullptr) {
        TELEPHONY_LOGE("smsIndexer is nullptr");
        return;
    }

    NativeRdb::DataAbilityPredicates predicates;
    predicates.EqualTo(SmsMmsData::SMS_SUBSECTION_ID, std::to_string(smsIndexer->GetMsgRefId()));
    DelayedSingleton<SmsDataBaseHelper>::GetInstance()->Delete(predicates);
}

bool SmsReceiveHandler::IsRepeatedMessagePart(const shared_ptr<SmsReceiveIndexer> &smsIndexer)
{
    if (smsIndexer != nullptr) {
        std::vector<SmsReceiveIndexer> dbIndexers;
        NativeRdb::DataAbilityPredicates predicates;
        predicates.EqualTo(SmsMmsData::SENDER_NUMBER, smsIndexer->GetOriginatingAddress())->And()
                    ->EqualTo(SmsMmsData::SMS_SUBSECTION_ID, std::to_string(smsIndexer->GetMsgRefId()))->And()
                    ->EqualTo(SmsMmsData::SIZE, std::to_string(smsIndexer->GetMsgCount()));
        DelayedSingleton<SmsDataBaseHelper>::GetInstance()->Query(predicates, dbIndexers);

        for (const auto &it : dbIndexers) {
            if (it.GetMsgSeqId() == smsIndexer->GetMsgSeqId()) {
                return true;
            }
        }
    }
    return false;
}

void SmsReceiveHandler::SendBroadcast(
    const std::shared_ptr<SmsReceiveIndexer> &indexer, const shared_ptr<vector<string>> &pdus)
{
    if (pdus == nullptr || indexer == nullptr) {
        TELEPHONY_LOGE("pdus is nullptr");
        return;
    }
    std::vector<std::string> newPdus;
    for (const auto &it : *pdus) {
        if (!it.empty()) {
            newPdus.emplace_back(it);
        }
    }
    Want want;
    want.SetAction(CommonEventSupport::COMMON_EVENT_SMS_RECEIVE_COMPLETED);
    want.SetParam("pdus", newPdus);
    want.SetParam("isCdma", indexer->GetIsCdma());
    CommonEventData data;
    data.SetWant(want);
    if (indexer->GetIsText()) {
        data.SetData("TEXT_SMS_RECEIVE");
        data.SetCode(TEXT_MSG_RECEIVE_CODE);
    } else {
        data.SetData("DATA_SMS_RECEIVE");
        data.SetCode(DATA_MSG_RECEIVE_CODE);
        want.SetParam("port", static_cast<short>(indexer->GetDestPort()));
    }
    CommonEventPublishInfo publishInfo;
    publishInfo.SetOrdered(true);
    bool publishResult = CommonEventManager::PublishCommonEvent(data, publishInfo, nullptr);
    if (!publishResult) {
        TELEPHONY_LOGE("SendBroadcast PublishBroadcastEvent result fail");
    }
}

std::shared_ptr<Core> SmsReceiveHandler::GetCore() const
{
    std::shared_ptr<Core> core = CoreManager::GetInstance().getCore(slotId_);
    if (core != nullptr && core->IsInitCore()) {
        return core;
    }
    return nullptr;
}
} // namespace Telephony
} // namespace OHOS