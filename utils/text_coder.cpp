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

#include "text_coder.h"
#include "glib.h"
#include "mms_charset.h"
#include "securec.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
using msg_encode_type_t = uint8_t;
using namespace std;
static constexpr uint8_t GSM7_DEFLIST_LEN = 128;
static constexpr uint8_t UCS2_LEN_MIN = 2;
static constexpr uint32_t UCS2_LEN_MAX = INT_MAX / sizeof(WCHAR);

const WCHAR GSM7_BIT_TO_UC_S2[] = { 0x0040, 0x00A3, 0x0024, 0x00A5, 0x00E8, 0x00E9, 0x00F9, 0x00EC, 0x00F2, 0x00C7,
    0x000A, 216, 0x00F8, 0x000D, 0x00C5, 0x00E5, 0x0394, 0x005F, 0x03A6, 0x0393, 0x039B, 0x03A9, 0x03A0, 0x03A8, 0x03A3,
    0x0398, 0x039E, 0x001B, 0x00C6, 0x00E6, 0x00DF, 0x00C9, 0x0020, 0x0021, 0x0022, 0x0023, 0x00A4, 0x0025, 0x0026,
    0x0027, 0x0028, 0x0029, 0x002A, 0x002B, 0x002C, 0x002D, 0x002E, 0x002F, 0x0030, 0x0031, 0x0032, 0x0033, 0x0034,
    0x0035, 0x0036, 0x0037, 0x0038, 0x0039, 0x003A, 0x003B, 0x003C, 0x003D, 0x003E, 0x003F, 0x00A1, 0x0041, 0x0042,
    0x0043, 0x0044, 0x0045, 0x0046, 0x0047, 0x0048, 0x0049, 0x004A, 0x004B, 0x004C, 0x004D, 0x004E, 0x004F, 0x0050,
    0x0051, 0x0052, 0x0053, 0x0054, 0x0055, 0x0056, 0x0057, 0x0058, 0x0059, 0x005A, 0x00C4, 0x00D6, 0x00D1, 0x00DC,
    0x00A7, 0x00BF, 0x0061, 0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067, 0x0068, 0x0069, 0x006A, 0x006B, 0x006C,
    0x006D, 0x006E, 0x006F, 0x0070, 0x0071, 0x0072, 0x0073, 0x0074, 0x0075, 0x0076, 0x0077, 0x0078, 0x0079, 0x007A,
    0x00E4, 0x00F6, 0x00F1, 0x00FC, 0x00E0 };

TextCoder::TextCoder()
{
    InitExtCharMap();
    InitGsm7bitDefMap();
    InitGsm7bitExtMap();
    InitTurkishMap();
    InitSpanishMap();
    InitPortuMap();
    InitReplaceCharMap();
}

TextCoder::~TextCoder()
{
    extCharMap_.clear();
    gsm7bitDefMap_.clear();
    gsm7bitExtMap_.clear();
    turkishMap_.clear();
    spanishMap_.clear();
    portuMap_.clear();
    replaceCharMap_.clear();
}

TextCoder &TextCoder::Instance()
{
    static TextCoder instance;
    return instance;
}

void TextCoder::InitExtCharMap()
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

void TextCoder::InitGsm7bitDefMap()
{
    gsm7bitDefMap_.clear();
    for (uint8_t i = 0; i < GSM7_DEFLIST_LEN; i++) {
        gsm7bitDefMap_[GSM7_BIT_TO_UC_S2[i]] = i;
    }
}

void TextCoder::InitGsm7bitExtMap()
{
    gsm7bitExtMap_.clear();
    gsm7bitExtMap_ = { { 0x005B, 0x3C }, { 0x005D, 0x3E }, { 0x007B, 0x28 }, { 0x007D, 0x29 }, { 0x000C, 0x0A },
        { 0x005C, 0x2F }, { 0x005E, 0x14 }, { 0x007C, 0x40 }, { 0x007E, 0x3D }, { 0x20AC, 0x65 } };
}

void TextCoder::InitTurkishMap()
{
    // Turkish
    turkishMap_.clear();
    turkishMap_ = { { 0x005B, 0x3C }, { 0x005D, 0x3E }, { 0x007B, 0x28 }, { 0x007D, 0x29 }, { 0x000C, 0x0A },
        { 0x005C, 0x2F }, { 0x005E, 0x14 }, { 0x007C, 0x40 }, { 0x007E, 0x3D }, { 0x20AC, 0x65 }, { 0x00E7, 0x63 },
        { 0x011E, 0x47 }, { 0x011F, 0x67 }, { 0x01E6, 0x47 }, { 0x01E7, 0x67 }, { 0x0130, 0x49 }, { 0x0131, 0x69 },
        { 0x015E, 0x53 }, { 0x015F, 0x73 } };
}

void TextCoder::InitSpanishMap()
{
    // Spanish
    spanishMap_.clear();
    spanishMap_ = { { 0x005B, 0x3C }, { 0x005D, 0x3E }, { 0x007B, 0x28 }, { 0x007D, 0x29 }, { 0x000C, 0x0A },
        { 0x005C, 0x2F }, { 0x005E, 0x14 }, { 0x007C, 0x40 }, { 0x007E, 0x3D }, { 0x20AC, 0x65 }, { 0x00C1, 0x41 },
        { 0x00E1, 0x61 }, { 0x00CD, 0x49 }, { 0x00ED, 0x69 }, { 0x00D3, 0x4F }, { 0x00F3, 0x6F }, { 0x00DA, 0x55 },
        { 0x00FA, 0x75 } };
}

