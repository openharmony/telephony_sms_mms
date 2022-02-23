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

#ifndef NAPI_MMS_H
#define NAPI_MMS_H
#include <codecvt>
#include <locale>
#include <string>
#include <vector>
#include <cstring>
#include <memory>

#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "i_sms_service_interface.h"
#include "base_context.h"
#include "short_message.h"
#include "napi_sms.h"

#include "mms_msg.h"
#include "mms_address.h"
#include "mms_attachment.h"
#include "telephony_log_wrapper.h"
#include "napi_util.h"
#include "mms_codec_type.h"
#include "mms_header.h"

namespace OHOS {
namespace Telephony {
enum MessageType {
    TYPE_MMS_SEND_REQ = 128,
    TYPE_MMS_SEND_CONF,
    TYPE_MMS_NOTIFICATION_IND,
    TYPE_MMS_RESP_IND,
    TYPE_MMS_RETRIEVE_CONF,
    TYPE_MMS_ACKNOWLEDGE_IND,
    TYPE_MMS_DELIVERY_IND,
    TYPE_MMS_READ_REC_IND,
    TYPE_MMS_READ_ORIG_IND,
};

enum DispositionValue {
    FROM_DATA = 0,
    ATTACHMENT,
    INLINE,
};

struct MmsAddressContext {
    std::string address;
    int32_t charset = DEFAULT_ERROR;
};

struct MmsAttachmentContext {
    std::string path;
    std::string fileName;
    std::string contentId;
    std::string contentLocation;
    std::string contentDisposition;
    std::string contentTransferEncoding;
    std::string contentType;
    bool isSmil = false;
    uint32_t inBuffLen;
    std::unique_ptr<char[]> inBuff;
    int32_t charset = DEFAULT_ERROR;
};

struct MmsSendConfContext {
    uint8_t responseState;
    std::string transactionId;
    uint16_t version;
    std::string messageId;
};

struct MmsSendReqContext {
    MmsAddress from;
    std::vector<MmsAddress> to;
    std::string transactionId;
    uint16_t version = DEFAULT_ERROR;
    int64_t date;
    std::vector<MmsAddress> cc;
    std::vector<MmsAddress> bcc;
    std::string subject;
    uint8_t messageClass;
    int32_t expiry = DEFAULT_ERROR;
    uint8_t priority = DEFAULT_ERROR;
    uint8_t senderVisibility;
    uint8_t deliveryReport;
    uint8_t readReport;
    std::string contentType;
};

struct MmsNotificationIndContext {
    std::string transactionId;
    uint8_t messageClass;
    int64_t messageSize;
    int32_t expiry;
    uint16_t version;
    MmsAddress from;
    std::string subject;
    uint8_t deliveryReport;
    std::string contentLocation;
    uint8_t contentClass;
    uint32_t charset;
};

struct MmsRespIndContext {
    std::string transactionId;
    uint8_t status;
    uint16_t version;
    uint8_t reportAllowed;
};

struct MmsRetrieveConfContext {
    std::string transactionId;
    std::string messageId;
    int64_t date;
    uint16_t version;
    std::vector<MmsAddress> to;
    MmsAddress from;
    std::vector<MmsAddress> cc;
    std::string subject;
    uint8_t priority;
    uint8_t deliveryReport;
    uint8_t readReport;
    uint8_t retrieveStatus;
    std::string retrieveText;
    std::string contentType;
    int32_t message;
};

struct MmsAcknowledgeIndContext {
    std::string transactionId;
    uint16_t version;
    uint8_t reportAllowed;
};

struct MmsDeliveryIndContext {
    std::string messageId;
    int64_t date;
    std::vector<MmsAddress> to;
    uint8_t status;
    uint16_t version;
};

struct MmsReadOrigIndContext {
    uint16_t version;
    std::string messageId;
    std::vector<MmsAddress> to;
    MmsAddress from;
    int64_t date;
    uint8_t readStatus;
};

struct MmsReadRecIndContext {
    uint16_t version;
    std::string messageId;
    std::vector<MmsAddress> to;
    MmsAddress from;
    int64_t date;
    uint8_t readStatus;
};

struct DecodeMmsContext : BaseContext {
    int32_t messageType = DEFAULT_ERROR;
    int32_t messageMatchResult;
    std::string textFilePath = "";
    std::unique_ptr<char[]> inBuffer = nullptr;
    uint32_t inLen;
    std::vector<MmsAttachmentContext> attachment;
    struct MmsSendConfContext sendConf;
    struct MmsSendReqContext sendReq;
    struct MmsNotificationIndContext notificationInd;
    struct MmsRespIndContext respInd;
    struct MmsRetrieveConfContext retrieveConf;
    struct MmsAcknowledgeIndContext acknowledgeInd;
    struct MmsDeliveryIndContext deliveryInd;
    struct MmsReadOrigIndContext readOrigInd;
    struct MmsReadRecIndContext readRecInd;
};

struct EncodeMmsContext : BaseContext {
    int32_t messageType = DEFAULT_ERROR;
    std::unique_ptr<char[]> outBuffer = nullptr;
    uint32_t bufferLen;
    std::vector<MmsAttachmentContext> attachment;
    struct MmsSendConfContext sendConf;
    struct MmsSendReqContext sendReq;
    struct MmsNotificationIndContext notificationInd;
    struct MmsRespIndContext respInd;
    struct MmsRetrieveConfContext retrieveConf;
    struct MmsAcknowledgeIndContext acknowledgeInd;
    struct MmsDeliveryIndContext deliveryInd;
    struct MmsReadOrigIndContext readOrigInd;
    struct MmsReadRecIndContext readRecInd;
};

class NapiMms {
public:
    NapiMms();
    ~NapiMms() = default;

    static napi_value DecodeMms(napi_env env, napi_callback_info info);
    static napi_value EncodeMms(napi_env env, napi_callback_info info);
    static napi_value InitEnumMmsCharSets(napi_env env, napi_value exports);
    static napi_value InitEnumMessageType(napi_env env, napi_value exports);
    static napi_value InitEnumPriorityType(napi_env env, napi_value exports);
    static napi_value InitEnumVersionType(napi_env env, napi_value exports);
    static napi_value InitEnumDispositionType(napi_env env, napi_value exports);
    static napi_value InitEnumReportAllowedType(napi_env env, napi_value exports);
    static napi_value InitSupportEnumMmsCharSets(napi_env env, napi_value exports);
    static napi_value InitSupportEnumMessageType(napi_env env, napi_value exports);
    static napi_value InitSupportEnumPriorityType(napi_env env, napi_value exports);
    static napi_value InitSupportEnumVersionType(napi_env env, napi_value exports);
    static napi_value InitSupportEnumDispositionType(napi_env env, napi_value exports);
    static napi_value InitSupportEnumReportAllowedType(napi_env env, napi_value exports);
};
} // namespace Telephony
} // namespace OHOS
#endif // NAPI_SMS_H