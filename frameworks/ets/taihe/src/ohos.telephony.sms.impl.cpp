/*
 * Copyright (C) 2025-2025 Huawei Device Co., Ltd.
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

#include "ohos.telephony.sms.impl.hpp"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>
#include "ani_delivery_callback.h"
#include "ani_send_callback.h"
#include "ani_send_recv_mms.h"
#include "ohos.telephony.sms.proj.hpp"
#include "sms_mms_errors.h"
#include "sms_service_manager_client.h"
#include "stdexcept"
#include "string_ex.h"
#include "string_utils.h"
#include "taihe/runtime.hpp"
#include "taihe/string.hpp"
#include "telephony_errors.h"
#include "telephony_log_wrapper.h"
#include "telephony_permission.h"
#include "telephony_types.h"


using namespace taihe;
using namespace OHOS;
using namespace OHOS::Telephony;
namespace {
// To be implemented.
enum AniError : int32_t {
    /**
     * Indicates there is no error.
     */
    ERROR_NONE = 0,

    /**
     * Indicates the default value of taihe error.
     */
    ERROR_DEFAULT = -1,

    /**
     * Indicates the service is unavailable.
     */
    ERROR_SERVICE_UNAVAILABLE = -2,

    /**
     * Indicates the count of parameter is error.
     */
    ERROR_PARAMETER_COUNTS_INVALID = -4,

    /**
     * Indicates the type of parameter is error.
     */
    ERROR_PARAMETER_TYPE_INVALID = -5,

    /**
     * Indicates the taihe API execute failed.
     */
    ERROR_TAIHE_API_EXECUTE_FAIL = -6,

    /**
     * Indicates the slot id is invalid.
     */
    ERROR_SLOT_ID_INVALID = 202,
};

static const int32_t MESSAGE_UNKNOWN_STATUS = -1;
static const int32_t SMS_INVALID_SIM_ID = 0;
constexpr size_t BUFF_LENGTH = 1001;
static const int32_t MESSAGE_PARAMETER_NOT_MATCH = 0;

enum DispositionValue : int32_t {
    FROM_DATA = 0,
    ATTACHMENT = 1,
    INLINE = 2,
};

struct MmsAttachmentContext {
    std::string path = "";
    std::string fileName = "";
    std::string contentId = "";
    std::string contentLocation = "";
    std::string contentDisposition = "";
    std::string contentTransferEncoding = "";
    std::string contentType = "";
    bool isSmil = false;
    uint32_t inBuffLen = 0;
    std::unique_ptr<char[]> inBuff = nullptr;
    int32_t charset = ERROR_DEFAULT;
};

struct BaseContext {
    bool resolved = false;
    int32_t errorCode = ERROR_DEFAULT;
};

struct SendMessageContext : BaseContext {
    int32_t slotId = DEFAULT_SIM_SLOT_ID;
    std::u16string destinationHost = u"";
    std::u16string serviceCenter = u"";
    std::u16string textContent = u"";
    std::vector<uint8_t> rawDataContent;
    ani_object sendCallbackRef = nullptr;
    ani_object deliveryCallbackRef = nullptr;
    int32_t messageType = MESSAGE_PARAMETER_NOT_MATCH;
    uint16_t destinationPort = 0;
};

static bool IsValidSlotId(int32_t slotId)
{
    return ((slotId >= DEFAULT_SIM_SLOT_ID) && (slotId < SIM_SLOT_COUNT));
}

static ::taihe::array<uint8_t> VectorToArray(std::vector<unsigned char> vec)
{
    return ::taihe::array<uint8_t>(taihe::copy_data_t{}, vec.data(), vec.size());
}

std::unique_ptr<char[]> ArrayToUniquePtr(const ::taihe::array<uint8_t> &arr)
{
    const uint8_t *data = arr.data();
    size_t size = arr.size();
    auto ptr = std::make_unique<char[]>(size);
    std::copy(data, data + size, ptr.get());
    return ptr;
}

static bool GetMmsValueLength(const std::string &value)
{
    size_t len = value.length();
    return (len > 0) && (len < BUFF_LENGTH);
}

static void GetMmsContext(::ohos::telephony::sms::MmsParams const & mmsParams, MmsContext &mmsContext)
{
    mmsContext.slotId = mmsParams.slotId;
    if (GetMmsValueLength(std::string(mmsParams.mmsc))) {
        mmsContext.mmsc = StringUtils::ToUtf16(std::string(mmsParams.mmsc));
    }
    if (GetMmsValueLength(std::string(mmsParams.data))) {
        mmsContext.data = StringUtils::ToUtf16(std::string(mmsParams.data));
    }
    if (mmsParams.mmsConfig.has_value()) {
        auto ua = std::string(mmsParams.mmsConfig.value().userAgent);
        if (GetMmsValueLength(ua)) {
            mmsContext.mmsConfig.userAgent = StringUtils::ToUtf16(ua);
        }
        auto uaprof = std::string(mmsParams.mmsConfig.value().userAgentProfile);
        if (GetMmsValueLength(uaprof)) {
            mmsContext.mmsConfig.userAgentProfile = StringUtils::ToUtf16(uaprof);
        }
    }
}

static std::string ParseDispositionValue(int32_t value)
{
    switch (value) {
        case FROM_DATA:
            return "from-data";
        case ATTACHMENT:
            return "attachment";
        case INLINE:
            return "inline";
        default:
            TELEPHONY_LOGE("Invalid contentDisposition value");
            return "";
    }
}

static int32_t FormatDispositionValue(const std::string &value)
{
    if (std::string("from-data") == value) {
        return FROM_DATA;
    } else if (std::string("attachment") == value) {
        return ATTACHMENT;
    } else {
        return INLINE;
    }
}

static void GetAttachmentByDecodeMms(MmsMsg &mmsMsg,
    std::vector<ohos::telephony::sms::MmsAttachment> &mmsAttachmentArrayList)
{
    std::vector<OHOS::Telephony::MmsAttachment> attachment;
    mmsMsg.GetAllAttachment(attachment);
    if (attachment.empty()) {
        return;
    }
    for (auto it : attachment) {
        std::unique_ptr<char[]> buffer = nullptr;
        uint32_t nLen = 0;
        buffer = it.GetDataBuffer(nLen);
        ::taihe::array<uint8_t> inBuff(taihe::copy_data_t{}, buffer.get(), nLen);
        ohos::telephony::sms::MmsAttachment mmsAttachment = { it.GetContentId(),
            it.GetContentLocation(),
            ::ohos::telephony::sms::DispositionType::from_value(FormatDispositionValue(it.GetContentDisposition())),
            it.GetContentTransferEncoding(),
            it.GetContentType(),
            it.IsSmilFile(),
            ::taihe::optional<::taihe::string>(std::in_place, it.GetAttachmentFilePath().c_str()),
            ::taihe::optional<::taihe::array<uint8_t>>(std::in_place, inBuff),
            ::taihe::optional<::taihe::string>(std::in_place, it.GetFileName()),
            ::taihe::optional<::ohos::telephony::sms::MmsCharSets>(std::in_place,
            ::ohos::telephony::sms::MmsCharSets::from_value(it.GetCharSet())) };

        mmsAttachmentArrayList.push_back(std::move(mmsAttachment));
    }
}

bool HasSmsCapability()
{
    auto ret = OHOS::Singleton<SmsServiceManagerClient>::GetInstance().HasSmsCapability();
    return static_cast<ani_boolean>(ret);
}

void SendMmsSync(uintptr_t context, ::ohos::telephony::sms::MmsParams const & mmsParams)
{
    struct MmsContext mmsContext;
    GetMmsContext(mmsParams, mmsContext);
    auto errorCode = AniSendRecvMms::SendMms(context, mmsContext);
    if (errorCode != TELEPHONY_ERR_SUCCESS) {
        set_business_error(errorCode, "SendMmsSync failed");
    }
}

int32_t GetDefaultSmsSlotIdSync()
{
    int32_t slotId = Singleton<SmsServiceManagerClient>::GetInstance().GetDefaultSmsSlotId();
    if (!IsValidSlotId(slotId)) {
        set_business_error(TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL, "GetDefaultSmsSlotIdSync slotId is invalid");
    }
    return slotId;
}

int32_t GetDefaultSmsSimIdSync()
{
    int32_t simId = SMS_INVALID_SIM_ID;
    auto errorCode = Singleton<SmsServiceManagerClient>::GetInstance().GetDefaultSmsSimId(simId);
    if (errorCode != TELEPHONY_ERR_SUCCESS) {
        set_business_error(errorCode, "GetDefaultSmsSimIdSync SmsSimId is invalid");
    }
    return simId;
}

