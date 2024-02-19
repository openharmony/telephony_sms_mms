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

#ifndef MMS_RECEIVER_MANAGER_H
#define MMS_RECEIVER_MANAGER_H

#include "mms_receive.h"
#include "tel_event_handler.h"

namespace OHOS {
namespace Telephony {
class MmsReceiveManager : public TelEventHandler {
public:
    explicit MmsReceiveManager(int32_t slotId);
    virtual ~MmsReceiveManager();
    void Init();
    int32_t DownloadMms(
        const std::u16string &mmsc, std::u16string &data, const std::u16string &ua, const std::u16string &uaprof);

private:
    std::shared_ptr<MmsReceive> mmsReceiver_;
    int32_t slotId_;
};
} // namespace Telephony
} // namespace OHOS
#endif