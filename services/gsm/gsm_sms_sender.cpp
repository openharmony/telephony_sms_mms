/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

#include "gsm_sms_sender.h"

#include "core_manager_inner.h"
#include "msg_text_convert.h"
#include "radio_event.h"
#include "securec.h"
#include "sms_hisysevent.h"
#include "string_utils.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
using namespace std;
GsmSmsSender::GsmSmsSender(const std::shared_ptr<AppExecFwk::EventRunner> &runner, int32_t slotId,
    function<void(std::shared_ptr<SmsSendIndexer>)> sendRetryFun)
    : SmsSender(runner, slotId, sendRetryFun)
{}

GsmSmsSender::~GsmSmsSender() {}

void GsmSmsSender::Init()
{
    if (!RegisterHandler()) {
        TELEPHONY_LOGI("GsmSmsSender::Init Register RADIO_SMS_STATUS fail.");
    }
}

void GsmSmsSender::TextBasedSmsDelivery(const string &desAddr, const string &scAddr, const string &text,
    const sptr<ISendShortMessageCallback> &sendCallback,
    const sptr<IDeliveryShortMessageCallback> &deliveryCallback)
{
    bool isMore = false;
    int ret = 0;
    int headerCnt;
    unsigned char msgRef8bit;
    SmsCodingScheme codingType;
    GsmSmsMessage gsmSmsMessage;
    std::vector<struct SplitInfo> cellsInfos;
    gsmSmsMessage.SplitMessage(cellsInfos, text, CheckForce7BitEncodeType(), codingType, false);
    bool isStatusReport = (deliveryCallback == nullptr) ? false : true;
    std::shared_ptr<struct SmsTpdu> tpdu =
        gsmSmsMessage.CreateDefaultSubmitSmsTpdu(desAddr, scAddr, text, isStatusReport, codingType);
    if (tpdu == nullptr) {
        SendCallbackBecauseTpduNull(sendCallback, "TextBasedSmsDelivery");
        return;
    }
    int cellsInfosSize = static_cast<int>(cellsInfos.size());
    if (cellsInfosSize > MAX_SEGMENT_NUM) {
        SendResultCallBack(sendCallback, ISendShortMessageCallback::SEND_SMS_FAILURE_UNKNOWN);
        TELEPHONY_LOGE("message exceed the limit.");
        SmsHiSysEvent::WriteSmsSendFaultEvent(slotId_, SmsMmsMessageType::SMS_SHORT_MESSAGE,
            SmsMmsErrorCode::SMS_ERROR_EXCEED_MAX_SEGMENT_NUM, "text sms gsm message cellsInfosSize exceed the limit");
        return;
    }
    msgRef8bit = GetMsgRef8Bit();
    isStatusReport = tpdu->data.submit.bStatusReport;

    TELEPHONY_LOGI("TextBasedSmsDelivery isStatusReport= %{public}d", isStatusReport);
    shared_ptr<uint8_t> unSentCellCount = make_shared<uint8_t>(cellsInfosSize);
    shared_ptr<bool> hasCellFailed = make_shared<bool>(false);
    if (unSentCellCount == nullptr || hasCellFailed == nullptr) {
        SendResultCallBack(sendCallback, ISendShortMessageCallback::SEND_SMS_FAILURE_UNKNOWN);
        return;
    }
    std::unique_lock<std::mutex> lock(mutex_);
    for (int i = 0; i < cellsInfosSize; i++) {
        std::shared_ptr<SmsSendIndexer> indexer = nullptr;
        std::string segmentText;
        segmentText.append((char *)(cellsInfos[i].encodeData.data()), cellsInfos[i].encodeData.size());
        indexer = make_shared<SmsSendIndexer>(desAddr, scAddr, segmentText, sendCallback, deliveryCallback);
        if (indexer == nullptr) {
            SendResultCallBack(sendCallback, ISendShortMessageCallback::SEND_SMS_FAILURE_UNKNOWN);
            return;
        }
        indexer->SetDcs(cellsInfos[i].encodeType);
        headerCnt = 0;
        (void)memset_s(tpdu->data.submit.userData.data, MAX_USER_DATA_LEN + 1, 0x00, MAX_USER_DATA_LEN + 1);
        if (cellsInfos[i].encodeData.size() > MAX_USER_DATA_LEN + 1) {
            TELEPHONY_LOGE("TextBasedSmsDelivery data length invalid.");
            return;
        }
        ret = memcpy_s(tpdu->data.submit.userData.data, MAX_USER_DATA_LEN + 1, &cellsInfos[i].encodeData[0],
            cellsInfos[i].encodeData.size());
        if (ret != EOK) {
            SendResultCallBack(indexer, ISendShortMessageCallback::SEND_SMS_FAILURE_UNKNOWN);
            return;
        }
        tpdu->data.submit.userData.length = cellsInfos[i].encodeData.size();
        tpdu->data.submit.userData.data[cellsInfos[i].encodeData.size()] = 0;
        tpdu->data.submit.msgRef = msgRef8bit;
        if (cellsInfosSize > 1) {
            indexer->SetIsConcat(true);
            SmsConcat concat;
            concat.is8Bits = true;
            concat.msgRef = msgRef8bit;
            concat.totalSeg = static_cast<uint16_t>(cellsInfosSize);
            concat.seqNum = static_cast<uint16_t>(i + 1);
            indexer->SetSmsConcat(concat);
            headerCnt += gsmSmsMessage.SetHeaderConcat(headerCnt, concat);
        }
        /* Set User Data Header for Alternate Reply Address */
        headerCnt += gsmSmsMessage.SetHeaderReply(headerCnt);
        /* Set User Data Header for National Language Single Shift */
        headerCnt += gsmSmsMessage.SetHeaderLang(headerCnt, codingType, cellsInfos[i].langId);
        indexer->SetLangId(cellsInfos[i].langId);
        tpdu->data.submit.userData.headerCnt = headerCnt;
        tpdu->data.submit.bHeaderInd = (headerCnt > 0) ? true : false;

        if (cellsInfosSize > 1 && i < (cellsInfosSize - 1)) {
            tpdu->data.submit.bStatusReport = false;
            isMore = true;
        } else {
            tpdu->data.submit.bStatusReport = isStatusReport;
            isMore = false;
        }
        std::shared_ptr<struct EncodeInfo> encodeInfo = gsmSmsMessage.GetSubmitEncodeInfo(scAddr, isMore);
        if (encodeInfo == nullptr) {
            SendResultCallBack(indexer, ISendShortMessageCallback::SEND_SMS_FAILURE_UNKNOWN);
            TELEPHONY_LOGE("create encodeInfo encodeInfo nullptr error.");
            continue;
        }
        SetSendIndexerInfo(indexer, encodeInfo, msgRef8bit);
        indexer->SetUnSentCellCount(unSentCellCount);
        indexer->SetHasCellFailed(hasCellFailed);
        SendSmsToRil(indexer);
    }
}

