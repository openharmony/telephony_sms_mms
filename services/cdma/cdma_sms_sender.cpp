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

#include "cdma_sms_sender.h"

#include "cdma_sms_message.h"
#include "securec.h"
#include "string_utils.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
CdmaSmsSender::CdmaSmsSender(const std::shared_ptr<AppExecFwk::EventRunner> &runner, int32_t slotId,
    std::function<void(std::shared_ptr<SmsSendIndexer>)> sendRetryFun)
    : SmsSender(runner, slotId, sendRetryFun)
{}

CdmaSmsSender::~CdmaSmsSender() {}

void CdmaSmsSender::TextBasedSmsDelivery(const std::string &desAddr, const std::string &scAddr, const std::string &text,
    const sptr<ISendShortMessageCallback> &sendCallback,
    const sptr<IDeliveryShortMessageCallback> &deliveryCallback)
{
    CdmaSmsMessage message;
    SmsCodingScheme codingType;
    std::vector<struct SplitInfo> splits;
    message.SplitMessage(splits, text, false, codingType);
    std::unique_ptr<SmsTransMsg> transMsg = nullptr;
    bool bStatusReport = (deliveryCallback == nullptr) ? false : true;
    transMsg = message.CreateSubmitTransMsg(desAddr, scAddr, text, bStatusReport, codingType);
    if (transMsg == nullptr) {
        SendResultCallBack(sendCallback, ISendShortMessageCallback::SEND_SMS_FAILURE_UNKNOWN);
        TELEPHONY_LOGE("CreateSubmitTransMsg nullptr fail.");
        return;
    }
    /* 1. Set Reply sequence number. */
    uint8_t msgRef8bit = GetSeqNum();
    transMsg->data.p2pMsg.transReplySeq = msgRef8bit;
    /* 2. Set msg ID. */
    transMsg->data.p2pMsg.telesvcMsg.data.submit.msgId.msgId = GetSubmitMsgId();
    std::shared_ptr<uint8_t> unSentCellCount = std::make_shared<uint8_t>(splits.size());
    std::shared_ptr<bool> hasCellFailed = std::make_shared<bool>(false);
    if (unSentCellCount == nullptr || hasCellFailed == nullptr) {
        SendResultCallBack(sendCallback, ISendShortMessageCallback::SEND_SMS_FAILURE_UNKNOWN);
        TELEPHONY_LOGE("unSentCellCount or hasCellFailed is nullptr.");
        return;
    }
    std::chrono::system_clock::duration timePoint = std::chrono::system_clock::now().time_since_epoch();
    long timeStamp = std::chrono::duration_cast<std::chrono::seconds>(timePoint).count();

    for (std::size_t i = 0; i < splits.size(); i++) {
        if (!InitEncodeData(transMsg, splits, i)) {
            SendResultCallBack(sendCallback, ISendShortMessageCallback::SEND_SMS_FAILURE_UNKNOWN);
            return;
        }
        SetPduSeqInfo(splits.size(), transMsg, i, msgRef8bit);
        transMsg->data.p2pMsg.telesvcMsg.data.submit.userData.userData.length = splits[i].encodeData.size();
        /* encode msg data */
        unsigned char pduStr[TAPI_NETTEXT_SMDATA_SIZE_MAX + 1] = {0};
        int len = CdmaSmsPduCodec::EncodeMsg(*transMsg.get(), pduStr);

        std::shared_ptr<SmsSendIndexer> indexer = nullptr;
        indexer = std::make_shared<SmsSendIndexer>(desAddr, scAddr, text, sendCallback, deliveryCallback);
        std::vector<uint8_t> pdu(pduStr, pduStr + len);
        indexer->SetEncodePdu(std::move(pdu));
        indexer->SetMsgRefId(msgRef8bit);
        indexer->SetNetWorkType(NET_TYPE_CDMA);
        indexer->SetUnSentCellCount(unSentCellCount);
        indexer->SetHasCellFailed(hasCellFailed);
        indexer->SetTimeStamp(timeStamp);
        SendSmsToRil(indexer);
    }
}

