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

#include "napi_sms_util.h"

#include <uv.h>

#include "telephony_log_wrapper.h"
#include "napi_mms.h"
namespace OHOS {
namespace Telephony {
    void __attribute__((noinline)) NapiSmsUtil::Unref(napi_env env, napi_ref ref)
{
    uint32_t refCount = 0;
    if (napi_reference_unref(env, ref, &refCount) == napi_ok && refCount == 0) {
        napi_delete_reference(env, ref);
    }
}

void NapiSmsUtil::CloseHandleScope(napi_handle_scope scope, napi_env env, napi_ref ref1, napi_ref ref2)
{
    napi_close_handle_scope(env, scope);
    Unref(env, ref1);
    Unref(env, ref2);
}
} // namespace Telephony
} // namespace OHOS