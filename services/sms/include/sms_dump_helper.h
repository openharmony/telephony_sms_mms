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

#ifndef SMS_DUMP_HELPER_H
#define SMS_DUMP_HELPER_H

#include <vector>
#include <string>

namespace OHOS {
namespace Telephony {
class SmsDumpHelper {
public:
    SmsDumpHelper();
    ~SmsDumpHelper() = default;
    bool Dump(const std::vector<std::string> &args, std::string &result) const;

private:
    void ShowHelp(std::string &result) const;
    void ShowSmsInfo(std::string &result) const;
    bool WhetherHasSimCard(const int32_t slotId) const;
};
} // namespace Telephony
} // namespace OHOS
#endif // SMS_DUMP_HELPER_H
