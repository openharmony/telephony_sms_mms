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
#include "mock/mock_ims_core_service_interface.h"
#include "mock/mock_ims_sms_callback_interface.h"
#include "mock/mock_ims_sms_interface.h"
#include "mock/mock_remote_object.h"

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

/**
 * @tc.number   Telephony_SmsImsServiceInteractionTest_smsImsServiceInteractionTest_001
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
 * @tc.number   Telephony_SmsImsServiceInteractionTest_smsImsServiceInteractionTest_002
 * @tc.name     smsServiceProxyTest_001
 * @tc.desc     Function test
 */
HWTEST_F(SmsImsServiceInteractionTest, smsImsServiceInteractionTest_002, Function | MediumTest | Level1)
{
    EXPECT_EQ(DelayedSingleton<ImsSmsClient>::GetInstance()->RegisterImsSmsCallback(), TELEPHONY_ERR_LOCAL_PTR_NULL);
    DelayedSingleton<ImsSmsClient>::DestroyInstance();
}

/**
 * @tc.number   Telephony_SmsImsServiceInteractionTest_smsImsServiceInteractionTest_003
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
 * @tc.number   Telephony_SmsImsServiceInteractionTest_smsImsServiceInteractionTest_004
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
 * @tc.number   Telephony_SmsImsServiceInteractionTest_smsImsServiceInteractionTest_005
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
 * @tc.number   Telephony_SmsImsServiceInteractionTest_smsImsServiceInteractionTest_006
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
 * @tc.number   Telephony_SmsImsServiceInteractionTest_smsImsServiceInteractionTest_007
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
 * @tc.number   Telephony_SmsImsServiceInteractionTest_smsImsServiceInteractionTest_008
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
 * @tc.number   Telephony_SmsImsServiceInteractionTest_smsImsServiceInteractionTest_009
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
 * @tc.number   Telephony_SmsImsServiceInteractionTest_smsImsServiceInteractionTest_010
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
 * @tc.number   Telephony_SmsImsServiceInteractionTest_smsImsServiceInteractionTest_011
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
 * @tc.number   Telephony_SmsImsServiceInteractionTest_smsImsServiceInteractionTest_012
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
 * @tc.number   Telephony_SmsImsServiceInteractionTest_smsImsServiceInteractionTest_013
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
 * @tc.number   Telephony_SmsImsServiceInteractionTest_smsImsServiceInteractionTest_014
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
 * @tc.number   Telephony_SmsImsServiceInteractionTest_smsImsServiceInteractionTest_015
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
 * @tc.number   Telephony_SmsImsServiceInteractionTest_smsImsServiceInteractionTest_016
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

/**
 * @tc.number   Telephony_SmsImsServiceInteractionTest_smsImsServiceInteractionTest_017
 * @tc.name     smsServiceProxyTest_001
 * @tc.desc     Function test
 */
HWTEST_F(SmsImsServiceInteractionTest, smsImsServiceInteractionTest_017, Function | MediumTest | Level1)
{
    sptr<MockRemoteObject> sptrRemoteObject = nullptr;
    std::unique_ptr<MockImsSmsProxy> imsSmsProxy = std::make_unique<MockImsSmsProxy>(sptrRemoteObject);
    
    int32_t slotId = 0;
    ImsMessageInfo imsMessageInfo;
    EXPECT_EQ(imsSmsProxy->ImsSendMessage(slotId, imsMessageInfo), TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL);
}

/**
 * @tc.number   Telephony_SmsImsServiceInteractionTest_smsImsServiceInteractionTest_018
 * @tc.name     smsServiceProxyTest_001
 * @tc.desc     Function test
 */
HWTEST_F(SmsImsServiceInteractionTest, smsImsServiceInteractionTest_018, Function | MediumTest | Level1)
{
    sptr<MockRemoteObject> sptrRemoteObject = new MockRemoteObject();
    sptrRemoteObject->retSendRequest = 0;
    std::unique_ptr<MockImsSmsProxy> imsSmsProxy = std::make_unique<MockImsSmsProxy>(sptrRemoteObject);
    
    int32_t slotId = 0;
    ImsMessageInfo imsMessageInfo;
    EXPECT_EQ(imsSmsProxy->ImsSendMessage(slotId, imsMessageInfo), 0);
}

/**
 * @tc.number   Telephony_SmsImsServiceInteractionTest_smsImsServiceInteractionTest_019
 * @tc.name     smsServiceProxyTest_001
 * @tc.desc     Function test
 */
HWTEST_F(SmsImsServiceInteractionTest, smsImsServiceInteractionTest_019, Function | MediumTest | Level1)
{
    sptr<MockRemoteObject> sptrRemoteObject = new MockRemoteObject();
    sptrRemoteObject->retSendRequest = -1;
    std::unique_ptr<MockImsSmsProxy> imsSmsProxy = std::make_unique<MockImsSmsProxy>(sptrRemoteObject);
    
    int32_t slotId = 0;
    ImsMessageInfo imsMessageInfo;
    EXPECT_EQ(imsSmsProxy->ImsSendMessage(slotId, imsMessageInfo), TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL);
}

/**
 * @tc.number   Telephony_SmsImsServiceInteractionTest_smsImsServiceInteractionTest_020
 * @tc.name     smsServiceProxyTest_001
 * @tc.desc     Function test
 */
