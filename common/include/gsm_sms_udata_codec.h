/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
 * Copyright (C) 2014 Samsung Electronics Co., Ltd. All rights reserved
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

#ifndef GSM_SMS_UDCODEC_H
#define GSM_SMS_UDCODEC_H

#include "gsm_sms_param_codec.h"

namespace OHOS {
namespace Telephony {
class GsmSmsUDataCodec {
public:
    GsmSmsUDataCodec() = default;
    virtual ~GsmSmsUDataCodec() = default;
    static int EncodeUserData(
        const struct SmsUserData *(&pUserData), SmsCodingScheme CodingScheme, char *(&pEncodeData));
    static int DecodeUserData(const unsigned char *pTpdu, const int tpduLen, bool bHeaderInd,
        SmsCodingScheme CodingScheme, struct SmsUserData *pUserData);
    static int DecodeUserData(const unsigned char *pTpdu, const int tpduLen, bool bHeaderInd,
        SmsCodingScheme CodingScheme, struct SmsUserData *pUserData, struct SmsTpud *pTPUD);

    static int EncodeGSMData(const struct SmsUserData *pUserData, char *pEncodeData);
    static int Encode8bitData(const struct SmsUserData *pUserData, char *pEncodeData);
    static int EncodeUCS2Data(const struct SmsUserData *pUserData, char *pEncodeData);

    static int DecodeGSMData(const unsigned char *pTpdu, const int tpduLen, bool bHeaderInd,
        struct SmsUserData *pUserData, struct SmsTpud *pTPUD);
    static int Decode8bitData(
        const unsigned char *pTpdu, bool bHeaderInd, struct SmsUserData *pUserData, struct SmsTpud *pTPUD);
    static int DecodeUCS2Data(const unsigned char *pTpdu, const int tpduLen, bool bHeaderInd,
        struct SmsUserData *pUserData, struct SmsTpud *pTPUD);
    static int EncodeHeader(const struct SmsUDH header, char *pEncodeHeader);
    static int DecodeHeader(const unsigned char *pTpdu, struct SmsUDH *pHeader);
    static void ResetUserData(struct SmsUserData &userData);

private:
    static int EncodeHeaderConcat(const struct SmsUDH header, char *pEncodeHeader);
    static void DebugDecodeHeader(const struct SmsUDH *pHeader);
    static bool GetHeaderCnt(const unsigned char *pTpdu, struct SmsUserData *pUserData, int &offset, int &udhl, int i);
};
} // namespace Telephony
} // namespace OHOS
#endif