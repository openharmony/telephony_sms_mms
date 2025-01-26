/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#ifndef SMS_FFI_H
#define SMS_FFI_H

#include "cj_common_ffi.h"
#include "sms_utils.h"

namespace OHOS {
namespace Telephony {
extern "C" {
FFI_EXPORT CJShortMessage FfiSMSCreateMessage(CArrI32 arr, char* specification);
FFI_EXPORT int32_t FfiSMSGetDefaultSmsSlotId();
FFI_EXPORT RetDataI32 FfiSMSGetDefaultSmsSimId();
FFI_EXPORT bool FfiSMSHasSmsCapability();
}
} // namespace Telephony
} // namespace OHOS

#endif
