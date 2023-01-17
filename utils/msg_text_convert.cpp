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

#include "msg_text_convert.h"

#include <cstdio>
#include <cstring>
#include <memory>

#include "glib.h"
#include "mms_charset.h"
#include "securec.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
using msg_encode_type_t = unsigned char;
using namespace std;
std::shared_ptr<MsgTextConvert> MsgTextConvert::instance_ = nullptr;
static constexpr uint8_t GSM7_DEFLIST_LEN = 128;

template<typename T>
inline void UniquePtrDeleterOneDimension(T **(&ptr))
{
    if (ptr && *ptr) {
        delete[] *ptr;
        *ptr = nullptr;
    }
}

const WCHAR GSM7_BIT_TO_UC_S2[] = {
    /* @ */
    0x0040, 0x00A3, 0x0024, 0x00A5, 0x00E8, 0x00E9, 0x00F9, 0x00EC, 0x00F2, 0x00C7, 0x000A, 0x00D8, 0x00F8, 0x000D,
    0x00C5, 0x00E5, 0x0394, 0x005F, 0x03A6, 0x0393, 0x039B, 0x03A9, 0x03A0, 0x03A8, 0x03A3, 0x0398, 0x039E, 0x001B,
    0x00C6, 0x00E6, 0x00DF, 0x00C9,
    /* SP */
    0x0020, 0x0021, 0x0022, 0x0023, 0x00A4, 0x0025, 0x0026, 0x0027, 0x0028, 0x0029, 0x002A, 0x002B, 0x002C, 0x002D,
    0x002E, 0x002F,
    /* 0 */
    0x0030, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037, 0x0038, 0x0039, 0x003A, 0x003B, 0x003C, 0x003D,
    0x003E, 0x003F, 0x00A1, 0x0041, 0x0042, 0x0043, 0x0044, 0x0045, 0x0046, 0x0047, 0x0048, 0x0049, 0x004A, 0x004B,
    0x004C, 0x004D, 0x004E, 0x004F,
    /* P */
    0x0050, 0x0051, 0x0052, 0x0053, 0x0054, 0x0055, 0x0056, 0x0057, 0x0058, 0x0059, 0x005A, 0x00C4, 0x00D6, 0x00D1,
    0x00DC, 0x00A7, 0x00BF, 0x0061, 0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067, 0x0068, 0x0069, 0x006A, 0x006B,
    0x006C, 0x006D, 0x006E, 0x006F,
    /* p */
    0x0070, 0x0071, 0x0072, 0x0073, 0x0074, 0x0075, 0x0076, 0x0077, 0x0078, 0x0079, 0x007A, 0x00E4, 0x00F6, 0x00F1,
    0x00FC, 0x00E0
};

MsgTextConvert::MsgTextConvert()
{
    InitExtCharMap();
    InitUCS2ToGSM7DefMap();
    InitUCS2ToExtMap();
    InitUCS2ToTurkishMap();
    InitUCS2ToSpanishMap();
    InitUCS2ToPortuMap();
    InitUCS2ToReplaceCharMap();
}

MsgTextConvert::~MsgTextConvert()
{
    extCharMap_.clear();
    ucs2toGSM7DefMap_.clear();
    ucs2toGSM7ExtMap_.clear();
    ucs2toTurkishMap_.clear();
    ucs2toSpanishMap_.clear();
    ucs2toPortuMap_.clear();
    replaceCharMap_.clear();
}

MsgTextConvert *MsgTextConvert::Instance()
{
    if (!instance_) {
        TELEPHONY_LOGI("pInstance is nullptr. Now creating instance.\r\n");
        struct make_shared_enabler : public MsgTextConvert {};
        instance_ = std::make_shared<make_shared_enabler>();
    }
    return instance_.get();
}

void MsgTextConvert::InitExtCharMap()
{
    extCharMap_.clear();
    extCharMap_ = { { 0x000C, MSG_GSM7EXT_CHAR }, { 0x005B, MSG_GSM7EXT_CHAR }, { 0x005C, MSG_GSM7EXT_CHAR },
        { 0x005D, MSG_GSM7EXT_CHAR }, { 0x005E, MSG_GSM7EXT_CHAR }, { 0x007B, MSG_GSM7EXT_CHAR },
        { 0x007C, MSG_GSM7EXT_CHAR }, { 0x007D, MSG_GSM7EXT_CHAR }, { 0x007E, MSG_GSM7EXT_CHAR },
        { 0x20AC, MSG_GSM7EXT_CHAR }, { 0x00E7, MSG_TURKISH_CHAR }, { 0x011E, MSG_TURKISH_CHAR },
        { 0x011F, MSG_TURKISH_CHAR }, { 0x01E6, MSG_TURKISH_CHAR }, { 0x01E7, MSG_TURKISH_CHAR },
        { 0x0130, MSG_TURKISH_CHAR }, { 0x0131, MSG_TURKISH_CHAR }, { 0x015E, MSG_TURKISH_CHAR },
        { 0x015F, MSG_TURKISH_CHAR }, { 0x00C1, MSG_SPANISH_CHAR }, { 0x00E1, MSG_SPANISH_CHAR },
        { 0x00CD, MSG_SPANISH_CHAR }, { 0x00ED, MSG_SPANISH_CHAR }, { 0x00D3, MSG_SPANISH_CHAR },
        { 0x00F3, MSG_SPANISH_CHAR }, { 0x00DA, MSG_SPANISH_CHAR }, { 0x00FA, MSG_SPANISH_CHAR },
        { 0x00D4, MSG_PORTUGUESE_CHAR }, { 0x00F4, MSG_PORTUGUESE_CHAR }, { 0x00CA, MSG_PORTUGUESE_CHAR },
        { 0x00EA, MSG_PORTUGUESE_CHAR }, { 0x00C0, MSG_PORTUGUESE_CHAR }, { 0x00E7, MSG_PORTUGUESE_CHAR },
        { 0x00C3, MSG_PORTUGUESE_CHAR }, { 0x00E3, MSG_PORTUGUESE_CHAR }, { 0x00D5, MSG_PORTUGUESE_CHAR },
        { 0x00F5, MSG_PORTUGUESE_CHAR }, { 0x00C2, MSG_PORTUGUESE_CHAR }, { 0x00E2, MSG_PORTUGUESE_CHAR } };
}

void MsgTextConvert::InitUCS2ToGSM7DefMap()
{
    ucs2toGSM7DefMap_.clear();
    for (unsigned char i = 0; i < GSM7_DEFLIST_LEN; i++) {
        ucs2toGSM7DefMap_[GSM7_BIT_TO_UC_S2[i]] = i;
    }
}

void MsgTextConvert::InitUCS2ToExtMap()
{
    ucs2toGSM7ExtMap_.clear();
    ucs2toGSM7ExtMap_ = { { 0x005B, 0x3C }, { 0x005D, 0x3E }, { 0x007B, 0x28 }, { 0x007D, 0x29 }, { 0x000C, 0x0A },
        { 0x005C, 0x2F }, { 0x005E, 0x14 }, { 0x007C, 0x40 }, { 0x007E, 0x3D }, { 0x20AC, 0x65 } };
}

void MsgTextConvert::InitUCS2ToTurkishMap()
{
    /* Turkish */
    ucs2toTurkishMap_.clear();
    ucs2toTurkishMap_ = { { 0x005B, 0x3C }, { 0x005D, 0x3E }, { 0x007B, 0x28 }, { 0x007D, 0x29 }, { 0x000C, 0x0A },
        { 0x005C, 0x2F }, { 0x005E, 0x14 }, { 0x007C, 0x40 }, { 0x007E, 0x3D }, { 0x20AC, 0x65 }, { 0x00E7, 0x63 },
        { 0x011E, 0x47 }, { 0x011F, 0x67 }, { 0x01E6, 0x47 }, { 0x01E7, 0x67 }, { 0x0130, 0x49 }, { 0x0131, 0x69 },
        { 0x015E, 0x53 }, { 0x015F, 0x73 } };
}

