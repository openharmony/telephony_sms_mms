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

#include "string_utils.h"

#include <sstream>
#include <string_ex.h>

#include "cstddef"
#include "cstdint"
#include "cstdlib"
#include "ostream"
#include "string"
#include "telephony_log_wrapper.h"
#include "vector"

namespace OHOS {
namespace Telephony {
static constexpr uint8_t HEX_OFFSET = 4;
static constexpr uint8_t STEP_2BIT = 2;
static constexpr char HEX_TABLE[] = "0123456789ABCDEF";

StringUtils::StringUtils() {}

StringUtils::~StringUtils() {}

uint16_t StringUtils::HexCharToInt(char c)
{
    const uint8_t decimal = 10;
    if (c >= '0' && c <= '9') {
        return (c - '0');
    }
    if (c >= 'A' && c <= 'F') {
        return (c - 'A' + decimal);
    }
    if (c >= 'a' && c <= 'f') {
        return (c - 'a' + decimal);
    }
    return 0;
}

std::string StringUtils::StringToHex(const std::string &data)
{
    std::stringstream ss;
    for (std::string::size_type i = 0; i < data.size(); ++i) {
        unsigned char temp = static_cast<unsigned char>(data[i]) >> HEX_OFFSET;
        ss << HEX_TABLE[temp] << HEX_TABLE[static_cast<unsigned char>(data[i]) & 0xf];
    }
    return ss.str();
}

std::string StringUtils::StringToHex(const char *data, int byteLength)
{
    std::stringstream ss;
    for (int i = 0; i < byteLength; ++i) {
        unsigned char temp = static_cast<unsigned char>(data[i]) >> HEX_OFFSET;
        ss << HEX_TABLE[temp] << HEX_TABLE[static_cast<unsigned char>(data[i]) & 0xf];
    }
    return ss.str();
}

std::string StringUtils::StringToHex(const std::vector<uint8_t> &data)
{
    std::stringstream ss;
    for (std::size_t i = 0; i < data.size(); ++i) {
        unsigned char temp = static_cast<unsigned char>(data[i]) >> HEX_OFFSET;
        ss << HEX_TABLE[temp] << HEX_TABLE[static_cast<unsigned char>(data[i]) & 0xf];
    }
    return ss.str();
}

std::string StringUtils::HexToString(const std::string &str)
{
    std::string result;
    uint8_t hexDecimal = 16;
    uint8_t hexStep = 2;
    if (str.length() <= 0) {
        return result;
    }
    for (size_t i = 0; i < str.length() - 1; i += STEP_2BIT) {
        std::string byte = str.substr(i, hexStep);
        char chr = 0;
        long strTemp = strtol(byte.c_str(), nullptr, hexDecimal);
        if (strTemp > 0) {
            chr = static_cast<char>(strTemp);
        }
        result.push_back(chr);
    }
    return result;
}

std::vector<uint8_t> StringUtils::HexToByteVector(const std::string &str)
{
    std::vector<uint8_t> ret;
    int sz = static_cast<int>(str.length());
    if (sz <= 0) {
        return ret;
    }
    for (int i = 0; i < (sz - 1); i += STEP_2BIT) {
        auto temp = static_cast<uint8_t>((HexCharToInt(str.at(i)) << HEX_OFFSET) | HexCharToInt(str.at(i + 1)));
        ret.push_back(temp);
    }
    return ret;
}

std::string StringUtils::ToUtf8(const std::u16string &str16)
{
    if (str16.empty()) {
        std::string ret;
        return ret;
    }
    return Str16ToStr8(str16);
}

std::u16string StringUtils::ToUtf16(const std::string &str)
{
    if (str.empty()) {
        std::u16string ret;
        return ret;
    }
    return Str8ToStr16(str);
}

bool StringUtils::IsEmpty(const std::string &str)
{
    if (str.empty()) {
        return true;
    }
    std::string strTemp = TrimStr(str);
    return strTemp.empty() || strlen(strTemp.c_str()) == 0;
}

std::string StringUtils::GetPortFromURL(const std::string &url)
{
    std::string delimiter = "://";
    std::string protocol = GetProtocolFromURL(url);
    std::string hostname = GetHostnameFromURL(url);
    size_t start = protocol.empty() ? hostname.size() : protocol.size() + delimiter.size() + hostname.size();
    size_t posStart = url.find_first_of(':', start);
    if (posStart == std::string::npos) {
        return "";
    }
    size_t posEnd = std::min({url.find('/', start), url.find('?', start)});
    if (posEnd == std::string::npos) {
        return url.substr(posStart + 1);
    }
    if (posStart > posEnd) {
        return "";
    }
    return url.substr(posStart + 1, posEnd - posStart - 1);
}

std::string StringUtils::GetHostnameFromURL(const std::string &url)
{
    if (url.empty()) {
        return "";
    }
    std::string delimiter = "://";
    std::string tempUrl = url;
    std::replace(tempUrl.begin(), tempUrl.end(), '\\', '/');
    size_t posStart = tempUrl.find(delimiter);
    if (posStart != std::string::npos) {
        posStart += delimiter.length();
    } else {
        posStart = 0;
    }
    size_t notSlash = tempUrl.find_first_not_of('/', posStart);
    if (notSlash != std::string::npos) {
        posStart = notSlash;
    }
    size_t posEnd = std::min({ tempUrl.find(':', posStart),
                              tempUrl.find('/', posStart), tempUrl.find('?', posStart) });
    if (posEnd != std::string::npos) {
        return tempUrl.substr(posStart, posEnd - posStart);
    }
    return tempUrl.substr(posStart);
}

std::string StringUtils::GetHostnameWithPortFromURL(const std::string& url)
{
    std::string portDelimiter = ":";
    auto hostname = GetHostnameFromURL(url);
    if (!hostname.empty()) {
        std::string port = GetPortFromURL(url);
        if (!port.empty()) {
            hostname += portDelimiter + port;
        }
    }
    return hostname;
}

std::string StringUtils::GetProtocolFromURL(const std::string &url)
{
    std::string delimiter = "://";
    size_t pos = url.find(delimiter);
    if (pos != std::string::npos) {
        return url.substr(0, pos);
    }
    return "";
}
} // namespace Telephony
} // namespace OHOS