void GsmSmsSender::DataBasedSmsDelivery(const std::string &desAddr, const std::string &scAddr, int32_t port,
    const uint8_t *data, uint32_t dataLen, const sptr<ISendShortMessageCallback> &sendCallback,
    const sptr<IDeliveryShortMessageCallback> &deliveryCallback)
{
    GsmSmsMessage gsmSmsMessage;
    std::vector<struct SplitInfo> cellsInfos;
    SmsCodingScheme codingType;
    std::string dataStr;
    CharArrayToString(data, dataLen, dataStr);

    gsmSmsMessage.SplitMessage(cellsInfos, dataStr, CheckForce7BitEncodeType(), codingType, true);
    uint8_t msgRef8bit = GetMsgRef8Bit();
    std::shared_ptr<struct SmsTpdu> tpdu = gsmSmsMessage.CreateDataSubmitSmsTpdu(
        desAddr, scAddr, port, data, dataLen, msgRef8bit, (deliveryCallback == nullptr) ? false : true);
    if (tpdu == nullptr) {
        SendCallbackBecauseTpduNull(sendCallback, "DataBasedSmsDelivery");
        return;
    }

    DataBasedSmsDeliverySplitPage(
        gsmSmsMessage, cellsInfos, tpdu, msgRef8bit, desAddr, scAddr, port, sendCallback, deliveryCallback);
}

