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

#include "mms_base64.h"

#include <glib.h>
#include <glib/gi18n.h>

#include "securec.h"

namespace OHOS {
namespace Telephony {
std::string MmsBase64::Encode(const std::string src)
{
    gchar *encode_data = g_base64_encode((guchar *)src.data(), src.length());
    if (encode_data == nullptr) {
        return "";
    }
    gsize out_len = 0;
    out_len = strlen(encode_data);
    std::string dest((char *)encode_data, out_len);
    if (encode_data != nullptr) {
        g_free(encode_data);
    }
    return dest;
}

std::string MmsBase64::Decode(const std::string src)
{
    gsize out_len = 0;
    char *decode_data = (char *)g_base64_decode(src.data(), &out_len);
    if (decode_data == nullptr) {
        return "";
    }
    std::string dest(decode_data, out_len);
    if (decode_data != nullptr) {
        g_free(decode_data);
    }
    return dest;
}
} // namespace Telephony
} // namespace OHOS
