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

#ifndef SMS_WAP_PUSH_BUFFER_H
#define SMS_WAP_PUSH_BUFFER_H

#include <memory>
#include <set>
#include <string>

namespace OHOS {
namespace Telephony {
class SmsWapPushBuffer {
public:
    SmsWapPushBuffer();
    virtual ~SmsWapPushBuffer();

    virtual std::unique_ptr<char[]> ReadDataBuffer(uint32_t desLen);
    virtual std::unique_ptr<char[]> ReadDataBuffer(uint32_t offset, uint32_t desLen);
    virtual bool WriteRawStringBuffer(const std::string &inSrc);
    virtual bool WriteDataBuffer(std::unique_ptr<char[]> inBuff, uint32_t len);
    virtual uint32_t GetCurPosition() const;
    virtual uint32_t GetSize() const;

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
    bool DecodeExtensionMedia();
    bool DecodeConstrainedEncoding();
    bool DecodeTextValue(std::string &str, bool &isNoValue);
    bool DecodeNoValue(bool &isNoValue);

private:
    std::unique_ptr<char[]> pduBuffer_;
    uint32_t curPosition_ = 0;
    uint32_t totolLength_ = 0;
    uint32_t savePosition_ = 0;

private:
    const uint32_t CODE_BUFFER_MAX_SIZE = 300 * 1024;
    static constexpr uint8_t EDN_S = 0;
    static constexpr uint8_t NARMAL_TEXT_MIN = 32;
    static constexpr uint8_t NARMAL_TEXT_MAX = 126;
    static constexpr uint8_t LONG_VALUE_LEN_MAX = 8;
};
} // namespace Telephony
} // namespace OHOS
#endif
