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

#include "securec.h"

#include "cdma_sms_message.h"
#include "core_manager_inner.h"
#include "radio_event.h"
#include "string_utils.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
using namespace std;
CdmaSmsSender::CdmaSmsSender(const shared_ptr<AppExecFwk::EventRunner> &runner, int32_t slotId,
    function<void(shared_ptr<SmsSendIndexer>)> sendRetryFun)
    : SmsSender(runner, slotId, sendRetryFun)
{}

CdmaSmsSender::~CdmaSmsSender() {}

void CdmaSmsSender::TextBasedSmsDelivery(const string &desAddr, const string &scAddr, const string &text,
    const sptr<ISendShortMessageCallback> &sendCallback,
    const sptr<IDeliveryShortMessageCallback> &deliveryCallback)
{
    CdmaSmsMessage message;
    SmsCodingScheme codingType;
    std::vector<struct SplitInfo> splits;
    message.SplitMessage(splits, text, CheckForce7BitEncodeType(), codingType);
    if (splits.size() > MAX_SEGMENT_NUM) {
        SendResultCallBack(sendCallback, ISendShortMessageCallback::SEND_SMS_FAILURE_UNKNOWN);
        TELEPHONY_LOGE("message exceed the limit.");
        return;
    }
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
    uint16_t msgId = GetSubmitMsgId();
    transMsg->data.p2pMsg.telesvcMsg.data.submit.msgId.msgId = msgId;
    shared_ptr<uint8_t> unSentCellCount = make_shared<uint8_t>(splits.size());
    shared_ptr<bool> hasCellFailed = make_shared<bool>(false);
    if (unSentCellCount == nullptr || hasCellFailed == nullptr) {
        SendResultCallBack(sendCallback, ISendShortMessageCallback::SEND_SMS_FAILURE_UNKNOWN);
        return;
    }
    chrono::system_clock::duration timePoint = chrono::system_clock::now().time_since_epoch();
    long timeStamp = chrono::duration_cast<chrono::seconds>(timePoint).count();

    for (std::size_t i = 0; i < splits.size(); i++) {
        (void)memset_s(transMsg->data.p2pMsg.telesvcMsg.data.submit.userData.userData.data,
            sizeof(transMsg->data.p2pMsg.telesvcMsg.data.submit.userData.userData.data), 0x00,
            sizeof(transMsg->data.p2pMsg.telesvcMsg.data.submit.userData.userData.data));
        if (memcpy_s(transMsg->data.p2pMsg.telesvcMsg.data.submit.userData.userData.data,
            sizeof(transMsg->data.p2pMsg.telesvcMsg.data.submit.userData.userData.data),
            splits[i].encodeData.data(), splits[i].encodeData.size()) != EOK) {
            SendResultCallBack(sendCallback, ISendShortMessageCallback::SEND_SMS_FAILURE_UNKNOWN);
            return;
        }
        std::string segmentText;
        segmentText.append((char *)(splits[i].encodeData.data()), splits[i].encodeData.size());
        std::shared_ptr<SmsSendIndexer> indexer = nullptr;
        indexer = make_shared<SmsSendIndexer>(desAddr, scAddr, segmentText, sendCallback, deliveryCallback);
        if (indexer == nullptr) {
            SendResultCallBack(sendCallback, ISendShortMessageCallback::SEND_SMS_FAILURE_UNKNOWN);
            return;
        }
        indexer->SetDcs(splits[i].encodeType);
        SetPduSeqInfo(indexer, splits.size(), transMsg, i, msgRef8bit);
        transMsg->data.p2pMsg.telesvcMsg.data.submit.userData.userData.length = splits[i].encodeData.size();
        /* encode msg data */
        unsigned char pduStr[TAPI_NETTEXT_SMDATA_SIZE_MAX + 1] = {0};
        int len = CdmaSmsPduCodec::EncodeMsg(*transMsg.get(), pduStr);
        std::vector<uint8_t> pdu(pduStr, pduStr + len);
        indexer->SetEncodePdu(std::move(pdu));
        indexer->SetMsgRefId(msgRef8bit);
        indexer->SetNetWorkType(NET_TYPE_CDMA);
        indexer->SetUnSentCellCount(unSentCellCount);
        indexer->SetHasCellFailed(hasCellFailed);
        indexer->SetTimeStamp(timeStamp);
        indexer->SetMsgId(msgId);
        SendSmsToRil(indexer);
    }
}

