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

#include "runner_pool.h"

#include "string_utils.h"
#include "telephony_log_wrapper.h"
#include "telephony_types.h"

namespace OHOS {
namespace Telephony {
using namespace std;
RunnerPool RunnerPool::runnerPool_;

RunnerPool &RunnerPool::GetInstance()
{
    return runnerPool_;
}

void RunnerPool::Init()
{
    if (isInit_) {
        TELEPHONY_LOGI("RunnerPool in sms_mms has init");
        return;
    }
    smsCommonRunner_ = CreateRunner("smsCommonRunner");
    if (smsCommonRunner_ == nullptr) {
        return;
    }
    for (int32_t slotId = 0; slotId < SIM_SLOT_COUNT; ++slotId) {
        auto runnerTmp = CreateRunner("smsSendReceiveRunner_" + to_string(slotId));
        if (runnerTmp == nullptr) {
            return;
        }
        smsSendReceiveRunnerMap_.insert(make_pair(slotId, runnerTmp));
    }
    isInit_ = true;
    TELEPHONY_LOGI("RunnerPool in sms_mms init success");
}

std::shared_ptr<AppExecFwk::EventRunner> RunnerPool::CreateRunner(const std::string &name)
{
    auto runner = AppExecFwk::EventRunner::Create(name);
    if (runner == nullptr) {
        TELEPHONY_LOGE("%{public}s runner create thread fail!", name.c_str());
        return nullptr;
    }
    runner->Run();
    return runner;
}

std::shared_ptr<AppExecFwk::EventRunner> RunnerPool::GetSmsCommonRunner()
{
    return smsCommonRunner_;
}

std::shared_ptr<AppExecFwk::EventRunner> RunnerPool::GetSmsSendReceiveRunnerBySlotId(const int32_t slotId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    std::map<uint32_t, std::shared_ptr<AppExecFwk::EventRunner>>::iterator iter = smsSendReceiveRunnerMap_.find(slotId);
    if (iter != smsSendReceiveRunnerMap_.end()) {
        return iter->second;
    }
    return nullptr;
}
} // namespace Telephony
} // namespace OHOS
