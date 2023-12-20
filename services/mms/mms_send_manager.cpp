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

#include "mms_send_manager.h"

#include "string_utils.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
using namespace std;
MmsSendManager::MmsSendManager(int32_t slotId) : TelEventHandler("MmsSendManager"), slotId_(slotId) {}

MmsSendManager::~MmsSendManager() {}

void MmsSendManager::Init()
{
    mmsSender_ = std::make_shared<MmsSender>(slotId_);
    if (mmsSender_ == nullptr) {
        TELEPHONY_LOGE("failed to create MmsSender");
        return;
    }
}

int32_t MmsSendManager::SendMms(
    const std::u16string &mmsc, const std::u16string &data, const std::u16string &ua, const std::u16string &uaprof)
{
    if (mmsSender_ == nullptr) {
        TELEPHONY_LOGE("mmsSender_ is nullptr");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }

    int32_t sendResult = mmsSender_->ExecuteSendMms(
        StringUtils::ToUtf8(mmsc), StringUtils::ToUtf8(data), StringUtils::ToUtf8(ua), StringUtils::ToUtf8(uaprof));
    if (sendResult == TELEPHONY_ERR_SUCCESS) {
        TELEPHONY_LOGI("send mms successed");
        return TELEPHONY_ERR_SUCCESS;
    } else {
        TELEPHONY_LOGI("send mms failed");
        return sendResult;
    }
}
} // namespace Telephony
} // namespace OHOS