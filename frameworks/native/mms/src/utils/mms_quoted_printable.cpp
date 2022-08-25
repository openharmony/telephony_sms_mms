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

#include "mms_quoted_printable.h"

#include "memory"
#include "securec.h"
#include "string"

namespace OHOS {
namespace Telephony {
std::string MmsQuotedPrintable::Encode(const std::string &input)
{
    const unsigned char asciiMin = 33;
    const unsigned char assciiMax = 126;
    const unsigned char equalChar = 61;
    const char hex[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
    std::string codeString = "";
    for (auto byte : input) {
        if ((byte >= asciiMin && byte < equalChar) || (byte > equalChar && byte <= assciiMax)) {
            codeString += byte;
        } else {
            codeString += '=';
            codeString += hex[((byte >> 0x04) & 0x0F)];
            codeString += hex[(byte & 0x0F)];
        }
    }
    return codeString;
}

bool MmsQuotedPrintable::Decode(const std::string src, std::string &dest)
{
    const int endLineCharNum = 3;

    uint32_t inLength = 0;
    inLength = src.length();
    if (inLength == 0) {
        return false;
    }

    std::unique_ptr<char[]> tempBuffer = std::make_unique<char[]>(src.length());
    if (tempBuffer == nullptr) {
        return false;
    }

    uint32_t index = 0;
    uint32_t outLength = 0;
    const char *input = src.data();
    while (index < inLength) {
        if (strncmp(input, "=/r/n", endLineCharNum) == 0) {
            input += endLineCharNum;
            index += endLineCharNum;
        } else {
            uint32_t hexChar = 0;
            if (*input == '=' && sscanf_s(input, "=%02X", &hexChar) >= 1) {
                tempBuffer[outLength] = static_cast<char>(hexChar);
                input += endLineCharNum;
                index += endLineCharNum;
            } else {
                tempBuffer[outLength] = *input;
                input++;
                index++;
            }
            outLength++;
        }
    }
    tempBuffer[outLength] = '\0';
    dest = std::string(tempBuffer.get(), outLength);
    if (tempBuffer) {
        tempBuffer.reset();
    }
    return true;
}
} // namespace Telephony
} // namespace OHOS
