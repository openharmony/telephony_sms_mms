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

#include "gsmsmsparamcodec_fuzzer.h"

#define private public
#include "addsmstoken_fuzzer.h"
#include "i_sms_service_interface.h"
#include "sms_service.h"

using namespace OHOS::Telephony;
namespace OHOS {
static bool g_isInited = false;
static constexpr int32_t UINT8_COUNT = 256;
static constexpr int32_t CODE_SCHEME_SIZE = 6;
static constexpr int32_t TIME_FORMAT_SIZE = 3;

bool IsServiceInited()
{
    if (!g_isInited) {
        DelayedSingleton<SmsService>::GetInstance()->OnStart();
        if (DelayedSingleton<SmsService>::GetInstance()->GetServiceRunningState() ==
            static_cast<int32_t>(Telephony::ServiceRunningState::STATE_RUNNING)) {
            g_isInited = true;
        }
    }
    return g_isInited;
}

void EncodePduItemsTest(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }
    auto codec = std::make_shared<GsmSmsParamCodec>();
    AddressNumber *pAddress = new AddressNumber();
    std::string addrParam(reinterpret_cast<const char *>(data), size);
    codec->EncodeAddressPdu(pAddress, addrParam);

    uint8_t decodeData[UINT8_COUNT + 1];
    uint16_t len = UINT8_COUNT < size ? UINT8_COUNT : size;
    if (memcpy_s(decodeData, len, data, len) != EOK) {
        return;
    }
    codec->EncodeSmscPdu(addrParam.c_str(), decodeData);
    pAddress->address[0] = data[0];
    codec->EncodeAddressPdu(pAddress, addrParam);
    codec->EncodeSmscPdu(pAddress, decodeData, len);
    codec->EncodeAddressPdu(nullptr, addrParam);
    codec->EncodeSmscPdu(nullptr, decodeData);
    codec->EncodeSmscPdu(nullptr, decodeData, len);

    std::string timeParam(reinterpret_cast<const char *>(data), size);
    codec->EncodeTimePdu(nullptr, timeParam);
    SmsTimeStamp *stamp = new SmsTimeStamp();
    stamp->format = static_cast<SmsTimeFormat>(size % TIME_FORMAT_SIZE);
    stamp->time.absolute.timeZone = size;
    codec->EncodeTimePdu(stamp, timeParam);

    SmsDcs *dcs = new SmsDcs();
    std::string dcsParam(reinterpret_cast<const char *>(data), size);
    codec->EncodeDCS(nullptr, dcsParam);
    dcs->codingGroup = static_cast<PduSchemeGroup>(size % CODE_SCHEME_SIZE);
    dcs->codingScheme = static_cast<DataCodingScheme>(size % CODE_SCHEME_SIZE);
    codec->EncodeDCS(dcs, dcsParam);
}

void DecodePduItemsTest(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }

    auto codec = std::make_shared<GsmSmsParamCodec>();
    std::string pdu(reinterpret_cast<const char *>(data), size);
    SmsReadBuffer buffer(pdu);
    AddressNumber *address = new AddressNumber();
    codec->DecodeAddressPdu(buffer, address);

    SmsTimeStamp *stamp = new SmsTimeStamp();
    codec->DecodeTimePdu(buffer, stamp);

    SmsDcs *dcs = new SmsDcs();
    codec->DecodeDcsPdu(buffer, dcs);

    int32_t setType[UINT8_COUNT] = { 0 };
    int32_t indType[UINT8_COUNT] { 0 };
    codec->CheckVoicemail(buffer, setType, indType);
}

void DoSomethingInterestingWithMyAPI(const uint8_t *data, size_t size)
{
    if (data == nullptr || size == 0) {
        return;
    }

    EncodePduItemsTest(data, size);
    DecodePduItemsTest(data, size);
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