::ohos::telephony::sms::SmsSegmentsInfo GetSmsSegmentsInfoSync(int32_t slotId, ::taihe::string_view message,
    bool force7bit)
{
    ::ohos::telephony::sms::SmsEncodingScheme scheme(
        ::ohos::telephony::sms::SmsEncodingScheme::key_t::SMS_ENCODING_UNKNOWN);
    ::ohos::telephony::sms::SmsSegmentsInfo smsInfo{ -1, -1, -1, scheme };

    ISmsServiceInterface::SmsSegmentsInfo info;
    if (!IsValidSlotId(slotId)) {
        set_business_error(TELEPHONY_ERR_SLOTID_INVALID, "GetSmsSegmentsInfoSync slotId is invalid");
        return smsInfo;
    }
    std::u16string content = StringUtils::ToUtf16(message.c_str());
    uint32_t errorCode =
        Singleton<SmsServiceManagerClient>::GetInstance().GetSmsSegmentsInfo(slotId, content, force7bit, info);
    if (errorCode != TELEPHONY_ERR_SUCCESS) {
        set_business_error(errorCode, "GetSmsSegmentsInfoSync failed");
    } else {
        smsInfo.encodeCount = info.msgEncodingCount;
        smsInfo.encodeCountRemaining = info.msgRemainCount;
        smsInfo.splitCount = info.msgSegCount;
        smsInfo.scheme.from_value(static_cast<int32_t>(info.msgCodeScheme));
    }
    return smsInfo;
}

bool IsImsSmsSupportedSync(int32_t slotId)
{
    bool setResult = false;
    uint32_t errorCode = Singleton<SmsServiceManagerClient>::GetInstance().IsImsSmsSupported(slotId, setResult);
    if (errorCode != TELEPHONY_ERR_SUCCESS) {
        set_business_error(errorCode, "IsImsSmsSupportedSync failed");
    }
    return setResult;
}

::taihe::string GetImsShortMessageFormatSync()
{
    std::u16string value = u"";
    uint32_t errorCode = Singleton<SmsServiceManagerClient>::GetInstance().GetImsShortMessageFormat(value);
    if (errorCode != TELEPHONY_ERR_SUCCESS) {
        set_business_error(errorCode, "GetImsShortMessageFormatSync failed");
    }
    return taihe::string(StringUtils::ToUtf8(value).c_str());
}

void DownloadMmsSync(uintptr_t context, ::ohos::telephony::sms::MmsParams const & mmsParams)
{
    struct MmsContext mmsContext;
    GetMmsContext(mmsParams, mmsContext);
    auto errorCode = AniSendRecvMms::DownloadMms(context, mmsContext);
    if (errorCode != TELEPHONY_ERR_SUCCESS) {
        set_business_error(errorCode, "DownloadMmsSync failed");
    }
}

static void GetMmsSendConf(MmsMsg mmsMsg, ohos::telephony::sms::MmsInformation &mmsInfo)
{
    ::ohos::telephony::sms::MmsSendConf mmsTypeSendConf = { mmsMsg.GetHeaderOctetValue(
        MmsFieldCode::MMS_RESPONSE_STATUS),
        mmsMsg.GetHeaderStringValue(MmsFieldCode::MMS_TRANSACTION_ID),
        ::ohos::telephony::sms::MmsVersionType::from_value(mmsMsg.GetHeaderIntegerValue(MmsFieldCode::MMS_MMS_VERSION)),
        ::taihe::optional<::taihe::string>(std::in_place, mmsMsg.GetHeaderStringValue(MmsFieldCode::MMS_MESSAGE_ID)) };

    mmsInfo.mmsType = ohos::telephony::sms::MmsType::make_mmsTypeSendConf(mmsTypeSendConf);
}

static void GetMmsSendReq(MmsMsg mmsMsg, ohos::telephony::sms::MmsInformation &mmsInfo)
{
    OHOS::Telephony::MmsAddress mmsFrom = mmsMsg.GetMmsFrom();
    ::ohos::telephony::sms::MmsAddress fromAddress = { mmsFrom.GetAddressString(),
        ::ohos::telephony::sms::MmsCharSets::from_value(static_cast<int32_t>(mmsFrom.GetAddressCharset())) };

    std::vector<OHOS::Telephony::MmsAddress> toAddrs;
    mmsMsg.GetMmsTo(toAddrs);
    ::ohos::telephony::sms::MmsAddress oneAddress = { "", ::ohos::telephony::sms::MmsCharSets::key_t::UTF_8 };
    ::taihe::array<::ohos::telephony::sms::MmsAddress> to(toAddrs.size(), oneAddress);
    for (size_t i = 0; i < toAddrs.size(); i++) {
        to[i].address = toAddrs[i].GetAddressString();
        to[i].charset.from_value(static_cast<int32_t>(toAddrs[i].GetAddressCharset()));
    }

    std::vector<OHOS::Telephony::MmsAddress> ccAddrs;
    mmsMsg.GetHeaderAllAddressValue(MmsFieldCode::MMS_CC, ccAddrs);
    ::taihe::array<::ohos::telephony::sms::MmsAddress> cc(ccAddrs.size(), oneAddress);
    for (size_t i = 0; i < ccAddrs.size(); i++) {
        cc[i].address = ccAddrs[i].GetAddressString();
        cc[i].charset.from_value(static_cast<int32_t>(ccAddrs[i].GetAddressCharset()));
    }

    std::vector<OHOS::Telephony::MmsAddress> bccAddrs;
    mmsMsg.GetHeaderAllAddressValue(MmsFieldCode::MMS_BCC, bccAddrs);
    ::taihe::array<::ohos::telephony::sms::MmsAddress> bcc(bccAddrs.size(), oneAddress);
    for (size_t i = 0; i < bccAddrs.size(); i++) {
        bcc[i].address = bccAddrs[i].GetAddressString();
        bcc[i].charset.from_value(static_cast<int32_t>(bccAddrs[i].GetAddressCharset()));
    }

    ::ohos::telephony::sms::MmsVersionType version =
        ::ohos::telephony::sms::MmsVersionType::from_value(static_cast<int32_t>(mmsMsg.GetMmsVersion()));
    ::ohos::telephony::sms::MmsPriorityType priority = ::ohos::telephony::sms::MmsPriorityType::from_value(
        static_cast<int32_t>(mmsMsg.GetHeaderOctetValue(MmsFieldCode::MMS_PRIORITY)));

    ::ohos::telephony::sms::MmsSendReq mmsTypeSendReq = { fromAddress,
        mmsMsg.GetMmsTransactionId(),
        mmsMsg.GetHeaderContentType(),
        version,
        ::taihe::optional<::taihe::array<::ohos::telephony::sms::MmsAddress>>(std::in_place, to),
        ::taihe::optional<int32_t>(std::in_place, mmsMsg.GetMmsDate()),
        ::taihe::optional<::taihe::array<::ohos::telephony::sms::MmsAddress>>(std::in_place, cc),
        ::taihe::optional<::taihe::array<::ohos::telephony::sms::MmsAddress>>(std::in_place, bcc),
        ::taihe::optional<::taihe::string>(std::in_place, mmsMsg.GetMmsSubject()),
        ::taihe::optional<int32_t>(std::in_place, mmsMsg.GetHeaderOctetValue(MmsFieldCode::MMS_MESSAGE_CLASS)),
        ::taihe::optional<int32_t>(std::in_place, mmsMsg.GetHeaderIntegerValue(MmsFieldCode::MMS_EXPIRY)),
        ::taihe::optional<::ohos::telephony::sms::MmsPriorityType>(std::in_place, priority),
        ::taihe::optional<int32_t>(std::in_place, mmsMsg.GetHeaderOctetValue(MmsFieldCode::MMS_SENDER_VISIBILITY)),
        ::taihe::optional<int32_t>(std::in_place, mmsMsg.GetHeaderOctetValue(MmsFieldCode::MMS_DELIVERY_REPORT)),
        ::taihe::optional<int32_t>(std::in_place, mmsMsg.GetHeaderOctetValue(MmsFieldCode::MMS_READ_REPORT)) };
    mmsInfo.mmsType = ohos::telephony::sms::MmsType::make_mmsTypeSendReq(mmsTypeSendReq);
}

