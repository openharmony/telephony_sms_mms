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

#include "mms_address.h"

#include "iosfwd"
#include "mms_codec_type.h"
#include "string"

namespace OHOS {
namespace Telephony {
MmsAddress::MmsAddress(const std::string addressString, MmsCharSets charset)
    : addressString_(addressString), charset_(charset)
{
    CheckAddressType();
}

MmsAddress::MmsAddress() {}

MmsAddress::~MmsAddress() {}

void MmsAddress::SetMmsAddressString(const std::string addressString, MmsCharSets charset)
{
    addressString_ = addressString;
    charset_ = charset;
    CheckAddressType();
}

std::string MmsAddress::GetAddressString()
{
    return addressString_;
}

MmsCharSets MmsAddress::GetAddressCharset()
{
    return charset_;
}

MmsAddress::MmsAddressType MmsAddress::GetAddressType()
{
    return addressType_;
}

void MmsAddress::CheckAddressType()
{
    if (addressString_.find("PLMN")) {
        addressType_ = ADDRESS_TYPE_PLMN;
    } else if (addressString_.find("IPv4")) {
        addressType_ = ADDRESS_TYPE_PLMN;
    } else if (addressString_.find("IPv6")) {
        addressType_ = ADDRESS_TYPE_PLMN;
    } else if (addressString_.find("EMAIL")) {
        addressType_ = ADDRESS_TYPE_PLMN;
    } else {
        addressType_ = ADDRESS_TYPE_UNKNOWN;
    }
}
} // namespace Telephony
} // namespace OHOS
