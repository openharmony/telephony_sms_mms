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
#define private public
#define protected public
#include "ims_sms_client.h"
#include "ims_sms_proxy.h"
#include "ims_sms_callback_stub.h"

namespace OHOS {
namespace Telephony {
using namespace testing::ext;

class SmsImsServiceInteractionTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};
void SmsImsServiceInteractionTest::SetUpTestCase() {}
void SmsImsServiceInteractionTest::TearDownTestCase() {}
void SmsImsServiceInteractionTest::SetUp() {}
void SmsImsServiceInteractionTest::TearDown() {}

class MockImsSms : public ImsSmsInterface {
public:
    int32_t ImsSendMessage(int32_t slotId, const ImsMessageInfo &imsMessageInfo) override
    {
        return retImsSendMessage;
    }
    int32_t ImsSetSmsConfig(int32_t slotId, int32_t imsSmsConfig) override
    {
        return retImsSetSmsConfig;
    }
    int32_t ImsGetSmsConfig(int32_t slotId) override
    {
        return retImsGetSmsConfig;
    }
    int32_t RegisterImsSmsCallback(const sptr<ImsSmsCallbackInterface> &callback) override
    {
        return retRegisterImsSmsCallback;
    }
    sptr<IRemoteObject> AsObject() override
    {
        return retRemoteObject;
    }
public:
    int32_t retImsSendMessage;
    int32_t retImsSetSmsConfig;
    int32_t retImsGetSmsConfig;
    int32_t retRegisterImsSmsCallback;
    IRemoteObject* retRemoteObject;
};

class MockImsCoreService : public ImsCoreServiceInterface {
public:
    int32_t GetImsRegistrationStatus(int32_t slotId) override
    {
        return retGetImsRegistrationStatus;
    }
    int32_t RegisterImsCoreServiceCallback(const sptr<ImsCoreServiceCallbackInterface> &callback) override
    {
        return retRegisterImsCoreServiceCallback;
    }
    sptr<IRemoteObject> GetProxyObjectPtr(ImsServiceProxyType proxyType) override
    {
        return retGetProxyObjectPtr;
    }
    int32_t GetPhoneNumberFromIMPU(int32_t slotId, std::string &phoneNumber) override
    {
        return retGetPhoneNumberFromIMPU;
    }
    sptr<IRemoteObject> AsObject() override
    {
        return retRemoteObject;
    }
public:
    int32_t retGetImsRegistrationStatus;
    int32_t retRegisterImsCoreServiceCallback;
    IRemoteObject* retGetProxyObjectPtr;
    int32_t retGetPhoneNumberFromIMPU;
    IRemoteObject* retRemoteObject;
};

class MockImsSmsCallback : public ImsSmsCallbackInterface {
public:
    int32_t ImsSendMessageResponse(int32_t slotId, const SendSmsResultInfo &result) override
    {
        return retImsSendMessageResponse1;
    }
    int32_t ImsSendMessageResponse(int32_t slotId, const RadioResponseInfo &info) override
    {
        return retImsSendMessageResponse2;
    }
    int32_t ImsSetSmsConfigResponse(int32_t slotId, const RadioResponseInfo &info) override
    {
        return retImsSetSmsConfigResponse;
    }
    int32_t ImsGetSmsConfigResponse(int32_t slotId, int32_t imsSmsConfig) override
    {
        return retImsGetSmsConfigResponse1;
    }
    int32_t ImsGetSmsConfigResponse(int32_t slotId, const RadioResponseInfo &info) override
    {
        return retImsGetSmsConfigResponse2;
    }
    sptr<IRemoteObject> AsObject() override
    {
        return retRemoteObject;
    }
public:
    int32_t retImsSendMessageResponse1;
    int32_t retImsSendMessageResponse2;
    int32_t retImsSetSmsConfigResponse;
    int32_t retImsGetSmsConfigResponse1;
    int32_t retImsGetSmsConfigResponse2;
    IRemoteObject* retRemoteObject;
};

/**
 * @tc.number   Telephony_SmsMmsGtest_smsImsServiceInteractionTest_001
 * @tc.name     smsServiceProxyTest_001
 * @tc.desc     Function test
 */
HWTEST_F(SmsImsServiceInteractionTest, smsImsServiceInteractionTest_001, Function | MediumTest | Level1)
{
    sptr<ImsSmsInterface> pImsSms = new MockImsSms();
    DelayedSingleton<ImsSmsClient>::GetInstance()->imsSmsProxy_ = pImsSms;
    DelayedSingleton<ImsSmsClient>::GetInstance()->Init();
    DelayedSingleton<ImsSmsClient>::GetInstance()->GetImsSmsProxy();
    EXPECT_EQ(DelayedSingleton<ImsSmsClient>::GetInstance()->IsConnect(), true);
    DelayedSingleton<ImsSmsClient>::DestroyInstance();
}

/**
 * @tc.number   Telephony_SmsMmsGtest_smsImsServiceInteractionTest_002
 * @tc.name     smsServiceProxyTest_001
 * @tc.desc     Function test
 */
HWTEST_F(SmsImsServiceInteractionTest, smsImsServiceInteractionTest_002, Function | MediumTest | Level1)
{
    EXPECT_EQ(DelayedSingleton<ImsSmsClient>::GetInstance()->RegisterImsSmsCallback(), TELEPHONY_ERR_LOCAL_PTR_NULL);
    DelayedSingleton<ImsSmsClient>::DestroyInstance();
}

/**
 * @tc.number   Telephony_SmsMmsGtest_smsImsServiceInteractionTest_003
 * @tc.name     smsServiceProxyTest_001
 * @tc.desc     Function test
 */
HWTEST_F(SmsImsServiceInteractionTest, smsImsServiceInteractionTest_003, Function | MediumTest | Level1)
{
    sptr<MockImsSms> pImsSms = new MockImsSms();
    pImsSms->retRegisterImsSmsCallback = 1;
    DelayedSingleton<ImsSmsClient>::GetInstance()->imsSmsProxy_ = pImsSms;
    EXPECT_EQ(DelayedSingleton<ImsSmsClient>::GetInstance()->RegisterImsSmsCallback(), TELEPHONY_ERR_FAIL);
    DelayedSingleton<ImsSmsClient>::DestroyInstance();
}

/**
 * @tc.number   Telephony_SmsMmsGtest_smsImsServiceInteractionTest_004
 * @tc.name     smsServiceProxyTest_001
 * @tc.desc     Function test
 */
HWTEST_F(SmsImsServiceInteractionTest, smsImsServiceInteractionTest_004, Function | MediumTest | Level1)
{
    sptr<MockImsSms> pImsSms = new MockImsSms();
    pImsSms->retImsSendMessage = 1;
    DelayedSingleton<ImsSmsClient>::GetInstance()->imsSmsProxy_ = pImsSms;
    int32_t slotId = 0;
    ImsMessageInfo imsMessageInfo;
    EXPECT_EQ(DelayedSingleton<ImsSmsClient>::GetInstance()->ImsSendMessage(slotId, imsMessageInfo),
        pImsSms->retImsSendMessage);
    DelayedSingleton<ImsSmsClient>::DestroyInstance();
}

/**
 * @tc.number   Telephony_SmsMmsGtest_smsImsServiceInteractionTest_005
 * @tc.name     smsServiceProxyTest_001
 * @tc.desc     Function test
 */
HWTEST_F(SmsImsServiceInteractionTest, smsImsServiceInteractionTest_005, Function | MediumTest | Level1)
{
    sptr<MockImsSms> pImsSms = new MockImsSms();
    pImsSms->retImsSetSmsConfig = 1;
    DelayedSingleton<ImsSmsClient>::GetInstance()->imsSmsProxy_ = pImsSms;
    int32_t slotId = 0, imsSmsConfig = 0;
    EXPECT_EQ(DelayedSingleton<ImsSmsClient>::GetInstance()->ImsSetSmsConfig(slotId, imsSmsConfig),
        pImsSms->retImsSetSmsConfig);
    DelayedSingleton<ImsSmsClient>::DestroyInstance();
}

/**
 * @tc.number   Telephony_SmsMmsGtest_smsImsServiceInteractionTest_006
 * @tc.name     smsServiceProxyTest_001
 * @tc.desc     Function test
 */
HWTEST_F(SmsImsServiceInteractionTest, smsImsServiceInteractionTest_006, Function | MediumTest | Level1)
{
    sptr<MockImsSms> pImsSms = new MockImsSms();
    pImsSms->retImsGetSmsConfig = 1;
    DelayedSingleton<ImsSmsClient>::GetInstance()->imsSmsProxy_ = pImsSms;
    int32_t slotId = 0;
    EXPECT_EQ(DelayedSingleton<ImsSmsClient>::GetInstance()->ImsGetSmsConfig(slotId), pImsSms->retImsGetSmsConfig);
    DelayedSingleton<ImsSmsClient>::DestroyInstance();
}

/**
 * @tc.number   Telephony_SmsMmsGtest_smsImsServiceInteractionTest_007
 * @tc.name     smsServiceProxyTest_001
 * @tc.desc     Function test
 */
HWTEST_F(SmsImsServiceInteractionTest, smsImsServiceInteractionTest_007, Function | MediumTest | Level1)
{
    int32_t slotId = 0;
    std::shared_ptr<AppExecFwk::EventHandler> handler = nullptr;
    EXPECT_EQ(DelayedSingleton<ImsSmsClient>::GetInstance()->RegisterImsSmsCallbackHandler(slotId, handler),
        TELEPHONY_ERR_LOCAL_PTR_NULL);
    EXPECT_EQ(DelayedSingleton<ImsSmsClient>::GetInstance()->GetHandler(slotId), handler);
    DelayedSingleton<ImsSmsClient>::DestroyInstance();
}

/**
 * @tc.number   Telephony_SmsMmsGtest_smsImsServiceInteractionTest_008
 * @tc.name     smsServiceProxyTest_001
 * @tc.desc     Function test
 */
HWTEST_F(SmsImsServiceInteractionTest, smsImsServiceInteractionTest_008, Function | MediumTest | Level1)
{
    int32_t slotId = 0;
    std::shared_ptr<AppExecFwk::EventHandler> handler = std::make_shared<AppExecFwk::EventHandler>();
    EXPECT_EQ(DelayedSingleton<ImsSmsClient>::GetInstance()->RegisterImsSmsCallbackHandler(slotId, handler),
        TELEPHONY_SUCCESS);
    EXPECT_EQ(DelayedSingleton<ImsSmsClient>::GetInstance()->GetHandler(slotId), handler);
    DelayedSingleton<ImsSmsClient>::DestroyInstance();
}

/**
 * @tc.number   Telephony_SmsMmsGtest_smsImsServiceInteractionTest_009
 * @tc.name     smsServiceProxyTest_001
 * @tc.desc     Function test
 */
HWTEST_F(SmsImsServiceInteractionTest, smsImsServiceInteractionTest_009, Function | MediumTest | Level1)
{
    sptr<MockImsSms> pImsSms = new MockImsSms();
    sptr<MockImsCoreService> pImsCoreService = new MockImsCoreService();
    sptr<MockImsSmsCallback> pImsSmsCallback = new MockImsSmsCallback();
    DelayedSingleton<ImsSmsClient>::GetInstance()->imsSmsProxy_ = pImsSms;
    DelayedSingleton<ImsSmsClient>::GetInstance()->imsSmsCallback_ = pImsSmsCallback;
    DelayedSingleton<ImsSmsClient>::GetInstance()->imsCoreServiceProxy_ = pImsCoreService;

    DelayedSingleton<ImsSmsClient>::GetInstance()->Clean();
    EXPECT_EQ(DelayedSingleton<ImsSmsClient>::GetInstance()->imsSmsProxy_, nullptr);
    EXPECT_EQ(DelayedSingleton<ImsSmsClient>::GetInstance()->imsSmsCallback_, nullptr);
    EXPECT_EQ(DelayedSingleton<ImsSmsClient>::GetInstance()->imsCoreServiceProxy_, nullptr);
    DelayedSingleton<ImsSmsClient>::DestroyInstance();
}

/**
 * @tc.number   Telephony_SmsMmsGtest_smsImsServiceInteractionTest_010
 * @tc.name     smsServiceProxyTest_001
 * @tc.desc     Function test
 */
HWTEST_F(SmsImsServiceInteractionTest, smsImsServiceInteractionTest_010, Function | MediumTest | Level1)
{
    ImsSmsClient::SystemAbilityListener listerner;
    int32_t systemAbilityId = 0;
    std::string deviceId = "";
    EXPECT_EQ(DelayedSingleton<ImsSmsClient>::GetInstance()->IsConnect(), false);
    listerner.OnRemoveSystemAbility(systemAbilityId, deviceId);
    EXPECT_EQ(DelayedSingleton<ImsSmsClient>::GetInstance()->IsConnect(), false);

    sptr<MockImsSms> pImsSms = new MockImsSms();
    sptr<MockImsCoreService> pImsCoreService = new MockImsCoreService();
    sptr<MockImsSmsCallback> pImsSmsCallback = new MockImsSmsCallback();
    DelayedSingleton<ImsSmsClient>::GetInstance()->imsSmsProxy_ = pImsSms;
    DelayedSingleton<ImsSmsClient>::GetInstance()->imsSmsCallback_ = pImsSmsCallback;
    DelayedSingleton<ImsSmsClient>::GetInstance()->imsCoreServiceProxy_ = pImsCoreService;

    EXPECT_EQ(DelayedSingleton<ImsSmsClient>::GetInstance()->IsConnect(), true);
    listerner.OnRemoveSystemAbility(systemAbilityId, deviceId);
    EXPECT_EQ(DelayedSingleton<ImsSmsClient>::GetInstance()->IsConnect(), false);
    EXPECT_EQ(DelayedSingleton<ImsSmsClient>::GetInstance()->imsSmsProxy_, nullptr);
    EXPECT_EQ(DelayedSingleton<ImsSmsClient>::GetInstance()->imsSmsCallback_, nullptr);
    EXPECT_EQ(DelayedSingleton<ImsSmsClient>::GetInstance()->imsCoreServiceProxy_, nullptr);
    DelayedSingleton<ImsSmsClient>::DestroyInstance();
}

/**
 * @tc.number   Telephony_SmsMmsGtest_smsImsServiceInteractionTest_011
 * @tc.name     smsServiceProxyTest_001
 * @tc.desc     Function test
 */
HWTEST_F(SmsImsServiceInteractionTest, smsImsServiceInteractionTest_011, Function | MediumTest | Level1)
{
    std::unique_ptr<ImsSmsCallbackStub> imsSmsCallbackStub = std::make_unique<ImsSmsCallbackStub>();

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(ImsSmsCallbackInterfaceCode::IMS_SEND_MESSAGE);
    EXPECT_EQ(imsSmsCallbackStub->OnRemoteRequest(code, data, reply, option), TELEPHONY_ERR_DESCRIPTOR_MISMATCH);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_smsImsServiceInteractionTest_012
 * @tc.name     smsServiceProxyTest_001
 * @tc.desc     Function test
 */
HWTEST_F(SmsImsServiceInteractionTest, smsImsServiceInteractionTest_012, Function | MediumTest | Level1)
{
    std::unique_ptr<ImsSmsCallbackStub> imsSmsCallbackStub = std::make_unique<ImsSmsCallbackStub>();

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(1<<31);
    data.WriteInterfaceToken(ImsSmsCallbackStub::GetDescriptor());
    EXPECT_EQ(imsSmsCallbackStub->OnRemoteRequest(code, data, reply, option), IPC_STUB_UNKNOW_TRANS_ERR);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_smsImsServiceInteractionTest_013
 * @tc.name     smsServiceProxyTest_001
 * @tc.desc     Function test
 */
HWTEST_F(SmsImsServiceInteractionTest, smsImsServiceInteractionTest_013, Function | MediumTest | Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    int32_t slotId = 0;
    int32_t imsSmsConfig = 0;
    std::unique_ptr<ImsSmsCallbackStub> imsSmsCallbackStub = std::make_unique<ImsSmsCallbackStub>();
    uint32_t code = static_cast<uint32_t>(ImsSmsCallbackInterfaceCode::IMS_GET_SMS_CONFIG);
    data.WriteInterfaceToken(ImsSmsCallbackStub::GetDescriptor());
    data.WriteInt32(slotId);
    data.WriteInt32(imsSmsConfig);
    EXPECT_EQ(imsSmsCallbackStub->OnRemoteRequest(code, data, reply, option), TELEPHONY_SUCCESS);
}

/**
 * @tc.number   Telephony_SmsMmsGtest_smsImsServiceInteractionTest_014
 * @tc.name     smsServiceProxyTest_001
 * @tc.desc     Function test
 */
HWTEST_F(SmsImsServiceInteractionTest, smsImsServiceInteractionTest_014, Function | MediumTest | Level1)
{
    std::unique_ptr<ImsSmsCallbackStub> imsSmsCallbackStub = std::make_unique<ImsSmsCallbackStub>();

    RadioResponseInfo radioResponseInfo;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    int32_t slotId = 0;
    uint32_t code = static_cast<uint32_t>(ImsSmsCallbackInterfaceCode::IMS_SEND_MESSAGE);
    data.WriteInterfaceToken(ImsSmsCallbackStub::GetDescriptor());
    data.WriteInt32(slotId);
    data.WriteRawData(&radioResponseInfo, sizeof(RadioResponseInfo));
    EXPECT_EQ(imsSmsCallbackStub->OnRemoteRequest(code, data, reply, option), TELEPHONY_SUCCESS);
    DelayedSingleton<ImsSmsClient>::DestroyInstance();
}

/**
 * @tc.number   Telephony_SmsMmsGtest_smsImsServiceInteractionTest_015
 * @tc.name     smsServiceProxyTest_001
 * @tc.desc     Function test
 */
HWTEST_F(SmsImsServiceInteractionTest, smsImsServiceInteractionTest_015, Function | MediumTest | Level1)
{
    std::unique_ptr<ImsSmsCallbackStub> imsSmsCallbackStub = std::make_unique<ImsSmsCallbackStub>();

    SendSmsResultInfo sendSmsResultInfo;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    int32_t slotId = 0;
    uint32_t code = static_cast<uint32_t>(ImsSmsCallbackInterfaceCode::IMS_SEND_MESSAGE);
    data.WriteInterfaceToken(ImsSmsCallbackStub::GetDescriptor());
    data.WriteInt32(slotId);
    data.WriteRawData(&sendSmsResultInfo, sizeof(SendSmsResultInfo));
    EXPECT_EQ(imsSmsCallbackStub->OnRemoteRequest(code, data, reply, option), TELEPHONY_SUCCESS);
    DelayedSingleton<ImsSmsClient>::DestroyInstance();
}

/**
 * @tc.number   Telephony_SmsMmsGtest_smsImsServiceInteractionTest_016
 * @tc.name     smsServiceProxyTest_001
 * @tc.desc     Function test
 */
HWTEST_F(SmsImsServiceInteractionTest, smsImsServiceInteractionTest_016, Function | MediumTest | Level1)
{
    std::unique_ptr<ImsSmsCallbackStub> imsSmsCallbackStub = std::make_unique<ImsSmsCallbackStub>();

    RadioResponseInfo radioResponseInfo;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    int32_t slotId = 0;
    uint32_t code = static_cast<uint32_t>(ImsSmsCallbackInterfaceCode::IMS_SET_SMS_CONFIG);
    data.WriteInterfaceToken(ImsSmsCallbackStub::GetDescriptor());
    data.WriteInt32(slotId);
    data.WriteRawData(&radioResponseInfo, sizeof(RadioResponseInfo));
    EXPECT_EQ(imsSmsCallbackStub->OnRemoteRequest(code, data, reply, option), TELEPHONY_SUCCESS);
    DelayedSingleton<ImsSmsClient>::DestroyInstance();
}
}
}