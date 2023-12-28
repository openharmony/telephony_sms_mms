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

#ifndef NAPI_SEND_RECV_H
#define NAPI_SEND_RECV_H
#include <codecvt>
#include <locale>

#include "base_context.h"
#include "mms_codec_type.h"
#include "mms_msg.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "napi_sms.h"

namespace OHOS {
namespace Telephony {
struct MmsConfigPara {
    std::u16string userAgent = u"";
    std::u16string userAgentProfile = u"";
};

struct MmsContext : BaseContext {
    int32_t slotId = DEFAULT_SIM_SLOT_ID;
    std::u16string mmsc = u"";
    std::u16string data = u"";
    MmsConfigPara mmsConfig;
};

class NapiSendRecvMms {
public:
    NapiSendRecvMms();
    ~NapiSendRecvMms() = default;
    static napi_value SendMms(napi_env env, napi_callback_info info);
    static napi_value DownloadMms(napi_env env, napi_callback_info info);

public:
    static std::mutex downloadCtx_;
    static std::mutex countCtx_;
    static std::int32_t reqCount_;
    static bool waitFlag;
};
} // namespace Telephony
} // namespace OHOS
#endif // NAPI_SEND_RECV_H