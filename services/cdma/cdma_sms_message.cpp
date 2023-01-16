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

#include "cdma_sms_message.h"

#include "msg_text_convert.h"
#include "securec.h"
#include "sms_common_utils.h"
#include "string_utils.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
static constexpr uint16_t CDMA_MAX_UD_HEADER_NUM = 7;

std::unique_ptr<SmsTransMsg> CdmaSmsMessage::CreateSubmitTransMsg(const std::string &dest, const std::string &sc,
    const std::string &text, bool bStatusReport, const SmsCodingScheme codingScheme)
{
    std::unique_ptr<SmsTransMsg> transMsg = GreateTransMsg();
    if (transMsg == nullptr) {
        TELEPHONY_LOGE("CreateMessage message transMsg nullptr");
        return nullptr;
    }
    scAddress_ = sc;
    originatingAddress_ = dest;
    visibleMessageBody_ = text;
    bStatusReportMessage_ = bStatusReport;
    transMsg->data.p2pMsg.telesvcMsg.data.submit.userData.encodeType = CovertEncodingType(codingScheme);
    /* Set Reply option */
    transMsg->data.p2pMsg.telesvcMsg.data.submit.replyOpt.deliverAckReq = bStatusReport;
    /* Convert Address values */
    transMsg->data.p2pMsg.address.digitMode = SMS_DIGIT_4BIT_DTMF;
    transMsg->data.p2pMsg.address.numberMode = SMS_NUMBER_MODE_NONE_DATANETWORK;
    transMsg->data.p2pMsg.address.numberPlan = SMS_NPI_UNKNOWN;
    transMsg->data.p2pMsg.address.addrLen = dest.length();
    if (strncpy_s(transMsg->data.p2pMsg.address.szData, sizeof(transMsg->data.p2pMsg.address.szData), dest.c_str(),
            dest.length()) != EOK) {
        transMsg->data.p2pMsg.address.addrLen = sizeof(transMsg->data.p2pMsg.address.szData) - 1;
        transMsg->data.p2pMsg.address.szData[transMsg->data.p2pMsg.address.addrLen] = '\0';
    }
    if (dest.at(0) == '+') {
        transMsg->data.p2pMsg.address.digitMode = SMS_DIGIT_8BIT;
        transMsg->data.p2pMsg.address.numberType = SMS_NUMBER_TYPE_INTERNATIONAL;
    } else {
        transMsg->data.p2pMsg.address.numberType = SMS_NUMBER_TYPE_NATIONAL;
    }
    return transMsg;
}

std::unique_ptr<SmsTransMsg> CdmaSmsMessage::CreateSubmitTransMsg(const std::string &dest, const std::string &sc,
    int32_t port, const uint8_t *data, uint32_t dataLen, bool bStatusReport)
{
    std::unique_ptr<SmsTransMsg> transMsg = GreateTransMsg();
    if (transMsg == nullptr) {
        TELEPHONY_LOGE("CreateMessage message transMsg nullptr");
        return nullptr;
    }

    scAddress_ = sc;
    destPort_ = static_cast<uint16_t>(port);
    originatingAddress_ = dest;
    bStatusReportMessage_ = bStatusReport;
    /* Set Reply option */
    transMsg->data.p2pMsg.telesvcMsg.data.submit.replyOpt.deliverAckReq = bStatusReport;
    /* Convert Address values */
    transMsg->data.p2pMsg.address.digitMode = SMS_DIGIT_4BIT_DTMF;
    transMsg->data.p2pMsg.address.numberMode = SMS_NUMBER_MODE_NONE_DATANETWORK;
    transMsg->data.p2pMsg.address.numberPlan = SMS_NPI_UNKNOWN;
    transMsg->data.p2pMsg.address.addrLen = dest.length();
    if (dest.length() > SMS_TRANS_ADDRESS_MAX_LEN + 1) {
        TELEPHONY_LOGE("CreateSubmitTransMsg data length invalid.");
        return nullptr;
    }
    if (strncpy_s(transMsg->data.p2pMsg.address.szData, sizeof(transMsg->data.p2pMsg.address.szData), dest.c_str(),
            dest.length()) != EOK) {
        transMsg->data.p2pMsg.address.addrLen = sizeof(transMsg->data.p2pMsg.address.szData) - 1;
        transMsg->data.p2pMsg.address.szData[transMsg->data.p2pMsg.address.addrLen] = '\0';
    }
    if (dest.at(0) == '+') {
        transMsg->data.p2pMsg.address.digitMode = SMS_DIGIT_8BIT;
        transMsg->data.p2pMsg.address.numberType = SMS_NUMBER_TYPE_INTERNATIONAL;
    } else {
        transMsg->data.p2pMsg.address.numberType = SMS_NUMBER_TYPE_NATIONAL;
    }
    return transMsg;
}

