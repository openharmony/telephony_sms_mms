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
#include "mock/mock_remote_object.h"
#include "sms_persist_helper.h"
#include "sms_service_proxy.h"
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

constexpr uint32_t EVENT_RELEASE_DATA_SHARE_HELPER = 10000;
void SmsServiceProxyTest::TearDownTestCase()
{
    DelayedSingleton<SmsPersistHelper>::GetInstance()->RemoveEvent(EVENT_RELEASE_DATA_SHARE_HELPER);
}

void SmsServiceProxyTest::SetUp() {}

void SmsServiceProxyTest::TearDown() {}

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
/**
 * @tc.number   Telephony_SmsMmsGtest_smsServiceProxyTest_005
 * @tc.name     smsServiceProxyTest_GetSmsShortCodeType_1
 * @tc.desc     Function test
 */
HWTEST_F(SmsServiceProxyTest, smsServiceProxyTest_GetSmsShortCodeType_1, Function | MediumTest | Level1)
{
    sptr<IRemoteObject> sptrRemoteObject = nullptr;
    SmsServiceProxy smsServiceProxy(sptrRemoteObject);
    int32_t slotId = 0;
    std::string desAddr = "10086";
    int32_t smsShortCodeType = -1;
    EXPECT_EQ(smsServiceProxy.Remote().GetRefPtr(), nullptr);
    smsServiceProxy.GetSmsShortCodeType(slotId, desAddr, smsShortCodeType);
}
/**
 * @tc.number   Telephony_SmsMmsGtest_smsServiceProxyTest_006
 * @tc.name     smsServiceProxyTest_GetSmsShortCodeType_2
 * @tc.desc     Function test
 */
HWTEST_F(SmsServiceProxyTest, smsServiceProxyTest_GetSmsShortCodeType_2, Function | MediumTest | Level1)
{
    sptr<MockRemoteObject> sptrRemoteObject = new MockRemoteObject();
    sptrRemoteObject->retGetObjectRefCount = 1;
    sptrRemoteObject->retSendRequest = 0;
    SmsServiceProxy smsServiceProxy(sptrRemoteObject);
    int32_t slotId = 0;
    std::string desAddr = "10650";
    int32_t smsShortCodeType = -1;
    EXPECT_EQ(smsServiceProxy.Remote().GetRefPtr(), sptrRemoteObject);
    EXPECT_EQ(smsServiceProxy.GetSmsShortCodeType(slotId, desAddr, smsShortCodeType), 0);
    EXPECT_EQ(smsShortCodeType, 0);
}
}
}