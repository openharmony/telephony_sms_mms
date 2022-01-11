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

#ifndef MMS_ADDRESS_H
#define MMS_ADDRESS_H

#include <string>

#include "mms_decode_buffer.h"
#include "mms_encode_buffer.h"
#include "mms_codec_type.h"

namespace OHOS {
namespace Telephony {
class MmsAddress {
public:
    using MmsAddressType = enum {
        ADDRESS_TYPE_UNKNOWN = 0,
        ADDRESS_TYPE_PLMN = 1,
        ADDRESS_TYPE_IPV4 = 2,
        ADDRESS_TYPE_IPV6 = 3,
        ADDRESS_TYPE_EMAIL = 4,
    };

    /**
     * @brief Get the Address String
     * for example: +8610086/TYPE=PLMN
     * @brief Get the Address String
     * @return std::string
     */
    std::string GetAddressString();

    /**
     * @brief Get the Address Charset
     * for example: MmsCharSets::UTF_8
     * @return MmsCharSets
     */
    MmsCharSets GetAddressCharset();

    /**
     * @brief Get the Address Type
     * for example: MmsAddressType::ADDRESS_TYPE_PLMN
     * @return MmsAddressType
     */
    MmsAddressType GetAddressType();

    /**
     * @brief Set the Mms Address String
     * for example: +8610086/TYPE=PLMN
     * @param addressString
     * @param charset
     */
    void SetMmsAddressString(const std::string addressString, MmsCharSets charset = MmsCharSets::UTF_8);

    /**
     * @brief Construct a new Mms Address
     *
     * @param addressString for example: +8610086/TYPE=PLMN
     * @param charset defualt UTF_8
     */
    MmsAddress(const std::string addressString, MmsCharSets charset = MmsCharSets::UTF_8);

    /**
     * @brief Construct a new Mms Address
     *
     */
    MmsAddress();

    /**
     * @brief Destroy the Mms Address
     *
     */
    ~MmsAddress();
private:
    void CheckAddressType();

    std::string addressString_;
    MmsCharSets charset_ = MmsCharSets::UTF_8;
    MmsAddressType addressType_ = ADDRESS_TYPE_UNKNOWN;
};
} // namespace Telephony
} // namespace OHOS
#endif