std::unique_ptr<struct SmsTransMsg> CdmaSmsMessage::GreateTransMsg()
{
    std::unique_ptr<SmsTransMsg> transMsg = std::make_unique<SmsTransMsg>();
    if (transMsg == nullptr) {
        TELEPHONY_LOGE("CreateMessage message transMsg nullptr");
        return nullptr;
    }
    (void)memset_s(transMsg.get(), sizeof(SmsTransMsg), 0x00, sizeof(SmsTransMsg));
    transMsg->type = (SmsTransMsgType)SMS_TRANS_P2P_MSG;
    transMsg->data.p2pMsg.telesvcMsg.type = (SmsMessageType)SMS_TYPE_SUBMIT;
    /* 1. Set Teleservice ID */
    transMsg->data.p2pMsg.transTelesvcId = SMS_TRANS_TELESVC_CMT_95;
    /* 2. Set Service category */
    transMsg->data.p2pMsg.transSvcCtg = SMS_TRANS_SVC_CTG_UNDEFINED;

    /* 3. Set Valid period */
    transMsg->data.p2pMsg.telesvcMsg.data.submit.valPeriod.format = SMS_TIME_RELATIVE;
    transMsg->data.p2pMsg.telesvcMsg.data.submit.valPeriod.time.relTime.time = SMS_REL_TIME_INDEFINITE;
    /* 4. Set Priority */
    transMsg->data.p2pMsg.telesvcMsg.data.submit.priority = SMS_PRIORITY_NORMAL;
    /* 5. Set Privacy */
    transMsg->data.p2pMsg.telesvcMsg.data.submit.privacy = SMS_PRIVACY_NOT_RESTRICTED;
    /* 6. Set Alert priority */
    transMsg->data.p2pMsg.telesvcMsg.data.submit.alertPriority = SMS_ALERT_MOBILE_DEFAULT;
    /* 7. Set Language */
    transMsg->data.p2pMsg.telesvcMsg.data.submit.language = SMS_LAN_UNKNOWN;
    return transMsg;
}

SmsEncodingType CdmaSmsMessage::CovertEncodingType(const SmsCodingScheme &codingScheme)
{
    SmsEncodingType encodingType = SMS_ENCODE_7BIT_ASCII;
    switch (codingScheme) {
        case SMS_CODING_7BIT:
            encodingType = SMS_ENCODE_GSM7BIT;
            break;
        case SMS_CODING_ASCII7BIT:
            encodingType = SMS_ENCODE_7BIT_ASCII;
            break;
        case SMS_CODING_8BIT:
            encodingType = SMS_ENCODE_OCTET;
            break;
        case SMS_CODING_UCS2:
        default:
            encodingType = SMS_ENCODE_UNICODE;
            break;
    }
    return encodingType;
}

std::shared_ptr<CdmaSmsMessage> CdmaSmsMessage::CreateMessage(const std::string &pdu)
{
    std::shared_ptr<CdmaSmsMessage> message = std::make_shared<CdmaSmsMessage>();
    if (message == nullptr) {
        TELEPHONY_LOGE("CreateMessage message nullptr");
        return nullptr;
    }
    message->transMsg_ = std::make_unique<struct SmsTransMsg>();
    if (message->transMsg_ == nullptr) {
        TELEPHONY_LOGE("CreateMessage message transMsg_ nullptr");
        return nullptr;
    }

    (void)memset_s(message->transMsg_.get(), sizeof(struct SmsTransMsg), 0x00, sizeof(struct SmsTransMsg));
    if (message->PduAnalysis(pdu)) {
        return message;
    }
    return nullptr;
}

