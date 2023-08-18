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

#include "sms_gsm_message_test.h"

#include <iostream>

#include "access_mms_token.h"
#include "cdma_sms_message.h"
#include "gsm_cb_codec.h"

namespace OHOS {
namespace Telephony {
using namespace OHOS::Telephony;
std::unique_ptr<ShortMessage> SmsGsmMessageTest::gsmMessage_ = nullptr;

void SmsGsmMessageTest::ProcessInput(int inputCMD, bool &loopFlag) const
{
    std::cout << "inputCMD is:" << inputCMD << std::endl;
    switch (inputCMD) {
        case 0x00:
            Test7BitSubmitSms();
            break;
        case 0x01:
            TestUcs2SubmitSms();
            break;
        case 0x02:
            Test7BitDeliverySms();
            break;
        case 0x03:
            TestUcs2DeliverySms();
            break;
        case 0x04:
            TestStatusReportSms();
            break;
        case 0x05:
            TestMultiPageUcs2Sms();
            break;
        case 0x06:
            TestWapPushSms();
            break;
        case 0x07:
            TestDataSmsDeliverySms();
            break;
        case 0x08:
            TestSmsc00DeliverySms();
            break;
        case 0x09:
            TestMultiPage7bitSms();
            break;
        case 0x0a:
            Test7BitOtherSubmitSms();
            break;
        default:
            loopFlag = false;
            break;
    }
}

void SmsGsmMessageTest::ProcessTest() const
{
    bool loopFlag = true;
    while (loopFlag) {
        std::cout << "\nusage:please input a cmd num:\n"
                     "0:Test7BitSubmitSms\n"
                     "1:TestUcs2SubmitSms\n"
                     "2:Test7BitDeliverySms\r\n"
                     "3:TestUcs2DeliverySms\r\n"
                     "4:TestStatusReportSms\r\n"
                     "5:TestMultiPageUcs2Sms\r\n"
                     "6:TestWapPushSms\r\n"
                     "7:TestDataSmsDeliverySms\r\n"
                     "8:TestSmsc00DeliverySms\r\n"
                     "9:TestMultiPage7bitSms\r\n"
                     "10:Test7BitOtherSubmitSms\r\n"
                     "Other key:exit \n\n";

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

void SmsGsmMessageTest::Test7BitSubmitSms() const
{
    AccessMmsToken token;
    std::vector<unsigned char> pdu = StringUtils::HexToByteVector("21010B818176251308F4000002C130");
    ShortMessage *message = new ShortMessage();
    if (message == nullptr) {
        std::cout << "message is nullptr!" << std::endl;
        return;
    }
    ShortMessage::CreateMessage(pdu, u"3gpp", *message);
    gsmMessage_ = std::unique_ptr<ShortMessage>(message);
    if (gsmMessage_ == nullptr) {
        std::cout << "Test7BitSubmitSms fail!!!" << std::endl;
    } else {
        std::cout << "Test7BitSubmitSms success!!!" << std::endl;
    }
}

void SmsGsmMessageTest::TestUcs2SubmitSms() const
{
    AccessMmsToken token;
    std::vector<unsigned char> pdu = StringUtils::HexToByteVector("21020B818176251308F40008046D4B8BD5");
    ShortMessage *message = new ShortMessage();
    if (message == nullptr) {
        std::cout << "message is nullptr!" << std::endl;
        return;
    }
    ShortMessage::CreateMessage(pdu, u"3gpp", *message);
    gsmMessage_ = std::unique_ptr<ShortMessage>(message);
    if (gsmMessage_ == nullptr) {
        std::cout << "TestUcs2SubmitSms fail!!!" << std::endl;
    } else {
        std::cout << "TestUcs2SubmitSms success!!!" << std::endl;
    }
}

void SmsGsmMessageTest::Test7BitDeliverySms() const
{
    AccessMmsToken token;
    std::vector<unsigned char> pdu =
        StringUtils::HexToByteVector("0891683108200075F4240D91688129562983F600001240800102142302C130");
    ShortMessage *message = new ShortMessage();
    if (message == nullptr) {
        std::cout << "message is nullptr!" << std::endl;
        return;
    }
    ShortMessage::CreateMessage(pdu, u"3gpp", *message);
    gsmMessage_ = std::unique_ptr<ShortMessage>(message);
    if (gsmMessage_ == nullptr) {
        std::cout << "Test7BitDeliverySms fail!!!" << std::endl;
    } else {
        std::cout << "Test7BitDeliverySms success!!!" << std::endl;
    }
}

void SmsGsmMessageTest::TestUcs2DeliverySms() const
{
    AccessMmsToken token;
    std::vector<unsigned char> pdu =
        StringUtils::HexToByteVector("0891683110206005F0240DA1688176251308F4000832500381459323044F60597D");
    ShortMessage *message = new ShortMessage();
    if (message == nullptr) {
        std::cout << "message is nullptr!" << std::endl;
        return;
    }
    ShortMessage::CreateMessage(pdu, u"3gpp", *message);
    gsmMessage_ = std::unique_ptr<ShortMessage>(message);
    if (gsmMessage_ == nullptr) {
        std::cout << "TestUcs2DeliverySms fail!!!" << std::endl;
    } else {
        std::cout << "TestUcs2DeliverySms success!!!" << std::endl;
    }
}

void SmsGsmMessageTest::TestStatusReportSms() const
{
    AccessMmsToken token;
    std::vector<unsigned char> pdu =
        StringUtils::HexToByteVector("0891683110808805F006510B818176251308F4325013113382233250131143802300");

    ShortMessage *message = new ShortMessage();
    if (message == nullptr) {
        std::cout << "message is nullptr!" << std::endl;
        return;
    }
    ShortMessage::CreateMessage(pdu, u"3gpp", *message);
    gsmMessage_ = std::unique_ptr<ShortMessage>(message);
    if (gsmMessage_ == nullptr) {
        std::cout << "TestStatusReportSms fail!!!" << std::endl;
    } else {
        std::cout << "TestStatusReportSms success!!!" << std::endl;
    }
}

void SmsGsmMessageTest::TestMultiPageUcs2Sms() const
{
    AccessMmsToken token;
    std::vector<unsigned char> pdu = StringUtils::HexToByteVector(
        "0891683110205005F06005A00110F00008325052214182238C050003D3030200310030002E0063006E002F007100410053004B00380020"
        "FF0C4EE5514D6B216708521D6263966476F851738D397528540E5F7154CD60A876846B635E384F7F752830024E2D56FD"
        "8054901A0041005000507545723D65B04F539A8CFF0C70B951FB0020002000680074007400700073003A002F002F0075002E0031003000"
        "3000310030002E");

    ShortMessage *message = new ShortMessage();
    if (message == nullptr) {
        std::cout << "message is nullptr!" << std::endl;
        return;
    }
    ShortMessage::CreateMessage(pdu, u"3gpp", *message);
    gsmMessage_ = std::unique_ptr<ShortMessage>(message);
    if (gsmMessage_ == nullptr) {
        std::cout << "TestMultiPageUcs2Sms fail!!!" << std::endl;
    } else {
        std::cout << "TestMultiPageUcs2Sms success!!!" << std::endl;
    }
}

void SmsGsmMessageTest::TestWapPushSms() const
{
    AccessMmsToken token;
    std::vector<unsigned char> pdu = StringUtils::HexToByteVector(
        "0891683110205005F0640BA10156455102F1000432109261715023880605040B8423F04C06246170706C69636174696F6E2F766E642E77"
        "61702E6D6D732D6D65737361676500B487AF848C829850765030303031365A645430008D9089178031363630373532313930382F545950"
        "453D504C4D4E008A808E040001298D"
        "8805810303F47B83687474703A2F2F31302E3132332E31382E38303A3138302F76564F455F3000");

    ShortMessage *message = new ShortMessage();
    if (message == nullptr) {
        std::cout << "message is nullptr!" << std::endl;
        return;
    }
    ShortMessage::CreateMessage(pdu, u"3gpp", *message);
    gsmMessage_ = std::unique_ptr<ShortMessage>(message);
    if (gsmMessage_ == nullptr) {
        std::cout << "TestWapPushSms fail!!!" << std::endl;
    } else {
        std::cout << "TestWapPushSms success!!!" << std::endl;
    }
}

void SmsGsmMessageTest::TestDataSmsDeliverySms() const
{
    AccessMmsToken token;
    std::vector<unsigned char> pdu = StringUtils::HexToByteVector(
        "0891683110808805F0640D91686106571209F80000327030021205231306050400640000E8329BFD06DDDF723619");

    ShortMessage *message = new ShortMessage();
    if (message == nullptr) {
        std::cout << "message is nullptr!" << std::endl;
        return;
    }
    ShortMessage::CreateMessage(pdu, u"3gpp", *message);
    gsmMessage_ = std::unique_ptr<ShortMessage>(message);
    if (gsmMessage_ == nullptr) {
        std::cout << "TestDataSmsDeliverySms fail!!!" << std::endl;
    } else {
        std::cout << "TestDataSmsDeliverySms success!!!" << std::endl;
    }
}

void SmsGsmMessageTest::TestSmsc00DeliverySms() const
{
    AccessMmsToken token;
    std::vector<unsigned char> pdu = StringUtils::HexToByteVector("00240D91689141468496F600001270721142432302B319");

    ShortMessage *message = new ShortMessage();
    if (message == nullptr) {
        std::cout << "message is nullptr!" << std::endl;
        return;
    }
    ShortMessage::CreateMessage(pdu, u"3gpp", *message);
    gsmMessage_ = std::unique_ptr<ShortMessage>(message);
    if (gsmMessage_ == nullptr) {
        std::cout << "TestSmsc00DeliverySms fail!!!" << std::endl;
    } else {
        std::cout << "TestSmsc00DeliverySms success!!!" << std::endl;
    }
}

void SmsGsmMessageTest::TestMultiPage7bitSms() const
{
    AccessMmsToken token;
    std::vector<unsigned char> pdu = StringUtils::HexToByteVector(
        "0891683110206005F0640DA1688176251308F4000032806190051123A00500030F020162B1582C168BC562B1582C168BC562B2198D369B"
        "CD68B5582C269BCD62B1582C168BC562B1582C168BC562B1582C168BC562B1582C168BC562B1582C168BC562B1582C168BC562B1582C16"
        "8BC562B1582C168BC562B1582C168BC562B1582C168BC562B1582C168BC562B1582C168BC540B1582C168BC562B1582C168BC56231D98C"
        "469BCD66");

    ShortMessage *message = new ShortMessage();
    if (message == nullptr) {
        std::cout << "message is nullptr!" << std::endl;
        return;
    }
    ShortMessage::CreateMessage(pdu, u"3gpp", *message);
    gsmMessage_ = std::unique_ptr<ShortMessage>(message);
    if (gsmMessage_ == nullptr) {
        std::cout << "TestMultiPage7bitSms fail!!!" << std::endl;
    } else {
        std::cout << "TestMultiPage7bitSms success!!!" << std::endl;
    }
}

void SmsGsmMessageTest::Test7BitOtherSubmitSms() const
{
    AccessMmsToken token;
    std::vector<unsigned char> pdu = StringUtils::HexToByteVector("00010005910110F0000003E17018");
    ShortMessage *message = new ShortMessage();
    if (message == nullptr) {
        std::cout << "message is nullptr!" << std::endl;
        return;
    }
    ShortMessage::CreateMessage(pdu, u"3gpp", *message);
    gsmMessage_ = std::unique_ptr<ShortMessage>(message);
    if (gsmMessage_ == nullptr) {
        std::cout << "Test7BitOtherSubmitSms fail!!!" << std::endl;
    } else {
        std::cout << "Test7BitOtherSubmitSms success!!!" << std::endl;
    }
}
} // namespace Telephony
} // namespace OHOS