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

#include "sms_broadcast_subscriber.h"

#include "common_event_support.h"
#include "short_message.h"
#include "sms_cb_data.h"
#include "string_utils.h"

namespace OHOS {
namespace Telephony {
using namespace OHOS::EventFwk;
SmsBroadcastSubscriber::SmsBroadcastSubscriber(const OHOS::EventFwk::CommonEventSubscribeInfo &subscriberInfo)
    : CommonEventSubscriber(subscriberInfo)
{}

void SmsBroadcastSubscriber::OnReceiveEvent(const OHOS::EventFwk::CommonEventData &data)
{
    std::cout << "CommonEventPublishOrderedEventTest::Subscriber OnReceiveEvent" << std::endl;
    OHOS::EventFwk::Want want = data.GetWant();
    std::string action = data.GetWant().GetAction();
    if (action == CommonEventSupport::COMMON_EVENT_SMS_RECEIVE_COMPLETED) {
        int msgcode = GetCode();
        std::string msgdata = GetData();
        bool isCdma = want.GetBoolParam("isCdma", false);
        std::cout << "Sms Receive::OnReceiveEvent msgcode" << msgcode << std::endl;
        std::cout << "Sms Receive::OnReceiveEvent data = " << msgdata.data() << std::endl;
        std::cout << "Sms Receive::OnReceiveEvent format Type = " << (isCdma ? "Cdma" : "Gsm") << std::endl;
        const std::vector<std::string> pdus = want.GetStringArrayParam("pdus");
        for (unsigned int index = 0; index < pdus.size(); ++index) {
            std::vector<unsigned char> pdu = StringUtils::HexToByteVector(pdus[index]);
            auto message = new ShortMessage();
            std::u16string netType = isCdma ? u"3gpp2" : u"3gpp";
            ShortMessage::CreateMessage(pdu, netType, *message);
            if (message != nullptr) {
                std::string messageBody = StringUtils::ToUtf8(message->GetVisibleMessageBody());
                std::cout << "receive new sms = " << messageBody.c_str() << std::endl;
                delete message;
                message = nullptr;
            } else {
                std::cout << "Sms Receive::OnReceiveEvent pdus = " << pdus[index] << std::endl;
            }
        }
    } else if (action == CommonEventSupport::COMMON_EVENT_SMS_EMERGENCY_CB_RECEIVE_COMPLETED ||
        action == CommonEventSupport::COMMON_EVENT_SMS_CB_RECEIVE_COMPLETED) {
        CbMessageTest(want);
    } else if (action == CommonEventSupport::COMMON_EVENT_SMS_WAPPUSH_RECEIVE_COMPLETED) {
        WapPushMessageTest(want);
    } else {
        std::cout << "CommonEventPublishOrderedEventTest::Subscriber OnReceiveEvent do nothing" << std::endl;
    }
}

void SmsBroadcastSubscriber::CbMessageTest(const OHOS::EventFwk::Want &want) const
{
    std::cout << SmsCbData::SLOT_ID << ":" << want.GetIntParam(SmsCbData::SLOT_ID, DEFAULT_VALUE) << std::endl;
    std::cout << SmsCbData::CB_MSG_TYPE << ":"
              << std::to_string(static_cast<uint8_t>(want.GetByteParam(SmsCbData::CB_MSG_TYPE, DEFAULT_VALUE)))
              << std::endl;
    std::cout << SmsCbData::LANG_TYPE << ":"
              << std::to_string(static_cast<uint8_t>(want.GetByteParam(SmsCbData::LANG_TYPE, DEFAULT_VALUE)))
              << std::endl;
    std::cout << SmsCbData::DCS << ":"
              << std::to_string(static_cast<uint8_t>(want.GetByteParam(SmsCbData::DCS, DEFAULT_VALUE)))
              << std::endl;
    std::cout << SmsCbData::PRIORITY << ":"
              << std::to_string(want.GetByteParam(SmsCbData::PRIORITY, DEFAULT_VALUE)) << std::endl;
    std::cout << SmsCbData::CMAS_CLASS << ":"
              << std::to_string(want.GetByteParam(SmsCbData::CMAS_CLASS, DEFAULT_VALUE)) << std::endl;
    std::cout << SmsCbData::CMAS_CATEGORY << ":"
              << std::to_string(want.GetByteParam(SmsCbData::CMAS_CATEGORY, DEFAULT_VALUE)) << std::endl;
    std::cout << SmsCbData::CMAS_RESPONSE << ":"
              << std::to_string(want.GetByteParam(SmsCbData::CMAS_RESPONSE, DEFAULT_VALUE)) << std::endl;
    std::cout << SmsCbData::SEVERITY << ":"
              << std::to_string(want.GetByteParam(SmsCbData::SEVERITY, DEFAULT_VALUE)) << std::endl;
    std::cout << SmsCbData::URGENCY << ":" << std::to_string(want.GetByteParam(SmsCbData::URGENCY, DEFAULT_VALUE))
              << std::endl;
    std::cout << SmsCbData::CERTAINTY << ":"
              << std::to_string(want.GetByteParam(SmsCbData::CERTAINTY, DEFAULT_VALUE)) << std::endl;
    std::cout << SmsCbData::MSG_BODY << ":" << want.GetStringParam(SmsCbData::MSG_BODY) << std::endl;
    std::cout << SmsCbData::FORMAT << ":" << std::to_string(want.GetByteParam(SmsCbData::FORMAT, DEFAULT_VALUE))
              << std::endl;
    std::cout << SmsCbData::SERIAL_NUM << ":"
              << want.GetIntParam(SmsCbData::SERIAL_NUM, DEFAULT_VALUE) << std::endl;
    std::cout << SmsCbData::RECV_TIME << ":" << want.GetStringParam(SmsCbData::RECV_TIME) << std::endl;
    std::cout << SmsCbData::MSG_ID << ":"
              << want.GetIntParam(SmsCbData::MSG_ID, DEFAULT_VALUE) << std::endl;
    std::cout << SmsCbData::SERVICE_CATEGORY << ":"
              << want.GetIntParam(SmsCbData::SERVICE_CATEGORY, DEFAULT_VALUE) << std::endl;
    std::cout << SmsCbData::IS_ETWS_PRIMARY << ":" << want.GetBoolParam(SmsCbData::IS_ETWS_PRIMARY, false)
              << std::endl;
    std::cout << SmsCbData::IS_CMAS_MESSAGE << ":" << want.GetBoolParam(SmsCbData::IS_CMAS_MESSAGE, false)
              << std::endl;
    std::cout << SmsCbData::IS_ETWS_MESSAGE << ":" << want.GetBoolParam(SmsCbData::IS_ETWS_MESSAGE, false)
              << std::endl;
    std::cout << SmsCbData::PLMN << ":" << want.GetStringParam(SmsCbData::PLMN) << std::endl;
    std::cout << SmsCbData::LAC << ":" << want.GetIntParam(SmsCbData::LAC, DEFAULT_VALUE) << std::endl;
    std::cout << SmsCbData::CID << ":" << want.GetIntParam(SmsCbData::CID, DEFAULT_VALUE) << std::endl;
    std::cout << SmsCbData::WARNING_TYPE << ":"
              << want.GetIntParam(SmsCbData::WARNING_TYPE, DEFAULT_VALUE) << std::endl;
    std::cout << SmsCbData::GEO_SCOPE << ":"
              << std::to_string(want.GetByteParam(SmsCbData::GEO_SCOPE, DEFAULT_VALUE)) << std::endl;
}

void SmsBroadcastSubscriber::WapPushMessageTest(const OHOS::EventFwk::Want &want) const
{
    std::cout << "wap push slotId:" << want.GetIntParam("slotId", 0) << std::endl;
    std::cout << "wap push pushType:" << want.GetIntParam("pushType", 0) << std::endl;
    std::cout << "wap push transactionId:" << want.GetIntParam("transactionId", 0) << std::endl;
    std::cout << "wap push strAppId:" << want.GetStringParam("strAppId") << std::endl;
    std::cout << "wap push contentType:" << want.GetStringParam("contentType") << std::endl;
    std::cout << "wap push header:" << want.GetStringParam("headerData") << std::endl;
    std::cout << "wap push data:" << want.GetStringParam("rawData") << std::endl;
}
} // namespace Telephony
} // namespace OHOS
