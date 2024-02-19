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

#ifndef MMS_RECEIVER_H
#define MMS_RECEIVER_H

#include <mutex>

#include "data_request.h"
#include "mms_network_manager.h"

namespace OHOS {
namespace Telephony {
class MmsReceive : public DataRequest {
public:
    explicit MmsReceive(int32_t slotId);
    virtual ~MmsReceive();
    int32_t ExecuteDownloadMms(
        const std::string &mmsc, std::string &data, const std::string &ua, const std::string &uaprof);

private:
    std::mutex downloadMmsMutex_;
};
} // namespace Telephony
} // namespace OHOS
#endif