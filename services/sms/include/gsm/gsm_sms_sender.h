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

#ifndef GSM_SMS_SENDER_H
#define GSM_SMS_SENDER_H

#include <functional>
#include <mutex>

#include "gsm_sms_message.h"
#include "i_delivery_short_message_callback.h"
#include "i_send_short_message_callback.h"
#include "ims_sms_client.h"
#include "satellite_sms_callback.h"
#include "sms_sender.h"
#include "telephony_types.h"

namespace OHOS {
namespace Telephony {
class GsmSmsSender : public SmsSender {
public:
    GsmSmsSender(int32_t slotId, const std::function<void(std::shared_ptr<SmsSendIndexer>)> sendRetryFun);
    ~GsmSmsSender() override;
    void Init() override;
    void TextBasedSmsDelivery(const std::string &desAddr, const std::string &scAddr, const std::string &text,
        const sptr<ISendShortMessageCallback> &sendCallback,
        const sptr<IDeliveryShortMessageCallback> &deliveryCallback) override;
    void DataBasedSmsDelivery(const std::string &desAddr, const std::string &scAddr, int32_t port, const uint8_t *data,
        uint32_t dataLen, const sptr<ISendShortMessageCallback> &sendCallback,
        const sptr<IDeliveryShortMessageCallback> &deliveryCallback) override;
    void SendSmsToRil(const std::shared_ptr<SmsSendIndexer> &smsIndexer) override;
    void ResendTextDelivery(const std::shared_ptr<SmsSendIndexer> &smsIndexer) override;
    void ResendDataDelivery(const std::shared_ptr<SmsSendIndexer> &smsIndexer) override;
    int32_t IsImsSmsSupported(int32_t slotId, bool &isSupported) override;
    void StatusReportSetImsSms(const AppExecFwk::InnerEvent::Pointer &event) override;
    void StatusReportGetImsSms(const AppExecFwk::InnerEvent::Pointer &event) override;
    void RegisterImsHandler() override;
    void RegisterSatelliteCallback();
    void UnregisterSatelliteCallback();

protected:
    void StatusReportAnalysis(const AppExecFwk::InnerEvent::Pointer &event) override;

private:
    void SetSendIndexerInfo(const std::shared_ptr<SmsSendIndexer> &indexer,
        const std::shared_ptr<struct EncodeInfo> &encodeInfo, unsigned char msgRef8bit);
    bool RegisterHandler();
    bool SetPduInfo(const std::shared_ptr<SmsSendIndexer> &smsIndexer, GsmSmsMessage &gsmSmsMessage, bool &isMore);
    void SendImsSms(const std::shared_ptr<SmsSendIndexer> &smsIndexer, GsmSimMessageParam smsData);
    void SendCsSms(const std::shared_ptr<SmsSendIndexer> &smsIndexer, GsmSimMessageParam smsData);
    void SendSatelliteSms(const std::shared_ptr<SmsSendIndexer> &smsIndexer, GsmSimMessageParam smsData);
    void OnSatelliteStateChange(bool satelliteSwitch);
    void TextBasedSmsSplitDelivery(const std::string &desAddr, const std::string &scAddr,
        std::vector<struct SplitInfo> cellsInfos, DataCodingScheme codingType, bool isStatusReport,
        std::shared_ptr<struct SmsTpdu> tpdu, GsmSmsMessage &gsmSmsMessage,
        const sptr<ISendShortMessageCallback> &sendCallback,
        const sptr<IDeliveryShortMessageCallback> &deliveryCallback);

private:
    std::mutex mutex_;
    bool isImsGsmHandlerRegistered = false;
    sptr<ISatelliteSmsCallback> satelliteCallback_ = nullptr;
};
} // namespace Telephony
} // namespace OHOS
#endif