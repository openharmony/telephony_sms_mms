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

#ifndef MMS_ENCODE_STRING_H
#define MMS_ENCODE_STRING_H

#include <string>

#include "mms_decode_buffer.h"
#include "mms_encode_buffer.h"
#include "mms_address.h"

namespace OHOS {
namespace Telephony {
class MmsEncodeString {
public:
    MmsEncodeString();
    ~MmsEncodeString();
    MmsEncodeString(const MmsEncodeString &obj);
    bool DecodeEncodeString(MmsDecodeBuffer &decodeBuffer);
    bool EncodeEncodeString(MmsEncodeBuffer &encodeBuffer);
    bool GetEncodeString(std::string &encodeString);
    bool SetEncodeString(uint32_t charset, const std::string &encodeStringt);
    bool SetAddressString(MmsAddress &addrsss);
private:
    static constexpr uint32_t CHARSET_UTF8 = 0x6A;
    uint32_t charset_ = 0;
    uint32_t valLength_ = 0;
    std::string strEncodeString_ = "";
};
} // namespace Telephony
} // namespace OHOS
#endif
