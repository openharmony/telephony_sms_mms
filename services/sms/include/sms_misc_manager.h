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

#ifndef SMS_MISC_MANAGER_H
#define SMS_MISC_MANAGER_H

#include <condition_variable>
#include <list>
#include <mutex>
#include <string>
#include <vector>

#include "tel_ril_sms_parcel.h"
#include "i_sms_service_interface.h"
#include "tel_event_handler.h"

namespace OHOS {
namespace Telephony {
class SmsMiscManager : public TelEventHandler {
public:
    enum {
        SET_CB_CONFIG_FINISH = 0,
        GET_CB_CONFIG_FINISH,
        SET_SMSC_ADDR_FINISH,
        GET_SMSC_ADDR_FINISH,
    };
    using gsmCBRangeInfo = struct RangeInfo {
        RangeInfo(uint32_t fromId, uint32_t toId)
        {
            fromMsgId = fromId;
            toMsgId = toId;
        }
        uint32_t fromMsgId = 0;
        uint32_t toMsgId = 0;
        bool operator<(const RangeInfo &other) const
        {
            return fromMsgId < other.fromMsgId || (fromMsgId == other.fromMsgId && toMsgId < other.toMsgId);
        }
        bool operator==(const RangeInfo &other) const
        {
            return fromMsgId == other.fromMsgId && toMsgId == other.toMsgId;
        }
    };
    explicit SmsMiscManager(int32_t slotId);
    virtual ~SmsMiscManager() {};
    int32_t SetCBConfig(bool enable, uint32_t fromMsgId, uint32_t toMsgId, uint8_t netType);
    std::list<gsmCBRangeInfo> GetRangeInfo() const;
    virtual void ProcessEvent(const AppExecFwk::InnerEvent::Pointer &event) override;
    int32_t AddSimMessage(
        const std::string &smsc, const std::string &pdu, ISmsServiceInterface::SimMessageStatus status);
    int32_t DelSimMessage(uint32_t msgIndex);
    int32_t UpdateSimMessage(uint32_t msgIndex, ISmsServiceInterface::SimMessageStatus newStatus,
        const std::string &pdu, const std::string &smsc);
    int32_t GetAllSimMessages(std::vector<ShortMessage> &message);
    int32_t SetSmscAddr(const std::string &scAddr);
    int32_t GetSmscAddr(std::u16string &smscAddress);
    int32_t SetDefaultSmsSlotId(int32_t slotId);
    int32_t GetDefaultSmsSlotId();
    int32_t GetDefaultSmsSimId(int32_t &simId);
    int32_t SetCBConfigList(const std::vector<int32_t>& messageIds, int32_t ranType);

private:
    using infoData = struct info {
        info(uint32_t fromMsgId, uint32_t toMsgId)
        {
            startPos = fromMsgId;
            endPos = toMsgId;
        }
        bool isMerge = false;
        uint32_t startPos = 0;
        uint32_t endPos = 0;
    };

private:
    bool OpenCBRange(uint32_t fromMsgId, uint32_t toMsgId);
    bool CloseCBRange(uint32_t fromMsgId, uint32_t toMsgId);
    void NotifyHasResponse();
    bool IsEmpty() const;
    std::string RangeListToString(const std::list<gsmCBRangeInfo> rangeList);
    bool SendDataToRil(bool enable, std::list<gsmCBRangeInfo> list);
    void SplitMsgId(uint32_t fromMsgId, uint32_t toMsgId, const std::list<gsmCBRangeInfo>::iterator &oldIter);
    void UpdateCbRangList(std::shared_ptr<CBConfigInfo> res);
    void SplitMids(std::string src, std::vector<std::string> &dest, const std::string delimiter);
    bool SplitMidValue(std::string value, std::string &start, std::string &end, const std::string delimiter);
    void GetModemCBRange();
    void CombineCBRange();
    void CombineCBRange(std::list<gsmCBRangeInfo>& ranges);
    void GetCBConfigFinish(const AppExecFwk::InnerEvent::Pointer &event);
    std::list<gsmCBRangeInfo> ConvertToRangeList(const std::vector<int32_t>& messageIds);

private:
    std::list<int32_t> fairList_;
    int32_t fairVar_ = -1;
    int32_t conditonVar_ = 0;
    bool isSuccess_ = false;
    int32_t slotId_;
    std::list<gsmCBRangeInfo> rangeList_;
    std::condition_variable condVar_;
    std::mutex mutex_;
    std::mutex cbMutex_;
    std::string smscAddr_;
    std::string codeScheme_ { "0-255" };
    std::list<gsmCBRangeInfo> mdRangeList_;
    int32_t smsCapacityOfSim_ = 0;
    static bool hasGotCbRange_;
};
} // namespace Telephony
} // namespace OHOS
#endif