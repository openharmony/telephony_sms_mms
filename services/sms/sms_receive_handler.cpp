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

#include "gsm_sms_message.h"
#include "radio_event.h"
#include "sms_hisysevent.h"
#include "sms_persist_helper.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
using namespace std;
constexpr static uint16_t PDU_POS_OFFSET = 1;
constexpr static uint8_t SMS_TYPE_GSM = 1;
constexpr static uint8_t SMS_TYPE_CDMA = 2;
static const std::string WAP_SEQ_NUMBER_TAG = "0003";
constexpr static size_t WAP_SEQ_NUMBER_LEN = 10;

SmsReceiveHandler::SmsReceiveHandler(int32_t slotId) : TelEventHandler("SmsReceiveHandler"), slotId_(slotId)
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

void SmsReceiveHandler::HandleReceivedSms(const std::shared_ptr<SmsBaseMessage> smsBaseMessage)
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
    auto reliabilityHandler = std::make_shared<SmsReceiveReliabilityHandler>(slotId_);
    if ((reliabilityHandler == nullptr)) {
        TELEPHONY_LOGE("reliabilityHandler is nullptr");
        return;
    }
    if (indexer->IsSingleMsg()) {
        string pdu = StringUtils::StringToHex(indexer->GetPdu());
        pdus->push_back(pdu);
    } else {
        if (!CombineMultiPageMessage(indexer, pdus, reliabilityHandler)) {
            TELEPHONY_LOGI("The multi-page text didn't all arrive");
            return;
        }
    }

    if (indexer->GetIsWapPushMsg()) {
        if (smsWapPushHandler_ != nullptr) {
            auto rawWapPushUserData = indexer->GetRawWapPushUserData();
            if (!smsWapPushHandler_->DecodeWapPushPdu(indexer, rawWapPushUserData)) {
                SmsHiSysEvent::WriteSmsReceiveFaultEvent(slotId_, SmsMmsMessageType::WAP_PUSH,
                    SmsMmsErrorCode::SMS_ERROR_PDU_DECODE_FAIL, "Wap push decode wap push fail");
            }
        }
        return;
    }
    reliabilityHandler->SendBroadcast(indexer, pdus);
}

bool SmsReceiveHandler::CombineMultiPageMessage(const std::shared_ptr<SmsReceiveIndexer> &indexer,
    std::shared_ptr<std::vector<std::string>> pdus, std::shared_ptr<SmsReceiveReliabilityHandler> reliabilityHandler)
{
    pdus->assign(MAX_SEGMENT_NUM, "");
    int msgSeg = static_cast<int>(indexer->GetMsgCount());
    int8_t notNullPart = msgSeg;
    std::vector<SmsReceiveIndexer> dbIndexers;
    DataShare::DataSharePredicates predicates;
    predicates.EqualTo(SmsSubsection::SENDER_NUMBER, indexer->GetOriginatingAddress())
        ->And()
        ->EqualTo(SmsSubsection::SMS_SUBSECTION_ID, std::to_string(indexer->GetMsgRefId()))
        ->And()
        ->EqualTo(SmsSubsection::SIZE, std::to_string(indexer->GetMsgCount()));
    DelayedSingleton<SmsPersistHelper>::GetInstance()->Query(predicates, dbIndexers);
    int8_t count = 0;
    for (const auto &v : dbIndexers) {
        ++count;
        string pdu = StringUtils::StringToHex(v.GetPdu());
        if ((v.GetMsgSeqId() - PDU_POS_OFFSET >= MAX_SEGMENT_NUM) || (v.GetMsgSeqId() - PDU_POS_OFFSET < 0)) {
            reliabilityHandler->DeleteMessageFormDb(indexer->GetMsgRefId());
            return false;
        }
        pdus->at(v.GetMsgSeqId() - PDU_POS_OFFSET) = pdu;
        if (v.GetPdu().size() == 0) {
            --notNullPart;
        }
    }
    if ((count != msgSeg) || (pdus->empty()) || (notNullPart != msgSeg)) {
        return false;
    }
    UpdateMultiPageMessage(indexer, pdus);
    return true;
}