void MsgTextConvert::InitUCS2ToSpanishMap()
{
    /* Spanish */
    ucs2toSpanishMap_.clear();
    ucs2toSpanishMap_ = { { 0x005B, 0x3C }, { 0x005D, 0x3E }, { 0x007B, 0x28 }, { 0x007D, 0x29 }, { 0x000C, 0x0A },
        { 0x005C, 0x2F }, { 0x005E, 0x14 }, { 0x007C, 0x40 }, { 0x007E, 0x3D }, { 0x20AC, 0x65 }, { 0x00C1, 0x41 },
        { 0x00E1, 0x61 }, { 0x00CD, 0x49 }, { 0x00ED, 0x69 }, { 0x00D3, 0x4F }, { 0x00F3, 0x6F }, { 0x00DA, 0x55 },
        { 0x00FA, 0x75 } };
}

void MsgTextConvert::InitUCS2ToPortuMap()
{
    /* Portuguese */
    ucs2toPortuMap_.clear();
    ucs2toPortuMap_ = { { 0x005B, 0x3C }, { 0x005D, 0x3E }, { 0x007B, 0x28 }, { 0x007D, 0x29 }, { 0x000C, 0x0A },
        { 0x005C, 0x2F }, { 0x005E, 0x14 }, { 0x007C, 0x40 }, { 0x007E, 0x3D }, { 0x20AC, 0x65 }, { 0x00D4, 0x0B },
        { 0x00F4, 0x0C }, { 0x00C1, 0x0E }, { 0x00E1, 0x0F }, { 0x00CA, 0x1F }, { 0x00EA, 0x05 }, { 0x00C0, 0x41 },
        { 0x00E7, 0x09 }, { 0x00CD, 0x49 }, { 0x00ED, 0x69 }, { 0x00D3, 0x4F }, { 0x00F3, 0x6F }, { 0x00DA, 0x55 },
        { 0x00FA, 0x75 }, { 0x00C3, 0x61 }, { 0x00E3, 0x7B }, { 0x00D5, 0x5C }, { 0x00F5, 0x7C }, { 0x00C2, 0x61 },
        { 0x00E2, 0x7F }, { 0x03A6, 0x12 }, { 0x0393, 0x13 }, { 0x03A9, 0x15 }, { 0x03A0, 0x16 }, { 0x03A8, 0x17 },
        { 0x03A3, 0x18 }, { 0x0398, 0x19 } };
}

void MsgTextConvert::InitUCS2ToReplaceCharMap()
{
    /* character replacement table */
    replaceCharMap_.clear();
    replaceCharMap_ = { { 0x00E0, 0x61 }, { 0x00E1, 0x61 }, { 0x00E2, 0x61 }, { 0x00E3, 0x61 }, { 0x00E4, 0x61 },
        { 0x00E5, 0x61 }, { 0x00E6, 0x61 }, { 0x0101, 0x61 }, { 0x0103, 0x61 }, { 0x0105, 0x61 }, { 0x01CE, 0x61 },
        { 0x00C0, 0x41 }, { 0x00C1, 0x41 }, { 0x00C2, 0x41 }, { 0x00C3, 0x41 }, { 0x00C4, 0x41 }, { 0x00C5, 0x41 },
        { 0x00C6, 0x41 }, { 0x0100, 0x41 }, { 0x0102, 0x41 }, { 0x0104, 0x41 }, { 0x01CD, 0x41 }, { 0x00E7, 0x63 },
        { 0x0107, 0x63 }, { 0x0109, 0x63 }, { 0x010B, 0x63 }, { 0x010D, 0x63 }, { 0x00C7, 0x43 }, { 0x0106, 0x43 },
        { 0x0108, 0x43 }, { 0x010A, 0x43 }, { 0x010C, 0x43 }, { 0x010F, 0x64 }, { 0x0111, 0x64 }, { 0x010E, 0x44 },
        { 0x0110, 0x44 }, { 0x00E8, 0x65 }, { 0x00E9, 0x65 }, { 0x00EA, 0x65 }, { 0x00EB, 0x65 }, { 0x0113, 0x65 },
        { 0x0115, 0x65 }, { 0x0117, 0x65 }, { 0x0119, 0x65 }, { 0x011B, 0x65 }, { 0x0259, 0x65 }, { 0x00C8, 0x45 },
        { 0x00C9, 0x45 }, { 0x00CA, 0x45 }, { 0x00CB, 0x45 }, { 0x0112, 0x45 }, { 0x0114, 0x45 }, { 0x0116, 0x45 },
        { 0x0118, 0x45 }, { 0x011A, 0x45 }, { 0x018F, 0x45 }, { 0x011D, 0x67 }, { 0x011F, 0x67 }, { 0x0121, 0x67 },
        { 0x0123, 0x67 }, { 0x01E7, 0x67 }, { 0x01F5, 0x67 }, { 0x1E21, 0x67 }, { 0x011C, 0x47 }, { 0x011E, 0x47 },
        { 0x0120, 0x47 }, { 0x0122, 0x47 }, { 0x01E6, 0x47 }, { 0x01F4, 0x47 }, { 0x1E20, 0x47 }, { 0x00EC, 0x69 },
        { 0x00ED, 0x69 }, { 0x00EE, 0x69 }, { 0x00EF, 0x69 }, { 0x0129, 0x69 }, { 0x012B, 0x69 }, { 0x012D, 0x69 },
        { 0x012F, 0x69 }, { 0x01D0, 0x69 }, { 0x0131, 0x69 }, { 0x00CC, 0x49 }, { 0x00CD, 0x49 }, { 0x00CE, 0x49 },
        { 0x00CF, 0x49 }, { 0x0128, 0x49 }, { 0x012A, 0x49 }, { 0x012C, 0x49 }, { 0x012E, 0x49 }, { 0x0130, 0x49 },
        { 0x0137, 0x6B }, { 0x0136, 0x4B }, { 0x013A, 0x6C }, { 0x013C, 0x6C }, { 0x013E, 0x6C }, { 0x0140, 0x6C },
        { 0x0142, 0x6C }, { 0x0139, 0x4C }, { 0x013B, 0x4C }, { 0x013D, 0x4C }, { 0x013F, 0x4C }, { 0x0141, 0x4C },
        { 0x00F1, 0x6E }, { 0x0144, 0x6E }, { 0x0146, 0x6E }, { 0x0148, 0x6E }, { 0x00D1, 0x4E }, { 0x0143, 0x4E },
        { 0x0145, 0x4E }, { 0x0147, 0x4E }, { 0x00F2, 0x6F }, { 0x00F3, 0x6F }, { 0x00F4, 0x6F }, { 0x00F5, 0x6F },
        { 0x00F6, 0x6F }, { 0x00F8, 0x6F }, { 0x014D, 0x6F }, { 0x014F, 0x6F }, { 0x01D2, 0x6F }, { 0x01EB, 0x6F },
        { 0x0151, 0x6F }, { 0x0153, 0x6F }, { 0x00D2, 0x4F }, { 0x00D3, 0x4F }, { 0x00D4, 0x4F }, { 0x00D5, 0x4F },
        { 0x00D6, 0x4F }, { 0x00D8, 0x4F }, { 0x014C, 0x4F }, { 0x014E, 0x4F }, { 0x01D1, 0x4F }, { 0x01EA, 0x4F },
        { 0x0150, 0x4F }, { 0x0152, 0x4F }, { 0x0155, 0x72 }, { 0x0157, 0x72 }, { 0x0159, 0x72 }, { 0x0154, 0x52 },
        { 0x0156, 0x52 }, { 0x0158, 0x52 }, { 0x015B, 0x73 }, { 0x015D, 0x73 }, { 0x015F, 0x73 }, { 0x0161, 0x73 },
        { 0x015A, 0x53 }, { 0x015C, 0x53 }, { 0x015E, 0x53 }, { 0x0160, 0x53 }, { 0x00FE, 0x74 }, { 0x0163, 0x74 },
        { 0x0165, 0x74 }, { 0x0167, 0x74 }, { 0x021B, 0x74 }, { 0x00DE, 0x54 }, { 0x0162, 0x54 }, { 0x0164, 0x54 },
        { 0x0166, 0x54 }, { 0x00F9, 0x75 }, { 0x00FA, 0x75 }, { 0x00FB, 0x75 }, { 0x00FC, 0x75 }, { 0x0169, 0x75 },
        { 0x016B, 0x75 }, { 0x016D, 0x75 }, { 0x016F, 0x75 }, { 0x0171, 0x75 }, { 0x0173, 0x75 }, { 0x01D4, 0x75 },
        { 0x00D9, 0x55 }, { 0x00DA, 0x55 }, { 0x00DB, 0x55 }, { 0x00DC, 0x55 }, { 0x0168, 0x55 }, { 0x016A, 0x55 },
        { 0x016C, 0x55 }, { 0x016E, 0x55 }, { 0x0170, 0x55 }, { 0x0172, 0x55 }, { 0x01D3, 0x55 }, { 0x00FD, 0x79 },
        { 0x00FF, 0x79 }, { 0x0177, 0x79 }, { 0x0233, 0x79 }, { 0x1EF3, 0x79 }, { 0x1EF9, 0x79 }, { 0x00DD, 0x59 },
        { 0x0176, 0x59 }, { 0x0178, 0x59 }, { 0x0232, 0x59 }, { 0x1EF2, 0x59 }, { 0x1EF8, 0x59 }, { 0x017A, 0x7A },
        { 0x017C, 0x7A }, { 0x017E, 0x7A }, { 0x0179, 0x5A }, { 0x017B, 0x5A }, { 0x017D, 0x5A } };
}