bool CdmaSmsMessage::PduAnalysis(const std::string &pduHex)
{
    if (transMsg_ == nullptr || pduHex.empty()) {
        TELEPHONY_LOGE("PduAnalysis is unInvalid param!");
        return false;
    }

    rawPdu_ = StringUtils::HexToByteVector(pduHex);
    if (!CdmaSmsPduCodec::CheckInvalidPDU(rawPdu_)) {
        TELEPHONY_LOGE("PduAnalysis is unInvalid pdu data!");
        return false;
    }

    int decodeLen = 0;
    std::string pdu = StringUtils::HexToString(pduHex);
    decodeLen =
        CdmaSmsPduCodec::DecodeMsg(reinterpret_cast<const unsigned char *>(pdu.c_str()), pdu.length(), *transMsg_);
    if (decodeLen <= 0) {
        TELEPHONY_LOGE("Pdu DecodeMsg has failure.");
        return false;
    }
    if (transMsg_->type == SMS_TRANS_BROADCAST_MSG) {
        if (transMsg_->data.cbMsg.telesvcMsg.data.deliver.cmasData.isWrongRecodeType) {
            TELEPHONY_LOGE("Invalid CMAS Record Type");
            return false;
        }
        SmsEncodingType encodeType = transMsg_->data.cbMsg.telesvcMsg.data.deliver.cmasData.encodeType;
        if ((encodeType == SMS_ENCODE_KOREAN) || (encodeType == SMS_ENCODE_GSMDCS)) {
            TELEPHONY_LOGE("This encode type is not supported [%{public}d]", encodeType);
            return false;
        }
    }
    switch (transMsg_->type) {
        case SMS_TRANS_P2P_MSG:
            AnalysisP2pMsg(transMsg_->data.p2pMsg);
            break;
        case SMS_TRANS_BROADCAST_MSG:
            AnalysisCbMsg(transMsg_->data.cbMsg);
            break;
        case SMS_TRANS_ACK_MSG:
            AnalsisAckMsg(transMsg_->data.ackMsg);
            break;
        default:
            return false;
    }
    return true;
}

void CdmaSmsMessage::AnalysisP2pMsg(const SmsTransP2PMsg &p2pMsg)
{
    if (p2pMsg.transTelesvcId == SMS_TRANS_TELESVC_RESERVED) {
        TELEPHONY_LOGE("this Incoming Message has Unknown Teleservice ID");
        return;
    }

    address_ = std::make_unique<struct SmsTransAddr>();
    if (address_ == nullptr) {
        TELEPHONY_LOGE("AnalysisP2pMsg make address == nullptr");
        return;
    }

    if (memcpy_s(address_.get(), sizeof(SmsTransAddr), &p2pMsg.address, sizeof(SmsTransAddr)) != EOK) {
        TELEPHONY_LOGE("AnalysisP2pMsg address memcpy_s error.");
        return;
    }

    originatingAddress_ = address_->szData;
    switch (p2pMsg.telesvcMsg.type) {
        case SmsMessageType::SMS_TYPE_DELIVER:
            AnalsisDeliverMwi(p2pMsg);
            AnalsisDeliverMsg(p2pMsg.telesvcMsg.data.deliver);
            break;
        case SmsMessageType::SMS_TYPE_DELIVERY_ACK:
            AnalsisDeliverAck(p2pMsg.telesvcMsg.data.deliveryAck);
            break;
        case SmsMessageType::SMS_TYPE_USER_ACK:
        case SmsMessageType::SMS_TYPE_READ_ACK:
            break;
        case SmsMessageType::SMS_TYPE_SUBMIT_REPORT:
            AnalsisSubmitReport(p2pMsg.telesvcMsg.data.report);
            break;
        case SmsMessageType::SMS_TYPE_SUBMIT:
            AnalsisSubmitMsg(p2pMsg.telesvcMsg.data.submit);
            break;
        default:
            TELEPHONY_LOGI("AnalysisP2pMsg unkown type =%{public}d", p2pMsg.telesvcMsg.type);
            break;
    }
}

