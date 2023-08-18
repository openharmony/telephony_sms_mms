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

#ifndef SMS_GSM_MESSAGE_TEST_H
#define SMS_GSM_MESSAGE_TEST_H

#include "short_message.h"

namespace OHOS {
namespace Telephony {
class SmsGsmMessageTest {
public:
    void ProcessTest() const;
    void ProcessInput(int inputCMD, bool &loopFlag) const;
    void Test7BitSubmitSms() const;
    void TestUcs2SubmitSms() const;
    void Test7BitDeliverySms() const;
    void TestUcs2DeliverySms() const;
    void TestStatusReportSms() const;
    void TestMultiPageUcs2Sms() const;
    void TestWapPushSms() const;
    void TestDataSmsDeliverySms() const;
    void TestSmsc00DeliverySms() const;
    void TestMultiPage7bitSms() const;
    void Test7BitOtherSubmitSms() const;

public:
    static std::unique_ptr<ShortMessage> gsmMessage_;
};
} // namespace Telephony
} // namespace OHOS
#endif // SMS_GSM_MESSAGE_TEST_H