void TextCoder::InitPortuMap()
{
    // Portuguese
    portuMap_.clear();
    portuMap_ = { { 0x005B, 0x3C }, { 0x005D, 0x3E }, { 0x007B, 0x28 }, { 0x007D, 0x29 }, { 0x000C, 0x0A },
        { 0x005C, 0x2F }, { 0x005E, 0x14 }, { 0x007C, 0x40 }, { 0x007E, 0x3D }, { 0x20AC, 0x65 }, { 0x00D4, 0x0B },
        { 0x00F4, 0x0C }, { 0x00C1, 0x0E }, { 0x00E1, 0x0F }, { 0x00CA, 0x1F }, { 0x00EA, 0x05 }, { 0x00C0, 0x41 },
        { 0x00E7, 0x09 }, { 0x00CD, 0x49 }, { 0x00ED, 0x69 }, { 0x00D3, 0x4F }, { 0x00F3, 0x6F }, { 0x00DA, 0x55 },
        { 0x00FA, 0x75 }, { 0x00C3, 0x61 }, { 0x00E3, 0x7B }, { 0x00D5, 0x5C }, { 0x00F5, 0x7C }, { 0x00C2, 0x61 },
        { 0x00E2, 0x7F }, { 0x03A6, 0x12 }, { 0x0393, 0x13 }, { 0x03A9, 0x15 }, { 0x03A0, 0x16 }, { 0x03A8, 0x17 },
        { 0x03A3, 0x18 }, { 0x0398, 0x19 } };
}

