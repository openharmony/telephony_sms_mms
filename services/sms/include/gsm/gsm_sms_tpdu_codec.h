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

#ifndef GSM_SMS_TPDU_CODEC_H
#define GSM_SMS_TPDU_CODEC_H

#include "gsm_user_data_pdu.h"

namespace OHOS {
namespace Telephony {
enum MessageTypeIndicator {
    TYPE_INDICATOR_DELIVER = 0,
    TYPE_INDICATOR_SUBMIT,
    TYPE_INDICATOR_STATUS_REP,
};

enum SmsParseType {
    PARSE_SUBMIT_TYPE,
    PARSE_DELIVER_TYPE,
    PARSE_STATUS_REP_TYPE,
};

class GsmSmsTpduCodec : public std::enable_shared_from_this<GsmSmsTpduCodec> {
public:
    GsmSmsTpduCodec();
    ~GsmSmsTpduCodec();
    bool EncodeSmsPdu(std::shared_ptr<SmsTpdu> sourceData, char *pTpdu, uint16_t pduLen, uint16_t &bufLen);
    bool DecodeSmsPdu(const uint8_t *pTpdu, uint16_t TpduLen, struct SmsTpdu *pSmsTpdu);
    enum SmsPid ParsePid(const uint8_t pid);
    void DebugTpdu(SmsReadBuffer &buffer, const enum SmsParseType type);

private:
    std::shared_ptr<GsmUserDataPdu> uDataCodec_ = nullptr;
    std::shared_ptr<GsmSmsParamCodec> paramCodec_ = nullptr;
    std::shared_ptr<GsmSmsTpduCodec> tpduCodec_ = nullptr;
};
} // namespace Telephony
} // namespace OHOS
#endif