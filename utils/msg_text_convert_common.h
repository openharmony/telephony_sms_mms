/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#ifndef MSG_TEXT_CONVERT_COMMON_H
#define MSG_TEXT_CONVERT_COMMON_H

namespace OHOS {
namespace Telephony {
using WCHAR = unsigned long;
using MSG_CHAR_TYPE_T = unsigned char;
using MSG_LANGUAGE_ID_T = unsigned char;
using msg_encode_type_t = unsigned char;

enum MsgCharType { MSG_DEFAULT_CHAR = 0, MSG_GSM7EXT_CHAR, MSG_TURKISH_CHAR, MSG_SPANISH_CHAR, MSG_PORTUGUESE_CHAR };

enum MsgLanguageId {
    MSG_ID_RESERVED_LANG = 0,
    MSG_ID_TURKISH_LANG,
    MSG_ID_SPANISH_LANG,
    MSG_ID_PORTUGUESE_LANG,
    MSG_ID_BENGALI_LANG,
    MSG_ID_GUJARATI_LANG,
    MSG_ID_HINDI_LANG,
    MSG_ID_KANNADA_LANG,
    MSG_ID_MALAYALAM_LANG,
    MSG_ID_ORIYA_LANG,
    MSG_ID_PUNJABI_LANG,
    MSG_ID_TAMIL_LANG,
    MSG_ID_TELUGU_LANG,
    MSG_ID_URDU_LANG,
};

struct MsgLangInfo {
    bool bSingleShift;
    bool bLockingShift;
    MSG_LANGUAGE_ID_T singleLang;
    MSG_LANGUAGE_ID_T lockingLang;
};
} // namespace Telephony
} // namespace OHOS
#endif // MSG_TEXT_CONVERT_COMMON_H