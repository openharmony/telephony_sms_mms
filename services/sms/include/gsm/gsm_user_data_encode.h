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

#ifndef GSM_USER_DATA_ENCODE_H
#define GSM_USER_DATA_ENCODE_H

#include "gsm_user_data_pdu.h"

namespace OHOS {
namespace Telephony {
class GsmUserDataEncode {
public:
    explicit GsmUserDataEncode(std::shared_ptr<GsmUserDataPdu> data);
    ~GsmUserDataEncode();
    bool EncodeGsmPdu(SmsWriteBuffer &buffer, const struct SmsUDPackage *pUserData);
    bool Encode8bitPdu(SmsWriteBuffer &buffer, const struct SmsUDPackage *pUserData, std::string &destAddr);
    bool EncodeUcs2Pdu(SmsWriteBuffer &buffer, const struct SmsUDPackage *pUserData);

private:
    bool EncodeGsmHeadPdu(SmsWriteBuffer &buffer, const struct SmsUDPackage *pUserData, uint8_t &fillBits);
    bool EncodeGsmBodyPdu(SmsWriteBuffer &buffer, const struct SmsUDPackage *pUserData, uint8_t fillBits);
    bool Encode8bitHeadPdu(SmsWriteBuffer &buffer, const struct SmsUDPackage *pUserData, std::string &destAddr);
    bool Encode8bitBodyPdu(SmsWriteBuffer &buffer, const struct SmsUDPackage *pUserData);
    bool EncodeUcs2HeadPdu(SmsWriteBuffer &buffer, const struct SmsUDPackage *pUserData);
    bool EncodeUcs2BodyPdu(SmsWriteBuffer &buffer, const struct SmsUDPackage *pUserData);

private:
    std::shared_ptr<GsmUserDataPdu> userData_ = nullptr;
};
} // namespace Telephony
} // namespace OHOS
#endif