static void GetMmsNotificationInd(MmsMsg mmsMsg, ohos::telephony::sms::MmsInformation &mmsInfo)
{
    OHOS::Telephony::MmsAddress mmsFrom = mmsMsg.GetMmsFrom();
    ::ohos::telephony::sms::MmsAddress fromAddress = { mmsFrom.GetAddressString(),
        ::ohos::telephony::sms::MmsCharSets::from_value(static_cast<int32_t>(mmsFrom.GetAddressCharset())) };

    ::ohos::telephony::sms::MmsVersionType version =
        ::ohos::telephony::sms::MmsVersionType::from_value(static_cast<int32_t>(mmsMsg.GetMmsVersion()));
    ::ohos::telephony::sms::MmsNotificationInd mmsTypeNotificationInd = { mmsMsg.GetMmsTransactionId(),
        mmsMsg.GetHeaderOctetValue(MmsFieldCode::MMS_MESSAGE_CLASS),
        mmsMsg.GetHeaderLongValue(MmsFieldCode::MMS_MESSAGE_SIZE),
        mmsMsg.GetHeaderIntegerValue(MmsFieldCode::MMS_EXPIRY),
        mmsMsg.GetHeaderStringValue(MmsFieldCode::MMS_CONTENT_LOCATION),
        version,
        ::taihe::optional<::ohos::telephony::sms::MmsAddress>(std::in_place, fromAddress),
        ::taihe::optional<::taihe::string>(std::in_place, mmsMsg.GetMmsSubject()),
        ::taihe::optional<int32_t>(std::in_place, mmsMsg.GetHeaderOctetValue(MmsFieldCode::MMS_DELIVERY_REPORT)),
        ::taihe::optional<int32_t>(std::in_place, mmsMsg.GetHeaderOctetValue(MmsFieldCode::MMS_CONTENT_CLASS)) };
    mmsInfo.mmsType.make_mmsTypeNotificationInd(mmsTypeNotificationInd);
}

static void GetMmsRespInd(MmsMsg mmsMsg, ohos::telephony::sms::MmsInformation &mmsInfo)
{
    ::ohos::telephony::sms::MmsVersionType version =
        ::ohos::telephony::sms::MmsVersionType::from_value(static_cast<int32_t>(mmsMsg.GetMmsVersion()));
    ::ohos::telephony::sms::MmsRespInd mmsTypeRespInd = { mmsMsg.GetMmsTransactionId(),
        mmsMsg.GetHeaderOctetValue(MmsFieldCode::MMS_STATUS), version,
        ::taihe::optional<::ohos::telephony::sms::ReportType>(std::in_place,
        ::ohos::telephony::sms::ReportType::from_value(
            static_cast<int32_t>(mmsMsg.GetHeaderOctetValue(MmsFieldCode::MMS_REPORT_ALLOWED))))};
    mmsInfo.mmsType = ohos::telephony::sms::MmsType::make_mmsTypeRespInd(mmsTypeRespInd);
}

static void GetMmsRetrieveConf(MmsMsg mmsMsg, ohos::telephony::sms::MmsInformation &mmsInfo)
{
    std::vector<OHOS::Telephony::MmsAddress> toAddrs;
    mmsMsg.GetMmsTo(toAddrs);
    ::ohos::telephony::sms::MmsAddress oneAddress = { "", ::ohos::telephony::sms::MmsCharSets::key_t::UTF_8 };
    ::taihe::array<::ohos::telephony::sms::MmsAddress> to(toAddrs.size(), oneAddress);
    for (size_t i = 0; i < toAddrs.size(); i++) {
        to[i].address = toAddrs[i].GetAddressString();
        to[i].charset.from_value(static_cast<int32_t>(toAddrs[i].GetAddressCharset()));
    }

    ::ohos::telephony::sms::MmsVersionType version =
        ::ohos::telephony::sms::MmsVersionType::from_value(static_cast<int32_t>(mmsMsg.GetMmsVersion()));
    OHOS::Telephony::MmsAddress mmsFrom = mmsMsg.GetMmsFrom();
    ::ohos::telephony::sms::MmsAddress fromAddress = { mmsFrom.GetAddressString(),
        ::ohos::telephony::sms::MmsCharSets::from_value(static_cast<int32_t>(mmsFrom.GetAddressCharset())) };

    std::vector<OHOS::Telephony::MmsAddress> ccAddrs;
    mmsMsg.GetHeaderAllAddressValue(MmsFieldCode::MMS_CC, ccAddrs);
    ::taihe::array<::ohos::telephony::sms::MmsAddress> cc(ccAddrs.size(), oneAddress);
    for (size_t i = 0; i < ccAddrs.size(); i++) {
        cc[i].address = ccAddrs[i].GetAddressString();
        cc[i].charset.from_value(static_cast<int32_t>(ccAddrs[i].GetAddressCharset()));
    }

    ::ohos::telephony::sms::MmsPriorityType priority = ::ohos::telephony::sms::MmsPriorityType::from_value(
        static_cast<int32_t>(mmsMsg.GetHeaderOctetValue(MmsFieldCode::MMS_PRIORITY)));

    ::ohos::telephony::sms::MmsRetrieveConf mmsTypeRetrieveConf = { mmsMsg.GetMmsTransactionId(),
        mmsMsg.GetHeaderStringValue(MmsFieldCode::MMS_MESSAGE_ID),
        mmsMsg.GetMmsDate(),
        mmsMsg.GetHeaderContentType(),
        to,
        version,
        ::taihe::optional<::ohos::telephony::sms::MmsAddress>(std::in_place, fromAddress),
        ::taihe::optional<::taihe::array<::ohos::telephony::sms::MmsAddress>>(std::in_place, cc),
        ::taihe::optional<::taihe::string>(std::in_place, mmsMsg.GetMmsSubject()),
        ::taihe::optional<::ohos::telephony::sms::MmsPriorityType>(std::in_place, priority),
        ::taihe::optional<int32_t>(std::in_place, mmsMsg.GetHeaderOctetValue(MmsFieldCode::MMS_DELIVERY_REPORT)),
        ::taihe::optional<int32_t>(std::in_place, mmsMsg.GetHeaderOctetValue(MmsFieldCode::MMS_READ_REPORT)),
        ::taihe::optional<int32_t>(std::in_place, mmsMsg.GetHeaderOctetValue(MmsFieldCode::MMS_RETRIEVE_STATUS)),
        ::taihe::optional<::taihe::string>(std::in_place,
        mmsMsg.GetHeaderStringValue(MmsFieldCode::MMS_RETRIEVE_TEXT)) };
    mmsInfo.mmsType = ohos::telephony::sms::MmsType::make_mmsTypeRetrieveConf(mmsTypeRetrieveConf);
}

static void GetMmsAcknowledgeInd(MmsMsg mmsMsg, ohos::telephony::sms::MmsInformation &mmsInfo)
{
    ::ohos::telephony::sms::MmsVersionType version =
        ::ohos::telephony::sms::MmsVersionType::from_value(static_cast<int32_t>(mmsMsg.GetMmsVersion()));
    ::ohos::telephony::sms::ReportType reportAllowed = ::ohos::telephony::sms::ReportType::from_value(
        static_cast<int32_t>(mmsMsg.GetHeaderOctetValue(MmsFieldCode::MMS_REPORT_ALLOWED)));

    ::ohos::telephony::sms::MmsAcknowledgeInd mmsTypeAcknowledgeInd = { mmsMsg.GetMmsTransactionId(), version,
        ::taihe::optional<::ohos::telephony::sms::ReportType>(std::in_place, reportAllowed) };
    mmsInfo.mmsType = ohos::telephony::sms::MmsType::make_mmsTypeAcknowledgeInd(mmsTypeAcknowledgeInd);
}

static void GetMmsDeliveryInd(MmsMsg mmsMsg, ohos::telephony::sms::MmsInformation &mmsInfo)
{
    std::vector<OHOS::Telephony::MmsAddress> toAddrs;
    mmsMsg.GetMmsTo(toAddrs);
    ::ohos::telephony::sms::MmsAddress oneAddress = { "", ::ohos::telephony::sms::MmsCharSets::key_t::UTF_8 };
    ::taihe::array<::ohos::telephony::sms::MmsAddress> to(toAddrs.size(), oneAddress);
    for (size_t i = 0; i < toAddrs.size(); i++) {
        to[i].address = toAddrs[i].GetAddressString();
        to[i].charset.from_value(static_cast<int32_t>(toAddrs[i].GetAddressCharset()));
    }
    ::ohos::telephony::sms::MmsVersionType version =
        ::ohos::telephony::sms::MmsVersionType::from_value(static_cast<int32_t>(mmsMsg.GetMmsVersion()));

    ::ohos::telephony::sms::MmsDeliveryInd mmsTypeDeliveryInd = { mmsMsg.GetHeaderStringValue(
        MmsFieldCode::MMS_MESSAGE_ID),
        mmsMsg.GetMmsDate(), to, mmsMsg.GetHeaderOctetValue(MmsFieldCode::MMS_STATUS), version };
    mmsInfo.mmsType = ohos::telephony::sms::MmsType::make_mmsTypeDeliveryInd(mmsTypeDeliveryInd);
}

