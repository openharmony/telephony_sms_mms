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

#ifndef GSM_SMS_CB_HANDLER_H
#define GSM_SMS_CB_HANDLER_H

#include <map>
#include <memory>

#include "common_event.h"
#include "common_event_manager.h"
#include "want.h"

#include "event_handler.h"
#include "event_runner.h"
#include "hril_sms_parcel.h"
#include "sms_cb_data.h"
#include "sms_cb_message.h"

enum SmsCbType { SMS_CB_TYPE = 0, SMS_ETWS_TYPE };

namespace OHOS {
namespace Telephony {
using SmsCbInfo = struct CbInfo {
    CbInfo(const std::shared_ptr<SmsCbMessage::SmsCbMessageHeader> &headPtr,
        const std::map<unsigned char, std::shared_ptr<SmsCbMessage>> &cbPtr)
        : header(headPtr), cbMsgs(cbPtr)
    {}
    CbInfo() {}
    std::shared_ptr<SmsCbMessage::SmsCbMessageHeader> header;
    std::map<unsigned char, std::shared_ptr<SmsCbMessage>> cbMsgs;
    std::u16string plmn_;
    int32_t lac_ = -1;
    int32_t cid_ = -1;

    bool MatchLocation(std::u16string plmn, int32_t lac, int32_t cid)
    {
        const int32_t defaultValue = -1;
        if (plmn != plmn_) {
            return false;
        }
        if (lac_ != defaultValue && lac_ != lac) {
            return false;
        }
        if (cid_ != defaultValue && cid_ != cid) {
            return false;
        }
        return true;
    }
};

class GsmSmsCbHandler : public AppExecFwk::EventHandler {
public:
    GsmSmsCbHandler(const std::shared_ptr<AppExecFwk::EventRunner> &runner, int32_t slotId);
    ~GsmSmsCbHandler() = default;
    void Init();
    void UnRegisterHandler();
    virtual void ProcessEvent(const AppExecFwk::InnerEvent::Pointer &event) override;

    static constexpr uint16_t MAX_CB_MSG_TEXT_LEN = 4200;
    static constexpr uint8_t MAX_CB_MSG_LANGUAGE_TYPE_LEN = 3;
    static constexpr uint8_t MAX_ETWS_WARNING_DATA_INFO_LEN = 50;

private:
    bool CheckCbActive(const std::shared_ptr<SmsCbMessage> &cbMessage);
    unsigned char CheckCbMessage(const std::shared_ptr<SmsCbMessage> &cbMessage);
    std::unique_ptr<SmsCbInfo> FindCbMessage(const std::shared_ptr<SmsCbMessage> &cbMessage);
    bool AddCbMessageToList(const std::shared_ptr<SmsCbMessage> &cbMessage);
    bool RemoveCbMessageFromList(const std::shared_ptr<SmsCbMessage> &cbMessage);
    bool SendCbMessageBroadcast(const std::shared_ptr<SmsCbMessage> &cbMessage);
    void HandleCbMessage(std::shared_ptr<CBConfigReportInfo> &message);
    bool SetWantData(EventFwk::Want &want, const std::shared_ptr<SmsCbMessage> &cbMessage);
    bool InitLocation(SmsCbInfo &info);
    void GetCbData(const std::shared_ptr<SmsCbMessage> &cbMessage,
        SmsCbData::CbData &SendData);

    int32_t slotId_;
    std::vector<SmsCbInfo> cbMsgList_;
    int32_t cid_ = -1;
    int32_t lac_ = -1;
    std::u16string plmn_;
};
} // namespace Telephony
} // namespace OHOS
#endif