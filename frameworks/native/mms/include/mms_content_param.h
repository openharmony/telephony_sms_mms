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

#ifndef MMS_CONTENT_PARAM_H
#define MMS_CONTENT_PARAM_H

#include <map>

#include "mms_decode_buffer.h"
#include "mms_encode_buffer.h"

namespace OHOS {
namespace Telephony {
/**
 * wap-230-wsp-20010705-a   section:Table 38. Well-Known Parameter Assignments
 */
enum class ContentParam {
    // Encoding Version 1.1
    CT_P_Q = 0x80,                  /* Q */
    CT_P_CHARSET = 0x81,            /* Charset */
    CT_P_LEVEL = 0x82,              /* Level */
    CT_P_TYPE = 0x83,               /* Type */
    CT_P_NAME = 0x85,               /* Name */
    CT_P_FILENAME = 0x86,           /* Filename */
    CT_P_DIFFERENCES = 0x87,        /* Differences */
    CT_P_PADDING = 0x88,            /* Padding */
    // Encoding Version 1.2
    CT_P_TYPE_STRING = 0x89,        /* Type */
    CT_P_START = 0x8A,              /* Start Text-string */
    CT_P_START_INFO = 0x8B,         /* Start-info  Text-string */
    // Encoding Version 1.3
    CT_P_COMMENT = 0x8C,            /* Comment */
    CT_P_DOMAIN = 0x8D,             /* Domain */
    CT_P_MAX_AGE = 0x8E,            /* Max-Age */
    CT_P_PATH = 0x8F,               /* Path */
    CT_P_SECURE = 0x90,             /* Secure */
    // Encoding Version 1.4
    CT_P_SEC = 0x91,                /* SEC */
    CT_P_MAC = 0x92,                /* MAC */
    CT_P_CREATION_DATE = 0x93,      /* Creation-date */
    CT_P_MODIFICATION_DATE = 0x94,  /* Modification-date */
    CT_P_READ_DATE = 0x95,          /* Read-date */
    CT_P_SIZE = 0x96,               /* Size */
    CT_P_NAME_VALUE = 0x97,         /* Name */
    CT_P_FILENAME_VALUE = 0x98,     /* Filename */
    CT_P_START_VALUE = 0x99,        /* Start Text-value */
    CT_P_START_INFO_VALUE = 0x9A,   /* Start-info Text-value */
    CT_P_COMMENT_VALUE = 0x9B,      /* Comment */
    CT_P_DOMAIN_VALUE = 0x9C,       /* Domain */
    CT_P_PATH_VALUE = 0x9D,         /* Path */
};

class MmsContentParam {
public:
    MmsContentParam() = default;
    ~MmsContentParam() = default;
    void DumpContentParam();
    MmsContentParam& operator=(const MmsContentParam &srcContentParam);
    uint32_t GetCharSet();
    void SetCharSet(uint32_t charset);
    std::string GetType();
    void SetType(std::string type);
    void GetFileName(std::string &fileName);
    void SetFileName(std::string fileName);
    void GetStart(std::string &start);
    void SetStart(std::string start);
    bool AddNormalField(uint8_t field, std::string value);
    bool GetNormalField(uint8_t field, std::string &value);
    std::map<uint8_t, std::string> &GetParamMap();
private:
    uint32_t charset_ = 0;
    std::string type_ = "";
    std::map<uint8_t, std::string> textMap_;
};
} // namespace Telephony
} // namespace OHOS
#endif