/**
 * @brief ConvertUTF8ToGSM7bit
 * max # of UCS2 chars, NOT bytes. when all utf8 chars are only one byte,
 * UCS2Length is maxUCS2 Length. otherwise (ex: 2 bytes of UTF8 is one char)
 * UCS2Length must be  less than utf8Length
 */
int MsgTextConvert::ConvertUTF8ToGSM7bit(std::tuple<unsigned char *, int,
    unsigned char *, int, MSG_LANGUAGE_ID_T *, bool *> &parameters)
{
    unsigned char *pDestText;
    int maxLength;
    unsigned char *pSrcText;
    int srcTextLen;
    MSG_LANGUAGE_ID_T *pLangId;
    bool *bIncludeAbnormalChar;
    std::tie(pDestText, maxLength, pSrcText, srcTextLen, pLangId, bIncludeAbnormalChar) = parameters;
    int utf8Length = 0;
    int gsm7bitLength = 0;
    int ucs2Length = 0;
    if (srcTextLen <= 0 && pSrcText) {
        utf8Length = strlen(reinterpret_cast<const char *>(pSrcText));
        srcTextLen = utf8Length;
    } else {
        utf8Length = srcTextLen;
    }

    int maxUCS2Length = utf8Length;
    WCHAR *pUCS2Text = nullptr;
    unique_ptr<WCHAR *, void (*)(WCHAR * *(&))> buf(&pUCS2Text, UniquePtrDeleterOneDimension);
    pUCS2Text = reinterpret_cast<WCHAR *>(new (std::nothrow) char[maxUCS2Length * sizeof(WCHAR)]);
    if (pUCS2Text == nullptr) {
        return gsm7bitLength;
    }
    (void)memset_s(pUCS2Text, maxUCS2Length * sizeof(WCHAR), 0x00, maxUCS2Length * sizeof(WCHAR));
    TELEPHONY_LOGI("srcTextLen = %{public}d", srcTextLen);

    ucs2Length = ConvertUTF8ToUCS2(
        reinterpret_cast<unsigned char *>(pUCS2Text), maxUCS2Length * sizeof(WCHAR), pSrcText, srcTextLen);
    gsm7bitLength = ConvertUCS2ToGSM7bit(pDestText, maxLength, reinterpret_cast<unsigned char *>(pUCS2Text),
        ucs2Length, pLangId, bIncludeAbnormalChar);
    return gsm7bitLength;
}

int MsgTextConvert::ConvertUTF8ToUCS2(
    OUT unsigned char *pDestText, IN int maxLength, IN const unsigned char *pSrcText, IN int srcTextLen)
{
    gsize textLen;
    auto unicodeTemp = reinterpret_cast<unsigned char *>(pDestText);
    gsize ucs2Length = 0;
    gsize remainedBuffer = maxLength;
#ifdef CONVERT_DUMP
    int srcLen = srcTextLen;
    const unsigned char *pTempSrcText = pSrcText;
    const unsigned char *pTempDestText = pDestText;
#endif
    if (maxLength == 0 || pSrcText == nullptr || pDestText == nullptr) {
        TELEPHONY_LOGE("UTF8 to UCS2 Failed as text length is 0");
        return -1;
    }
    /* nullptr terminated string */
    if (srcTextLen == -1) {
        textLen = strlen(reinterpret_cast<char *>(const_cast<unsigned char *>(pSrcText)));
    } else {
        textLen = srcTextLen;
    }

    GIConv cd;
    int err = 0;
    cd = g_iconv_open("UTF16BE", "UTF8");
    if (cd != nullptr) {
        err = g_iconv(cd, reinterpret_cast<char **>(const_cast<unsigned char **>(&pSrcText)),
            reinterpret_cast<gsize *>(&textLen), reinterpret_cast<char **>(&unicodeTemp),
            reinterpret_cast<gsize *>(&remainedBuffer));
    }
    ucs2Length = (err < 0) ? -1 : (maxLength - remainedBuffer);
#ifdef CONVERT_DUMP
    TELEPHONY_LOGI("########## Dump UTF8 -> UCS2");
    ConvertDumpTextToHex((unsigned char *)pTempSrcText, srcLen);
    ConvertDumpTextToHex((unsigned char *)pTempDestText, ucs2Length);
#endif
    g_iconv_close(cd);
    return ucs2Length;
}

int MsgTextConvert::ConvertGsmUTF8ToAuto(OUT unsigned char *pDestText, IN int maxLength,
    IN const unsigned char *pSrcText, IN int srcTextLen, OUT SmsCodingScheme *pCharType)
{
    int ret = 0;
    int tempTextLen = 0;
    int utf8Length = 0;
    int gsm7bitLength = 0;
    int ucs2Length = 0;
    bool bUnknown = false;
    utf8Length = srcTextLen;

    int maxUCS2Length = utf8Length;
    WCHAR pUCS2Text[maxUCS2Length];
    ret = memset_s(pUCS2Text, sizeof(pUCS2Text), 0x00, maxUCS2Length * sizeof(WCHAR));
    if (ret != EOK) {
        TELEPHONY_LOGE("memset_s err = %{public}d", ret);
    }
    ucs2Length = ConvertUTF8ToUCS2(
        reinterpret_cast<unsigned char *>(pUCS2Text), maxUCS2Length * sizeof(WCHAR), pSrcText, srcTextLen);
    if (ucs2Length < 0) {
        *pCharType = SMS_CODING_8BIT;
        tempTextLen = (srcTextLen > maxLength) ? maxLength : srcTextLen;
        ret = memcpy_s(pDestText, tempTextLen, pSrcText, tempTextLen);
        if (ret != EOK) {
            TELEPHONY_LOGE("memcpy_s err = %{public}d", ret);
        }
        return tempTextLen;
    } else {
        gsm7bitLength = ConvertUCS2ToGSM7bitAuto(
            pDestText, maxLength, reinterpret_cast<unsigned char *>(pUCS2Text), ucs2Length, &bUnknown);
        if (bUnknown) {
            *pCharType = SMS_CODING_UCS2;
            if (ucs2Length <= 0) {
                return gsm7bitLength;
            }
            tempTextLen = (ucs2Length > maxLength) ? maxLength : ucs2Length;
            ret = memcpy_s(pDestText, tempTextLen, pUCS2Text, tempTextLen);
            if (ret != EOK) {
                TELEPHONY_LOGE("memcpy_s err = %{public}d", ret);
            }
            return tempTextLen;
        } else {
            *pCharType = SMS_CODING_7BIT;
        }
        return gsm7bitLength;
    }
}

