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

#ifndef MMS_BUFFER_H
#define MMS_BUFFER_H

#include <string>
#include <vector>
#include <memory>

namespace OHOS {
namespace Telephony {
class MmsBuffer {
public:
    MmsBuffer();
    virtual ~MmsBuffer();
    virtual std::unique_ptr<char[]> ReadDataBuffer(uint32_t desLen);
    virtual std::unique_ptr<char[]> ReadDataBuffer(uint32_t offset, uint32_t desLen);
    virtual bool WriteDataBuffer(std::unique_ptr<char[]> inBuff, uint32_t len);
    virtual bool WriteBufferFromFile(std::string &strPathName);
    virtual uint32_t GetCurPosition() const;
    virtual uint32_t GetSize() const;
protected:
    std::unique_ptr<char[]> pduBuffer_;
    uint32_t curPosition_ = 0;
    uint32_t totolLength_ = 0;
    uint32_t savePosition_ = 0;
    const uint32_t CODE_BUFFER_MAX_SIZE = 300 * 1024;
};
} // namespace Telephony
} // namespace OHOS
#endif