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

#include "sms_send_manager.h"

#include <functional>
#include <memory.h>

#include "cdma_sms_message.h"
#include "gsm_sms_message.h"
#include "gsm_sms_tpdu_codec.h"
#include "i_sms_service_interface.h"
#include "sms_receive_manager.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
using namespace std;
SmsSendManager::SmsSendManager(int32_t slotId) : slotId_(slotId) {}

SmsSendManager::~SmsSendManager()
{
    if (gsmSmsSender_ != nullptr) {
        if (auto id = gsmSmsSender_->GetNetworkId(); id.has_value()) {
            networkManager_->NetworkUnregister(id.value());
        }
    }
    if (cdmaSmsSender_ != nullptr) {
        if (auto id = cdmaSmsSender_->GetNetworkId(); id.has_value()) {
            networkManager_->NetworkUnregister(id.value());
        }
    }
    if (networkManager_ != nullptr) {
        networkManager_->UnRegisterHandler();
    }
}

void SmsSendManager::Init()
{
    gsmSmsSendRunner_ = AppExecFwk::EventRunner::Create("gsmSmsSenderLoop" + to_string(slotId_));
    if (gsmSmsSendRunner_ == nullptr) {
        TELEPHONY_LOGE("failed to create GsmSenderEventRunner");
        return;
    }
    gsmSmsSender_ = std::make_shared<GsmSmsSender>(
        gsmSmsSendRunner_, slotId_, bind(&SmsSendManager::RetriedSmsDelivery, this, placeholders::_1));
    if (gsmSmsSender_ == nullptr) {
        TELEPHONY_LOGE("failed to create GsmSmsSender");
        return;
    }
    gsmSmsSender_->Init();
    gsmSmsSendRunner_->Run();

    cdmaSmsSendRunner_ = AppExecFwk::EventRunner::Create("cdmaSmsSenderLoop" + to_string(slotId_));
    if (cdmaSmsSendRunner_ == nullptr) {
        TELEPHONY_LOGE("failed to create CdmaSenderEventRunner");
        return;
    }
    cdmaSmsSender_ = std::make_shared<CdmaSmsSender>(
        cdmaSmsSendRunner_, slotId_, bind(&SmsSendManager::RetriedSmsDelivery, this, placeholders::_1));
    if (cdmaSmsSender_ == nullptr) {
        TELEPHONY_LOGE("failed to create CdmaSmsSender");
        return;
    }
    cdmaSmsSender_->Init();
    cdmaSmsSendRunner_->Run();
    InitNetworkHandle();
}

void SmsSendManager::InitNetworkHandle()
{
    networkRunner_ = AppExecFwk::EventRunner::Create("networkLoop" + to_string(slotId_));
    if (networkRunner_ == nullptr) {
        TELEPHONY_LOGE("failed to create networkRunner");
        return;
    }
    networkManager_ = std::make_shared<SmsNetworkPolicyManager>(networkRunner_, slotId_);
    if (networkManager_ == nullptr) {
        TELEPHONY_LOGE("failed to create networkManager");
        return;
    }
    networkManager_->Init();
    networkRunner_->Run();
    TELEPHONY_LOGI("Init SmsSendManager successfully.");
    if (auto ret = networkManager_->NetworkRegister(
        std::bind(&SmsSender::SetNetworkState, gsmSmsSender_, std::placeholders::_1, std::placeholders::_2));
        ret.has_value()) {
        gsmSmsSender_->SetNetworkId(ret);
    } else {
        TELEPHONY_LOGE("gsm failed to register networkManager");
    }
    if (auto ret = networkManager_->NetworkRegister(
        std::bind(&SmsSender::SetNetworkState, cdmaSmsSender_, std::placeholders::_1, std::placeholders::_2));
        ret.has_value()) {
        cdmaSmsSender_->SetNetworkId(ret);
    } else {
        TELEPHONY_LOGE("cdma failed to register networkManager");
    }
}

void SmsSendManager::TextBasedSmsDelivery(const string &desAddr, const string &scAddr, const string &text,
    const sptr<ISendShortMessageCallback> &sendCallback,
    const sptr<IDeliveryShortMessageCallback> &deliveryCallback)
{
    if (desAddr.empty() || text.empty()) {
        SmsSender::SendResultCallBack(sendCallback, ISendShortMessageCallback::SEND_SMS_FAILURE_UNKNOWN);
        TELEPHONY_LOGE("TextBasedSmsDelivery::param Set Error.");
        return;
    }
    if (networkManager_ == nullptr) {
        SmsSender::SendResultCallBack(sendCallback, ISendShortMessageCallback::SEND_SMS_FAILURE_UNKNOWN);
        TELEPHONY_LOGE("TextBasedSmsDelivery::networkManager nullptr error.");
        return;
    }
    if (gsmSmsSender_ == nullptr || cdmaSmsSender_ == nullptr) {
        SmsSender::SendResultCallBack(sendCallback, ISendShortMessageCallback::SEND_SMS_FAILURE_UNKNOWN);
        TELEPHONY_LOGE("gsmSmsSender or cdmaSmsSender nullptr error.");
        return;
    }

    NetWorkType netWorkType = networkManager_->GetNetWorkType();
    TELEPHONY_LOGI("netWorkType = %{public}d.", netWorkType);
    if (netWorkType == NetWorkType::NET_TYPE_GSM) {
        gsmSmsSender_->RegisterImsHandler();
        gsmSmsSender_->TextBasedSmsDelivery(desAddr, scAddr, text, sendCallback, deliveryCallback);
    } else if (netWorkType == NetWorkType::NET_TYPE_CDMA) {
        cdmaSmsSender_->RegisterImsHandler();
        cdmaSmsSender_->TextBasedSmsDelivery(desAddr, scAddr, text, sendCallback, deliveryCallback);
    } else {
        SmsSender::SendResultCallBack(
            sendCallback, ISendShortMessageCallback::SEND_SMS_FAILURE_SERVICE_UNAVAILABLE);
        TELEPHONY_LOGI("network unknown send error.");
    }
}