void CdmaSmsMessage::AnalsisDeliverMwi(const SmsTransP2PMsg &p2pMsg)
{
    specialSmsInd_ = nullptr;
    if (p2pMsg.transTelesvcId == SMS_TRANS_TELESVC_VMN_95) {
        specialSmsInd_ = std::make_shared<SpecialSmsIndication>();
        if (specialSmsInd_ == nullptr) {
            TELEPHONY_LOGE("SpecialSmsIndication is null!");
            return;
        }

        specialSmsInd_->msgInd = SMS_VOICE_INDICATOR;
        if (p2pMsg.telesvcMsg.data.deliver.enhancedVmn.faxIncluded) {
            specialSmsInd_->msgInd = SMS_FAX_INDICATOR;
        }

        if (p2pMsg.telesvcMsg.data.deliver.numMsg < 0) {
            specialSmsInd_->waitMsgNum = 0;
            bMwiClear_ = true;
            bMwiSet_ = false;
        } else {
            specialSmsInd_->waitMsgNum = p2pMsg.telesvcMsg.data.deliver.numMsg;
            bMwiClear_ = false;
            bMwiSet_ = true;
        }

        bMwiNotStore_ = false;
        if (bMwiSet_ && (p2pMsg.telesvcMsg.data.deliver.userData.userData.length == 0)) {
            bMwiNotStore_ = true;
        }
    }
}

void CdmaSmsMessage::AnalsisDeliverMsg(const SmsTeleSvcDeliver &deliver)
{
    isCmas_ = false;
    msgClass_ = SMS_CLASS_UNKNOWN;
    if (deliver.displayMode == SMS_DISPLAY_IMMEDIATE) {
        msgClass_ = SMS_INSTANT_MESSAGE;
    }

    msgRef_ = deliver.msgId.msgId;
    bHeaderInd_ = deliver.msgId.headerInd;
    scTimestamp_ = SmsCommonUtils::ConvertTime(deliver.timeStamp);
    AnalsisUserData(deliver.userData);
}

void CdmaSmsMessage::AnalsisDeliverAck(const SmsTeleSvcDeliverAck &deliverAck)
{
    bStatusReportMessage_ = true;
    scTimestamp_ = SmsCommonUtils::ConvertTime(deliverAck.timeStamp);
    AnalsisUserData(deliverAck.userData);
}

void CdmaSmsMessage::AnalsisSubmitReport(const SmsTeleSvcDeliverReport &report)
{
    AnalsisUserData(report.userData);
}

void CdmaSmsMessage::AnalsisSubmitMsg(const SmsTeleSvcSubmit &submit)
{
    msgRef_ = submit.msgId.msgId;
    bHeaderInd_ = submit.msgId.headerInd;
    scTimestamp_ = SmsCommonUtils::ConvertTime(submit.valPeriod.time.absTime);
    AnalsisUserData(submit.userData);
}

void CdmaSmsMessage::AnalsisUserData(const SmsTeleSvcUserData &userData)
{
    int dataSize = 0;
    MsgLangInfo langinfo = {
        0,
    };
    AnalsisHeader(userData);
    MsgTextConvert *textCvt = MsgTextConvert::Instance();
    if (textCvt == nullptr) {
        return;
    }
    unsigned char buff[MAX_MSG_TEXT_LEN + 1] = { 0 };
    switch (userData.encodeType) {
        case SMS_ENCODE_GSM7BIT: {
            dataSize = textCvt->ConvertGSM7bitToUTF8(
                buff, MAX_MSG_TEXT_LEN, (unsigned char *)&userData.userData, userData.userData.length, &langinfo);
            break;
        }
        case SMS_ENCODE_KOREAN:
        case SMS_ENCODE_EUCKR: {
            dataSize = textCvt->ConvertEUCKRToUTF8(
                buff, MAX_MSG_TEXT_LEN, (unsigned char *)&userData.userData, userData.userData.length);
            break;
        }
        case SMS_ENCODE_IA5:
        case SMS_ENCODE_7BIT_ASCII:
        case SMS_ENCODE_LATIN_HEBREW:
        case SMS_ENCODE_LATIN:
        case SMS_ENCODE_OCTET: {
            if (memcpy_s(buff, sizeof(buff), userData.userData.data, userData.userData.length) != EOK) {
                TELEPHONY_LOGE("AnalsisDeliverMsg memcpy_s fail.");
                return;
            }
            dataSize = userData.userData.length;
            buff[dataSize] = '\0';
            break;
        }
        case SMS_ENCODE_SHIFT_JIS: {
            dataSize = textCvt->ConvertSHIFTJISToUTF8(
                buff, MAX_MSG_TEXT_LEN, (unsigned char *)&userData.userData.data, userData.userData.length);
            break;
        }
        default: {
            dataSize = textCvt->ConvertUCS2ToUTF8(
                buff, MAX_MSG_TEXT_LEN, (unsigned char *)&userData.userData.data, userData.userData.length);
            break;
        }
    }
    visibleMessageBody_.insert(0, reinterpret_cast<char *>(buff), dataSize);
    TELEPHONY_LOGI("AnalsisDeliverMsg userData == %{private}s", visibleMessageBody_.c_str());
}

