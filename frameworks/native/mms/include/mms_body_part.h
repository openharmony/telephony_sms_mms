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

#ifndef MMS_BODY_PART_H
#define MMS_BODY_PART_H

#include "mms_attachment.h"
#include "mms_content_type.h"
#include "mms_body_part_header.h"

namespace OHOS {
namespace Telephony {
class MmsBodyPart {
public:
    MmsBodyPart();
    MmsBodyPart(const MmsBodyPart &obj);
    MmsBodyPart &operator=(const MmsBodyPart &srcBodyPart);
    ~MmsBodyPart();
    void DumpMmsBodyPart();
    bool DecodePartHeader(MmsDecodeBuffer &decodeBuffer, uint32_t headerLen);
    bool DecodePart(MmsDecodeBuffer &decodeBuffer); // main entry
    bool DecodePartBody(MmsDecodeBuffer &decodeBuffer, uint32_t bodyLength);
    bool EncodeMmsBodyPart(MmsEncodeBuffer &encodeBuffer);
    bool SetAttachment(MmsAttachment &attachment);

public:
    bool IsSmilFile();
    void SetSmilFile(bool isSmil);
    bool SetContentType(std::string strContentType);
    bool GetContentType(std::string &strContentType);
    bool SetContentId(std::string contentId);
    bool GetContentId(std::string &contentId);
    bool SetContentLocation(std::string contentLocation);
    bool GetContentLocation(std::string &contentLocation);
    bool SetContentDisposition(std::string contentDisposition);
    bool GetContentDisposition(std::string &contentDisposition);

    void SetFileName(std::string fileName);
    MmsContentType &GetContentType();
    MmsBodyPartHeader &GetPartHeader();
    std::string GetPartFileName();
    std::unique_ptr<char[]> ReadBodyPartBuffer(uint32_t &len);

private:
    void AssignBodyPart(const MmsBodyPart &obj);
    bool WriteBodyFromAttachmentBuffer(MmsAttachment &attachment);
    bool WriteBodyFromFile(std::string path);
    void DecodeSetFileName();

    bool isSmilFile_ = false;
    std::string strFileName_ = "";
    uint32_t headerLen_ = 0;
    uint32_t bodyLen_ = 0;
    MmsContentType bodyPartContentType_;
    MmsBodyPartHeader mmsBodyPartHeader_;
    std::unique_ptr<char[]> pbodyPartBuffer_ = nullptr;

    static constexpr uint32_t MAX_MMS_MSG_PART_LEN = 300 * 1024;
    const std::string ENCODE_BINARY = "binary";
    const std::string ENCODE_BASE64 = "base64";
    const std::string ENCODE_QUOTED_PRINTABLE = "quoted-printable";
};
} // namespace Telephony
} // namespace OHOS
#endif
