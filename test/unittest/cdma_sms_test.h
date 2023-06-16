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

#ifndef CDMA_SMS_TEST_H
#define CDMA_SMS_TEST_H

#include <memory>

#include "cdma_sms_common.h"
#include "cdma_sms_message.h"
#include "sms_send_indexer.h"

namespace OHOS {
namespace Telephony {
class CdmaSmsTest {
public:
    void ProcessTest() const;

private:
    void TestTextBasedSmsDelivery() const;

    void TestEncodeSubmitMsg() const;
    void TestEncodeDeliverReportMsg() const;
    void TestEncodeCancelMsg() const;
    void TestEncodeAckMsg() const;

    void TestDecodeMsg() const;
    void TestDecodeBearerData() const;

    void SetPduSeqInfo(const std::size_t size, const std::unique_ptr<CdmaTransportMsg> &transMsg,
        const std::size_t index, const uint8_t msgRef8bit) const;
    void EncodeMsg(CdmaTransportMsg &msg) const;
    void EncodeSubmitMsg(
        std::string desAddr, std::string scAddr, std::string text, bool force7Bit, bool statusReport) const;

    std::string UnicodeToString(uint8_t *s, int len) const;

    void PrintAddr(TransportAddr &addr, std::string s = "    ") const;
    void PrintSubAddr(TransportSubAddr &addr, std::string s = "    ") const;
    void PrintSmsTeleSvcAddr(SmsTeleSvcAddr &addr, std::string s = "        ") const;
    void PrintHeader(const SmsUDH &header, std::string s = "    ") const;
    void PrintTimeAbs(SmsTimeAbs &time, std::string s = "        ") const;
    void PrintEncodeType(SmsEncodingType encodeType, std::string s = "    ") const;
    void PrintUserData(SmsTeleSvcUserData &userData, std::string s = "        ") const;
    void PrintCmasData(SmsTeleSvcCmasData cmasData, std::string s = "    ") const;
    void PrintSmsReplyOption(SmsReplyOption &replyOpt, std::string s = "        ") const;
    void PrintSmsValPeriod(SmsValPeriod &valPeriod, std::string s = "        ") const;
    void PrintSmsEnhancedVmn(SmsEnhancedVmn &vmn, std::string s = "        ") const;
    void PrintTeleserviceDeliver(TeleserviceDeliver &deliver, bool isCMAS, std::string s = "    ") const;
    void PrintTeleserviceSubmit(TeleserviceSubmit submit, std::string s = "    ") const;
    void PrintTeleserviceDeliverAck(TeleserviceDeliverAck deliveryAck, std::string s = "    ") const;
    void PrintTelesvc(CdmaTeleserviceMsg &telesvcMsg, bool isCMAS = false, std::string s = "    ") const;
    void PrintMsg(CdmaTransportMsg &msg) const;
};

} // namespace Telephony
} // namespace OHOS
#endif