void SmsReceiveHandler::UpdateMultiPageMessage(
    const std::shared_ptr<SmsReceiveIndexer> &indexer, std::shared_ptr<std::vector<std::string>> pdus)
{
    if ((indexer == nullptr) || (pdus == nullptr) || (pdus->empty())) {
        TELEPHONY_LOGE("indexer or pdus is null");
        return;
    }
    std::string messagBody;
    std::string userDataRaw;
    std::string rawWapPushUserData;
    for (const auto &pdu : *pdus) {
        if (pdu.empty()) {
            continue;
        }
        std::shared_ptr<SmsBaseMessage> baseMessage = GsmSmsMessage::CreateMessage(pdu);
        if (baseMessage == nullptr) {
            continue;
        }
        messagBody.append(baseMessage->GetVisibleMessageBody());
        userDataRaw.append(baseMessage->GetRawUserData());
        if (!indexer->GetIsWapPushMsg()) {
            continue;
        }
        auto wapDataHex = StringUtils::StringToHex(baseMessage->GetRawWapPushUserData());
        if (wapDataHex.substr(0, WAP_SEQ_NUMBER_TAG.size()) == WAP_SEQ_NUMBER_TAG) {
            rawWapPushUserData.append(StringUtils::HexToString(wapDataHex.substr(WAP_SEQ_NUMBER_LEN)));
        } else {
            rawWapPushUserData.append(StringUtils::HexToString(wapDataHex));
        }
    }

    indexer->SetVisibleMessageBody(messagBody);
    indexer->SetRawUserData(userDataRaw);
    if (indexer->GetIsWapPushMsg()) {
        indexer->SetRawWapPushUserData(rawWapPushUserData);
    }
}

bool SmsReceiveHandler::IsRepeatedMessagePart(const shared_ptr<SmsReceiveIndexer> &smsIndexer)
{
    if (smsIndexer != nullptr) {
        std::vector<SmsReceiveIndexer> dbIndexers;
        DataShare::DataSharePredicates predicates;
        predicates.EqualTo(SmsSubsection::SENDER_NUMBER, smsIndexer->GetOriginatingAddress())
            ->And()
            ->EqualTo(SmsSubsection::SMS_SUBSECTION_ID, std::to_string(smsIndexer->GetMsgRefId()))
            ->And()
            ->EqualTo(SmsSubsection::SIZE, std::to_string(smsIndexer->GetMsgCount()));
        DelayedSingleton<SmsPersistHelper>::GetInstance()->Query(predicates, dbIndexers);

        for (const auto &it : dbIndexers) {
            if (it.GetMsgSeqId() == smsIndexer->GetMsgSeqId()) {
                return true;
            }
        }
    }
    return false;
}

bool SmsReceiveHandler::AddMsgToDB(const std::shared_ptr<SmsReceiveIndexer> indexer)
{
    if (indexer == nullptr) {
        TELEPHONY_LOGE("indexer is nullptr.");
        return false;
    }

    DataShare::DataShareValuesBucket bucket;
    bucket.Put(SmsSubsection::SLOT_ID, std::to_string(slotId_));
    bucket.Put(SmsSubsection::RECEIVER_NUMBER, indexer->GetOriginatingAddress());
    bucket.Put(SmsSubsection::SENDER_NUMBER, indexer->GetOriginatingAddress());
    bucket.Put(SmsSubsection::START_TIME, std::to_string(indexer->GetTimestamp()));
    bucket.Put(SmsSubsection::END_TIME, std::to_string(indexer->GetTimestamp()));
    bucket.Put(SmsSubsection::REW_PUD, StringUtils::StringToHex(indexer->GetPdu()));

    bucket.Put(SmsSubsection::FORMAT, indexer->GetIsCdma() ? SMS_TYPE_CDMA : SMS_TYPE_GSM);
    bucket.Put(SmsSubsection::DEST_PORT, indexer->GetDestPort());
    bucket.Put(SmsSubsection::SMS_SUBSECTION_ID, indexer->GetMsgRefId());
    bucket.Put(SmsSubsection::SIZE, indexer->GetMsgCount());
    bucket.Put(SmsSubsection::SUBSECTION_INDEX, indexer->GetMsgSeqId());
    uint16_t dataBaseId = 0;
    bool ret = DelayedSingleton<SmsPersistHelper>::GetInstance()->Insert(bucket, dataBaseId);
    indexer->SetDataBaseId(dataBaseId);
    if (!ret) {
        SmsHiSysEvent::WriteSmsReceiveFaultEvent(slotId_, SmsMmsMessageType::SMS_SHORT_MESSAGE,
            SmsMmsErrorCode::SMS_ERROR_ADD_TO_DATABASE_FAIL, "add msg to database error");
    }
    return ret;
}
} // namespace Telephony
} // namespace OHOS