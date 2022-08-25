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

#include "mms_charset.h"

#include "cstdint"
#include "map"
#include "utility"

namespace OHOS {
namespace Telephony {
MmsCharSet::MmsCharSet()
{
    mapCharSet_.emplace("US-ASCII", 0x03);
    mapCharSet_.emplace("UTF-16", 0x03F7);
    mapCharSet_.emplace("CSUNICODE", 0x03E8);
    mapCharSet_.emplace("UTF-8", 0x6A);
    mapCharSet_.emplace("ISO-2022-KR", 0x25);
    mapCharSet_.emplace("KS_C_5601-1987", 0x24);
    mapCharSet_.emplace("EUC-KR", 0x26);
    mapCharSet_.emplace("ISO-2022-JP", 0x27);
    mapCharSet_.emplace("ISO-2022-JP-2", 0x28);
    mapCharSet_.emplace("ISO_8859-1", 0x04);
    mapCharSet_.emplace("ISO_8859-2", 0x05);
    mapCharSet_.emplace("ISO-8859-3", 0x06);
    mapCharSet_.emplace("ISO-8859-4", 0x07);
    mapCharSet_.emplace("ISO-8859-5", 0x08);
    mapCharSet_.emplace("ISO-8859-6", 0x09);
    mapCharSet_.emplace("ISO-8859-7", 0x0A);
    mapCharSet_.emplace("ISO-8859-8", 0x0B);
    mapCharSet_.emplace("ISO-8859-9", 0x0C);
    mapCharSet_.emplace("ISO-8859-10", 0x0D);
    mapCharSet_.emplace("ISO-8859-15", 0x6F);
    mapCharSet_.emplace("SHIFT_JIS", 0x11);
    mapCharSet_.emplace("EUC-JP", 0x13);
    mapCharSet_.emplace("GB2312", 0x07E9);
    mapCharSet_.emplace("BIG5", 0x0d);
    mapCharSet_.emplace("WINDOWS-1251", 0x08CB);
    mapCharSet_.emplace("KOI8-R", 0x0824);
    mapCharSet_.emplace("KOI8-U", 0x0828);
}

MmsCharSet::~MmsCharSet()
{
    mapCharSet_.clear();
}

bool MmsCharSet::GetCharSetIntFromString(uint32_t &charSet, const std::string &strCharSet)
{
    auto iterMap = mapCharSet_.find(strCharSet);
    if (iterMap != mapCharSet_.end()) {
        charSet = iterMap->second;
        return true;
    }
    return false;
}

bool MmsCharSet::GetCharSetStrFromInt(std::string &strCharSet, uint32_t charSet)
{
    for (auto it = mapCharSet_.begin(); it != mapCharSet_.end(); it++) {
        if (it->second == charSet) {
            strCharSet = it->first;
            return true;
        }
    }
    return false;
}
} // namespace Telephony
} // namespace OHOS
