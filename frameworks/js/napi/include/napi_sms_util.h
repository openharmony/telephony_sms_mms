/*
 * Copyright (C) 2026 Huawei Device Co., Ltd.
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

#ifndef NAPI_SMS_UTIL_H
#define NAPI_SMS_UTIL_H

#include <string>
#include <vector>

#include "base_context.h"
#include "js_error_code.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "i_sms_service_interface.h"
#include "telephony_napi_common_error.h"
#include "short_message.h"
#include "napi_util.h"

namespace OHOS {
namespace Telephony {
class NapiSmsUtil {
public:
    static __attribute__((noinline)) void Unref(napi_env env, napi_ref ref);
};
} // namespace Telephony
} // namespace OHOS
#endif // NAPI_SMS_UTIL_H