void SmsSendManager::DataBasedSmsDelivery(const string &desAddr, const string &scAddr, uint16_t port,
    const uint8_t *data, uint16_t dataLen, const sptr<ISendShortMessageCallback> &sendCallback,
    const sptr<IDeliveryShortMessageCallback> &deliveryCallback)
{
    if (desAddr.empty() || data == nullptr) {
        SmsSender::SendResultCallBack(sendCallback, ISendShortMessageCallback::SEND_SMS_FAILURE_UNKNOWN);
        TELEPHONY_LOGE("DataBasedSmsDelivery::param Set Error.");
        return;
    }
    if (networkManager_ == nullptr) {
        SmsSender::SendResultCallBack(sendCallback, ISendShortMessageCallback::SEND_SMS_FAILURE_UNKNOWN);
        TELEPHONY_LOGE("DataBasedSmsDelivery::networkManager nullptr error.");
        return;
    }
    if (gsmSmsSender_ == nullptr || cdmaSmsSender_ == nullptr) {
        SmsSender::SendResultCallBack(sendCallback, ISendShortMessageCallback::SEND_SMS_FAILURE_UNKNOWN);
        TELEPHONY_LOGE("gsmSmsSender or cdmaSmsSender nullptr error.");
        return;
    }

    NetWorkType netWorkType = networkManager_->GetNetWorkType();
    if (netWorkType == NetWorkType::NET_TYPE_GSM) {
        gsmSmsSender_->DataBasedSmsDelivery(desAddr, scAddr, port, data, dataLen, sendCallback, deliveryCallback);
    } else if (netWorkType == NetWorkType::NET_TYPE_CDMA) {
        cdmaSmsSender_->DataBasedSmsDelivery(desAddr, scAddr, port, data, dataLen, sendCallback, deliveryCallback);
    } else {
        SmsSender::SendResultCallBack(
            sendCallback, ISendShortMessageCallback::SEND_SMS_FAILURE_SERVICE_UNAVAILABLE);
        TELEPHONY_LOGI("network unknown send error.");
    }
}

void SmsSendManager::RetriedSmsDelivery(const shared_ptr<SmsSendIndexer> smsIndexer)
{
    if (smsIndexer == nullptr) {
        TELEPHONY_LOGI("RetriedSmsDelivery::smsIndexer is nullptr error.");
        return;
    }
    if (gsmSmsSender_ == nullptr || cdmaSmsSender_ == nullptr || networkManager_ == nullptr) {
        TELEPHONY_LOGE("Sender or network nullptr error.");
        return;
    }

    NetWorkType oldNetWorkType = smsIndexer->GetNetWorkType();
    NetWorkType newNetWorkType = networkManager_->GetNetWorkType();
    if (oldNetWorkType != newNetWorkType) {
        smsIndexer->SetNetWorkType(newNetWorkType);
        shared_ptr<SmsSendIndexer> indexer = smsIndexer;
        switch (newNetWorkType) {
            case NetWorkType::NET_TYPE_CDMA:
                if (smsIndexer->GetIsText()) {
                    cdmaSmsSender_->ResendTextDelivery(indexer);
                } else {
                    cdmaSmsSender_->ResendDataDelivery(indexer);
                }
                break;
            case NetWorkType::NET_TYPE_GSM:
                if (smsIndexer->GetIsText()) {
                    gsmSmsSender_->ResendTextDelivery(indexer);
                } else {
                    gsmSmsSender_->ResendDataDelivery(indexer);
                }
                break;
            default:
                SmsSender::SendResultCallBack(
                    smsIndexer, ISendShortMessageCallback::SEND_SMS_FAILURE_SERVICE_UNAVAILABLE);
                TELEPHONY_LOGI("network unknown send error.");
                break;
        }
        return;
    }

    if (newNetWorkType == NetWorkType::NET_TYPE_GSM) {
        gsmSmsSender_->SendSmsToRil(smsIndexer);
    } else if (newNetWorkType == NetWorkType::NET_TYPE_CDMA) {
        cdmaSmsSender_->SendSmsToRil(smsIndexer);
    } else {
        SmsSender::SendResultCallBack(smsIndexer, ISendShortMessageCallback::SEND_SMS_FAILURE_SERVICE_UNAVAILABLE);
        TELEPHONY_LOGI("Network Unknown.");
    }
}

