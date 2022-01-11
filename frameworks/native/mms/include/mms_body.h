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

#ifndef MMS_BODY_H
#define MMS_BODY_H

#include "mms_decode_buffer.h"
#include "mms_encode_buffer.h"
#include "mms_body_part.h"
#include "mms_header.h"

#include <string>
#include <vector>

namespace OHOS {
namespace Telephony {
class MmsBody {
public:
    MmsBody();
    ~MmsBody() = default;
    void DumpMmsBody();
    bool DecodeMmsBody(MmsDecodeBuffer &decodeBuffer, MmsHeader &header);
    bool DecodeMultipart(MmsDecodeBuffer &decodeBuffer);
    bool EncodeMmsBody(MmsEncodeBuffer &encodeBuffer);
    bool EncodeMmsHeaderContentType(MmsHeader &mmsHeader, MmsEncodeBuffer &encodeBuffer);

    uint32_t GetBodyPartCount();
    bool IsBodyPartExist(MmsBodyPart &bodyPart);
    bool AddMmsBodyPart(MmsBodyPart &bodyPart);
    void GetMmsBodyPart(std::vector<MmsBodyPart> &parts);

private:
    bool IsContentLocationPartExist(std::string contentLocation);
    bool IsContentIdPartExist(std::string contentId);

    bool bHaveSmilPart_ = false; // presentation body is exist
    uint32_t bodyEntrys_;
    std::string scPresentationBody_;
    std::vector<MmsBodyPart> mmsBodyParts_;
};
} // namespace Telephony
} // namespace OHOS
#endif
