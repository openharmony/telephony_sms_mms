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

#ifndef MMS_ENCODE_BUFFER_H
#define MMS_ENCODE_BUFFER_H

#include <string>

#include "mms_buffer.h"

namespace OHOS {
namespace Telephony {
class MmsEncodeBuffer : public MmsBuffer {
public:
    bool WriteByte(uint8_t byte);
    bool WriteBuffer(MmsEncodeBuffer &buff);
    bool WriteBuffer(std::unique_ptr<char[]> input, uint32_t length);

    bool EncodeUintvar(uint32_t value);
    bool EncodeShortLength(uint8_t value);
    bool EncodeValueLength(uint32_t value);

    bool EncodeOctet(uint8_t value);
    bool EncodeShortInteger(uint8_t value);
    bool EncodeInteger(uint32_t value);
    bool EncodeLongInteger(uint64_t value);
    bool EncodeText(std::string value);
    bool EncodeQuotedText(std::string value);
    bool EncodeTokenText(std::string value);
};
} // namespace Telephony
} // namespace OHOS
#endif
