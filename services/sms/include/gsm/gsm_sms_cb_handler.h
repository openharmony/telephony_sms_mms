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

#ifndef GSM_SMS_CB_HANDLER_H
#define GSM_SMS_CB_HANDLER_H

#include <map>

#include "common_event.h"
#include "common_event_manager.h"
#include "gsm_cb_codec.h"
#include "tel_ril_sms_parcel.h"
#include "sms_cb_data.h"
#include "tel_event_handler.h"
#include "want.h"

enum SmsCbType {
    SMS_CB_TYPE = 0,
    SMS_ETWS_TYPE,
};

namespace OHOS {
namespace Telephony {
using SmsCbInfo = struct CbInfo {
    CbInfo(const std::shared_ptr<GsmCbCodec::GsmCbMessageHeader> &headPtr,
        const std::map<uint8_t, std::shared_ptr<GsmCbCodec>> &cbPtr)
        : header(headPtr), cbMsgs(cbPtr)
    {}
    CbInfo() {}
    std::shared_ptr<GsmCbCodec::GsmCbMessageHeader> header = nullptr;
    std::map<uint8_t, std::shared_ptr<GsmCbCodec>> cbMsgs {};
    std::u16string plmn_ = u"";
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

class GsmSmsCbHandler : public TelEventHandler {
public:
    explicit GsmSmsCbHandler(int32_t slotId);
    ~GsmSmsCbHandler() = default;
    void Init();
    void UnRegisterHandler();
    virtual void ProcessEvent(const AppExecFwk::InnerEvent::Pointer &event) override;

    static constexpr uint16_t MAX_CB_MSG_LEN = 4200;
    static constexpr uint8_t MAX_CB_MSG_LANGUAGE_TYPE_LEN = 3;
    static constexpr uint8_t MAX_ETWS_WARNING_DATA_INFO_LEN = 50;

private:
    bool CheckCbActive(const std::shared_ptr<GsmCbCodec> &cbMessage);
    uint8_t CheckCbMessage(const std::shared_ptr<GsmCbCodec> &cbMessage);
    std::unique_ptr<SmsCbInfo> FindCbMessage(const std::shared_ptr<GsmCbCodec> &cbMessage);
    bool AddCbMessageToList(const std::shared_ptr<GsmCbCodec> &cbMessage);
    bool RemoveCbMessageFromList(const std::shared_ptr<GsmCbCodec> &cbMessage);
    bool SendCbMessageBroadcast(const std::shared_ptr<GsmCbCodec> &cbMessage);
    void HandleCbMessage(std::shared_ptr<CBConfigReportInfo> &message);
    bool SetWantData(EventFwk::Want &want, const std::shared_ptr<GsmCbCodec> &cbMessage);
    bool InitLocation(SmsCbInfo &info);
    void GetCbData(const std::shared_ptr<GsmCbCodec> &cbMessage, SmsCbData::CbData &SendData);
    void PackageWantData(SmsCbData::CbData &sendData, EventFwk::Want &want);

private:
    int32_t slotId_;
    std::vector<SmsCbInfo> cbMsgList_;
    int32_t cid_ = -1;
    int32_t lac_ = -1;
    std::u16string plmn_;
};
} // namespace Telephony
} // namespace OHOS
#endif