int MsgTextConvert::ConvertCdmaUTF8ToAuto(OUT unsigned char *pDestText, IN int maxLength,
    IN const unsigned char *pSrcText, IN int srcTextLen, OUT SmsCodingScheme *pCharType)
{
    int ret = 0;
    int tempTextLen = 0;
    int utf8Length = 0;
    int gsm7bitLength = 0;
    int ucs2Length = 0;
    bool bUnknown = false;
    utf8Length = srcTextLen;

    int maxUCS2Length = utf8Length;
    WCHAR pUCS2Text[maxUCS2Length];
    ret = memset_s(pUCS2Text, sizeof(pUCS2Text), 0x00, maxUCS2Length * sizeof(WCHAR));
    if (ret != EOK) {
        TELEPHONY_LOGE("memset_s err = %{public}d", ret);
    }
    ucs2Length = ConvertUTF8ToUCS2(
        reinterpret_cast<unsigned char *>(pUCS2Text), maxUCS2Length * sizeof(WCHAR), pSrcText, srcTextLen);
    if (ucs2Length < 0) {
        *pCharType = SMS_CODING_8BIT;
        tempTextLen = (srcTextLen > maxLength) ? maxLength : srcTextLen;
        ret = memcpy_s(pDestText, tempTextLen, pSrcText, tempTextLen);
        if (ret != EOK) {
            TELEPHONY_LOGE("memcpy_s err = %{public}d", ret);
        }
        return tempTextLen;
    } else {
        gsm7bitLength = ConvertUCS2ToASCII(
            pDestText, maxLength, reinterpret_cast<unsigned char *>(pUCS2Text), ucs2Length, &bUnknown);
        if (bUnknown) {
            *pCharType = SMS_CODING_UCS2;
            if (ucs2Length <= 0) {
                return gsm7bitLength;
            }
            tempTextLen = (ucs2Length > maxLength) ? maxLength : ucs2Length;
            ret = memcpy_s(pDestText, tempTextLen, pUCS2Text, tempTextLen);
            if (ret != EOK) {
                TELEPHONY_LOGE("memcpy_s err = %{public}d", ret);
            }
            return tempTextLen;
        } else {
            *pCharType = SMS_CODING_ASCII7BIT;
        }
        return gsm7bitLength;
    }
}

/* max # of UCS2 chars, NOT bytes. when all gsm7 chars are only one byte(-there
   is no extension), UCS2Length is maxUCS2 Length. otherwise(ex: gsm7 char
   starts with 0x1b) UCS2Length must be less than gsm7 length */
int MsgTextConvert::ConvertGSM7bitToUTF8(OUT unsigned char *pDestText, IN int maxLength,
    IN const unsigned char *pSrcText, IN int srcTextLen, const IN MsgLangInfo *pLangInfo)
{
    int utf8Length = 0;
    int ucs2Length = 0;
    int maxUCS2Length = srcTextLen;

    WCHAR pUCS2Text[maxUCS2Length];
    if (memset_s(pUCS2Text, sizeof(pUCS2Text), 0x00, sizeof(pUCS2Text)) != EOK) {
        TELEPHONY_LOGE("ConvertGSM7bitToUTF8 memset_sfail");
        return utf8Length;
    }
    TELEPHONY_LOGI("srcTextLen = %{public}d", srcTextLen);
    TELEPHONY_LOGI("max dest Length = %{public}d", maxLength);
    ucs2Length = ConvertGSM7bitToUCS2(reinterpret_cast<unsigned char *>(pUCS2Text), maxUCS2Length * sizeof(WCHAR),
        pSrcText, srcTextLen, pLangInfo);
    utf8Length = ConvertUCS2ToUTF8(pDestText, maxLength, reinterpret_cast<unsigned char *>(pUCS2Text), ucs2Length);
    return utf8Length;
}

int MsgTextConvert::ConvertUCS2ToUTF8(
    OUT unsigned char *pDestText, IN int maxLength, IN const unsigned char *pSrcText, IN int srcTextLen)
{
    gsize textLen;
    gsize remainedBuffer = maxLength;
    int utf8Length;

#ifdef CONVERT_DUMP
    int srcLen = srcTextLen;
    const unsigned char *pTempSrcText = pSrcText;
#endif
    unsigned char *pTempDestText = pDestText;
    if (srcTextLen == 0 || pSrcText == nullptr || pDestText == nullptr || maxLength == 0) {
        TELEPHONY_LOGE("UCS2 to UTF8 Failed as text length is 0");
        return false;
    }

    if (srcTextLen == -1) {
        textLen = strlen(reinterpret_cast<char *>(const_cast<unsigned char *>(pSrcText)));
    } else {
        textLen = srcTextLen;
    }

    GIConv cd;
    int err = 0;
    cd = g_iconv_open("UTF8", "UTF16BE");
    if (cd != nullptr) {
        err = g_iconv(cd, reinterpret_cast<char **>(const_cast<unsigned char **>(&pSrcText)),
            reinterpret_cast<gsize *>(&textLen), reinterpret_cast<char **>(&pDestText),
            reinterpret_cast<gsize *>(&remainedBuffer));
    }
    if (err != 0) {
        TELEPHONY_LOGI("g_iconv() return value = %{public}d", err);
    }
    utf8Length = maxLength - remainedBuffer;
    pTempDestText[utf8Length] = 0x00;
#ifdef CONVERT_DUMP
    TELEPHONY_LOGI("\n########## Dump UCS2 -> UTF8");
    ConvertDumpTextToHex((unsigned char *)pTempSrcText, srcLen);
    ConvertDumpTextToHex((unsigned char *)pTempDestText, utf8Length);
#endif
    g_iconv_close(cd);
    return utf8Length;
}

int MsgTextConvert::ConvertEUCKRToUTF8(
    OUT unsigned char *pDestText, IN int maxLength, IN const unsigned char *pSrcText, IN int srcTextLen)
{
    gsize textLen;
    gsize remainedBuffer = maxLength;
    int utf8Length;

#ifdef CONVERT_DUMP
    int srcLen = srcTextLen;
    const unsigned char *pTempSrcText = pSrcText;
#endif
    unsigned char *pTempDestText = pDestText;
    if (srcTextLen == 0 || pSrcText == nullptr || pDestText == nullptr || maxLength == 0) {
        TELEPHONY_LOGE("EUCKR to UTF8 Failed as text length is 0");
        return false;
    }
    if (srcTextLen == -1) {
        textLen = strlen(reinterpret_cast<char *>(const_cast<unsigned char *>(pSrcText)));
    } else {
        textLen = srcTextLen;
    }

    GIConv cd;
    int err = 0;
    cd = g_iconv_open("UTF8", "EUCKR");
    if (cd != nullptr) {
        err = g_iconv(cd, reinterpret_cast<char **>(const_cast<unsigned char **>(&pSrcText)),
            reinterpret_cast<gsize *>(&textLen), reinterpret_cast<char **>(&pDestText),
            reinterpret_cast<gsize *>(&remainedBuffer));
    }
    if (err != 0) {
        TELEPHONY_LOGI("g_iconv() return value = %{public}d", err);
    }
    utf8Length = maxLength - remainedBuffer;
    pTempDestText[utf8Length] = 0x00;
#ifdef CONVERT_DUMP
    TELEPHONY_LOGI("\n########## Dump EUCKR -> UTF8\n");
    ConvertDumpTextToHex((unsigned char *)pTempSrcText, srcLen);
    ConvertDumpTextToHex((unsigned char *)pTempDestText, utf8Length);
#endif
    g_iconv_close(cd);
    return utf8Length;
}