void CdmaSmsMessage::AnalsisCMASMsg(const SmsTeleSvcDeliver &deliver)
{
    isCmas_ = true;
    category_ = deliver.cmasData.category;
    responseType_ = deliver.cmasData.responseType;
    severity_ = deliver.cmasData.severity;
    urgency_ = deliver.cmasData.urgency;
    certainty_ = deliver.cmasData.certainty;
    messageClass_ = deliver.cmasData.alertHandle;
    msgClass_ = SMS_CLASS_UNKNOWN;
    scTimestamp_ = SmsCommonUtils::ConvertTime(deliver.timeStamp);
    SmsTeleSvcUserData userData;
    (void)memset_s(&userData, sizeof(SmsTeleSvcUserData), 0x00, sizeof(SmsTeleSvcUserData));
    userData.userData.length = deliver.cmasData.dataLen;
    userData.encodeType = deliver.cmasData.encodeType;
    if (deliver.cmasData.dataLen > sizeof(userData.userData.data)) {
        TELEPHONY_LOGE("AnalsisCMASMsg memcpy_s data length invalid.");
        return;
    }
    if (memcpy_s(userData.userData.data, sizeof(userData.userData.data), deliver.cmasData.alertText,
            deliver.cmasData.dataLen) == EOK) {
        AnalsisUserData(userData);
    }
}

void CdmaSmsMessage::AnalysisCbMsg(const SmsTransBroadCastMsg &cbMsg)
{
    serviceCategory_ = cbMsg.transSvcCtg;
    if (cbMsg.telesvcMsg.type != SmsMessageType::SMS_TYPE_DELIVER) {
        TELEPHONY_LOGE("No matching type = [%{public}d]", cbMsg.telesvcMsg.type);
        return;
    }
    messageId_ = cbMsg.telesvcMsg.data.deliver.msgId.msgId;
    priority_ = cbMsg.telesvcMsg.data.deliver.priority;
    language_ = cbMsg.telesvcMsg.data.deliver.language;
    TELEPHONY_LOGI("analysisCbMsg transSvcCtg %{public}hu", cbMsg.transSvcCtg);
    if ((cbMsg.transSvcCtg >= SMS_TRANS_SVC_CTG_CMAS_PRESIDENTIAL) &&
        (cbMsg.transSvcCtg <= SMS_TRANS_SVC_CTG_CMAS_TEST)) {
        AnalsisCMASMsg(cbMsg.telesvcMsg.data.deliver);
    } else {
        AnalsisDeliverMsg(cbMsg.telesvcMsg.data.deliver);
    }
}

void CdmaSmsMessage::AnalsisAckMsg(const SmsTransAckMsg &ackMsg)
{
    originatingAddress_ = ackMsg.address.szData;
}

bool CdmaSmsMessage::AddUserDataHeader(const struct SmsUDH &header)
{
    if (userHeaders_.size() >= CDMA_MAX_UD_HEADER_NUM) {
        return false;
    }

    userHeaders_.push_back(header);
    return true;
}

