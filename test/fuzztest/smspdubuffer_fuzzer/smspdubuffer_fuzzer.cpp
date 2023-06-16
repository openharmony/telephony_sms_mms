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

#include "smspdubuffer_fuzzer.h"

#include "addsmstoken_fuzzer.h"
#include "sms_pdu_buffer.h"

using namespace OHOS::Telephony;
namespace OHOS {
static constexpr uint8_t TEST_BITS = 16;

void ReadBuffer(const uint8_t *data, size_t size)
{
    std::string strValue(reinterpret_cast<const char *>(data), size);
    auto buffer = std::make_unique<SmsReadBuffer>(strValue);
    if (buffer == nullptr) {
        return;
    }
    buffer->IsEmpty();

    uint16_t desLen = static_cast<uint16_t>(size);
    desLen += desLen;
    for (uint16_t i = 0; i < desLen; i++) {
        buffer->SetIndex(i);
        buffer->GetIndex();
    }
    for (uint16_t i = 0; i < desLen; i++) {
        buffer->MoveBack(i);
    }
    for (uint16_t i = 0; i < desLen; i++) {
        buffer->MoveForward(i);
    }

    buffer->SetIndex(0);
    uint8_t v1 = 0;
    uint16_t v2 = 0;
    for (uint16_t i = 0; i < desLen; i++) {
        buffer->ReadByte(v1);
        buffer->ReadWord(v2);
    }

    buffer->SetIndex(0);
    uint8_t v3 = 0;
    for (uint16_t i = 0; i < desLen; i++) {
        for (uint8_t j = 0; j < TEST_BITS; j++) {
            buffer->ReadBits(v3, j);
        }
    }
    buffer->SkipBits();
}

void WriteBuffer(const uint8_t *data, size_t size)
{
    auto buffer = std::make_unique<SmsWriteBuffer>();
    if (buffer == nullptr) {
        return;
    }
    buffer->IsEmpty();

    for (uint16_t i = 0; i < size; i++) {
        buffer->SetIndex(i);
        buffer->GetIndex();
    }
    for (uint16_t i = 0; i < size; i++) {
        buffer->MoveBack(i);
    }
    for (uint16_t i = 0; i < size; i++) {
        buffer->MoveForward(i);
    }

    buffer->SetIndex(0);
    for (uint16_t i = 0; i < size; i++) {
        buffer->WriteByte(data[i]);
    }
    auto pduBuffer = buffer->GetPduBuffer();

    buffer->SetIndex(0);
    for (uint16_t i = 0; i < size; i++) {
        buffer->WriteWord(static_cast<uint16_t>(data[i]));
    }
    auto pduBuffer2 = buffer->GetPduBuffer();

    buffer->SetIndex(0);
    uint8_t v3 = 0;
    for (uint16_t i = 0; i < size; i++) {
        for (uint8_t j = 0; j < TEST_BITS; j++) {
            buffer->WriteBits(v3, j);
        }
    }
    buffer->SkipBits();
    auto pduBuffer3 = buffer->GetPduBuffer();

    buffer->SetIndex(0);
    for (uint16_t i = 0; i < size; i++) {
        buffer->InsertByte(data[i], i);
    }
    auto pduBuffer4 = buffer->GetPduBuffer();
}

void DoSomethingInterestingWithMyAPI(const uint8_t *data, size_t size)
{
    if (data == nullptr || size == 0) {
        return;
    }

    ReadBuffer(data, size);
    WriteBuffer(data, size);
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
