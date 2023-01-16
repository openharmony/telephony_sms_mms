/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
 * Copyright (C) 2014 Samsung Electronics Co., Ltd. All rights reserved
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

#ifndef MSG_TEXT_CONVERT_H
#define MSG_TEXT_CONVERT_H

#include <map>
#include <memory>

#include "gsm_pdu_code_type.h"
#include "msg_text_convert_common.h"

namespace OHOS {
namespace Telephony {
#define IN
#define OUT

class MsgTextConvert {
public:
    static MsgTextConvert *Instance();
    int ConvertUTF8ToGSM7bit(std::tuple<unsigned char *, int, unsigned char *, int,
        MSG_LANGUAGE_ID_T *, bool *> &parameters);
    int ConvertUTF8ToUCS2(
        OUT unsigned char *pDestText, IN int maxLength, IN const unsigned char *pSrcText, IN int srcTextLen);
    int ConvertCdmaUTF8ToAuto(OUT unsigned char *pDestText, IN int maxLength, IN const unsigned char *pSrcText,
        IN int srcTextLen, OUT SmsCodingScheme *pCharType);
    int ConvertGsmUTF8ToAuto(OUT unsigned char *pDestText, IN int maxLength, IN const unsigned char *pSrcText,
        IN int srcTextLen, OUT SmsCodingScheme *pCharType);
    int ConvertGSM7bitToUTF8(OUT unsigned char *pDestText, IN int maxLength, IN const unsigned char *pSrcText,
        IN int srcTextLen, const IN MsgLangInfo *pLangInfo);
    int ConvertUCS2ToUTF8(
        OUT unsigned char *pDestText, IN int maxLength, IN const unsigned char *pSrcText, IN int srcTextLen);
    int ConvertEUCKRToUTF8(
        OUT unsigned char *pDestText, IN int maxLength, IN const unsigned char *pSrcText, IN int srcTextLen);
    int ConvertSHIFTJISToUTF8(
        OUT unsigned char *pDestText, IN int maxLength, IN const unsigned char *pSrcText, IN int srcTextLen) const;

    void Base64Encode(const std::string &src, std::string &dest);
    void Base64Decode(const std::string &src, std::string &dest);
    bool GetEncodeString(
        std::string &encodeString, uint32_t charset, uint32_t valLength, const std::string &strEncodeString);

private:
    MsgTextConvert();
    virtual ~MsgTextConvert();
    int ConvertUCS2ToGSM7bit(OUT unsigned char *pDestText, IN int maxLength, IN const unsigned char *pSrcText,
        IN int srcTextLen, OUT MSG_LANGUAGE_ID_T *pLangId, OUT bool *abnormalChar);
    int ConvertUCS2ToGSM7bitAuto(OUT unsigned char *pDestText, IN int maxLength, IN const unsigned char *pSrcText,
        IN int srcTextLen, OUT bool *pUnknown);
    int ConvertUCS2ToASCII(OUT unsigned char *pDestText, IN int maxLength, IN const unsigned char *pSrcText,
        IN int srcTextLen, OUT bool *pUnknown);
    unsigned char GetLangType(const unsigned char *pSrcText, int srcTextLen);
    int FindUCS2toGSM7Ext(unsigned char *pDestText, int maxLength, const unsigned short inText, bool &abnormalChar);
    int FindUCS2toTurkish(unsigned char *pDestText, int maxLength, const unsigned short inText, bool &abnormalChar);
    int FindUCS2toSpanish(unsigned char *pDestText, int maxLength, const unsigned short inText, bool &abnormalChar);
    int FindUCS2toPortu(unsigned char *pDestText, int maxLength, const unsigned short inText, bool &abnormalChar);
    unsigned char FindUCS2ReplaceChar(const unsigned short inText);

    int ConvertGSM7bitToUCS2(OUT unsigned char *pDestText, IN int maxLength, IN const unsigned char *pSrcText,
        IN int srcTextLen, const IN MsgLangInfo *pLangInfo);
    void ConvertDumpTextToHex(const unsigned char *pText, int length);
    int EscapeTurkishLockingToUCS2(
        const unsigned char *pSrcText, int srcLen, const MsgLangInfo &pLangInfo, unsigned short &result);
    int EscapePortuLockingToUCS2(
        const unsigned char *pSrcText, int srcLen, const MsgLangInfo &pLangInfo, unsigned short &result);
    int EscapeGSM7BitToUCS2(
        const unsigned char *pSrcText, int srcLen, const MsgLangInfo &pLangInfo, unsigned short &result);
    unsigned short EscapeToUCS2(const unsigned char srcText, const MsgLangInfo &pLangInfo);

    void GetTurkishSingleToUCS2(const unsigned char &srcText, unsigned short &result);
    void GetSpanishSingleToUCS2(const unsigned char &srcText, unsigned short &result);
    void GetGsm7BitExtToUCS2(const unsigned char &srcText, unsigned short &result);
    void GetPortuSingleToUCS2(const unsigned char &srcText, unsigned short &result);

    void InitExtCharMap();
    void InitUCS2ToGSM7DefMap();
    void InitUCS2ToExtMap();
    void InitUCS2ToTurkishMap();
    void InitUCS2ToSpanishMap();
    void InitUCS2ToPortuMap();
    void InitUCS2ToReplaceCharMap();

private:
    static std::shared_ptr<MsgTextConvert> instance_;
    std::map<unsigned short, unsigned char> extCharMap_;
    std::map<unsigned short, unsigned char> ucs2toGSM7DefMap_;
    std::map<unsigned short, unsigned char> ucs2toGSM7ExtMap_;
    std::map<unsigned short, unsigned char> ucs2toTurkishMap_;
    std::map<unsigned short, unsigned char> ucs2toSpanishMap_;
    std::map<unsigned short, unsigned char> ucs2toPortuMap_;
    std::map<unsigned short, unsigned char> replaceCharMap_;
};
} // namespace Telephony
} // namespace OHOS
#endif