void CdmaSmsSender::SetPduSeqInfo(const std::shared_ptr<SmsSendIndexer> &smsIndexer, const std::size_t size,
    const std::unique_ptr<SmsTransMsg> &transMsg, const std::size_t index, const uint8_t msgRef8bit)
{
    if (size > 1) {
        smsIndexer->SetIsConcat(true);
        SmsConcat smsConcat;
        transMsg->data.p2pMsg.transTelesvcId = SMS_TRANS_TELESVC_WEMT;
        transMsg->data.p2pMsg.telesvcMsg.data.submit.msgId.headerInd = true;
        transMsg->data.p2pMsg.telesvcMsg.data.submit.userData.userData.headerCnt = 1;
        transMsg->data.p2pMsg.telesvcMsg.data.submit.userData.userData.header[0].udhType = SMS_UDH_CONCAT_8BIT;
        transMsg->data.p2pMsg.telesvcMsg.data.submit.userData.userData.header[0].udh.concat8bit.msgRef = msgRef8bit;
        transMsg->data.p2pMsg.telesvcMsg.data.submit.userData.userData.header[0].udh.concat8bit.totalSeg =
            static_cast<unsigned char>(size);
        transMsg->data.p2pMsg.telesvcMsg.data.submit.userData.userData.header[0].udh.concat8bit.seqNum = index + 1;
        smsConcat.msgRef = msgRef8bit;
        smsConcat.seqNum = index + 1;
        smsConcat.totalSeg = static_cast<unsigned char>(size);
        smsConcat.is8Bits = true;
        smsIndexer->SetSmsConcat(smsConcat);
    }
}

