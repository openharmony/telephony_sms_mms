/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
#include "mms_conn_callback_stub.h"

#include <condition_variable>
#include <memory>
#include <mutex>

#include "core_manager_inner.h"
#include "data_request.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
std::mutex DataRequest::ctx_;
bool DataRequest::networkReady_ = false;
std::condition_variable DataRequest::cv_;

int32_t MmsConnCallbackStub::NetAvailable(sptr<NetManagerStandard::NetHandle> &netHandle)
{
    TELEPHONY_LOGI("MmsConnCallbackStub::NetAvailable");
    if (netHandle == nullptr) {
        TELEPHONY_LOGE("netHandle is nullptr");
        return TELEPHONY_ERR_FAIL;
    }
    SyncNetworkResponse(true);
    return ERR_NONE;
}

void MmsConnCallbackStub::SyncNetworkResponse(bool netGot)
{
    std::unique_lock<std::mutex> lck(DataRequest::ctx_);
    DataRequest::networkReady_ = netGot;
    TELEPHONY_LOGI("networkReady_ = %{public}d", DataRequest::networkReady_);
}

int32_t MmsConnCallbackStub::NetCapabilitiesChange(
    sptr<NetManagerStandard::NetHandle> &netHandle, const sptr<NetManagerStandard::NetAllCapabilities> &netAllCap)
{
    TELEPHONY_LOGI("MmsConnCallbackStub::NetCapabilitiesChange");
    return ERR_NONE;
}

int32_t MmsConnCallbackStub::NetConnectionPropertiesChange(
    sptr<NetManagerStandard::NetHandle> &netHandle, const sptr<NetManagerStandard::NetLinkInfo> &info)
{
    TELEPHONY_LOGI("MmsConnCallbackStub::NetConnectionPropertiesChange");
    DataRequest::cv_.notify_one();
    return ERR_NONE;
}

int32_t MmsConnCallbackStub::NetLost(sptr<NetManagerStandard::NetHandle> &netHandle)
{
    TELEPHONY_LOGI("MmsConnCallbackStub::NetLost");
    return ERR_NONE;
}

int32_t MmsConnCallbackStub::NetUnavailable()
{
    SyncNetworkResponse(false);
    TELEPHONY_LOGI("MmsConnCallbackStub::NetUnavailable");
    return ERR_NONE;
}

int32_t MmsConnCallbackStub::NetBlockStatusChange(sptr<NetManagerStandard::NetHandle> &netHandle, bool blocked)
{
    TELEPHONY_LOGI("MmsConnCallbackStub::NetBlockStatusChange");
    return ERR_NONE;
}
} // namespace Telephony
} // namespace OHOS
