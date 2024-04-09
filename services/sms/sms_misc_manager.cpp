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

#include "sms_misc_manager.h"

#include "core_manager_inner.h"
#include "short_message.h"
#include "sms_mms_errors.h"
#include "string_utils.h"
#include "telephony_common_utils.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
static constexpr int32_t WAIT_TIME_SECOND = 1;
static constexpr int32_t TIME_OUT_SECOND = 3;
static constexpr uint8_t GSM_TYPE = 1;
static constexpr uint8_t MIN_SMSC_LEN = 2;
static constexpr uint32_t RANG_MAX = 65535;
bool SmsMiscManager::hasGotCbRange_ = false;

SmsMiscManager::SmsMiscManager(int32_t slotId) : TelEventHandler("SmsMiscManager"), slotId_(slotId) {}

int32_t SmsMiscManager::SetCBConfig(bool enable, uint32_t fromMsgId, uint32_t toMsgId, uint8_t netType)
{
    bool ret = false;
    if ((toMsgId > RANG_MAX) || (fromMsgId > toMsgId) || (netType != GSM_TYPE)) {
        TELEPHONY_LOGI("cb channel invalid");
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }
    std::unique_lock<std::mutex> lock(cbMutex_);
    if (!hasGotCbRange_) {
        GetModemCBRange();
        if (mdRangeList_.size() != 0) {
            if (!SendDataToRil(false, mdRangeList_)) {
                return TELEPHONY_ERR_RIL_CMD_FAIL;
            }
            mdRangeList_.clear();
        }
    }
    std::list<gsmCBRangeInfo> oldRangeList = rangeList_;
    if (enable) {
        ret = OpenCBRange(fromMsgId, toMsgId);
    } else {
        ret = CloseCBRange(fromMsgId, toMsgId);
    }
    if (ret) {
        if (!SendDataToRil(false, oldRangeList)) {
            rangeList_ = oldRangeList;
            return TELEPHONY_ERR_RIL_CMD_FAIL;
        }
        if (!SendDataToRil(true, rangeList_)) {
            rangeList_ = oldRangeList;
            return TELEPHONY_ERR_RIL_CMD_FAIL;
        }
    } else {
        rangeList_ = oldRangeList;
    }
    return TELEPHONY_ERR_SUCCESS;
}

std::list<SmsMiscManager::gsmCBRangeInfo> SmsMiscManager::GetRangeInfo() const
{
    return rangeList_;
}

// from 3GPP TS 27.005 3.3.4 Select Cell Broadcast Message Types
bool SmsMiscManager::OpenCBRange(uint32_t fromMsgId, uint32_t toMsgId)
{
    infoData data(fromMsgId, toMsgId);
    rangeList_.emplace_back(fromMsgId, toMsgId);
    CombineCBRange();
    return true;
}

void SmsMiscManager::SplitMsgId(
    uint32_t fromMsgId, uint32_t toMsgId, const std::list<gsmCBRangeInfo>::iterator &oldIter)
{
    auto &info = *oldIter;
    rangeList_.emplace_back(info.fromMsgId, fromMsgId - 1);
    rangeList_.emplace_back(toMsgId + 1, info.toMsgId);
    rangeList_.erase(oldIter);
}

// from 3GPP TS 27.005 3.3.4 Select Cell Broadcast Message Types
bool SmsMiscManager::CloseCBRange(uint32_t fromMsgId, uint32_t toMsgId)
{
    auto iter = rangeList_.begin();
    while (iter != rangeList_.end()) {
        auto oldIter = iter++;
        auto &info = *oldIter;
        if (fromMsgId == info.fromMsgId && toMsgId == info.toMsgId) {
            rangeList_.erase(oldIter);
            continue;
        } else if (fromMsgId == info.fromMsgId && toMsgId > info.toMsgId) {
            rangeList_.erase(oldIter);
            continue;
        } else if (fromMsgId == info.fromMsgId && toMsgId < info.toMsgId) {
            rangeList_.emplace_back(toMsgId + 1, info.toMsgId);
            rangeList_.erase(oldIter);
            continue;
        } else if (fromMsgId < info.fromMsgId && toMsgId == info.toMsgId) {
            rangeList_.erase(oldIter);
            continue;
        } else if (fromMsgId > info.fromMsgId && toMsgId == info.toMsgId) {
            rangeList_.emplace_back(info.fromMsgId, fromMsgId - 1);
            rangeList_.erase(oldIter);
            continue;
        } else if (fromMsgId > info.fromMsgId && toMsgId < info.toMsgId) {
            SplitMsgId(fromMsgId, toMsgId, oldIter);
            continue;
        } else if (fromMsgId > info.fromMsgId && fromMsgId <= info.toMsgId) {
            rangeList_.emplace_back(info.fromMsgId, fromMsgId - 1);
            rangeList_.erase(oldIter);
            continue;
        } else if (fromMsgId < info.fromMsgId && toMsgId > info.toMsgId) {
            rangeList_.erase(oldIter);
            continue;
        } else if (toMsgId < info.toMsgId && toMsgId >= info.fromMsgId) {
            rangeList_.emplace_back(toMsgId + 1, info.toMsgId);
            rangeList_.erase(oldIter);
            continue;
        }
    }
    CombineCBRange();
    return true;
}

