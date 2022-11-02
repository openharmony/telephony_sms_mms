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

#include "sms_wap_push_handler.h"

#include <memory>

#include "common_event.h"
#include "common_event_manager.h"
#include "common_event_support.h"
#include "mms_msg.h"
#include "securec.h"
#include "sms_hisysevent.h"
#include "sms_persist_helper.h"
#include "string_utils.h"
#include "telephony_log_wrapper.h"
#include "telephony_permission.h"
#include "want.h"

namespace OHOS {
namespace Telephony {
using namespace OHOS::EventFwk;
SmsWapPushHandler::SmsWapPushHandler(int32_t slotId) : slotId_(slotId) {}

SmsWapPushHandler::~SmsWapPushHandler() {}

bool SmsWapPushHandler::DecodeWapPushPduData(SmsWapPushBuffer &decodeBuffer, uint32_t startPos, uint32_t len)
{
    uint32_t headerLength = len;
    uint32_t startHeader = startPos;

    std::unique_ptr<char[]> headerBuffer = nullptr;
    headerBuffer = decodeBuffer.ReadDataBuffer(startHeader, headerLength);
    if (headerBuffer == nullptr) {
        TELEPHONY_LOGE("Read Header Buffer nullptr error");
        return false;
    }
    hexHeaderData_ = StringUtils::StringToHex(static_cast<char *>(headerBuffer.get()), headerLength);

    uint32_t posData = 0;
    uint32_t dataLength = 0;
    if (contentType_.GetContentType() == std::string(CONTENT_MIME_TYPE_B_PUSH_CO)) {
        dataLength = decodeBuffer.GetSize();
    } else {
        dataLength = decodeBuffer.GetSize() - startHeader - headerLength;
        posData = startHeader + headerLength;
    }
    std::unique_ptr<char[]> pduBuffer = nullptr;
    pduBuffer = decodeBuffer.ReadDataBuffer(posData, dataLength);
    if (pduBuffer == nullptr) {
        TELEPHONY_LOGE("Read Pdu Buffer nullptr error");
        return false;
    }
    hexWbXmlData_ = StringUtils::StringToHex(static_cast<char *>(pduBuffer.get()), dataLength);
    return true;
}
/*
 * wap-230-wsp-20010705-a 8.2.4.1 Push and ConfirmedPush
 */
bool SmsWapPushHandler::DecodeWapPushPdu(std::string &wapPdu)
{
    SmsWapPushBuffer decodeBuffer;
    if (!decodeBuffer.WriteRawStringBuffer(wapPdu)) {
        TELEPHONY_LOGE("Wap push WriteRawStringBuffer fail.");
        return false;
    }
    if (!DecodePushType(decodeBuffer)) {
        TELEPHONY_LOGE("Wap push DecodePushType fail.");
        return false;
    }

    uint32_t count = 0;
    uint32_t headerLength = 0;
    if (!decodeBuffer.DecodeUintvar(headerLength, count)) {
        TELEPHONY_LOGE("Wap push DecodeUintvar fail.");
        return false;
    }

    int32_t contentTypeLength = 0;
    uint32_t startHeader = decodeBuffer.GetCurPosition();
    if (!contentType_.DecodeContentType(decodeBuffer, contentTypeLength)) {
        TELEPHONY_LOGE("Wap push DecodeContentType fail.");
        return false;
    }
    uint32_t headersLen = 0;
    uint32_t curentPosition = decodeBuffer.GetCurPosition();
    if (headerLength + startHeader <= curentPosition) {
        TELEPHONY_LOGE("Wap push headersLen fail.");
        return false;
    }
    headersLen = headerLength - curentPosition + startHeader;
    DecodeXWapApplication(decodeBuffer, headersLen);

    if (!DecodeWapPushPduData(decodeBuffer, startHeader, headerLength)) {
        TELEPHONY_LOGE("Wap push DecodeWapPushPduData fail.");
        return false;
    }

    if (DeocdeCheckIsBlock(hexWbXmlData_)) {
        TELEPHONY_LOGI("Wap Push Mms-message Is Blocked Dispatcher.");
        return true;
    }
    SendWapPushMessageBroadcast();
    return true;
}

/*
 * wap-230-wsp-20010705-a 8.2.4.1 Push and ConfirmedPush
 * 8.2.4 Push and Confirmed Push Facilities
 */
bool SmsWapPushHandler::DecodePushType(SmsWapPushBuffer &decodeBuffer)
{
    transactionId_ = 0;
    if (!decodeBuffer.GetOneByte(transactionId_)) {
        TELEPHONY_LOGE("Decode Transaction Id error.");
        return false;
    }

    pushType_ = 0;
    if (!decodeBuffer.GetOneByte(pushType_)) {
        TELEPHONY_LOGE("Decode PushType Error.");
        return false;
    }
    /** 8.2.4 Push and Confirmed Push Facilities **/
    if (pushType_ != PDU_TYPE_PUSH && pushType_ != PDU_TYPE_CONFIRMED_PUSH) {
        TELEPHONY_LOGE("unSupported this pushType [%{public}ud]", pushType_);
        return false;
    }
    return true;
}

/**
 * @brief DeocdeCheckIsBlock
 * Check Block From Address From Contact BataBase
 * @param pdus [in]
 * @param len [in]
 * @return true
 * @return false
 */
bool SmsWapPushHandler::DeocdeCheckIsBlock(std::string &hexData)
{
    const uint8_t mmsNotificationInd = 130;

    std::string pdustr = StringUtils::HexToString(hexData);
    uint32_t pduLen = pdustr.length();

    std::unique_ptr<char[]> pdus = std::make_unique<char[]>(pduLen);
    if (pdus == nullptr || pduLen == 0) {
        TELEPHONY_LOGE("pdu buffer data param error");
        return false;
    }
    if (memcpy_s(pdus.get(), pduLen, pdustr.data(), pduLen) != EOK) {
        TELEPHONY_LOGE("Memcpy_s DeocdeCheckIsBlock Error.");
        return false;
    }

    MmsMsg mmsMsg;
    bool result = mmsMsg.DecodeMsg(std::move(pdus), pduLen);
    if (result && (mmsMsg.GetMmsMessageType() == mmsNotificationInd)) {
        mmsMsg.DumpMms();
        MmsAddress fromAddress = mmsMsg.GetMmsFrom();
        auto helper = DelayedSingleton<SmsPersistHelper>::GetInstance();
        if (helper == nullptr) {
            TELEPHONY_LOGE("SmsPersist Helper nullptr error");
            return false;
        }
        std::string address = fromAddress.GetAddressString();
        std::size_t pos = address.find('/');
        if (pos != std::string::npos) {
            return helper->QueryBlockPhoneNumber(address.substr(pos));
        }
    }
    TELEPHONY_LOGI("wap push decode is block.");
    return false;
}

/**
 * @brief DecodeXwapApplication
 * WAP-251-PushMessage-20010322-a    5.2.1 5.2.2. WAP Headers
 * @param decodeBuffer [in]
 * @param headersLen [in]
 * @return true
 * @return false
 */
bool SmsWapPushHandler::DecodeXWapApplication(SmsWapPushBuffer &decodeBuffer, uint32_t headersLen)
{
    std::unique_ptr<char[]> tempHeadersBuffer = nullptr;
    tempHeadersBuffer = decodeBuffer.ReadDataBuffer(headersLen);
    if (headersLen > 0 && tempHeadersBuffer != nullptr) {
        SmsWapPushBuffer tempXWapDataBuffer;
        if (!tempXWapDataBuffer.WriteDataBuffer(std::move(tempHeadersBuffer), headersLen)) {
            TELEPHONY_LOGE("Wap push WriteDataBuffer fail.");
            return false;
        }
        decodeBuffer.IncreasePointer(headersLen);
        return DecodeXWapApplicationField(tempXWapDataBuffer, strAppId_);
    }
    return false;
}

/**
 * @brief DecodeXWapApplicationField
 * WAP-251-PushMessage-20010322-a    5.2.1 5.2.2. WAP Headers
 * @param decodeBuffer [in]
 * @param strWapAppId [out]
 * @return true
 * @return false
 */
bool SmsWapPushHandler::DecodeXWapApplicationField(SmsWapPushBuffer &decodeBuffer, std::string &strWapAppId)
{
    while (decodeBuffer.GetCurPosition() < decodeBuffer.GetSize()) {
        uint64_t fieldValue = 0;
        if (!decodeBuffer.DecodeInteger(fieldValue)) {
            TELEPHONY_LOGE("Wap push DecodeInteger fail.");
            return false;
        }
        if (fieldValue == PARAMETER_X_WAP_APPLICATION_ID) {
            return DecodeXWapApplicationValue(decodeBuffer, strWapAppId);
        } else {
            DecodeXWapAbandonHeaderValue(decodeBuffer);
        }
    }
    return false;
}

/*
 * wap-230-wsp-20010705-a
 * 8.4.2.54 X-Wap-Application-Id field
 * The following rule is used to encode the X-Wap-Application-Id field.
 * Application-id-value = Uri-value | App-assigned-code
 * App-assigned-code = Integer-value
 */
bool SmsWapPushHandler::DecodeXWapApplicationValue(SmsWapPushBuffer &decodeBuffer, std::string &strWapAppId)
{
    uint64_t appIdValue = 0;
    if (decodeBuffer.DecodeInteger(appIdValue)) {
        return true;
    }
    uint32_t len = 0;
    if (!decodeBuffer.DecodeText(strWapAppId, len)) {
        TELEPHONY_LOGE("Wap push DecodeText fail.");
        return false;
    }
    return true;
}

/*
 * wap-230-wsp-20010705-a
 * 8.4.1.2 Field values, We abancon beyond that X-Wap-Application-Id
 * Value Interpretation of First Octet
 * 0 - 30 This octet is followed by the indicated number (0 –30) of data octets
 * 31 This octet is followed by a uintvar, which indicates the number of data octets after it
 * 32 - 127 The value is a text string, terminated by a zero octet (NUL character)
 * 128 - 255 It is an encoded 7-bit value; this header has no more data
 */
bool SmsWapPushHandler::DecodeXWapAbandonHeaderValue(SmsWapPushBuffer &decodeBuffer)
{
    const uint8_t wapShortLengthMax = 30;
    const uint8_t wapLengthQuote = 31;
    const uint8_t textLengthMax = 127;

    uint8_t oneByte = 0;
    if (!decodeBuffer.GetOneByte(oneByte)) {
        TELEPHONY_LOGE("Wap push GetOneByte fail.");
        return false;
    }

    if ((oneByte >= 0) && (oneByte <= wapShortLengthMax)) {
        if (!decodeBuffer.IncreasePointer(oneByte)) {
            TELEPHONY_LOGE("Wap push IncreasePointer fail.");
            return false;
        }
    } else if (oneByte == wapLengthQuote) {
        uint32_t length = 0;
        uint32_t count = 0;
        if (!decodeBuffer.DecodeUintvar(length, count)) {
            TELEPHONY_LOGE("Wap push DecodeUintvar fail.");
            return false;
        }
        if (!decodeBuffer.IncreasePointer(length)) {
            TELEPHONY_LOGE("Wap push IncreasePointer fail.");
            return false;
        }
    } else if ((wapLengthQuote < oneByte) && (oneByte <= textLengthMax)) {
        std::string strTemp = "";
        uint32_t length = 0;
        if (!decodeBuffer.DecodeText(strTemp, length)) {
            TELEPHONY_LOGE("Wap push DecodeText fail.");
            return false;
        }
    }
    return true;
}

bool SmsWapPushHandler::SendWapPushMessageBroadcast()
{
    EventFwk::Want want;
    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_SMS_WAPPUSH_RECEIVE_COMPLETED);

