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

#ifndef MMS_HEADER_CATEG_H
#define MMS_HEADER_CATEG_H

#include <map>
#include <stdint.h>

#include "__functional_base"
#include "mms_codec_type.h"

namespace OHOS {
namespace Telephony {
class MmsHeaderCateg {
public:
    MmsHeaderCateg();
    ~MmsHeaderCateg();
    MmsFieldValueType FindFieldDes(uint8_t fieldId);
    MmsFieldOptType FindSendReqOptType(uint8_t fieldId);
    MmsFieldOptType FindSendConfOptType(uint8_t fieldId);
    bool CheckIsValueLen(uint8_t fieldId);
private:
    void InitFieldDesMap();
    void InitFieldDesV10();
    void InitFieldDesV11();
    void InitFieldDesV12();
    void InitSendReqMap();
    void InitSendConfMap();

    std::map<uint8_t, MmsFieldValueType> mmsFieldDesMap_;

    std::map<uint8_t, MmsFieldOptType> mmsSendReqOptMap_;
    std::map<uint8_t, MmsFieldOptType> mmsSendConfOptMap_;
};
} // namespace Telephony
} // namespace OHOS
#endif
