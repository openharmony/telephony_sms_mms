/*
 * Copyright (C)2023 Huawei Device Co., Ltd.
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

#include "cb_start_ability.h"

#include "ability_manager_client.h"
#include "int_wrapper.h"
#include "string_wrapper.h"
#include "telephony_errors.h"
#include "telephony_log_wrapper.h"
#include "want.h"

namespace OHOS {
namespace Telephony {
const std::string BUNDLE_NAME_BEGIN = "com.hua";
const std::string BUNDLE_NAME_END = "wei.hmos.cellbroadcast";
constexpr const char *ABILITY_NAME = "AlertService";
constexpr const char *MODULE_NAME = "entry";
static constexpr int32_t USER_ID = 100;

CbStartAbility::~CbStartAbility() {}

CbStartAbility::CbStartAbility() {}

void CbStartAbility::StartAbility(AAFwk::Want &want)
{
    TELEPHONY_LOGI("start cellbroadcast ability");
    want.SetElementName("", BUNDLE_NAME_BEGIN + BUNDLE_NAME_END, ABILITY_NAME, MODULE_NAME);
    ErrCode code = AAFwk::AbilityManagerClient::GetInstance()->StartExtensionAbility(want, nullptr, USER_ID);
    TELEPHONY_LOGI("start ability code:%{public}d", code);
}
} // namespace Telephony
} // namespace OHOS
