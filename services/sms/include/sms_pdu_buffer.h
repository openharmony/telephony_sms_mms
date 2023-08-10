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

#ifndef SMS_PDU_BUFFER_H
#define SMS_PDU_BUFFER_H

#include <vector>

namespace OHOS {
namespace Telephony {

enum BITS : uint8_t { BIT0 = 0, BIT1, BIT2, BIT3, BIT4, BIT5, BIT6, BIT7, BIT8 };

class SmsPduBuffer {
public:
    virtual ~SmsPduBuffer();
    virtual bool IsEmpty();
    uint16_t GetIndex();
    uint16_t GetSize();
    bool SetIndex(uint16_t index);
    uint16_t MoveForward(uint16_t len = 1);
    uint16_t MoveBack(uint16_t len = 1);
    uint16_t SkipBits();
    std::unique_ptr<std::vector<uint8_t>> GetPduBuffer();

public:
    std::unique_ptr<uint8_t[]> data_ { nullptr };

protected:
    uint16_t index_ { 0 };
    uint16_t length_ { 0 };
    uint8_t bitIndex_ { 0 };
};

class SmsReadBuffer : public SmsPduBuffer {
public:
    explicit SmsReadBuffer(const std::string &pdu);
    bool ReadByte(uint8_t &v);
    bool PickOneByte(uint8_t &v);
    bool PickOneByteFromIndex(uint16_t index, uint8_t &v);
    bool ReadWord(uint16_t &v);
    bool ReadBits(uint8_t &v, uint8_t l = BIT1);
};

class SmsWriteBuffer : public SmsPduBuffer {
public:
    SmsWriteBuffer();
    bool WriteByte(uint8_t v);
    bool GetTopValue(uint8_t &oneByte);
    bool GetValueFromIndex(uint16_t index, uint8_t &v);
    bool WriteWord(uint16_t v);
    bool WriteBits(uint8_t v, uint8_t l = BIT1);
    bool InsertByte(uint8_t v, uint16_t index);
};

} // namespace Telephony
} // namespace OHOS
#endif
