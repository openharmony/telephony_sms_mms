/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#ifndef MMS_DECODE_BUFFER_H
#define MMS_DECODE_BUFFER_H

#include <string>

#include "mms_buffer.h"

namespace OHOS {
namespace Telephony {
class MmsDecodeBuffer : public MmsBuffer {
public:
    bool PeekOneByte(uint8_t &oneByte);
    bool GetOneByte(uint8_t &oneByte);
    bool IncreasePointer(uint32_t offset);
    bool DecreasePointer(uint32_t offset);
    void MarkPosition();
    void UnMarkPosition();

    bool DecodeIsString();
    bool DecodeShortLength(uint8_t &sValueLength);
    bool DecodeValueLengthReturnLen(uint32_t &valueLength, uint32_t &length);
    bool DecodeValueLength(uint32_t &valueLength);
    bool DecodeUintvar(uint32_t &uintVar, uint32_t &count);
    bool DecodeText(std::string &str, uint32_t &len);
    static bool CharIsToken(uint8_t oneByte);
    bool DecodeTokenText(std::string &str, uint32_t &len);
    bool DecodeQuotedText(std::string &str, uint32_t &len);
    bool DecodeShortInteger(uint8_t &sInteger);
    bool DecodeIsShortInt();
    bool DecodeLongInteger(uint64_t &lInteger);
    bool DecodeInteger(uint64_t &iInteger);
    bool DecodeIsValueLength();

private:
    static constexpr uint8_t EDN_S = 0;
    static constexpr uint8_t NARMAL_TEXT_MIN = 32;
    static constexpr uint8_t NARMAL_TEXT_MAX = 126;
    static constexpr uint8_t LONG_VALUE_LEN_MAX = 8;
};
} // namespace Telephony
} // namespace OHOS
#endif