void GsmSmsSender::DataBasedSmsDeliverySplitPage(GsmSmsMessage &gsmSmsMessage, std::vector<struct SplitInfo> cellsInfos,
    std::shared_ptr<struct SmsTpdu> tpdu, uint8_t msgRef8bit, const std::string &desAddr, const std::string &scAddr,
    int32_t port, const sptr<ISendShortMessageCallback> &sendCallback,
    const sptr<IDeliveryShortMessageCallback> &deliveryCallback)
{
    uint32_t cellsInfosSize = static_cast<uint32_t>(cellsInfos.size());
    for (uint32_t indexData = 0; indexData < cellsInfosSize; indexData++) {
        const uint8_t *dataItem = reinterpret_cast<uint8_t *>(cellsInfos[indexData].text.data());
        uint32_t dataItemLen = static_cast<uint32_t>(cellsInfos[indexData].text.size());

        std::shared_ptr<SmsSendIndexer> indexer =
            make_shared<SmsSendIndexer>(desAddr, scAddr, port, dataItem, dataItemLen, sendCallback, deliveryCallback);
        if (indexer == nullptr) {
            SendResultCallBack(indexer, ISendShortMessageCallback::SEND_SMS_FAILURE_UNKNOWN);
            TELEPHONY_LOGE("DataBasedSmsDeliverySplitPage create SmsSendIndexer nullptr");
            return;
        }

        (void)memset_s(tpdu->data.submit.userData.data, MAX_USER_DATA_LEN + 1, 0x00, MAX_USER_DATA_LEN + 1);
        if (cellsInfos[indexData].encodeData.size() > MAX_USER_DATA_LEN + 1) {
            TELEPHONY_LOGE("DataBasedSmsDeliverySplitPage data length invalid.");
            return;
        }
        int ret = memcpy_s(tpdu->data.submit.userData.data, MAX_USER_DATA_LEN + 1, &cellsInfos[indexData].encodeData[0],
            cellsInfos[indexData].encodeData.size());
        if (ret != EOK) {
            SendResultCallBack(indexer, ISendShortMessageCallback::SEND_SMS_FAILURE_UNKNOWN);
            TELEPHONY_LOGE("DataBasedSmsDeliverySplitPage ret:%{public}d", ret);
            return;
        }
        DataBasedSmsDeliveryPacketSplitPage(gsmSmsMessage, tpdu, msgRef8bit, indexData, port, scAddr, sendCallback,
            deliveryCallback, indexer, cellsInfos);
    }
}

void GsmSmsSender::DataBasedSmsDeliveryPacketSplitPage(GsmSmsMessage &gsmSmsMessage,
    std::shared_ptr<struct SmsTpdu> tpdu, uint8_t msgRef8bit, uint32_t indexData, int32_t port,
    const std::string &scAddr, const sptr<ISendShortMessageCallback> &sendCallback,
    const sptr<IDeliveryShortMessageCallback> &deliveryCallback, std::shared_ptr<SmsSendIndexer> indexer,
    std::vector<struct SplitInfo> cellsInfos)
{
    tpdu->data.submit.userData.length = static_cast<int>(cellsInfos[indexData].encodeData.size());
    tpdu->data.submit.userData.data[cellsInfos[indexData].encodeData.size()] = 0;
    tpdu->data.submit.msgRef = msgRef8bit;

    int headerCnt = 0;
    uint32_t cellsInfosSize = static_cast<uint32_t>(cellsInfos.size());
    if (cellsInfosSize > 1) {
        indexer->SetIsConcat(true);
        SmsConcat concat;
        concat.is8Bits = true;
        concat.msgRef = msgRef8bit;
        concat.totalSeg = static_cast<uint16_t>(cellsInfosSize);
        concat.seqNum = static_cast<uint16_t>(indexData + 1);
        indexer->SetSmsConcat(concat);
        headerCnt += gsmSmsMessage.SetHeaderConcat(headerCnt, concat);
    }

    if (headerCnt >= MAX_UD_HEADER_NUM) {
        TELEPHONY_LOGE("PDU header Array out of bounds");
        return;
    }

    tpdu->data.submit.userData.header[headerCnt].udhType = SMS_UDH_APP_PORT_16BIT;
    tpdu->data.submit.userData.header[headerCnt].udh.appPort16bit.destPort = ((unsigned short)port & 0xFFFF);
    tpdu->data.submit.userData.header[headerCnt].udh.appPort16bit.originPort = 0;
    headerCnt++;

    tpdu->data.submit.bHeaderInd = (headerCnt > 0) ? true : false;
    /* Set User Data Header for Alternate Reply Address */
    headerCnt += gsmSmsMessage.SetHeaderReply(headerCnt);
    /* Set User Data Header for National Language Single Shift */
    SmsCodingScheme pCodingType = SMS_CODING_7BIT;
    MSG_LANGUAGE_ID_T langId = MSG_ID_RESERVED_LANG;
    headerCnt += gsmSmsMessage.SetHeaderLang(headerCnt, pCodingType, langId);
    tpdu->data.submit.userData.headerCnt = headerCnt;

    std::shared_ptr<struct EncodeInfo> encodeInfo = gsmSmsMessage.GetSubmitEncodeInfo(scAddr, false);
    if (encodeInfo == nullptr) {
        SendResultCallBack(sendCallback, ISendShortMessageCallback::SEND_SMS_FAILURE_UNKNOWN);
        TELEPHONY_LOGE("DataBasedSmsDeliveryPacketSplitPage encodeInfo nullptr error.");
        SmsHiSysEvent::WriteSmsSendFaultEvent(slotId_, SmsMmsMessageType::SMS_SHORT_MESSAGE,
            SmsMmsErrorCode::SMS_ERROR_PDU_ENCODEING_FAIL, "data sms gsm encodeInfo nullptr error");
        return;
    }

    if (cellsInfosSize > 1 && indexData < (cellsInfosSize - 1)) {
        tpdu->data.submit.bStatusReport = false;
    } else {
        tpdu->data.submit.bStatusReport = (deliveryCallback == nullptr) ? false : true;
    }
    encodeInfo->isMore_ = (cellsInfosSize > 1) ? true : false;
    DataBasedSmsDeliverySendSplitPage(encodeInfo, sendCallback, indexer, msgRef8bit, cellsInfosSize);
}

