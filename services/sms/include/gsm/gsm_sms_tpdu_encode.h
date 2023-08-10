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

#ifndef GSM_SMS_TPDU_ENCODE_H
#define GSM_SMS_TPDU_ENCODE_H

#include "gsm_sms_tpdu_codec.h"

namespace OHOS {
namespace Telephony {
class GsmSmsTpduEncode {
public:
    GsmSmsTpduEncode(std::shared_ptr<GsmUserDataPdu> dataCodec, std::shared_ptr<GsmSmsParamCodec> paramCodec,
        std::shared_ptr<GsmSmsTpduCodec> tpdu);
    ~GsmSmsTpduEncode();

    bool EncodeSubmitPdu(SmsWriteBuffer &buffer, const struct SmsSubmit *pSubmit);
    void EncodeSubmitTpduType(SmsWriteBuffer &buffer, const struct SmsSubmit &pSubmit);
    bool EncodeDeliverPdu(SmsWriteBuffer &buffer, const struct SmsDeliver *pDeliver);
    bool EncodeDeliverReportPdu(SmsWriteBuffer &buffer, const struct SmsDeliverReport *pDeliverRep);
    bool EncodeStatusReportPdu(SmsWriteBuffer &buffer, const struct SmsStatusReport *pStatusRep);

private:
    bool EncodeSubmitPduPartData(SmsWriteBuffer &buffer, const struct SmsSubmit *pSubmit);
    void EncodeSubmitTypePartData(SmsWriteBuffer &buffer, const struct SmsSubmit &pSubmit);
    void EncodeSubmitTypeData(SmsWriteBuffer &buffer, const struct SmsSubmit &pSubmit);
    bool EncodeDeliverPartData(SmsWriteBuffer &buffer, const struct SmsDeliver *pDeliver);
    bool EncodeDeliverData(SmsWriteBuffer &buffer, const struct SmsDeliver *pDeliver);
    bool EncodeDeliverReportPartData(SmsWriteBuffer &buffer, const struct SmsDeliverReport *pDeliverRep);
    bool EncodeStatusReportPartData(SmsWriteBuffer &buffer, const struct SmsStatusReport *pStatusRep);
    bool EncodeStatusReportData(SmsWriteBuffer &buffer, const struct SmsStatusReport *pStatusRep, uint8_t length);

private:
    std::shared_ptr<GsmUserDataPdu> uDataCodec_ = nullptr;
    std::shared_ptr<GsmSmsParamCodec> paramCodec_ = nullptr;
    std::shared_ptr<GsmSmsTpduCodec> tpdu_ = nullptr;
};
} // namespace Telephony
} // namespace OHOS
#endif