static void GetMmsReadOrigInd(MmsMsg mmsMsg, ohos::telephony::sms::MmsInformation &mmsInfo)
{
    ::ohos::telephony::sms::MmsVersionType version =
        ::ohos::telephony::sms::MmsVersionType::from_value(static_cast<int32_t>(mmsMsg.GetMmsVersion()));
    std::vector<OHOS::Telephony::MmsAddress> toAddrs;
    mmsMsg.GetMmsTo(toAddrs);
    ::ohos::telephony::sms::MmsAddress oneAddress = { "", ::ohos::telephony::sms::MmsCharSets::key_t::UTF_8 };
    ::taihe::array<::ohos::telephony::sms::MmsAddress> to(toAddrs.size(), oneAddress);
    for (size_t i = 0; i < toAddrs.size(); i++) {
        to[i].address = toAddrs[i].GetAddressString();
        to[i].charset.from_value(static_cast<int32_t>(toAddrs[i].GetAddressCharset()));
    }

    OHOS::Telephony::MmsAddress mmsFrom = mmsMsg.GetMmsFrom();
    ::ohos::telephony::sms::MmsAddress fromAddress = { mmsFrom.GetAddressString(),
        ::ohos::telephony::sms::MmsCharSets::from_value(static_cast<int32_t>(mmsFrom.GetAddressCharset())) };

    ::ohos::telephony::sms::MmsReadOrigInd mmsTypeReadOrigInd = { version,
        mmsMsg.GetHeaderStringValue(MmsFieldCode::MMS_MESSAGE_ID),
        to,
        fromAddress,
        mmsMsg.GetMmsDate(),
        mmsMsg.GetHeaderOctetValue(MmsFieldCode::MMS_READ_STATUS) };
    mmsInfo.mmsType = ohos::telephony::sms::MmsType::make_mmsTypeReadOrigInd(mmsTypeReadOrigInd);
}

static void GetMmsReadRecInd(MmsMsg mmsMsg, ohos::telephony::sms::MmsInformation &mmsInfo)
{
    ::ohos::telephony::sms::MmsVersionType version =
        ::ohos::telephony::sms::MmsVersionType::from_value(static_cast<int32_t>(mmsMsg.GetMmsVersion()));
    std::vector<OHOS::Telephony::MmsAddress> toAddrs;
    mmsMsg.GetMmsTo(toAddrs);
    ::ohos::telephony::sms::MmsAddress oneAddress = { "", ::ohos::telephony::sms::MmsCharSets::key_t::UTF_8 };
    ::taihe::array<::ohos::telephony::sms::MmsAddress> to(toAddrs.size(), oneAddress);
    for (size_t i = 0; i < toAddrs.size(); i++) {
        to[i].address = toAddrs[i].GetAddressString();
        to[i].charset.from_value(static_cast<int32_t>(toAddrs[i].GetAddressCharset()));
    }

    OHOS::Telephony::MmsAddress mmsFrom = mmsMsg.GetMmsFrom();
    ::ohos::telephony::sms::MmsAddress fromAddress = { mmsFrom.GetAddressString(),
        ::ohos::telephony::sms::MmsCharSets::from_value(static_cast<int32_t>(mmsFrom.GetAddressCharset())) };
    ::ohos::telephony::sms::MmsReadRecInd mmsTypeReadRecInd = { version,
        mmsMsg.GetHeaderStringValue(MmsFieldCode::MMS_MESSAGE_ID),
        to,
        fromAddress,
        mmsMsg.GetHeaderOctetValue(MmsFieldCode::MMS_READ_STATUS),
        ::taihe::optional<int32_t>(std::in_place, mmsMsg.GetMmsDate()) };
    mmsInfo.mmsType = ohos::telephony::sms::MmsType::make_mmsTypeReadRecInd(mmsTypeReadRecInd);
}

static void GetMmsValueByMessageType(const ::ohos::telephony::sms::MessageType messageType, const MmsMsg mmsMsg,
    ohos::telephony::sms::MmsInformation &mmsInfo)
{
    if (messageType.get_key() == ::ohos::telephony::sms::MessageType::key_t::TYPE_MMS_SEND_CONF) {
        GetMmsSendConf(mmsMsg, mmsInfo);
    } else if (messageType.get_key() == ::ohos::telephony::sms::MessageType::key_t::TYPE_MMS_SEND_REQ) {
        GetMmsSendReq(mmsMsg, mmsInfo);
    } else if (messageType.get_key() == ::ohos::telephony::sms::MessageType::key_t::TYPE_MMS_NOTIFICATION_IND) {
        GetMmsNotificationInd(mmsMsg, mmsInfo);
    } else if (messageType.get_key() == ::ohos::telephony::sms::MessageType::key_t::TYPE_MMS_RESP_IND) {
        GetMmsRespInd(mmsMsg, mmsInfo);
    } else if (messageType.get_key() == ::ohos::telephony::sms::MessageType::key_t::TYPE_MMS_RETRIEVE_CONF) {
        GetMmsRetrieveConf(mmsMsg, mmsInfo);
    } else if (messageType.get_key() == ::ohos::telephony::sms::MessageType::key_t::TYPE_MMS_ACKNOWLEDGE_IND) {
        GetMmsAcknowledgeInd(mmsMsg, mmsInfo);
    } else if (messageType.get_key() == ::ohos::telephony::sms::MessageType::key_t::TYPE_MMS_DELIVERY_IND) {
        GetMmsDeliveryInd(mmsMsg, mmsInfo);
    } else if (messageType.get_key() == ::ohos::telephony::sms::MessageType::key_t::TYPE_MMS_READ_ORIG_IND) {
        GetMmsReadOrigInd(mmsMsg, mmsInfo);
    } else if (messageType.get_key() == ::ohos::telephony::sms::MessageType::key_t::TYPE_MMS_READ_REC_IND) {
        GetMmsReadRecInd(mmsMsg, mmsInfo);
    }
}

::ohos::telephony::sms::MmsInformation DecodeMmsSync(::ohos::telephony::sms::FilePathName const & mmsFilePathName)
{
    ::ohos::telephony::sms::MessageType messageType =
        static_cast<::ohos::telephony::sms::MessageType::key_t>(TELEPHONY_ERROR);
    ::ohos::telephony::sms::MmsAcknowledgeInd mmsTypeAcknowledgeInd = { ::taihe::string(""),
        ::ohos::telephony::sms::MmsVersionType::from_value(0),
        ::taihe::optional<::ohos::telephony::sms::ReportType>(std::in_place,
        ::ohos::telephony::sms::ReportType::from_value(0)) };
    ::ohos::telephony::sms::MmsType mmsType =
        ::ohos::telephony::sms::MmsType::make_mmsTypeAcknowledgeInd(mmsTypeAcknowledgeInd);

    std::vector<ohos::telephony::sms::MmsAttachment> mmsAttachmentArrayList;
    ohos::telephony::sms::MmsInformation mmsInfo = { messageType, mmsType,
        ::taihe::optional<::taihe::array<ohos::telephony::sms::MmsAttachment>>(std::nullopt) };

    if (!TelephonyPermission::CheckCallerIsSystemApp()) {
        set_business_error(TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API, "Non-system applications use system APIs!");
        return mmsInfo;
    }
    MmsMsg mmsMsg;
    bool mmsResult = false;
    const ::taihe::string *pstrMmsPath = mmsFilePathName.get_pathNameString_ptr();
    const ::taihe::array<uint8_t> *parrayMmsPath = mmsFilePathName.get_pathNameArray_ptr();
    if (pstrMmsPath != nullptr && !pstrMmsPath->empty()) {
        mmsResult = mmsMsg.DecodeMsg(mmsFilePathName.get_pathNameString_ref().c_str());
    } else if (parrayMmsPath != nullptr && !parrayMmsPath->empty()) {
        auto unique_ptr = std::make_unique<char[]>(mmsFilePathName.get_pathNameArray_ref().size());
        std::copy(mmsFilePathName.get_pathNameArray_ref().begin(), mmsFilePathName.get_pathNameArray_ref().end(),
            unique_ptr.get());
        mmsResult = mmsMsg.DecodeMsg(std::move(unique_ptr), mmsFilePathName.get_pathNameArray_ref().size());
    }
    if (!mmsResult) {
        set_business_error(TELEPHONY_ERR_FAIL, "DecodeMsg error!");
        return mmsInfo;
    }
    mmsMsg.DumpMms();
    messageType = ::ohos::telephony::sms::MessageType::from_value(static_cast<int32_t>(mmsMsg.GetMmsMessageType()));
    GetMmsValueByMessageType(messageType, mmsMsg, mmsInfo);
    GetAttachmentByDecodeMms(mmsMsg, mmsAttachmentArrayList);
    mmsInfo.attachment = ::taihe::optional<::taihe::array<ohos::telephony::sms::MmsAttachment>>(std::in_place,
        ::taihe::array<ohos::telephony::sms::MmsAttachment>(taihe::copy_data_t{}, mmsAttachmentArrayList.data(),
        mmsAttachmentArrayList.size()));
    return mmsInfo;
}