void GsmSmsSender::CharArrayToString(const uint8_t *data, uint32_t dataLen, std::string &dataStr)
{
    uint32_t indexData = 0;
    while (indexData < dataLen) {
        dataStr += data[indexData];
        indexData++;
    }
}

void GsmSmsSender::SendCallbackBecauseTpduNull(const sptr<ISendShortMessageCallback> &sendCallback, std::string str)
{
    SendResultCallBack(sendCallback, ISendShortMessageCallback::SEND_SMS_FAILURE_UNKNOWN);
    TELEPHONY_LOGE("%{public}s tpdu nullptr error.", str.c_str());
    SmsHiSysEvent::WriteSmsSendFaultEvent(slotId_, SmsMmsMessageType::SMS_SHORT_MESSAGE,
        SmsMmsErrorCode::SMS_ERROR_PDU_ENCODEING_FAIL, "data sms gsm encodeInfo nullptr error");
}

void GsmSmsSender::DataBasedSmsDeliverySendSplitPage(std::shared_ptr<struct EncodeInfo> encodeInfo,
    const sptr<ISendShortMessageCallback> &sendCallback, shared_ptr<SmsSendIndexer> indexer, uint8_t msgRef8bit,
    uint32_t cellsInfosSize)
{
    std::vector<uint8_t> smca(encodeInfo->smcaData_, encodeInfo->smcaData_ + encodeInfo->smcaLen);
    std::vector<uint8_t> pdu(encodeInfo->tpduData_, encodeInfo->tpduData_ + encodeInfo->tpduLen);
    TELEPHONY_LOGE("DataBasedSmsDeliverySendSplitPage cellsInfosSize:%{public}d", cellsInfosSize);
    std::shared_ptr<uint8_t> unSentCellCount = make_shared<uint8_t>(cellsInfosSize);
    std::shared_ptr<bool> hasCellFailed = make_shared<bool>(false);
    if (unSentCellCount == nullptr || hasCellFailed == nullptr) {
        SendResultCallBack(sendCallback, ISendShortMessageCallback::SEND_SMS_FAILURE_UNKNOWN);
        return;
    }

    chrono::system_clock::duration timePoint = chrono::system_clock::now().time_since_epoch();
    long timeStamp = chrono::duration_cast<chrono::seconds>(timePoint).count();
    indexer->SetUnSentCellCount(unSentCellCount);
    indexer->SetHasCellFailed(hasCellFailed);
    indexer->SetEncodeSmca(std::move(smca));
    indexer->SetEncodePdu(std::move(pdu));
    indexer->SetHasMore(encodeInfo->isMore_);
    indexer->SetMsgRefId(msgRef8bit);
    indexer->SetNetWorkType(NET_TYPE_GSM);
    indexer->SetTimeStamp(timeStamp);
    std::unique_lock<std::mutex> lock(mutex_);
    SendSmsToRil(indexer);
}

