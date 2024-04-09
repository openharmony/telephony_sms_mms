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

#ifndef SMS_SENDER_H
#define SMS_SENDER_H

#include <functional>
#include <list>
#include <optional>
#include <queue>
#include <unordered_map>

#include "gsm_sms_message.h"
#include "tel_ril_sms_parcel.h"
#include "i_sms_service_interface.h"
#include "network_state.h"
#include "sms_persist_helper.h"
#include "sms_send_indexer.h"
#include "tel_event_handler.h"

namespace OHOS {
namespace Telephony {
class SmsSender : public TelEventHandler {
public:
    virtual ~SmsSender();
    SmsSender(int32_t slotId, std::function<void(std::shared_ptr<SmsSendIndexer>)> &sendRetryFun);

    virtual void TextBasedSmsDelivery(const std::string &desAddr, const std::string &scAddr,
        const std::string &text, const sptr<ISendShortMessageCallback> &sendCallback,
        const sptr<IDeliveryShortMessageCallback> &deliveryCallback) = 0;

    virtual void DataBasedSmsDelivery(const std::string &desAddr, const std::string &scAddr, int32_t port,
        const uint8_t *data, uint32_t dataLen, const sptr<ISendShortMessageCallback> &sendCallback,
        const sptr<IDeliveryShortMessageCallback> &deliveryCallback) = 0;

    virtual void SendSmsToRil(const std::shared_ptr<SmsSendIndexer> &smsIndexer) = 0;
    void HandleMessageResponse(const std::shared_ptr<SmsSendIndexer> &smsIndexer);
    virtual void ProcessEvent(const AppExecFwk::InnerEvent::Pointer &event) override;
    virtual void Init() = 0;
    virtual void ResendTextDelivery(const std::shared_ptr<SmsSendIndexer> &smsIndexer) = 0;
    virtual void ResendDataDelivery(const std::shared_ptr<SmsSendIndexer> &smsIndexer) = 0;
    virtual int32_t IsImsSmsSupported(int32_t slotId, bool &isSupported) = 0;
    virtual void StatusReportSetImsSms(const AppExecFwk::InnerEvent::Pointer &event) = 0;
    virtual void StatusReportGetImsSms(const AppExecFwk::InnerEvent::Pointer &event) = 0;
    virtual void RegisterImsHandler() = 0;