void CdmaSmsMessage::AnalsisHeader(const SmsTeleSvcUserData &userData)
{
    if (memset_s(&smsUserData_, sizeof(SmsUserData), 0x00, sizeof(SmsUserData)) != EOK) {
        return;
    }
    headerDataLen_ = userData.userData.length;
    if (memcpy_s(&smsUserData_, sizeof(SmsUserData), &(userData.userData), sizeof(SmsUserData)) != EOK) {
        return;
    }

    if (bHeaderInd_ && userData.userData.headerCnt > 0) {
        userHeaders_.clear();
        for (int i = 0; i < userData.userData.headerCnt; i++) {
            userHeaders_.push_back(userData.userData.header[i]);
        }
    }
}

/**
 * @brief GetTransMsgType
 * 0x00 is point to point message
 * 0x01 is broadcast message
 * 0x02 is ack message
 * 0x03 is unkown message
 * @return int
 */
int CdmaSmsMessage::GetTransMsgType() const
{
    if (transMsg_ == nullptr) {
        TELEPHONY_LOGE("Trans message type unkown!");
        return SMS_TRANS_TYPE_RESERVED;
    }

    return transMsg_->type;
}

/**
 * @brief Get the Trans Tele Service object
 * 0x1000 IS-91 Extended Protocol Enhanced Services
 * 0x1001 Wireless Paging Teleservice
 * 0x1002 Wireless Messaging Teleservice
 * 0x1003 Voice Mail Notification
 * 0x1004 Wireless Application Protocol
 * 0x1005 Wireless Enhanced Messaging Teleservice
 * 0x1006 Service Category Programming Teleservice
 * 0x1007 Card Application Toolkit Protocol Teleservice
 * 0xffff
 * @return int
 */
int CdmaSmsMessage::GetTransTeleService() const
{
    if ((transMsg_ == nullptr) || (transMsg_->type != SMS_TRANS_P2P_MSG)) {
        TELEPHONY_LOGE("Trans Tele Service is error");
        return SMS_TRANS_TELESVC_RESERVED;
    }

    return transMsg_->data.p2pMsg.transTelesvcId;
}

int CdmaSmsMessage::GetProtocolId() const
{
    return 0;
}

bool CdmaSmsMessage::IsReplaceMessage()
{
    return false;
}

bool CdmaSmsMessage::IsCphsMwi() const
{
    return false;
}

bool CdmaSmsMessage::IsWapPushMsg()
{
    if (transMsg_ == nullptr) {
        return false;
    }

    if (transMsg_->type == SMS_TRANS_P2P_MSG) {
        return (transMsg_->data.p2pMsg.transTelesvcId == SMS_TRANS_TELESVC_WAP);
    }
    return false;
}

std::shared_ptr<SpecialSmsIndication> CdmaSmsMessage::GetSpecialSmsInd()
{
    return specialSmsInd_;
}

bool CdmaSmsMessage::IsStatusReport() const
{
    return (transMsg_->data.p2pMsg.telesvcMsg.type == SMS_TYPE_DELIVERY_ACK);
}

int16_t CdmaSmsMessage::GetDestPort() const
{
    return destPort_;
}

bool CdmaSmsMessage::IsBroadcastMsg() const
{
    return GetTransMsgType() == SMS_TRANS_BROADCAST_MSG;
}