void CdmaSmsSender::DataBasedSmsDelivery(const string &desAddr, const string &scAddr, int32_t port,
    const uint8_t *data, uint32_t dataLen, const sptr<ISendShortMessageCallback> &sendCallback,
    const sptr<IDeliveryShortMessageCallback> &deliveryCallback)
{
    CdmaSmsMessage message;
    SmsCodingScheme codingType;
    std::vector<struct SplitInfo> splits;
    std::string text((char *)data, dataLen);
    message.SplitMessage(splits, text, false, codingType);
    if (splits.size() == 0) {
        TELEPHONY_LOGE("splits fail.");
        return;
    }
    std::unique_ptr<SmsTransMsg> transMsg = nullptr;
    bool bStatusReport = (deliveryCallback == nullptr) ? false : true;
    transMsg = message.CreateSubmitTransMsg(desAddr, scAddr, port, data, dataLen, bStatusReport);
    if (transMsg == nullptr) {
        SendResultCallBack(sendCallback, ISendShortMessageCallback::SEND_SMS_FAILURE_UNKNOWN);
        TELEPHONY_LOGE("CreateSubmitTransMsg nullptr fail.");
        return;
    }
    /* Set Reply sequence number. */
    uint8_t msgRef8bit = GetSeqNum();
    transMsg->data.p2pMsg.transReplySeq = msgRef8bit;
    /* Set msg ID. */
    uint16_t msgId = GetSubmitMsgId();
    transMsg->data.p2pMsg.telesvcMsg.data.submit.msgId.msgId = msgId;
    /* while user data header isn't exist, headerInd must be set false. */
    transMsg->data.p2pMsg.telesvcMsg.data.submit.msgId.headerInd = true;

    chrono::system_clock::duration timePoint = chrono::system_clock::now().time_since_epoch();
    long timeStamp = chrono::duration_cast<chrono::seconds>(timePoint).count();
    transMsg->data.p2pMsg.telesvcMsg.data.submit.userData.userData.length = splits[0].encodeData.size();
    if (memcpy_s(transMsg->data.p2pMsg.telesvcMsg.data.submit.userData.userData.data,
        sizeof(transMsg->data.p2pMsg.telesvcMsg.data.submit.userData.userData.data),
        splits[0].encodeData.data(), splits[0].encodeData.size()) != EOK) {
        SendResultCallBack(sendCallback, ISendShortMessageCallback::SEND_SMS_FAILURE_UNKNOWN);
        TELEPHONY_LOGE("memcpy_s return error.");
        return;
    }
    /* encode msg data */
    unsigned char pduStr[TAPI_NETTEXT_SMDATA_SIZE_MAX + 1] = {0};
    int len = CdmaSmsPduCodec::EncodeMsg(*transMsg.get(), pduStr);
    if (len <= 0) {
        TELEPHONY_LOGE("EncodeMsg Error len = %{public}d", len);
        SendResultCallBack(sendCallback, ISendShortMessageCallback::SEND_SMS_FAILURE_UNKNOWN);
        return;
    }

    const uint8_t segmentCount = 1;
    std::shared_ptr<SmsSendIndexer> indexer = nullptr;
    shared_ptr<uint8_t> unSentCellCount = make_shared<uint8_t>(segmentCount);
    shared_ptr<bool> hasCellFailed = make_shared<bool>(false);
    indexer = make_shared<SmsSendIndexer>(desAddr, scAddr, port, splits[0].encodeData.data(),
        splits[0].encodeData.size(), sendCallback, deliveryCallback);
    if (indexer == nullptr || unSentCellCount == nullptr || hasCellFailed == nullptr) {
        SendResultCallBack(sendCallback, ISendShortMessageCallback::SEND_SMS_FAILURE_UNKNOWN);
        TELEPHONY_LOGE("Init SmsSend Indexer Error.");
        return;
    }
    std::vector<uint8_t> pdu(pduStr, pduStr + len);
    indexer->SetEncodePdu(std::move(pdu));
    indexer->SetMsgRefId(msgRef8bit);
    indexer->SetNetWorkType(NET_TYPE_CDMA);
    indexer->SetUnSentCellCount(unSentCellCount);
    indexer->SetHasCellFailed(hasCellFailed);
    indexer->SetTimeStamp(timeStamp);
    indexer->SetMsgId(msgId);
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
    if (deliveryCallback != nullptr) {
        std::string ackpdu = StringUtils::StringToHex(message->GetRawPdu());
        deliveryCallback->OnSmsDeliveryResult(StringUtils::ToUtf16(ackpdu));
        TELEPHONY_LOGI("gsm_sms_sender: StatusReportAnalysis %{public}s", pdu.c_str());
    }
}

void CdmaSmsSender::SendSmsToRil(const shared_ptr<SmsSendIndexer> &smsIndexer)
{
    if (smsIndexer == nullptr) {
        TELEPHONY_LOGE("cdma_sms_sender: SendSms smsIndexer nullptr");
        return;
    }
    if ((!isImsNetDomain_ && voiceServiceState_ != static_cast<int32_t>(RegServiceState::REG_STATE_IN_SERVICE))) {
        SendResultCallBack(smsIndexer, ISendShortMessageCallback::SEND_SMS_FAILURE_SERVICE_UNAVAILABLE);
        TELEPHONY_LOGE("cdma_sms_sender: SendSms not in service");
        return;
    }
    int64_t refId = GetMsgRef64Bit();
    if (!SendCacheMapAddItem(refId, smsIndexer)) {
        TELEPHONY_LOGE("SendCacheMapAddItem Error!!");
    }

    std::string pdu = StringUtils::StringToHex(smsIndexer->GetEncodePdu());
    if (!isImsNetDomain_ && (smsIndexer->GetPsResendCount() == 0)) {
        smsIndexer->SetCsResendCount(smsIndexer->GetCsResendCount() + 1);
        CoreManagerInner::GetInstance().SendCdmaSms(
            slotId_, RadioEvent::RADIO_SEND_CDMA_SMS, pdu, refId, shared_from_this());
        TELEPHONY_LOGI("SendSmsToRil pdu = %{public}s", pdu.c_str());
    } else {
        smsIndexer->SetPsResendCount(smsIndexer->GetPsResendCount() + 1);
        TELEPHONY_LOGI("ims network domain send unsupported.!");
    }
}

