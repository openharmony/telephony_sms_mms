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

#include "sms_sender.h"

#include "core_manager_inner.h"
#include "ims_sms_client.h"
#include "radio_event.h"
#include "sms_hisysevent.h"
#include "string_utils.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
using namespace std;
using namespace std::chrono;
SmsSender::SmsSender(const shared_ptr<AppExecFwk::EventRunner> &runner, int32_t slotId,
    function<void(shared_ptr<SmsSendIndexer>)> &sendRetryFun)
    : AppExecFwk::EventHandler(runner), slotId_(slotId), sendRetryFun_(sendRetryFun)
{}

SmsSender::~SmsSender() {}

void SmsSender::ProcessEvent(const AppExecFwk::InnerEvent::Pointer &event)
{
    if (event == nullptr) {
        TELEPHONY_LOGE("event is nullptr");
        return;
    }
    shared_ptr<SmsSendIndexer> smsIndexer = nullptr;
    uint32_t eventId = event->GetInnerEventId();
    TELEPHONY_LOGI("SmsSender::ProcessEvent eventId %{public}d", eventId);
    switch (eventId) {
        case RadioEvent::RADIO_SEND_SMS:
        case RadioEvent::RADIO_SEND_CDMA_SMS:
        case RadioEvent::RADIO_SEND_IMS_GSM_SMS:
        case RadioEvent::RADIO_SEND_SMS_EXPECT_MORE: {
            smsIndexer = FindCacheMapAndTransform(event);
            HandleMessageResponse(smsIndexer);
            break;
        }
        case MSG_SMS_RETRY_DELIVERY: {
            smsIndexer = event->GetSharedObject<SmsSendIndexer>();
            if (sendRetryFun_ != nullptr) {
                sendRetryFun_(smsIndexer);
            }
            break;
        }
        case RadioEvent::RADIO_SMS_STATUS: {
            StatusReportAnalysis(event);
            break;
        }
        case RadioEvent::RADIO_SET_IMS_SMS: {
            StatusReportSetImsSms(event);
            SyncSwitchISmsResponse();
            break;
        }
        case RadioEvent::RADIO_GET_IMS_SMS: {
            StatusReportGetImsSms(event);
            SyncSwitchISmsResponse();
            break;
        }
        default:
            TELEPHONY_LOGE("SmsSender::ProcessEvent Unknown %{public}d", eventId);
            break;
    }
}

void SmsSender::SyncSwitchISmsResponse()
{
    std::unique_lock<std::mutex> lck(ctx_);
    resIsSmsReady_ = true;
    TELEPHONY_LOGI("resIsSmsReady_ = %{public}d", resIsSmsReady_);
    cv_.notify_one();
}

void SmsSender::HandleMessageResponse(const shared_ptr<SmsSendIndexer> &smsIndexer)
{
    if (smsIndexer == nullptr) {
        TELEPHONY_LOGE("smsIndexer is nullptr");
        return;
    }
    if (!SendCacheMapEraseItem(smsIndexer->GetMsgRefId64Bit())) {
        TELEPHONY_LOGE("SendCacheMapEraseItem fail !!!!!");
    }
    SendCacheMapTimeoutCheck();
    if (!smsIndexer->GetIsFailure()) {
        if (smsIndexer->GetDeliveryCallback() != nullptr) {
            // Expecting a status report.  Add it to the list.
            if (reportList_.size() > MAX_REPORT_LIST_LIMIT) {
                reportList_.pop_front();
            }
            reportList_.push_back(smsIndexer);
        }
        SendMessageSucceed(smsIndexer);
    } else {
        HandleResend(smsIndexer);
    }
}

void SmsSender::SendMessageSucceed(const shared_ptr<SmsSendIndexer> &smsIndexer)
{
    if (smsIndexer == nullptr) {
        TELEPHONY_LOGE("SendMessageSucceed but smsIndexer drop!");
        return;
    }
    bool isLastPart = false;
    std::shared_ptr<uint8_t> unSentCellCount = smsIndexer->GetUnSentCellCount();
    if (unSentCellCount != nullptr) {
        (*unSentCellCount) = (*unSentCellCount) - 1;
        if ((*unSentCellCount) == 0) {
            isLastPart = true;
        }
    }
    if (isLastPart) {
        smsIndexer->SetPsResendCount(INITIAL_COUNT);
        smsIndexer->SetCsResendCount(INITIAL_COUNT);
        ISendShortMessageCallback::SmsSendResult messageType = ISendShortMessageCallback::SEND_SMS_SUCCESS;
        if (smsIndexer->GetHasCellFailed() != nullptr) {
            if (*smsIndexer->GetHasCellFailed()) {
                messageType = ISendShortMessageCallback::SEND_SMS_FAILURE_UNKNOWN;
            }
        }
        SendResultCallBack(smsIndexer->GetSendCallback(), messageType);
        if (messageType == ISendShortMessageCallback::SEND_SMS_SUCCESS) {
            SmsHiSysEvent::WriteSmsSendBehaviorEvent(slotId_, SmsMmsMessageType::SMS_SHORT_MESSAGE);
        }
    }
}