void GsmSmsSender::SendSmsToRil(const shared_ptr<SmsSendIndexer> &smsIndexer)
{
    if (smsIndexer == nullptr) {
        SendResultCallBack(smsIndexer, ISendShortMessageCallback::SEND_SMS_FAILURE_UNKNOWN);
        TELEPHONY_LOGE("gsm_sms_sender: SendSms smsIndexer nullptr");
        return;
    }
    if (!isImsNetDomain_ && (voiceServiceState_ != static_cast<int32_t>(RegServiceState::REG_STATE_IN_SERVICE))) {
        SendResultCallBack(smsIndexer, ISendShortMessageCallback::SEND_SMS_FAILURE_SERVICE_UNAVAILABLE);
        TELEPHONY_LOGE("gsm_sms_sender: SendSms not in service");
        SmsHiSysEvent::WriteSmsSendFaultEvent(slotId_, SmsMmsMessageType::SMS_SHORT_MESSAGE,
            SmsMmsErrorCode::SMS_ERROR_SENDSMS_NOT_IN_SERVICE, "gsm send sms not in service");
        return;
    }
    int64_t refId = GetMsgRef64Bit();
    if (!SendCacheMapAddItem(refId, smsIndexer)) {
        TELEPHONY_LOGE("SendCacheMapAddItem Error!!");
        return;
    }

    GsmSimMessageParam smsData;
    smsData.refId = refId;
    smsData.smscPdu = StringUtils::StringToHex(smsIndexer->GetEncodeSmca());
    smsData.pdu = StringUtils::StringToHex(smsIndexer->GetEncodePdu());
    bool sendCsSMS = false;
    if ((!isImsNetDomain_ || !imsSmsCfg_) || (smsIndexer->GetPsResendCount() == MAX_SEND_RETRIES)) {
        sendCsSMS = true;
    }
    if (sendCsSMS) {
        SendCsSms(smsIndexer, smsData);
    } else {
        SendImsSms(smsIndexer, smsData);
    }
}

void GsmSmsSender::SendCsSms(const shared_ptr<SmsSendIndexer> &smsIndexer, GsmSimMessageParam smsData)
{
    uint8_t tryCount = smsIndexer->GetCsResendCount();
    lastSmsDomain_ = CS_DOMAIN;
    if (tryCount > 0) {
        smsIndexer->UpdatePduForResend();
    }
    if (tryCount == 0 && smsIndexer->GetHasMore()) {
        TELEPHONY_LOGI("SendSmsMoreMode pdu len = %{public}zu", smsIndexer->GetEncodePdu().size());
        CoreManagerInner::GetInstance().SendSmsMoreMode(
            slotId_, RadioEvent::RADIO_SEND_SMS_EXPECT_MORE, smsData, shared_from_this());
    } else {
        TELEPHONY_LOGI("SendSms pdu len = %{public}zu", smsIndexer->GetEncodePdu().size());
        CoreManagerInner::GetInstance().SendGsmSms(slotId_, RadioEvent::RADIO_SEND_SMS, smsData, shared_from_this());
    }
}

void GsmSmsSender::SendImsSms(const shared_ptr<SmsSendIndexer> &smsIndexer, GsmSimMessageParam smsData)
{
    auto smsClient = DelayedSingleton<ImsSmsClient>::GetInstance();
    TELEPHONY_LOGI("ims network domain send sms interface.!");
    if (smsClient == nullptr) {
        TELEPHONY_LOGE("SendSmsToRil return, ImsSmsClient is nullptr.");
        return;
    }
    lastSmsDomain_ = IMS_DOMAIN;
    ImsMessageInfo imsMessageInfo;
    imsMessageInfo.refId = smsData.refId;
    imsMessageInfo.smscPdu = smsData.smscPdu;
    imsMessageInfo.pdu = smsData.pdu;
    imsMessageInfo.tech = SMS_RADIO_TECH_3GPP;
    int32_t reply = smsClient->ImsSendMessage(slotId_, imsMessageInfo);
    TELEPHONY_LOGI("SendImsSms reply = %{public}d", reply);
}