static bool SetAttachmentToCore(MmsMsg &mmsMsg, std::vector<MmsAttachmentContext> &attachment)
{
    if (attachment.size() > 0) {
        for (auto it = attachment.begin(); it != attachment.end(); it++) {
            MmsAttachment itAttachment;
            if (it->path.size() > 0) {
                itAttachment.SetAttachmentFilePath(it->path, it->isSmil);
            }
            itAttachment.SetIsSmilFile(it->isSmil);
            if (it->fileName.size() > 0) {
                itAttachment.SetFileName(it->fileName);
            }
            if (it->contentId.size() > 0) {
                itAttachment.SetContentId(it->contentId);
            }
            if (it->contentLocation.size() > 0) {
                itAttachment.SetContentLocation(it->contentLocation);
            }
            if (it->contentDisposition.size() > 0) {
                itAttachment.SetContentDisposition(it->contentDisposition);
            }
            if (it->contentTransferEncoding.size() > 0) {
                itAttachment.SetContentTransferEncoding(it->contentTransferEncoding);
            }
            if (it->contentType.size() > 0) {
                itAttachment.SetContentType(it->contentType);
            }
            if (it->charset != ERROR_DEFAULT) {
                itAttachment.SetCharSet(it->charset);
            }
            if (it->inBuffLen > 0) {
                itAttachment.SetDataBuffer(std::move(it->inBuff), it->inBuffLen);
            }
            if (!mmsMsg.AddAttachment(itAttachment)) {
                TELEPHONY_LOGE("attachment file error");
                return false;
            }
        }
    }
    return true;
}

static void SendReqToCoreSetMmsTo(const ::ohos::telephony::sms::MmsSendReq sendReq, MmsMsg &mmsMsg)
{
    if (sendReq.to->size() > 0) {
        std::vector<MmsAddress> toAddrs;
        for (size_t i = 0; i < sendReq.to->size(); i++) {
            MmsAddress oneAddress;
            OHOS::Telephony::MmsCharSets charset = static_cast<MmsCharSets>(sendReq.to->at(i).charset.get_value());
            oneAddress.SetMmsAddressString(sendReq.to->at(i).address.c_str(), charset);
            toAddrs.push_back(oneAddress);
        }
        mmsMsg.SetMmsTo(toAddrs);
    }
}

static void SendReqToCoreAddHeaderAddressValue(const ::ohos::telephony::sms::MmsSendReq sendReq, MmsMsg &mmsMsg)
{
    if (sendReq.cc->size() > 0) {
        for (::ohos::telephony::sms::MmsAddress ccAddress : sendReq.cc.value()) {
            OHOS::Telephony::MmsAddress oneAddress;
            OHOS::Telephony::MmsCharSets ccCharset = static_cast<MmsCharSets>(ccAddress.charset.get_value());
            oneAddress.SetMmsAddressString(ccAddress.address.c_str(), ccCharset);
            mmsMsg.AddHeaderAddressValue(MmsFieldCode::MMS_CC, oneAddress);
        }
    }

    if (sendReq.bcc->size() > 0) {
        for (::ohos::telephony::sms::MmsAddress bccAddress : sendReq.bcc.value()) {
            OHOS::Telephony::MmsAddress oneAddress;
            OHOS::Telephony::MmsCharSets bccCharset = static_cast<MmsCharSets>(bccAddress.charset.get_value());
            oneAddress.SetMmsAddressString(bccAddress.address.c_str(), bccCharset);
            mmsMsg.AddHeaderAddressValue(MmsFieldCode::MMS_CC, oneAddress);
        }
    }
}

static void SetSendReqToCore(MmsMsg &mmsMsg, ::ohos::telephony::sms::MmsInformation const & mms)
{
    auto sendReq = mms.mmsType.get_mmsTypeSendReq_ref();
    OHOS::Telephony::MmsCharSets fromCharset = static_cast<MmsCharSets>(sendReq.fromAddress.charset.get_value());
    MmsAddress address(sendReq.fromAddress.address.c_str(), fromCharset);
    mmsMsg.SetMmsFrom(address);
    SendReqToCoreSetMmsTo(sendReq, mmsMsg);
    if (sendReq.transactionId.size() > 0) {
        mmsMsg.SetMmsTransactionId(sendReq.transactionId.c_str());
    }
    if (sendReq.version > 0) {
        mmsMsg.SetMmsVersion(sendReq.version.get_value());
    }
    if (sendReq.date.value() > 0) {
        mmsMsg.SetMmsDate(sendReq.date.value());
    }
    SendReqToCoreAddHeaderAddressValue(sendReq, mmsMsg);
    if (sendReq.subject->size() > 0) {
        mmsMsg.SetMmsSubject(sendReq.subject.value().c_str());
    }
    if (sendReq.messageClass.value() > 0) {
        mmsMsg.SetHeaderOctetValue(MmsFieldCode::MMS_MESSAGE_CLASS, sendReq.messageClass.value());
    }
    if (sendReq.expiry.value() > 0) {
        mmsMsg.SetHeaderIntegerValue(MmsFieldCode::MMS_EXPIRY, sendReq.expiry.value());
    }
    if (sendReq.priority.value() > 0) {
        mmsMsg.SetHeaderOctetValue(MmsFieldCode::MMS_PRIORITY, sendReq.priority.value());
    }
    if (sendReq.senderVisibility.value() > 0) {
        mmsMsg.SetHeaderOctetValue(MmsFieldCode::MMS_SENDER_VISIBILITY, sendReq.senderVisibility.value());
    }
    mmsMsg.SetHeaderOctetValue(MmsFieldCode::MMS_DELIVERY_REPORT, sendReq.deliveryReport.value());
    mmsMsg.SetHeaderOctetValue(MmsFieldCode::MMS_READ_REPORT, sendReq.readReport.value());
    mmsMsg.SetHeaderContentType(sendReq.contentType.c_str());
}

static void SetSendConfToCore(MmsMsg &mmsMsg, ::ohos::telephony::sms::MmsInformation const & mms)
{
    auto context = mms.mmsType.get_mmsTypeSendConf_ref();
    mmsMsg.SetHeaderOctetValue(MmsFieldCode::MMS_RESPONSE_STATUS, context.responseState);
    mmsMsg.SetMmsTransactionId(context.transactionId.c_str());
    mmsMsg.SetMmsVersion(context.version.get_value());
    std::string value(context.messageId.value().c_str());
    mmsMsg.SetHeaderStringValue(MmsFieldCode::MMS_MESSAGE_ID, value);
}

static void SetNotificationIndToCore(MmsMsg &mmsMsg, ::ohos::telephony::sms::MmsInformation const & mms)
{
    auto context = mms.mmsType.get_mmsTypeNotificationInd_ref();
    mmsMsg.SetMmsTransactionId(context.transactionId.c_str());
    mmsMsg.SetHeaderOctetValue(MmsFieldCode::MMS_MESSAGE_CLASS, context.messageClass);
    mmsMsg.SetHeaderLongValue(MmsFieldCode::MMS_MESSAGE_SIZE, context.messageSize);
    mmsMsg.SetHeaderIntegerValue(MmsFieldCode::MMS_EXPIRY, context.expiry);
    mmsMsg.SetMmsVersion(context.version.get_value());

    if (context.fromAddress.has_value()) {
        OHOS::Telephony::MmsCharSets charset = static_cast<MmsCharSets>(context.fromAddress->charset.get_value());
        MmsAddress address(context.fromAddress->address.c_str(), charset);
        mmsMsg.SetMmsFrom(address);
    }

    mmsMsg.SetMmsSubject(context.subject.value().c_str());
    mmsMsg.SetHeaderOctetValue(MmsFieldCode::MMS_DELIVERY_REPORT, context.deliveryReport.value());
    std::string value(context.contentLocation.c_str());
    mmsMsg.SetHeaderStringValue(MmsFieldCode::MMS_CONTENT_LOCATION, value);
    mmsMsg.SetHeaderOctetValue(MmsFieldCode::MMS_CONTENT_CLASS, context.contentClass.value());
}

static void SetRespIndToCore(MmsMsg &mmsMsg, ::ohos::telephony::sms::MmsInformation const & mms)
{
    auto context = mms.mmsType.get_mmsTypeRespInd_ref();
    mmsMsg.SetMmsTransactionId(context.transactionId.c_str());
    mmsMsg.SetHeaderOctetValue(MmsFieldCode::MMS_STATUS, context.status);
    mmsMsg.SetMmsVersion(context.version.get_value());
    mmsMsg.SetHeaderOctetValue(MmsFieldCode::MMS_REPORT_ALLOWED, context.reportAllowed->get_value());
}

