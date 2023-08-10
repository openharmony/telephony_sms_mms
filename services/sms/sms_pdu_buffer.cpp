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

#include "sms_pdu_buffer.h"

#include "securec.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
static constexpr uint16_t PDU_BUFFER_MAX_SIZE = 0xFF;
static constexpr uint16_t PDU_BUFFER_MIN_SIZE = 2;

SmsPduBuffer::~SmsPduBuffer()
{
    if (data_ != nullptr) {
        data_.reset();
    }
}

bool SmsPduBuffer::IsEmpty()
{
    return data_ == nullptr;
}

uint16_t SmsPduBuffer::GetIndex()
{
    return index_;
}

uint16_t SmsPduBuffer::GetSize()
{
    return length_;
}

bool SmsPduBuffer::SetIndex(uint16_t index)
{
    if (bitIndex_ != BIT0) {
        TELEPHONY_LOGE("buffer in bit mode");
        return false;
    }
    if (index > length_) {
        TELEPHONY_LOGE("index over buffer");
        return false;
    }

    index_ = index;
    return true;
}

uint16_t SmsPduBuffer::MoveForward(uint16_t len)
{
    if (bitIndex_ != BIT0) {
        TELEPHONY_LOGE("buffer in bit mode");
        return index_;
    }
    if (index_ + len > length_) {
        TELEPHONY_LOGE("move over buffer");
        return index_;
    }

    uint16_t pos = index_;
    index_ += len;
    return pos;
}

uint16_t SmsPduBuffer::MoveBack(uint16_t len)
{
    if (bitIndex_ != BIT0) {
        TELEPHONY_LOGE("buffer in bit mode");
        return index_;
    }
    if (index_ < len) {
        TELEPHONY_LOGE("back over buffer");
        return index_;
    }

    uint16_t pos = index_;
    index_ -= len;
    return pos;
}

uint16_t SmsPduBuffer::SkipBits()
{
    if (bitIndex_ > BIT0) {
        index_++;
        bitIndex_ = BIT0;
    }
    return index_;
}

std::unique_ptr<std::vector<uint8_t>> SmsPduBuffer::GetPduBuffer()
{
    if (data_ == nullptr || index_ == 0) {
        TELEPHONY_LOGE("data is null");
        return nullptr;
    }
    if (bitIndex_ != BIT0) {
        TELEPHONY_LOGE("buffer in bit mode");
        return nullptr;
    }

    if (index_ >= length_) {
        TELEPHONY_LOGE("data error.");
        return nullptr;
    }

    std::unique_ptr<std::vector<uint8_t>> data =
        std::make_unique<std::vector<uint8_t>>(data_.get(), data_.get() + index_);
    if (data == nullptr) {
        TELEPHONY_LOGE("make unique error");
        return nullptr;
    }
    return data;
}

SmsReadBuffer::SmsReadBuffer(const std::string &hex)
{
    size_t len = hex.length();
    if (len < PDU_BUFFER_MIN_SIZE || len > PDU_BUFFER_MAX_SIZE + 1) {
        TELEPHONY_LOGE("invalid PDU");
        return;
    }

    data_ = std::make_unique<uint8_t[]>(len);
    if (data_ == nullptr) {
        TELEPHONY_LOGE("make_unique error");
        return;
    }
    if (memset_s(data_.get(), len, 0x00, len) != EOK) {
        TELEPHONY_LOGE("memset_s error");
        data_.reset();
        data_ = nullptr;
        return;
    }
    if (memcpy_s(data_.get(), len, hex.c_str(), len) != EOK) {
        TELEPHONY_LOGE("memcpy_s error");
        data_.reset();
        data_ = nullptr;
        return;
    }
    length_ = len;
}

bool SmsReadBuffer::ReadByte(uint8_t &v)
{
    if (data_ == nullptr) {
        TELEPHONY_LOGE("data is null");
        return false;
    }
    if (bitIndex_ != BIT0) {
        return ReadBits(v, BIT8);
    }

    if (index_ == length_) {
        TELEPHONY_LOGW("read over buffer");
        return false;
    }

    v = data_[index_++];
    return true;
}

bool SmsReadBuffer::PickOneByte(uint8_t &v)
{
    if (data_ == nullptr || index_ >= length_) {
        TELEPHONY_LOGE("peek one byte fail.");
        return false;
    }
    v = data_[index_];
    return true;
}

bool SmsReadBuffer::PickOneByteFromIndex(uint16_t index, uint8_t &v)
{
    if (data_ == nullptr || length_ == 0 || index >= length_) {
        TELEPHONY_LOGE("peek index byte fail.");
        return false;
    }
    v = data_[index];
    return true;
}