    want.SetParam("slotId", static_cast<int>(slotId_));
    want.SetParam("pushType", static_cast<int>(pushType_));
    want.SetParam("applicationId", strAppId_);
    want.SetParam("transactionId", static_cast<int>(transactionId_));
    want.SetParam("contentType", contentType_.GetContentType());
    want.SetParam("headerData", hexHeaderData_);
    want.SetParam("rawData", hexWbXmlData_);

    EventFwk::CommonEventData data;
    data.SetWant(want);
    data.SetData("Sms WapPush Message");
    data.SetCode(0);
    EventFwk::CommonEventPublishInfo publishInfo;
    publishInfo.SetOrdered(true);
    std::vector<std::string> wappushPermissions;
    wappushPermissions.emplace_back(Permission::RECEIVE_MESSAGES);
    publishInfo.SetSubscriberPermissions(wappushPermissions);
    bool publishResult = EventFwk::CommonEventManager::PublishCommonEvent(data, publishInfo, nullptr);
    if (!publishResult) {
        TELEPHONY_LOGE("SendBroadcast PublishBroadcastEvent result fail");
        SmsHiSysEvent::WriteSmsReceiveFaultEvent(slotId_, SmsMmsMessageType::WAP_PUSH,
            SmsMmsErrorCode::SMS_ERROR_PUBLISH_COMMON_EVENT_FAIL, "publish wpa push broadcast event fail");
    }
    DelayedSingleton<SmsHiSysEvent>::GetInstance()->SetWapPushBroadcastStartTime();
    return true;
}
} // namespace Telephony
} // namespace OHOS
