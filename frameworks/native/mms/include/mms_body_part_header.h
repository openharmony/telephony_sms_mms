/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef MMS_BODY_PART_HEADER_H
#define MMS_BODY_PART_HEADER_H

#include "mms_content_type.h"

#include <map>

namespace OHOS {
namespace Telephony {
enum class MmsHeaderParam {
    P_CONTENT_LOCATION_V1 = 0x04,
    P_CONTENT_LOCATION_V2 = 0x0E,
    P_CONTENT_ID = 0x40,
    P_CONTENT_DISPOSITION_V1 = 0x2E,
    P_CONTENT_DISPOSITION_V2 = 0x45
};

enum class MmsDispositonParam {
    P_DISPOSITION_FROM_DATA = 0x80,
    P_DISPOSITION_ATTACHMENT = 0x81,
    P_DISPOSITION_INLINE = 0x82
};

class MmsBodyPartHeader {
public:
    MmsBodyPartHeader() = default;
    ~MmsBodyPartHeader() = default;
    MmsBodyPartHeader(const MmsBodyPartHeader &obj);
    MmsBodyPartHeader &operator=(const MmsBodyPartHeader &srcHeader);
    void DumpBodyPartHeader();
    bool DecodeWellKnownHeader(MmsDecodeBuffer &decodeBuffer, uint32_t &headerLen);
    bool DecodeApplicationHeader(MmsDecodeBuffer &decodeBuffer, uint32_t &headerLen);
    bool DecodeContentLocation(MmsDecodeBuffer &decodeBuffer, uint32_t &Len);
    bool DecodeContentId(MmsDecodeBuffer &decodeBuffer, uint32_t &Len);
    bool DecodeContentDisposition(MmsDecodeBuffer &decodeBuffer, uint32_t &Len);
    bool DecodeDispositionParameter(MmsDecodeBuffer &decodeBuffer, uint32_t dispLen, uint32_t beginPos);

    bool EncodeContentLocation(MmsEncodeBuffer &encodeBuffer);
    bool EncodeContentId(MmsEncodeBuffer &encodeBuffer);
    bool EncodeContentDisposition(MmsEncodeBuffer &encodeBuffer);
    bool EncodeContentTransferEncoding(MmsEncodeBuffer &encodeBuffer);
    bool EncodeMmsBodyPartHeader(MmsEncodeBuffer &encodeBuffer);

public:
    bool GetContentId(std::string &contentId);
    bool GetContentLocation(std::string &contentLocation);
    bool GetContentDisposition(std::string &contentDisposition);
    bool GetContentTransferEncoding(std::string &contentTransferEncoding);

    bool SetContentId(std::string contentId);
    bool SetContentLocation(std::string contentLocation);
    bool SetContentDisposition(std::string contentDisposition);
    bool SetContentTransferEncoding(std::string contentTransferEncoding);

private:
    const std::string DISPOSITION_FROM_DATA = "from-data";
    const std::string DISPOSITION_ATTACHMENT = "attachment";
    const std::string DISPOSITION_INLINE = "inline";

    std::string strContentTransferEncoding_ = "binary";
    std::string strFileName_ = "";
    std::string strDisposition_ = "";
    std::string strContentID_ = "";
    std::string strContentLocation_ = "";
    std::map<std::string, std::string> textMap_;
};
} // namespace Telephony
} // namespace OHOS
#endif
