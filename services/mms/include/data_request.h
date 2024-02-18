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

#ifndef DATA_REQUEST_H
#define DATA_REQUEST_H

#include <condition_variable>
#include <mutex>

#include "mms_network_manager.h"
#include "telephony_errors.h"

namespace OHOS {
namespace Telephony {
class DataRequest {
public:
    explicit DataRequest(int32_t slotId);
    ~DataRequest();
    int32_t HttpRequest(int32_t slotId, const std::string &method, std::shared_ptr<MmsNetworkManager> netMgr,
        const std::string &contentUrl, std::string &pduDir);
    int32_t ExecuteMms(const std::string &method, std::shared_ptr<MmsNetworkManager> mmsNetworkMgr,
        const std::string &contentUrl, std::string &pduDir);
    uint8_t GetRequestId();
    void CreateRequestId();

public:
    static bool networkReady_;
    static std::mutex ctx_;
    static std::condition_variable cv_;
    int32_t slotId_ = -1;
    uint8_t requestId_ = 0;
};
} // namespace Telephony
} // namespace OHOS
#endif