static void SetRetrieveConfToCore(MmsMsg &mmsMsg, ::ohos::telephony::sms::MmsInformation const & mms)
{
    auto context = mms.mmsType.get_mmsTypeRetrieveConf_ref();
    mmsMsg.SetMmsTransactionId(context.transactionId.c_str());
    std::string value(context.messageId.c_str());
    mmsMsg.SetHeaderStringValue(MmsFieldCode::MMS_MESSAGE_ID, value);
    mmsMsg.SetMmsDate(context.date);
    mmsMsg.SetMmsVersion(context.version.get_value());

    std::vector<MmsAddress> toAddrs;
    for (size_t i = 0; i < context.to.size(); i++) {
        MmsAddress oneAddress;
        OHOS::Telephony::MmsCharSets charset = static_cast<MmsCharSets>(context.to.at(i).charset.get_value());
        oneAddress.SetMmsAddressString(context.to.at(i).address.c_str(), charset);
        toAddrs.push_back(oneAddress);
    }
    mmsMsg.SetMmsTo(toAddrs);

    if (context.fromAddress.has_value()) {
        OHOS::Telephony::MmsCharSets charset = static_cast<MmsCharSets>(context.fromAddress->charset.get_value());
        MmsAddress address(context.fromAddress->address.c_str(), charset);
        mmsMsg.SetMmsFrom(address);
    }

    if (context.cc->size() > 0) {
        for (::ohos::telephony::sms::MmsAddress ccAddress : context.cc.value()) {
            OHOS::Telephony::MmsAddress oneAddress;
            OHOS::Telephony::MmsCharSets charset = static_cast<MmsCharSets>(ccAddress.charset.get_value());
            oneAddress.SetMmsAddressString(ccAddress.address.c_str(), charset);
            mmsMsg.AddHeaderAddressValue(MmsFieldCode::MMS_CC, oneAddress);
        }
    }

    mmsMsg.SetMmsSubject(context.subject->c_str());
    mmsMsg.SetHeaderOctetValue(MmsFieldCode::MMS_PRIORITY, context.priority->get_value());
    mmsMsg.SetHeaderOctetValue(MmsFieldCode::MMS_DELIVERY_REPORT, context.deliveryReport.value());
    mmsMsg.SetHeaderOctetValue(MmsFieldCode::MMS_READ_REPORT, context.readReport.value());
    mmsMsg.SetHeaderOctetValue(MmsFieldCode::MMS_RETRIEVE_STATUS, context.retrieveStatus.value());
    if (!context.retrieveText->empty()) {
        std::string retrieveText(context.retrieveText->c_str());
        mmsMsg.SetHeaderEncodedStringValue(MmsFieldCode::MMS_RETRIEVE_TEXT, retrieveText, (uint32_t)MmsCharSets::UTF_8);
    }
    mmsMsg.SetHeaderContentType(context.contentType.c_str());
}

static void SetAcknowledgeIndToCore(MmsMsg &mmsMsg, ::ohos::telephony::sms::MmsInformation const & mms)
{
    auto context = mms.mmsType.get_mmsTypeAcknowledgeInd_ref();
    mmsMsg.SetMmsTransactionId(context.transactionId.c_str());
    mmsMsg.SetMmsVersion(context.version.get_value());
    mmsMsg.SetHeaderOctetValue(MmsFieldCode::MMS_REPORT_ALLOWED, context.reportAllowed->get_value());
}

static void SetDeliveryIndToCore(MmsMsg &mmsMsg, ::ohos::telephony::sms::MmsInformation const & mms)
{
    auto context = mms.mmsType.get_mmsTypeDeliveryInd_ref();
    std::string value(context.messageId.c_str());
    mmsMsg.SetHeaderStringValue(MmsFieldCode::MMS_MESSAGE_ID, value);
    mmsMsg.SetMmsDate(context.date);
    std::vector<MmsAddress> toAddrs;
    for (size_t i = 0; i < context.to.size(); i++) {
        MmsAddress oneAddress;
        OHOS::Telephony::MmsCharSets charset = static_cast<MmsCharSets>(context.to.at(i).charset.get_value());
        oneAddress.SetMmsAddressString(context.to.at(i).address.c_str(), charset);
        toAddrs.push_back(oneAddress);
    }
    mmsMsg.SetMmsTo(toAddrs);
    mmsMsg.SetHeaderOctetValue(MmsFieldCode::MMS_STATUS, context.status);
    mmsMsg.SetMmsVersion(context.version.get_value());
}

static void SetReadOrigIndToCore(MmsMsg &mmsMsg, ::ohos::telephony::sms::MmsInformation const & mms)
{
    auto context = mms.mmsType.get_mmsTypeReadOrigInd_ref();
    mmsMsg.SetMmsVersion(context.version.get_value());
    std::string value(context.messageId.c_str());
    mmsMsg.SetHeaderStringValue(MmsFieldCode::MMS_MESSAGE_ID, value);
    std::vector<MmsAddress> toAddrs;
    for (size_t i = 0; i < context.to.size(); i++) {
        MmsAddress oneAddress;
        OHOS::Telephony::MmsCharSets charset = static_cast<MmsCharSets>(context.to.at(i).charset.get_value());
        oneAddress.SetMmsAddressString(context.to.at(i).address.c_str(), charset);
        toAddrs.push_back(oneAddress);
    }
    mmsMsg.SetMmsTo(toAddrs);

    OHOS::Telephony::MmsCharSets charset = static_cast<MmsCharSets>(context.fromAddress.charset.get_value());
    MmsAddress address(context.fromAddress.address.c_str(), charset);
    mmsMsg.SetMmsFrom(address);

    if (context.date != 0) {
        mmsMsg.SetMmsDate(context.date);
    }
    mmsMsg.SetHeaderOctetValue(MmsFieldCode::MMS_READ_STATUS, context.readStatus);
}

static void SetReadRecIndToCore(MmsMsg &mmsMsg, ::ohos::telephony::sms::MmsInformation const & mms)
{
    auto context = mms.mmsType.get_mmsTypeReadRecInd_ref();
    mmsMsg.SetMmsVersion(context.version.get_value());
    std::string value(context.messageId.c_str());
    mmsMsg.SetHeaderStringValue(MmsFieldCode::MMS_MESSAGE_ID, value);
    std::vector<MmsAddress> toAddrs;
    for (size_t i = 0; i < context.to.size(); i++) {
        MmsAddress oneAddress;
        OHOS::Telephony::MmsCharSets charset = static_cast<MmsCharSets>(context.to.at(i).charset.get_value());
        oneAddress.SetMmsAddressString(context.to.at(i).address.c_str(), charset);
        toAddrs.push_back(oneAddress);
    }
    mmsMsg.SetMmsTo(toAddrs);

    OHOS::Telephony::MmsCharSets charset = static_cast<MmsCharSets>(context.fromAddress.charset.get_value());
    MmsAddress address(context.fromAddress.address.c_str(), charset);
    mmsMsg.SetMmsFrom(address);
    if (context.date.value() != 0) {
        mmsMsg.SetMmsDate(context.date.value());
    }
    mmsMsg.SetHeaderOctetValue(MmsFieldCode::MMS_READ_STATUS, context.readStatus);
}

static void SetRequestToCore(MmsMsg &mmsMsg, ::ohos::telephony::sms::MmsInformation const & mms)
{
    switch (mms.messageType.get_key()) {
        case ::ohos::telephony::sms::MessageType::key_t::TYPE_MMS_SEND_REQ:
            SetSendReqToCore(mmsMsg, mms);
            break;
        case ::ohos::telephony::sms::MessageType::key_t::TYPE_MMS_SEND_CONF:
            SetSendConfToCore(mmsMsg, mms);
            break;
        case ::ohos::telephony::sms::MessageType::key_t::TYPE_MMS_NOTIFICATION_IND:
            SetNotificationIndToCore(mmsMsg, mms);
            break;
        case ::ohos::telephony::sms::MessageType::key_t::TYPE_MMS_RESP_IND:
            SetRespIndToCore(mmsMsg, mms);
            break;
        case ::ohos::telephony::sms::MessageType::key_t::TYPE_MMS_RETRIEVE_CONF:
            SetRetrieveConfToCore(mmsMsg, mms);
            break;
        case ::ohos::telephony::sms::MessageType::key_t::TYPE_MMS_ACKNOWLEDGE_IND:
            SetAcknowledgeIndToCore(mmsMsg, mms);
            break;
        case ::ohos::telephony::sms::MessageType::key_t::TYPE_MMS_DELIVERY_IND:
            SetDeliveryIndToCore(mmsMsg, mms);
            break;
        case ::ohos::telephony::sms::MessageType::key_t::TYPE_MMS_READ_ORIG_IND:
            SetReadOrigIndToCore(mmsMsg, mms);
            break;
        case ::ohos::telephony::sms::MessageType::key_t::TYPE_MMS_READ_REC_IND:
            SetReadRecIndToCore(mmsMsg, mms);
            break;
        default:
            break;
    }
}