void CdmaSmsSender::Init() {}

void CdmaSmsSender::ReceiveStatusReport(const std::shared_ptr<SmsReceiveIndexer> &smsIndexer)
{
    SendEvent(RadioEvent::RADIO_SMS_STATUS, smsIndexer);
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

void CdmaSmsSender::ResendTextDelivery(const std::shared_ptr<SmsSendIndexer> &smsIndexer)
{
    if (smsIndexer == nullptr) {
        TELEPHONY_LOGE("smsIndexer is nullptr!!");
        return;
    }
    CdmaSmsMessage message;
    SmsCodingScheme codingType = smsIndexer->GetDcs();
    std::unique_ptr<SmsTransMsg> transMsg = nullptr;
    bool bStatusReport = (smsIndexer->GetDeliveryCallback() == nullptr) ? false : true;
    transMsg = message.CreateSubmitTransMsg(
        smsIndexer->GetDestAddr(), smsIndexer->GetSmcaAddr(), smsIndexer->GetText(), bStatusReport, codingType);
    if (transMsg == nullptr) {
        SendResultCallBack(smsIndexer->GetSendCallback(), ISendShortMessageCallback::SEND_SMS_FAILURE_UNKNOWN);
        TELEPHONY_LOGE("CreateSubmitTransMsg nullptr fail.");
        return;
    }
    /* 1. Set Reply sequence number. */
    uint8_t msgRef8bit = smsIndexer->GetMsgRefId();
    transMsg->data.p2pMsg.transReplySeq = msgRef8bit;
    /* 2. Set msg ID. */
    transMsg->data.p2pMsg.telesvcMsg.data.submit.msgId.msgId = smsIndexer->GetMsgId();
    chrono::system_clock::duration timePoint = chrono::system_clock::now().time_since_epoch();
    long timeStamp = chrono::duration_cast<chrono::seconds>(timePoint).count();
    smsIndexer->SetTimeStamp(timeStamp);
    transMsg->data.p2pMsg.telesvcMsg.data.submit.userData.encodeType = SMS_ENCODE_OCTET;
    (void)memset_s(transMsg->data.p2pMsg.telesvcMsg.data.submit.userData.userData.data,
        sizeof(transMsg->data.p2pMsg.telesvcMsg.data.submit.userData.userData.data), 0x00,
        sizeof(transMsg->data.p2pMsg.telesvcMsg.data.submit.userData.userData.data));
    if (memcpy_s(transMsg->data.p2pMsg.telesvcMsg.data.submit.userData.userData.data,
        sizeof(transMsg->data.p2pMsg.telesvcMsg.data.submit.userData.userData.data),
        smsIndexer->GetText().data(), smsIndexer->GetText().length()) != EOK) {
        SendResultCallBack(smsIndexer->GetSendCallback(), ISendShortMessageCallback::SEND_SMS_FAILURE_UNKNOWN);
        return;
    }
    SetConcact(smsIndexer, transMsg);
    /* encode msg data */
    unsigned char pduStr[TAPI_NETTEXT_SMDATA_SIZE_MAX + 1] = {0};
    int len = CdmaSmsPduCodec::EncodeMsg(*transMsg.get(), pduStr);
    std::vector<uint8_t> pdu(pduStr, pduStr + len);
    smsIndexer->SetEncodePdu(std::move(pdu));
    smsIndexer->SetNetWorkType(NET_TYPE_CDMA);
    smsIndexer->SetTimeStamp(timeStamp);
    SendSmsToRil(smsIndexer);
}

void CdmaSmsSender::ResendDataDelivery(const std::shared_ptr<SmsSendIndexer> &smsIndexer)
{
    CdmaSmsMessage message;
    std::unique_ptr<SmsTransMsg> transMsg = nullptr;
    bool bStatusReport = (smsIndexer->GetDeliveryCallback() == nullptr) ? false : true;
    transMsg = message.CreateSubmitTransMsg(smsIndexer->GetDestAddr(), smsIndexer->GetSmcaAddr(),
        smsIndexer->GetDestPort(), smsIndexer->GetData().data(), smsIndexer->GetData().size(), bStatusReport);
    if (transMsg == nullptr) {
        SendResultCallBack(smsIndexer->GetSendCallback(), ISendShortMessageCallback::SEND_SMS_FAILURE_UNKNOWN);
        TELEPHONY_LOGE("CreateSubmitTransMsg nullptr fail.");
        return;
    }
    /* Set Reply sequence number. */
    uint8_t msgRef8bit = smsIndexer->GetMsgRefId();
    transMsg->data.p2pMsg.transReplySeq = msgRef8bit;
    /* Set msg ID. */
    transMsg->data.p2pMsg.telesvcMsg.data.submit.msgId.msgId = smsIndexer->GetMsgId();
    /* while user data header isn't exist, headerInd must be set false. */
    transMsg->data.p2pMsg.telesvcMsg.data.submit.msgId.headerInd = true;

    chrono::system_clock::duration timePoint = chrono::system_clock::now().time_since_epoch();
    long timeStamp = chrono::duration_cast<chrono::seconds>(timePoint).count();
    transMsg->data.p2pMsg.telesvcMsg.data.submit.userData.encodeType = SMS_ENCODE_OCTET;
    if (memcpy_s(transMsg->data.p2pMsg.telesvcMsg.data.submit.userData.userData.data,
        sizeof(transMsg->data.p2pMsg.telesvcMsg.data.submit.userData.userData.data),
        smsIndexer->GetData().data(), smsIndexer->GetData().size()) != EOK) {
        SendResultCallBack(smsIndexer->GetSendCallback(), ISendShortMessageCallback::SEND_SMS_FAILURE_UNKNOWN);
        return;
    }

    transMsg->data.p2pMsg.telesvcMsg.data.submit.userData.userData.length = smsIndexer->GetData().size();
    /* encode msg data */
    unsigned char pduStr[TAPI_NETTEXT_SMDATA_SIZE_MAX + 1] = {0};
    int len = CdmaSmsPduCodec::EncodeMsg(*transMsg.get(), pduStr);
    std::vector<uint8_t> pdu(pduStr, pduStr + len);
    smsIndexer->SetEncodePdu(std::move(pdu));
    smsIndexer->SetNetWorkType(NET_TYPE_CDMA);
    smsIndexer->SetTimeStamp(timeStamp);
    SendSmsToRil(smsIndexer);
}

void CdmaSmsSender::SetConcact(
    const std::shared_ptr<SmsSendIndexer> &smsIndexer, const std::unique_ptr<SmsTransMsg> &transMsg)
{
    if (smsIndexer->GetIsConcat()) {
        SmsConcat smsConcat = smsIndexer->GetSmsConcat();
        transMsg->data.p2pMsg.transTelesvcId = SMS_TRANS_TELESVC_WEMT;
        transMsg->data.p2pMsg.telesvcMsg.data.submit.msgId.headerInd = true;
        transMsg->data.p2pMsg.telesvcMsg.data.submit.userData.userData.headerCnt = 1;
        if (smsConcat.is8Bits) {
            transMsg->data.p2pMsg.telesvcMsg.data.submit.userData.userData.header[0].udhType = SMS_UDH_CONCAT_8BIT;
        } else {
            transMsg->data.p2pMsg.telesvcMsg.data.submit.userData.userData.header[0].udhType = SMS_UDH_CONCAT_16BIT;
        }
        transMsg->data.p2pMsg.telesvcMsg.data.submit.userData.userData.header[0].udh.concat8bit.msgRef =
            smsConcat.msgRef;
        transMsg->data.p2pMsg.telesvcMsg.data.submit.userData.userData.header[0].udh.concat8bit.totalSeg =
            smsConcat.totalSeg;
        transMsg->data.p2pMsg.telesvcMsg.data.submit.userData.userData.header[0].udh.concat8bit.seqNum =
            smsConcat.seqNum;
    }
}
} // namespace Telephony
} // namespace OHOS