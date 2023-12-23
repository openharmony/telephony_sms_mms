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

#ifndef TEXT_CODER_H
#define TEXT_CODER_H

#include <map>

#include "gsm_pdu_code_type.h"
#include "msg_text_convert_common.h"

namespace OHOS {
namespace Telephony {

class TextCoder {
public:
    static TextCoder &Instance();
    void Base64Encode(const std::string &src, std::string &dest);
    void Base64Decode(const std::string &src, std::string &dest);
    bool GetEncodeString(
        std::string &encodeString, uint32_t charset, uint32_t valLength, const std::string &strEncodeString);
    int Utf8ToGsm7bit(uint8_t *dest, int maxLength, const uint8_t *src, int srcLength, MSG_LANGUAGE_ID_T &langId);
    int Utf8ToUcs2(uint8_t *dest, int maxLength, const uint8_t *src, int srcLength);
    int CdmaUtf8ToAuto(uint8_t *dest, int maxLength, const uint8_t *src, int srcLength, DataCodingScheme &scheme);
    int GsmUtf8ToAuto(uint8_t *dest, int maxLength, const uint8_t *src, int srcLength, DataCodingScheme &scheme,
        SmsCodingNationalType codingNationalType, MSG_LANGUAGE_ID_T &langId);
    int Gsm7bitToUtf8(uint8_t *dest, int maxLength, const uint8_t *src, int srcLength, const MsgLangInfo &langInfo);
    int Ucs2ToUtf8(uint8_t *dest, int maxLength, const uint8_t *src, int srcLength);
    int EuckrToUtf8(uint8_t *dest, int maxLength, const uint8_t *src, int srcLength);
    int ShiftjisToUtf8(uint8_t *dest, int maxLength, const uint8_t *src, int srcLength) const;

private:
    TextCoder();
    virtual ~TextCoder();
    int Ucs2ToGsm7bit(uint8_t *dest, int maxLength, const uint8_t *src, int srcLength, MSG_LANGUAGE_ID_T &langId);
    int Ucs2ToGsm7bitAuto(uint8_t *dest, int maxLength, const uint8_t *src, int srcLength, bool &unknown,
        SmsCodingNationalType codingNationalType);
    int Ucs2ToAscii(uint8_t *dest, int maxLength, const uint8_t *src, int srcLength, bool &unknown);
    uint8_t GetLangType(const uint8_t *src, int srcLength);
    int FindGsm7bitExt(uint8_t *dest, int maxLength, const uint16_t inText);
    int FindTurkish(uint8_t *dest, int maxLength, const uint16_t inText);
    int FindSpanish(uint8_t *dest, int maxLength, const uint16_t inText);
    int FindPortu(uint8_t *dest, int maxLength, const uint16_t inText);
    uint8_t FindReplaceChar(const uint16_t inText);
    std::map<uint16_t, uint8_t> Get7BitCodingExtMap(SmsCodingNationalType codingNationalType) const;

    int Gsm7bitToUcs2(uint8_t *dest, int maxLength, const uint8_t *src, int srcLength, const MsgLangInfo &langInfo);
    int EscapeTurkishLockingToUcs2(const uint8_t *src, int srcLen, const MsgLangInfo &langInfo, uint16_t &result);
    int EscapePortuLockingToUcs2(const uint8_t *src, int srcLen, const MsgLangInfo &langInfo, uint16_t &result);
    int EscapeGsm7bitToUcs2(const uint8_t *src, int srcLen, const MsgLangInfo &langInfo, uint16_t &result);
    uint16_t EscapeToUcs2(const uint8_t srcText, const MsgLangInfo &langInfo);

    void GetTurkishSingleToUcs2(const uint8_t &srcText, uint16_t &result);
    void GetSpanishSingleToUcs2(const uint8_t &srcText, uint16_t &result);
    void GetGsm7BitExtToUcs2(const uint8_t &srcText, uint16_t &result);
    void GetPortuSingleToUcs2(const uint8_t &srcText, uint16_t &result);

    void InitExtCharMap();
    void InitGsm7bitDefMap();
    void InitGsm7bitExtMap();
    void InitTurkishMap();
    void InitSpanishMap();
    void InitPortuMap();
    void InitReplaceCharMap();

private:
    std::map<uint16_t, uint8_t> extCharMap_;
    std::map<uint16_t, uint8_t> gsm7bitDefMap_;
    std::map<uint16_t, uint8_t> gsm7bitExtMap_;
    std::map<uint16_t, uint8_t> turkishMap_;
    std::map<uint16_t, uint8_t> spanishMap_;
    std::map<uint16_t, uint8_t> portuMap_;
    std::map<uint16_t, uint8_t> replaceCharMap_;
};
} // namespace Telephony
} // namespace OHOS
#endif
