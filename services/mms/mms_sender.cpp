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

#include "mms_sender.h"

#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
std::string SEND_METHOD = "POST";

MmsSender::MmsSender(int32_t slotId) : DataRequest(slotId)
{
    slotId_ = slotId;
    CreateRequestId();
}

MmsSender::~MmsSender() {}

int32_t MmsSender::ExecuteSendMms(
    const std::string &contentUrl, std::string pduDir, const std::string &ua, const std::string &uaprof)
{
    auto mmsNetworkMgr = std::make_shared<MmsNetworkManager>();
    if (mmsNetworkMgr == nullptr) {
        TELEPHONY_LOGE("mmsNetworkMgr_ is nullptr");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }

    std::unique_lock<std::mutex> lock(sendMmsMutex_);
    return ExecuteMms(SEND_METHOD, mmsNetworkMgr, contentUrl, pduDir);
}
} // namespace Telephony
} // namespace OHOS