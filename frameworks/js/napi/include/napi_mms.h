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

#ifndef NAPI_MMS_H
#define NAPI_MMS_H
#include <codecvt>
#include <cstring>
#include <locale>

#include "base_context.h"
#include "mms_codec_type.h"
#include "mms_msg.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "napi_sms.h"

namespace OHOS {
namespace Telephony {
enum MessageType {
    TYPE_MMS_SEND_REQ = 128,
    TYPE_MMS_SEND_CONF,
    TYPE_MMS_NOTIFICATION_IND,
    TYPE_MMS_RESP_IND,
    TYPE_MMS_RETRIEVE_CONF,
    TYPE_MMS_ACKNOWLEDGE_IND,
    TYPE_MMS_DELIVERY_IND,
    TYPE_MMS_READ_REC_IND,
    TYPE_MMS_READ_ORIG_IND,
};

enum DispositionValue {
    FROM_DATA = 0,
    ATTACHMENT,
    INLINE,
};

class NapiMms {
public:
    NapiMms();
    ~NapiMms() = default;

    static napi_value InitEnumMmsCharSets(napi_env env, napi_value exports);
    static napi_value InitEnumMessageType(napi_env env, napi_value exports);
    static napi_value InitEnumPriorityType(napi_env env, napi_value exports);
    static napi_value InitEnumVersionType(napi_env env, napi_value exports);
    static napi_value InitEnumDispositionType(napi_env env, napi_value exports);
    static napi_value InitEnumReportAllowedType(napi_env env, napi_value exports);
    static napi_value InitSupportEnumMmsCharSets(napi_env env, napi_value exports);
    static napi_value InitSupportEnumMessageType(napi_env env, napi_value exports);
    static napi_value InitSupportEnumPriorityType(napi_env env, napi_value exports);
    static napi_value InitSupportEnumVersionType(napi_env env, napi_value exports);
    static napi_value InitSupportEnumDispositionType(napi_env env, napi_value exports);
    static napi_value InitSupportEnumReportAllowedType(napi_env env, napi_value exports);
};
} // namespace Telephony
} // namespace OHOS
#endif // NAPI_SMS_H