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

#include "short_message_test.h"

#include <iostream>

#include "string_utils.h"

namespace OHOS {
namespace Telephony {
using namespace OHOS::Telephony;
std::unique_ptr<ShortMessage> ShortMessageTest::shortMessage_ = nullptr;

void ShortMessageTest::TestCreateMessage() const
{
    std::vector<unsigned char> pdu = StringUtils::HexToByteVector(
        "0891683108200075F4240D91688129562983F6000012408"
        "00102142302C130");
    ShortMessage *message = new ShortMessage();
    if (message == nullptr) {
        std::cout << "message is nullptr!" << std::endl;
        return;
    }
    ShortMessage::CreateMessage(pdu, u"3gpp", *message);
    shortMessage_ = std::unique_ptr<ShortMessage>(message);
    if (shortMessage_ == nullptr) {
        std::cout << "TestCreateMessage fail!!!" << std::endl;
    } else {
        std::cout << "TestCreateMessage success!!!" << std::endl;
    }
}

void ShortMessageTest::Test3Gpp2CreateMessage() const
{
    std::vector<unsigned char> pdu = StringUtils::HexToByteVector(
        "0000021002020702c6049064c4d40601fc081b00031000200106102e8cbb366f03061409011126310a01400d0101");
    ShortMessage *message = new ShortMessage();
    if (message == nullptr) {
        std::cout << "message is nullptr!" << std::endl;
        return;
    }
    ShortMessage::CreateMessage(pdu, u"3gpp2", *message);
    shortMessage_ = std::unique_ptr<ShortMessage>(message);
    if (shortMessage_ == nullptr) {
        std::cout << "TestCreateMessage Cdma fail!!!" << std::endl;
    } else {
        std::cout << "TestCreateMessage Cdma success!!!" << std::endl;
    }
}

void ShortMessageTest::TestGetVisibleMessageBody() const
{
    if (shortMessage_ == nullptr) {
        std::cout << "please create a short message!" << std::endl;
        return;
    }
    std::cout << "GetVisibleMessageBody = " << StringUtils::ToUtf8(shortMessage_->GetVisibleMessageBody())
              << std::endl;
}

void ShortMessageTest::TestShowShortMessage() const
{
    if (shortMessage_ == nullptr) {
        std::cout << "please create a short message!" << std::endl;
        return;
    }
    std::u16string smscAddress;
    shortMessage_->GetScAddress(smscAddress);
    std::cout << "GetSmscAddr = " << StringUtils::ToUtf8(smscAddress) << std::endl;
    std::cout << "GetVisibleMessageBody = " << StringUtils::ToUtf8(shortMessage_->GetVisibleMessageBody())
              << std::endl;
    std::cout << "GetVisibleRawAddress = " << StringUtils::ToUtf8(shortMessage_->GetVisibleRawAddress())
              << std::endl;
    int64_t time = shortMessage_->GetScTimestamp();
    std::cout << "GetScTimestamp = " << ctime(static_cast<time_t *>(&(time)));
    std::cout << "GetProtocolId = " << shortMessage_->GetProtocolId() << std::endl;
    std::cout << "GetStatus = " << shortMessage_->GetStatus() << std::endl;
    std::cout << "GetMessageClass = " << shortMessage_->GetMessageClass() << std::endl;
    std::cout << "HasReplyPath = " << shortMessage_->HasReplyPath() << std::endl;
    std::cout << "IsSmsStatusReportMessage = " << shortMessage_->IsSmsStatusReportMessage() << std::endl;
    std::cout << "IsReplaceMessage = " << shortMessage_->IsReplaceMessage() << std::endl;
    std::cout << "HasReplyPath = " << shortMessage_->HasReplyPath() << std::endl;
    std::cout << "raw pdu = " << StringUtils::StringToHex(shortMessage_->GetPdu()) << std::endl;
}
} // namespace Telephony
} // namespace OHOS