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

#ifndef MMS_CODEC_TYPE_H
#define MMS_CODEC_TYPE_H

namespace OHOS {
namespace Telephony {
/**
 * @brief MmsFieldCode
 * OMA-TS-MMS_CONF-V1_3-20110913-A   section:7.4 Header Field Names and Assigned Numbers
 * Table 25: Field Name Assignments
 */
enum MmsFieldCode {
    MMS_BCC                         = 0x01 | 0x80,
    MMS_CC                          = 0x02 | 0x80,
    MMS_CONTENT_LOCATION            = 0x03 | 0x80,
    MMS_CONTENT_TYPE                = 0x04 | 0x80,
    MMS_DATE                        = 0x05 | 0x80,
    MMS_DELIVERY_REPORT             = 0x06 | 0x80,
    MMS_DELIVERY_TIME               = 0x07 | 0x80,
    MMS_EXPIRY                      = 0x08 | 0x80,
    MMS_FROM                        = 0x09 | 0x80,
    MMS_MESSAGE_CLASS               = 0x0A | 0x80,
    MMS_MESSAGE_ID                  = 0x0B | 0x80,
    MMS_MESSAGE_TYPE                = 0x0C | 0x80,
    MMS_MMS_VERSION                 = 0x0D | 0x80,
    MMS_MESSAGE_SIZE                = 0x0E | 0x80,
    MMS_PRIORITY                    = 0x0F | 0x80,
    MMS_READ_REPORT                 = 0x10 | 0x80,
    MMS_REPORT_ALLOWED              = 0x11 | 0x80,
    MMS_RESPONSE_STATUS             = 0x12 | 0x80,
    MMS_RESPONSE_TEXT               = 0x13 | 0x80,
    MMS_SENDER_VISIBILITY           = 0x14 | 0x80,
    MMS_STATUS                      = 0x15 | 0x80,
    MMS_SUBJECT                     = 0x16 | 0x80,
    MMS_TO                          = 0x17 | 0x80,
    MMS_TRANSACTION_ID              = 0x18 | 0x80,
    MMS_RETRIEVE_STATUS             = 0x19 | 0x80,
    MMS_RETRIEVE_TEXT               = 0x1A | 0x80,
    MMS_READ_STATUS                 = 0x1B | 0x80,
    MMS_REPLY_CHARGING              = 0x1C | 0x80,
    MMS_REPLY_CHARGING_DEADLINE     = 0x1D | 0x80,
    MMS_REPLY_CHARGING_ID           = 0x1E | 0x80,
    MMS_REPLY_CHARGING_SIZE         = 0x1F | 0x80,
    MMS_PREVIOUSLY_SENT_BY          = 0x20 | 0x80,
    MMS_PREVIOUSLY_SENT_DATE        = 0x21 | 0x80,
    MMS_STORE                       = 0x22 | 0x80,
    MMS_MM_STATE                    = 0x23 | 0x80,
    MMS_MM_FLAGS                    = 0x24 | 0x80,
    MMS_STORE_STATUS                = 0x25 | 0x80,
    MMS_STORE_STATUS_TEXT           = 0x26 | 0x80,
    MMS_STORED                      = 0x27 | 0x80,
    MMS_ATTRIBUTES                  = 0x28 | 0x80,
    MMS_TOTALS                      = 0x29 | 0x80,
    MMS_MBOX_TOTALS                 = 0x2A | 0x80,
    MMS_QUOTAS                      = 0x2B | 0x80,
    MMS_MBOX_QUOTAS                 = 0x2C | 0x80,
    MMS_MESSAGE_COUNT               = 0x2D | 0x80,
    MMS_CONTENT                     = 0x2E | 0x80, // un
    MMS_START                       = 0x2F | 0x80,
    MMS_ADDITIONAL_HEADERS          = 0x30 | 0x80, // un
    MMS_DISTRIBUTION_INDICATOR      = 0x31 | 0x80,
    MMS_ELEMENT_DESCRIPTOR          = 0x32 | 0x80,
    MMS_LIMIT                       = 0x33 | 0x80,
    MMS_RECOMMENDED_RETRIEVAL_MODE  = 0x34 | 0x80,
    MMS_RECOMMENDED_RETRIEVAL_MODE_TEXT = 0x35 | 0x80,
    MMS_STATUS_TEXT                 = 0x36 | 0x80,
    MMS_APPLIC_ID                   = 0x37 | 0x80,
    MMS_REPLY_APPLIC_ID             = 0x38 | 0x80,
    MMS_AUX_APPLIC_INFO             = 0x39 | 0x80,
    MMS_CONTENT_CLASS               = 0x3A | 0x80,
    MMS_DRM_CONTENT                 = 0x3B | 0x80,
    MMS_ADAPTATION_ALLOWED          = 0x3C | 0x80,
    MMS_REPLACE_ID                  = 0x3D | 0x80,
    MMS_CANCEL_ID                   = 0x3E | 0x80,
    MMS_CANCEL_STATUS               = 0x3F | 0x80,
};

/**
 * @brief MmsMsgType
 * OMA-TS-MMS_CONF-V1_3-20110913-A   section:7.3.30 X-Mms-Message-Type Field
 */
enum MmsMsgType {
    MMS_MSGTYPE_SEND_REQ            = 128,
    MMS_MSGTYPE_SEND_CONF           = 129,
    MMS_MSGTYPE_NOTIFICATION_IND    = 130,
    MMS_MSGTYPE_NOTIFYRESP_IND      = 131,
    MMS_MSGTYPE_RETRIEVE_CONF       = 132,
    MMS_MSGTYPE_ACKNOWLEDGE_IND     = 133,
    MMS_MSGTYPE_DELIVERY_IND        = 134,
    MMS_MSGTYPE_READ_REC_IND        = 135,
    MMS_MSGTYPE_READ_ORIG_IND       = 136,
    MMS_MSGTYPE_FORWARD_REQ         = 137,
    MMS_MSGTYPE_FORWARD_CONF        = 138,
    MMS_MSGTYPE_MBOX_STORE_REQ      = 139,
    MMS_MSGTYPE_MBOX_STORE_CONF     = 140,
    MMS_MSGTYPE_MBOX_VIEW_REQ       = 141,
    MMS_MSGTYPE_MBOX_VIEW_CONF      = 142,
    MMS_MSGTYPE_MBOX_UPLOAD_REQ     = 143,
    MMS_MSGTYPE_MBOX_UPLOAD_CONF    = 144,
    MMS_MSGTYPE_MBOX_DELETE_REQ     = 145,
    MMS_MSGTYPE_MBOX_DELETE_CONF    = 146,
    MMS_MSGTYPE_MBOX_DESCR          = 147,
    MMS_MSGTYPE_DELETE_REQ          = 148,
    MMS_MSGTYPE_DELETE_CONF         = 149,
    MMS_MSGTYPE_CANCEL_REQ          = 150,
    MMS_MSGTYPE_CANCEL_CONF         = 151,
};

enum class MmsVersionType {
    MMS_VERSION_1_0 = 0x10,
    MMS_VERSION_1_1 = 0x11,
    MMS_VERSION_1_2 = 0x12,
    MMS_VERSION_1_3 = 0x13,
};

/**
 * @brief MmsBoolType
 * OMA-TS-MMS_CONF-V1_3-20110913-A   section:7.3.37 X-Mms-Read-Report Field
 */
enum class MmsBoolType {
    MMS_YES                         = 128,
    MMS_NO                          = 129,
};

/**
 * @brief MmsMessageClass
 * OMA-TS-MMS_CONF-V1_3-20110913-A   section:7.3.27 X-Mms-Message-Class Field
 */
enum class MmsMessageClass {
    PERSONAL =	128,
    ADVERTISEMENT =	129,
    INFORMATIONAL =	130,
    AUTO = 	131,
};

/**
 * @brief MmsExpiry
 * OMA-TS-MMS_CONF-V1_3-20110913-A   section:7.3.20 X-Mms-Expiry Field
 */
enum class MmsExpiry {
    ABSOLUTE_TOKEN = 128,
    RELATIVE_TOKEN = 129,
};

/**
 * @brief MmsPriority
 * OMA-TS-MMS_CONF-V1_3-20110913-A   section:7.3.35 X-Mms-Priority Field
 */
enum class MmsPriority {
    MMS_LOW                         = 128,
    MMS_NORMAL                      = 129,
    MMS_HIGH                        = 130,
};

/**
 * @brief MmsResponseStatus
 * OMA-TS-MMS_CONF-V1_3-20110913-A   section:7.3.48 X-Mms-Response-Status Field
 */
enum class MmsResponseStatus {
    MMS_OK                                                  = 128,
    MMS_ERROR_UNSPECIFIED                                   = 129,
    MMS_ERROR_SERVICE_DENIED                                = 130,
    MMS_ERROR_MESSAGE_FORMAT_CORRUPT                        = 131,
    MMS_ERROR_SENDING_ADDRESS_UNRESOLVED                    = 132,
    MMS_ERROR_MESSAGE_NOT_FOUND                             = 133,
    MMS_ERROR_NETWORK_PROBLEM                               = 134,
    MMS_ERROR_CONTENT_NOT_ACCEPTED                          = 135,
    MMS_ERROR_UNSUPPORTED_MESSAGE                           = 136,
    MMS_ERROR_TRANSIENT_FAILURE                             = 192,
    MMS_ERROR_TRANSIENT_SENDING_ADDRESS_UNRESOLVED          = 193,
    MMS_ERROR_TRANSIENT_MESSAGE_NOT_FOUND                   = 194,
    MMS_ERROR_TRANSIENT_NETWORK_PROBLEM                     = 195,
    MMS_ERROR_TRANSIENT_PARTIAL_SUCCESS                     = 196,
    MMS_ERROR_PERMANENT_FAILURE                             = 224,
    MMS_ERROR_PERMANENT_SERVICE_DENIED                      = 225,
    MMS_ERROR_PERMANENT_MESSAGE_FORMAT_CORRUPT              = 226,
    MMS_ERROR_PERMANENT_SENDING_ADDRESS_UNRESOLVED          = 227,
    MMS_ERROR_PERMANENT_MESSAGE_NOT_FOUND                   = 228,
    MMS_ERROR_PERMANENT_CONTENT_NOT_ACCEPTED                = 229,
    MMS_ERROR_PERMANENT_REPLY_CHARGING_LIMITATIONS_NOT_MET  = 230,
    MMS_ERROR_PERMANENT_REPLY_CHARGING_REQUEST_NOT_ACCEPTED = 231,
    MMS_ERROR_PERMANENT_REPLY_CHARGING_FORWARDING_DENIED    = 232,
    MMS_ERROR_PERMANENT_REPLY_CHARGING_NOT_SUPPORTED        = 233,
    MMS_ERROR_PERMANENT_ADDRESS_HIDING_NOT_SUPPORTED        = 234,
    MMS_ERROR_PERMANENT_LACK_OF_PREPAID                     = 235,
};

/**
 * @brief MmsSenderVisibility
 * OMA-TS-MMS_CONF-V1_3-20110913-A   section:7.3.52 X-Mms-Sender-Visibility Field
 */
enum class MmsSenderVisibility {
    MMS_HIDE                         = 128,
    MMS_SHOW                         = 129,
};

/**
 * @brief MmsStatus
 * OMA-TS-MMS_CONF-V1_3-20110913-A   section:7.3.54 X-Mms-Status Field
 */
enum class MmsStatus {
    MMS_EXPIRED                      = 128,
    MMS_RETRIEVED                    = 129,
    MMS_REJECTED                     = 130,
    MMS_DEFERRED                     = 131,
    MMS_MMS_UNRECOGNISED             = 132,
    MMS_INDETERMINATE                = 133,
    MMS_FORWARDED                    = 134,
    MMS_UNREACHABLE                  = 135,
};

/**
 * @brief MmsRetrieveStatus
 * OMA-TS-MMS_CONF-V1_3-20110913-A   section:7.3.50 X-Mms-Retrieve-Status Field
 */
enum class MmsRetrieveStatus {
    MMS_RETRIEVE_STATUS_OK                                  = 128,
    MMS_RETRIEVE_STATUS_ERROR_TRANSIENT_FAILURE             = 192,
    MMS_RETRIEVE_STATUS_ERROR_TRANSIENT_MESSAGE_NOT_FOUND   = 193,
    MMS_RETRIEVE_STATUS_ERROR_TRANSIENT_NETWORK_PROBLEM     = 194,
    MMS_RETRIEVE_STATUS_ERROR_PERMANENT_FAILURE             = 224,
    MMS_RETRIEVE_STATUS_ERROR_PERMANENT_SERVICE_DENIED      = 225,
    MMS_RETRIEVE_STATUS_ERROR_PERMANENT_MESSAGE_NOT_FOUND   = 226,
    MMS_RETRIEVE_STATUS_ERROR_PERMANENT_CONTENT_UNSUPPORTED = 227,
};

/**
 * @brief MmsReadStatus
 * OMA-TS-MMS_CONF-V1_3-20110913-A   section:7.3.38 X-Mms-Read-Status Field
 */
enum class MmsReadStatus {
    MMS_READ                            = 128,
    MMS_DELETED_WITHOUT_BEING_READ      = 129,
};

/**
 * @brief MmsReplyCharging
 * OMA-TS-MMS_CONF-V1_3-20110913-A   section:7.3.43 X-Mms-Reply-Charging Field
 */
enum class MmsReplyCharging {
    MMS_REQUESTED               = 128,
    MMS_REQUESTED_TEXT_ONLY     = 129,
    MMS_ACCEPTED                = 130,
    MMS_ACCEPTED_TEXT_ONLY      = 131,
};

/**
 * @brief MmsReplyChargingDeadline
 * OMA-TS-MMS_CONF-V1_3-20110913-A   section:7.3.44 X-Mms-Reply-Charging-Deadline Field
 */
enum class MmsReplyChargingDeadline {
    MMS_ABSOLUTE_TOKEN              = 128,
    MMS_RELATIVE_TOKEN              = 129,
};

/**
 * @brief MmsMmState
 * OMA-TS-MMS_CONF-V1_3-20110913-A   section:7.3.33 X-Mms-MM-State Field
 */
enum class MmsMmState {
    MMS_MM_STATE_DRAFT                        = 128,
    MMS_MM_STATE_SENT                         = 129,
    MMS_MM_STATE_NEW                          = 130,
    MMS_MM_STATE_RETRIEVED                    = 131,
    MMS_MM_STATE_FORWARDED                    = 132,
};

/**
 * @brief MmsMmFlags
 * OMA-TS-MMS_CONF-V1_3-20110913-A   section:7.3.32 X-Mms-MM-Flags Field
 */
enum class MmsMmFlags {
    MMS_ADD_TOKEN                       = 128,
    MMS_REMOVE_TOKEN                    = 129,
    MMS_FILTER_TOKEN                    = 130,
};

/**
 * @brief MmsStoreStatus
 * OMA-TS-MMS_CONF-V1_3-20110913-A   section:7.3.58 X-Mms-Store-Status Field
 */
enum class MmsStoreStatus {
    MMS_STORE_STATUS_SUCCESS                                 = 128,
    MMS_STORE_STATUS_ERROR_TRANSIENT_FAILURE                 = 192,
    MMS_STORE_STATUS_ERROR_TRANSIENT_NETWORK_PROBLEM         = 193,
    MMS_STORE_STATUS_ERROR_PERMANENT_FAILURE                 = 224,
    MMS_STORE_STATUS_ERROR_PERMANENT_SERVICE_DENIED          = 225,
    MMS_STORE_STATUS_ERROR_PERMANENT_MESSAGE_FORMAT_CORRUPT  = 226,
    MMS_STORE_STATUS_ERROR_PERMANENT_MESSAGE_NOT_FOUND       = 227,
    MMS_STORE_STATUS_ERROR_PERMANENT_MMBOX_FULL              = 228,
};

/**
 * @brief MmsMboxTotals
 * OMA-TS-MMS_CONF-V1_3-20110913-A   section:7.3.26 X-Mms-Mbox-Totals Field
 */
enum class MmsMboxTotals {
    MMS_MESSAGE_TOTAL_TOKEN                     = 128,
    MMS_SIZE_TOTAL_TOKEN                        = 129,
};

/**
 * @brief MmsMboxQuotas
 * OMA-TS-MMS_CONF-V1_3-20110913-A   section:7.3.25 X-Mms-Mbox-Quotas Field
 */
enum class MmsMboxQuotas {
    MMS_MESSAGE_QUOTA_TOKEN                     = 128,
    MMS_SIZE_QUOTA_TOKEN                        = 129,
};

/**
 * @brief MmsRecommendedRetrievalMode
 * OMA-TS-MMS_CONF-V1_3-20110913-A   section:7.3.39 X-Mms-Recommended-Retrieval-Mode Field
 */
enum class MmsRecommendedRetrievalMode {
    MMS_MANUAL                                  = 128,
};

/**
 * @brief MmsContentClass
 * OMA-TS-MMS_CONF-V1_3-20110913-A   section:7.3.9 X-Mms-Content-Class Field
 */
enum class MmsContentClass {
    MMS_TEXT                                    = 128,
    MMS_IMAGE_BASIC                             = 129,
    MMS_IMAGE_RICH                              = 130,
    MMS_VIDEO_BASIC                             = 131,
    MMS_VIDEO_RICH                              = 132,
    MMS_MEGAPIXEL                               = 133,
    MMS_CONTENT_BASIC                           = 134,
    MMS_CONTENT_RICH                            = 135,
};

/**
 * @brief MmsCancelStatus
 * OMA-TS-MMS_CONF-V1_3-20110913-A   section:7.3.7 X-Mms-Cancel-Status Field
 */
enum class MmsCancelStatus {
    MMS_CANCEL_REQUEST_SUCCESSFULLY_RECEIVED    = 128,
    MMS_CANCEL_REQUEST_CORRUPTED                = 129,
};

/**
 * @brief MmsCharSets
 * wap-230-wsp-20010705-a   Table 42. Character Set Assignment Examples
 */
enum class MmsCharSets {
    BIG5 = 0X07EA,
    ISO_10646_UCS_2 = 0X03E8,
    ISO_8859_1 = 0X04,
    ISO_8859_2 = 0X05,
    ISO_8859_3 = 0X06,
    ISO_8859_4 = 0X07,
    ISO_8859_5 = 0X08,
    ISO_8859_6 = 0X09,
    ISO_8859_7 = 0X0A,
    ISO_8859_8 = 0X0B,
    ISO_8859_9 = 0X0C,
    SHIFT_JIS = 0X11,
    US_ASCII = 0X03,
    UTF_8 = 0X6A
};

enum class MmsFieldOptType {
    MMS_MANDATORY_TYPE,
    MMS_OPTIONAL_TYPE,
    MMS_UNSUPPORTED_TYPE
};

enum class MmsFieldValueType {
    MMS_FIELD_OCTET_TYPE = 0x01,
    MMS_FIELD_INTEGER_TYPE = 0x02,
    MMS_FIELD_LONG_TYPE = 0x04,
    MMS_FIELD_TEXT_TYPE = 0x08,
    MMS_FIELD_ENCODE_TEXT_TYPE = 0x10,
    MMS_FIELD_ENCODE_ADDR_TYPE = 0x20,
    MMS_FIELD_UNKOWN_TYPE = 0x00
};

enum class MmsEncodeErrcode {
    MMS_OK,
    MMS_NOT_SET_ERR,
    MMS_BUFF_ERR
};

struct MmsHeaderCategDes {
    unsigned char fieldId;
    MmsFieldOptType fieldOpt;
    MmsFieldValueType valueType;
};

struct KeyString {
    unsigned char key;
    const char *value;
};

struct MmsFieldDes {
    unsigned char fieldId;
    MmsFieldValueType type;
};

/**
* OMA-TS-MMS_ENC-V1_3-20110913-A.pdf
* 7.4 Header Field Names and Assigned Numbers
*/
static const KeyString mmsHeaderNames[] = {
    /* MMS 1.0 */
    { MMS_BCC,                           "Mms-Bcc" },
    { MMS_CC,                            "Mms-Cc" },
    { MMS_CONTENT_LOCATION,              "Mms-Content-Location" },
    { MMS_CONTENT_TYPE,                  "Mms-Content-Type" },
    { MMS_DATE,                          "Mms-Date" },
    { MMS_DELIVERY_REPORT,               "Mms-Delivery-Report" },
    { MMS_DELIVERY_TIME,                 "Mms-Delivery-Time" },
    { MMS_EXPIRY,                        "Mms-Expiry" },
    { MMS_FROM,                          "Mms-From" },
    { MMS_MESSAGE_CLASS,                 "Mms-Message-Class" },
    { MMS_MESSAGE_ID,                    "Mms-Message-ID" },
    { MMS_MESSAGE_TYPE,                  "Mms-Message-Type" },
    { MMS_MMS_VERSION,                   "Mms-Mms-Version" },
    { MMS_MESSAGE_SIZE,                  "Mms-Message-Size" },
    { MMS_PRIORITY,                      "Mms-Priority" },
    { MMS_READ_REPORT,                   "Mms-Read-Report" },
    { MMS_REPORT_ALLOWED,                "Mms-Report-Allowed" },
    { MMS_RESPONSE_STATUS,               "Mms-Response-Status" },
    { MMS_RESPONSE_TEXT,                 "Mms-Response-Text" },
    { MMS_SENDER_VISIBILITY,             "Mms-X-Mms-Sender-Visibility" },
    { MMS_STATUS,                        "Mms-X-Mms-Status" },
    { MMS_SUBJECT,                       "Mms-Subject" },
    { MMS_TO,                            "Mms-To" },
    { MMS_TRANSACTION_ID,                "Mms-X-Mms-Transaction-Id" },
    /* MMS 1.1 */
    { MMS_RETRIEVE_STATUS,               "Mms-X-Mms-Retrieve-Status" },
    { MMS_RETRIEVE_TEXT,                 "Mms-X-Mms-Retrieve-Text" },
    { MMS_READ_STATUS,                   "Mms-X-Mms-Read-Status" },
    { MMS_REPLY_CHARGING,                "Mms-X-Mms-Reply-Charging" },
    { MMS_REPLY_CHARGING_DEADLINE,       "Mms-X-Mms-Reply-Charging-Deadline" },
    { MMS_REPLY_CHARGING_ID,             "Mms-X-Mms-Reply-Charging-ID" },
    { MMS_REPLY_CHARGING_SIZE,           "Mms-X-Mms-Reply-Charging-Size" },
    { MMS_PREVIOUSLY_SENT_BY,            "Mms-X-Mms-Previously-Sent-By" },
    { MMS_PREVIOUSLY_SENT_DATE,          "Mms-X-Mms-Previously-Sent-Date" },
    /* MMS 1.2 */
    { MMS_STORE,                         "Mms-X-Mms-Store" },
    { MMS_MM_STATE,                      "Mms-X-Mms-MM-State" },
    { MMS_MM_FLAGS,                      "Mms-X-Mms-MM-Flags" },
    { MMS_STORE_STATUS,                  "Mms-X-Mms-Store-Status" },
    { MMS_STORE_STATUS_TEXT,             "Mms-X-Mms-Store-Status-Text" },
    { MMS_STORED,                        "Mms-X-Mms-Stored" },
    { MMS_ATTRIBUTES,                    "Mms-X-Mms-Attributes" },
    { MMS_TOTALS,                        "Mms-X-Mms-Totals" },
    { MMS_MBOX_TOTALS,                   "Mms-X-Mms-Mbox-Totals" },
    { MMS_QUOTAS,                        "Mms-X-Mms-Quotas" },
    { MMS_MBOX_QUOTAS,                   "Mms-X-Mms-Mbox-Quotas" },
    { MMS_MESSAGE_COUNT,                 "Mms-X-Mms-Message-Count" },
    { MMS_CONTENT,                       "Mms-Content" },
    { MMS_START,                         "Mms-X-Mms-Start" },
    { MMS_ADDITIONAL_HEADERS,            "Mms-Additional-headers" },
    { MMS_DISTRIBUTION_INDICATOR,        "Mms-X-Mms-Distribution-Indicator" },
    { MMS_ELEMENT_DESCRIPTOR,            "Mms-X-Mms-Element-Descriptor" },
    { MMS_LIMIT,                         "Mms-X-Mms-Limit" },
    { MMS_RECOMMENDED_RETRIEVAL_MODE,     "Mms-X-Mms-Recommended-Retrieval-Mode-Text" },
    { MMS_RECOMMENDED_RETRIEVAL_MODE_TEXT, "Mms-X-Mms-Recommended-Retrieval-Mode-Text" },
    { MMS_STATUS_TEXT,                   "Mms-X-Mms-Status-Text" },
    { MMS_APPLIC_ID,                     "Mms-X-Mms-Applic-ID" },
    { MMS_REPLY_APPLIC_ID,               "Mms-X-Mms-Reply-Applic-ID" },
    { MMS_AUX_APPLIC_INFO,               "Mms-X-Mms-Aux-Applic-Info" },
    { MMS_CONTENT_CLASS,                 "Mms-X-Mms-Content-Class" },
    { MMS_DRM_CONTENT,                   "Mms-X-Mms-DRM-Content" },
    { MMS_ADAPTATION_ALLOWED,            "Mms-X-Mms-Adaptation-Allowed" },
    { MMS_REPLACE_ID,                    "Mms-X-Mms-Replace-ID" },
    { MMS_CANCEL_ID,                     "Mms-X-Mms-Cancel-ID" },
    { MMS_CANCEL_STATUS,                 "Mms-X-Mms-Cancel-Status" },

    { 0x00, "empty" },
};
} // namespace Telephony
} // namespace OHOS
#endif
