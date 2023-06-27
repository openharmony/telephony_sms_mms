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

#include "gsm_cb_pdu_decode_buffer.h"

#include "securec.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
using namespace std;

GsmCbPduDecodeBuffer::GsmCbPduDecodeBuffer(uint8_t len)
{
    pduBuffer_ = std::make_unique<char[]>(len);
    if (pduBuffer_ == nullptr) {
        TELEPHONY_LOGE("pduBuffer_ nullptr error");
        totolLength_ = 0;
        return;
    }
    totolLength_ = len;
}

GsmCbPduDecodeBuffer::~GsmCbPduDecodeBuffer()
{
    if (pduBuffer_ != nullptr) {
        pduBuffer_.reset();
    }
}

bool GsmCbPduDecodeBuffer::PickOneByte(uint8_t &oneByte)
{
    if (pduBuffer_ == nullptr || curPosition_ >= totolLength_) {
        TELEPHONY_LOGE("curPosition_ over size.");
        return false;
    }
    oneByte = pduBuffer_[curPosition_];
    return true;
}

bool GsmCbPduDecodeBuffer::GetOneByte(uint8_t &oneByte)
{
    if (pduBuffer_ == nullptr || curPosition_ >= totolLength_) {
        return false;
    }
    oneByte = pduBuffer_[curPosition_++];
    return true;
}

void GsmCbPduDecodeBuffer::IncreasePointer(uint32_t offset)
{
    curPosition_ += offset;
}

void GsmCbPduDecodeBuffer::SetPointer(uint32_t offset)
{
    curPosition_ = offset;
}

uint32_t GsmCbPduDecodeBuffer::GetCurPosition()
{
    return curPosition_;
}

uint32_t GsmCbPduDecodeBuffer::GetSize()
{
    return totolLength_;
}
} // namespace Telephony
} // namespace OHOS
