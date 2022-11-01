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
#include "mms_msg_test.h"

#include <iostream>
#include <sstream>

#include "mms_codec_type.h"

namespace OHOS {
namespace Telephony {
static constexpr char HEX_TABLE[] = "0123456789ABCDEF";
static constexpr uint8_t HEX_OFFSET = 4;
static constexpr uint8_t MAX_LINE_NUM = 16;

MmsMsgTest::MmsMsgTest() {}

MmsMsgTest::~MmsMsgTest() {}

void MmsMsgTest::ProcessDecodeInput(int inputCMD) const
{
    switch (inputCMD) {
        case 0x00:
            MmsDecodeTest("/data/telephony/deSrc/SendReq.mms");
            break;
        case 0x01:
            MmsDecodeTest("/data/telephony/deSrc/SendConf.mms");
            break;
        case 0x02:
            MmsDecodeTest("/data/telephony/deSrc/NotificationInd.mms");
            break;
        case 0x03:
            MmsDecodeTest("/data/telephony/deSrc/NotifyRespInd.mms");
            break;
        case 0x04:
            MmsDecodeTest("/data/telephony/deSrc/RetrieveConf.mms");
            break;
        case 0x05:
            MmsDecodeTest("/data/telephony/deSrc/AcknowledgeInd.mms");
            break;
        case 0x06:
            MmsDecodeTest("/data/telephony/deSrc/DeliveryInd.mms");
            break;
        case 0x07:
            MmsDecodeTest("/data/telephony/deSrc/ReadRecInd.mms");
            break;
        case 0x08:
            MmsDecodeTest("/data/telephony/deSrc/ReadOrigInd.mms");
            break;
        default:
            break;
    }
}

void MmsMsgTest::ProcessEncodeInput(int inputCMD) const
{
    switch (inputCMD) {
        case 0x09:
            MmsSendReqEncodeTest();
            break;
        case 0x0a:
            MmsSendConfEncodeTest();
            break;
        case 0x0b:
            MmsNotificationIndEncodeTest();
            break;
        case 0x0c:
            MmsNotifyRespIndEncodeTest();
            break;
        case 0x0d:
            MmsRetrieveConfEncodeTest();
            break;
        case 0x0e:
            MmsAcknowledgeIndEncodeTest();
            break;
        case 0x0f:
            MmsDeliveryIndEncodeTest();
            break;
        case 0x10:
            MmsReadRecIndEncodeTest();
            break;
        case 0x11:
            MmsReadOrigIndEncodeTest();
            break;
        default:
            break;
    }
}

void MmsMsgTest::ProcessTest() const
{
    bool loopFlag = true;
    const int exitKey = 100;
    while (loopFlag) {
        std::cout << "\nusage:please input a cmd num:\n"
                     "0:TestDecodeMmsSendReq\n"
                     "1:TestDecodeMmsSendConf\n"
                     "2:TestDecodeMmsNotificationInd\r\n"
                     "3:TestDecodeMmsNotifyRespInd\r\n"
                     "4:TestDecodeMmsRetrieveConf\r\n"
                     "5:TestDecodeMmsAcknowledgeInd\r\n"
                     "6:TestDecodeMmsDeliveryInd\r\n"
                     "7:TestDecodeMmsReadRecInd\r\n"
                     "8:TestDecodeMmsReadOrigInd\r\n"
                     "9:TestEncodeMmsSendReq\r\n"
                     "10:TestEncodeMmsSendConf\r\n"
                     "11:TestEncodeMmsNotificationInd\r\n"
                     "12:TestEncodeMmsNotifyRespInd\r\n"
                     "13:TestEncodeMmsRetrieveConf\r\n"
                     "14:TestEncodeMmsAcknowledgeInd\r\n"
                     "15:TestEncodeMmsDeliveryInd\r\n"
                     "16:TestEncodeMmsReadRecInd\r\n"
                     "17:TestEncodeMmsReadOrigInd\r\n"
                     "100:exit test mms msg\n"
                  << std::endl;

        int inputCMD = 0;
        std::cin >> inputCMD;
        while (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore();
            std::cin >> inputCMD;
        }
        if (inputCMD == exitKey) {
            return;
        }
        std::cout << "inputCMD is:" << inputCMD << std::endl;
        ProcessDecodeInput(inputCMD);
        ProcessEncodeInput(inputCMD);
    }
}

void MmsMsgTest::MmsDecodeTest(std::string strPath) const
{
    MmsMsg decodeMsg;
    if (!decodeMsg.DecodeMsg(strPath)) {
        std::cout << "mms decode message fail." << std::endl;
        std::cout << "mms file path name:" << strPath << std::endl;
        return;
    }
    decodeMsg.DumpMms();
    uint8_t messageType = decodeMsg.GetMmsMessageType();
    switch (messageType) {
        case MMS_MSGTYPE_SEND_REQ:
            MmsSendReqDecodeTest(decodeMsg);
            break;
        case MMS_MSGTYPE_SEND_CONF:
            MmsSendConfDecodeTest(decodeMsg);
            break;
        case MMS_MSGTYPE_NOTIFICATION_IND:
            MmsNotificationIndDecodeTest(decodeMsg);
            break;
        case MMS_MSGTYPE_NOTIFYRESP_IND:
            MmsNotifyRespIndDecodeTest(decodeMsg);
            break;
        case MMS_MSGTYPE_RETRIEVE_CONF:
            MmsRetrieveConfDecodeTest(decodeMsg);
            break;
        case MMS_MSGTYPE_ACKNOWLEDGE_IND:
            MmsAcknowledgeIndDecodeTest(decodeMsg);
            break;
        case MMS_MSGTYPE_DELIVERY_IND:
            MmsDeliveryIndDecodeTest(decodeMsg);
            break;
        case MMS_MSGTYPE_READ_REC_IND:
            MmsReadRecIndDecodeTest(decodeMsg);
            break;
        case MMS_MSGTYPE_READ_ORIG_IND:
            MmsReadOrigIndDecodeTest(decodeMsg);
            break;
        default:
            break;
    }
}

void MmsMsgTest::MmsSendReqEncodeTest() const
{
    std::cout << "Start MmsSendReqEncodeTest" << std::endl;
    MmsMsg encodeMsg;
    // Mandatory
    if (!encodeMsg.SetMmsMessageType(MMS_MSGTYPE_SEND_REQ)) {
        std::cout << "SetMmsMessageType fail" << std::endl;
        return;
    }
    if (!encodeMsg.SetMmsTransactionId("2077.1427358451410")) {
        std::cout << "SetMmsTransactionId fail" << std::endl;
        return;
    }
    if (!encodeMsg.SetMmsVersion(static_cast<uint16_t>(MmsVersionType::MMS_VERSION_1_2))) {
        std::cout << "SetMmsVersion fail" << std::endl;
        return;
    }
    MmsAddress address;
    address.SetMmsAddressString("+8613812345678/TYPE=PLMN");
    if (!encodeMsg.SetMmsFrom(address)) {
        std::cout << "SetMmsFrom fail" << std::endl;
        return;
    }
    if (!encodeMsg.SetHeaderContentType("application/vnd.wap.multipart.related")) {
        std::cout << "SetHeaderContentType fail" << std::endl;
        return;
    }
    // Optional
    if (!encodeMsg.SetMmsSubject("Test mms")) {
        std::cout << "SetMmsSubject fail" << std::endl;
        return;
    }
    if (!encodeMsg.SetHeaderOctetValue(MMS_CONTENT_CLASS, static_cast<uint8_t>(MmsContentClass::MMS_TEXT))) {
        std::cout << "SetHeaderOctetValue MMS_CONTENT_CLASS fail" << std::endl;
        return;
    }
    std::vector<MmsAddress> vecAddrs;
    MmsAddress toAddrs("+8613888888888/TYPE=PLMN");
    vecAddrs.push_back(toAddrs);
    MmsAddress toAddrs2("+8613812345678/TYPE=PLMN");
    vecAddrs.push_back(toAddrs2);
    if (!encodeMsg.SetMmsTo(vecAddrs)) {
        std::cout << "SetMmsTo fail" << std::endl;
        return;
    }
    // add smil file
    const std::string filePathNameSmil = "/data/telephony/enSrc/618C0A89.smil";
    if (!MmsAddAttachment(encodeMsg, filePathNameSmil, "<0000>", "application/smil", true)) {
        std::cout << "MmsAddAttachment smil fail" << std::endl;
        return;
    }
    // add text file
    const std::string filePathNameText = "/data/telephony/enSrc/content.text";
    if (!MmsAddAttachment(encodeMsg, filePathNameText, "<content.text>", "text/plain", false)) {
        std::cout << "MmsAddAttachment text fail" << std::endl;
        return;
    }
    // add image file
    const std::string filePathNameGif = "/data/telephony/enSrc/picture.gif";
    if (!MmsAddAttachment(encodeMsg, filePathNameGif, "<picture.gif>", "image/gif", false)) {
        std::cout << "MmsAddAttachment gif fail" << std::endl;
        return;
    }
    uint32_t len = 0;
    std::unique_ptr<char[]> result = encodeMsg.EncodeMsg(len);
    if (result == nullptr) {
        std::cout << "encode fail result nullptr error." << std::endl;
        return;
    }
    if (!WriteBufferToFile(std::move(result), len, "/data/telephony/deSrc/SendReq.mms")) {
        std::cout << "Encode write to file error." << std::endl;
        return;
    }
    std::cout << "MmsSendReqEncodeTest encode success, data len = " << len << std::endl;
}

void MmsMsgTest::MmsSendConfEncodeTest() const
{
    std::cout << "Start MmsSendConfEncodeTest" << std::endl;
    MmsMsg encodeMsg;
    // Mandatory
    if (!encodeMsg.SetMmsMessageType(MMS_MSGTYPE_SEND_CONF)) {
        std::cout << "SetMmsMessageType fail" << std::endl;
        return;
    }
    if (!encodeMsg.SetMmsTransactionId("2077.1427358451410")) {
        std::cout << "SetMmsTransactionId fail" << std::endl;
        return;
    }
    if (!encodeMsg.SetMmsVersion(static_cast<uint16_t>(MmsVersionType::MMS_VERSION_1_2))) {
        std::cout << "SetMmsVersion fail" << std::endl;
        return;
    }
    if (!encodeMsg.SetHeaderOctetValue(MMS_RESPONSE_STATUS, static_cast<uint8_t>(MmsResponseStatus::MMS_OK))) {
        std::cout << "SetHeaderOctetValue MMS_RESPONSE_STATUS fail" << std::endl;
        return;
    }
    uint32_t len = 0;
    std::unique_ptr<char[]> result = encodeMsg.EncodeMsg(len);
    if (result == nullptr) {
        std::cout << "encode fail result nullptr error." << std::endl;
        return;
    }
    if (!WriteBufferToFile(std::move(result), len, "/data/telephony/deSrc/SendConf.mms")) {
        std::cout << "Encode write to file error." << std::endl;
        return;
    }
    std::cout << "MmsSendConfEncodeTest encode success, data len =" << len << std::endl;
}

void MmsMsgTest::MmsNotificationIndEncodeTest() const
{
    std::cout << "Start MmsNotificationIndEncodeTest" << std::endl;
    const long expiryTemp = 1637141707;
    const long messageSize = 12345678;
    MmsMsg encodeMsg;
    // Mandatory
    if (!encodeMsg.SetMmsMessageType(MMS_MSGTYPE_NOTIFICATION_IND)) {
        std::cout << "SetMmsMessageType fail" << std::endl;
        return;
    }
    if (!encodeMsg.SetMmsTransactionId("2077.1427358451410")) {
        std::cout << "SetMmsTransactionId fail" << std::endl;
        return;
    }
    if (!encodeMsg.SetMmsVersion(static_cast<uint16_t>(MmsVersionType::MMS_VERSION_1_2))) {
        std::cout << "SetMmsVersion fail" << std::endl;
        return;
    }
    if (!encodeMsg.SetHeaderOctetValue(MMS_MESSAGE_CLASS, static_cast<uint8_t>(MmsMessageClass::PERSONAL))) {
        std::cout << "SetHeaderOctetValue fail" << std::endl;
        return;
    }
    if (!encodeMsg.SetHeaderLongValue(MMS_EXPIRY, expiryTemp)) {
        std::cout << "SetHeaderLongValue MMS_EXPIRY fail" << std::endl;
        return;
    }
    if (!encodeMsg.SetHeaderLongValue(MMS_MESSAGE_SIZE, messageSize)) {
        std::cout << "SetHeaderLongValue MMS_MESSAGE_SIZE fail" << std::endl;
        return;
    }
    std::string strContentLocation = "Test";
    if (!encodeMsg.SetHeaderStringValue(MMS_CONTENT_LOCATION, strContentLocation)) {
        std::cout << "SetHeaderStringValue fail" << std::endl;
        return;
    } // need to confirmation
    // Optional
    MmsAddress address;
    address.SetMmsAddressString("+8613812345678/TYPE=PLMN");
    if (!encodeMsg.SetMmsFrom(address)) {
        std::cout << "SetMmsFrom fail" << std::endl;
        return;
    }
    if (!encodeMsg.SetMmsSubject("Test mms")) {
        std::cout << "SetMmsSubject fail" << std::endl;
        return;
    }
    uint32_t len = 0;
    std::unique_ptr<char[]> result = encodeMsg.EncodeMsg(len);
    if (result == nullptr) {
        std::cout << "encode fail result nullptr error." << std::endl;
        return;
    }
    if (!WriteBufferToFile(std::move(result), len, "/data/telephony/deSrc/NotificationInd.mms")) {
        std::cout << "Encode write to file error." << std::endl;
        return;
    }
    std::cout << "MmsNotificationIndEncodeTest encode success, data len =" << len << std::endl;
}

void MmsMsgTest::MmsNotifyRespIndEncodeTest() const
{
    std::cout << "Start MmsNotifyRespIndEncodeTest" << std::endl;
    MmsMsg encodeMsg;
    // Mandatory
    if (!encodeMsg.SetMmsMessageType(MMS_MSGTYPE_NOTIFYRESP_IND)) {
        std::cout << "SetMmsMessageType fail" << std::endl;
        return;
    }
    if (!encodeMsg.SetMmsTransactionId("2077.1427358451410")) {
        std::cout << "SetMmsTransactionId fail" << std::endl;
        return;
    }
    if (!encodeMsg.SetMmsVersion(static_cast<uint16_t>(MmsVersionType::MMS_VERSION_1_2))) {
        std::cout << "SetMmsVersion fail" << std::endl;
        return;
    }
    if (!encodeMsg.SetHeaderOctetValue(MMS_STATUS, static_cast<uint8_t>(MmsStatus::MMS_EXPIRED))) {
        std::cout << "SetHeaderOctetValue MMS_STATUS fail" << std::endl;
        return;
    }
    uint32_t len = 0;
    std::unique_ptr<char[]> result = encodeMsg.EncodeMsg(len);
    if (result == nullptr) {
        std::cout << "encode fail result nullptr error." << std::endl;
        return;
    }
    if (!WriteBufferToFile(std::move(result), len, "/data/telephony/deSrc/NotifyRespInd.mms")) {
        std::cout << "Encode write to file error." << std::endl;
        return;
    }
    std::cout << "MmsNotifyRespIndEncodeTest encode success, data len =" << len << std::endl;
}

void MmsMsgTest::MmsRetrieveConfEncodeTest() const
{
    std::cout << "Start MmsRetrieveConfEncodeTest" << std::endl;
    MmsMsg encodeMsg;
    // Mandatory
    if (!encodeMsg.SetMmsMessageType(MMS_MSGTYPE_RETRIEVE_CONF)) {
        std::cout << "SetMmsMessageType fail" << std::endl;
        return;
    }
    if (!encodeMsg.SetMmsTransactionId("2077.1427358451410")) {
        std::cout << "SetMmsTransactionId fail" << std::endl;
        return;
    }
    if (!encodeMsg.SetMmsVersion(static_cast<uint16_t>(MmsVersionType::MMS_VERSION_1_2))) {
        std::cout << "SetMmsVersion fail" << std::endl;
        return;
    }
    const long dateTemp = 1637141707;
    if (!encodeMsg.SetMmsDate(dateTemp)) {
        std::cout << "SetMmsDate fail" << std::endl;
        return;
    }
    if (!encodeMsg.SetHeaderContentType("application/vnd.wap.multipart.related")) {
        std::cout << "SetHeaderContentType fail" << std::endl;
        return;
    }

    // Optional
    MmsAddress address;
    address.SetMmsAddressString("+8613812345678/TYPE=PLMN");
    if (!encodeMsg.SetMmsFrom(address)) {
        std::cout << "SetMmsFrom fail" << std::endl;
        return;
    }
    if (!encodeMsg.SetMmsSubject("Test mms")) {
        std::cout << "SetMmsSubject fail" << std::endl;
        return;
    }
    // add smil file
    const std::string filePathNameSmil = "/data/telephony/enSrc/618C0A89.smil";
    if (!MmsAddAttachment(encodeMsg, filePathNameSmil, "<0000>", "application/smil", true)) {
        std::cout << "MmsAddAttachment smil fail" << std::endl;
        return;
    }
    // add text file
    const std::string filePathNameText = "/data/telephony/enSrc/content.text";
    if (!MmsAddAttachment(encodeMsg, filePathNameText, "<content.text>", "text/plain", false)) {
        std::cout << "MmsAddAttachment text fail" << std::endl;
        return;
    }
    // add image file
    const std::string filePathNameGif("/data/telephony/enSrc/picture.gif");
    if (!MmsAddAttachment(encodeMsg, filePathNameGif, "picture.gif", "image/gif", false)) {
        std::cout << "MmsAddAttachment gif fail" << std::endl;
        return;
    }
    uint32_t len = 0;
    std::unique_ptr<char[]> result = encodeMsg.EncodeMsg(len);
    if (result == nullptr) {
        std::cout << "encode fail result nullptr error." << std::endl;
        return;
    }
    if (!WriteBufferToFile(std::move(result), len, "/data/telephony/deSrc/RetrieveConf.mms")) {
        std::cout << "Encode write to file error." << std::endl;
        return;
    }
    std::cout << "MmsRetrieveConfEncodeTest encode success, data len =" << len << std::endl;
}

void MmsMsgTest::MmsAcknowledgeIndEncodeTest() const
{
    std::cout << "Start MmsAcknowledgeIndEncodeTest" << std::endl;
    MmsMsg encodeMsg;
    // Mandatory
    if (!encodeMsg.SetMmsMessageType(MMS_MSGTYPE_ACKNOWLEDGE_IND)) {
        std::cout << "SetMmsMessageType fail" << std::endl;
        return;
    }
    if (!encodeMsg.SetMmsTransactionId("2077.1427358451410")) {
        std::cout << "SetMmsTransactionId fail" << std::endl;
        return;
    }
    if (!encodeMsg.SetMmsVersion(static_cast<uint16_t>(MmsVersionType::MMS_VERSION_1_2))) {
        std::cout << "SetMmsVersion fail" << std::endl;
        return;
    }
    uint32_t len = 0;
    std::unique_ptr<char[]> result = encodeMsg.EncodeMsg(len);
    if (result == nullptr) {
        std::cout << "encode fail result nullptr error." << std::endl;
        return;
    }
    if (!WriteBufferToFile(std::move(result), len, "/data/telephony/deSrc/AcknowledgeInd.mms")) {
        std::cout << "Encode write to file error." << std::endl;
        return;
    }
    std::cout << "MmsAcknowledgeIndEncodeTest encode success, data len =" << len << std::endl;
}

void MmsMsgTest::MmsDeliveryIndEncodeTest() const
{
    std::cout << "Start MmsDeliveryIndEncodeTest" << std::endl;
    MmsMsg encodeMsg;
    // Mandatory
    if (!encodeMsg.SetMmsMessageType(MMS_MSGTYPE_DELIVERY_IND)) {
        std::cout << "SetMmsMessageType fail" << std::endl;
        return;
    }
    if (!encodeMsg.SetMmsTransactionId("2077.1427358451410")) {
        std::cout << "SetMmsTransactionId fail" << std::endl;
        return;
    }
    if (!encodeMsg.SetMmsVersion(static_cast<uint16_t>(MmsVersionType::MMS_VERSION_1_2))) {
        std::cout << "SetMmsVersion fail" << std::endl;
        return;
    }
    std::string strMessageID = "0001";
    if (!encodeMsg.SetHeaderStringValue(MMS_MESSAGE_ID, strMessageID)) {
        std::cout << "SetHeaderStringValue fail" << std::endl;
        return;
    }
    const long dateTemp = 1637141707;
    if (!encodeMsg.SetMmsDate(dateTemp)) {
        std::cout << "SetMmsDate fail" << std::endl;
        return;
    }
    std::vector<MmsAddress> vecAddrs;
    MmsAddress toAddrs("+8613888888888/TYPE=PLMN");
    vecAddrs.push_back(toAddrs);
    if (!encodeMsg.SetMmsTo(vecAddrs)) {
        std::cout << "SetMmsTo fail" << std::endl;
        return;
    }
    if (!encodeMsg.SetHeaderOctetValue(MMS_STATUS, static_cast<uint8_t>(MmsStatus::MMS_EXPIRED))) {
        std::cout << "SetHeaderOctetValue MMS_STATUS fail" << std::endl;
        return;
    }
    uint32_t len = 0;
    std::unique_ptr<char[]> result = encodeMsg.EncodeMsg(len);
    if (result == nullptr) {
        std::cout << "encode fail result nullptr error." << std::endl;
        return;
    }
    if (!WriteBufferToFile(std::move(result), len, "/data/telephony/deSrc/DeliveryInd.mms")) {
        std::cout << "Encode write to file error." << std::endl;
        return;
    }
    std::cout << "MmsDeliveryIndEncodeTest encode success, data len =" << len << std::endl;
}

void MmsMsgTest::MmsReadRecIndEncodeTest() const
{
    std::cout << "Start MmsReadRecIndEncodeTest" << std::endl;
    MmsMsg encodeMsg;
    // Mandatory
    if (!encodeMsg.SetMmsMessageType(MMS_MSGTYPE_READ_REC_IND)) {
        std::cout << "SetMmsMessageType fail" << std::endl;
        return;
    }
    if (!encodeMsg.SetMmsVersion(static_cast<uint16_t>(MmsVersionType::MMS_VERSION_1_2))) {
        std::cout << "SetMmsVersion fail" << std::endl;
        return;
    }
    std::string strMessageID = "0001";
    if (!encodeMsg.SetHeaderStringValue(MMS_MESSAGE_ID, strMessageID)) {
        std::cout << "SetHeaderStringValue fail" << std::endl;
        return;
    }
    std::vector<MmsAddress> vecAddrs;
    MmsAddress toAddrs("+8613888888888/TYPE=PLMN");
    vecAddrs.push_back(toAddrs);
    if (!encodeMsg.SetMmsTo(vecAddrs)) {
        std::cout << "SetMmsTo fail" << std::endl;
        return;
    }
    MmsAddress address;
    address.SetMmsAddressString("+8613812345678/TYPE=PLMN");
    if (!encodeMsg.SetMmsFrom(address)) {
        std::cout << "SetMmsFrom fail" << std::endl;
        return;
    }
    if (!encodeMsg.SetHeaderOctetValue(MMS_READ_STATUS, static_cast<uint8_t>(MmsReadStatus::MMS_READ))) {
        std::cout << "SetHeaderOctetValue MMS_READ_STATUS fail" << std::endl;
        return;
    }
    // Optional
    const long dateTemp = 1637141707;
    encodeMsg.SetMmsDate(dateTemp);
    uint32_t len = 0;
    std::unique_ptr<char[]> result = encodeMsg.EncodeMsg(len);
    if (result == nullptr) {
        std::cout << "encode fail result nullptr error." << std::endl;
        return;
    }
    if (!WriteBufferToFile(std::move(result), len, "/data/telephony/deSrc/ReadRecInd.mms")) {
        std::cout << "Encode write to file error." << std::endl;
        return;
    }
    std::cout << "MmsReadRecIndEncodeTest encode success data len =" << len << std::endl;
}

void MmsMsgTest::MmsReadOrigIndEncodeTest() const
{
    std::cout << "Start MmsReadOrigIndEncodeTest" << std::endl;
    MmsMsg encodeMsg;
    // Mandatory
    if (!encodeMsg.SetMmsMessageType(MMS_MSGTYPE_READ_ORIG_IND)) {
        std::cout << "SetMmsMessageType fail" << std::endl;
        return;
    }
    if (!encodeMsg.SetMmsVersion(static_cast<uint16_t>(MmsVersionType::MMS_VERSION_1_2))) {
        std::cout << "SetMmsVersion fail" << std::endl;
        return;
    }
    std::string strMessageID = "0001";
    if (!encodeMsg.SetHeaderStringValue(MMS_MESSAGE_ID, strMessageID)) {
        std::cout << "SetHeaderStringValue fail" << std::endl;
        return;
    }
    std::vector<MmsAddress> vecAddrs;
    MmsAddress toAddrs("+8613888888888/TYPE=PLMN");
    vecAddrs.push_back(toAddrs);
    if (!encodeMsg.SetMmsTo(vecAddrs)) {
        std::cout << "SetMmsTo fail" << std::endl;
        return;
    }
    MmsAddress address;
    address.SetMmsAddressString("+8613812345678/TYPE=PLMN");
    if (!encodeMsg.SetMmsFrom(address)) {
        std::cout << "SetMmsFrom fail" << std::endl;
        return;
    }
    const long dateTemp = 1637141707;
    if (!encodeMsg.SetMmsDate(dateTemp)) {
        std::cout << "SetMmsDate fail" << std::endl;
        return;
    }
    if (!encodeMsg.SetHeaderOctetValue(MMS_READ_STATUS, static_cast<uint8_t>(MmsReadStatus::MMS_READ))) {
        std::cout << "SetHeaderOctetValue MMS_READ_STATUS fail" << std::endl;
        return;
    }
    // Optional
    uint32_t len = 0;
    std::unique_ptr<char[]> result = encodeMsg.EncodeMsg(len);
    if (result == nullptr) {
        std::cout << "encode fail result nullptr error." << std::endl;
        return;
    }
    if (!WriteBufferToFile(std::move(result), len, "/data/telephony/deSrc/ReadOrigInd.mms")) {
        std::cout << "Encode write to file error." << std::endl;
        return;
    }
    std::cout << "MmsReadOrigIndEncodeTest encode success, data len =" << len << std::endl;
}

bool MmsMsgTest::MmsAddAttachment(
    MmsMsg &msg, std::string pathName, std::string contentId, std::string contenType, bool isSmil) const
{
    MmsAttachment imageAttachment;
    std::size_t pos = pathName.find_last_of('/');
    std::string fileName(pathName.substr(pos + 1));

    if (!imageAttachment.SetAttachmentFilePath(pathName, isSmil)) {
        std::cout << "MmsAddAttachment SetAttachmentFilePath fail" << std::endl;
        return false;
    }
    if (!imageAttachment.SetFileName(fileName)) {
        std::cout << "MmsAddAttachment SetFileName fail" << std::endl;
        return false;
    }
    if (!imageAttachment.SetContentId(contentId)) {
        std::cout << "MmsAddAttachment SetContentId fail" << std::endl;
        return false;
    }
    if (!imageAttachment.SetContentLocation(fileName)) {
        std::cout << "MmsAddAttachment SetContentLocation fail" << std::endl;
        return false;
    }
    if (!imageAttachment.SetContentType(contenType)) {
        std::cout << "MmsAddAttachment SetContentType fail" << std::endl;
        return false;
    }
    imageAttachment.SetContentDisposition("attachment");
    if (!msg.AddAttachment(imageAttachment)) {
        std::cout << "MmsAddAttachment AddAttachment fail" << std::endl;
        return false;
    }
    return true;
}

void MmsMsgTest::MmsSendReqDecodeTest(MmsMsg &decodeMsg) const
{
    std::cout << "======= Start SendReqDecodeTest ===========" << std::endl;
    // Mandatory
    std::cout << "TransactionId: " << decodeMsg.GetMmsTransactionId() << std::endl;
    std::cout << "Version: " << decodeMsg.GetMmsVersion() << std::endl;
    std::cout << "Date: " << decodeMsg.GetMmsDate() << std::endl;
    std::cout << "From: " << decodeMsg.GetMmsFrom().GetAddressString() << std::endl;
    std::vector<MmsAddress> toAddress;
    decodeMsg.GetMmsTo(toAddress);
    for (auto itTo : toAddress) {
        std::cout << "To: " << itTo.GetAddressString() << std::endl;
    }
    std::string deliverReport = std::to_string(decodeMsg.GetHeaderOctetValue(MMS_DELIVERY_REPORT));
    std::string sendVisbility = std::to_string(decodeMsg.GetHeaderOctetValue(MMS_SENDER_VISIBILITY));
    std::string readResport = std::to_string(decodeMsg.GetHeaderOctetValue(MMS_READ_REPORT));
    std::string messageClass = std::to_string(decodeMsg.GetHeaderOctetValue(MMS_MESSAGE_CLASS));
    std::string priority = std::to_string(decodeMsg.GetHeaderOctetValue(MMS_PRIORITY));

    std::cout << "Subject: " << decodeMsg.GetMmsSubject() << std::endl;
    std::cout << "DeliveryReport: " << deliverReport << std::endl;
    std::cout << "SenderVisibility: " << sendVisbility << std::endl;
    std::cout << "ReadReport: " << readResport << std::endl;
    std::cout << "MessageClass: " << messageClass << std::endl;
    std::cout << "Prioity: " << priority << std::endl;

    std::vector<MmsAttachment> attachments;
    decodeMsg.GetAllAttachment(attachments);
    int attachmentCnt = 0;
    for (auto it : attachments) {
        std::cout << "=======part:" << attachmentCnt << " attachments infos ======" << std::endl;
        std::cout << "ContentDisposition: " << it.GetContentDisposition() << std::endl;
        std::cout << "ContentLocation: " << it.GetContentLocation() << std::endl;
        std::cout << "ContentId: " << it.GetContentId() << std::endl;
        std::cout << "FileName: " << it.GetFileName() << std::endl;
        uint32_t len = 0;
        std::unique_ptr<char[]> buff = it.GetDataBuffer(len);
        if (buff != nullptr) {
            std::cout << "attachments buffer size = " << len << std::endl;
        }
        std::cout << "=======part:" << attachmentCnt << " attachments infos ======" << std::endl;
        attachmentCnt++;
    }
    std::cout << "======= End SendReqDecodeTest ===========" << std::endl;
}

void MmsMsgTest::MmsSendConfDecodeTest(MmsMsg &decodeMsg) const
{
    std::cout << "======== Start SendConfEncodeTest =============" << std::endl;
    // Mandatory
    std::cout << "TransactionId: " << decodeMsg.GetMmsTransactionId() << std::endl;
    std::cout << "Version: " << decodeMsg.GetMmsVersion() << std::endl;
    std::string responseStatus = std::to_string(decodeMsg.GetHeaderOctetValue(MMS_RESPONSE_STATUS));
    std::cout << "ResponseStatus: " << responseStatus << std::endl;
    std::cout << "======== End SendConfEncodeTest =============" << std::endl;
}

void MmsMsgTest::MmsNotificationIndDecodeTest(MmsMsg &decodeMsg) const
{
    std::cout << "========== Start NotificationIndDecodeTest ========" << std::endl;
    // Mandatory
    std::cout << "TransactionId: " << decodeMsg.GetMmsTransactionId() << std::endl;
    std::cout << "Version: " << decodeMsg.GetMmsVersion() << std::endl;
    std::cout << "MessageSize: " << decodeMsg.GetHeaderLongValue(MMS_MESSAGE_SIZE) << std::endl;
    std::cout << "Expiry: " << decodeMsg.GetHeaderLongValue(MMS_EXPIRY) << std::endl;
    std::cout << "ContentLocation: " << decodeMsg.GetHeaderStringValue(MMS_CONTENT_LOCATION) << std::endl;
    std::cout << "========== End NotificationIndDecodeTest ========" << std::endl;
}

void MmsMsgTest::MmsNotifyRespIndDecodeTest(MmsMsg &decodeMsg) const
{
    std::cout << "========= Start NotifyRespIndDecodeTest ======" << std::endl;
    // Mandatory
    std::cout << "TransactionId: " << decodeMsg.GetMmsTransactionId() << std::endl;
    std::cout << "Version: " << decodeMsg.GetMmsVersion() << std::endl;
    std::string status = std::to_string(decodeMsg.GetHeaderOctetValue(MMS_STATUS));
    std::cout << "Status: " << status << std::endl;
    std::cout << "========= End NotifyRespIndDecodeTest ======" << std::endl;
}

void MmsMsgTest::MmsRetrieveConfDecodeTest(MmsMsg &decodeMsg) const
{
    std::cout << "======== Start RetrieveConfDecodeTest =========" << std::endl;
    // Mandatory
    std::cout << "TransactionId: " << decodeMsg.GetMmsTransactionId() << std::endl;
    std::cout << "Version: " << decodeMsg.GetMmsVersion() << std::endl;
    std::cout << "Date: " << decodeMsg.GetMmsDate() << std::endl;

    std::vector<MmsAttachment> attachments;
    decodeMsg.GetAllAttachment(attachments);
    int attachmentCnt = 0;
    for (auto it : attachments) {
        std::cout << "======= part:" << attachmentCnt << " attachments infos ======" << std::endl;
        std::cout << "ContentDisposition: " << it.GetContentDisposition() << std::endl;
        std::cout << "ContentLocation: " << it.GetContentLocation() << std::endl;
        std::cout << "ContentId: " << it.GetContentId() << std::endl;
        std::cout << "FileName: " << it.GetFileName() << std::endl;
        uint32_t len = 0;
        std::unique_ptr<char[]> buff = it.GetDataBuffer(len);
        if (buff != nullptr) {
            std::cout << "attachments buffer size = " << len << std::endl;
        }
        std::cout << "======= part:" << attachmentCnt << " attachments infos ======" << std::endl;
        attachmentCnt++;
    }
    std::cout << "======== End RetrieveConfDecodeTest =========" << std::endl;
}

void MmsMsgTest::MmsAcknowledgeIndDecodeTest(MmsMsg &decodeMsg) const
{
    std::cout << "======= Start AcknowledgeIndDecodeTest ========" << std::endl;
    // Mandatory
    std::cout << "TransactionId: " << decodeMsg.GetMmsTransactionId() << std::endl;
    std::cout << "Version: " << decodeMsg.GetMmsVersion() << std::endl;
    std::cout << "======= End AcknowledgeIndDecodeTest ========" << std::endl;
}

void MmsMsgTest::MmsDeliveryIndDecodeTest(MmsMsg &decodeMsg) const
{
    std::cout << "======== Start DeliveryIndDecodeTest ==========" << std::endl;
    // Mandatory
    std::cout << "Version: " << decodeMsg.GetMmsVersion() << std::endl;
    std::cout << "MessageID: " << decodeMsg.GetHeaderStringValue(MMS_MESSAGE_ID) << std::endl;
    std::vector<MmsAddress> toAddress;
    decodeMsg.GetMmsTo(toAddress);
    for (auto itTo : toAddress) {
        std::cout << "To: " << itTo.GetAddressString() << std::endl;
    }
    std::cout << "Date: " << decodeMsg.GetMmsDate() << std::endl;
    std::string status = std::to_string(decodeMsg.GetHeaderOctetValue(MMS_STATUS));
    std::cout << "Status: " << status << std::endl;
    std::cout << "======== End DeliveryIndDecodeTest ==========" << std::endl;
}

void MmsMsgTest::MmsReadRecIndDecodeTest(MmsMsg &decodeMsg) const
{
    std::cout << "======= Start ReadRecIndDecodeTest =========" << std::endl;
    // Mandatory
    std::cout << "Version: " << decodeMsg.GetMmsVersion() << std::endl;
    std::cout << "MessageID: " << decodeMsg.GetHeaderStringValue(MMS_MESSAGE_ID) << std::endl;
    std::vector<MmsAddress> toAddress;
    decodeMsg.GetMmsTo(toAddress);
    for (auto itTo : toAddress) {
        std::cout << "To: " << itTo.GetAddressString() << std::endl;
    }
    MmsAddress fromAddress = decodeMsg.GetMmsFrom();
    std::cout << "From: " << fromAddress.GetAddressString() << std::endl;
    std::cout << "Date: " << decodeMsg.GetMmsDate() << std::endl;
    std::string status = std::to_string(decodeMsg.GetHeaderOctetValue(MMS_STATUS));
    std::cout << "Status: " << status << std::endl;
    std::cout << "======= End ReadRecIndDecodeTest =========" << std::endl;
}

void MmsMsgTest::MmsReadOrigIndDecodeTest(MmsMsg &decodeMsg) const
{
    std::cout << "======== Start MmsReadOrigIndDecodeTest ========" << std::endl;
    // Mandatory
    std::cout << "Version: " << decodeMsg.GetMmsVersion() << std::endl;
    std::cout << "MessageID: " << decodeMsg.GetHeaderStringValue(MMS_MESSAGE_ID) << std::endl;
    std::vector<MmsAddress> toAddress;
    decodeMsg.GetMmsTo(toAddress);
    for (auto itTo : toAddress) {
        std::cout << "To: " << itTo.GetAddressString() << std::endl;
    }
    MmsAddress fromAddress = decodeMsg.GetMmsFrom();
    std::cout << "From: " << fromAddress.GetAddressString() << std::endl;
    std::cout << "Date: " << decodeMsg.GetMmsDate() << std::endl;
    std::string readStatus = std::to_string(decodeMsg.GetHeaderOctetValue(MMS_READ_STATUS));
    std::cout << "ReadStatus: " << readStatus << std::endl;
    std::cout << "======== End MmsReadOrigIndDecodeTest ========" << std::endl;
}

std::string MmsMsgTest::BuffToHex(const std::unique_ptr<char[]> &buff, uint32_t len) const
{
    std::stringstream ss;
    for (std::size_t i = 0; i < len; ++i) {
        unsigned char temp = static_cast<unsigned char>(buff[i]) >> HEX_OFFSET;
        ss << "0x" << HEX_TABLE[temp] << HEX_TABLE[buff[i] & 0xf] << " ";
        if ((i % MAX_LINE_NUM == 0) && (i != 0)) {
            ss << "\r\n";
        }
    }
    return ss.str();
}

bool MmsMsgTest::WriteBufferToFile(
    const std::unique_ptr<char[]> &buff, uint32_t len, const std::string &strPathName) const
{
    FILE *pFile = nullptr;
    pFile = fopen(strPathName.c_str(), "wb");
    if (!pFile || buff == nullptr) {
        std::cout << "open file: " << strPathName << "fail!" << std::endl;
        return false;
    }
    uint32_t fileLen = fwrite(buff.get(), len, 1, pFile);
    if (fileLen > 0) {
        std::cout << "write mms buffer to file success name:" << strPathName << std::endl;
    } else {
        std::cout << "write mms buffer to file error name:" << strPathName << std::endl;
    }
    (void)fclose(pFile);
    return true;
}
} // namespace Telephony
} // namespace OHOS
