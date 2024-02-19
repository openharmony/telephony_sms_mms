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

#ifndef MMS_SENDER_H
#define MMS_SENDER_H

#include <mutex>

#include "data_request.h"
#include "mms_network_manager.h"

namespace OHOS {
namespace Telephony {
class MmsSender : public DataRequest {
public:
    explicit MmsSender(int32_t slotId);
    virtual ~MmsSender();
    int32_t ExecuteSendMms(
        const std::string &mmsc, std::string pduDir, const std::string &ua, const std::string &uaprof);

private:
    std::mutex sendMmsMutex_;
};
} // namespace Telephony
} // namespace OHOS
#endif