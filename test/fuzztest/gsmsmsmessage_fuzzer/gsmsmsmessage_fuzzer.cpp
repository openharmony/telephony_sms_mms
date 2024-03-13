/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "gsmsmsmessage_fuzzer.h"

#define private public
#include "addsmstoken_fuzzer.h"
#include "core_manager_inner.h"
#include "i_sms_service_interface.h"
#include "sms_service.h"

using namespace OHOS::Telephony;
namespace OHOS {
static bool g_isInited = false;
static constexpr int32_t SLOT_NUM = 2;
static constexpr int32_t UINT8_COUNT = 256;
static constexpr int32_t UINT16_COUNT = 65536;
static constexpr int32_t DATA_LEN = 160 * 15;
static constexpr int32_t CODE_SCHEME_SIZE = 6;
constexpr int32_t SLEEP_TIME_SECONDS = 1;

bool IsServiceInited()
{
    if (!g_isInited) {
        CoreManagerInner::GetInstance().isInitAllObj_ = true;
        DelayedSingleton<SmsService>::GetInstance()->registerToService_ = true;
        DelayedSingleton<SmsService>::GetInstance()->WaitCoreServiceToInit();
        DelayedSingleton<SmsService>::GetInstance()->OnStart();
        if (DelayedSingleton<SmsService>::GetInstance()->GetServiceRunningState() ==
            static_cast<int32_t>(Telephony::ServiceRunningState::STATE_RUNNING)) {
            g_isInited = true;
        }
    }
    return g_isInited;
}

void CreateMessageTest(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }
    std::string pdu(reinterpret_cast<const char *>(data), size);
    GsmSmsMessage msg;
    msg.CreateMessage(pdu);
    msg.PduAnalysis(pdu);
    SmsDeliver deliver;
    msg.AnalysisMsgDeliver(deliver);
    SmsStatusReport status;
    msg.AnalysisMsgStatusReport(status);
    SmsSubmit submit;
    msg.AnalysisMsgSubmit(submit);
}

void CalcReplyEncodeAddress(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }
    GsmSmsMessage msg;
    std::string replyAddr(reinterpret_cast<const char *>(data), size);
    msg.CalcReplyEncodeAddress(replyAddr);
    std::string replyAddress(reinterpret_cast<const char *>(data), 0);
    msg.CalcReplyEncodeAddress(replyAddress);

    SmsTimeStamp times;
    times.format = static_cast<SmsTimeFormat>(size);
    times.time.absolute.day = static_cast<uint8_t>(size);
    times.time.absolute.hour = static_cast<uint8_t>(size);
    times.time.absolute.minute = static_cast<uint8_t>(size);
    times.time.absolute.month = static_cast<uint8_t>(size);
    times.time.absolute.second = static_cast<uint8_t>(size);
    times.time.absolute.timeZone = static_cast<uint8_t>(size);
    times.time.absolute.year = static_cast<uint8_t>(size);
    msg.ConvertMsgTimeStamp(times);

    SmsTimeStamp stamp;
    stamp.format = static_cast<SmsTimeFormat>(size);
    stamp.time.relative.time = static_cast<uint8_t>(size);
    msg.ConvertMsgTimeStamp(stamp);

    msg.GetIsSIMDataTypeDownload();
    msg.GetIsTypeZeroInd();
    msg.GetGsm();
    msg.GetIsSmsText();
    msg.GetDestPort();
    msg.GetDestAddress();
    msg.GetReplyAddress();
    msg.GetFullText();

    std::string text(reinterpret_cast<const char *>(data), size);
    msg.SetFullText(text);
    msg.ConvertUserData();
    msg.ConvertUserPartData();
    msg.GetFullText();
    msg.CreateDeliverSmsTpdu();
    msg.CreateDeliverReportSmsTpdu();
    msg.CreateStatusReportSmsTpdu();
    msg.ConvertMessageDcs();
    std::string addr(reinterpret_cast<const char *>(data), size);
    msg.SetDestAddress(addr);
}

void SplitMessageAndCreateSubmitTest(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }
    GsmSmsMessage msg;
    std::vector<struct SplitInfo> cellsInfos;
    std::string text(reinterpret_cast<const char *>(data), size);
    bool force7BitCode = (size % SLOT_NUM == 1);
    DataCodingScheme codingType = static_cast<DataCodingScheme>(size % CODE_SCHEME_SIZE);
    msg.SplitMessage(cellsInfos, text, force7BitCode, codingType, false, "");
    bool isStatusReport = (size % SLOT_NUM == 0);
    std::string desAddr(reinterpret_cast<const char *>(data), size);
    std::string scAddr(reinterpret_cast<const char *>(data), size);
    msg.CreateDefaultSubmitSmsTpdu(desAddr, scAddr, text, isStatusReport, codingType);
    msg.SplitMessage(cellsInfos, text, force7BitCode, codingType, true, "");
    uint8_t msgRef8bit = size % UINT8_COUNT;
    msg.CreateDataSubmitSmsTpdu(desAddr, scAddr, size, data, size, msgRef8bit, codingType, isStatusReport);
    bool bMore = (size % SLOT_NUM == 1);
    msg.ConvertUserData();
    msg.ConvertUserPartData();
    msg.GetSubmitEncodeInfo(text, bMore);
    uint8_t decodeData[DATA_LEN + 1];
    uint16_t len = DATA_LEN < size ? DATA_LEN : size;
    if (memcpy_s(decodeData, len, data, len) != EOK) {
        return;
    }
    msg.GetSubmitEncodeInfoPartData(decodeData, size, bMore);
    msg.SetHeaderReply(size);
    SmsConcat contact;
    contact.is8Bits = (size % SLOT_NUM == 1);
    contact.msgRef = size % UINT16_COUNT;
    contact.seqNum = size % UINT16_COUNT;
    contact.totalSeg = size % UINT16_COUNT;
    msg.IsSpecialMessage();
}

void DoSomethingInterestingWithMyAPI(const uint8_t *data, size_t size)
{
    if (data == nullptr || size == 0) {
        return;
    }
    CalcReplyEncodeAddress(data, size);
    SplitMessageAndCreateSubmitTest(data, size);
    CreateMessageTest(data, size);
    sleep(SLEEP_TIME_SECONDS);
    DelayedSingleton<SmsService>::DestroyInstance();
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    /* Run your code on data */
    OHOS::AddSmsTokenFuzzer token;
    OHOS::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}