bool CdmaSmsSender::InitEncodeData(std::unique_ptr<SmsTransMsg> &transMsg,
    const std::vector<struct SplitInfo> &splits, std::size_t index)
{
    bool result = false;
    if (transMsg == nullptr || index >= splits.size()) {
        TELEPHONY_LOGE("Parameter is invalid.");
        return result;
    }
    if (memset_s(transMsg->data.p2pMsg.telesvcMsg.data.submit.userData.userData.data,
        sizeof(transMsg->data.p2pMsg.telesvcMsg.data.submit.userData.userData.data), 0x00,
        sizeof(transMsg->data.p2pMsg.telesvcMsg.data.submit.userData.userData.data)) != EOK) {
        TELEPHONY_LOGE("InitEncodeData memset_s fail.");
        return result;
    }
    if (memcpy_s(transMsg->data.p2pMsg.telesvcMsg.data.submit.userData.userData.data,
        sizeof(transMsg->data.p2pMsg.telesvcMsg.data.submit.userData.userData.data),
        splits[index].encodeData.data(), splits[index].encodeData.size()) != EOK) {
        TELEPHONY_LOGE("InitEncodeData memcpy_s fail.");
        return result;
    }
    return true;
}

void CdmaSmsSender::SetPduSeqInfo(const std::size_t size, const std::unique_ptr<SmsTransMsg> &transMsg,
    const std::size_t index, const uint8_t msgRef8bit)
{
    if (size > 1) {
        transMsg->data.p2pMsg.transTelesvcId = SMS_TRANS_TELESVC_WEMT;
        transMsg->data.p2pMsg.telesvcMsg.data.submit.msgId.headerInd = true;
        transMsg->data.p2pMsg.telesvcMsg.data.submit.userData.userData.headerCnt = 1;
        transMsg->data.p2pMsg.telesvcMsg.data.submit.userData.userData.header[0].udhType = SMS_UDH_CONCAT_8BIT;
        transMsg->data.p2pMsg.telesvcMsg.data.submit.userData.userData.header[0].udh.concat8bit.msgRef =
            msgRef8bit;
        transMsg->data.p2pMsg.telesvcMsg.data.submit.userData.userData.header[0].udh.concat8bit.totalSeg =
            static_cast<unsigned char>(size);
        transMsg->data.p2pMsg.telesvcMsg.data.submit.userData.userData.header[0].udh.concat8bit.seqNum = index + 1;
    }
}

