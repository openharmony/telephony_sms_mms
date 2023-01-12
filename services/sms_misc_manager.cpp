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

#include "sms_misc_manager.h"

#include "core_manager_inner.h"
#include "hril_sms_parcel.h"
#include "short_message.h"
#include "sms_mms_errors.h"
#include "string_utils.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
static constexpr int32_t WAIT_TIME_SECOND = 1;
static constexpr uint8_t GSM_TYPE = 1;
static constexpr uint8_t MIN_SMSC_LEN = 2;
static constexpr uint32_t RANG_MAX = 65535;

SmsMiscManager::SmsMiscManager(const std::shared_ptr<AppExecFwk::EventRunner> &runner, int32_t slotId)
    : AppExecFwk::EventHandler(runner), slotId_(slotId)
{}

int32_t SmsMiscManager::SetCBConfig(bool enable, uint32_t fromMsgId, uint32_t toMsgId, uint8_t netType)
{
    bool ret = false;
    if ((toMsgId > RANG_MAX) || (fromMsgId > toMsgId) || (netType != GSM_TYPE)) {
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }
    oldRangeList_ = rangeList_;
    if (enable) {
        ret = OpenCBRange(fromMsgId, toMsgId);
    } else {
        ret = CloseCBRange(fromMsgId, toMsgId);
    }
    if (ret) {
        if (!SendDataToRil(false, oldRangeList_)) {
            rangeList_ = oldRangeList_;
            return TELEPHONY_ERR_RIL_CMD_FAIL;
        } else {
            oldRangeList_.clear();
        }
        if (!SendDataToRil(true, rangeList_)) {
            rangeList_ = oldRangeList_;
            return TELEPHONY_ERR_RIL_CMD_FAIL;
        }
    }
    return TELEPHONY_ERR_SUCCESS;
}

std::list<SmsMiscManager::gsmCBRangeInfo> SmsMiscManager::GetRangeInfo() const
{
    return rangeList_;
}

bool SmsMiscManager::ExpandMsgId(
    uint32_t fromMsgId, uint32_t toMsgId, const std::list<gsmCBRangeInfo>::iterator &oldIter, infoData &data)
{
    if (static_cast<int32_t>(toMsgId) < static_cast<int32_t>((*oldIter).fromMsgId) - 1) {
        return true;
    } else if (static_cast<int32_t>(toMsgId) == static_cast<int32_t>((*oldIter).fromMsgId) - 1) {
        data.endPos = (*oldIter).toMsgId;
        rangeList_.erase(oldIter);
        return true;
    } else if (toMsgId > (*oldIter).fromMsgId) {
        data.endPos = (*oldIter).toMsgId;
        rangeList_.erase(oldIter);
        return true;
    } else if ((static_cast<int32_t>(toMsgId) == static_cast<int32_t>((*oldIter).fromMsgId) - 1) ||
               (toMsgId == (*oldIter).fromMsgId)) {
        data.endPos = (*oldIter).toMsgId;
        rangeList_.erase(oldIter);
        return false;
    } else if (((fromMsgId >= (*oldIter).fromMsgId && fromMsgId <= (*oldIter).toMsgId &&
                    toMsgId > (*oldIter).toMsgId) ||
                   (static_cast<int32_t>(fromMsgId) - 1 == static_cast<int32_t>((*oldIter).toMsgId)) ||
                   ((fromMsgId < (*oldIter).fromMsgId) && (toMsgId > (*oldIter).toMsgId)))) {
        data.isMerge = true;
        data.startPos = (data.startPos < (*oldIter).fromMsgId) ? data.startPos : (*oldIter).fromMsgId;
        rangeList_.erase(oldIter);
        return false;
    } else {
        return false;
    }
}

