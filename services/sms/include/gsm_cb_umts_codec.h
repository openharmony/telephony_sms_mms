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

#ifndef GSM_CB_UMTS_CODEC_H
#define GSM_CB_UMTS_CODEC_H

#include "gsm_cb_codec.h"

namespace OHOS {
namespace Telephony {
class GsmCbUmtsCodec {
public:
    GsmCbUmtsCodec(std::shared_ptr<GsmCbCodec::GsmCbMessageHeader> header, std::shared_ptr<GsmCbPduDecodeBuffer> buffer,
        std::shared_ptr<GsmCbCodec> cbCodec);
    ~GsmCbUmtsCodec();
    bool Decode3gHeader();
    bool Decode3gCbMsg();

private:
    bool Decode3g7Bit();
    bool Decode3gUCS2();
    bool Decode3gHeaderPartData(uint8_t dcs);

private:
    std::shared_ptr<GsmCbCodec::GsmCbMessageHeader> cbHeader_ { nullptr };
    std::string messageRaw_;
    std::shared_ptr<GsmCbPduDecodeBuffer> cbPduBuffer_ { nullptr };
    std::shared_ptr<GsmCbCodec> cbCodec_ { nullptr };
};
} // namespace Telephony
} // namespace OHOS
#endif // GSM_CB_UMTS_CODEC_H