bool SmsReadBuffer::ReadWord(uint16_t &v)
{
    if (data_ != nullptr && index_ < (length_ - 1) && bitIndex_ == BIT0) {
        v = data_[index_++];
        v = (v << BIT8) | data_[index_++];
        return true;
    }
    TELEPHONY_LOGE("read word error");
    return false;
}

bool SmsReadBuffer::ReadBits(uint8_t &v, uint8_t l)
{
    if (data_ == nullptr) {
        TELEPHONY_LOGE("data is null");
        return false;
    }
    if (l == BIT0 || l > BIT8) {
        TELEPHONY_LOGE("read bits : invalid length");
        return false;
    }

    if (bitIndex_ + l <= BIT8) {
        if (index_ == length_) {
            TELEPHONY_LOGE("read bits over buffer");
            return false;
        }
        v = data_[index_] << bitIndex_;
        v = v >> (BIT8 - l);
        bitIndex_ += l;
        if (bitIndex_ == BIT8) {
            bitIndex_ = BIT0;
            index_++;
        }
    } else {
        if (index_ == length_ - 1) {
            TELEPHONY_LOGE("read bits over buffer");
            return false;
        }
        v = data_[index_++] << bitIndex_;
        bitIndex_ = l + bitIndex_ - BIT8;
        v = (v >> (BIT8 - l)) | (data_[index_] >> (BIT8 - bitIndex_));
    }
    return true;
}

SmsWriteBuffer::SmsWriteBuffer()
{
    length_ = PDU_BUFFER_MAX_SIZE + 1;
    data_ = std::make_unique<uint8_t[]>(length_);
    if (data_ == nullptr) {
        TELEPHONY_LOGE("make_unique create data error");
        length_ = 0;
        return;
    }
}

bool SmsWriteBuffer::WriteByte(uint8_t v)
{
    if (bitIndex_ != BIT0) {
        return WriteBits(v, BIT8);
    }
    if (data_ != nullptr && index_ < length_) {
        data_[index_++] = v;
        return true;
    }
    TELEPHONY_LOGE("write byte error");
    return false;
}

bool SmsWriteBuffer::WriteWord(uint16_t v)
{
    if (data_ != nullptr && index_ < (length_ - 1) && bitIndex_ == BIT0) {
        data_[index_++] = (0xff00 & v) >> BIT8;
        data_[index_++] = (0x00ff & v);
        return true;
    }
    TELEPHONY_LOGE("write word error");
    return false;
}

bool SmsWriteBuffer::WriteBits(uint8_t v, uint8_t l)
{
    if (data_ == nullptr) {
        TELEPHONY_LOGE("data is null");
        return false;
    }
    if (l == BIT0 || l > BIT8) {
        TELEPHONY_LOGE("write bits : invalid length");
        return false;
    }

    if (bitIndex_ + l <= BIT8) {
        if (index_ == length_) {
            TELEPHONY_LOGE("write bits over buffer[%d]", l);
            return false;
        }
        data_[index_] |= v << (BIT8 - bitIndex_ - l);
        bitIndex_ += l;
        if (bitIndex_ == BIT8) {
            bitIndex_ = BIT0;
            index_++;
        }
    } else {
        if (index_ == length_ - 1) {
            TELEPHONY_LOGE("write bits over buffer[%d]", l);
            return false;
        }
        data_[index_++] |= v >> (l + bitIndex_ - BIT8);
        bitIndex_ = l + bitIndex_ - BIT8;
        data_[index_] |= v << (BIT8 - bitIndex_);
    }
    return true;
}

bool SmsWriteBuffer::InsertByte(uint8_t v, uint16_t index)
{
    if (data_ != nullptr && index < length_ && bitIndex_ == BIT0) {
        data_[index] = v;
        return true;
    }
    TELEPHONY_LOGE("insert byte error");
    return false;
}

bool SmsWriteBuffer::GetTopValue(uint8_t &oneByte)
{
    if (data_ == nullptr || length_ == 0 || index_ >= length_) {
        TELEPHONY_LOGE("buffer error");
        return false;
    }
    oneByte = data_[index_];
    return true;
}

bool SmsWriteBuffer::GetValueFromIndex(uint16_t index, uint8_t &v)
{
    if (data_ == nullptr || length_ == 0 || index >= length_) {
        TELEPHONY_LOGE("buffer error");
        return false;
    }
    v = data_[index];
    return true;
}
} // namespace Telephony
} // namespace OHOS
