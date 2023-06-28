/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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

#ifndef GSM_CB_PDU_DECODE_BUFFER_H
#define GSM_CB_PDU_DECODE_BUFFER_H

#include <memory>

namespace OHOS {
namespace Telephony {
class GsmCbPduDecodeBuffer {
public:
    explicit GsmCbPduDecodeBuffer(uint32_t len);
    virtual ~GsmCbPduDecodeBuffer();
    bool GetOneByte(uint8_t &oneByte);
    uint32_t GetCurPosition();
    uint32_t GetSize();
    void IncreasePointer(uint32_t offset);
    bool PickOneByte(uint8_t &oneByte);
    void SetPointer(uint32_t offset);

public:
    std::unique_ptr<char[]> pduBuffer_ { nullptr };

private:
    uint32_t curPosition_ = 0;
    uint32_t totolLength_ = 0;
};
} // namespace Telephony
} // namespace OHOS
#endif