static int32_t WrapEncodeMmsStatus(::ohos::telephony::sms::MessageType messageType)
{
    TELEPHONY_LOGI("WrapEncodeMmsStatus messageType = %{public}d", messageType.get_key());
    switch (messageType.get_key()) {
        case ::ohos::telephony::sms::MessageType::key_t::TYPE_MMS_SEND_REQ: {
            return MmsMsgType::MMS_MSGTYPE_SEND_REQ;
        }
        case ::ohos::telephony::sms::MessageType::key_t::TYPE_MMS_SEND_CONF: {
            return MmsMsgType::MMS_MSGTYPE_SEND_CONF;
        }
        case ::ohos::telephony::sms::MessageType::key_t::TYPE_MMS_NOTIFICATION_IND: {
            return MmsMsgType::MMS_MSGTYPE_NOTIFICATION_IND;
        }
        case ::ohos::telephony::sms::MessageType::key_t::TYPE_MMS_RESP_IND: {
            return MmsMsgType::MMS_MSGTYPE_NOTIFYRESP_IND;
        }
        case ::ohos::telephony::sms::MessageType::key_t::TYPE_MMS_RETRIEVE_CONF: {
            return MmsMsgType::MMS_MSGTYPE_RETRIEVE_CONF;
        }
        case ::ohos::telephony::sms::MessageType::key_t::TYPE_MMS_ACKNOWLEDGE_IND: {
            return MmsMsgType::MMS_MSGTYPE_ACKNOWLEDGE_IND;
        }
        case ::ohos::telephony::sms::MessageType::key_t::TYPE_MMS_DELIVERY_IND: {
            return MmsMsgType::MMS_MSGTYPE_DELIVERY_IND;
        }
        case ::ohos::telephony::sms::MessageType::key_t::TYPE_MMS_READ_ORIG_IND: {
            return MmsMsgType::MMS_MSGTYPE_READ_ORIG_IND;
        }
        case ::ohos::telephony::sms::MessageType::key_t::TYPE_MMS_READ_REC_IND: {
            return MmsMsgType::MMS_MSGTYPE_READ_REC_IND;
        }
        default: {
            return MESSAGE_UNKNOWN_STATUS;
        }
    }
}

static void AttachmentArrayToVector(const ::taihe::array<::ohos::telephony::sms::MmsAttachment> arrayAttachment,
    std::vector<MmsAttachmentContext> &attachment)
{
    for (::ohos::telephony::sms::MmsAttachment oneArrayAttachment : arrayAttachment) {
        MmsAttachmentContext oneAttachment;
        oneAttachment.contentId = oneArrayAttachment.contentId;
        oneAttachment.contentLocation = oneArrayAttachment.contentLocation;
        oneAttachment.contentDisposition = ParseDispositionValue(oneArrayAttachment.contentDisposition.get_value());
        oneAttachment.contentTransferEncoding = oneArrayAttachment.contentTransferEncoding;
        oneAttachment.contentType = oneArrayAttachment.contentType;
        oneAttachment.isSmil = oneArrayAttachment.isSmil;
        oneAttachment.path = oneArrayAttachment.path.value_or("");
        if (oneArrayAttachment.inBuff.has_value()) {
            oneAttachment.inBuff = ArrayToUniquePtr(oneArrayAttachment.inBuff.value());
            oneAttachment.inBuffLen = oneArrayAttachment.inBuff->size();
        }
        oneAttachment.fileName = oneArrayAttachment.fileName.value_or("");
        if (oneArrayAttachment.charset.has_value()) {
            oneAttachment.charset = oneArrayAttachment.charset->get_value();
        }
        attachment.push_back(std::move(oneAttachment));
    }
}

::taihe::array<uint8_t> EncodeMmsSync(::ohos::telephony::sms::MmsInformation const & mms)
{
    ::taihe::array<uint8_t> arrayEncodeResult = {};
    if (!TelephonyPermission::CheckCallerIsSystemApp()) {
        set_business_error(TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API, "Non-system applications use system APIs!");
        return arrayEncodeResult;
    }
    MmsMsg mmsMsg;
    mmsMsg.SetMmsMessageType(static_cast<uint8_t>(WrapEncodeMmsStatus(mms.messageType)));
    std::vector<MmsAttachmentContext> attachment;
    if (mms.attachment.has_value()) {
        AttachmentArrayToVector(mms.attachment.value(), attachment);
    }

    if (!SetAttachmentToCore(mmsMsg, attachment)) {
        set_business_error(TELEPHONY_ERR_ARGUMENT_INVALID, "EncodeMmsSync SetAttachmentToCore Failed!");
        return arrayEncodeResult;
    }
    SetRequestToCore(mmsMsg, mms);
    uint32_t outLen = 0;
    auto encodeResult = mmsMsg.EncodeMsg(outLen);
    if (encodeResult == nullptr || outLen == 0) {
        set_business_error(TELEPHONY_ERR_FAIL, "EncodeMmsSync Failed!");
        return arrayEncodeResult;
    }
    return ::taihe::array<uint8_t>(taihe::copy_data_t{}, encodeResult.get(), outLen);
}

::taihe::string GetSmscAddrSync(int32_t slotId)
{
    if (!IsValidSlotId(slotId)) {
        set_business_error(ERROR_SLOT_ID_INVALID, "GetSmscAddrSync slotId is invalid");
        return "";
    }
    std::u16string smscAddress = u"";
    int32_t errorCode = Singleton<SmsServiceManagerClient>::GetInstance().GetScAddress(slotId, smscAddress);
    if (errorCode != TELEPHONY_ERR_SUCCESS) {
        set_business_error(errorCode, "GetSmscAddrSync GetScAddress failed");
    }
    return taihe::string(StringUtils::ToUtf8(smscAddress).c_str());
}

void AddSimMessageSync(::ohos::telephony::sms::SimMessageOptions const & options)
{
    int32_t wrapStatus = static_cast<int32_t>(options.status);
    if (wrapStatus != MESSAGE_UNKNOWN_STATUS) {
        ISmsServiceInterface::SimMessageStatus status = static_cast<ISmsServiceInterface::SimMessageStatus>(wrapStatus);
        std::u16string strSmsc(StringUtils::ToUtf16(options.smsc.c_str()));
        std::u16string strPdu(StringUtils::ToUtf16(options.pdu.c_str()));
        int32_t errorCode =
            Singleton<SmsServiceManagerClient>::GetInstance().AddSimMessage(options.slotId, strSmsc, strPdu, status);
        if (errorCode != TELEPHONY_ERR_SUCCESS) {
            set_business_error(errorCode, "AddSimMessageSync AddSimMessage failed");
        }
    } else {
        set_business_error(SMS_MMS_UNKNOWN_SIM_MESSAGE_STATUS, "AddSimMessageSync status is unknown");
    }
}

void DelSimMessageSync(int32_t slotId, int32_t msgIndex)
{
    if (!IsValidSlotId(slotId)) {
        set_business_error(ERROR_SLOT_ID_INVALID, "DelSimMessageSync slotId is invalid");
        return;
    }
    int32_t errorCode = Singleton<SmsServiceManagerClient>::GetInstance().DelSimMessage(slotId, msgIndex);
    if (errorCode != TELEPHONY_ERR_SUCCESS) {
        set_business_error(errorCode, "DelSimMessageSync DelSimMessage failed");
    }
}