void CdmaSmsSender::DataBasedSmsDelivery(const std::string &desAddr, const std::string &scAddr, int32_t port,
    const uint8_t *data, uint32_t dataLen, const sptr<ISendShortMessageCallback> &sendCallback,
    const sptr<IDeliveryShortMessageCallback> &deliveryCallback)
{
    CdmaSmsMessage message;
    SmsCodingScheme codingType;
    std::vector<struct SplitInfo> splits;
    std::string text(data, data + dataLen);
    message.SplitMessage(splits, text, true, codingType);
    std::unique_ptr<SmsTransMsg> transMsg = nullptr;
    bool bStatusReport = (deliveryCallback == nullptr) ? false : true;
    transMsg = message.CreateSubmitTransMsg(desAddr, scAddr, port, data, dataLen, bStatusReport);
    if (transMsg == nullptr || splits.size() <= 0) {
        SendResultCallBack(sendCallback, ISendShortMessageCallback::SEND_SMS_FAILURE_UNKNOWN);
        TELEPHONY_LOGE("transMsg is nullptr or splits.size() less than 0.");
        return;
    }
    /* Set Reply sequence number. */
    uint8_t msgRef8bit = GetSeqNum();
    transMsg->data.p2pMsg.transReplySeq = msgRef8bit;
    /* Set msg ID. */
    transMsg->data.p2pMsg.telesvcMsg.data.submit.msgId.msgId = GetSubmitMsgId();
    /* while user data header isn't exist, headerInd must be set false. */
    transMsg->data.p2pMsg.telesvcMsg.data.submit.msgId.headerInd = true;
    std::shared_ptr<uint8_t> unSentCellCount = std::make_shared<uint8_t>(splits.size());
    std::shared_ptr<bool> hasCellFailed = std::make_shared<bool>(false);
    if (unSentCellCount == nullptr || hasCellFailed == nullptr) {
        SendResultCallBack(sendCallback, ISendShortMessageCallback::SEND_SMS_FAILURE_UNKNOWN);
        TELEPHONY_LOGE("unSentCellCount or hasCellFailed is nullptr.");
        return;
    }
    std::chrono::system_clock::duration timePoint = std::chrono::system_clock::now().time_since_epoch();
    long timeStamp = std::chrono::duration_cast<std::chrono::seconds>(timePoint).count();

    if (memcpy_s(transMsg->data.p2pMsg.telesvcMsg.data.submit.userData.userData.data,
        sizeof(transMsg->data.p2pMsg.telesvcMsg.data.submit.userData.userData.data),
        splits[0].encodeData.data(), splits[0].encodeData.size()) != EOK) {
        TELEPHONY_LOGE("DataBasedSmsDelivery memcpy_s fail.");
        SendResultCallBack(sendCallback, ISendShortMessageCallback::SEND_SMS_FAILURE_UNKNOWN);
        return;
    }
    transMsg->data.p2pMsg.telesvcMsg.data.submit.userData.userData.length = splits[0].encodeData.size();
    /* encode msg data */
    unsigned char pduStr[TAPI_NETTEXT_SMDATA_SIZE_MAX + 1] = {0};
    int len = CdmaSmsPduCodec::EncodeMsg(*transMsg.get(), pduStr);
    std::shared_ptr<SmsSendIndexer> indexer = nullptr;
    indexer = std::make_shared<SmsSendIndexer>(desAddr, scAddr, text, sendCallback, deliveryCallback);
    std::vector<uint8_t> pdu(pduStr, pduStr + len);
    indexer->SetEncodePdu(std::move(pdu));
    indexer->SetMsgRefId(msgRef8bit);
    indexer->SetNetWorkType(NET_TYPE_CDMA);
    indexer->SetUnSentCellCount(unSentCellCount);
    indexer->SetHasCellFailed(hasCellFailed);
    indexer->SetTimeStamp(timeStamp);
    SendSmsToRil(indexer);
}

void CdmaSmsSender::StatusReportAnalysis(const AppExecFwk::InnerEvent::Pointer &event)
{
    if (event == nullptr) {
        TELEPHONY_LOGE("cdma_sms_sender: StatusReportAnalysis event nullptr error.");
        return;
    }
    std::shared_ptr<SmsReceiveIndexer> statusInfo = event->GetSharedObject<SmsReceiveIndexer>();
    if (statusInfo == nullptr) {
        TELEPHONY_LOGE("cdma_sms_sender: StatusReportAnalysis statusInfo nullptr error.");
        return;
    }
    std::string pdu = StringUtils::StringToHex(statusInfo->GetPdu());
    std::shared_ptr<CdmaSmsMessage> message = CdmaSmsMessage::CreateMessage(pdu);
    if (message == nullptr) {
        TELEPHONY_LOGE("message is nullptr.");
        return;
    }
    sptr<IDeliveryShortMessageCallback> deliveryCallback = nullptr;
    auto oldIndexer = reportList_.begin();
    while (oldIndexer != reportList_.end()) {
        auto iter = oldIndexer++;
        if (*iter != nullptr) {
            if (message->GetMsgRef() == (*iter)->GetMsgRefId()) {
                // save the message to db, or updata to db msg state(success or fail)
                deliveryCallback = (*iter)->GetDeliveryCallback();
                reportList_.erase(iter);
            }
        }
    }
    std::shared_ptr<Core> core = GetCore();
    auto ackPointer = AppExecFwk::InnerEvent::Pointer(nullptr, nullptr);
    if (core != nullptr) {
        core->SendSmsAck(true, AckIncomeCause::SMS_ACK_PROCESSED, ackPointer);
    }
    if (deliveryCallback != nullptr) {
        std::string ackpdu = StringUtils::StringToHex(message->GetRawPdu());
        deliveryCallback->OnSmsDeliveryResult(StringUtils::ToUtf16(ackpdu));
        TELEPHONY_LOGI("gsm_sms_sender: StatusReportAnalysis %{public}s", pdu.c_str());
    }
}