int32_t GsmSmsSender::IsImsSmsSupported(int32_t slotId, bool &isSupported)
{
    auto smsClient = DelayedSingleton<ImsSmsClient>::GetInstance();
    if (smsClient == nullptr) {
        TELEPHONY_LOGE("GetImsSmsConfig return, ImsSmsClient is nullptr.");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    std::unique_lock<std::mutex> lck(ctx_);
    resIsSmsReady_ = false;
    int32_t reply = smsClient->ImsGetSmsConfig(slotId);
    TELEPHONY_LOGI("GetImsSms reply = %{public}d", reply);
    while (resIsSmsReady_) {
        TELEPHONY_LOGI("GetImsSmsConfig::wait(), resIsSmsReady_ = false");
        if (cv_.wait_for(lck, std::chrono::seconds(WAIT_TIME_SECOND)) == std::cv_status::timeout) {
            break;
        }
    }
    TELEPHONY_LOGI("GsmSmsSender::IsImsSmsSupported() imsSmsCfg_:%{public}d", imsSmsCfg_);
    isSupported = (imsSmsCfg_ == IMS_SMS_ENABLE);
    return TELEPHONY_ERR_SUCCESS;
}

void GsmSmsSender::StatusReportAnalysis(const AppExecFwk::InnerEvent::Pointer &event)
{
    if (event == nullptr) {
        TELEPHONY_LOGE("GsmSmsSender: StatusReportAnalysis event nullptr error.");
        return;
    }

    std::shared_ptr<SmsMessageInfo> statusInfo = event->GetSharedObject<SmsMessageInfo>();
    if (statusInfo == nullptr) {
        TELEPHONY_LOGE("GsmSmsSender: StatusReportAnalysis statusInfo nullptr error.");
        return;
    }

    std::string pdu = StringUtils::StringToHex(statusInfo->pdu);
    TELEPHONY_LOGI("StatusReport pdu size == %{public}zu", pdu.length());
    std::shared_ptr<GsmSmsMessage> message = GsmSmsMessage::CreateMessage(pdu);
    if (message == nullptr) {
        TELEPHONY_LOGE("GsmSmsSender: StatusReportAnalysis message nullptr error.");
        return;
    }
    sptr<IDeliveryShortMessageCallback> deliveryCallback = nullptr;
    auto oldIndexer = reportList_.begin();
    while (oldIndexer != reportList_.end()) {
        auto iter = oldIndexer++;
        if (*iter != nullptr && (message->GetMsgRef() == (*iter)->GetMsgRefId())) {
            // save the message to db, or updata to db msg state(success or failed)
            TELEPHONY_LOGI("StatusReport %{public}d %{public}d", message->GetMsgRef(), (*iter)->GetMsgRefId());
            deliveryCallback = (*iter)->GetDeliveryCallback();
            reportList_.erase(iter);
        }
    }
    CoreManagerInner::GetInstance().SendSmsAck(slotId_, 0, AckIncomeCause::SMS_ACK_PROCESSED, true, nullptr);
    if (deliveryCallback != nullptr) {
        std::string ackpdu = StringUtils::StringToHex(message->GetRawPdu());
        deliveryCallback->OnSmsDeliveryResult(StringUtils::ToUtf16(ackpdu));
    }
}

void GsmSmsSender::StatusReportSetImsSms(const AppExecFwk::InnerEvent::Pointer &event)
{
    if (event == nullptr) {
        TELEPHONY_LOGE("GsmSmsSender: StatusReportSetImsSms event nullptr error.");
        return;
    }
    std::shared_ptr<HRilRadioResponseInfo> imsResponseInfo = event->GetSharedObject<HRilRadioResponseInfo>();

    if (imsResponseInfo->error != HRilErrType::NONE) {
        imsSmsCfg_ = IMS_SMS_DISABLE;
    }
}

void GsmSmsSender::StatusReportGetImsSms(const AppExecFwk::InnerEvent::Pointer &event)
{
    if (event == nullptr) {
        TELEPHONY_LOGE("GsmSmsSender: StatusReportGetImsSms event nullptr error.");
        return;
    }
    std::shared_ptr<int32_t> imsSmsInfo = event->GetSharedObject<int32_t>();
    if (imsSmsInfo == nullptr) {
        TELEPHONY_LOGE("GsmSmsSender: StatusReportGetImsSms imsSmsInfo nullptr error.");
        return;
    }
    imsSmsCfg_ = *imsSmsInfo;
}

void GsmSmsSender::SetSendIndexerInfo(const std::shared_ptr<SmsSendIndexer> &indexer,
    const std::shared_ptr<struct EncodeInfo> &encodeInfo, unsigned char msgRef8bit)
{
    if (encodeInfo == nullptr || indexer == nullptr) {
        TELEPHONY_LOGE("CreateSendIndexer encodeInfo nullptr");
        return;
    }

    std::vector<uint8_t> smca(encodeInfo->smcaData_, encodeInfo->smcaData_ + encodeInfo->smcaLen);
    std::vector<uint8_t> pdu(encodeInfo->tpduData_, encodeInfo->tpduData_ + encodeInfo->tpduLen);
    chrono::system_clock::duration timePoint = chrono::system_clock::now().time_since_epoch();
    long timeStamp = chrono::duration_cast<chrono::seconds>(timePoint).count();
    indexer->SetTimeStamp(timeStamp);
    indexer->SetEncodeSmca(std::move(smca));
    indexer->SetEncodePdu(std::move(pdu));
    indexer->SetHasMore(encodeInfo->isMore_);
    indexer->SetMsgRefId(msgRef8bit);
    indexer->SetNetWorkType(NET_TYPE_GSM);
}

bool GsmSmsSender::RegisterHandler()
{
    CoreManagerInner::GetInstance().RegisterCoreNotify(
        slotId_, shared_from_this(), RadioEvent::RADIO_SMS_STATUS, nullptr);
    return true;
}

void GsmSmsSender::RegisterImsHandler()
{
    if (isImsGsmHandlerRegistered) {
        return;
    }
    auto smsClient = DelayedSingleton<ImsSmsClient>::GetInstance();
    if (smsClient == nullptr) {
        TELEPHONY_LOGE("RegisterHandler return, ImsSmsClient is nullptr.");
        return;
    }

    smsClient->RegisterImsSmsCallbackHandler(slotId_, shared_from_this());
    TELEPHONY_LOGE("RegisterHandler  gsm ImsSmsClient successs");
    isImsGsmHandlerRegistered = true;
}

void GsmSmsSender::ResendTextDelivery(const std::shared_ptr<SmsSendIndexer> &smsIndexer)
{
    if (smsIndexer == nullptr) {
        TELEPHONY_LOGE("ResendTextDelivery smsIndexer == nullptr");
        return;
    }
    GsmSmsMessage gsmSmsMessage;
    bool isMore = false;
    if (!SetPduInfo(smsIndexer, gsmSmsMessage, isMore)) {
        TELEPHONY_LOGE("SetPduInfo fail.");
        return;
    }

    std::shared_ptr<struct EncodeInfo> encodeInfo = nullptr;
    encodeInfo = gsmSmsMessage.GetSubmitEncodeInfo(smsIndexer->GetSmcaAddr(), isMore);
    if (encodeInfo == nullptr) {
        SendResultCallBack(smsIndexer, ISendShortMessageCallback::SEND_SMS_FAILURE_UNKNOWN);
        TELEPHONY_LOGE("create encodeInfo indexer == nullptr\r\n");
        return;
    }
    SetSendIndexerInfo(smsIndexer, encodeInfo, smsIndexer->GetMsgRefId());
    std::unique_lock<std::mutex> lock(mutex_);
    SendSmsToRil(smsIndexer);
}

void GsmSmsSender::ResendDataDelivery(const std::shared_ptr<SmsSendIndexer> &smsIndexer)
{
    if (smsIndexer == nullptr) {
        TELEPHONY_LOGE("DataBasedSmsDelivery ResendDataDelivery smsIndexer nullptr");
        return;
    }

    bool isStatusReport = false;
    unsigned char msgRef8bit = 0;
    msgRef8bit = smsIndexer->GetMsgRefId();
    isStatusReport = (smsIndexer->GetDeliveryCallback() == nullptr) ? false : true;

    GsmSmsMessage gsmSmsMessage;
    std::shared_ptr<struct SmsTpdu> tpdu = nullptr;
    tpdu = gsmSmsMessage.CreateDataSubmitSmsTpdu(smsIndexer->GetDestAddr(), smsIndexer->GetSmcaAddr(),
        smsIndexer->GetDestPort(), smsIndexer->GetData().data(), smsIndexer->GetData().size(), msgRef8bit,
        isStatusReport);

    std::shared_ptr<struct EncodeInfo> encodeInfo = nullptr;
    encodeInfo = gsmSmsMessage.GetSubmitEncodeInfo(smsIndexer->GetSmcaAddr(), false);
    if (encodeInfo == nullptr || tpdu == nullptr) {
        SendResultCallBack(smsIndexer, ISendShortMessageCallback::SEND_SMS_FAILURE_UNKNOWN);
        TELEPHONY_LOGE("DataBasedSmsDelivery encodeInfo or tpdu nullptr");
        return;
    }
    std::vector<uint8_t> smca(encodeInfo->smcaData_, encodeInfo->smcaData_ + encodeInfo->smcaLen);
    std::vector<uint8_t> pdu(encodeInfo->tpduData_, encodeInfo->tpduData_ + encodeInfo->tpduLen);
    std::shared_ptr<uint8_t> unSentCellCount = make_shared<uint8_t>(1);
    if (unSentCellCount == nullptr) {
        SendResultCallBack(smsIndexer, ISendShortMessageCallback::SEND_SMS_FAILURE_UNKNOWN);
        TELEPHONY_LOGE("DataBasedSmsDelivery unSentCellCount nullptr");
        return;
    }
    std::shared_ptr<bool> hasCellFailed = make_shared<bool>(false);
    if (hasCellFailed == nullptr) {
        SendResultCallBack(smsIndexer, ISendShortMessageCallback::SEND_SMS_FAILURE_UNKNOWN);
        TELEPHONY_LOGE("DataBasedSmsDelivery hasCellFailed nullptr");
        return;
    }
    chrono::system_clock::duration timePoint = chrono::system_clock::now().time_since_epoch();
    long timeStamp = chrono::duration_cast<chrono::seconds>(timePoint).count();

    smsIndexer->SetUnSentCellCount(unSentCellCount);
    smsIndexer->SetHasCellFailed(hasCellFailed);
    smsIndexer->SetEncodeSmca(std::move(smca));
    smsIndexer->SetEncodePdu(std::move(pdu));
    smsIndexer->SetHasMore(encodeInfo->isMore_);
    smsIndexer->SetMsgRefId(msgRef8bit);
    smsIndexer->SetNetWorkType(NET_TYPE_GSM);
    smsIndexer->SetTimeStamp(timeStamp);
    std::unique_lock<std::mutex> lock(mutex_);
    SendSmsToRil(smsIndexer);
}

bool GsmSmsSender::SetPduInfo(
    const std::shared_ptr<SmsSendIndexer> &smsIndexer, GsmSmsMessage &gsmSmsMessage, bool &isMore)
{
    bool ret = false;
    if (smsIndexer == nullptr) {
        TELEPHONY_LOGE("Indexer is nullptr");
        return ret;
    }
    SmsCodingScheme codingType = smsIndexer->GetDcs();
    std::shared_ptr<struct SmsTpdu> tpdu = nullptr;
    tpdu = gsmSmsMessage.CreateDefaultSubmitSmsTpdu(smsIndexer->GetDestAddr(), smsIndexer->GetSmcaAddr(),
        smsIndexer->GetText(), (smsIndexer->GetDeliveryCallback() == nullptr) ? false : true, codingType);
    if (tpdu == nullptr) {
        SendResultCallBack(smsIndexer, ISendShortMessageCallback::SEND_SMS_FAILURE_UNKNOWN);
        TELEPHONY_LOGE("ResendTextDelivery CreateDefaultSubmitSmsTpdu err.");
        return ret;
    }
    (void)memset_s(tpdu->data.submit.userData.data, MAX_USER_DATA_LEN + 1, 0x00, MAX_USER_DATA_LEN + 1);
    if (smsIndexer->GetText().length() > MAX_USER_DATA_LEN + 1) {
        TELEPHONY_LOGE("SetPduInfo data length invalid.");
        return ret;
    }
    if (memcpy_s(tpdu->data.submit.userData.data, MAX_USER_DATA_LEN + 1, smsIndexer->GetText().c_str(),
        smsIndexer->GetText().length()) != EOK) {
        SendResultCallBack(smsIndexer, ISendShortMessageCallback::SEND_SMS_FAILURE_UNKNOWN);
        TELEPHONY_LOGE("TextBasedSmsDelivery memcpy_s error.");
        return ret;
    }
    int headerCnt = 0;
    tpdu->data.submit.userData.length = static_cast<int>(smsIndexer->GetText().length());
    tpdu->data.submit.userData.data[smsIndexer->GetText().length()] = 0;
    tpdu->data.submit.msgRef = smsIndexer->GetMsgRefId();
    if (smsIndexer->GetIsConcat()) {
        headerCnt += gsmSmsMessage.SetHeaderConcat(headerCnt, smsIndexer->GetSmsConcat());
    }
    /* Set User Data Header for Alternate Reply Address */
    headerCnt += gsmSmsMessage.SetHeaderReply(headerCnt);
    /* Set User Data Header for National Language Single Shift */
    headerCnt += gsmSmsMessage.SetHeaderLang(headerCnt, codingType, smsIndexer->GetLangId());
    tpdu->data.submit.userData.headerCnt = headerCnt;
    tpdu->data.submit.bHeaderInd = (headerCnt > 0) ? true : false;

    unsigned char totalSeg = smsIndexer->GetSmsConcat().totalSeg;
    if ((totalSeg > 1) && (smsIndexer->GetSmsConcat().seqNum < totalSeg)) {
        tpdu->data.submit.bStatusReport = false;
        isMore = true;
    } else {
        tpdu->data.submit.bStatusReport = (smsIndexer->GetDeliveryCallback() == nullptr) ? false : true;
        isMore = false;
    }
    return true;
}
} // namespace Telephony
} // namespace OHOS