void SmsMiscManager::ProcessEvent(const AppExecFwk::InnerEvent::Pointer &event)
{
    if (event == nullptr) {
        TELEPHONY_LOGE("SmsMiscManager ProcessEvent event == nullptr");
        return;
    }

    uint32_t eventId = 0;
    eventId = event->GetInnerEventId();
    switch (eventId) {
        case SET_CB_CONFIG_FINISH: {
            TELEPHONY_LOGI("SmsMiscManager::ProcessEvent Set cb config finish");
            isSuccess_ = true;
            std::shared_ptr<RadioResponseInfo> res = event->GetSharedObject<RadioResponseInfo>();
            if (res != nullptr) {
                isSuccess_ = (res->error == ErrType::NONE);
            }
            NotifyHasResponse();
            break;
        }
        case GET_CB_CONFIG_FINISH: {
            TELEPHONY_LOGI("SmsMiscManager::ProcessEvent Get cb config finish");
            GetCBConfigFinish(event);
            break;
        }
        case SET_SMSC_ADDR_FINISH: {
            TELEPHONY_LOGI("SmsMiscManager::ProcessEvent Set smsc addr finish");
            isSuccess_ = true;
            std::shared_ptr<RadioResponseInfo> res = event->GetSharedObject<RadioResponseInfo>();
            if (res != nullptr) {
                isSuccess_ = (res->error == ErrType::NONE);
            }
            NotifyHasResponse();
            break;
        }
        case GET_SMSC_ADDR_FINISH: {
            TELEPHONY_LOGI("SmsMiscManager::ProcessEvent Get smsc addr finish");
            isSuccess_ = true;
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

void SmsMiscManager::GetCBConfigFinish(const AppExecFwk::InnerEvent::Pointer &event)
{
    isSuccess_ = true;
    std::shared_ptr<CBConfigInfo> res = event->GetSharedObject<CBConfigInfo>();
    if (res == nullptr) {
        isSuccess_ = false;
    } else {
        UpdateCbRangList(res);
    }
    NotifyHasResponse();
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

void SmsMiscManager::UpdateCbRangList(std::shared_ptr<CBConfigInfo> res)
{
    mdRangeList_.clear();
    if (res->mids.empty()) {
        return;
    }
    std::vector<std::string> dest;
    SplitMids(res->mids, dest, ",");
    for (auto v : dest) {
        std::string start;
        std::string end;
        if (!SplitMidValue(v, start, end, "-")) {
            continue;
        }
        if (!IsValidDecValue(start) || !IsValidDecValue(end)) {
            TELEPHONY_LOGE("start or end not decimal");
            return;
        }
        if (std::stoi(start) <= std::stoi(end)) {
            infoData data(std::stoi(start), std::stoi(end));
            mdRangeList_.emplace_back(data.startPos, data.endPos);
        }
    }
    rangeList_ = mdRangeList_;
    CombineCBRange();
}

void SmsMiscManager::CombineCBRange()
{
    rangeList_.sort();
    rangeList_.unique();
    auto iter = rangeList_.begin();
    while (iter != rangeList_.end()) {
        auto OtherIter = iter;
        OtherIter++;
        bool eraseFlag = false;
        while (OtherIter != rangeList_.end()) {
            if (OtherIter->fromMsgId == iter->fromMsgId) {
                eraseFlag = true;
                break;
            } else if (OtherIter->toMsgId <= iter->toMsgId) {
                OtherIter = rangeList_.erase(OtherIter);
                continue;
            } else if (OtherIter->fromMsgId <= static_cast<uint32_t>(iter->toMsgId + 1)) {
                iter->toMsgId = OtherIter->toMsgId;
                OtherIter = rangeList_.erase(OtherIter);
                continue;
            }
            OtherIter++;
        }
        if (eraseFlag) {
            iter = rangeList_.erase(iter);
        } else {
            iter++;
        }
    }
}

void SmsMiscManager::SplitMids(std::string src, std::vector<std::string> &dest, const std::string delimiter)
{
    size_t pos = src.find(delimiter);
    while (pos != std::string::npos) {
        dest.push_back(src.substr(0, pos));
        src.erase(0, pos + delimiter.length());
        pos = src.find(delimiter);
    }
    dest.push_back(src);
}

bool SmsMiscManager::SplitMidValue(std::string value, std::string &start, std::string &end, const std::string delimiter)
{
    if (value.empty()) {
        return false;
    }
    size_t pos = value.find(delimiter);
    if (pos == 0 || pos == value.size() - 1) {
        return false;
    } else if (pos == std::string::npos) {
        start = value;
        end = value;
        return true;
    }
    start = value.substr(0, pos);
    end = value.substr(pos + 1);
    return true;
}

std::string SmsMiscManager::RangeListToString(const std::list<gsmCBRangeInfo> rangeList)
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

bool SmsMiscManager::SendDataToRil(bool enable, std::list<gsmCBRangeInfo> list)
{
    TELEPHONY_LOGI("enable CB channel:%{public}d", enable);
    for (auto item : list) {
        TELEPHONY_LOGD("[%{public}d-%{public}d]", item.fromMsgId, item.toMsgId);
    }
    std::unique_lock<std::mutex> lock(mutex_);
    if (!list.empty()) {
        isSuccess_ = false;
        int32_t condition = conditonVar_++;
        fairList_.push_back(condition);
        CBConfigParam cbData;
        cbData.mode = enable ? 1 : 0;
        cbData.idList = RangeListToString(list);
        cbData.dcsList = codeScheme_;
        CoreManagerInner::GetInstance().SetCBConfig(
            slotId_, SmsMiscManager::SET_CB_CONFIG_FINISH, cbData, shared_from_this());
        while (!isSuccess_) {
            TELEPHONY_LOGD("SendDataToRil::wait(), isSuccess_ = false");
            if (condVar_.wait_for(lock, std::chrono::seconds(WAIT_TIME_SECOND)) == std::cv_status::timeout) {
                break;
            }
        }
        return isSuccess_;
    } else {
        return true;
    }
}

void SmsMiscManager::GetModemCBRange()
{
    std::unique_lock<std::mutex> lock(mutex_);
    if (!hasGotCbRange_) {
        isSuccess_ = false;
        int32_t condition = conditonVar_++;
        fairList_.push_back(condition);
        CoreManagerInner::GetInstance().GetCBConfig(slotId_, SmsMiscManager::GET_CB_CONFIG_FINISH, shared_from_this());
        while (!isSuccess_) {
            TELEPHONY_LOGI("GetCBConfig::wait(), isSuccess_ = false");
            if (condVar_.wait_for(lock, std::chrono::seconds(WAIT_TIME_SECOND)) == std::cv_status::timeout) {
                break;
            }
        }
        hasGotCbRange_ = true;
    }
}

int32_t SmsMiscManager::AddSimMessage(
    const std::string &smsc, const std::string &pdu, ISmsServiceInterface::SimMessageStatus status)
{
    std::vector<ShortMessage> message;
    int32_t result = GetAllSimMessages(message);
    if (result != TELEPHONY_ERR_SUCCESS) {
        TELEPHONY_LOGE("SmsMiscManager::AddSimMessage get result fail");
        return result;
    }
    int32_t smsCountCurrent = static_cast<int32_t>(message.size());
    TELEPHONY_LOGI("smsCountCurrent = %{public}d smsCapacityOfSim_ = %{public}d", smsCountCurrent, smsCapacityOfSim_);
    if (smsCountCurrent >= smsCapacityOfSim_) {
        TELEPHONY_LOGE("AddSimMessage sim card is full");
        return TELEPHONY_ERR_FAIL;
    }

    TELEPHONY_LOGI("smscLen = %{public}zu pudLen = %{public}zu status = %{public}d", smsc.size(), pdu.size(), status);
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
    bool hasSimCard = false;
    CoreManagerInner::GetInstance().HasSimCard(slotId_, hasSimCard);
    if (!hasSimCard) {
        TELEPHONY_LOGE("no sim card");
        return TELEPHONY_ERR_SLOTID_INVALID;
    }
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
    bool hasSimCard = false;
    CoreManagerInner::GetInstance().HasSimCard(slotId_, hasSimCard);
    if (!hasSimCard) {
        TELEPHONY_LOGE("no sim card");
        return TELEPHONY_ERR_SLOTID_INVALID;
    }
    std::vector<std::string> pdus = CoreManagerInner::GetInstance().ObtainAllSmsOfIcc(slotId_);
    smsCapacityOfSim_ = static_cast<int32_t>(pdus.size());
    TELEPHONY_LOGI("smsCapacityOfSim_[%{public}d]", smsCapacityOfSim_);
    int index = 1;
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
        v = v.substr(0, v.find("FFFFF"));
        if (v.compare("00") == 0) {
            index++;
            continue;
        }
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
    while (!isSuccess_) {
        TELEPHONY_LOGI("SetSmscAddr::wait(), isSuccess_ = false");
        if (condVar_.wait_for(lock, std::chrono::seconds(TIME_OUT_SECOND)) == std::cv_status::timeout) {
            break;
        }
    }

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
    while (!isSuccess_) {
        TELEPHONY_LOGI("GetSmscAddr::wait(), isSuccess_ = false");
        if (condVar_.wait_for(lock, std::chrono::seconds(TIME_OUT_SECOND)) == std::cv_status::timeout) {
            break;
        }
    }
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

int32_t SmsMiscManager::GetDefaultSmsSimId(int32_t &simId)
{
    TELEPHONY_LOGI("GetDefaultSmsSimId");
    return CoreManagerInner::GetInstance().GetDefaultSmsSimId(simId);
}
} // namespace Telephony
} // namespace OHOS