// from 3GPP TS 27.005 3.3.4 Select Cell Broadcast Message Types
bool SmsMiscManager::OpenCBRange(uint32_t fromMsgId, uint32_t toMsgId)
{
    infoData data(fromMsgId, toMsgId);
    if (rangeList_.size() == 0) {
        rangeList_.emplace_back(fromMsgId, toMsgId);
        return true;
    }
    auto iter = rangeList_.begin();
    while (iter != rangeList_.end()) {
        auto oldIter = iter++;
        auto &info = *oldIter;
        if (fromMsgId >= info.fromMsgId && toMsgId <= info.toMsgId) {
            return false;
        } else if (!data.isMerge) {
            if (ExpandMsgId(fromMsgId, toMsgId, oldIter, data)) {
                break;
            }
        } else {
            if (static_cast<int32_t>(toMsgId) < static_cast<int32_t>(info.fromMsgId) - 1) {
                data.endPos = toMsgId;
                break;
            } else if (static_cast<int32_t>(toMsgId) == static_cast<int32_t>(info.fromMsgId) - 1) {
                data.endPos = info.toMsgId;
                rangeList_.erase(oldIter);
                break;
            } else if (toMsgId >= info.fromMsgId && toMsgId <= info.toMsgId) {
                data.endPos = info.toMsgId;
                rangeList_.erase(oldIter);
                break;
            } else if (toMsgId > info.toMsgId) {
                rangeList_.erase(oldIter);
            }
        }
    }
    rangeList_.emplace_back(data.startPos, data.endPos);
    rangeList_.sort();
    return true;
}

void SmsMiscManager::SplitMsgId(
    uint32_t fromMsgId, uint32_t toMsgId, const std::list<gsmCBRangeInfo>::iterator &oldIter, infoData &data)
{
    data.isMerge = true;
    auto &info = *oldIter;
    if (info.fromMsgId == fromMsgId && info.toMsgId == toMsgId) {
        rangeList_.erase(oldIter);
    } else if (info.fromMsgId == fromMsgId && info.toMsgId != toMsgId) {
        rangeList_.emplace_back(toMsgId + 1, info.toMsgId);
        rangeList_.erase(oldIter);
    } else if (info.fromMsgId != fromMsgId && info.toMsgId == toMsgId) {
        rangeList_.emplace_back(info.fromMsgId, fromMsgId - 1);
        rangeList_.erase(oldIter);
    } else if (fromMsgId > info.fromMsgId && toMsgId < info.toMsgId) {
        rangeList_.emplace_back(info.fromMsgId, fromMsgId - 1);
        rangeList_.emplace_back(toMsgId + 1, info.toMsgId);
        rangeList_.erase(oldIter);
    }
}

// from 3GPP TS 27.005 3.3.4 Select Cell Broadcast Message Types
bool SmsMiscManager::CloseCBRange(uint32_t fromMsgId, uint32_t toMsgId)
{
    bool ret = false;
    infoData data(fromMsgId, toMsgId);
    auto iter = rangeList_.begin();
    while (iter != rangeList_.end()) {
        auto oldIter = iter++;
        auto &info = *oldIter;
        if (fromMsgId >= info.fromMsgId && toMsgId <= info.toMsgId) {
            SplitMsgId(fromMsgId, toMsgId, oldIter, data);
            ret = true;
            break;
        } else if (fromMsgId <= info.fromMsgId && toMsgId >= info.toMsgId && !data.isMerge) {
            data.isMerge = true;
            ret = true;
            rangeList_.erase(oldIter);
        } else if (data.isMerge && toMsgId >= info.toMsgId) {
            ret = true;
            rangeList_.erase(oldIter);
        } else if (data.isMerge && toMsgId < info.toMsgId && toMsgId >= info.fromMsgId) {
            ret = true;
            rangeList_.emplace_back(toMsgId + 1, info.toMsgId);
            rangeList_.erase(oldIter);
            rangeList_.sort();
            break;
        } else if ((fromMsgId > info.fromMsgId && fromMsgId < info.toMsgId) && toMsgId >= info.toMsgId &&
            !data.isMerge) {
            data.isMerge = true;
            ret = true;
            rangeList_.emplace_back(info.fromMsgId, fromMsgId - 1);
            rangeList_.erase(oldIter);
            rangeList_.sort();
        } else if (fromMsgId == info.toMsgId && toMsgId >= info.toMsgId && !data.isMerge) {
            data.isMerge = true;
            ret = true;
            rangeList_.emplace_back(info.fromMsgId, fromMsgId - 1);
            rangeList_.erase(oldIter);
            rangeList_.sort();
        } else if (fromMsgId < info.toMsgId && toMsgId <= info.toMsgId) {
            if (toMsgId != info.toMsgId) {
                rangeList_.emplace_back(toMsgId + 1, info.toMsgId);
                rangeList_.sort();
            }
            rangeList_.erase(oldIter);
            ret = true;
            break;
        }
    }
    return ret;
}