int MsgTextConvert::ConvertSHIFTJISToUTF8(
    OUT unsigned char *pDestText, IN int maxLength, IN const unsigned char *pSrcText, IN int srcTextLen) const
{
    gsize textLen;
    gsize remainedBuffer = maxLength;
    int utf8Length;

#ifdef CONVERT_DUMP
    int srcLen = srcTextLen;
    const unsigned char *pTempSrcText = pSrcText;
#endif
    unsigned char *pTempDestText = pDestText;
    if (srcTextLen == 0 || pSrcText == nullptr || pDestText == nullptr || maxLength == 0) {
        TELEPHONY_LOGE("EUCKR to UTF8 Failed as text length is 0");
        return false;
    }
    if (srcTextLen == -1) {
        textLen = strlen(reinterpret_cast<char *>(const_cast<unsigned char *>(pSrcText)));
    } else {
        textLen = srcTextLen;
    }

    GIConv cd;
    int err = 0;
    cd = g_iconv_open("UTF8", "SHIFT-JIS");
    if (cd != nullptr) {
        err = g_iconv(cd, reinterpret_cast<char **>(const_cast<unsigned char **>(&pSrcText)),
            reinterpret_cast<gsize *>(&textLen), reinterpret_cast<char **>(&pDestText),
            reinterpret_cast<gsize *>(&remainedBuffer));
    }

    TELEPHONY_LOGI("g_iconv() return value = %{public}d", err);
    utf8Length = maxLength - remainedBuffer;
    pTempDestText[utf8Length] = 0x00;
#ifdef CONVERT_DUMP
    TELEPHONY_LOGI("\n########## Dump SHIFT-JIS -> UTF8");
    ConvertDumpTextToHex((unsigned char *)pTempSrcText, srcLen);
    ConvertDumpTextToHex((unsigned char *)pTempDestText, utf8Length);
#endif
    g_iconv_close(cd);
    return utf8Length;
}

int MsgTextConvert::ConvertUCS2ToGSM7bit(OUT unsigned char *pDestText, IN int maxLength,
    IN const unsigned char *pSrcText, IN int srcTextLen, OUT MSG_LANGUAGE_ID_T *pLangId, OUT bool *abnormalChar)
{
    /* for UNICODE */
    int outTextLen = 0;
    int remainLen = 0;
    if (srcTextLen == 0 || pSrcText == nullptr || pDestText == nullptr || maxLength <= 0) {
        TELEPHONY_LOGE("Invalid parameter.");
        return -1;
    }
    unsigned short inText;
    unsigned char lowerByte;
    unsigned char upperByte;
    unsigned char currType = GetLangType(pSrcText, srcTextLen);
    std::map<unsigned short, unsigned char>::iterator itChar;
    for (int index = 0; index < (srcTextLen - 1); index++) {
        upperByte = pSrcText[index++];
        lowerByte = pSrcText[index];
        inText = ((upperByte << 0x08) & 0xFF00) | lowerByte;
        itChar = ucs2toGSM7DefMap_.find(inText); /* Check Default Char */
        if (itChar != ucs2toGSM7DefMap_.end()) {
            pDestText[outTextLen++] = static_cast<unsigned char>(itChar->second);
        } else {
            switch (currType) {
                case MSG_GSM7EXT_CHAR:
                    remainLen = maxLength - outTextLen;
                    outTextLen += FindUCS2toGSM7Ext(&pDestText[outTextLen], remainLen, inText, *abnormalChar);
                    outTextLen++;
                    break;
                case MSG_TURKISH_CHAR:
                    *pLangId = MSG_ID_TURKISH_LANG;
                    remainLen = maxLength - outTextLen;
                    outTextLen += FindUCS2toTurkish(&pDestText[outTextLen], remainLen, inText, *abnormalChar);
                    outTextLen++;
                    break;
                case MSG_SPANISH_CHAR:
                    *pLangId = MSG_ID_SPANISH_LANG;
                    remainLen = maxLength - outTextLen;
                    outTextLen += FindUCS2toSpanish(&pDestText[outTextLen], remainLen, inText, *abnormalChar);
                    outTextLen++;
                    break;
                case MSG_PORTUGUESE_CHAR:
                    *pLangId = MSG_ID_PORTUGUESE_LANG;
                    remainLen = maxLength - outTextLen;
                    outTextLen += FindUCS2toPortu(&pDestText[outTextLen], remainLen, inText, *abnormalChar);
                    outTextLen++;
                    break;
                default:
                    pDestText[outTextLen++] = FindUCS2ReplaceChar(inText);
                    *abnormalChar = true;
                    break;
            }
        }
        /* prevent buffer overflow */
        if (maxLength <= outTextLen) {
            TELEPHONY_LOGE("Buffer full.");
            break;
        }
    }
    return outTextLen;
}

int MsgTextConvert::ConvertUCS2ToGSM7bitAuto(OUT unsigned char *pDestText, IN int maxLength,
    IN const unsigned char *pSrcText, IN int srcTextLen, OUT bool *pUnknown)
{
    /* for UNICODE */
    int outTextLen = 0;
    unsigned char lowerByte;
    unsigned char upperByte;
    if (srcTextLen == 0 || pSrcText == nullptr || pDestText == nullptr || maxLength == 0) {
        TELEPHONY_LOGE("UCS2 to GSM7bit Failed as text length is 0");
        return -1;
    }
    std::map<unsigned short, unsigned char>::iterator itChar;
    std::map<unsigned short, unsigned char>::iterator itExt;
    unsigned short inText;
    for (int index = 0; index < srcTextLen; index++) {
        upperByte = pSrcText[index++];
        lowerByte = pSrcText[index];
        inText = (upperByte << 0x08) & 0xFF00;
        inText = inText | lowerByte;
        /* Check Default Char */
        itChar = ucs2toGSM7DefMap_.find(inText);
        if (itChar != ucs2toGSM7DefMap_.end()) {
            pDestText[outTextLen++] = static_cast<unsigned char>(itChar->second);
        } else {
            itExt = ucs2toGSM7ExtMap_.find(inText);
            if (itExt == ucs2toGSM7ExtMap_.end()) {
                TELEPHONY_LOGI("Abnormal character is included. inText : [%{public}04x]", inText);
                *pUnknown = true;
                return 0;
            } else if (maxLength <= outTextLen + 1) {
                TELEPHONY_LOGE("Buffer Full.");
                break;
            } else {
                pDestText[outTextLen++] = 0x1B;
                pDestText[outTextLen++] = static_cast<unsigned char>(itExt->second);
            }
        }
        /* prevent buffer overflow */
        if (maxLength <= outTextLen) {
            TELEPHONY_LOGE("Buffer full");
            break;
        }
    }
#ifdef CONVERT_DUMP
    TELEPHONY_LOGI("\n########## Dump UCS2 -> GSM7bit");
    ConvertDumpTextToHex((unsigned char *)pSrcText, srcTextLen);
    ConvertDumpTextToHex((unsigned char *)pDestText, outTextLen);
#endif
    return outTextLen;
}

int MsgTextConvert::ConvertUCS2ToASCII(OUT unsigned char *pDestText, IN int maxLength,
    IN const unsigned char *pSrcText, IN int srcTextLen, OUT bool *pUnknown)
{
    /* for UNICODE */
    int outTextLen = 0;
    unsigned char lowerByte;
    unsigned char upperByte;
    if (srcTextLen == 0 || pSrcText == nullptr || pDestText == nullptr || maxLength == 0) {
        TELEPHONY_LOGI("UCS2 to GSM7bit Failed as text length is 0");
        return -1;
    }
    std::map<unsigned short, unsigned char>::iterator itChar;
    std::map<unsigned short, unsigned char>::iterator itExt;
    unsigned short inText;
    for (int index = 0; index < srcTextLen; index++) {
        upperByte = pSrcText[index++];
        lowerByte = pSrcText[index];
        inText = (upperByte << 0x08) & 0xFF00;
        inText = inText | lowerByte;
        /* Check Default Char */
        if (inText > 0x007f) {
            TELEPHONY_LOGI("Abnormal character is included. inText : [%{public}04x]", inText);
            *pUnknown = true;
            return 0;
        }
        pDestText[outTextLen++] = static_cast<unsigned char>(inText);
        /* prevent buffer overflow */
        if (maxLength <= outTextLen) {
            TELEPHONY_LOGI("Buffer full\n");
            break;
        }
    }

#ifdef CONVERT_DUMP
    TELEPHONY_LOGI("\n########## Dump UCS2 -> GSM7bit");
    ConvertDumpTextToHex((unsigned char *)pSrcText, srcTextLen);
    ConvertDumpTextToHex((unsigned char *)pDestText, outTextLen);
#endif
    return outTextLen;
}

