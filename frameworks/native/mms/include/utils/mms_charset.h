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

#ifndef MMS_CHARSET_H
#define MMS_CHARSET_H

#include <map>
#include <stdint.h>

#include "__functional_base"
#include "iosfwd"
#include "singleton.h"
#include "string"

namespace OHOS {
namespace Telephony {
class MmsCharSet {
DECLARE_DELAYED_SINGLETON(MmsCharSet)
public:
    bool GetCharSetIntFromString(uint32_t &charSet, const std::string &strCharSet);
    bool GetCharSetStrFromInt(std::string &strCharSet, uint32_t charSet);

private:
    std::map<std::string, uint32_t> mapCharSet_;
};
} // namespace Telephony
} // namespace OHOS
#endif /* CHARSET_H */