void SmsMiscManager::ProcessEvent(const AppExecFwk::InnerEvent::Pointer &event)
{
    if (event == nullptr) {
        TELEPHONY_LOGE("SmsMiscManager ProcessEvent event == nullptr");
        return;
    }

    uint32_t eventId = 0;
    TELEPHONY_LOGI("SmsMiscManager::ProcessEvent");
    eventId = event->GetInnerEventId();
    switch (eventId) {
        case SET_CB_CONFIG_FINISH: {
            std::unique_lock<std::mutex> lock(mutex_);
            isSuccess_ = true;
            std::shared_ptr<HRilRadioResponseInfo> res = event->GetSharedObject<HRilRadioResponseInfo>();
            if (res != nullptr) {
                isSuccess_ = (res->error == HRilErrType::NONE);
            }
            NotifyHasResponse();
            break;
        }
        case SET_SMSC_ADDR_FINISH: {
            std::unique_lock<std::mutex> lock(mutex_);
            isSuccess_ = true;
            std::shared_ptr<HRilRadioResponseInfo> res = event->GetSharedObject<HRilRadioResponseInfo>();
            if (res != nullptr) {
                isSuccess_ = (res->error == HRilErrType::NONE);
            }
            NotifyHasResponse();
            break;
        }
        case GET_SMSC_ADDR_FINISH: {
            std::unique_lock<std::mutex> lock(mutex_);
            std::shared_ptr<ServiceCenterAddress> addr = event->GetSharedObject<ServiceCenterAddress>();
            if (addr != nullptr) {
                smscAddr_ = addr->address;
            }
            NotifyHasResponse();
            break;
        }
        default:
            TELEPHONY_LOGI("SmsMiscManager::ProcessEvent default!");
            break;
    }
}

bool SmsMiscManager::IsEmpty() const
{
    return rangeList_.empty();
}

void SmsMiscManager::NotifyHasResponse()
{
    if (fairList_.size() > 0) {
        fairVar_ = fairList_.front();
        fairList_.pop_front();
    } else {
        return;
    }
    condVar_.notify_all();
}

std::string SmsMiscManager::RangeListToString(const std::list<gsmCBRangeInfo> &rangeList)
{
    std::string ret;
    bool isFirst = true;
    for (const auto &item : rangeList) {
        if (isFirst) {
            isFirst = false;
        } else {
            ret += ",";
        }
        if (item.fromMsgId == item.toMsgId) {
            ret += std::to_string(item.fromMsgId);
        } else {
            ret += std::to_string(item.fromMsgId) + "-" + std::to_string(item.toMsgId);
        }
    }
    return ret;
}

bool SmsMiscManager::SendDataToRil(bool enable, std::list<gsmCBRangeInfo> &list)
{
    for (auto &item : list) {
        TELEPHONY_LOGI("[%{public}d-%{public}d]", item.fromMsgId, item.toMsgId);
    }
    std::unique_lock<std::mutex> lock(mutex_);
    if (!list.empty()) {
        isSuccess_ = false;
        int32_t condition = conditonVar_++;
        fairList_.push_back(condition);
        CBConfigParam cbData {.mode = enable ? 0 : 1, .idList = RangeListToString(list), .dcsList = codeScheme_};
        CoreManagerInner::GetInstance().SetCBConfig(
            slotId_, SmsMiscManager::SET_CB_CONFIG_FINISH, cbData, shared_from_this());
        while (!isSuccess_) {
            TELEPHONY_LOGI("SendDataToRil::wait(), isSuccess_ = false");
            if (condVar_.wait_for(lock, std::chrono::seconds(WAIT_TIME_SECOND)) == std::cv_status::timeout) {
                break;
            }
        }
        return isSuccess_;
    } else {
        return true;
    }
}

int32_t SmsMiscManager::AddSimMessage(
    const std::string &smsc, const std::string &pdu, ISmsServiceInterface::SimMessageStatus status)
{
    TELEPHONY_LOGI(
        "smscLen = %{public}zu pudLen = %{public}zu status = %{public}d", smsc.size(), pdu.size(), status);

    std::string smscAddr(smsc);
    if (smsc.length() <= MIN_SMSC_LEN) {
        smscAddr.clear();
        smscAddr.insert(0, "00");
    }
    TELEPHONY_LOGD("smscAddr = %{private}s", smscAddr.c_str());
    return CoreManagerInner::GetInstance().AddSmsToIcc(
        slotId_, static_cast<int>(status), const_cast<std::string &>(pdu), smscAddr);
}