std::vector<std::string> SmsSendManager::SplitMessage(const std::string &message)
{
    std::vector<std::string> result;
    if (networkManager_ == nullptr) {
        TELEPHONY_LOGE("SmsSendManager::SplitMessage networkManager nullptr Error.");
        return result;
    }

    SmsCodingScheme codingType;
    std::vector<struct SplitInfo> cellsInfos;
    NetWorkType netWorkType = networkManager_->GetNetWorkType();
    switch (netWorkType) {
        case NetWorkType::NET_TYPE_CDMA: {
            GsmSmsMessage gsmSmsMessage;
            gsmSmsMessage.SplitMessage(cellsInfos, message, false, codingType);
            break;
        }
        case NetWorkType::NET_TYPE_GSM: {
            CdmaSmsMessage cdmaSmsMessage;
            cdmaSmsMessage.SplitMessage(cellsInfos, message, false, codingType);
            break;
        }
        default:
            TELEPHONY_LOGE("netWorkType is NET_TYPE_UNKNOWN");
            break;
    }

    for (auto &item : cellsInfos) {
        result.emplace_back(item.text);
    }
    return result;
}

bool SmsSendManager::GetSmsSegmentsInfo(const std::string &message, bool force7BitCode, LengthInfo &lenInfo)
{
    if (networkManager_ == nullptr) {
        TELEPHONY_LOGE("GetSmsSegmentsInfo networkManager_ Nullptr Error.");
        return false;
    }
    NetWorkType netWorkType = networkManager_->GetNetWorkType();
    switch (netWorkType) {
        case NetWorkType::NET_TYPE_CDMA: {
            CdmaSmsMessage cdmaSmsMessage;
            return cdmaSmsMessage.GetSmsSegmentsInfo(message, force7BitCode, lenInfo);
        }
        case NetWorkType::NET_TYPE_GSM: {
            GsmSmsMessage gsmSmsMessage;
            return gsmSmsMessage.GetSmsSegmentsInfo(message, force7BitCode, lenInfo);
        }
        default:
            TELEPHONY_LOGE("netWorkType is NET_TYPE_UNKNOWN");
            break;
    }
    return false;
}

std::shared_ptr<SmsSender> SmsSendManager::GetCdmaSmsSender() const
{
    return cdmaSmsSender_;
}

bool SmsSendManager::IsImsSmsSupported(int32_t slotId)
{
    bool result = false;
    if (networkManager_ == nullptr) {
        TELEPHONY_LOGE("networkManager is nullptr error.");
        return result;
    }

    if (gsmSmsSender_ == nullptr || cdmaSmsSender_ == nullptr || networkManager_ == nullptr) {
        TELEPHONY_LOGE("Sender or network nullptr error.");
        return result;
    }

    NetWorkType newNetWorkType = networkManager_->GetNetWorkType();
    switch (newNetWorkType) {
        case NetWorkType::NET_TYPE_GSM:
            result = gsmSmsSender_->IsImsSmsSupported(slotId);
            break;
        case NetWorkType::NET_TYPE_CDMA:
            result = cdmaSmsSender_->IsImsSmsSupported(slotId);
            break;
        default:
            TELEPHONY_LOGE("network unknown send error.");
            break;
    }
    return result;
}

bool SmsSendManager::SetImsSmsConfig(int32_t slotId, int32_t enable)
{
    bool result = false;
    if (gsmSmsSender_ == nullptr || cdmaSmsSender_ == nullptr || networkManager_ == nullptr) {
        TELEPHONY_LOGE("Sender or network nullptr error.");
        return result;
    }
    NetWorkType newNetWorkType = networkManager_->GetNetWorkType();

    switch (newNetWorkType) {
        case NetWorkType::NET_TYPE_GSM:
            result = gsmSmsSender_->SetImsSmsConfig(slotId, enable);
            break;
        case NetWorkType::NET_TYPE_CDMA:
            result = cdmaSmsSender_->SetImsSmsConfig(slotId, enable);
            break;
        default:
            TELEPHONY_LOGE("network unknown send error.");
            break;
    }
    return result;
}

std::string SmsSendManager::GetImsShortMessageFormat()
{
    if (networkManager_ == nullptr) {
        TELEPHONY_LOGE("networkManager is nullptr error.");
        return "unknown";
    }
    switch (networkManager_->GetNetWorkType()) {
        case NetWorkType::NET_TYPE_GSM:
            return "3gpp";
        case NetWorkType::NET_TYPE_CDMA:
            return "3gpp2";
        default:
            return "unknown";
    }
}
} // namespace Telephony
} // namespace OHOS