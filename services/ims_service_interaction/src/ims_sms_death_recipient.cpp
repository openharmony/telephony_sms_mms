/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#include "ims_sms_death_recipient.h"
#include "ims_sms_client.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
ImsSmsDeathRecipient::ImsSmsDeathRecipient()
{}

void ImsSmsDeathRecipient::OnRemoteDied(const OHOS::wptr<OHOS::IRemoteObject> &object)
{
    std::shared_ptr<ImsSmsClient> imsSmsClient =
        DelayedSingleton<ImsSmsClient>::GetInstance();
    if (imsSmsClient == nullptr) {
        TELEPHONY_LOGE("ImsSmsDeathRecipient OnRemoteDied, imsSmsClient is nullptr!");
        return;
    }

    bool res = false;
    int32_t i = 0;

    do {
        TELEPHONY_LOGI("ImsSmsDeathRecipient ReConnect service!");
        imsSmsClient->Clean();
        res = imsSmsClient->ReConnectService();
        if (!res) {
            i++;
            sleep(1);
        }
    } while (!res && (i < ImsSmsClient::RE_CONNECT_SERVICE_COUNT_MAX));
    if (!res) {
        TELEPHONY_LOGE("ImsSmsDeathRecipient Reconnect service is failed!");
        return;
    }
    TELEPHONY_LOGI("ImsSmsDeathRecipient Reconnect service is successful!");
}
}  // namespace Telephony
}  // namespace OHOS
