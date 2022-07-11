/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef SMS_CB_DATA_H
#define SMS_CB_DATA_H

#include <string>

#include "cdma_sms_types.h"
#include "sms_cb_message.h"

namespace OHOS {
namespace Telephony {
class SmsCbData {
public:
    struct CbData {
        int32_t slotId = -1;
        std::string msgBody;
        uint16_t serial = 0;
        long recvTime = 0;
        uint16_t msgId = 0;
        int8_t format = 0;
        uint16_t category = SMS_CMAE_CTG_RESERVED;
        bool isEtws = false;
        bool isCmas = false;
        int8_t cmasClass = SMS_CMAE_ALERT_RESERVED;
        int8_t cmasCate = SMS_CMAE_CTG_RESERVED;
        int8_t severity = SMS_CMAE_SEVERITY_RESERVED;
        int8_t urgency = SMS_CMAE_URGENCY_RESERVED;
        int8_t certainty = SMS_CMAE_CERTAINTY_RESERVED;
        uint16_t warnType = 0;
        uint8_t msgType = SmsCbMessage::SMS_CBMSG_TYPE_UNKNOWN;
        uint8_t langType = SmsCbMessage::SMS_CBMSG_LANG_DUMMY;
        uint8_t dcs = 0;
        int8_t priority = SMS_PRIORITY_NORMAL;
        bool isPrimary = false;
        int8_t cmasRes = SMS_CMAE_RESP_TYPE_RESERVED;
        uint8_t geoScope = 0;
    };
    inline static const std::string SLOT_ID = "slotId";
    inline static const std::string CB_MSG_TYPE = "cbMsgType";
    inline static const std::string LANG_TYPE = "langType";
    inline static const std::string DCS = "dcs";
    inline static const std::string PRIORITY = "priority";
    inline static const std::string CMAS_CLASS = "cmasClass";
    inline static const std::string CMAS_CATEGORY = "cmasCategory";
    inline static const std::string CMAS_RESPONSE = "cmasResponseType";
    inline static const std::string SEVERITY = "severity";
    inline static const std::string URGENCY = "urgency";
    inline static const std::string CERTAINTY = "certainty";
    inline static const std::string MSG_BODY = "msgBody";
    inline static const std::string FORMAT = "format";
    inline static const std::string SERIAL_NUM = "serialNum";
    inline static const std::string RECV_TIME = "recvTime";
    inline static const std::string MSG_ID = "msgId";
    inline static const std::string SERVICE_CATEGORY = "serviceCategory";
    inline static const std::string IS_ETWS_PRIMARY = "isEtwsPrimary";
    inline static const std::string IS_CMAS_MESSAGE = "isCmasMessage";
    inline static const std::string IS_ETWS_MESSAGE = "isEtwsMessage";
    inline static const std::string PLMN = "plmn";
    inline static const std::string LAC = "lac";
    inline static const std::string CID = "cid";
    inline static const std::string WARNING_TYPE = "warningType";
    inline static const std::string GEO_SCOPE = "geoScope";
};
} // namespace Telephony
} // namespace OHOS
#endif // SMS_CB_DATA_H

