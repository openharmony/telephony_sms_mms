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

#ifndef GSM_SMS_TPDU_DECODE_H
#define GSM_SMS_TPDU_DECODE_H

#include "gsm_sms_tpdu_codec.h"

namespace OHOS {
namespace Telephony {
class GsmSmsTpduDecode {
public:
    GsmSmsTpduDecode(std::shared_ptr<GsmUserDataPdu> dataCodec, std::shared_ptr<GsmSmsParamCodec> paramCodec,
        std::shared_ptr<GsmSmsTpduCodec> tpdu);
    ~GsmSmsTpduDecode();
    bool DecodeSubmit(SmsReadBuffer &buffer, struct SmsSubmit *submit);
    bool DecodeDeliver(SmsReadBuffer &buffer, struct SmsDeliver *deliver);
    bool DecodeStatusReport(SmsReadBuffer &buffer, struct SmsStatusReport *statusRep);

private:
    bool DecodeSubmitPartData(SmsReadBuffer &buffer, struct SmsSubmit *submit);
    bool DecodeDeliverPartData(SmsReadBuffer &buffer, struct SmsDeliver *deliver);
    bool DecodeStatusReportPartData(SmsReadBuffer &buffer, struct SmsStatusReport *statusRep);
    bool DecodeStatusReportData(SmsReadBuffer &buffer, struct SmsStatusReport *statusRep);

private:
    std::shared_ptr<GsmUserDataPdu> uDataCodec_ = nullptr;
    std::shared_ptr<GsmSmsParamCodec> paramCodec_ = nullptr;
    std::shared_ptr<GsmSmsTpduCodec> tpdu_ = nullptr;
};
} // namespace Telephony
} // namespace OHOS
#endif