void CdmaSmsSender::SendSmsToRil(const std::shared_ptr<SmsSendIndexer> &smsIndexer)
{
    if (smsIndexer == nullptr) {
        TELEPHONY_LOGE("cdma_sms_sender: SendSms smsIndexer nullptr");
        return;
    }
    std::shared_ptr<Core> core = GetCore();
    if (core == nullptr) {
        SendResultCallBack(smsIndexer, ISendShortMessageCallback::SEND_SMS_FAILURE_SERVICE_UNAVAILABLE);
        TELEPHONY_LOGE("cdma_sms_sender: SendSms core nullptr");
        return;
    }
    int64_t refId = GetMsgRef64Bit();
    if (!SendCacheMapAddItem(refId, smsIndexer)) {
        TELEPHONY_LOGE("SendCacheMapAddItem Error!!");
        return;
    }
    if (netDomainType_ != NET_DOMAIN_IMS && (smsIndexer->GetPsResendCount() == 0)) {
        smsIndexer->SetCsResendCount(smsIndexer->GetCsResendCount() + 1);
        auto reply = AppExecFwk::InnerEvent::Get(ObserverHandler::RADIO_SEND_SMS, refId);
        reply->SetOwner(shared_from_this());
    } else {
        smsIndexer->SetPsResendCount(smsIndexer->GetPsResendCount() + 1);
    }
}

void CdmaSmsSender::Init() {}

void CdmaSmsSender::ReceiveStatusReport(const std::shared_ptr<SmsReceiveIndexer> &smsIndexer)
{
    SendEvent(ObserverHandler::RADIO_SMS_STATUS, smsIndexer);
}

uint8_t CdmaSmsSender::GetSeqNum()
{
    msgSeqNum_ = ((msgSeqNum_ + 1) % SMS_SEQ_NUM_MAX);
    return msgSeqNum_;
}

uint8_t CdmaSmsSender::GetSubmitMsgId()
{
    msgSubmitId_ = ((msgSubmitId_ + 1) % SMS_MAX_MESSAGE_ID);
    return msgSubmitId_;
}

void CdmaSmsSender::ResendTextDelivery(const std::shared_ptr<SmsSendIndexer> &smsIndexer) {}

void CdmaSmsSender::ResendDataDelivery(const std::shared_ptr<SmsSendIndexer> &smsIndexer) {}

std::vector<std::string> CdmaSmsSender::SplitMessage(const std::string &message)
{
    std::vector<std::string> result;
    SmsCodingScheme codingType;
    CdmaSmsMessage cdmaSmsMessage;
    std::vector<struct SplitInfo> cellsInfos;
    cdmaSmsMessage.SplitMessage(cellsInfos, message, false, codingType);
    for (auto &item : cellsInfos) {
        result.emplace_back(item.text);
    }
    return result;
}

std::vector<int32_t> CdmaSmsSender::CalculateLength(const std::string &message, bool force7BitCode)
{
    std::vector<int32_t> result;
    CdmaSmsMessage cdmaSmsMessage;
    LengthInfo lenInfo;
    cdmaSmsMessage.CalculateLength(message, force7BitCode, lenInfo);
    result.emplace_back(static_cast<int32_t>(lenInfo.msgSegCount));
    result.emplace_back(static_cast<int32_t>(lenInfo.msgEncodeCount));
    result.emplace_back(static_cast<int32_t>(lenInfo.msgRemainCount));
    result.emplace_back(static_cast<int32_t>(lenInfo.dcs));
    return result;
}
} // namespace Telephony
} // namespace OHOS