int32_t SmsMiscManager::DelSimMessage(uint32_t msgIndex)
{
    TELEPHONY_LOGI("messageIndex = %{public}d", msgIndex);
    return CoreManagerInner::GetInstance().DelSmsIcc(slotId_, msgIndex);
}

int32_t SmsMiscManager::UpdateSimMessage(uint32_t msgIndex, ISmsServiceInterface::SimMessageStatus newStatus,
    const std::string &pdu, const std::string &smsc)
{
    std::string smscAddr(smsc);
    if (smsc.length() <= MIN_SMSC_LEN) {
        smscAddr.clear();
        smscAddr.insert(0, "00");
    }
    return CoreManagerInner::GetInstance().UpdateSmsIcc(
        slotId_, msgIndex, static_cast<int>(newStatus), const_cast<std::string &>(pdu), smscAddr);
}

int32_t SmsMiscManager::GetAllSimMessages(std::vector<ShortMessage> &message)
{
    TELEPHONY_LOGI("GetAllSimMessages");
    std::vector<std::string> pdus = CoreManagerInner::GetInstance().ObtainAllSmsOfIcc(slotId_);
    int index = 0;
    PhoneType type = CoreManagerInner::GetInstance().GetPhoneType(slotId_);
    std::string specification;
    if (PhoneType::PHONE_TYPE_IS_GSM == type) {
        specification = "3gpp";
    } else if (PhoneType::PHONE_TYPE_IS_CDMA == type) {
        specification = "3gpp2";
    } else {
        return TELEPHONY_ERR_UNKNOWN_NETWORK_TYPE;
    }
    for (auto &v : pdus) {
        std::vector<unsigned char> pdu = StringUtils::HexToByteVector(v);
        ShortMessage item = ShortMessage::CreateIccMessage(pdu, specification, index);
        if (item.GetIccMessageStatus() != ShortMessage::SMS_SIM_MESSAGE_STATUS_FREE) {
            message.emplace_back(item);
        }
        index++;
    }
    return TELEPHONY_ERR_SUCCESS;
}

int32_t SmsMiscManager::SetSmscAddr(const std::string &scAddr)
{
    TELEPHONY_LOGI("SmsMiscManager::SetSmscAddr [%{private}s]", scAddr.c_str());
    std::unique_lock<std::mutex> lock(mutex_);
    isSuccess_ = false;
    int32_t condition = conditonVar_++;
    fairList_.push_back(condition);
    CoreManagerInner::GetInstance().SetSmscAddr(
        slotId_, SmsMiscManager::SET_SMSC_ADDR_FINISH, 0, scAddr, shared_from_this());
    condVar_.wait(lock, [&]() { return fairVar_ == condition; });
    if (isSuccess_ == false) {
        return TELEPHONY_ERR_RIL_CMD_FAIL;
    }
    return TELEPHONY_ERR_SUCCESS;
}

int32_t SmsMiscManager::GetSmscAddr(std::u16string &smscAddress)
{
    TELEPHONY_LOGI("SmsMiscManager::GetSmscAddr");
    std::unique_lock<std::mutex> lock(mutex_);
    smscAddr_.clear();
    isSuccess_ = false;
    int32_t condition = conditonVar_++;
    fairList_.push_back(condition);
    CoreManagerInner::GetInstance().GetSmscAddr(slotId_, SmsMiscManager::GET_SMSC_ADDR_FINISH, shared_from_this());
    condVar_.wait(lock, [&]() { return fairVar_ == condition; });
    smscAddress = StringUtils::ToUtf16(smscAddr_);
    return TELEPHONY_ERR_SUCCESS;
}

int32_t SmsMiscManager::SetDefaultSmsSlotId(int32_t slotId)
{
    TELEPHONY_LOGI("SetDefaultSmsSlotId slotId = %{public}d", slotId);
    return CoreManagerInner::GetInstance().SetDefaultSmsSlotId(slotId);
}

int32_t SmsMiscManager::GetDefaultSmsSlotId()
{
    TELEPHONY_LOGI("GetDefaultSmsSlotId");
    return CoreManagerInner::GetInstance().GetDefaultSmsSlotId();
}
} // namespace Telephony
} // namespace OHOS
