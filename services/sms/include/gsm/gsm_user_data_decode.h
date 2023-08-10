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

#ifndef GSM_USER_DATA_DECODE_H
#define GSM_USER_DATA_DECODE_H

#include "gsm_user_data_pdu.h"

namespace OHOS {
namespace Telephony {
class GsmUserDataDecode {
public:
    explicit GsmUserDataDecode(std::shared_ptr<GsmUserDataPdu> data);
    ~GsmUserDataDecode();
    bool DecodeGsmPdu(SmsReadBuffer &buffer, bool bHeaderInd, struct SmsUDPackage *pUserData, struct SmsTpud *pTPUD);
    bool DecodeGsmHeadPdu(SmsReadBuffer &buffer, bool bHeaderInd, struct SmsUDPackage *pUserData, struct SmsTpud *pTPUD,
        uint8_t &udl, uint8_t &fillBits);
    bool DecodeGsmBodyPdu(SmsReadBuffer &buffer, bool bHeaderInd, struct SmsUDPackage *pUserData, struct SmsTpud *pTPUD,
        uint8_t &udl, uint8_t fillBits);
    bool Decode8bitPdu(SmsReadBuffer &buffer, bool bHeaderInd, struct SmsUDPackage *pUserData, struct SmsTpud *pTPUD);
    bool DecodeUcs2Pdu(SmsReadBuffer &buffer, bool bHeaderInd, struct SmsUDPackage *pUserData, struct SmsTpud *pTPUD);

private:
    bool DecodeGsmHeadPduPartData(
        SmsReadBuffer &buffer, bool bHeaderInd, struct SmsUDPackage *userData, uint8_t &udl, uint8_t &fillBits);
    bool Decode8bitPduPartData(SmsReadBuffer &buffer, bool bHeaderInd, struct SmsUDPackage *userData,
        struct SmsTpud *pTPUD, uint16_t current, uint8_t udl);
    bool DecodeUcs2PduPartData(
        SmsReadBuffer &buffer, bool bHeaderInd, struct SmsUDPackage *userData, uint16_t current, uint8_t udl);

private:
    std::shared_ptr<GsmUserDataPdu> userData_ = nullptr;
};
} // namespace Telephony
} // namespace OHOS
#endif