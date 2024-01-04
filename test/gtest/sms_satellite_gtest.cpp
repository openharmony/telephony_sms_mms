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

#include "gtest/gtest.h"
#include "radio_event.h"
#include "satellite/satellite_sms_service_ipc_interface_code.h"
#include "satellite_sms_callback.h"
#include "satellite_sms_proxy.h"
#include "sms_common.h"
#include "telephony_errors.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
using namespace testing::ext;

namespace {
class MockIRemoteObject : public IRemoteObject {
public:
    uint32_t requestCode_ = -1;

public:
    MockIRemoteObject() : IRemoteObject(u"mock_i_remote_object") {}

    ~MockIRemoteObject() {}

    int32_t GetObjectRefCount() override
    {
        return 0;
    }

    int SendRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override
    {
        TELEPHONY_LOGI("Mock SendRequest");
        requestCode_ = code;
        reply.WriteInt32(0);
        return 0;
    }

    bool IsProxyObject() const override
    {
        return true;
    }

    bool CheckObjectLegality() const override
    {
        return true;
    }

    bool AddDeathRecipient(const sptr<DeathRecipient> &recipient) override
    {
        return true;
    }

    bool RemoveDeathRecipient(const sptr<DeathRecipient> &recipient) override
    {
        return true;
    }

    bool Marshalling(Parcel &parcel) const override
    {
        return true;
    }

    sptr<IRemoteBroker> AsInterface() override
    {
        return nullptr;
    }

    int Dump(int fd, const std::vector<std::u16string> &args) override
    {
        return 0;
    }

    std::u16string GetObjectDescriptor() const
    {
        std::u16string descriptor = std::u16string();
        return descriptor;
    }
};
} // namespace

class SmsSatelliteGtest : public testing::Test {
public:
    int32_t slotId_ = 0;
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void SmsSatelliteGtest::TearDownTestCase() {}

void SmsSatelliteGtest::SetUp() {}

void SmsSatelliteGtest::TearDown() {}

void SmsSatelliteGtest::SetUpTestCase() {}

uint32_t ToCode(SatelliteSmsServiceInterfaceCode code)
{
    return static_cast<uint32_t>(code);
}

/**
 * @tc.number   Telephony_SmsSatelliteGtest_RegisterSmsNotify_0001
 * @tc.name     register sms callback
 * @tc.desc     Function test
 */
HWTEST_F(SmsSatelliteGtest, RegisterSmsNotify_0001, Function | MediumTest | Level2)
{
    TELEPHONY_LOGI("SmsSatelliteGtest::RegisterSmsNotify_0001 -->");
    sptr<MockIRemoteObject> remote = new (std::nothrow) MockIRemoteObject();
    SatelliteSmsProxy proxy(remote);

    int32_t ret = proxy.RegisterSmsNotify(slotId_, RadioEvent::RADIO_SEND_SMS, nullptr);
    ASSERT_NE(ret, TELEPHONY_SUCCESS);

    sptr<ISatelliteSmsCallback> callback = std::make_unique<SatelliteSmsCallback>(nullptr).release();
    ret = proxy.RegisterSmsNotify(slotId_, RadioEvent::RADIO_SEND_SMS, callback);
    ASSERT_EQ(remote->requestCode_, ToCode(SatelliteSmsServiceInterfaceCode::REGISTER_SMS_NOTIFY));
    ASSERT_EQ(ret, TELEPHONY_SUCCESS);
}

/**
 * @tc.number   Telephony_SmsSatelliteGtest_UnRegisterSmsNotify_0001
 * @tc.name     unregister sms callback
 * @tc.desc     Function test
 */
HWTEST_F(SmsSatelliteGtest, UnRegisterSmsNotify_0001, Function | MediumTest | Level2)
{
    TELEPHONY_LOGI("SmsSatelliteGtest::UnRegisterSmsNotify_0001 -->");
    sptr<MockIRemoteObject> remote = new (std::nothrow) MockIRemoteObject();
    SatelliteSmsProxy proxy(remote);

    int32_t ret = proxy.UnRegisterSmsNotify(slotId_, RadioEvent::RADIO_SEND_SMS);
    ASSERT_EQ(remote->requestCode_, ToCode(SatelliteSmsServiceInterfaceCode::UNREGISTER_SMS_NOTIFY));
    ASSERT_EQ(ret, TELEPHONY_SUCCESS);
}

/**
 * @tc.number   Telephony_SmsSatelliteGtest_SendSms_0001
 * @tc.name     send sms
 * @tc.desc     Function test
 */
HWTEST_F(SmsSatelliteGtest, SendSms_0001, Function | MediumTest | Level2)
{
    TELEPHONY_LOGI("SmsSatelliteGtest::SendSms_0001 -->");
    sptr<MockIRemoteObject> remote = new (std::nothrow) MockIRemoteObject();
    SatelliteSmsProxy proxy(remote);

    SatelliteMessage message;
    int32_t ret = proxy.SendSms(slotId_, RadioEvent::RADIO_SEND_SMS, message);
    ASSERT_EQ(remote->requestCode_, ToCode(SatelliteSmsServiceInterfaceCode::SEND_SMS));
    ASSERT_EQ(ret, TELEPHONY_SUCCESS);
}

/**
 * @tc.number   Telephony_SmsSatelliteGtest_SendSmsMoreMode_0001
 * @tc.name     send sms more
 * @tc.desc     Function test
 */
HWTEST_F(SmsSatelliteGtest, SendSmsMoreMode_0001, Function | MediumTest | Level2)
{
    TELEPHONY_LOGI("SmsSatelliteGtest::SendSmsMoreMode_0001 -->");
    sptr<MockIRemoteObject> remote = new (std::nothrow) MockIRemoteObject();
    SatelliteSmsProxy proxy(remote);

    SatelliteMessage message;
    int32_t ret = proxy.SendSmsMoreMode(slotId_, RadioEvent::RADIO_SEND_SMS_EXPECT_MORE, message);
    ASSERT_EQ(remote->requestCode_, ToCode(SatelliteSmsServiceInterfaceCode::SEND_SMS_MORE_MODE));
    ASSERT_EQ(ret, TELEPHONY_SUCCESS);
}

/**
 * @tc.number   Telephony_SmsSatelliteGtest_SendSmsAck_0001
 * @tc.name     send sms ack
 * @tc.desc     Function test
 */
HWTEST_F(SmsSatelliteGtest, SendSmsAck_0001, Function | MediumTest | Level2)
{
    TELEPHONY_LOGI("SmsSatelliteGtest::SendSmsAck_0001 -->");
    sptr<MockIRemoteObject> remote = new (std::nothrow) MockIRemoteObject();
    SatelliteSmsProxy proxy(remote);

    int32_t ret = proxy.SendSmsAck(slotId_, SMS_EVENT_NEW_SMS_REPLY, true, AckIncomeCause::SMS_ACK_RESULT_OK);
    ASSERT_EQ(remote->requestCode_, ToCode(SatelliteSmsServiceInterfaceCode::SEND_SMS_ACK));
    ASSERT_EQ(ret, TELEPHONY_SUCCESS);
}

} // namespace Telephony
} // namespace OHOS