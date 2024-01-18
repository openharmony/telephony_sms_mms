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

#ifndef SMS_RECEIVE_INDEXER_H
#define SMS_RECEIVE_INDEXER_H

#include <string>
#include <vector>

namespace OHOS {
namespace Telephony {
class SmsReceiveIndexer {
public:
    SmsReceiveIndexer();
    virtual ~SmsReceiveIndexer() = default;
    SmsReceiveIndexer(const std::vector<uint8_t> &pdu, int64_t timestamp, int16_t destPort, bool isCdma,
        const std::string &address, const std::string &displayAddress, uint16_t msgRefId, uint16_t msgSeqId,
        uint16_t msgCount, bool isCdmaWapPdu, const std::string &messageBody);
    SmsReceiveIndexer(const std::vector<uint8_t> &pdu, int64_t timestamp, int16_t destPort, bool isCdma,
        bool isCdmaWapPdu, const std::string &address, const std::string &displayAddress,
        const std::string &messageBody);
    const std::vector<uint8_t>& GetPdu() const;
    void SetPdu(const std::vector<uint8_t> &pdu);
    int64_t GetTimestamp() const;
    void SetTimestamp(int64_t timestamp);
    int16_t GetDestPort() const;
    void SetDestPort(int16_t destPort);
    bool GetIsCdma() const;
    void SetIsCdma(bool isCdma);
    bool GetIsCdmaWapPdu() const;
    void SetIsCdmaWapPdu(bool isCdmaWapPdu);
    std::string GetVisibleMessageBody() const;
    void SetVisibleMessageBody(const std::string &messageBody);
    std::string GetOriginatingAddress() const;
    void SetOriginatingAddress(const std::string &address);
    uint16_t GetMsgRefId() const;
    void SetMsgRefId(uint16_t msgRefId);
    uint16_t GetMsgSeqId() const;
    void SetMsgSeqId(uint16_t msgSeqId);
    uint16_t GetDataBaseId() const;
    void SetDataBaseId(uint16_t dataBaseId);
    uint16_t GetMsgCount() const;
    void SetMsgCount(uint16_t msgCount);
    std::string GetEraseRefId() const;
    void SetEraseRefId(const std::string &eraseRefId);
    std::string GetVisibleAddress() const;
    void SetVisibleAddress(const std::string &visibleAddress);
    bool GetIsText() const;
    bool GetIsWapPushMsg() const;
    void SetPdu(const std::vector<uint8_t> &&pdu);
    bool IsSingleMsg() const;
    std::string GetRawUserData() const;
    std::string GetRawWapPushUserData() const;
    void SetRawUserData(const std::string &rawUserData);
    void SetRawWapPushUserData(const std::string &rawWapPushUserData);

private:
    std::vector<uint8_t> pdu_;
    int64_t timestamp_;
    int16_t destPort_;
    bool isCdma_ = false;
    bool isCdmaWapPdu_ = false;
    std::string visibleMessageBody_;
    std::string originatingAddress_;
    uint16_t msgRefId_ = 0;
    uint16_t msgSeqId_ = 0;
    uint16_t dataBaseId_ = 0;
    uint16_t msgCount_ = 0;
    std::string eraseRefId_;
    std::string visibleAddress_;
    std::string rawUserData_;
    std::string rawWapPushUserData_;
};
} // namespace Telephony
} // namespace OHOS
#endif