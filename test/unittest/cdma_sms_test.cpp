/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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

#include "cdma_sms_test.h"

#include <chrono>
#include <codecvt>
#include <iostream>
#include <locale>
#include <sstream>

#include "cdma_sms_sender.h"
#include "cdma_sms_transport_message.h"
#include "delivery_short_message_callback_stub.h"
#include "send_short_message_callback_stub.h"
#include "sms_sender.h"
#include "string_utils.h"

namespace OHOS {
namespace Telephony {
using namespace std;
using namespace OHOS::Telephony;
static constexpr uint8_t TEST_MENU_TEXT_DELIVERY = 0;
static constexpr uint8_t TEST_MENU_ENCODE_SUBMIT = 1;
static constexpr uint8_t TEST_MENU_ENCODE_CANCEL = 2;
static constexpr uint8_t TEST_MENU_ENCODE_DELIVER_REPORT = 3;
static constexpr uint8_t TEST_MENU_ENCODE_ACK = 4;
static constexpr uint8_t TEST_MENU_DECODE_MSG = 5;
static constexpr uint8_t TEST_MENU_DECODE_BEARER_DATA = 6;
static constexpr uint8_t TEST_MENU_EXIT = 100;
static constexpr uint8_t BIT_8 = 8;
static constexpr uint8_t HEX_CHAR_LEN = 2;
static constexpr uint8_t VALUE_INDEX = 2;
static constexpr uint16_t MSG_ID = 100;
static constexpr uint8_t TP_FAIL_CAUSE = 0x81;
static constexpr uint8_t REPLY_SEQ = 10;

void CdmaSmsTest::SetPduSeqInfo(const std::size_t size, const std::unique_ptr<CdmaTransportMsg> &transMsg,
    const std::size_t index, const uint8_t msgRef8bit) const
{
    if (size > 1) {
        transMsg->data.p2p.teleserviceId = static_cast<uint16_t>(SmsTransTelsvcId::WEMT);
        transMsg->data.p2p.telesvcMsg.data.submit.msgId.headerInd = true;
        transMsg->data.p2p.telesvcMsg.data.submit.userData.userData.headerCnt = 1;
        transMsg->data.p2p.telesvcMsg.data.submit.userData.userData.header[0].udhType = UDH_CONCAT_8BIT;
        transMsg->data.p2p.telesvcMsg.data.submit.userData.userData.header[0].udh.concat8bit.msgRef = msgRef8bit;
        transMsg->data.p2p.telesvcMsg.data.submit.userData.userData.header[0].udh.concat8bit.totalSeg =
            static_cast<uint8_t>(size);
        transMsg->data.p2p.telesvcMsg.data.submit.userData.userData.header[0].udh.concat8bit.seqNum = index + 1;
    }
}

std::string CdmaSmsTest::UnicodeToString(uint8_t *s, int len) const
{
    std::string u8str;
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> converter;
    for (int i = 0; i < len; i += HEX_CHAR_LEN) {
        char32_t u8char = (s[i] << BIT_8) | s[i + 1];
        u8str += converter.to_bytes(u8char);
    }
    return u8str;
}

void CdmaSmsTest::PrintAddr(TransportAddr &addr, string s) const
{
    if (addr.addrLen == 0) {
        return;
    }
    stringstream ss;
    ss << s << "digitMode  : " << addr.digitMode << endl;
    ss << s << "numberMode : " << addr.numberMode << endl;
    ss << s << "numberType : " << static_cast<int>(addr.numberType) << endl;
    ss << s << "numberPlan : " << static_cast<int>(addr.numberPlan) << endl;
    ss << s << "addrLen    : " << addr.addrLen << endl;
    ss << s << "szData     : ";
    for (uint8_t i = 0; i < addr.addrLen; i++) {
        ss << addr.szData[i];
    }
    ss << endl;
    cout << ss.str() << flush;
}

void CdmaSmsTest::PrintSubAddr(TransportSubAddr &addr, string s) const
{
    if (addr.addrLen == 0) {
        return;
    }
    switch (addr.type) {
        case TransportSubAddrType::NSAP:
            cout << s << "type    : NSAP - " << static_cast<int>(addr.type) << endl;
            break;
        case TransportSubAddrType::USER:
            cout << s << "type    : USER - " << static_cast<int>(addr.type) << endl;
            break;
        default:
            cout << s << "type    : " << static_cast<int>(addr.type) << endl;
            break;
    }
    cout << s << "odd     : " << addr.odd << endl;
    cout << s << "addrLen : " << addr.addrLen << endl;
    stringstream ss;
    for (uint8_t i = 0; i < addr.addrLen; i++) {
        ss << addr.szData[i];
    }
    cout << s << "szData  : " << ss.str() << endl << flush;
}

void CdmaSmsTest::PrintSmsTeleSvcAddr(SmsTeleSvcAddr &addr, string s) const
{
    if (addr.addrLen == 0) {
        return;
    }
    cout << s << "digitMode  : " << addr.digitMode << endl;
    cout << s << "numberType : " << static_cast<int>(addr.numberType) << endl;
    cout << s << "numberPlan : " << static_cast<int>(addr.numberPlan) << endl;
    cout << s << "addrLen    : " << addr.addrLen << endl;
    stringstream ss;
    for (uint8_t i = 0; i < addr.addrLen; i++) {
        ss << addr.szData[i];
    }
    cout << s << "szData     : " << ss.str() << endl << flush;
}

void CdmaSmsTest::PrintHeader(const SmsUDH &header, string s) const
{
    switch (header.udhType) {
        case UDH_CONCAT_8BIT: {
            cout << s << "udhType : UDH_CONCAT_8BIT - " << static_cast<int>(header.udhType) << endl;
            cout << s << "concat8bit: msgRef[" << static_cast<int>(header.udh.concat8bit.msgRef) << "] totalSeg["
                 << static_cast<int>(header.udh.concat8bit.totalSeg) << "] seqNum["
                 << static_cast<int>(header.udh.concat8bit.seqNum) << "]" << endl;
            break;
        }
        case UDH_CONCAT_16BIT: {
            cout << s << "udhType : UDH_CONCAT_16BIT - " << static_cast<int>(header.udhType) << endl;
            cout << s << "concat16bit.seqNum [" << static_cast<int>(header.udh.concat16bit.seqNum) << "]" << endl;
            break;
        }
        case UDH_APP_PORT_8BIT: {
            cout << s << "udhType : UDH_APP_PORT_8BIT - " << static_cast<int>(header.udhType) << endl;
            cout << s << "appPort8bit.destPort [" << static_cast<int>(header.udh.appPort8bit.destPort) << "]" << endl;
            cout << s << "appPort8bit.originPort [" << static_cast<int>(header.udh.appPort8bit.originPort) << "]"
                 << endl;
            break;
        }
        case UDH_APP_PORT_16BIT: {
            cout << s << "udhType : UDH_APP_PORT_16BIT - " << static_cast<int>(header.udhType) << endl;
            cout << s << "appPort16bit.destPort [" << static_cast<int>(header.udh.appPort16bit.destPort) << "]" << endl;
            cout << s << "appPort16bit.originPort [" << static_cast<int>(header.udh.appPort16bit.originPort) << "]"
                 << endl;
            break;
        }
        case UDH_SPECIAL_SMS: {
            cout << s << "udhType : UDH_SPECIAL_SMS - " << static_cast<int>(header.udhType) << endl;
            cout << s << "Decoding special sms udh." << endl;
            break;
        }
        case UDH_SINGLE_SHIFT: {
            cout << s << "udhType : UDH_SINGLE_SHIFT - " << static_cast<int>(header.udhType) << endl;
            cout << s << "singleShift.langId [" << static_cast<int>(header.udh.singleShift.langId) << "]" << endl;
            break;
        }
        case UDH_LOCKING_SHIFT: {
            cout << s << "udhType : UDH_LOCKING_SHIFT - " << static_cast<int>(header.udhType) << endl;
            cout << s << "lockingShift.langId [" << static_cast<int>(header.udh.lockingShift.langId) << "]" << endl;
            break;
        }
        default:
            cout << s << "udhType : " << static_cast<int>(header.udhType) << endl;
            break;
    }
    cout << flush;
}

void CdmaSmsTest::PrintTimeAbs(SmsTimeAbs &time, string s) const
{
    cout << dec << s << static_cast<int>(time.year) << "/" << static_cast<int>(time.month) << "/"
         << static_cast<int>(time.day) << " " << static_cast<int>(time.hour) << ":" << static_cast<int>(time.minute)
         << ":" << static_cast<int>(time.second) << endl;
    cout << flush;
}

void CdmaSmsTest::PrintEncodeType(SmsEncodingType encodeType, string s) const
{
    switch (encodeType) {
        case SmsEncodingType::OCTET:
            cout << s << "encodeType : OCTET - " << static_cast<int>(encodeType) << endl;
            break;
        case SmsEncodingType::EPM:
            cout << s << "encodeType : EPM - " << static_cast<int>(encodeType) << endl;
            break;
        case SmsEncodingType::ASCII_7BIT:
            cout << s << "encodeType : ASCII_7BIT - " << static_cast<int>(encodeType) << endl;
            break;
        case SmsEncodingType::IA5:
            cout << s << "encodeType : IA5 - " << static_cast<int>(encodeType) << endl;
            break;
        case SmsEncodingType::UNICODE:
            cout << s << "encodeType : UNICODE - " << static_cast<int>(encodeType) << endl;
            break;
        case SmsEncodingType::SHIFT_JIS:
            cout << s << "encodeType : SHIFT_JIS - " << static_cast<int>(encodeType) << endl;
            break;
        case SmsEncodingType::KOREAN:
            cout << s << "encodeType : KOREAN - " << static_cast<int>(encodeType) << endl;
            break;
        case SmsEncodingType::LATIN_HEBREW:
            cout << s << "encodeType : LATIN_HEBREW - " << static_cast<int>(encodeType) << endl;
            break;
        case SmsEncodingType::LATIN:
            cout << s << "encodeType : LATIN - " << static_cast<int>(encodeType) << endl;
            break;
        case SmsEncodingType::GSM7BIT:
            cout << s << "encodeType : GSM7BIT - " << static_cast<int>(encodeType) << endl;
            break;
        case SmsEncodingType::GSMDCS:
            cout << s << "encodeType : GSMDCS - " << static_cast<int>(encodeType) << endl;
            break;
        case SmsEncodingType::EUCKR:
            cout << s << "encodeType : EUCKR - " << static_cast<int>(encodeType) << endl;
            break;
        case SmsEncodingType::RESERVED:
            cout << s << "encodeType : RESERVED - " << static_cast<int>(encodeType) << endl;
            break;
        default:
            cout << s << "encodeType : - " << static_cast<int>(encodeType) << endl;
            break;
    }
    cout << flush;
}

void CdmaSmsTest::PrintUserData(SmsTeleSvcUserData &userData, string s) const
{
    if (userData.userData.length == 0) {
        return;
    }
    PrintEncodeType(userData.encodeType, s);
    cout << s << "msgType    : " << static_cast<int>(userData.msgType) << endl;
    cout << s << "headerCnt  : " << static_cast<int>(userData.userData.headerCnt) << endl;
    for (int i = 0; i < userData.userData.headerCnt; i++) {
        PrintHeader(userData.userData.header[i]);
    }
    cout << s << "length     : " << userData.userData.length << endl;
    stringstream ss;
    if (userData.encodeType == SmsEncodingType::UNICODE) {
        unsigned char unicodeChar[userData.userData.length];
        for (uint8_t i = 0; i < userData.userData.length; i++) {
            unicodeChar[i] = userData.userData.data[i];
        }
        cout << s << "data       : " << UnicodeToString(unicodeChar, userData.userData.length) << endl;
    } else {
        for (uint8_t i = 0; i < userData.userData.length; i++) {
            ss << userData.userData.data[i];
        }
        cout << s << "data       : " << ss.str() << endl;
    }
    cout << flush;
}

void CdmaSmsTest::PrintCmasData(SmsTeleSvcCmasData cmasData, string s) const
{
    if (cmasData.dataLen == 0) {
        return;
    }
    PrintEncodeType(cmasData.encodeType, s);
    cout << s << "dataLen : " << cmasData.dataLen << endl;
    stringstream ss;
    if (cmasData.encodeType == SmsEncodingType::UNICODE) {
        unsigned char unicodeChar[cmasData.dataLen];
        for (uint8_t i = 0; i < cmasData.dataLen; i++) {
            unicodeChar[i] = cmasData.alertText[i];
        }
        ss << UnicodeToString(unicodeChar, cmasData.dataLen);
    } else {
        for (uint8_t i = 0; i < cmasData.dataLen; i++) {
            ss << static_cast<uint8_t>(cmasData.alertText[i]);
        }
    }
    cout << s << "alertText         : " << ss.str() << endl;
    cout << s << "category          :  " << static_cast<uint32_t>(cmasData.category) << endl;
    cout << s << "responseType      :  " << static_cast<uint32_t>(cmasData.responseType) << endl;
    cout << s << "severity          :  " << static_cast<uint32_t>(cmasData.severity) << endl;
    cout << s << "urgency           :  " << static_cast<uint32_t>(cmasData.urgency) << endl;
    cout << s << "certainty         :  " << static_cast<uint32_t>(cmasData.certainty) << endl;
    cout << s << "id                :  " << static_cast<uint32_t>(cmasData.id) << endl;
    cout << s << "alertHandle       :  " << static_cast<uint32_t>(cmasData.alertHandle) << endl;
    cout << s << "language          :  " << static_cast<uint32_t>(cmasData.language) << endl;
    cout << s << "isWrongRecodeType :  " << static_cast<uint32_t>(cmasData.isWrongRecodeType) << endl;
    cout << s << "expires           : " << endl;
    PrintTimeAbs(cmasData.expires, "             ");
    cout << flush;
}

void CdmaSmsTest::PrintSmsReplyOption(SmsReplyOption &replyOpt, string s) const
{
    cout << s << "userAck : " << replyOpt.userAck << endl;
    cout << s << "dak     : " << replyOpt.dak << endl;
    cout << s << "readAck : " << replyOpt.readAck << endl;
    cout << s << "report  : " << replyOpt.report << endl;
    cout << flush;
}

void CdmaSmsTest::PrintSmsValPeriod(SmsValPeriod &valPeriod, string s) const
{
    switch (valPeriod.format) {
        case SMS_TIME_RELATIVE:
            cout << s << "RELATIVE : " << static_cast<uint32_t>(valPeriod.time.relTime.time) << endl;
            break;
        case SMS_TIME_ABSOLUTE:
            cout << s << "ABSOLUTE : " << endl;
            PrintTimeAbs(valPeriod.time.absTime);
            break;
        case SMS_TIME_EMPTY:
            cout << s << "EMPTY" << endl;
            break;
        default:
            cout << s << " " << static_cast<uint32_t>(valPeriod.format) << endl;
            break;
    }
    cout << flush;
}

void CdmaSmsTest::PrintSmsEnhancedVmn(SmsEnhancedVmn &vmn, string s) const
{
    cout << s << "priority : " << static_cast<int>(vmn.priority) << endl;
    cout << s << "vmMsgId : " << static_cast<int>(vmn.vmMsgId) << endl;
    cout << s << "anNumField : " << static_cast<int>(vmn.anNumField) << endl;
    cout << s << "cliNumField : " << static_cast<int>(vmn.cliNumField) << endl;
    cout << flush;
}

void CdmaSmsTest::PrintTeleserviceDeliver(TeleserviceDeliver &deliver, bool isCMAS, string s) const
{
    cout << s << "  msgId          : " << static_cast<int>(deliver.msgId.msgId) << endl;
    cout << s << "  headerInd      : " << static_cast<int>(deliver.msgId.headerInd) << endl;
    if (isCMAS) {
        cout << s << "  cmasData       : " << endl;
        PrintCmasData(deliver.cmasData);
    } else {
        cout << s << "  userData       : " << endl;
        PrintUserData(deliver.userData);
    }
    cout << s << "  timeStamp      : " << endl;
    PrintTimeAbs(deliver.timeStamp);
    cout << s << "  valPeriod      : " << endl;
    PrintSmsValPeriod(deliver.valPeriod);
    cout << s << "  deferValPeriod : " << endl;
    PrintSmsValPeriod(deliver.deferValPeriod);
    cout << s << "  replyOpt       : " << endl;
    PrintSmsReplyOption(deliver.replyOpt);
    cout << s << "  numMsg         : " << static_cast<int>(deliver.numMsg) << endl;
    cout << s << "  language       : " << static_cast<int>(deliver.language) << endl;
    cout << s << "  callbackNumber : " << endl;
    PrintSmsTeleSvcAddr(deliver.callbackNumber);
    cout << s << "  depositId      : " << static_cast<int>(deliver.depositId) << endl;
    cout << s << "  vmn            : " << endl;
    PrintSmsEnhancedVmn(deliver.vmn);
}

void CdmaSmsTest::PrintTeleserviceSubmit(TeleserviceSubmit submit, string s) const
{
    cout << s << "  msgId          : " << static_cast<int>(submit.msgId.msgId) << endl;
    cout << s << "  headerInd      : " << static_cast<int>(submit.msgId.headerInd) << endl;
    cout << s << "  userData       : " << endl;
    PrintUserData(submit.userData);
    cout << s << "  valPeriod      : " << endl;
    PrintSmsValPeriod(submit.valPeriod);
    cout << s << "  deferValPeriod : " << endl;
    PrintSmsValPeriod(submit.deferValPeriod);
    cout << s << "  replyOpt       : " << endl;
    PrintSmsReplyOption(submit.replyOpt);
    cout << s << "  language       : " << static_cast<int>(submit.language) << endl;
    cout << s << "  callbackNumber : " << endl;
    PrintSmsTeleSvcAddr(submit.callbackNumber);
    cout << s << "  depositId      : " << static_cast<int>(submit.depositId) << endl;
}

void CdmaSmsTest::PrintTeleserviceDeliverAck(TeleserviceDeliverAck deliveryAck, string s) const
{
    cout << s << "  msgId     : " << static_cast<int>(deliveryAck.msgId.msgId) << endl;
    cout << s << "  headerInd : " << static_cast<int>(deliveryAck.msgId.headerInd) << endl;
    cout << s << "  userData  : " << endl;
    PrintUserData(deliveryAck.userData);
    cout << s << "  timeStamp : " << endl;
    PrintTimeAbs(deliveryAck.timeStamp);
    cout << s << "  msgStatus : " << static_cast<int>(deliveryAck.msgStatus) << endl;
}

void CdmaSmsTest::PrintTelesvc(CdmaTeleserviceMsg &telesvcMsg, bool isCMAS, string s) const
{
    cout << s << "TeleserviceMsg >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << endl;
    switch (telesvcMsg.type) {
        case TeleserviceMsgType::RESERVED:
            cout << s << "Type: RESERVED - " << static_cast<int>(telesvcMsg.type) << endl;
            break;
        case TeleserviceMsgType::DELIVER:
            cout << s << "Type: DELIVER - " << static_cast<int>(telesvcMsg.type) << endl;
            PrintTeleserviceDeliver(telesvcMsg.data.deliver, isCMAS, s);
            break;
        case TeleserviceMsgType::SUBMIT: {
            cout << s << "Type: SUBMIT - " << static_cast<int>(telesvcMsg.type) << endl;
            PrintTeleserviceSubmit(telesvcMsg.data.submit, s);
            break;
        }
        case TeleserviceMsgType::CANCEL:
            cout << s << "Type: CANCEL - " << static_cast<int>(telesvcMsg.type) << endl;
            cout << s << "  msgId     : " << static_cast<int>(telesvcMsg.data.cancel.msgId.msgId) << endl;
            cout << s << "  headerInd : " << static_cast<int>(telesvcMsg.data.cancel.msgId.headerInd) << endl;
            break;
        case TeleserviceMsgType::DELIVERY_ACK:
            cout << s << "Type: DELIVERY_ACK - " << static_cast<int>(telesvcMsg.type) << endl;
            PrintTeleserviceDeliverAck(telesvcMsg.data.deliveryAck, s);
            break;
        case TeleserviceMsgType::DELIVER_REPORT:
            cout << s << "Type: DELIVER_REPORT - " << static_cast<int>(telesvcMsg.type) << endl;
            cout << s << "  msgId           : " << static_cast<int>(telesvcMsg.data.report.msgId.msgId) << endl;
            cout << s << "  headerInd       : " << static_cast<int>(telesvcMsg.data.report.msgId.headerInd) << endl;
            cout << s << "  userData        : " << endl;
            PrintUserData(telesvcMsg.data.report.userData);
            cout << s << "  tpFailCause     : " << static_cast<int>(telesvcMsg.data.report.tpFailCause) << endl;
            cout << s << "  language        : " << static_cast<int>(telesvcMsg.data.report.language) << endl;
            cout << s << "  multiEncodeData : " << static_cast<int>(telesvcMsg.data.report.multiEncodeData) << endl;
            break;
        case TeleserviceMsgType::SUBMIT_REPORT:
            cout << s << "Type: SUBMIT_REPORT - " << static_cast<int>(telesvcMsg.type) << endl;
            break;
        default:
            cout << s << "Type: RESERVED - " << static_cast<int>(telesvcMsg.type) << endl;
            break;
    }
    cout << s << "TeleserviceMsg <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << endl << flush;
}

void CdmaSmsTest::PrintMsg(CdmaTransportMsg &msg) const
{
    cout << "TransportMsg >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << endl;
    if (msg.type == CdmaTransportMsgType::P2P) {
        cout << "P2P" << endl;
        cout << "  teleserviceId : " << hex << showbase << static_cast<int>(msg.data.p2p.teleserviceId) << dec << endl;
        cout << "  serviceCtg    : " << hex << showbase << static_cast<int>(msg.data.p2p.serviceCtg) << dec << endl;
        cout << "  address       : " << endl;
        PrintAddr(msg.data.p2p.address);
        cout << "  subAddress    : " << endl;
        PrintSubAddr(msg.data.p2p.subAddress);
        cout << "  replySeq      : " << static_cast<int>(msg.data.p2p.replySeq) << endl;
        cout << "  telesvcMsg    : " << endl;
        PrintTelesvc(msg.data.p2p.telesvcMsg);
    } else if (msg.type == CdmaTransportMsgType::BROADCAST) {
        cout << "BROADCAST" << endl;
        cout << "  serviceCtg : " << msg.data.broadcast.serviceCtg << endl;
        cout << "  telesvcMsg : " << endl;
        if (msg.data.broadcast.serviceCtg >= static_cast<uint16_t>(SmsServiceCtg::CMAS_PRESIDENTIAL) &&
            msg.data.broadcast.serviceCtg <= static_cast<uint16_t>(SmsServiceCtg::CMAS_TEST)) {
            PrintTelesvc(msg.data.broadcast.telesvcMsg, true);
        } else {
            PrintTelesvc(msg.data.broadcast.telesvcMsg);
        }
    } else if (msg.type == CdmaTransportMsgType::ACK) {
        cout << "ACK" << endl;
        cout << "  address       : " << endl;
        PrintAddr(msg.data.ack.address);
        cout << "  subAddress    : " << endl;
        cout << "  causeCode     : " << endl;
        cout << "  transReplySeq : " << static_cast<int>(msg.data.ack.causeCode.transReplySeq) << endl;
        cout << "  errorClass    : " << static_cast<int>(msg.data.ack.causeCode.errorClass) << endl;
        cout << "  causeCode     : " << static_cast<int>(msg.data.ack.causeCode.causeCode) << endl;
    } else {
        cout << "Unknown type : " << static_cast<int>(msg.type) << endl;
    }
    cout << "TransportMsg <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << endl << flush;
}

void CdmaSmsTest::EncodeMsg(CdmaTransportMsg &msg) const
{
    std::unique_ptr<CdmaSmsTransportMessage> transportMessage = CdmaSmsTransportMessage::CreateTransportMessage(msg);
    if (transportMessage == nullptr || transportMessage->IsEmpty()) {
        std::cout << "create transport message error" << std::endl;
        return;
    }
    SmsWriteBuffer pduBuffer;
    if (!transportMessage->Encode(pduBuffer)) {
        std::cout << "encode error" << std::endl;
        return;
    }
    std::unique_ptr<std::vector<uint8_t>> pdu = pduBuffer.GetPduBuffer();
    if (pdu == nullptr) {
        std::cout << "get pdubuffer error" << std::endl;
        return;
    }
    PrintMsg(msg);
    std::cout << "TestEncodeSubmitMsg : " << StringUtils::StringToHex(*pdu) << std::endl;
}

void CdmaSmsTest::TestTextBasedSmsDelivery() const
{
    std::cout << "CdmaSmsTest::TestTextBasedSmsDelivery" << std::endl;
    std::function<void(std::shared_ptr<SmsSendIndexer>)> fun = nullptr;
    auto cdmaSmsSender = std::make_shared<CdmaSmsSender>(1, fun);
    const sptr<ISendShortMessageCallback> sendCallback =
        iface_cast<ISendShortMessageCallback>(new SendShortMessageCallbackStub());
    if (sendCallback == nullptr) {
        return;
    }
    const sptr<IDeliveryShortMessageCallback> deliveryCallback =
        iface_cast<IDeliveryShortMessageCallback>(new DeliveryShortMessageCallbackStub());
    if (deliveryCallback == nullptr) {
        return;
    }
    const std::string desAddr = "qwe";
    const std::string scAddr = "123";
    const std::string text = "123";
    cdmaSmsSender->TextBasedSmsDelivery(desAddr, scAddr, text, sendCallback, deliveryCallback);
}

void CdmaSmsTest::EncodeSubmitMsg(
    std::string desAddr, std::string scAddr, std::string text, bool force7Bit, bool statusReport) const
{
    CdmaSmsMessage message;
    DataCodingScheme codingType;
    std::vector<struct SplitInfo> splits;
    message.SplitMessage(splits, text, force7Bit, codingType, false, "");
    std::cout << "codingType : " << codingType << std::endl;
    if (splits.size() > MAX_SEGMENT_NUM) {
        std::cout << "message exceed the limit." << std::endl;
        return;
    }
    std::unique_ptr<CdmaTransportMsg> transMsg =
        message.CreateSubmitTransMsg(desAddr, scAddr, text, statusReport, codingType);
    if (transMsg == nullptr) {
        std::cout << "CreateSubmitTransMsg nullptr fail." << std::endl;
        return;
    }
    /* 1. Set Reply sequence number. */
    uint8_t msgRef8bit = 1;
    transMsg->data.p2p.replySeq = msgRef8bit;
    /* 2. Set msg ID. */
    transMsg->data.p2p.telesvcMsg.data.submit.msgId.msgId = MSG_ID;
    for (std::size_t i = 0; i < splits.size(); i++) {
        (void)memset_s(transMsg->data.p2p.telesvcMsg.data.submit.userData.userData.data,
            sizeof(transMsg->data.p2p.telesvcMsg.data.submit.userData.userData.data), 0x00,
            sizeof(transMsg->data.p2p.telesvcMsg.data.submit.userData.userData.data));
        if (splits[i].encodeData.size() > sizeof(transMsg->data.p2p.telesvcMsg.data.submit.userData.userData.data)) {
            TELEPHONY_LOGE("data length invalid");
            return;
        }
        if (memcpy_s(transMsg->data.p2p.telesvcMsg.data.submit.userData.userData.data,
            sizeof(transMsg->data.p2p.telesvcMsg.data.submit.userData.userData.data), splits[i].encodeData.data(),
            splits[i].encodeData.size()) != EOK) {
            return;
        }
        SetPduSeqInfo(splits.size(), transMsg, i, msgRef8bit);
        transMsg->data.p2p.telesvcMsg.data.submit.userData.userData.length =
            static_cast<int>(splits[i].encodeData.size());

        /* encode msg data */
        cout << "P2P SUBMIT" << endl;
        EncodeMsg(*transMsg.get());
    }
}

void CdmaSmsTest::TestEncodeSubmitMsg() const
{
    std::cout << "CdmaSmsTest::TestEncodeSubmitMsg" << std::endl;

    std::cout << "Please enter the desAddr: " << std::endl;
    std::string desAddr;
    std::cin >> desAddr;

    std::cout << "Please enter the scAddr: " << std::endl;
    std::string scAddr;
    std::cin >> scAddr;

    std::cout << "Please enter the text: " << std::endl;
    std::string text;
    std::cin >> text;

    std::cout << "Please enter the force7Bit: " << std::endl;
    bool force7Bit = true;
    std::cin >> force7Bit;

    std::cout << "Please enter the statusReport: " << std::endl;
    bool statusReport = true;
    std::cin >> statusReport;

    EncodeSubmitMsg(desAddr, scAddr, text, force7Bit, statusReport);
}

void CdmaSmsTest::TestEncodeCancelMsg() const
{
    std::cout << "CdmaSmsTest::TestEncodeCancelMsg" << std::endl;
    std::cout << "Please enter the address: " << std::endl;
    std::string address;
    std::cin >> address;

    CdmaTransportMsg msg;
    memset_s(&msg, sizeof(msg), 0, sizeof(msg));
    msg.type = CdmaTransportMsgType::P2P;
    msg.data.p2p.teleserviceId = static_cast<uint16_t>(SmsTransTelsvcId::CMT_95);
    msg.data.p2p.address.addrLen = address.length();
    if (memcpy_s(msg.data.p2p.address.szData, msg.data.p2p.address.addrLen, address.c_str(),
        msg.data.p2p.address.addrLen) != EOK) {
        return;
    }
    msg.data.p2p.telesvcMsg.type = TeleserviceMsgType::CANCEL;
    msg.data.p2p.telesvcMsg.data.cancel.msgId.msgId = MSG_ID;
    msg.data.p2p.telesvcMsg.data.cancel.msgId.headerInd = 0;
    cout << "P2P CANCEL" << endl;
    EncodeMsg(msg);
}

void CdmaSmsTest::TestEncodeDeliverReportMsg() const
{
    std::cout << "CdmaSmsTest::TestEncodeDeliverReportMsg" << std::endl;
    std::cout << "Please enter the address: " << std::endl;
    std::string address;
    std::cin >> address;

    std::cout << "Please enter the text: " << std::endl;
    std::string text;
    std::cin >> text;

    CdmaTransportMsg msg;
    memset_s(&msg, sizeof(msg), 0, sizeof(msg));
    msg.type = CdmaTransportMsgType::P2P;
    msg.data.p2p.teleserviceId = static_cast<uint16_t>(SmsTransTelsvcId::CMT_95);
    msg.data.p2p.address.addrLen = address.length();
    if (memcpy_s(msg.data.p2p.address.szData, msg.data.p2p.address.addrLen, address.c_str(),
        msg.data.p2p.address.addrLen) != EOK) {
        return;
    }
    msg.data.p2p.telesvcMsg.type = TeleserviceMsgType::DELIVER_REPORT;
    msg.data.p2p.telesvcMsg.data.report.msgId.msgId = MSG_ID;
    msg.data.p2p.telesvcMsg.data.report.msgId.headerInd = 0;
    msg.data.p2p.telesvcMsg.data.report.tpFailCause = TP_FAIL_CAUSE;
    msg.data.p2p.telesvcMsg.data.report.userData.encodeType = SmsEncodingType::ASCII_7BIT;
    msg.data.p2p.telesvcMsg.data.report.userData.userData.length = text.length();
    if (memcpy_s(msg.data.p2p.telesvcMsg.data.report.userData.userData.data,
        msg.data.p2p.telesvcMsg.data.report.userData.userData.length, text.c_str(),
        msg.data.p2p.telesvcMsg.data.report.userData.userData.length) != EOK) {
        return;
    }
    cout << "P2P DELIVER_REPORT" << endl;
    EncodeMsg(msg);
}

void CdmaSmsTest::TestEncodeAckMsg() const
{
    std::cout << "CdmaSmsTest::TestEncodeAckMsg" << std::endl;
    std::cout << "Please enter the address: " << std::endl;
    std::string address;
    std::cin >> address;

    std::cout << "Please enter the errorClass(NONE = 0, TEMPORARY = 2, PERMANENT = 3): " << std::endl;
    int errorClass;
    std::cin >> errorClass;

    std::cout << "Please enter the causeCode(INVAILD_TELESERVICE_ID = 4, SERVICE_TERMINATION_DENIED = 98): "
              << std::endl;
    int causeCode;
    std::cin >> causeCode;

    CdmaTransportMsg msg;
    memset_s(&msg, sizeof(msg), 0, sizeof(msg));
    msg.type = CdmaTransportMsgType::ACK;
    msg.data.ack.address.addrLen = address.length();
    if (memcpy_s(msg.data.ack.address.szData, msg.data.ack.address.addrLen, address.c_str(),
        msg.data.ack.address.addrLen) != EOK) {
        return;
    }
    msg.data.ack.causeCode.transReplySeq = REPLY_SEQ;
    msg.data.ack.causeCode.errorClass = TransportErrClass(errorClass);
    msg.data.ack.causeCode.causeCode = TransportCauseCodeType(causeCode);
    cout << "ACK" << endl;
    EncodeMsg(msg);
}

void CdmaSmsTest::TestDecodeMsg() const
{
    std::cout << "CdmaSmsTest::TestDecodeMsg" << std::endl;
    std::cout << "Please enter the pdu: " << std::endl;
    std::string pdu;
    std::cin >> pdu;

    CdmaTransportMsg msg;
    memset_s(&msg, sizeof(msg), 0, sizeof(msg));
    SmsReadBuffer pduBuffer(StringUtils::HexToString(pdu));
    std::unique_ptr<CdmaSmsTransportMessage> message = CdmaSmsTransportMessage::CreateTransportMessage(msg, pduBuffer);
    if (message == nullptr) {
        std::cout << "create transport message error" << std::endl;
        return;
    }
    if (!message->Decode(pduBuffer)) {
        std::cout << "decode error" << std::endl;
        return;
    }
    cout << "TestDecodeMsg : " << pdu << endl;
    PrintMsg(msg);
}

void CdmaSmsTest::TestDecodeBearerData() const
{
    std::cout << "CdmaSmsTest::TestDecodeBearerData" << std::endl;
    std::cout << "Please enter the pdu: " << std::endl;
    std::string pdu;
    std::cin >> pdu;

    std::stringstream ss;
    ss << static_cast<uint8_t>(CdmaSmsParameterRecord::BEARER_DATA);
    ss << static_cast<uint8_t>(pdu.size() / HEX_CHAR_LEN);
    ss << StringUtils::HexToString(pdu);

    CdmaTeleserviceMsg v;
    memset_s(&v, sizeof(CdmaTeleserviceMsg), 0x00, sizeof(CdmaTeleserviceMsg));
    SmsReadBuffer rBuffer(ss.str());
    rBuffer.SetIndex(VALUE_INDEX);
    auto message = std::make_shared<CdmaSmsBearerData>(v, rBuffer);
    if (message == nullptr) {
        std::cout << "create bearer data error" << std::endl;
        return;
    }
    rBuffer.SetIndex(0);
    if (!message->Decode(rBuffer)) {
        cout << "Decode error!!!!!!!!!!!!!!!!!!!" << endl;
        return;
    }

    cout << "TestDecodeBearerData : " << pdu << endl;
    PrintTelesvc(v);
}

void CdmaSmsTest::ProcessTest() const
{
    bool loopFlag = true;
    while (loopFlag) {
        std::cout << "\nusage:please input a cmd num:\n"
                     "0:TestTextBasedSmsDelivery\n1:TestEncodeSubmitMsg\n2:TestEncodeCancelMsg\n"
                     "3:TestEncodeDeliverReportMsg\n4:TestEncodeAckMsg\n5:TestDecodeMsg\n"
                     "6:TestDecodeBearerData\n100:exit test cdma sms\n"
                  << std::endl;

        int inputCMD = 0;
        std::cin >> inputCMD;
        while (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore();
            std::cin >> inputCMD;
        }

        std::cout << "inputCMD is:" << inputCMD << std::endl;

        switch (inputCMD) {
            case TEST_MENU_TEXT_DELIVERY:
                TestTextBasedSmsDelivery();
                break;
            case TEST_MENU_ENCODE_SUBMIT:
                TestEncodeSubmitMsg();
                break;
            case TEST_MENU_ENCODE_CANCEL:
                TestEncodeCancelMsg();
                break;
            case TEST_MENU_ENCODE_DELIVER_REPORT:
                TestEncodeDeliverReportMsg();
                break;
            case TEST_MENU_ENCODE_ACK:
                TestEncodeAckMsg();
                break;
            case TEST_MENU_DECODE_MSG:
                TestDecodeMsg();
                break;
            case TEST_MENU_DECODE_BEARER_DATA:
                TestDecodeBearerData();
                break;
            case TEST_MENU_EXIT:
                loopFlag = false;
                break;
            default:
                break;
        }
    }
}

} // namespace Telephony
} // namespace OHOS
