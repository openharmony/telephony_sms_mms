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

#ifndef GSM_SMS_SENDER_TEST_H
#define GSM_SMS_SENDER_TEST_H

#include <cstring>
#include <iostream>

#include "securec.h"

#include "data_ability_helper.h"
#include "i_sms_service_interface.h"
#include "sms_service_proxy.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace Telephony {
class GsmSmsSenderTest {
public:
    void TestGsmSendShortData(const sptr<ISmsServiceInterface> &smsService) const;
    void TestGsmSendLongData(const sptr<ISmsServiceInterface> &smsService) const;
    void TestSendShortText(const sptr<ISmsServiceInterface> &smsService) const;
    void TestSendLongText(const sptr<ISmsServiceInterface> &smsService) const;
    void TestSetSmscAddr(const sptr<ISmsServiceInterface> &smsService) const;
    void TestGetSmscAddr(const sptr<ISmsServiceInterface> &smsService) const;
    void TestAddSimMessage(const sptr<ISmsServiceInterface> &smsService) const;
    void TestDelSimMessage(const sptr<ISmsServiceInterface> &smsService) const;
    void TestUpdateSimMessage(const sptr<ISmsServiceInterface> &smsService) const;
    void TestGetAllSimMessages(const sptr<ISmsServiceInterface> &smsService) const;
    void TestEnableCBRangeConfig(const sptr<ISmsServiceInterface> &smsService) const;
    void TestDisableCBRangeConfig(const sptr<ISmsServiceInterface> &smsService) const;
    void TestEnableCBConfig(const sptr<ISmsServiceInterface> &smsService) const;
    void TestDisableCBConfig(const sptr<ISmsServiceInterface> &smsService) const;
    void TestSetDefaultSmsSlotId(const sptr<ISmsServiceInterface> &smsService) const;
    void TestGetDefaultSmsSlotId(const sptr<ISmsServiceInterface> &smsService) const;
    void TestSplitMessage(const sptr<ISmsServiceInterface> &smsService) const;
    void TestGetSmsSegmentsInfo(const sptr<ISmsServiceInterface> &smsService) const;
    void TestIsImsSmsSupported(const sptr<ISmsServiceInterface> &smsService) const;
    void TestSetImsSmsConfig(const sptr<ISmsServiceInterface> &smsService) const;
    void TestGetImsShortMessageFormat(const sptr<ISmsServiceInterface> &smsService) const;
    void TestAddBlockPhone() const;
    void TestRemoveBlockPhone() const;
    void TestHasSmsCapability(const sptr<ISmsServiceInterface> &smsService) const;

private:
    std::shared_ptr<AppExecFwk::DataAbilityHelper> CreateDataAHelper() const;
};
} // namespace Telephony
} // namespace OHOS
#endif