::taihe::array<::ohos::telephony::sms::SimShortMessage> GetAllSimMessagesSync(int32_t slotId)
{
    ohos::telephony::sms::ShortMessageClass enuSmc(ohos::telephony::sms::ShortMessageClass::key_t::UNKNOWN);
    ohos::telephony::sms::ShortMessage stuSm =
        ohos::telephony::sms::ShortMessage{ "", "", enuSmc, -1, "", -1, false, false, {}, -1, false };
    ohos::telephony::sms::SimMessageStatus enuSMS(
        ohos::telephony::sms::SimMessageStatus::key_t::SIM_MESSAGE_STATUS_FREE);
    ohos::telephony::sms::SimShortMessage stuSSM = ohos::telephony::sms::SimShortMessage{ stuSm, enuSMS, -1 };
    taihe::array<ohos::telephony::sms::SimShortMessage> megArr = {};
    if (!IsValidSlotId(slotId)) {
        set_business_error(ERROR_SLOT_ID_INVALID, "GetAllSimMessagesSync slotId is invalid");
        return megArr;
    }
    std::vector<ShortMessage> messageArray;
    int32_t errorCode = Singleton<SmsServiceManagerClient>::GetInstance().GetAllSimMessages(slotId, messageArray);
    if (errorCode == TELEPHONY_ERR_SUCCESS) {
        for (const auto &message : messageArray) {
            stuSm.visibleMessageBody = taihe::string(StringUtils::ToUtf8(message.visibleMessageBody_).c_str());
            stuSm.visibleRawAddress = taihe::string(StringUtils::ToUtf8(message.visibleMessageBody_).c_str());
            stuSm.messageClass.from_value(static_cast<int32_t>(message.messageClass_));
            stuSm.protocolId = message.protocolId_;
            stuSm.scAddress = taihe::string(StringUtils::ToUtf8(message.scAddress_).c_str());
            stuSm.scTimestamp = message.scTimestamp_;
            stuSm.isReplaceMessage = message.isSmsStatusReportMessage_;
            stuSm.hasReplyPath = message.hasReplyPath_;
            stuSm.pdu = VectorToArray(message.pdu_);
            stuSm.status = message.status_;
            stuSm.isSmsStatusReportMessage = message.isSmsStatusReportMessage_;

            enuSMS.from_value(static_cast<int32_t>(message.simMessageStatus_));

            stuSSM.shortMessage = stuSm;
            stuSSM.simMessageStatus.from_value(enuSMS);
            stuSSM.indexOnSim = message.indexOnSim_;
        }
    } else {
        set_business_error(errorCode, "GetAllSimMessagesSync GetAllSimMessages failed");
    }
    return megArr;
}

void SetCBConfigSync(::ohos::telephony::sms::CBConfigOptions const & options)
{
    int32_t errorCode = Singleton<SmsServiceManagerClient>::GetInstance().SetCBConfig(options.slotId, options.enable,
        options.startMessageId, options.endMessageId, options.ranType);
    if (errorCode != TELEPHONY_ERR_SUCCESS) {
        set_business_error(errorCode, "SetCBConfigSync SetCBConfig failed");
    }
}

::taihe::array<::taihe::string> SplitMessageSync(::taihe::string_view content)
{
    std::u16string content16 = StringUtils::ToUtf16(content.c_str());
    taihe::array<taihe::string> result = {};
    std::vector<std::u16string> messageArray;
    int32_t errorCode = Singleton<SmsServiceManagerClient>::GetInstance().SplitMessage(content16, messageArray);
    if (errorCode == TELEPHONY_ERR_SUCCESS) {
        std::vector<std::string> message;
        std::transform(messageArray.begin(), messageArray.end(), std::back_inserter(message), StringUtils::ToUtf8);
        result = ::taihe::array<taihe::string>(taihe::copy_data_t{}, message.data(), message.size());
    } else {
        set_business_error(errorCode, "SplitMessageSync SplitMessage failed");
    }
    return result;
}

::ohos::telephony::sms::ShortMessage CreateMessageSync(::taihe::array_view<uint8_t> pdu,
    ::taihe::string_view specification)
{
    ohos::telephony::sms::ShortMessageClass shortMessageClass(ohos::telephony::sms::ShortMessageClass::key_t::UNKNOWN);
    ohos::telephony::sms::ShortMessage shortMessage =
        ohos::telephony::sms::ShortMessage{ "", "", shortMessageClass, -1, "", -1, false, false, {}, -1, false };
    if (specification.empty() || pdu.empty()) {
        set_business_error(TELEPHONY_ERR_ARGUMENT_INVALID, "CreateMessageSync failed");
        return shortMessage;
    }
    std::u16string specification16(StringUtils::ToUtf16(specification.c_str()));
    std::vector<unsigned char> strpdu(pdu.begin(), pdu.end());
    auto shortMessageObj = new ShortMessage();
    int32_t errorCode = ShortMessage::CreateMessage(strpdu, specification16, *shortMessageObj);
    if (errorCode == TELEPHONY_ERR_SUCCESS) {
        shortMessage.visibleMessageBody =
            taihe::string(StringUtils::ToUtf8(shortMessageObj->visibleMessageBody_).c_str());
        shortMessage.visibleRawAddress =
            taihe::string(StringUtils::ToUtf8(shortMessageObj->visibleRawAddress_).c_str());
        shortMessage.messageClass.from_value(static_cast<int32_t>(shortMessageObj->messageClass_));
        shortMessage.protocolId = shortMessageObj->protocolId_;
        shortMessage.scAddress = taihe::string(StringUtils::ToUtf8(shortMessageObj->scAddress_).c_str());
        shortMessage.scTimestamp = shortMessageObj->scTimestamp_;
        shortMessage.isReplaceMessage = shortMessageObj->isReplaceMessage_;
        shortMessage.hasReplyPath = shortMessageObj->hasReplyPath_;
        shortMessage.pdu = VectorToArray(shortMessageObj->pdu_);
        shortMessage.status = shortMessageObj->status_;
        shortMessage.isSmsStatusReportMessage = shortMessageObj->isSmsStatusReportMessage_;
    } else {
        set_business_error(errorCode, "CreateMessageSync CreateMessage failed");
        return shortMessage;
    }
    return shortMessage;
}

void SetDefaultSmsSlotIdSync(int32_t slotId)
{
    if (!IsValidSlotId(slotId) && (slotId != DEFAULT_SIM_SLOT_ID_REMOVE)) {
        set_business_error(ERROR_SLOT_ID_INVALID, "SetDefaultSmsSlotIdSync slotId is invalid");
        return;
    }
    uint32_t errorCode = Singleton<SmsServiceManagerClient>::GetInstance().SetDefaultSmsSlotId(slotId);
    if (errorCode != TELEPHONY_ERR_SUCCESS) {
        set_business_error(errorCode, "SetDefaultSmsSlotIdSync SetDefaultSmsSlotId failed");
    }
}

void SetSmscAddrSync(int32_t slotId, ::taihe::string_view smscAddr)
{
    if (!IsValidSlotId(slotId)) {
        set_business_error(ERROR_SLOT_ID_INVALID, "SetSmscAddrSync slotId is invalid");
        return;
    }
    uint32_t errorCode =
        Singleton<SmsServiceManagerClient>::GetInstance().SetScAddress(slotId, StringUtils::ToUtf16(smscAddr.c_str()));
    if (errorCode != TELEPHONY_ERR_SUCCESS) {
        set_business_error(errorCode, "SetSmscAddrSync SetScAddress failed");
    }
}
} // namespace

// Since these macros are auto-generate, lint will cause false positive.
// NOLINTBEGIN
TH_EXPORT_CPP_API_HasSmsCapability(HasSmsCapability);
TH_EXPORT_CPP_API_SendMmsSync(SendMmsSync);
TH_EXPORT_CPP_API_GetDefaultSmsSlotIdSync(GetDefaultSmsSlotIdSync);
TH_EXPORT_CPP_API_GetDefaultSmsSimIdSync(GetDefaultSmsSimIdSync);
TH_EXPORT_CPP_API_GetSmsSegmentsInfoSync(GetSmsSegmentsInfoSync);
TH_EXPORT_CPP_API_IsImsSmsSupportedSync(IsImsSmsSupportedSync);
TH_EXPORT_CPP_API_GetImsShortMessageFormatSync(GetImsShortMessageFormatSync);
TH_EXPORT_CPP_API_DownloadMmsSync(DownloadMmsSync);
TH_EXPORT_CPP_API_DecodeMmsSync(DecodeMmsSync);
TH_EXPORT_CPP_API_EncodeMmsSync(EncodeMmsSync);
TH_EXPORT_CPP_API_GetSmscAddrSync(GetSmscAddrSync);
TH_EXPORT_CPP_API_AddSimMessageSync(AddSimMessageSync);
TH_EXPORT_CPP_API_DelSimMessageSync(DelSimMessageSync);
TH_EXPORT_CPP_API_GetAllSimMessagesSync(GetAllSimMessagesSync);
TH_EXPORT_CPP_API_SetCBConfigSync(SetCBConfigSync);
TH_EXPORT_CPP_API_SplitMessageSync(SplitMessageSync);
TH_EXPORT_CPP_API_CreateMessageSync(CreateMessageSync);
TH_EXPORT_CPP_API_SetDefaultSmsSlotIdSync(SetDefaultSmsSlotIdSync);
TH_EXPORT_CPP_API_SetSmscAddrSync(SetSmscAddrSync);
// NOLINTEND