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

#ifndef SMS_WAP_PUSH_HANDLER_H
#define SMS_WAP_PUSH_HANDLER_H

#include <memory>

#include "sms_broadcast_subscriber_receiver.h"
#include "sms_receive_indexer.h"
#include "sms_wap_push_buffer.h"
#include "sms_wap_push_content_type.h"

namespace OHOS {
namespace Telephony {
class SmsWapPushHandler {
public:
    explicit SmsWapPushHandler(int32_t slotId);
    virtual ~SmsWapPushHandler();

    bool DecodeWapPushPduData(SmsWapPushBuffer &decodeBuffer, uint32_t startPos, uint32_t len);
    bool DecodeWapPushPdu(std::shared_ptr<SmsReceiveIndexer> indexer, std::string &wapPdu);
    bool DecodePushType(SmsWapPushBuffer &decodeBuffer);
    bool DeocdeCheckIsBlock(std::string &hexData);
    bool DecodeXWapApplicationField(SmsWapPushBuffer &decodeBuffer, std::string &strWapAppId);
    bool DecodeXWapApplication(SmsWapPushBuffer &decodeBuffer, uint32_t headersLen);
    bool DecodeXWapApplicationValue(SmsWapPushBuffer &decodeBuffer, std::string &strWapAppId);
    bool DecodeXWapAbandonHeaderValue(SmsWapPushBuffer &decodeBuffer);
    bool SendWapPushMessageBroadcast(std::shared_ptr<SmsReceiveIndexer> indexer);

private:
    void HiSysEventWapPushResult(bool publishResult);
    void DeleteWapPush(std::shared_ptr<SmsReceiveIndexer> indexer);

private:
    static std::shared_ptr<SmsBroadcastSubscriberReceiver> g_receiver;
    int32_t slotId_ = 0;
    uint8_t pushType_ = 0;
    std::string strAppId_;
    uint8_t transactionId_ = 0;
    SmsWapPushContentType contentType_;
    std::string hexHeaderData_;
    std::string hexWbXmlData_;
};
} // namespace Telephony
} // namespace OHOS
#endif