unsigned char MsgTextConvert::GetLangType(const unsigned char *pSrcText, int srcTextLen)
{
    /* for UNICODE */
    if (srcTextLen <= 0 || pSrcText == nullptr) {
        TELEPHONY_LOGE("Invalid parameter.");
        return MSG_DEFAULT_CHAR;
    }

    unsigned char lowerByte;
    unsigned char upperByte;
    std::map<unsigned short, unsigned char>::iterator itExt;
    unsigned char currType = MSG_DEFAULT_CHAR;
    unsigned char newType = MSG_DEFAULT_CHAR;
    unsigned short inText;
    /* Get Language Type by checking each character */
    for (int index = 0; index < (srcTextLen - 1); index++) {
        upperByte = pSrcText[index++];
        lowerByte = pSrcText[index];
        inText = (upperByte << 0x08) & 0xFF00;
        inText = inText | lowerByte;
        itExt = extCharMap_.find(inText);
        if (itExt == extCharMap_.end()) {
            continue;
        }
        newType = static_cast<unsigned char>(itExt->second);
        if (newType >= currType) {
            bool isTurkisk = (inText == 0x00e7 && currType <= MSG_TURKISH_CHAR);
            currType = isTurkisk ? MSG_TURKISH_CHAR : newType;
        }
    }
    TELEPHONY_LOGI("charType : [%{public}hhu]", currType);
    return currType;
}

int MsgTextConvert::FindUCS2toGSM7Ext(
    unsigned char *pDestText, int maxLength, const unsigned short inText, bool &abnormalChar)
{
    int outTextLen = 0;
    if (pDestText == nullptr || maxLength <= 0) {
        TELEPHONY_LOGE("Invalid parameter.");
        return outTextLen;
    }

    auto itExt = ucs2toGSM7ExtMap_.find(inText);
    if (itExt == ucs2toGSM7ExtMap_.end()) {
        pDestText[outTextLen++] = FindUCS2ReplaceChar(inText);
        abnormalChar = true;
#ifdef CONVERT_DUMP
        TELEPHONY_LOGI("Abnormal character is included. inText : [%{public}04x]", inText);
#endif
        return outTextLen;
    }
    /* prevent buffer overflow */
    if (maxLength <= outTextLen + 1) {
        TELEPHONY_LOGE("Buffer Full.");
        return outTextLen;
    }
    pDestText[outTextLen++] = 0x1B;
    pDestText[outTextLen++] = static_cast<unsigned char>(itExt->second);
    return outTextLen;
}

int MsgTextConvert::FindUCS2toTurkish(
    unsigned char *pDestText, int maxLength, const unsigned short inText, bool &abnormalChar)
{
    int outTextLen = 0;
    if (pDestText == nullptr || maxLength <= 0) {
        TELEPHONY_LOGE("Invalid parameter.");
        return outTextLen;
    }

    auto itExt = ucs2toTurkishMap_.find(inText);
    if (itExt == ucs2toTurkishMap_.end()) {
        pDestText[outTextLen++] = FindUCS2ReplaceChar(inText);
        abnormalChar = true;
#ifdef CONVERT_DUMP
        TELEPHONY_LOGI("Abnormal character is included. inText : [%{public}04x]", inText);
#endif
        return outTextLen;
    }
    /* prevent buffer overflow */
    if (maxLength <= outTextLen + 1) {
        TELEPHONY_LOGE("Buffer Full.");
        return outTextLen;
    }
    pDestText[outTextLen++] = 0x1B;
    pDestText[outTextLen++] = static_cast<unsigned char>(itExt->second);
    return outTextLen;
}

int MsgTextConvert::FindUCS2toSpanish(
    unsigned char *pDestText, int maxLength, const unsigned short inText, bool &abnormalChar)
{
    int outTextLen = 0;
    if (pDestText == nullptr || maxLength <= 0) {
        TELEPHONY_LOGE("Invalid parameter.");
        return outTextLen;
    }

    auto itExt = ucs2toSpanishMap_.find(inText);
    if (itExt == ucs2toSpanishMap_.end()) {
        pDestText[outTextLen++] = FindUCS2ReplaceChar(inText);
        abnormalChar = true;
#ifdef CONVERT_DUMP
        TELEPHONY_LOGI("Abnormal character is included. inText : [%{public}04x]", inText);
#endif
        return outTextLen;
    }
    /* prevent buffer overflow */
    if (maxLength <= outTextLen + 1) {
        TELEPHONY_LOGE("Buffer Full.");
        return outTextLen;
    }
    pDestText[outTextLen++] = 0x1B;
    pDestText[outTextLen++] = static_cast<unsigned char>(itExt->second);
    return outTextLen;
}

int MsgTextConvert::FindUCS2toPortu(
    unsigned char *pDestText, int maxLength, const unsigned short inText, bool &abnormalChar)
{
    int outTextLen = 0;
    if (pDestText == nullptr || maxLength <= 0) {
        TELEPHONY_LOGE("Invalid parameter.");
        return outTextLen;
    }

    auto itExt = ucs2toPortuMap_.find(inText);
    if (itExt == ucs2toPortuMap_.end()) {
        pDestText[outTextLen++] = FindUCS2ReplaceChar(inText);
        abnormalChar = true;
#ifdef CONVERT_DUMP
        TELEPHONY_LOGI("Abnormal character is included. inText : [%{public}04x]", inText);
#endif
        return outTextLen;
    }
    /* prevent buffer overflow */
    if (maxLength <= outTextLen + 1) {
        TELEPHONY_LOGE("Buffer Full.");
        return outTextLen;
    }
    pDestText[outTextLen++] = 0x1B;
    pDestText[outTextLen++] = static_cast<unsigned char>(itExt->second);
    return outTextLen;
}

unsigned char MsgTextConvert::FindUCS2ReplaceChar(const unsigned short inText)
{
    unsigned char result = 0;
    auto itReplace = replaceCharMap_.find(inText);
    if (itReplace != replaceCharMap_.end()) {
        result = static_cast<unsigned char>(itReplace->second);
    } else {
        result = 0x3F;
    }
    return result;
}

int MsgTextConvert::ConvertGSM7bitToUCS2(OUT unsigned char *pDestText, IN int maxLength,
    IN const unsigned char *pSrcText, IN int srcTextLen, const IN MsgLangInfo *pLangInfo)
{
    int outTextLen = 0;
    unsigned char lowerByte = 0;
    unsigned char upperByte = 0;
    unsigned short result;
    if (srcTextLen == 0 || pSrcText == nullptr || pDestText == nullptr || maxLength <= 0) {
        TELEPHONY_LOGE("UCS2 to GSM7bit Failed as text length is 0");
        return -1;
    }
    for (int i = 0; i < srcTextLen && maxLength > 0; i++) {
        if (pSrcText[i] >= 0x80) {
            TELEPHONY_LOGE("a_pTextString[i]=%{public}x, The alpha isn't the gsm 7bit code", pSrcText[i]);
            return -1;
        }
        if (pLangInfo->bLockingShift) { /* National Language Locking Shift */
            TELEPHONY_LOGI("Locking Shift [%{public}d]", pLangInfo->lockingLang);
            if (pLangInfo->lockingLang == MSG_ID_TURKISH_LANG) {
                /* Check Escape */
                i += EscapeTurkishLockingToUCS2(&pSrcText[i], (srcTextLen - i), *pLangInfo, result);
                lowerByte = static_cast<unsigned char>(result & 0x00FF);
                upperByte = static_cast<unsigned char>(result >> 0x08);
            } else if (pLangInfo->lockingLang == MSG_ID_PORTUGUESE_LANG) {
                /* Check Escape */
                i += EscapePortuLockingToUCS2(&pSrcText[i], (srcTextLen - i), *pLangInfo, result);
                lowerByte = static_cast<unsigned char>(result & 0x00FF);
                upperByte = static_cast<unsigned char>(result >> 0x08);
            }
        } else {
            /* Check Escape */
            i += EscapeGSM7BitToUCS2(&pSrcText[i], (srcTextLen - i), *pLangInfo, result);
            lowerByte = static_cast<unsigned char>(result & 0x00FF);
            upperByte = static_cast<unsigned char>(result >> 0x08);
        }
        pDestText[outTextLen++] = upperByte;
        pDestText[outTextLen++] = lowerByte;
        maxLength -= 0x02;
    }

#ifdef CONVERT_DUMP
    TELEPHONY_LOGI("\n########## Dump GSM7bit -> UCS2");
    ConvertDumpTextToHex((unsigned char *)pSrcText, srcTextLen);
    ConvertDumpTextToHex((unsigned char *)pDestText, outTextLen);
#endif
    return outTextLen;
}

