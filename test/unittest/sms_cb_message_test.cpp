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

#include "sms_cb_message_test.h"

#include <iostream>
#include <vector>

#include "cdma_sms_message.h"
#include "sms_cb_message.h"

namespace OHOS {
namespace Telephony {
void SmsCbMessageTest::ProcessInput(int inputCMD, bool &loopFlag) const
{
    std::cout << "inputCMD is:" << inputCMD << std::endl;
    switch (inputCMD) {
        case 0x00:
            TestMessageBody7Bit();
            break;
        case 0x01:
            TestMessageBody7BitUmts();
            break;
        case 0x02:
            TestMessageBody8Bit();
            break;
        case 0x03:
            TestMessageBodyUcs2();
            break;
        case 0x04:
            TestMessageBodyUcs2Umts();
            break;
        case 0x05:
            TestEtwsWarningType0();
            break;
        case 0x06:
            TestEtwsWarningType1();
            break;
        case 0x07:
            TestEtwsWarningType2();
            break;
        case 0x08:
            TestEtwsWarningType3();
            break;
        case 0x09:
            TestEtwsWarningType4();
            break;
        case 0x0A:
            TestMessageBody7BitGs2();
            break;
        case 0x0B:
            TestCdmaUserData();
            break;
        case 0x0C:
            TestCdmaCmasType1();
            break;
        default:
            loopFlag = false;
            break;
    }
}

void SmsCbMessageTest::ProcessTest() const
{
    bool loopFlag = true;
    while (loopFlag) {
        std::cout << "\n-----------start test cbmessage api--------------\n"
                     "usage:please input a cmd num:\n"
                     "0:TestMessageBody7Bit\n"
                     "1:TestMessageBody7BitUmts\n"
                     "2:TestMessageBody8Bit\r\n"
                     "3:TestMessageBodyUcs2\r\n"
                     "4:TestMessageBodyUcs2Umts\r\n"
                     "5:TestEtwsWarningType0\r\n"
                     "6:TestEtwsWarningType1\r\n"
                     "7:TestEtwsWarningType2\r\n"
                     "8:TestEtwsWarningType3\r\n"
                     "9:TestEtwsWarningType4\r\n"
                     "10:TestMessageBody7BitGs2\r\n"
                     "11:TestCdmaUserData\r\n"
                     "12:TestCdmaCmasType1\r\n"
                     "Other key:exit \n"
                  << std::endl;

        int inputCMD = 0;
        std::cin >> inputCMD;
        while (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore();
            std::cin >> inputCMD;
        }
        ProcessInput(inputCMD, loopFlag);
    }
}

void SmsCbMessageTest::TestMessageBody7Bit() const
{
    std::string data(
        "C0000032401174747A0E4ACF41E8B0BCFD76E741EF39685C66B34162F93B4C1"
        "E87E77410BD3CA7836EC2341D440ED3C321");

    std::shared_ptr<SmsCbMessage> msg = SmsCbMessage::CreateCbMessage(data);
    if (msg == nullptr) {
        std::cout << "CreateCbMessage fail." << std::endl;
    } else {
        std::cout << "msginfo: " << msg->ToString() << std::endl;
    }
}

void SmsCbMessageTest::TestMessageBody7BitUmts() const
{
    std::string data(
        "010032C000400174747A0E4ACF41E8B0BCFD76E741EF39685C66B3C5F277983"
        "C0ECFE9207A794E07DD84693AA8EC1EBFC96550B54D9F83C8617A9845479741"
        "37719A0EAAB7E973D038EC060DC372791ED47ECBCB2072981E0652");

    std::shared_ptr<SmsCbMessage> msg = SmsCbMessage::CreateCbMessage(data);
    if (msg == nullptr) {
        std::cout << "CreateCbMessage fail." << std::endl;
    } else {
        std::cout << "msginfo: " << msg->ToString() << std::endl;
    }
}

void SmsCbMessageTest::TestMessageBody8Bit() const
{
    std::string data(
        "C0000032441174686973206973206861726D6F6E79206F732063656C6C20627"
        "26F6164636173742074657374203842697420656E636F64652064617461");

    std::shared_ptr<SmsCbMessage> msg = SmsCbMessage::CreateCbMessage(data);
    if (msg == nullptr) {
        std::cout << "CreateCbMessage fail." << std::endl;
    } else {
        std::cout << "msginfo: " << msg->ToString() << std::endl;
    }
}

void SmsCbMessageTest::TestMessageBodyUcs2() const
{
    std::string data(
        "C00000324811006800610072006D006F006E00790020006F00730020005500"
        "630073003200200065006E0063006F0064006500200064006100740061");

    std::shared_ptr<SmsCbMessage> msg = SmsCbMessage::CreateCbMessage(data);
    if (msg == nullptr) {
        std::cout << "CreateCbMessage fail." << std::endl;
    } else {
        std::cout << "msginfo: " << msg->ToString() << std::endl;
    }
}

void SmsCbMessageTest::TestMessageBodyUcs2Umts() const
{
    std::string data(
        "01a41f51101102ea3030a830ea30a230e130fc30eb914d4fe130c630b930c8000"
        "d000a3053308c306f8a669a137528306e30e130c330bb30fc30b8306730593002"
        "000d000aff080032003000310033002f00310031002f003252ea3000370020003"
        "10035003a00340034ff09000d000aff0830a830ea30a25e02ff09000000000000"
        "00000000000000000000000000000000000000000000000000000000000000000"
        "000000000000000000022");

    std::shared_ptr<SmsCbMessage> msg = SmsCbMessage::CreateCbMessage(data);
    if (msg == nullptr) {
        std::cout << "CreateCbMessage fail." << std::endl;
    } else {
        std::cout << "msginfo: " << msg->ToString() << std::endl;
    }
}

void SmsCbMessageTest::TestEtwsWarningType0() const
{
    std::string data(
      "0000110011010D0A004800610072006D006F006E00790020004F007300200045007400"
      "770073005700610072006E0069006E00670020004D0065007300730061006700650055"
      "00630073003200200045006D00740073");
    std::shared_ptr<SmsCbMessage> msg = SmsCbMessage::CreateCbMessage(data);
    if (msg == nullptr) {
        std::cout << "CreateCbMessage fail." << std::endl;
    } else {
        std::cout << "msginfo: " << msg->ToString() << std::endl;
    }
}

void SmsCbMessageTest::TestEtwsWarningType1() const
{
    std::string data(
      "0000110111010D0A004800610072006D006F006E00790020004F007300200045007400"
      "770073005700610072006E0069006E00670020004D0065007300730061006700650055"
      "00630073003200200045006D00740073");

    std::shared_ptr<SmsCbMessage> msg = SmsCbMessage::CreateCbMessage(data);
    if (msg == nullptr) {
        std::cout << "CreateCbMessage fail." << std::endl;
    } else {
        std::cout << "msginfo: " << msg->ToString() << std::endl;
    }
}

void SmsCbMessageTest::TestEtwsWarningType2() const
{
    std::string data(
      "0000110211010D0A004800610072006D006F006E00790020004F007300200045007400"
      "770073005700610072006E0069006E00670020004D0065007300730061006700650055"
      "00630073003200200045006D00740073");

    std::shared_ptr<SmsCbMessage> msg = SmsCbMessage::CreateCbMessage(data);
    if (msg == nullptr) {
        std::cout << "CreateCbMessage fail." << std::endl;
    } else {
        std::cout << "msginfo: " << msg->ToString() << std::endl;
    }
}

void SmsCbMessageTest::TestEtwsWarningType3() const
{
    std::string data(
        "0000110311010D0A5BAE57CE770C531790E85C716CBF3044573065B93067573097"
        "07300263FA308C306B5099304830664E0B30553044FF086C178C615E81FF090000"
        "0000000000000000000000000000000000000000");
    std::shared_ptr<SmsCbMessage> msg = SmsCbMessage::CreateCbMessage(data);
    if (msg == nullptr) {
        std::cout << "CreateCbMessage fail." << std::endl;
    } else {
        std::cout << "msginfo: " << msg->ToString() << std::endl;
    }
}

void SmsCbMessageTest::TestEtwsWarningType4() const
{
    std::string data(
      "0000110411010D0A004800610072006D006F006E00790020004F007300200045007400"
      "770073005700610072006E0069006E00670020004D0065007300730061006700650055"
      "00630073003200200045006D00740073");

    std::shared_ptr<SmsCbMessage> msg = SmsCbMessage::CreateCbMessage(data);
    if (msg == nullptr) {
        std::cout << "CreateCbMessage fail." << std::endl;
    } else {
        std::cout << "msginfo: " << msg->ToString() << std::endl;
    }
}

void SmsCbMessageTest::TestMessageBody7BitGs2() const
{
    std::string data(
        "80000032401174747A0E4ACF41E8B0BCFD76E741EF39685C66B34162F93B4C1"
        "E87E77410BD3CA7836EC2341D440ED3C321");

    std::shared_ptr<SmsCbMessage> msg = SmsCbMessage::CreateCbMessage(data);
    if (msg == nullptr) {
        std::cout << "CreateCbMessage fail." << std::endl;
    } else {
        std::cout << "msginfo: " << msg->ToString() << std::endl;
    }
}

void SmsCbMessageTest::TestCdmaUserData() const
{
    std::string pdu = "0101020004081300031008d00106102c2870e1420801c00c01c0";
    std::shared_ptr<CdmaSmsMessage> message = CdmaSmsMessage::CreateMessage(pdu);
    if (message == nullptr) {
        std::cout << "message is nullptr!" << std::endl;
        return;
    }
    std::cout << message->GetCbInfo() << std::endl;
}

void SmsCbMessageTest::TestCdmaCmasType1() const
{
    std::string pdu = "0101021000081500031008d001080100000800c1E1000801c00c01c0";
    std::shared_ptr<CdmaSmsMessage> message = CdmaSmsMessage::CreateMessage(pdu);
    if (message == nullptr) {
        std::cout << "message is nullptr!" << std::endl;
        return;
    }
    std::cout << message->GetCbInfo() << std::endl;
}
} // namespace Telephony
} // namespace OHOS