/*
 * Copyright (C) 2021-2023 Huawei Device Co., Ltd.
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

#ifndef SMS_SEND_INDEXER_H
#define SMS_SEND_INDEXER_H

#include <vector>

#include "i_sms_service_interface.h"
#include "gsm_pdu_code_type.h"
#include "sms_base_message.h"
#include "sms_common.h"

namespace OHOS {
namespace Telephony {
class SmsSendIndexer {
public:
    SmsSendIndexer(const std::string &desAddr, const std::string &scAddr, const std::string &text,
        const sptr<ISendShortMessageCallback> &sendCallback,
        const sptr<IDeliveryShortMessageCallback> &deliveryCallback);
    SmsSendIndexer(const std::string &desAddr, const std::string &scAddr, int32_t port, const uint8_t *data,
        uint32_t dataLen, const sptr<ISendShortMessageCallback> &sendCallback,
        const sptr<IDeliveryShortMessageCallback> &deliveryCallback);
    ~SmsSendIndexer();
    uint8_t GetPsResendCount() const;
    void SetPsResendCount(uint8_t resendCount);
    uint8_t GetCsResendCount() const;
    void SetCsResendCount(uint8_t resendCount);
    uint8_t GetMsgRefId() const;
    void SetMsgRefId(uint8_t msgRefId);
    int64_t GetMsgRefId64Bit() const;
    void SetMsgRefId64Bit(int64_t msgRefId64Bit);
    const std::vector<uint8_t>& GetAckPdu() const;
    void SetAckPdu(const std::vector<uint8_t> &ackPdu);
    const std::vector<uint8_t>& GetData() const;
    void SetData(const std::vector<uint8_t> &data);
    uint8_t GetErrorCode() const;
    void SetErrorCode(uint8_t errorCode);
    bool GetIsText() const;
    std::string GetSmcaAddr() const;
    void SetSmcaAddr(const std::string &scAddr);
    std::string GetDestAddr() const;
    void SetDestAddr(const std::string &destAddr);
    int32_t GetDestPort() const;
    void SetDestPort(int32_t destPort);
    sptr<ISendShortMessageCallback> GetSendCallback() const;
    void SetSendCallback(const sptr<ISendShortMessageCallback> &sendCallback);
    sptr<IDeliveryShortMessageCallback> GetDeliveryCallback() const;
    void SetDeliveryCallback(const sptr<IDeliveryShortMessageCallback> &deliveryCallback);
    std::string GetText() const;
    void SetText(const std::string &text);
    uint8_t GetUnSentCellCount() const;
    void SetUnSentCellCount(const uint8_t unSentCellCount);
    std::shared_ptr<bool> GetHasCellFailed() const;
    void SetHasCellFailed(const std::shared_ptr<bool> &hasCellFailed);
    bool GetIsFailure() const;
    void SetIsFailure(bool isFailure);
    bool GetHasMore() const;
    void SetHasMore(bool hasMore);
    int64_t GetTimeStamp() const;
    void SetTimeStamp(int64_t timeStamp);
    void SetNetWorkType(NetWorkType netWorkType);
    NetWorkType GetNetWorkType() const;
    void SetEncodeSmca(const std::vector<uint8_t> &smca);
    const std::vector<uint8_t>& GetEncodeSmca() const;
    void SetEncodePdu(const std::vector<uint8_t> &pdu);
    const std::vector<uint8_t>& GetEncodePdu() const;
    void SetImsSmsForCdma(const bool isImsSms);
    const bool &IsImsSmsForCdma() const;
    void UpdatePduForResend();
    void SetEncodePdu(const std::vector<uint8_t> &&pdu);
    void SetEncodeSmca(const std::vector<uint8_t> &&smca);
    void SetAckPdu(const std::vector<uint8_t> &&ackPdu);
    void SetData(const std::vector<uint8_t> &&data);
    void SetDcs(enum DataCodingScheme dcs);
    enum DataCodingScheme GetDcs() const;
    bool GetIsConcat() const;
    void SetIsConcat(bool concat);
    void SetSmsConcat(const SmsConcat &smsConcat);
    SmsConcat GetSmsConcat() const;
    uint8_t GetLangId() const;
    void SetLangId(uint8_t langId);
    uint16_t GetMsgId() const;
    void SetMsgId(uint16_t msgId);
    int32_t GetDataBaseId() const;
    void SetDataBaseId(int32_t msgId);
    bool GetIsMmsApp() const;
    void SetIsMmsApp(bool isMmsApp);

private:
    std::vector<uint8_t> pdu_;
    std::vector<uint8_t> smca_;
    std::vector<uint8_t> ackPdu_;
    std::vector<uint8_t> data_;
    uint8_t errorCode_ = 0;
    std::string text_;
    NetWorkType netWorkType_ = NetWorkType::NET_TYPE_UNKNOWN;
    std::string scAddr_;
    std::string destAddr_;
    int32_t destPort_ = 0;
    bool isCdmaIms_ = false;
    bool hasMore_ = false;
    long timeStamp_ = 0;
    uint8_t csResendCount_ = 0;
    uint8_t psResendCount_ = 0;
    uint8_t msgRefId_ = 0;
    int64_t msgRefId64Bit_ = 0;
    uint8_t unSentCellCount_ = 0;
    std::shared_ptr<bool> hasCellFailed_ = nullptr;
    sptr<ISendShortMessageCallback> sendCallback_ = nullptr;
    sptr<IDeliveryShortMessageCallback> deliveryCallback_ = nullptr;
    bool isFailure_ = false;
    enum DataCodingScheme dcs_ = DATA_CODING_7BIT;
    bool isConcat_ = false;
    SmsConcat smsConcat_ = {0, 0, 0, false};
    uint8_t langId_ = 0;
    bool isText_ = false;
    bool isMmsApp_ = true;
    uint16_t msgId_ = 0;
    int32_t dataBaseId_ = 0;
};
} // namespace Telephony
} // namespace OHOS
#endif