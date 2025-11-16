/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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

#ifndef SMS_SHORT_CODE_MATCHER_H
#define SMS_SHORT_CODE_MATCHER_H

#include <cstdint>
#include <nlohmann/json.hpp>
#include <string>
#include <unordered_map>

namespace OHOS {
namespace Telephony {
enum class SmsShortCodeType {
    SMS_SHORT_CODE_TYPE_UNKNOWN = -1,
    SMS_SHORT_CODE_TYPE_NOT_SHORT_CODE,
    SMS_SHORT_CODE_TYPE_FREE,
    SMS_SHORT_CODE_TYPE_STANDARD,
    SMS_SHORT_CODE_TYPE_PREMIUM,
    SMS_SHORT_CODE_TYPE_POSSIBLE_PREMIUM,
};

enum class PremiumSmsType {
    UNKNOWN = -1,
    NOT_PREMIUM,
    PREMIUM_OR_POSSIBLE_PREMIUM,
};

struct ShortCodeRule {
    std::string countryCode;
    std::string pattern;
    std::vector<std::string> premiumCodes;
    std::vector<std::string> standardCodes;
    std::vector<std::string> freeCodes;
};

class SmsShortCodeMatcher {
public:
    SmsShortCodeMatcher();
    PremiumSmsType GetPremiumSmsType(const int32_t &slotId, const std::string &desAddr);

private:
    std::string RemovePlusSign(const std::string &addr);
    void ParseCodeArray(const nlohmann::json &jsonArray, std::vector<std::string> &result);
    bool MatchesRegexList(const std::string &str, const std::vector<std::string> &regexList);
    bool LoadShortCodeRulesFromJson(const std::string &jsonPath);
    SmsShortCodeType MatchShortCodeType(const std::string &countryCode, const std::string &desAddr);
    void GetCountryCode(const int32_t &slotId, std::string &countryCode);
    void GetCountryCodeFromNetwork(std::u16string &countryCode16);
    std::unordered_map<std::string, ShortCodeRule> countryShortCodeRules_;
    bool loadFileSuccess_ = false;
};
} // namespace Telephony
} // namespace OHOS
#endif