int MsgTextConvert::EscapeTurkishLockingToUCS2(
    const unsigned char *pSrcText, int srcLen, const MsgLangInfo &pLangInfo, unsigned short &result)
{
    int index = 0;
    if (pSrcText == nullptr || srcLen <= 0) {
        return index;
    }
    /* Turkish National Language Locking Shift Table -> UCS2 */
    const WCHAR g_turkishLockingToUCS2[] = { /* @ */
        0x0040, 0x00A3, 0x0024, 0x00A5, 0x20AC, 0x00E9, 0x00F9, 0x00EC, 0x00F2, 0x00C7, 0x000A, 0x011E, 0x011F, 0x000D,
        0x00C5, 0x00E5, 0x0394, 0x005F, 0x03A6, 0x0393, 0x039B, 0x03A9, 0x03A0, 0x03A8, 0x03A3, 0x0398, 0x039E, 0x001B,
        0x015E, 0x015F, 0x00DF, 0x00C9,
        /* SP */
        0x0020, 0x0021, 0x0022, 0x0023, 0x00A4, 0x0025, 0x0026, 0x0027, 0x0028, 0x0029, 0x002A, 0x002B, 0x002C, 0x002D,
        0x002E, 0x002F,
        /* 0 */
        0x0030, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037, 0x0038, 0x0039, 0x003A, 0x003B, 0x003C, 0x003D,
        0x003E, 0x003F, 0x0130, 0x0041, 0x0042, 0x0043, 0x0044, 0x0045, 0x0046, 0x0047, 0x0048, 0x0049, 0x004A, 0x004B,
        0x004C, 0x004D, 0x004E, 0x004F,
        /* P */
        0x0050, 0x0051, 0x0052, 0x0053, 0x0054, 0x0055, 0x0056, 0x0057, 0x0058, 0x0059, 0x005A, 0x00C4, 0x00D6, 0x00D1,
        0x00DC, 0x00A7,
        /* c */
        0x00E7, 0x0061, 0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067, 0x0068, 0x0069, 0x006A, 0x006B, 0x006C, 0x006D,
        0x006E, 0x006F,
        /* p */
        0x0070, 0x0071, 0x0072, 0x0073, 0x0074, 0x0075, 0x0076, 0x0077, 0x0078, 0x0079, 0x007A, 0x00E4, 0x00F6, 0x00F1,
        0x00FC, 0x00E0
    };
    /* Check Escape */
    if (g_turkishLockingToUCS2[pSrcText[index]] == 0x001B) {
        index++;
        if (index > srcLen) {
            return index;
        }
        result = EscapeToUCS2(pSrcText[index], pLangInfo);
    } else { /* TURKISH - National Language Locking Shift */
        result = g_turkishLockingToUCS2[pSrcText[index]];
    }
    return index;
}

int MsgTextConvert::EscapePortuLockingToUCS2(
    const unsigned char *pSrcText, int srcLen, const MsgLangInfo &pLangInfo, unsigned short &result)
{
    int index = 0;
    if (pSrcText == nullptr || srcLen <= 0) {
        return index;
    }
    /* Portuguese National Language Locking Shift Table -> UCS2 */
    const WCHAR g_portuLockingToUCS2[] = { /* @ */
        0x0040, 0x00A3, 0x0024, 0x00A5, 0x00EA, 0x00E9, 0x00FA, 0x00ED, 0x00F3, 0x00E7, 0x000A, 0x00D4, 0x00F4, 0x000D,
        0x00C1, 0x00E1, 0x0394, 0x005F, 0x0020, 0x00C7, 0x00C0, 0x0020, 0x005E, 0x005C, 0x20AC, 0x00D3, 0x007C, 0x001B,
        0x00C2, 0x00E2, 0x00CA, 0x00C9,
        /* SP */
        0x0020, 0x0021, 0x0022, 0x0023, 0x00A4, 0x0025, 0x0026, 0x0027, 0x0028, 0x0029, 0x002A, 0x002B, 0x002C, 0x002D,
        0x002E, 0x002F,
        /* 0 */
        0x0030, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037, 0x0038, 0x0039, 0x003A, 0x003B, 0x003C, 0x003D,
        0x003E, 0x003F, 0x00CD, 0x0041, 0x0042, 0x0043, 0x0044, 0x0045, 0x0046, 0x0047, 0x0048, 0x0049, 0x004A, 0x004B,
        0x004C, 0x004D, 0x004E, 0x004F,
        /* P */
        0x0050, 0x0051, 0x0052, 0x0053, 0x0054, 0x0055, 0x0056, 0x0057, 0x0058, 0x0059, 0x005A, 0x00C3, 0x00D5, 0x00DA,
        0x00DC, 0x00A7, 0x00BF, 0x0061, 0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067, 0x0068, 0x0069, 0x006A, 0x006B,
        0x006C, 0x006D, 0x006E, 0x006F,
        /* p */
        0x0070, 0x0071, 0x0072, 0x0073, 0x0074, 0x0075, 0x0076, 0x0077, 0x0078, 0x0079, 0x007A, 0x00E3, 0x00F5, 0x0020,
        0x00FC, 0x00E0
    };
    if (g_portuLockingToUCS2[pSrcText[index]] == 0x001B) {
        index++;
        if (index > srcLen) {
            return index;
        }
        result = EscapeToUCS2(pSrcText[index], pLangInfo);
    } else { /* PORTUGUESE - National Language Locking Shift */
        result = g_portuLockingToUCS2[pSrcText[index]];
    }
    return index;
}

int MsgTextConvert::EscapeGSM7BitToUCS2(
    const unsigned char *pSrcText, int srcLen, const MsgLangInfo &pLangInfo, unsigned short &result)
{
    int index = 0;
    if (pSrcText == nullptr || srcLen <= 0) {
        return index;
    }
    if (GSM7_BIT_TO_UC_S2[pSrcText[index]] == 0x001B) {
        index++;
        if (index > srcLen) {
            return index;
        }
        result = EscapeToUCS2(pSrcText[index], pLangInfo);
    } else {
        result = GSM7_BIT_TO_UC_S2[pSrcText[index]];
    }
    return index;
}

unsigned short MsgTextConvert::EscapeToUCS2(const unsigned char srcText, const MsgLangInfo &pLangInfo)
{
    unsigned short result = 0;
    if (pLangInfo.bSingleShift) { /* National Language Single Shift */
        TELEPHONY_LOGI("Single Shift [%{public}d]", pLangInfo.singleLang);
        switch (pLangInfo.singleLang) {
            case MSG_ID_TURKISH_LANG:
                GetTurkishSingleToUCS2(srcText, result);
                break;
            case MSG_ID_SPANISH_LANG:
                GetSpanishSingleToUCS2(srcText, result);
                break;
            case MSG_ID_PORTUGUESE_LANG:
                GetPortuSingleToUCS2(srcText, result);
                break;
            default:
                GetGsm7BitExtToUCS2(srcText, result);
                break;
        }
    } else { /* GSM 7 bit Default Alphabet Extension Table */
        GetGsm7BitExtToUCS2(srcText, result);
    }
    return result;
}

