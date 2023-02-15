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
#include "gsm_sms_message.h"
#include "radio_event.h"
#include "singleton.h"
#include "sms_hisysevent.h"
#include "string_utils.h"
#include "telephony_log_wrapper.h"
#include "telephony_permission.h"
#include "want.h"

namespace OHOS {
namespace Telephony {
using namespace std;
using namespace EventFwk;
SmsReceiveHandler::SmsReceiveHandler(const std::shared_ptr<AppExecFwk::EventRunner> &runner, int32_t slotId)
    : AppExecFwk::EventHandler(runner), slotId_(slotId)
{
    smsWapPushHandler_ = std::make_unique<SmsWapPushHandler>(slotId);
    if (smsWapPushHandler_ == nullptr) {
        TELEPHONY_LOGE("make sms wapPush Hander error.");
    }
}

SmsReceiveHandler::~SmsReceiveHandler() {}

void SmsReceiveHandler::ProcessEvent(const AppExecFwk::InnerEvent::Pointer &event)
{
    if (event == nullptr) {
        TELEPHONY_LOGE("SmsReceiveHandler::ProcessEvent event == nullptr");
        return;
    }

    uint32_t eventId = 0;
    eventId = event->GetInnerEventId();
    TELEPHONY_LOGI("SmsReceiveHandler::ProcessEvent eventId = %{public}d", eventId);
    switch (eventId) {
        case RadioEvent::RADIO_GSM_SMS:
        case RadioEvent::RADIO_CDMA_SMS: {
            std::shared_ptr<SmsBaseMessage> message = nullptr;
            message = TransformMessageInfo(event->GetSharedObject<SmsMessageInfo>());
            if (message != nullptr) {
                TELEPHONY_LOGI("[raw pdu] =%{private}s", StringUtils::StringToHex(message->GetRawPdu()).c_str());
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
    std::string userDataRaw;
    if (indexer->IsSingleMsg()) {
        string pdu = StringUtils::StringToHex(indexer->GetPdu());
        messagBody.append(indexer->GetVisibleMessageBody());
        userDataRaw.append(indexer->GetRawUserData());
        pdus->push_back(pdu);
    } else {
        pdus->assign(MAX_SEGMENT_NUM, "");
        int msgSeg = static_cast<int>(indexer->GetMsgCount());
        int8_t count = 0;
        int8_t notNullPart = msgSeg;

        std::vector<SmsReceiveIndexer> dbIndexers;
        NativeRdb::DataAbilityPredicates predicates;
        predicates.EqualTo(SmsMmsData::SENDER_NUMBER, indexer->GetOriginatingAddress())
            ->And()
            ->EqualTo(SmsMmsData::SMS_SUBSECTION_ID, std::to_string(indexer->GetMsgRefId()))
            ->And()
            ->EqualTo(SmsMmsData::SIZE, std::to_string(indexer->GetMsgCount()));
        DelayedSingleton<SmsPersistHelper>::GetInstance()->Query(predicates, dbIndexers);

        for (const auto &v : dbIndexers) {
            ++count;
            string pdu = StringUtils::StringToHex(v.GetPdu());
            if ((v.GetMsgSeqId() - PDU_POS_OFFSET >= MAX_SEGMENT_NUM) || (v.GetMsgSeqId() - PDU_POS_OFFSET < 0)) {
                DeleteMessageFormDb(indexer);
                return;
            }
            pdus->at(v.GetMsgSeqId() - PDU_POS_OFFSET) = pdu;
            if (v.GetPdu().size() == 0) {
                --notNullPart;
            }
            std::shared_ptr<SmsBaseMessage> baseMessage = GsmSmsMessage::CreateMessage(pdu);
            if (baseMessage != nullptr) {
                userDataRaw.append(baseMessage->GetRawUserData());
                messagBody.append(baseMessage->GetVisibleMessageBody());
            }
        }
        if ((count != msgSeg) || (pdus->empty()) || (notNullPart != msgSeg)) {
            return;
        }
    }

    indexer->SetVisibleMessageBody(messagBody);
    indexer->SetRawUserData(userDataRaw);
    DeleteMessageFormDb(indexer);
    if (CheckBlockPhone(indexer)) {
        TELEPHONY_LOGI("indexer display address is block");
        return;
    }
    if (indexer->GetIsWapPushMsg()) {
        if (smsWapPushHandler_ != nullptr) {
            if (!smsWapPushHandler_->DecodeWapPushPdu(userDataRaw)) {
                SmsHiSysEvent::WriteSmsReceiveFaultEvent(slotId_, SmsMmsMessageType::WAP_PUSH,
                    SmsMmsErrorCode::SMS_ERROR_PDU_DECODE_FAIL, "Wap push decode wap push fail");
            }
        }
        return;
    }
    SendBroadcast(indexer, pdus);
}

void SmsReceiveHandler::DeleteMessageFormDb(const std::shared_ptr<SmsReceiveIndexer> &smsIndexer)
{
    if (smsIndexer == nullptr) {
        TELEPHONY_LOGE("smsIndexer is nullptr");
        return;
    }

    NativeRdb::DataAbilityPredicates predicates;
    predicates.EqualTo(SmsMmsData::SMS_SUBSECTION_ID, std::to_string(smsIndexer->GetMsgRefId()));
    DelayedSingleton<SmsPersistHelper>::GetInstance()->Delete(predicates);
}

bool SmsReceiveHandler::IsRepeatedMessagePart(const shared_ptr<SmsReceiveIndexer> &smsIndexer)
{
    if (smsIndexer != nullptr) {
        std::vector<SmsReceiveIndexer> dbIndexers;
        NativeRdb::DataAbilityPredicates predicates;
        predicates.EqualTo(SmsMmsData::SENDER_NUMBER, smsIndexer->GetOriginatingAddress())
            ->And()
            ->EqualTo(SmsMmsData::SMS_SUBSECTION_ID, std::to_string(smsIndexer->GetMsgRefId()))
            ->And()
            ->EqualTo(SmsMmsData::SIZE, std::to_string(smsIndexer->GetMsgCount()));
        DelayedSingleton<SmsPersistHelper>::GetInstance()->Query(predicates, dbIndexers);

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
    want.SetParam("slotId", static_cast<int>(slotId_));
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
    std::vector<std::string> smsPermissions;
    smsPermissions.emplace_back(Permission::RECEIVE_MESSAGES);
    publishInfo.SetSubscriberPermissions(smsPermissions);
    bool publishResult = CommonEventManager::PublishCommonEvent(data, publishInfo, nullptr);
    if (!publishResult) {
        TELEPHONY_LOGE("SendBroadcast PublishBroadcastEvent result fail");
        SmsHiSysEvent::WriteSmsReceiveFaultEvent(slotId_, SmsMmsMessageType::SMS_SHORT_MESSAGE,
            SmsMmsErrorCode::SMS_ERROR_PUBLISH_COMMON_EVENT_FAIL, "publish short message broadcast event fail");
    }
    DelayedSingleton<SmsHiSysEvent>::GetInstance()->SetSmsBroadcastStartTime();
}

bool SmsReceiveHandler::AddMsgToDB(const std::shared_ptr<SmsReceiveIndexer> &indexer)
{
    if (indexer == nullptr) {
        TELEPHONY_LOGE("indexer is nullptr.");
        return false;
    }
    const uint8_t gsm = 1;
    const uint8_t cdma = 2;
    NativeRdb::ValuesBucket bucket;
    bucket.PutString(SmsMmsData::SLOT_ID, std::to_string(slotId_));
    bucket.PutString(SmsMmsData::RECEIVER_NUMBER, indexer->GetOriginatingAddress());
    bucket.PutString(SmsMmsData::SENDER_NUMBER, indexer->GetOriginatingAddress());
    bucket.PutString(SmsMmsData::START_TIME, std::to_string(indexer->GetTimestamp()));
    bucket.PutString(SmsMmsData::END_TIME, std::to_string(indexer->GetTimestamp()));
    bucket.PutString(SmsMmsData::RAW_PUD, StringUtils::StringToHex(indexer->GetPdu()));

    bucket.PutInt(SmsMmsData::FORMAT, indexer->GetIsCdma() ? cdma : gsm);
    bucket.PutInt(SmsMmsData::DEST_PORT, indexer->GetDestPort());
    bucket.PutInt(SmsMmsData::SMS_SUBSECTION_ID, indexer->GetMsgRefId());
    bucket.PutInt(SmsMmsData::SIZE, indexer->GetMsgCount());
    bucket.PutInt(SmsMmsData::SUBSECTION_INDEX, indexer->GetMsgSeqId());
    bool ret = DelayedSingleton<SmsPersistHelper>::GetInstance()->Insert(bucket);
    if (!ret) {
        SmsHiSysEvent::WriteSmsReceiveFaultEvent(slotId_, SmsMmsMessageType::SMS_SHORT_MESSAGE,
            SmsMmsErrorCode::SMS_ERROR_ADD_TO_DATABASE_FAIL, "add msg to database error");
    }
    return ret;
}

bool SmsReceiveHandler::CheckBlockPhone(const std::shared_ptr<SmsReceiveIndexer> &indexer)
{
    if (indexer == nullptr) {
        TELEPHONY_LOGE("CheckBlockPhone sms indexer nullptr error.");
        return false;
    }
    TELEPHONY_LOGD("indexer originating =%{private}s", indexer->GetOriginatingAddress().c_str());
    return DelayedSingleton<SmsPersistHelper>::GetInstance()->QueryBlockPhoneNumber(
        indexer->GetOriginatingAddress());
}

bool SmsReceiveHandler::CheckSmsCapable()
{
    auto helperPtr = DelayedSingleton<SmsPersistHelper>::GetInstance();
    if (helperPtr == nullptr) {
        return true;
    }
    return helperPtr->QueryParamBoolean(SmsPersistHelper::SMS_CAPABLE_PARAM_KEY, true);
}
} // namespace Telephony
} // namespace OHOS