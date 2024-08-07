/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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

#include "gtest/gtest.h"
#include "iremote_object.h"
#define private public
#define protected public
#include "sms_send_short_message_proxy.h"
#include "sms_delivery_short_message_proxy.h"
namespace OHOS {
namespace Telephony {
using namespace testing::ext;

class SmsServiceProxyTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};
void SmsServiceProxyTest::SetUpTestCase() {}

void SmsServiceProxyTest::TearDownTestCase() {}

void SmsServiceProxyTest::SetUp() {}

void SmsServiceProxyTest::TearDown() {}

class MockRemoteObject final : public IRemoteObject {
public:
    MockRemoteObject() : IRemoteObject(u"")
    {
    }
    int32_t GetObjectRefCount() override
    {
        return retGetObjectRefCount;
    }
    int SendRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override
    {
        return retSendRequest;
    }
    bool AddDeathRecipient(const sptr<DeathRecipient> &recipient) override
    {
        return retAddDeathRecipient;
    }
    bool RemoveDeathRecipient(const sptr<DeathRecipient> &recipient) override
    {
        return retRemoveDeathRecipient;
    }
    int Dump(int fd, const std::vector<std::u16string> &args) override
    {
        return retDump;
    }
public:
    int32_t retGetObjectRefCount;
    int     retSendRequest;
    bool    retAddDeathRecipient;
    bool    retRemoveDeathRecipient;
    int     retDump;
};

/**
 * @tc.number   Telephony_SmsMmsGtest_smsServiceProxyTest_001
 * @tc.name     smsServiceProxyTest_001
 * @tc.desc     Function test
 */
HWTEST_F(SmsServiceProxyTest, smsServiceProxyTest_001, Function | MediumTest | Level1)
{
    sptr<IRemoteObject> sptrRemoteObject = nullptr;
    ISendShortMessageCallback::SmsSendResult sendResult =
        ISendShortMessageCallback::SmsSendResult::SEND_SMS_FAILURE_UNKNOWN;

    SmsSendShortMessageProxy sendShortMessageProxy(sptrRemoteObject);
    EXPECT_EQ(sendShortMessageProxy.Remote().GetRefPtr(), nullptr);
    sendShortMessageProxy.OnSmsSendResult(sendResult);
}
/**
 * @tc.number   Telephony_SmsMmsGtest_smsServiceProxyTest_002
 * @tc.name     smsServiceProxyTest_001
 * @tc.desc     Function test
 */
HWTEST_F(SmsServiceProxyTest, smsServiceProxyTest_002, Function | MediumTest | Level1)
{
    sptr<MockRemoteObject> sptrRemoteObject = new MockRemoteObject();
    sptrRemoteObject->retGetObjectRefCount = 1;
    sptrRemoteObject->retSendRequest = 0;
    
    ISendShortMessageCallback::SmsSendResult sendResult =
        ISendShortMessageCallback::SmsSendResult::SEND_SMS_FAILURE_UNKNOWN;
    SmsSendShortMessageProxy sendShortMessageProxy(sptrRemoteObject);
    EXPECT_EQ(sendShortMessageProxy.Remote().GetRefPtr(), sptrRemoteObject);
    sendShortMessageProxy.OnSmsSendResult(sendResult);
}
/**
 * @tc.number   Telephony_SmsMmsGtest_smsServiceProxyTest_003
 * @tc.name     smsServiceProxyTest_001
 * @tc.desc     Function test
 */
HWTEST_F(SmsServiceProxyTest, smsServiceProxyTest_003, Function | MediumTest | Level1)
{
    sptr<IRemoteObject> sptrRemoteObject = nullptr;
    std::u16string strPdu = u"";
    SmsDeliveryShortMessageProxy sendShortMessageProxy(sptrRemoteObject);
    EXPECT_EQ(sendShortMessageProxy.Remote().GetRefPtr(), nullptr);
    sendShortMessageProxy.OnSmsDeliveryResult(strPdu);
}
/**
 * @tc.number   Telephony_SmsMmsGtest_smsServiceProxyTest_004
 * @tc.name     smsServiceProxyTest_001
 * @tc.desc     Function test
 */
HWTEST_F(SmsServiceProxyTest, smsServiceProxyTest_004, Function | MediumTest | Level1)
{
    sptr<MockRemoteObject> sptrRemoteObject = new MockRemoteObject();
    sptrRemoteObject->retGetObjectRefCount = 1;
    sptrRemoteObject->retSendRequest = 0;

    std::u16string strPdu = u"";
    SmsDeliveryShortMessageProxy sendShortMessageProxy(sptrRemoteObject);
    EXPECT_EQ(sendShortMessageProxy.Remote().GetRefPtr(), sptrRemoteObject);
    sendShortMessageProxy.OnSmsDeliveryResult(strPdu);
}
}
}