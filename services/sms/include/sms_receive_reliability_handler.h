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

#ifndef SMS_RECEIVE_RELIABILITY_HANDLER_H
#define SMS_RECEIVE_RELIABILITY_HANDLER_H

#include "common_event_manager.h"
#include "sms_wap_push_handler.h"
#include "want.h"

namespace OHOS {
namespace Telephony {
class SmsReceiveReliabilityHandler : public std::enable_shared_from_this<SmsReceiveReliabilityHandler> {
public:
    explicit SmsReceiveReliabilityHandler(int32_t slotId);
    virtual ~SmsReceiveReliabilityHandler();
    void SmsReceiveReliabilityProcessing();
    std::string GetSmsExpire();
    bool DeleteExpireSmsFromDB();
    bool CheckSmsCapable();
    void DeleteMessageFormDb(const uint16_t refId, const uint16_t dataBaseId = 0);
    bool CheckBlockedPhoneNumber(std::string originatingAddress);
    void SendBroadcast(
        const std::shared_ptr<SmsReceiveIndexer> indexer, const std::shared_ptr<std::vector<std::string>> pdus);

private:
    void RemoveBlockedSms(std::vector<SmsReceiveIndexer> &dbIndexers);
    void CheckUnReceiveWapPush(std::vector<SmsReceiveIndexer> &dbIndexers);
    void HiSysEventCBResult(bool publishResult);
    void GetWapPushUserDataSinglePage(
        SmsReceiveIndexer &indexer, std::shared_ptr<std::vector<std::string>> userDataRaws);
    void GetWapPushUserDataMultipage(int32_t &smsPagesCount, std::vector<SmsReceiveIndexer> &dbIndexers, int32_t place,
        std::shared_ptr<std::vector<std::string>> userDataRaws);
    void ReadyDecodeWapPushUserData(SmsReceiveIndexer &indexer, std::shared_ptr<std::vector<std::string>> userDataRaws);
    void GetSmsUserDataMultipage(int32_t &smsPagesCount, std::vector<SmsReceiveIndexer> &dbIndexers, int32_t position,
        std::shared_ptr<std::vector<std::string>> pdus);
    void ReadySendSmsBroadcast(SmsReceiveIndexer &indexerObj, std::shared_ptr<std::vector<std::string>> pdus);
    void PacketSmsData(EventFwk::Want &want, const std::shared_ptr<SmsReceiveIndexer> indexer,
        EventFwk::CommonEventData &data, EventFwk::CommonEventPublishInfo &publishInfo);
    void DeleteAutoSmsFromDB(
        std::shared_ptr<SmsReceiveReliabilityHandler> handler, uint16_t refId, uint16_t dataBaseId);

private:
    std::unique_ptr<SmsWapPushHandler> smsWapPushHandler_;
    int32_t slotId_ = -1;
};
} // namespace Telephony
} // namespace OHOS
#endif