HWTEST_F(SmsImsServiceInteractionTest, smsImsServiceInteractionTest_020, Function | MediumTest | Level1)
{
    sptr<MockRemoteObject> sptrRemoteObject = new MockRemoteObject();
    sptrRemoteObject->retSendRequest = 0;
    std::unique_ptr<MockImsSmsProxy> imsSmsProxy = std::make_unique<MockImsSmsProxy>(sptrRemoteObject);
    
    int32_t slotId = 0;
    int32_t imsSmsConfig = 0;
    EXPECT_EQ(imsSmsProxy->ImsSetSmsConfig(slotId, imsSmsConfig), 0);
}

/**
 * @tc.number   Telephony_SmsImsServiceInteractionTest_smsImsServiceInteractionTest_021
 * @tc.name     smsServiceProxyTest_001
 * @tc.desc     Function test
 */
HWTEST_F(SmsImsServiceInteractionTest, smsImsServiceInteractionTest_021, Function | MediumTest | Level1)
{
    sptr<MockRemoteObject> sptrRemoteObject = new MockRemoteObject();
    sptrRemoteObject->retSendRequest = 0;
    std::unique_ptr<MockImsSmsProxy> imsSmsProxy = std::make_unique<MockImsSmsProxy>(sptrRemoteObject);
    
    int32_t slotId = 0;
    EXPECT_EQ(imsSmsProxy->ImsGetSmsConfig(slotId), 0);
}

/**
 * @tc.number   Telephony_SmsImsServiceInteractionTest_smsImsServiceInteractionTest_022
 * @tc.name     smsServiceProxyTest_001
 * @tc.desc     Function test
 */
HWTEST_F(SmsImsServiceInteractionTest, smsImsServiceInteractionTest_022, Function | MediumTest | Level1)
{
    sptr<MockRemoteObject> sptrRemoteObject = nullptr;
    std::unique_ptr<MockImsSmsProxy> imsSmsProxy = std::make_unique<MockImsSmsProxy>(sptrRemoteObject);
    EXPECT_EQ(imsSmsProxy->RegisterImsSmsCallback(nullptr), TELEPHONY_ERR_ARGUMENT_INVALID);
}

/**
 * @tc.number   Telephony_SmsImsServiceInteractionTest_smsImsServiceInteractionTest_023
 * @tc.name     smsServiceProxyTest_001
 * @tc.desc     Function test
 */
HWTEST_F(SmsImsServiceInteractionTest, smsImsServiceInteractionTest_023, Function | MediumTest | Level1)
{
    sptr<MockRemoteObject> sptrRemoteObject = nullptr;
    std::unique_ptr<MockImsSmsProxy> imsSmsProxy = std::make_unique<MockImsSmsProxy>(sptrRemoteObject);
    sptr<MockImsSmsCallback> pImsSmsCallback = new MockImsSmsCallback();
    EXPECT_EQ(imsSmsProxy->RegisterImsSmsCallback(pImsSmsCallback), TELEPHONY_ERR_WRITE_DATA_FAIL);
}

/**
 * @tc.number   Telephony_SmsImsServiceInteractionTest_smsImsServiceInteractionTest_024
 * @tc.name     smsServiceProxyTest_001
 * @tc.desc     Function test
 */
HWTEST_F(SmsImsServiceInteractionTest, smsImsServiceInteractionTest_024, Function | MediumTest | Level1)
{
    sptr<MockRemoteObject> sptrRemoteObject = new MockRemoteObject();
    std::unique_ptr<MockImsSmsProxy> imsSmsProxy = std::make_unique<MockImsSmsProxy>(nullptr);
    sptr<MockImsSmsCallback> pImsSmsCallback = new MockImsSmsCallback();
    pImsSmsCallback->retRemoteObject = sptrRemoteObject;
    EXPECT_EQ(imsSmsProxy->RegisterImsSmsCallback(pImsSmsCallback), TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL);
}

/**
 * @tc.number   Telephony_SmsImsServiceInteractionTest_smsImsServiceInteractionTest_025
 * @tc.name     smsServiceProxyTest_001
 * @tc.desc     Function test
 */
HWTEST_F(SmsImsServiceInteractionTest, smsImsServiceInteractionTest_025, Function | MediumTest | Level1)
{
    sptr<MockRemoteObject> sptrRemoteObject = new MockRemoteObject();
    sptrRemoteObject->retSendRequest = 0;
    std::unique_ptr<MockImsSmsProxy> imsSmsProxy = std::make_unique<MockImsSmsProxy>(sptrRemoteObject);

    sptr<MockImsSmsCallback> pImsSmsCallback = new MockImsSmsCallback();
    pImsSmsCallback->retRemoteObject = sptrRemoteObject;
    EXPECT_EQ(imsSmsProxy->RegisterImsSmsCallback(pImsSmsCallback), 0);
}

/**
 * @tc.number   Telephony_SmsImsServiceInteractionTest_smsImsServiceInteractionTest_026
 * @tc.name     smsServiceProxyTest_001
 * @tc.desc     Function test
 */
HWTEST_F(SmsImsServiceInteractionTest, smsImsServiceInteractionTest_026, Function | MediumTest | Level1)
{
    sptr<MockRemoteObject> sptrRemoteObject = new MockRemoteObject();
    sptrRemoteObject->retSendRequest = -1;
    std::unique_ptr<MockImsSmsProxy> imsSmsProxy = std::make_unique<MockImsSmsProxy>(sptrRemoteObject);

    sptr<MockImsSmsCallback> pImsSmsCallback = new MockImsSmsCallback();
    pImsSmsCallback->retRemoteObject = sptrRemoteObject;
    EXPECT_EQ(imsSmsProxy->RegisterImsSmsCallback(pImsSmsCallback), TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL);
}
}
}