int CdmaSmsMessage::DecodeMessage(unsigned char *decodeData, unsigned int len, SmsCodingScheme &codingType,
    const std::string &msgText, bool &bAbnormal, MSG_LANGUAGE_ID_T &langId)
{
    int decodeLen = 0;
    int dataLen = static_cast<int>(msgText.length());
    const unsigned int maxDecodeLen = len;
    const unsigned char *pMsgText = reinterpret_cast<const unsigned char *>(msgText.c_str());

    MsgTextConvert *textCvt = MsgTextConvert::Instance();
    if (textCvt == nullptr) {
        TELEPHONY_LOGE("MsgTextConvert Instance is nullptr");
        return decodeLen;
    }
    if (msgText.empty()) {
        TELEPHONY_LOGE("MsgText is empty!");
        return decodeLen;
    }

    switch (codingType) {
        case SMS_CODING_7BIT: {
            if (static_cast<unsigned int>(dataLen) > maxDecodeLen) {
                TELEPHONY_LOGE("DecodeMessage memcpy_s data length invalid.");
                return decodeLen;
            }
            if (memcpy_s(decodeData, maxDecodeLen, pMsgText, dataLen) != EOK) {
                TELEPHONY_LOGE("SplitMessage SMS_CHARSET_8BIT memcpy_s error!");
                return decodeLen;
            }
            decodeLen = dataLen;
            codingType = SMS_CODING_ASCII7BIT;
            break;
        }
        case SMS_CODING_8BIT: {
            if (memcpy_s(decodeData, maxDecodeLen, pMsgText, dataLen) != EOK) {
                TELEPHONY_LOGE("SplitMessage SMS_CHARSET_8BIT memcpy_s error!");
                return decodeLen;
            }
            decodeLen = dataLen;
            break;
        }
        case SMS_CODING_UCS2: {
            decodeLen = textCvt->ConvertUTF8ToUCS2(decodeData, maxDecodeLen, pMsgText, dataLen);
            break;
        }
        case SMS_CODING_AUTO:
        default: {
            decodeLen = textCvt->ConvertCdmaUTF8ToAuto(decodeData, maxDecodeLen, pMsgText, dataLen, &codingType);
            break;
        }
    }
    return decodeLen;
}

int8_t CdmaSmsMessage::GetCMASCategory() const
{
    return category_;
}

int8_t CdmaSmsMessage::GetCMASResponseType() const
{
    return responseType_;
}

int8_t CdmaSmsMessage::GetCMASSeverity() const
{
    return severity_;
}

int8_t CdmaSmsMessage::GetCMASUrgency() const
{
    return urgency_;
}

int8_t CdmaSmsMessage::GetCMASCertainty() const
{
    return certainty_;
}

int8_t CdmaSmsMessage::GetCMASMessageClass() const
{
    return messageClass_;
}

bool CdmaSmsMessage::IsCMAS() const
{
    return isCmas_;
}

uint16_t CdmaSmsMessage::GetMessageId() const
{
    return messageId_;
}

int8_t CdmaSmsMessage::GetFormat() const
{
    constexpr int8_t FORMAT_3GPP2 = 2;
    return FORMAT_3GPP2;
}

int8_t CdmaSmsMessage::GetLanguage() const
{
    return language_;
}

std::string CdmaSmsMessage::GetCbInfo() const
{
    std::string info;
    info.append("isCmas:")
        .append(isCmas_ ? "true" : "false")
        .append("\n")
        .append("format:")
        .append(std::to_string(GetFormat()))
        .append("\n")
        .append("messageId:")
        .append(std::to_string(messageId_))
        .append("\n")
        .append("serviceCategory:")
        .append(std::to_string(category_))
        .append("\n")
        .append("language:")
        .append(std::to_string(language_))
        .append("\n")
        .append("body:")
        .append(visibleMessageBody_)
        .append("\n")
        .append("priority:")
        .append(std::to_string(priority_))
        .append("\n")
        .append("responseType:")
        .append(std::to_string(responseType_))
        .append("\n")
        .append("severity:")
        .append(std::to_string(severity_))
        .append("\n")
        .append("urgency:")
        .append(std::to_string(urgency_))
        .append("\n")
        .append("certainty:")
        .append(std::to_string(certainty_))
        .append("\n")
        .append("messageClass:")
        .append(std::to_string(messageClass_))
        .append("\n")
        .append("serviceCategory:")
        .append(std::to_string(serviceCategory_));
    return info;
}

int8_t CdmaSmsMessage::GetPriority() const
{
    return priority_;
}

bool CdmaSmsMessage::IsEmergencyMsg() const
{
    return priority_ == SMS_PRIORITY_EMERGENCY;
}

uint16_t CdmaSmsMessage::GetServiceCategoty() const
{
    return serviceCategory_;
}

uint8_t CdmaSmsMessage::GetGeoScope() const
{
    const uint8_t scopePlmnWide = 1;
    return scopePlmnWide;
}

long CdmaSmsMessage::GetReceTime() const
{
    return scTimestamp_;
}
} // namespace Telephony
} // namespace OHOS