void SmsSender::SendMessageFailed(const shared_ptr<SmsSendIndexer> &smsIndexer)
{
    if (smsIndexer == nullptr) {
        TELEPHONY_LOGE("smsIndexer is nullptr");
        return;
    }
    shared_ptr<bool> hasCellFailed = smsIndexer->GetHasCellFailed();
    if (hasCellFailed != nullptr) {
        *hasCellFailed = true;
    }
    bool isLastPart = false;
    std::shared_ptr<uint8_t> unSentCellCount = smsIndexer->GetUnSentCellCount();
    if (unSentCellCount == nullptr) {
        TELEPHONY_LOGE("unSentCellCount is nullptr");
        return;
    }
    (*unSentCellCount) = (*unSentCellCount) - 1;
    if ((*unSentCellCount) == 0) {
        isLastPart = true;
    }
    if (isLastPart) {
        smsIndexer->SetPsResendCount(INITIAL_COUNT);
        smsIndexer->SetCsResendCount(INITIAL_COUNT);
        // save to db and update state
        sptr<ISendShortMessageCallback> sendCallback = smsIndexer->GetSendCallback();
        SendResultCallBack(sendCallback, ISendShortMessageCallback::SEND_SMS_FAILURE_UNKNOWN);
    }
}

void SmsSender::SendResultCallBack(
    const std::shared_ptr<SmsSendIndexer> &indexer, ISendShortMessageCallback::SmsSendResult result)
{
    if (indexer != nullptr && indexer->GetSendCallback() != nullptr) {
        indexer->GetSendCallback()->OnSmsSendResult(result);
    }
}

void SmsSender::SendResultCallBack(
    const sptr<ISendShortMessageCallback> &sendCallback, ISendShortMessageCallback::SmsSendResult result)
{
    if (sendCallback != nullptr) {
        sendCallback->OnSmsSendResult(result);
    }
}

void SmsSender::SendCacheMapTimeoutCheck()
{
    std::lock_guard<std::mutex> guard(sendCacheMapMutex_);
    system_clock::duration timePoint = system_clock::now().time_since_epoch();
    seconds sec = duration_cast<seconds>(timePoint);
    long timeStamp = sec.count();
    auto item = sendCacheMap_.begin();
    while (item != sendCacheMap_.end()) {
        auto iter = item++;
        shared_ptr<SmsSendIndexer> &indexer = iter->second;
        if (indexer == nullptr || (timeStamp - indexer->GetTimeStamp()) > EXPIRED_TIME) {
            sendCacheMap_.erase(iter);
        }
    }
}

bool SmsSender::SendCacheMapLimitCheck(const sptr<ISendShortMessageCallback> &sendCallback)
{
    std::lock_guard<std::mutex> guard(sendCacheMapMutex_);
    if (sendCacheMap_.size() > MSG_QUEUE_LIMIT) {
        SendResultCallBack(sendCallback, ISendShortMessageCallback::SEND_SMS_FAILURE_UNKNOWN);
        return true;
    }
    return false;
}

bool SmsSender::SendCacheMapAddItem(int64_t id, const std::shared_ptr<SmsSendIndexer> &smsIndexer)
{
    std::lock_guard<std::mutex> guard(sendCacheMapMutex_);
    if (smsIndexer != nullptr) {
        auto result = sendCacheMap_.emplace(id, smsIndexer);
        return result.second;
    }
    return false;
}

bool SmsSender::SendCacheMapEraseItem(int64_t id)
{
    std::lock_guard<std::mutex> guard(sendCacheMapMutex_);
    return (sendCacheMap_.erase(id) != 0);
}

std::shared_ptr<SmsSendIndexer> SmsSender::FindCacheMapAndTransform(const AppExecFwk::InnerEvent::Pointer &event)
{
    if (event == nullptr) {
        TELEPHONY_LOGE("event is nullptr");
        return nullptr;
    }
    std::shared_ptr<SmsSendIndexer> smsIndexer = nullptr;
    std::lock_guard<std::mutex> guard(sendCacheMapMutex_);
    std::shared_ptr<HRilRadioResponseInfo> res = event->GetSharedObject<HRilRadioResponseInfo>();
    if (res != nullptr) {
        auto iter = sendCacheMap_.find(res->flag);
        if (iter != sendCacheMap_.end()) {
            smsIndexer = iter->second;
            if (smsIndexer == nullptr) {
                TELEPHONY_LOGE("smsIndexer is nullptr");
                return nullptr;
            }
            smsIndexer->SetErrorCode(ISendShortMessageCallback::SEND_SMS_FAILURE_UNKNOWN);
            smsIndexer->SetMsgRefId64Bit(res->flag);
            smsIndexer->SetIsFailure(true);
        }
        return smsIndexer;
    }

    std::shared_ptr<SendSmsResultInfo> info = event->GetSharedObject<SendSmsResultInfo>();
    if (info != nullptr) {
        auto iter = sendCacheMap_.find(info->flag);
        if (iter != sendCacheMap_.end()) {
            TELEPHONY_LOGI("msgRef = %{public}d", info->msgRef);
            smsIndexer = iter->second;
            if (smsIndexer == nullptr) {
                TELEPHONY_LOGE("smsIndexer is nullptr");
                return nullptr;
            }
            smsIndexer->SetMsgRefId((uint8_t)info->msgRef);
            smsIndexer->SetAckPdu(std::move(StringUtils::HexToByteVector(info->pdu)));
            if (info->errCode != 0) {
                smsIndexer->SetIsFailure(true);
            }
            smsIndexer->SetErrorCode(info->errCode);
            smsIndexer->SetMsgRefId64Bit(info->flag);
        }
    }
    return smsIndexer;
}

