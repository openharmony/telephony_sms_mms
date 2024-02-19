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

#include "mms_receive_manager.h"

#include "string_utils.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
using namespace std;
MmsReceiveManager::MmsReceiveManager(int32_t slotId) : TelEventHandler("MmsReceiveManager"), slotId_(slotId) {}

MmsReceiveManager::~MmsReceiveManager() {}

void MmsReceiveManager::Init()
{
    mmsReceiver_ = std::make_shared<MmsReceive>(slotId_);
    if (mmsReceiver_ == nullptr) {
        TELEPHONY_LOGE("failed to create mmsReceiver");
        return;
    }
}

int32_t MmsReceiveManager::DownloadMms(
    const std::u16string &mmsc, std::u16string &data, const std::u16string &ua, const std::u16string &uaprof)
{
    if (mmsReceiver_ == nullptr) {
        TELEPHONY_LOGE("mmsReceiver_ is nullptr");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    std::string dataPdu;
    int32_t downloadResult = mmsReceiver_->ExecuteDownloadMms(
        StringUtils::ToUtf8(mmsc), dataPdu, StringUtils::ToUtf8(ua), StringUtils::ToUtf8(uaprof));
    if (downloadResult == TELEPHONY_ERR_SUCCESS) {
        data = StringUtils::ToUtf16(dataPdu);
        TELEPHONY_LOGI("download mms successed");
        return TELEPHONY_ERR_SUCCESS;
    } else {
        TELEPHONY_LOGI("download mms failed");
        return downloadResult;
    }
}
} // namespace Telephony
} // namespace OHOS