void MsgTextConvert::GetTurkishSingleToUCS2(const unsigned char &srcText, unsigned short &result)
{
    /* Turkish National Language Single Shift Table -> UCS2 */
    const WCHAR g_turkishSingleToUCS2[] = { /* 0x0020 -> (SP) for invalid code */
        /* Page Break */
        0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x000C, 0x0020, 0x0020, 0x0020,
        0x0020, 0x0020,
        /* ^ */
        0x0020, 0x0020, 0x0020, 0x0020, 0x005E, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x001B, 0x0020, 0x0020,
        0x0020, 0x0020,
        /* { */ /* } */
        /* \ */
        0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x007B, 0x007D, 0x0020, 0x0020, 0x0020, 0x0020,
        0x0020, 0x005C,
        /* [ */ /* ~ */ /* ] */
        0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x005B, 0x007E,
        0x005D, 0x0020,
        /* | */
        0x007C, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x011E, 0x0020, 0x0130, 0x0020, 0x0020, 0x0020, 0x0020,
        0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x015E, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
        0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x00E7, 0x0020, 0x20AC, 0x0020, 0x011F, 0x0020, 0x0131,
        0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x015F, 0x0020, 0x0020, 0x0020, 0x0020,
        0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020
    };
    result = g_turkishSingleToUCS2[srcText];
}

void MsgTextConvert::GetSpanishSingleToUCS2(const unsigned char &srcText, unsigned short &result)
{
    /* Spanish National Language Single Shift Table -> UCS2 */
    const WCHAR g_spanishSingleToUCS2[] = { /* 0x0020 -> (SP) for invalid code */
        /* Page Break */
        0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x00E7, 0x000C, 0x0020, 0x0020, 0x0020,
        0x0020, 0x0020,
        /* ^ */
        0x0020, 0x0020, 0x0020, 0x0020, 0x005E, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x001B, 0x0020, 0x0020,
        0x0020, 0x0020,
        /* { */ /* } */
        /* \ */
        0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x007B, 0x007D, 0x0020, 0x0020, 0x0020, 0x0020,
        0x0020, 0x005C,
        /* [ */ /* ~ */ /* ] */
        0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x005B, 0x007E,
        0x005D, 0x0020,
        /* | */
        0x007C, 0x00C1, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x00CD, 0x0020, 0x0020, 0x0020, 0x0020,
        0x0020, 0x00D3, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x00DA, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
        0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x00E1, 0x0020, 0x0020, 0x0020, 0x20AC, 0x0020, 0x0020, 0x0020, 0x00ED,
        0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x00F3, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x00FA, 0x0020, 0x0020,
        0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020
    };
    result = g_spanishSingleToUCS2[srcText];
}

void MsgTextConvert::GetGsm7BitExtToUCS2(const unsigned char &srcText, unsigned short &result)
{
    /* GSM 7 bit Default Alphabet Extension Table -> UCS2 */
    const WCHAR g_gsm7BitExtToUCS2[] = { /* 0x0020 -> (SP) for invalid code */
        /* Page Break */
        0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x000C, 0x0020, 0x0020, 0x0020,
        0x0020, 0x0020,
        /* ^ */
        0x0020, 0x0020, 0x0020, 0x0020, 0x005E, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x001B, 0x0020, 0x0020,
        0x0020, 0x0020,
        /* { */ /* } */
        /* \ */
        0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x007B, 0x007D, 0x0020, 0x0020, 0x0020, 0x0020,
        0x0020, 0x005C,
        /* [ */ /* ~ */ /* ] */
        0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x005B, 0x007E,
        0x005D, 0x0020,
        /* | */
        0x007C, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
        0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
        0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x20AC, 0x0020, 0x0020, 0x0020, 0x0020,
        0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
        0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020
    };
    result = g_gsm7BitExtToUCS2[srcText];
}

void MsgTextConvert::GetPortuSingleToUCS2(const unsigned char &srcText, unsigned short &result)
{
    /* Portuguese National Language Single Shift Table -> UCS2 */
    const WCHAR g_portuSingleToUCS2[] = { /* 0x0020 -> (SP) for invalid code */
        /* Page Break */
        0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x00EA, 0x0020, 0x0020, 0x0020, 0x00E7, 0x000C, 0x00D4, 0x00F4, 0x0020,
        0x00C1, 0x00E1,
        /* ^ */
        0x0020, 0x0020, 0x03A6, 0x0393, 0x005E, 0x03A9, 0x03A0, 0x03A8, 0x03A3, 0x0398, 0x0020, 0x001B, 0x0020, 0x0020,
        0x0020, 0x00CA,
        /* { */ /* } */
        /* \ */
        0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x007B, 0x007D, 0x0020, 0x0020, 0x0020, 0x0020,
        0x0020, 0x005C,
        /* [ */ /* ~ */ /* ] */
        0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x005B, 0x007E,
        0x005D, 0x0020,
        /* | */
        0x007C, 0x00C0, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x00CD, 0x0020, 0x0020, 0x0020, 0x0020,
        0x0020, 0x00D3, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x00DA, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x00C3,
        0x00D5, 0x0020, 0x0020, 0x0020, 0x0020, 0x00C2, 0x0020, 0x0020, 0x0020, 0x20AC, 0x0020, 0x0020, 0x0020, 0x00ED,
        0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x00F3, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x00FA, 0x0020, 0x0020,
        0x0020, 0x0020, 0x0020, 0x00E3, 0x00F5, 0x0020, 0x0020, 0x00E2
    };
    result = g_portuSingleToUCS2[srcText];
}

void MsgTextConvert::ConvertDumpTextToHex(const unsigned char *pText, int length)
{
    if (pText == nullptr) {
        return;
    }
    TELEPHONY_LOGI("=======================================");
    TELEPHONY_LOGI("   Dump Text To Hex - Length :%{public}d", length);
    TELEPHONY_LOGI("=======================================");
    for (int i = 0; i < length; i++) {
        TELEPHONY_LOGI("[%{public}02x]", pText[i]);
    }
    TELEPHONY_LOGI("=======================================");
}

void MsgTextConvert::Base64Encode(const std::string &src, std::string &dest)
{
    gchar *encode_data = g_base64_encode((guchar *)src.data(), src.length());
    if (encode_data == nullptr) {
        return;
    }
    gsize out_len = 0;
    out_len = strlen(encode_data);
    std::string temp(static_cast<char *>(encode_data), out_len);
    dest = temp;

    if (encode_data != nullptr) {
        g_free(encode_data);
    }
}

void MsgTextConvert::Base64Decode(const std::string &src, std::string &dest)
{
    gsize out_len = 0;
    char *decodeData = reinterpret_cast<char *>(g_base64_decode(src.data(), &out_len));
    if (decodeData == nullptr) {
        return;
    }
    std::string temp(decodeData, out_len);
    dest = temp;

    if (decodeData != nullptr) {
        g_free(decodeData);
    }
}

bool MsgTextConvert::GetEncodeString(
    std::string &encodeString, uint32_t charset, uint32_t valLength, const std::string &strEncodeString)
{
    bool ret = false;
    char *pDest = nullptr;
    gsize bytes_read = 0;
    gsize bytes_written = 0;
    GError *error = nullptr;
    std::string strToCodeset("UTF-8");
    std::string strFromCodeset;
    auto charSetInstance = DelayedSingleton<MmsCharSet>::GetInstance();
    if (charSetInstance == nullptr || (!charSetInstance->GetCharSetStrFromInt(strFromCodeset, charset))) {
        strFromCodeset = "UTF-8";
    }
    pDest = g_convert(strEncodeString.c_str(), valLength, strToCodeset.c_str(), strFromCodeset.c_str(), &bytes_read,
        &bytes_written, &error);
    if (!error) {
        encodeString = std::string(pDest, bytes_written);
        ret = true;
    } else {
        TELEPHONY_LOGE("EncodeString charset convert fail.");
        ret = false;
    }
    if (pDest != nullptr) {
        g_free(pDest);
    }
    return ret;
}
} // namespace Telephony
} // namespace OHOS