void TextCoder::InitReplaceCharMap()
{
    // character replacement table
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
        { 0x00D6, 0x4F }, { 216, 0x4F }, { 0x014C, 0x4F }, { 0x014E, 0x4F }, { 0x01D1, 0x4F }, { 0x01EA, 0x4F },
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

void TextCoder::Base64Encode(const std::string &src, std::string &dest)
{
    gchar *encode_data = g_base64_encode((guchar *)src.data(), src.length());
    if (encode_data == nullptr) {
        return;
    }
    gsize out_len = 0;
    out_len = strlen(encode_data);
    std::string temp(encode_data, out_len);
    dest = temp;

    if (encode_data != nullptr) {
        g_free(encode_data);
    }
}

void TextCoder::Base64Decode(const std::string &src, std::string &dest)
{
    gsize out_len = 0;
    gchar *decodeData = reinterpret_cast<gchar *>(g_base64_decode(src.data(), &out_len));
    if (decodeData == nullptr) {
        return;
    }
    std::string temp(decodeData, out_len);
    dest = temp;

    if (decodeData != nullptr) {
        g_free(decodeData);
    }
}

bool TextCoder::GetEncodeString(
    std::string &encodeString, uint32_t charset, uint32_t valLength, const std::string &strEncodeString)
{
    bool ret = false;
    gchar *pDest = nullptr;
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

/**
 * @brief Utf8ToGsm7bit
 * max # of Ucs2 chars, NOT bytes. when all utf8 chars are only one byte,
 * Ucs2Length is maxUcs2 Length. otherwise (ex: 2 bytes of UTF8 is one char)
 * Ucs2Length must be  less than utf8Length
 */
int TextCoder::Utf8ToGsm7bit(uint8_t *dest, int maxLength, const uint8_t *src, int srcLength, MSG_LANGUAGE_ID_T &langId)
{
    if (srcLength == -1 && src) {
        // null terminated string
        srcLength = strlen(reinterpret_cast<const gchar *>(src));
    }
    if (srcLength <= 0 || src == nullptr || dest == nullptr || maxLength <= 0) {
        TELEPHONY_LOGE("text is null");
        return 0;
    }

    int maxUcs2Length = srcLength;
    if (static_cast<uint32_t>(maxUcs2Length) >= UCS2_LEN_MAX) {
        TELEPHONY_LOGE("src over size");
        return 0;
    }
    std::unique_ptr<WCHAR[]> ucs2Text = std::make_unique<WCHAR[]>(maxUcs2Length);
    if (ucs2Text == nullptr) {
        TELEPHONY_LOGE("make_unique error");
        return 0;
    }
    WCHAR *pUcs2Text = ucs2Text.get();
    if (memset_s(pUcs2Text, maxUcs2Length * sizeof(WCHAR), 0x00, maxUcs2Length * sizeof(WCHAR)) != EOK) {
        TELEPHONY_LOGE("memset_s error");
        return 0;
    }

    TELEPHONY_LOGI("srcLength = %{public}d", srcLength);
    int ucs2Length = Utf8ToUcs2(reinterpret_cast<uint8_t *>(pUcs2Text), maxUcs2Length * sizeof(WCHAR), src, srcLength);
    return Ucs2ToGsm7bit(dest, maxLength, reinterpret_cast<uint8_t *>(pUcs2Text), ucs2Length, langId);
}

int TextCoder::Utf8ToUcs2(uint8_t *dest, int maxLength, const uint8_t *src, int srcLength)
{
    if (srcLength == -1 && src) {
        // null terminated string
        srcLength = strlen(reinterpret_cast<gchar *>(const_cast<uint8_t *>(src)));
    }
    if (srcLength <= 0 || src == nullptr || dest == nullptr || maxLength <= 0) {
        TELEPHONY_LOGE("text is null");
        return 0;
    }

    gsize textLen = static_cast<gsize>(srcLength);
    auto unicodeTemp = reinterpret_cast<uint8_t *>(dest);
    gsize remainedLength = static_cast<gsize>(maxLength);
    uint32_t err = 0;
    GIConv cd = g_iconv_open("UTF16BE", "UTF8");
    if (cd != nullptr) {
        err = g_iconv(cd, reinterpret_cast<gchar **>(const_cast<uint8_t **>(&src)), reinterpret_cast<gsize *>(&textLen),
            reinterpret_cast<gchar **>(&unicodeTemp), reinterpret_cast<gsize *>(&remainedLength));
    }
    g_iconv_close(cd);
    return (err != 0) ? -1 : (maxLength - static_cast<int>(remainedLength));
}

int TextCoder::GsmUtf8ToAuto(uint8_t *dest, int maxLength, const uint8_t *src, int srcLength,
    DataCodingScheme &scheme, SmsCodingNationalType codingNationalType, MSG_LANGUAGE_ID_T &langId)
{
    int maxUcs2Length = srcLength;
    if (maxUcs2Length <= 0 || static_cast<uint32_t>(maxUcs2Length) >= UCS2_LEN_MAX) {
        TELEPHONY_LOGE("src over size");
        return 0;
    }
    std::unique_ptr<WCHAR[]> ucs2Text = std::make_unique<WCHAR[]>(maxUcs2Length);
    if (ucs2Text == nullptr) {
        TELEPHONY_LOGE("GsmUtf8ToAuto make_unique error");
        return 0;
    }
    WCHAR *pUcs2Text = ucs2Text.get();
    if (memset_s(pUcs2Text, maxUcs2Length * sizeof(WCHAR), 0x00, maxUcs2Length * sizeof(WCHAR)) != EOK) {
        TELEPHONY_LOGE("GsmUtf8ToAuto memset_s error");
        return 0;
    }
    int ucs2Length = Utf8ToUcs2(reinterpret_cast<uint8_t *>(pUcs2Text), maxUcs2Length * sizeof(WCHAR), src, srcLength);
    int tempTextLen = 0;
    if (ucs2Length < 0) {
        scheme = DATA_CODING_8BIT;
        tempTextLen = (srcLength > maxLength) ? maxLength : srcLength;
        if (memcpy_s(dest, tempTextLen, src, tempTextLen) != EOK) {
            TELEPHONY_LOGE("GsmUtf8ToAuto memcpy_s error");
        }
        return tempTextLen;
    }
    bool unknown = false;
    int length = Ucs2ToGsm7bitAuto(dest, maxLength, reinterpret_cast<uint8_t *>(pUcs2Text), ucs2Length,
        unknown, codingNationalType);
    if (unknown) {
        scheme = DATA_CODING_UCS2;
        if (ucs2Length <= 0) {
            return length;
        }
        tempTextLen = (ucs2Length > maxLength) ? maxLength : ucs2Length;
        if (memcpy_s(dest, tempTextLen, pUcs2Text, tempTextLen) != EOK) {
            TELEPHONY_LOGE("memcpy_s error");
        }
        return tempTextLen;
    }
    langId = static_cast<MSG_LANGUAGE_ID_T>(codingNationalType);
    scheme = DATA_CODING_7BIT;
    return length;
}

int TextCoder::CdmaUtf8ToAuto(uint8_t *dest, int maxLength, const uint8_t *src, int srcLength, DataCodingScheme &scheme)
{
    int maxUcs2Length = srcLength;
    if (maxUcs2Length <= 0 || static_cast<uint32_t>(maxUcs2Length) >= UCS2_LEN_MAX) {
        TELEPHONY_LOGE("CdmaUtf8ToAuto src over size");
        return 0;
    }
    std::unique_ptr<WCHAR[]> ucs2Text = std::make_unique<WCHAR[]>(maxUcs2Length);
    if (ucs2Text == nullptr) {
        TELEPHONY_LOGE("CdmaUtf8ToAuto make_unique error");
        return 0;
    }
    WCHAR *pUcs2Text = ucs2Text.get();
    if (memset_s(pUcs2Text, maxUcs2Length * sizeof(WCHAR), 0x00, maxUcs2Length * sizeof(WCHAR)) != EOK) {
        TELEPHONY_LOGE("CdmaUtf8ToAuto memset_s error");
        return 0;
    }
    int ucs2Length = Utf8ToUcs2(reinterpret_cast<uint8_t *>(pUcs2Text), maxUcs2Length * sizeof(WCHAR), src, srcLength);
    int tempTextLen = 0;
    if (ucs2Length < 0) {
        scheme = DATA_CODING_8BIT;
        tempTextLen = (srcLength > maxLength) ? maxLength : srcLength;
        if (memcpy_s(dest, tempTextLen, src, tempTextLen) != EOK) {
            TELEPHONY_LOGE("memcpy_s error");
        }
        return tempTextLen;
    }
    bool unknown = false;
    int gsm7bitLength = Ucs2ToAscii(dest, maxLength, reinterpret_cast<uint8_t *>(pUcs2Text), ucs2Length, unknown);
    if (unknown) {
        scheme = DATA_CODING_UCS2;
        if (ucs2Length <= 0) {
            return gsm7bitLength;
        }
        tempTextLen = (ucs2Length > maxLength) ? maxLength : ucs2Length;
        if (memcpy_s(dest, tempTextLen, pUcs2Text, tempTextLen) != EOK) {
            TELEPHONY_LOGE("memcpy_s error");
        }
        return tempTextLen;
    }
    scheme = DATA_CODING_ASCII7BIT;
    return gsm7bitLength;
}

/**
 * @brief Gsm7bitToUtf8
 * max # of Ucs2 chars, NOT bytes. when all gsm7 chars are only one byte(-there is no extension)
 * Ucs2Length is maxUcs2 Length. otherwise(ex: gsm7 char starts with 0x1b)
 * Ucs2Length must be less than gsm7 length
 */
int TextCoder::Gsm7bitToUtf8(
    uint8_t *dest, int maxLength, const uint8_t *src, int srcLength, const MsgLangInfo &langInfo)
{
    int maxUcs2Length = srcLength;
    if (maxUcs2Length <= 0 || static_cast<uint32_t>(maxUcs2Length) >= UCS2_LEN_MAX) {
        TELEPHONY_LOGE("Gsm7bitToUtf8 src over size");
        return 0;
    }
    std::unique_ptr<WCHAR[]> ucs2Text = std::make_unique<WCHAR[]>(maxUcs2Length);
    if (ucs2Text == nullptr) {
        TELEPHONY_LOGE("Gsm7bitToUtf8 make_unique error");
        return 0;
    }
    WCHAR *pUcs2Text = ucs2Text.get();
    if (memset_s(pUcs2Text, maxUcs2Length * sizeof(WCHAR), 0x00, maxUcs2Length * sizeof(WCHAR)) != EOK) {
        TELEPHONY_LOGE("Gsm7bitToUtf8 memset_s error");
        return 0;
    }
    TELEPHONY_LOGI("max dest Length = %{public}d", maxLength);
    TELEPHONY_LOGI("srcLength = %{public}d", srcLength);
    int ucs2Length =
        Gsm7bitToUcs2(reinterpret_cast<uint8_t *>(pUcs2Text), maxUcs2Length * sizeof(WCHAR), src, srcLength, langInfo);
    if (ucs2Length > maxLength) {
        TELEPHONY_LOGE("src over size");
        return 0;
    }
    return Ucs2ToUtf8(dest, maxLength, reinterpret_cast<uint8_t *>(pUcs2Text), ucs2Length);
}

int TextCoder::Ucs2ToUtf8(uint8_t *dest, int maxLength, const uint8_t *src, int srcLength)
{
    if (srcLength == -1 && src) {
        TELEPHONY_LOGE("stcLength == -1 && src branch");
        // null terminated string
        srcLength = strlen(reinterpret_cast<gchar *>(const_cast<uint8_t *>(src)));
    }
    if (srcLength <= 0 || src == nullptr || dest == nullptr || maxLength <= 0) {
        TELEPHONY_LOGE("text is null");
        return 0;
    }

    gsize textLen = static_cast<gsize>(srcLength);
    uint32_t err = 0;
    gsize remainedLength = static_cast<gsize>(maxLength);
    GIConv cd = g_iconv_open("UTF8", "UTF16BE");
    if (cd != nullptr) {
        err = g_iconv(cd, reinterpret_cast<gchar **>(const_cast<uint8_t **>(&src)), reinterpret_cast<gsize *>(&textLen),
            reinterpret_cast<gchar **>(&dest), reinterpret_cast<gsize *>(&remainedLength));
    }
    g_iconv_close(cd);
    if (err != 0) {
        TELEPHONY_LOGE("g_iconv result is %{public}u", err);
    }
    int length = maxLength - static_cast<int>(remainedLength);
    if (length < 0 || length >= maxLength) {
        return 0;
    }
    return length;
}

int TextCoder::EuckrToUtf8(uint8_t *dest, int maxLength, const uint8_t *src, int srcLength)
{
    if (srcLength == -1 && src) {
        // null terminated string
        srcLength = strlen(reinterpret_cast<gchar *>(const_cast<uint8_t *>(src)));
    }
    if (srcLength <= 0 || src == nullptr || dest == nullptr || maxLength <= 0) {
        TELEPHONY_LOGE("text is null");
        return 0;
    }

    gsize remainedLength = static_cast<gsize>(maxLength);
    gsize textLen = static_cast<gsize>(srcLength);
    uint32_t err = 0;
    GIConv cd = g_iconv_open("UTF8", "EUCKR");
    if (cd != nullptr) {
        err = g_iconv(cd, reinterpret_cast<gchar **>(const_cast<uint8_t **>(&src)), reinterpret_cast<gsize *>(&textLen),
            reinterpret_cast<gchar **>(&dest), reinterpret_cast<gsize *>(&remainedLength));
    }
    g_iconv_close(cd);
    if (err != 0) {
        TELEPHONY_LOGE("g_iconv result is %{public}u", err);
    }
    int utf8Length = maxLength - static_cast<int>(remainedLength);
    if (utf8Length < 0 || utf8Length >= maxLength) {
        return 0;
    }
    dest[utf8Length] = 0x00;
    return utf8Length;
}

int TextCoder::ShiftjisToUtf8(uint8_t *dest, int maxLength, const uint8_t *src, int srcLength) const
{
    if (srcLength == -1 && src) {
        // null terminated string
        srcLength = strlen(reinterpret_cast<gchar *>(const_cast<uint8_t *>(src)));
    }
    if (srcLength <= 0 || src == nullptr || dest == nullptr || maxLength <= 0) {
        TELEPHONY_LOGE("text is null");
        return 0;
    }

    gsize textLen = static_cast<gsize>(srcLength);
    gsize remainedLength = static_cast<gsize>(maxLength);
    uint32_t err = 0;
    GIConv cd = g_iconv_open("UTF8", "SHIFT-JIS");
    if (cd != nullptr) {
        err = g_iconv(cd, reinterpret_cast<gchar **>(const_cast<uint8_t **>(&src)), reinterpret_cast<gsize *>(&textLen),
            reinterpret_cast<gchar **>(&dest), reinterpret_cast<gsize *>(&remainedLength));
    }
    g_iconv_close(cd);
    TELEPHONY_LOGI("g_iconv result is %{public}u", err);
    int utf8Length = maxLength - static_cast<int>(remainedLength);
    if (utf8Length < 0 || utf8Length >= maxLength) {
        return 0;
    }
    dest[utf8Length] = 0x00;
    return utf8Length;
}

int TextCoder::Ucs2ToGsm7bit(uint8_t *dest, int maxLength, const uint8_t *src, int srcLength, MSG_LANGUAGE_ID_T &langId)
{
    if (srcLength <= 0 || src == nullptr || dest == nullptr || maxLength <= 0) {
        TELEPHONY_LOGE("text is null");
        return -1;
    }
    int outTextLen = 0;
    int remainLen = 0;
    uint16_t inText = 0;
    uint8_t currType = GetLangType(src, srcLength);
    std::map<uint16_t, uint8_t>::iterator itChar;
    for (int index = 0; index < (srcLength - 1); index += UCS2_LEN_MIN) {
        inText = src[index];
        inText = ((inText << 0x08) & 0xFF00) | src[index + 1];
        itChar = gsm7bitDefMap_.find(inText); // check gsm7bit default char
        if (itChar != gsm7bitDefMap_.end()) {
            dest[outTextLen++] = static_cast<uint8_t>(itChar->second);
        } else {
            switch (currType) {
                case MSG_GSM7EXT_CHAR:
                    remainLen = maxLength - outTextLen;
                    outTextLen += FindGsm7bitExt(&dest[outTextLen], remainLen, inText);
                    break;
                case MSG_TURKISH_CHAR:
                    langId = MSG_ID_TURKISH_LANG;
                    remainLen = maxLength - outTextLen;
                    outTextLen += FindTurkish(&dest[outTextLen], remainLen, inText);
                    break;
                case MSG_SPANISH_CHAR:
                    langId = MSG_ID_SPANISH_LANG;
                    remainLen = maxLength - outTextLen;
                    outTextLen += FindSpanish(&dest[outTextLen], remainLen, inText);
                    break;
                case MSG_PORTUGUESE_CHAR:
                    langId = MSG_ID_PORTUGUESE_LANG;
                    remainLen = maxLength - outTextLen;
                    outTextLen += FindPortu(&dest[outTextLen], remainLen, inText);
                    break;
                default:
                    dest[outTextLen] = FindReplaceChar(inText);
                    break;
            }
            outTextLen++;
        }
        // prevent buffer overflow
        if (maxLength <= outTextLen) {
            TELEPHONY_LOGE("buffer overflow");
            break;
        }
    }
    return outTextLen;
}

std::map<uint16_t, uint8_t> TextCoder::Get7BitCodingExtMap(SmsCodingNationalType codingNationalType) const
{
    std::map<uint16_t, uint8_t> extMap = gsm7bitExtMap_;
    switch (codingNationalType) {
        case SMS_CODING_NATIONAL_TYPE_DEFAULT:
            extMap = gsm7bitExtMap_;
            break;
        case SMS_CODING_NATIONAL_TYPE_TURKISH:
            extMap = turkishMap_;
            break;
        case SMS_CODING_NATIONAL_TYPE_SPANISH:
            extMap = spanishMap_;
            break;
        case SMS_CODING_NATIONAL_TYPE_PORTUGUESE:
            extMap = portuMap_;
            break;
        default:
            extMap = gsm7bitExtMap_;
            break;
    }
    return extMap;
}

int TextCoder::Ucs2ToGsm7bitAuto(uint8_t *dest, int maxLength, const uint8_t *src, int srcLength,
    bool &unknown, SmsCodingNationalType codingNationalType)
{
    if (srcLength <= 0 || src == nullptr || dest == nullptr || maxLength <= 0) {
        TELEPHONY_LOGE("text is null");
        return -1;
    }

    std::map<uint16_t, uint8_t> extMap = Get7BitCodingExtMap(codingNationalType);
    int outTextLen = 0;
    std::map<uint16_t, uint8_t>::iterator itChar;
    std::map<uint16_t, uint8_t>::iterator itExt;
    uint16_t inText;
    for (int i = 0; i < srcLength - 1; i += UCS2_LEN_MIN) {
        inText = src[i];
        inText = ((inText << 0x08) & 0xFF00) | src[i + 1];
        itChar = gsm7bitDefMap_.find(inText); // check gsm7bit default char
        if (itChar != gsm7bitDefMap_.end()) {
            dest[outTextLen++] = static_cast<uint8_t>(itChar->second);
        } else {
            itExt = extMap.find(inText);
            if (itExt == extMap.end()) {
                TELEPHONY_LOGI("Abnormal character is included. inText : [%{public}04x]", inText);
                unknown = true;
                return 0;
            }
            if (maxLength <= outTextLen + 1) {
                TELEPHONY_LOGE("buffer overflow.");
                break;
            }
            dest[outTextLen++] = 0x1B;
            dest[outTextLen++] = static_cast<uint8_t>(itExt->second);
        }
        // prevent buffer overflow
        if (maxLength <= outTextLen) {
            TELEPHONY_LOGE("buffer overflow");
            break;
        }
    }
    return outTextLen;
}

int TextCoder::Ucs2ToAscii(uint8_t *dest, int maxLength, const uint8_t *src, int srcLength, bool &unknown)
{
    if (srcLength <= 0 || src == nullptr || dest == nullptr || maxLength <= 0) {
        TELEPHONY_LOGE("text is null");
        return -1;
    }

    int outTextLen = 0;
    std::map<uint16_t, uint8_t>::iterator itChar;
    std::map<uint16_t, uint8_t>::iterator itExt;
    uint16_t inText;
    for (int index = 0; index < srcLength - 1; index += UCS2_LEN_MIN) {
        inText = src[index];
        inText = ((inText << 0x08) & 0xFF00) | src[index + 1];
        // check default char
        if (inText > 0x007f) {
            TELEPHONY_LOGI("abnormal character is included [%{public}04x]", inText);
            unknown = true;
            return 0;
        }
        dest[outTextLen++] = static_cast<uint8_t>(inText);
        // prevent buffer overflow
        if (maxLength <= outTextLen) {
            TELEPHONY_LOGE("buffer overflow");
            break;
        }
    }
    return outTextLen;
}

uint8_t TextCoder::GetLangType(const uint8_t *src, int srcLength)
{
    if (srcLength <= 0 || src == nullptr) {
        TELEPHONY_LOGE("text is null");
        return MSG_DEFAULT_CHAR;
    }

    std::map<uint16_t, uint8_t>::iterator itExt;
    uint8_t currType = MSG_DEFAULT_CHAR;
    uint8_t newType = MSG_DEFAULT_CHAR;
    uint16_t inText;
    for (int index = 0; index < (srcLength - 1); index += UCS2_LEN_MIN) {
        inText = src[index];
        inText = ((inText << 0x08) & 0xFF00) | src[index + 1];
        itExt = extCharMap_.find(inText);
        if (itExt == extCharMap_.end()) {
            continue;
        }
        newType = static_cast<uint8_t>(itExt->second);
        if (newType >= currType) {
            bool isTurkisk = (inText == 0x00e7 && currType <= MSG_TURKISH_CHAR);
            currType = isTurkisk ? MSG_TURKISH_CHAR : newType;
        }
    }
    TELEPHONY_LOGI("charType : [%{public}hhu]", currType);
    return currType;
}

int TextCoder::FindGsm7bitExt(uint8_t *dest, int maxLength, const uint16_t inText)
{
    int outTextLen = 0;
    if (dest == nullptr || maxLength <= 0) {
        TELEPHONY_LOGE("Invalid parameter.");
        return outTextLen;
    }

    auto itExt = gsm7bitExtMap_.find(inText);
    if (itExt == gsm7bitExtMap_.end()) {
        dest[outTextLen++] = FindReplaceChar(inText);
        return outTextLen;
    }
    // prevent buffer overflow
    if (maxLength <= outTextLen + 1) {
        TELEPHONY_LOGE("FindGsm7bitExt buffer overflow");
        return outTextLen;
    }
    dest[outTextLen++] = 0x1B;
    dest[outTextLen++] = static_cast<uint8_t>(itExt->second);
    return outTextLen;
}

int TextCoder::FindTurkish(uint8_t *dest, int maxLength, const uint16_t inText)
{
    int outTextLen = 0;
    if (dest == nullptr || maxLength <= 0) {
        TELEPHONY_LOGE("Invalid parameter.");
        return outTextLen;
    }

    auto itExt = turkishMap_.find(inText);
    if (itExt == turkishMap_.end()) {
        dest[outTextLen++] = FindReplaceChar(inText);
        return outTextLen;
    }
    // prevent buffer overflow
    if (maxLength <= outTextLen + 1) {
        TELEPHONY_LOGE("FindTurkish buffer overflow");
        return outTextLen;
    }
    dest[outTextLen++] = 0x1B;
    dest[outTextLen++] = static_cast<uint8_t>(itExt->second);
    return outTextLen;
}

int TextCoder::FindSpanish(uint8_t *dest, int maxLength, const uint16_t inText)
{
    int outTextLen = 0;
    if (dest == nullptr || maxLength <= 0) {
        TELEPHONY_LOGE("Invalid parameter.");
        return outTextLen;
    }

    auto itExt = spanishMap_.find(inText);
    if (itExt == spanishMap_.end()) {
        dest[outTextLen++] = FindReplaceChar(inText);
        return outTextLen;
    }
    // prevent buffer overflow
    if (maxLength <= outTextLen + 1) {
        TELEPHONY_LOGE("FindSpanish buffer overflow");
        return outTextLen;
    }
    dest[outTextLen++] = 0x1B;
    dest[outTextLen++] = static_cast<uint8_t>(itExt->second);
    return outTextLen;
}

int TextCoder::FindPortu(uint8_t *dest, int maxLength, const uint16_t inText)
{
    int outTextLen = 0;
    if (dest == nullptr || maxLength <= 0) {
        TELEPHONY_LOGE("Invalid parameter.");
        return outTextLen;
    }

    auto itExt = portuMap_.find(inText);
    if (itExt == portuMap_.end()) {
        dest[outTextLen++] = FindReplaceChar(inText);
        return outTextLen;
    }
    // prevent buffer overflow
    if (maxLength <= outTextLen + 1) {
        TELEPHONY_LOGE("FindPortu buffer overflow");
        return outTextLen;
    }
    dest[outTextLen++] = 0x1B;
    dest[outTextLen++] = static_cast<uint8_t>(itExt->second);
    return outTextLen;
}

uint8_t TextCoder::FindReplaceChar(const uint16_t inText)
{
    uint8_t result = 0;
    auto itReplace = replaceCharMap_.find(inText);
    if (itReplace != replaceCharMap_.end()) {
        result = static_cast<uint8_t>(itReplace->second);
    } else {
        result = 0x3F;
    }
    return result;
}

int TextCoder::Gsm7bitToUcs2(
    uint8_t *dest, int maxLength, const uint8_t *src, int srcLength, const MsgLangInfo &langInfo)
{
    if (srcLength == 0 || src == nullptr || dest == nullptr || maxLength <= 0) {
        TELEPHONY_LOGE("text is null");
        return -1;
    }

    int outTextLen = 0;
    uint8_t lowerByte = 0;
    uint8_t upperByte = 0;
    uint16_t result = 0;
    for (int i = 0; i < srcLength && maxLength > UCS2_LEN_MIN; i++) {
        if (src[i] >= 0x80) {
            TELEPHONY_LOGE("a_pTextString[%{public}d]=%{public}x, The alpha isn't the gsm 7bit code", i, src[i]);
            return -1;
        }
        if (langInfo.bLockingShift) {
            TELEPHONY_LOGI("National Language Locking Shift [%{public}d]", langInfo.lockingLang);
            if (langInfo.lockingLang == MSG_ID_TURKISH_LANG) {
                i += EscapeTurkishLockingToUcs2(&src[i], (srcLength - i), langInfo, result);
                lowerByte = static_cast<uint8_t>(result & 0x00FF);
                upperByte = static_cast<uint8_t>(result >> 0x08);
            } else if (langInfo.lockingLang == MSG_ID_PORTUGUESE_LANG) {
                i += EscapePortuLockingToUcs2(&src[i], (srcLength - i), langInfo, result);
                lowerByte = static_cast<uint8_t>(result & 0x00FF);
                upperByte = static_cast<uint8_t>(result >> 0x08);
            }
        } else {
            i += EscapeGsm7bitToUcs2(&src[i], (srcLength - i), langInfo, result);
            lowerByte = static_cast<uint8_t>(result & 0x00FF);
            upperByte = static_cast<uint8_t>(result >> 0x08);
        }
        dest[outTextLen++] = upperByte;
        dest[outTextLen++] = lowerByte;
        maxLength -= 0x02;
    }
    dest[outTextLen] = '\0';

    return outTextLen;
}

int TextCoder::EscapeTurkishLockingToUcs2(const uint8_t *src, int srcLen, const MsgLangInfo &langInfo, uint16_t &result)
{
    int index = 0;
    if (src == nullptr || srcLen <= 0) {
        return index;
    }
    // Turkish National Language Locking Shift Table
    const WCHAR turkishLockingToUcs2[] = { 0x0040, 0x00A3, 0x0024, 0x00A5, 0x20AC, 0x00E9, 0x00F9, 0x00EC, 0x00F2,
        0x00C7, 0x000A, 0x011E, 0x011F, 0x000D, 0x00C5, 0x00E5, 0x0394, 0x005F, 0x03A6, 0x0393, 0x039B, 0x03A9, 0x03A0,
        0x03A8, 0x03A3, 0x0398, 0x039E, 0x001B, 0x015E, 0x015F, 0x00DF, 0x00C9, 0x0020, 0x0021, 0x0022, 0x0023, 0x00A4,
        0x0025, 0x0026, 0x0027, 0x0028, 0x0029, 0x002A, 0x002B, 0x002C, 0x002D, 0x002E, 0x002F, 0x0030, 0x0031, 0x0032,
        0x0033, 0x0034, 0x0035, 0x0036, 0x0037, 0x0038, 0x0039, 0x003A, 0x003B, 0x003C, 0x003D, 0x003E, 0x003F, 0x0130,
        0x0041, 0x0042, 0x0043, 0x0044, 0x0045, 0x0046, 0x0047, 0x0048, 0x0049, 0x004A, 0x004B, 0x004C, 0x004D, 0x004E,
        0x004F, 0x0050, 0x0051, 0x0052, 0x0053, 0x0054, 0x0055, 0x0056, 0x0057, 0x0058, 0x0059, 0x005A, 0x00C4, 0x00D6,
        0x00D1, 0x00DC, 0x00A7, 0x00E7, 0x0061, 0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067, 0x0068, 0x0069, 0x006A,
        0x006B, 0x006C, 0x006D, 0x006E, 0x006F, 0x0070, 0x0071, 0x0072, 0x0073, 0x0074, 0x0075, 0x0076, 0x0077, 0x0078,
        0x0079, 0x007A, 0x00E4, 0x00F6, 0x00F1, 0x00FC, 0x00E0 };
    // Check Escape
    if (turkishLockingToUcs2[src[index]] == 0x001B) {
        index++;
        if (index >= srcLen) {
            return index;
        }
        result = EscapeToUcs2(src[index], langInfo);
    } else {
        // TURKISH
        result = turkishLockingToUcs2[src[index]];
    }
    return index;
}

int TextCoder::EscapePortuLockingToUcs2(const uint8_t *src, int srcLen, const MsgLangInfo &langInfo, uint16_t &result)
{
    int index = 0;
    if (src == nullptr || srcLen <= 0) {
        return index;
    }
    // Portuguese National Language Locking Shift Table
    const WCHAR portuLockingToUcs2[] = { 0x0040, 0x00A3, 0x0024, 0x00A5, 0x00EA, 0x00E9, 0x00FA, 0x00ED, 0x00F3, 0x00E7,
        0x000A, 0x00D4, 0x00F4, 0x000D, 0x00C1, 0x00E1, 0x0394, 0x005F, 0x0020, 0x00C7, 0x00C0, 0x0020, 0x005E, 0x005C,
        0x20AC, 0x00D3, 0x007C, 0x001B, 0x00C2, 0x00E2, 0x00CA, 0x00C9, 0x0020, 0x0021, 0x0022, 0x0023, 0x00A4, 0x0025,
        0x0026, 0x0027, 0x0028, 0x0029, 0x002A, 0x002B, 0x002C, 0x002D, 0x002E, 0x002F, 0x0030, 0x0031, 0x0032, 0x0033,
        0x0034, 0x0035, 0x0036, 0x0037, 0x0038, 0x0039, 0x003A, 0x003B, 0x003C, 0x003D, 0x003E, 0x003F, 0x00CD, 0x0041,
        0x0042, 0x0043, 0x0044, 0x0045, 0x0046, 0x0047, 0x0048, 0x0049, 0x004A, 0x004B, 0x004C, 0x004D, 0x004E, 0x004F,
        0x0050, 0x0051, 0x0052, 0x0053, 0x0054, 0x0055, 0x0056, 0x0057, 0x0058, 0x0059, 0x005A, 0x00C3, 0x00D5, 0x00DA,
        0x00DC, 0x00A7, 0x00BF, 0x0061, 0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067, 0x0068, 0x0069, 0x006A, 0x006B,
        0x006C, 0x006D, 0x006E, 0x006F, 0x0070, 0x0071, 0x0072, 0x0073, 0x0074, 0x0075, 0x0076, 0x0077, 0x0078, 0x0079,
        0x007A, 0x00E3, 0x00F5, 0x0020, 0x00FC, 0x00E0 };
    if (portuLockingToUcs2[src[index]] == 0x001B) {
        index++;
        if (index >= srcLen) {
            return index;
        }
        result = EscapeToUcs2(src[index], langInfo);
    } else {
        // PORTUGUESE
        result = portuLockingToUcs2[src[index]];
    }
    return index;
}

int TextCoder::EscapeGsm7bitToUcs2(const uint8_t *src, int srcLen, const MsgLangInfo &langInfo, uint16_t &result)
{
    int index = 0;
    if (src == nullptr || srcLen <= 0) {
        return index;
    }
    if (GSM7_BIT_TO_UC_S2[src[index]] == 0x001B) {
        index++;
        if (index >= srcLen) {
            return index;
        }
        result = EscapeToUcs2(src[index], langInfo);
    } else {
        result = GSM7_BIT_TO_UC_S2[src[index]];
    }
    return index;
}

uint16_t TextCoder::EscapeToUcs2(const uint8_t srcText, const MsgLangInfo &langInfo)
{
    uint16_t result = 0;
    if (langInfo.bSingleShift) {
        TELEPHONY_LOGI("National Language Single Shift [%{public}d]", langInfo.singleLang);
        switch (langInfo.singleLang) {
            case MSG_ID_TURKISH_LANG:
                GetTurkishSingleToUcs2(srcText, result);
                break;
            case MSG_ID_SPANISH_LANG:
                GetSpanishSingleToUcs2(srcText, result);
                break;
            case MSG_ID_PORTUGUESE_LANG:
                GetPortuSingleToUcs2(srcText, result);
                break;
            default:
                GetGsm7BitExtToUcs2(srcText, result);
                break;
        }
    } else {
        GetGsm7BitExtToUcs2(srcText, result);
    }
    return result;
}

void TextCoder::GetTurkishSingleToUcs2(const uint8_t &srcText, uint16_t &result)
{
    // Turkish National Language Single Shift Table
    const WCHAR turkishSingleToUcs2[] = { 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
        0x0020, 0x000C, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x005E, 0x0020, 0x0020,
        0x0020, 0x0020, 0x0020, 0x0020, 0x001B, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
        0x0020, 0x0020, 0x0020, 0x007B, 0x007D, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x005C, 0x0020, 0x0020, 0x0020,
        0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x005B, 0x007E, 0x005D, 0x0020, 0x007C,
        0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x011E, 0x0020, 0x0130, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
        0x0020, 0x0020, 0x0020, 0x0020, 0x015E, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
        0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x00E7, 0x0020, 0x20AC, 0x0020, 0x011F, 0x0020, 0x0131, 0x0020,
        0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x015F, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
        0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020 };
    result = turkishSingleToUcs2[srcText];
}

void TextCoder::GetSpanishSingleToUcs2(const uint8_t &srcText, uint16_t &result)
{
    // Spanish National Language Single Shift Table
    const WCHAR spanishSingleToUcs2[] = { 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
        0x00E7, 0x000C, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x005E, 0x0020, 0x0020,
        0x0020, 0x0020, 0x0020, 0x0020, 0x001B, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
        0x0020, 0x0020, 0x0020, 0x007B, 0x007D, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x005C, 0x0020, 0x0020, 0x0020,
        0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x005B, 0x007E, 0x005D, 0x0020, 0x007C,
        0x00C1, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x00CD, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
        0x00D3, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x00DA, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
        0x0020, 0x0020, 0x0020, 0x0020, 0x00E1, 0x0020, 0x0020, 0x0020, 0x20AC, 0x0020, 0x0020, 0x0020, 0x00ED, 0x0020,
        0x0020, 0x0020, 0x0020, 0x0020, 0x00F3, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x00FA, 0x0020, 0x0020, 0x0020,
        0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020 };
    result = spanishSingleToUcs2[srcText];
}

void TextCoder::GetGsm7BitExtToUcs2(const uint8_t &srcText, uint16_t &result)
{
    // GSM 7 bit Default Alphabet Extension Table
    const WCHAR gsm7BitExtToUcs2[] = { 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
        0x000C, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x005E, 0x0020, 0x0020, 0x0020,
        0x0020, 0x0020, 0x0020, 0x001B, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
        0x0020, 0x0020, 0x007B, 0x007D, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x005C, 0x0020, 0x0020, 0x0020, 0x0020,
        0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x005B, 0x007E, 0x005D, 0x0020, 0x007C, 0x0020,
        0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
        0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
        0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x20AC, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
        0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
        0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020 };
    result = gsm7BitExtToUcs2[srcText];
}

void TextCoder::GetPortuSingleToUcs2(const uint8_t &srcText, uint16_t &result)
{
    // Portuguese National Language Single Shift Table
    const WCHAR portuSingleToUcs2[] = { 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x00EA, 0x0020, 0x0020, 0x0020, 0x00E7,
        0x000C, 0x00D4, 0x00F4, 0x0020, 0x00C1, 0x00E1, 0x0020, 0x0020, 0x03A6, 0x0393, 0x005E, 0x03A9, 0x03A0, 0x03A8,
        0x03A3, 0x0398, 0x0020, 0x001B, 0x0020, 0x0020, 0x0020, 0x00CA, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
        0x0020, 0x0020, 0x007B, 0x007D, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x005C, 0x0020, 0x0020, 0x0020, 0x0020,
        0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x005B, 0x007E, 0x005D, 0x0020, 0x007C, 0x00C0,
        0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x00CD, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x00D3,
        0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x00DA, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x00C3, 0x00D5, 0x0020,
        0x0020, 0x0020, 0x0020, 0x00C2, 0x0020, 0x0020, 0x0020, 0x20AC, 0x0020, 0x0020, 0x0020, 0x00ED, 0x0020, 0x0020,
        0x0020, 0x0020, 0x0020, 0x00F3, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x00FA, 0x0020, 0x0020, 0x0020, 0x0020,
        0x0020, 0x00E3, 0x00F5, 0x0020, 0x0020, 0x00E2 };
    result = portuSingleToUcs2[srcText];
}

} // namespace Telephony
} // namespace OHOS
