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
#include "mms_content_param.h"

#include "telephony_log_wrapper.h"

#include "mms_content_type.h"
#include "mms_decode_buffer.h"
#include "mms_charset.h"

namespace OHOS {
namespace Telephony {
void MmsContentParam::DumpContentParam()
{
    std::string charSetString;
    auto charSet = DelayedSingleton<MmsCharSet>::GetInstance();
    if (charSet == nullptr || (!charSet->GetCharSetStrFromInt(charSetString, charset_))) {
        charSetString = "UTF-8";
    }
    TELEPHONY_LOGI("Param Charset: %{public}s", charSetString.c_str());
    TELEPHONY_LOGI("Param Type: %{public}s", type_.c_str());
    for (auto it = textMap_.begin(); it != textMap_.end(); it++) {
        TELEPHONY_LOGI("Param textMap : %{public}s", it->second.c_str());
    }
}

MmsContentParam &MmsContentParam::operator=(const MmsContentParam &srcContentParam)
{
    if (this != &srcContentParam) {
        charset_ = srcContentParam.charset_;
        type_ = srcContentParam.type_;
        textMap_ = srcContentParam.textMap_;
    }
    return *this;
}

void MmsContentParam::SetCharSet(uint32_t charset)
{
    charset_ = charset;
}

void MmsContentParam::SetType(std::string type)
{
    type_ = type;
}

uint32_t MmsContentParam::GetCharSet()
{
    return charset_;
}

std::string MmsContentParam::GetType()
{
    return type_;
}

void MmsContentParam::GetFileName(std::string &fileName)
{
    fileName = "";
    uint8_t fieldCode = static_cast<uint8_t>(ContentParam::CT_P_NAME);
    if (textMap_.find(fieldCode) != textMap_.end()) {
        fileName = textMap_[fieldCode];
    }
}

void MmsContentParam::SetFileName(std::string fileName)
{
    if (fileName.empty()) {
        return;
    }
    uint8_t fieldCode = static_cast<uint8_t>(ContentParam::CT_P_NAME);
    textMap_.insert(std::make_pair(fieldCode, fileName));
}

void MmsContentParam::GetStart(std::string &start)
{
    start = "";
    uint8_t fieldCode = static_cast<uint8_t>(ContentParam::CT_P_START);
    if (textMap_.find(fieldCode) != textMap_.end()) {
        start = textMap_[fieldCode];
    }
}

void MmsContentParam::SetStart(std::string start)
{
    if (start.empty()) {
        return;
    }
    uint8_t fieldCode = static_cast<uint8_t>(ContentParam::CT_P_START);
    textMap_.insert(std::make_pair(fieldCode, start));
}

bool MmsContentParam::AddNormalField(uint8_t field, std::string value)
{
    if (textMap_.find(field) != textMap_.end()) {
        TELEPHONY_LOGE(" add normal fail.");
        return false;
    }
    textMap_.insert(std::make_pair(field, value));
    return true;
}

bool MmsContentParam::GetNormalField(uint8_t field, std::string &value)
{
    value = "";
    if (textMap_.find(field) != textMap_.end()) {
        value = textMap_[field];
    }
    return true;
}

std::map<uint8_t, std::string> &MmsContentParam::GetParamMap()
{
    return textMap_;
}
} // namespace Telephony
} // namespace OHOS