bool SmsSender::SetImsSmsConfig(int32_t slotId, int32_t enable)
{
    auto smsClient = DelayedSingleton<ImsSmsClient>::GetInstance();
    if (smsClient == nullptr) {
        TELEPHONY_LOGE("SetImsSmsConfig return, ImsSmsClient is nullptr.");
        return false;
    }
    imsSmsCfg_ = enable;
    std::unique_lock<std::mutex> lck(ctx_);
    resIsSmsReady_ = false;

    int32_t reply = smsClient->ImsSetSmsConfig(slotId, enable);
    TELEPHONY_LOGI("SetImsSmsConfig reply = %{public}d", reply);
    while (resIsSmsReady_) {
        TELEPHONY_LOGI("SetImsSmsConfig::wait(), resIsSmsReady_ = false");
        if (cv_.wait_for(lck, std::chrono::seconds(WAIT_TIME_SECOND)) == std::cv_status::timeout) {
            break;
        }
    }
    TELEPHONY_LOGI("SmsSender::SetImsSmsConfig(), %{public}d:", imsSmsCfg_);
    return true;
}

void SmsSender::HandleResend(const std::shared_ptr<SmsSendIndexer> &smsIndexer)
{
    if (smsIndexer == nullptr) {
        TELEPHONY_LOGE("smsIndexer is nullptr");
        return;
    }
    // resending mechanism
    bool errorCode = false;
    if ((smsIndexer->GetErrorCode() == HRIL_ERR_GENERIC_FAILURE) ||
        (smsIndexer->GetErrorCode() == HRIL_ERR_CMD_SEND_FAILURE)) {
        errorCode = true;
    }
    bool csResend = false;
    if (!lastSmsDomain_ && smsIndexer->GetCsResendCount() < MAX_SEND_RETRIES) {
        csResend = true;
    }
    bool psResend = false;
    if (lastSmsDomain_ && smsIndexer->GetPsResendCount() <= MAX_SEND_RETRIES) {
        psResend = true;
    }
    if (errorCode && (csResend || psResend)) {
        if (lastSmsDomain_ && psResend) {
            smsIndexer->SetPsResendCount(smsIndexer->GetPsResendCount() + 1);
            SendEvent(MSG_SMS_RETRY_DELIVERY, smsIndexer, DELAY_MAX_TIME_MSCE);
        } else if (csResend) {
            smsIndexer->SetCsResendCount(smsIndexer->GetCsResendCount() + 1);
            SendEvent(MSG_SMS_RETRY_DELIVERY, smsIndexer, DELAY_MAX_TIME_MSCE);
        }
    } else {
        SendMessageFailed(smsIndexer);
    }
}

uint8_t SmsSender::GetMsgRef8Bit()
{
    return ++msgRef8bit_;
}

int64_t SmsSender::GetMsgRef64Bit()
{
    return ++msgRef64bit_;
}

void SmsSender::SetNetworkState(bool isImsNetDomain, int32_t voiceServiceState)
{
    isImsNetDomain_ = isImsNetDomain;
    voiceServiceState_ = voiceServiceState;
    if (enableImsSmsOnceWhenImsReg_ && isImsNetDomain_) {
        SetImsSmsConfig(slotId_, IMS_SMS_ENABLE);
        enableImsSmsOnceWhenImsReg_ = false;
    }
    TELEPHONY_LOGI("isImsNetDomain = %{public}s voiceServiceState = %{public}d",
        isImsNetDomain_ ? "true" : "false", voiceServiceState_);
}

bool SmsSender::CheckForce7BitEncodeType()
{
    auto helperPtr = DelayedSingleton<SmsPersistHelper>::GetInstance();
    if (helperPtr == nullptr) {
        TELEPHONY_LOGE("Check User Force 7Bit Encode Type helperPtr nullptr error.");
        return false;
    }
    return helperPtr->QueryParamBoolean(SmsPersistHelper::SMS_ENCODING_PARAM_KEY, false);
}

std::optional<int32_t> SmsSender::GetNetworkId()
{
    return networkId_;
}

void SmsSender::SetNetworkId(std::optional<int32_t> &id)
{
    networkId_ = id;
}
} // namespace Telephony
} // namespace OHOS