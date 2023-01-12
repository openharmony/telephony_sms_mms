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

#ifndef SMS_MMS_ERRORS_H
#define SMS_MMS_ERRORS_H

#include "telephony_errors.h"

namespace OHOS {
namespace Telephony {
enum SmsMmsErrorCode {
    SMS_MMS_DECODE_DATA_EMPTY = SMS_MMS_ERR_OFFSET,
    SMS_MMS_UNKNOWN_SIM_MESSAGE_STATUS,
    SMS_MMS_MESSAGE_LENGTH_OUT_OF_RANGE,
};
} // namespace Telephony
} // namespace OHOS
#endif // SMS_MMS_ERRORS_H