    static void SendResultCallBack(
        const std::shared_ptr<SmsSendIndexer> &indexer, ISendShortMessageCallback::SmsSendResult result);
    static void SendResultCallBack(
        const sptr<ISendShortMessageCallback> &sendCallback, ISendShortMessageCallback::SmsSendResult result);
    void SetNetworkState(bool isImsNetDomain, int32_t voiceServiceState);
    std::optional<int32_t> GetNetworkId();
    void SetNetworkId(std::optional<int32_t> &id);
    void SyncSwitchISmsResponse();
    bool SetImsSmsConfig(int32_t slotId, int32_t enable);
    void OnRilAdapterHostDied();
    void DataBasedSmsDeliverySplitPage(GsmSmsMessage &gsmSmsMessage, std::vector<struct SplitInfo> cellsInfos,
        std::shared_ptr<struct SmsTpdu> tpdu, uint8_t msgRef8bit, const std::string &desAddr, const std::string &scAddr,
        int32_t port, const sptr<ISendShortMessageCallback> &sendCallback,
        const sptr<IDeliveryShortMessageCallback> &deliveryCallback);
    void DataBasedSmsDeliveryPacketSplitPage(GsmSmsMessage &gsmSmsMessage, std::shared_ptr<struct SmsTpdu> tpdu,
        uint8_t msgRef8bit, uint32_t indexData, int32_t port, const std::string &scAddr,
        const sptr<ISendShortMessageCallback> &sendCallback,
        const sptr<IDeliveryShortMessageCallback> &deliveryCallback, std::shared_ptr<SmsSendIndexer> indexer,
        std::vector<struct SplitInfo> cellsInfos);
    void DataBasedSmsDeliverySendSplitPage(std::shared_ptr<struct EncodeInfo> encodeInfo,
        const sptr<ISendShortMessageCallback> &sendCallback, std::shared_ptr<SmsSendIndexer> indexer,
        uint8_t msgRef8bit, uint32_t totalPage);
    void SendCallbackExceptionCase(const sptr<ISendShortMessageCallback> &sendCallback, std::string str);
    static SmsCodingNationalType GetSmsCodingNationalType(int slotId);

public:
    bool resIsSmsReady_ = false;
    int32_t imsSmsCfg_ = IMS_SMS_ENABLE;
    std::mutex ctx_;
    std::condition_variable cv_;

protected:
    void SendCacheMapTimeoutCheck();
    bool SendCacheMapLimitCheck(const sptr<ISendShortMessageCallback> &sendCallback);
    bool SendCacheMapAddItem(int64_t id, const std::shared_ptr<SmsSendIndexer> &smsIndexer);
    bool SendCacheMapEraseItem(int64_t id);
    std::shared_ptr<SmsSendIndexer> FindCacheMapAndTransform(const AppExecFwk::InnerEvent::Pointer &event);
    uint8_t GetMsgRef8Bit();
    int64_t GetMsgRef64Bit();
    virtual void StatusReportAnalysis(const AppExecFwk::InnerEvent::Pointer &event) = 0;
    void SendMessageSucceed(const std::shared_ptr<SmsSendIndexer> &smsIndexer);
    void SendMessageFailed(const std::shared_ptr<SmsSendIndexer> &smsIndexer);
    bool CheckForce7BitEncodeType();
    void CharArrayToString(const uint8_t *data, uint32_t dataLen, std::string &dataStr);

protected:
    int32_t slotId_ = -1;
    std::list<std::shared_ptr<SmsSendIndexer>> reportList_;
    bool isImsNetDomain_ = false;
    bool enableImsSmsOnceWhenImsReg_ = true;
    int32_t voiceServiceState_ = static_cast<int32_t>(RegServiceState::REG_STATE_UNKNOWN);
    int32_t lastSmsDomain_ = CS_DOMAIN;
    static constexpr uint8_t MAX_SEND_RETRIES = 3;
    static constexpr uint8_t INITIAL_COUNT = 0;
    static constexpr int32_t IMS_SMS_ENABLE = 1;
    static constexpr int32_t IMS_SMS_DISABLE = 0;
    static constexpr int32_t CS_DOMAIN = 0;
    static constexpr int32_t IMS_DOMAIN = 1;
    static constexpr int32_t WAIT_TIME_SECOND = 1;

private:
    static constexpr uint16_t EXPIRED_TIME = 60 * 3;
    static constexpr uint16_t DELAY_MAX_TIME_MSCE = 2000;
    static constexpr uint8_t MSG_QUEUE_LIMIT = 25;
    static constexpr uint8_t MAX_REPORT_LIST_LIMIT = 25;
    static constexpr const char *KEY_SMS_CODING_NATIONAL_INT = "sms_coding_national_int";

    SmsSender(const SmsSender &) = delete;
    SmsSender(const SmsSender &&) = delete;
    SmsSender &operator=(const SmsSender &) = delete;
    SmsSender &operator=(const SmsSender &&) = delete;

    void HandleResend(const std::shared_ptr<SmsSendIndexer> &smsIndexer);
    void UpdateUnSentCellCount(uint8_t refId);

private:
    std::function<void(std::shared_ptr<SmsSendIndexer>)> sendRetryFun_;
    static std::unordered_map<int64_t, std::shared_ptr<SmsSendIndexer>> sendCacheMap_;
    std::mutex sendCacheMapMutex_;
    uint8_t msgRef8bit_ = 0;
    static int64_t msgRef64bit_;
    std::optional<int32_t> networkId_ = std::nullopt;
};
} // namespace Telephony
} // namespace OHOS
#endif
