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

#include "sms_receive_manager.h"

#include "cdma_sms_receive_handler.h"
#include "gsm_sms_receive_handler.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
using namespace std;
SmsReceiveManager::SmsReceiveManager(int32_t slotId) : slotId_(slotId) {}

SmsReceiveManager::~SmsReceiveManager()
{
    if (gsmSmsReceiveHandler_ != nullptr) {
        gsmSmsReceiveHandler_->UnRegisterHandler();
    }

    if (cdmaSmsReceiveHandler_ != nullptr) {
        cdmaSmsReceiveHandler_->UnRegisterHandler();
    }
}

void SmsReceiveManager::Init()
{
    gsmSmsReceiveHandler_ = std::make_shared<GsmSmsReceiveHandler>(slotId_);
    if (gsmSmsReceiveHandler_ == nullptr) {
        TELEPHONY_LOGE("failed to create GsmSmsReceiveHandler");
        return;
    }
    cdmaSmsReceiveHandler_ = std::make_shared<CdmaSmsReceiveHandler>(slotId_);
    if (cdmaSmsReceiveHandler_ == nullptr) {
        TELEPHONY_LOGE("failed to create CdmaSmsReceiveHandler");
        return;
    }

    gsmSmsReceiveHandler_->Init();
    cdmaSmsReceiveHandler_->Init();
    TELEPHONY_LOGI("SmsReceiveManager init ok.");
}

void SmsReceiveManager::SetCdmaSender(const weak_ptr<SmsSender> &smsSender)
{
    if (cdmaSmsReceiveHandler_ != nullptr) {
        CdmaSmsReceiveHandler *cdmaSmsReceiveHandler = (CdmaSmsReceiveHandler *)(cdmaSmsReceiveHandler_.get());
        cdmaSmsReceiveHandler->SetCdmaSender(smsSender);
    }
}
} // namespace Telephony
} // namespace OHOS
