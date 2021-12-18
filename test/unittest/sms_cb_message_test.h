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

#ifndef SMS_CB_MESSAGE_TEST_H
#define SMS_CB_MESSAGE_TEST_H

namespace OHOS {
namespace Telephony {
class SmsCbMessageTest {
public:
    void ProcessTest() const;
    void ProcessInput(int inputCMD, bool &loopFlag) const;
    void TestMessageBody7Bit() const;
    void TestMessageBody7BitUmts() const;
    void TestMessageBody8Bit() const;
    void TestMessageBodyUcs2() const;
    void TestMessageBodyUcs2Umts() const;
    void TestEtwsWarningType0() const;
    void TestEtwsWarningType1() const;
    void TestEtwsWarningType2() const;
    void TestEtwsWarningType3() const;
    void TestEtwsWarningType4() const;
    void TestMessageBody7BitGs2() const;
    void TestCdmaUserData() const;
    void TestCdmaCmasType1() const;
private:
};
} // namespace Telephony
} // namespace OHOS
#endif // SMS_CB_MESSAGE_TEST_H