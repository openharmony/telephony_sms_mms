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

#ifndef CDMA_SMS_COMMON_H
#define CDMA_SMS_COMMON_H

#include "sms_pdu_code_type.h"

namespace OHOS {
namespace Telephony {
static constexpr uint16_t CDMASMS_USER_DATA_LEN_MAX = 160;
static constexpr uint16_t CDMASMS_ADDRESS_LEN_MAX = 256;
static constexpr uint16_t CDMASMS_NUMBER_OF_ACK_MAX = 8;

enum class CdmaTransportMsgType : uint8_t { P2P = 0x00, BROADCAST = 0x01, ACK = 0x02, RESERVED };

enum class TeleserviceMsgType : uint8_t {
    RESERVED = 0x00, // reserved
    DELIVER, // mobile-terminated only
    SUBMIT, // mobile-originated only
    CANCEL, // mobile-originated only
    DELIVERY_ACK, // mobile-terminated only
    USER_ACK, // either direction
    READ_ACK, // either direction
    DELIVER_REPORT, // mobile-originated only
    SUBMIT_REPORT = 0x08, // mobile-terminated only
    MAX_VALUE
};

enum class SmsLanguageType : uint8_t {
    UNKNOWN,
    ENGLISH,
    FRENCH,
    SPANISH,
    JAPANESE,
    KOREAN,
    CHINESE,
    HEBREW,
};

enum class SmsPriorityIndicator : uint8_t { NORMAL = 0x00, INTERACTIVE, URGENT, EMERGENCY, RESERVED };

enum class SmsPrivacyIndicator : uint8_t { NOT_RESTRICTED = 0x00, RESTRICTED, CONFIDENTIAL, SECRET };

enum class SmsAlertPriority : uint8_t { DEFAULT = 0x00, LOW, MEDIUM, HIGH };

enum class SmsDisplayMode : uint8_t { IMMEDIATE = 0x00, DEFAULT_SETTING, USER_INVOKE, RESERVED };

enum class SmsEncodingType : uint8_t {
    OCTET = 0x0,
    EPM = 0x1, // IS-91 Extended Protocol Message
    ASCII_7BIT = 0x2,
    IA5 = 0x3,
    UNICODE = 0x4,
    SHIFT_JIS = 0x5,
    KOREAN = 0x6,
    LATIN_HEBREW = 0x7,
    LATIN = 0x8,
    GSM7BIT = 0x9,
    GSMDCS = 0xa,
    EUCKR = 0x10,
    RESERVED
};

enum class SmsStatusCode : uint8_t {
    // ERROR_CLASS = '00' (no error)
    ACCEPTED = 0x00,
    DEPOSITED = 0x01,
    DELIVERED = 0x02,
    CANCELLED = 0x03,

    // ERROR_CLASS = '10' (temporary condition)
    TEMP_NETWORK_CONGESTION = 0x84,
    TEMP_NETWORK_ERROR = 0x85,
    TEMP_UNKNOWN_ERROR = 0x9F,

    // ERROR_CLASS = '11' (permanent condition)
    PERMANENT_NETWORK_CONGESTION = 0xC4,
    PERMANENT_NETWORK_ERROR = 0xC5,
    PERMANENT_CANCEL_FAILED = 0xC6,
    PERMANENT_BLOCKED_DESTINATION = 0xC7,
    PERMANENT_TEXT_TOO_LONG = 0xC8,
    PERMANENT_DUPLICATE_MESSAGE = 0xC9,
    PERMANENT_INVALID_DESTINATION = 0xCA,
    PERMANENT_MESSAGE_EXPIRED = 0xCD,
    PERMANENT_UNKNOWN_ERROR = 0xDF,
};

enum class SmsCmaeCategory : uint8_t {
    GEO = 0x00,
    MET = 0x01,
    SAFETY = 0x02,
    SECURITY = 0x03,
    RESCUE = 0x04,
    FIRE = 0x05,
    HEALTH = 0x06,
    ENV = 0x07,
    TRANSPORT = 0x08,
    INFRA = 0x09,
    CBRNE = 0x0a,
    OTHER = 0x0b,
    RESERVED,
};

enum class SmsCmaeResponseType : uint8_t {
    SHELTER = 0x00,
    EVACUATE = 0x01,
    PREPARE = 0x02,
    EXECUTE = 0x03,
    MONITOR = 0x04,
    AVOID = 0x05,
    ASSESS = 0x06,
    NONE = 0x07,
    RESERVED,
};

enum class SmsCmaeSeverity : uint8_t {
    EXTREME = 0x0,
    SEVERE = 0x1,
    RESERVED,
};

enum class SmsCmaeUrgency : uint8_t {
    IMMEDIATE = 0x0,
    EXPECTED = 0x1,
    RESERVED,
};

enum class SmsCmaeCertainty : uint8_t {
    OBSERVED = 0x0,
    LIKELY = 0x1,
    RESERVED,
};

enum class SmsCmaeAlertHandle : uint8_t {
    PRESIDENTIAL = 0x00,
    EXTREME = 0x01,
    SEVERE = 0x02,
    AMBER = 0x03,
    RESERVED,
};

enum class SmsTransTelsvcId : uint16_t {
    CMT_91 = 0x1000, // IS-91 Extended Protocol Enhanced Services
    CPT_95 = 0x1001, // Wireless Paging Teleservice
    CMT_95 = 0x1002, // Wireless Messaging Teleservice
    VMN_95 = 0x1003, // Voice Mail Notification
    WAP = 0x1004, // Wireless Application Protocol
    WEMT = 0x1005, // Wireless Enhanced Messaging Teleservice
    SCPT = 0x1006, // Service Category Programming Teleservice
    CATPT = 0x1007, // Card Application Toolkit Protocol Teleservice
    RESERVED = 0xffff
};

enum class SmsServiceCtg : uint16_t {
    UNKNOWN = 0x0000,
    EMERGENCY = 0x0001,
    ADMINISTRATIVE = 0x0002,
    MAINTENANCE = 0x0003,
    GNEWS_LOCAL = 0x0004,
    GNEWS_REGIONAL = 0x0005,
    GNEWS_NATIONAL = 0x0006,
    GNEWS_INTERNATIONAL = 0x0007,
    BFNEWS_LOCAL = 0x0008,
    BFNEWS_REGIONAL = 0x0009,
    BFNEWS_NATIONAL = 0x000a,
    BFNEWS_INTERNATIONAL = 0x000b,
    SNEWS_LOCAL = 0x000c,
    SNEWS_REGIONAL = 0x000d,
    SNEWS_NATIONAL = 0x000e,
    SNEWS_INTERNATIONAL = 0x000f,
    ENEWS_LOCAL = 0x0010,
    ENEWS_REGIONAL = 0x0011,
    ENEWS_NATIONAL = 0x0012,
    ENEWS_INTERNATIONAL = 0x0013,
    LOCAL_WEATHER = 0x0014,
    TRAFFIC_REPORTS = 0x0015,
    FLIGHT_SCHEDULES = 0x0016,
    RESTAURANTS = 0x0017,
    LODGINGS = 0x0018,
    RETAIL_DIRECTORY = 0x0019,
    ADVERTISEMENTS = 0x001a,
    STOCK_QUOTES = 0x001b,
    EMPLOYMENT = 0x001c,
    MEDICAL = 0x001d,
    TECHNOLOGY_NEWS = 0x001e,
    MULTI_CTG = 0x001f,
    CATPT = 0x0020,
    KDDI_CORP_MIN1 = 0x0021,
    KDDI_CORP_MAX1 = 0x003f,
    CMAS_PRESIDENTIAL = 0x1000,
    CMAS_EXTREME = 0x1001,
    CMAS_SEVERE = 0x1002,
    CMAS_AMBER = 0x1003,
    CMAS_TEST = 0x1004,
    KDDI_CORP_MIN2 = 0x8001,
    KDDI_CORP_MAX2 = 0x803f,
    KDDI_CORP_MIN3 = 0xc001,
    KDDI_CORP_MAX3 = 0xc03f,
    RESERVED,
    UNDEFINED = 0x8001,
};

enum class TransportErrClass { NONE = 0x00, TEMPORARY = 0x02, PERMANENT = 0x03 };

enum class TransportCauseCodeType : uint8_t {
    INVAILD_TELESERVICE_ID = 0x04,
    SERVICE_TERMINATION_DENIED = 0x62,
    SMS_TO_FILL_THIS_ENUMS
};

enum class TransportSubAddrType : uint8_t { NSAP = 0x00, USER = 0x01, RESERVED };

enum class SmsNumberType : uint8_t {
    UNKNOWN = 0x00,
    INTERNATIONAL = 0x01,
    NATIONAL = 0x02,
    NETWORK_SPECIFIC = 0x03,
    SUBSCRIBER = 0x04,
    RESERVED_5 = 0x05,
    ABBREVIATED = 0x06,
    RESERVED_7 = 0x07,
};

struct SmsTeleSvcMsgId {
    uint16_t msgId;
    bool headerInd;
};

struct SmsTeleSvcAddr {
    bool digitMode;
    uint8_t numberType;
    uint8_t numberPlan;
    uint32_t addrLen;
    char szData[CDMASMS_ADDRESS_LEN_MAX + 1];
};

struct SmsReplyOption {
    bool userAck : 1;
    bool dak : 1;
    bool readAck : 1;
    bool report : 1;
};

struct SmsValPeriod {
    uint8_t format;
    union {
        SmsTimeRel relTime;
        SmsTimeAbs absTime;
    } time;
};

struct SmsEncodingSpecific {
    SmsEncodingType encodeType;
    uint32_t dataLen;
    char userData[CDMASMS_USER_DATA_LEN_MAX + 1];
};

struct SmsTeleSvcUserData {
    SmsEncodingType encodeType;
    uint8_t msgType;
    SmsUDPackage userData;
};

struct SmsTeleSvcCmasData {
    uint32_t dataLen;
    SmsEncodingType encodeType;
    uint8_t alertText[CDMASMS_USER_DATA_LEN_MAX + 1];
    SmsCmaeCategory category;
    SmsCmaeResponseType responseType;
    SmsCmaeSeverity severity;
    SmsCmaeUrgency urgency;
    SmsCmaeCertainty certainty;
    uint16_t id;
    SmsCmaeAlertHandle alertHandle;
    SmsTimeAbs expires;
    SmsLanguageType language;
    bool isWrongRecodeType;
};

struct SmsEnhancedVmn {
    SmsPriorityIndicator priority;
    bool passwordReq;
    bool setupReq;
    bool pwChangeReq;
    uint8_t minPwLen;
    uint8_t maxPwLen;
    uint8_t vmNumUnheardMsg;
    bool vmMailboxAlmFull;
    bool vmMailboxFull;
    bool replyAllowed;
    bool faxIncluded;
    uint16_t vmLen;
    uint8_t vmRetDay;
    uint16_t vmMsgId;
    uint16_t vmMailboxId;
    bool anDigitMode;
    uint8_t anNumberType;
    uint8_t anNumberPlan;
    uint8_t anNumField;
    uint8_t anChar[CDMASMS_USER_DATA_LEN_MAX + 1];
    bool cliDigitMode;
    uint8_t cliNumberType;
    uint8_t cliNumberPlan;
    uint8_t cliNumField;
    uint8_t cliChar[CDMASMS_USER_DATA_LEN_MAX + 1];
};

struct SmsEnhancedVmnAck {
    uint16_t vmMailboxId;
    uint8_t vmNumUnheardMsg;
    uint8_t numDeleteAck;
    uint8_t numPlayAck;
    uint16_t daVmMsgId[CDMASMS_NUMBER_OF_ACK_MAX + 1];
    uint16_t paVmMsgId[CDMASMS_NUMBER_OF_ACK_MAX + 1];
};

struct TeleserviceDeliver {
    SmsTeleSvcMsgId msgId;
    SmsTeleSvcUserData userData;
    SmsTeleSvcCmasData cmasData;
    SmsTimeAbs timeStamp;
    SmsValPeriod valPeriod;
    SmsValPeriod deferValPeriod;
    SmsPriorityIndicator priority;
    SmsPrivacyIndicator privacy;
    SmsReplyOption replyOpt;
    uint32_t numMsg;
    SmsAlertPriority alertPriority;
    SmsLanguageType language;
    SmsTeleSvcAddr callbackNumber;
    SmsDisplayMode displayMode;
    SmsEncodingSpecific multiEncodeData;
    uint16_t depositId;
    SmsEnhancedVmn vmn;
    SmsEnhancedVmnAck vmnAck;
};

struct TeleserviceSubmit {
    SmsTeleSvcMsgId msgId;
    SmsTeleSvcUserData userData;
    SmsValPeriod valPeriod;
    SmsValPeriod deferValPeriod;
    SmsPriorityIndicator priority;
    SmsPrivacyIndicator privacy;
    SmsReplyOption replyOpt;
    SmsAlertPriority alertPriority;
    SmsLanguageType language;
    SmsTeleSvcAddr callbackNumber;
    SmsEncodingSpecific multiEncodeData;
    uint16_t depositId;
};

struct TeleserviceCancel {
    SmsTeleSvcMsgId msgId;
};

struct TeleserviceUserAck {
    SmsTeleSvcMsgId msgId;
    SmsTeleSvcUserData userData;
    uint8_t respCode;
    SmsTimeAbs timeStamp;
    SmsEncodingSpecific multiEncodeData;
    uint16_t depositId;
};

struct TeleserviceDeliverAck {
    SmsTeleSvcMsgId msgId;
    SmsTeleSvcUserData userData;
    SmsTimeAbs timeStamp;
    SmsEncodingSpecific multiEncodeData;
    SmsStatusCode msgStatus;
};

struct TeleserviceReadAck {
    SmsTeleSvcMsgId msgId;
    SmsTeleSvcUserData userData;
    SmsTimeAbs timeStamp;
    SmsEncodingSpecific multiEncodeData;
    uint16_t depositId;
};

struct TeleserviceDeliverReport {
    SmsTeleSvcMsgId msgId;
    uint8_t tpFailCause;
    SmsTeleSvcUserData userData;
    SmsLanguageType language;
    uint8_t multiEncodeData;
};

struct CdmaTeleserviceMsg {
    TeleserviceMsgType type;
    union {
        TeleserviceDeliver deliver;
        TeleserviceSubmit submit;
        TeleserviceCancel cancel;
        TeleserviceUserAck userAck;
        TeleserviceDeliverAck deliveryAck;
        TeleserviceReadAck readAck;
        TeleserviceDeliverReport report;
    } data;
};

struct TransportAddr {
    bool digitMode; // ture digit 8bit mode, false digit 4bit dtmf
    bool numberMode; // ture using data network address format, false none data network
    uint8_t numberType;
    uint8_t numberPlan;
    uint32_t addrLen;
    char szData[CDMASMS_ADDRESS_LEN_MAX + 1];
};

struct TransportSubAddr {
    TransportSubAddrType type;
    bool odd;
    uint32_t addrLen;
    char szData[CDMASMS_ADDRESS_LEN_MAX + 1];
};

struct TransportCauseCode {
    uint8_t transReplySeq;
    TransportErrClass errorClass;
    TransportCauseCodeType causeCode;
};

struct CdmaP2PMsg {
    uint16_t teleserviceId;
    uint16_t serviceCtg;
    TransportAddr address;
    TransportSubAddr subAddress;
    uint8_t replySeq;
    CdmaTeleserviceMsg telesvcMsg;
};

struct CdmaBroadCastMsg {
    uint16_t serviceCtg;
    CdmaTeleserviceMsg telesvcMsg;
};

struct CdmaAckMsg {
    TransportAddr address;
    TransportSubAddr subAddress;
    TransportCauseCode causeCode;
};

struct CdmaTransportMsg {
    CdmaTransportMsgType type;
    union {
        CdmaP2PMsg p2p;
        CdmaBroadCastMsg broadcast;
        CdmaAckMsg ack;
    } data;
};

} // namespace Telephony
} // namespace OHOS
#endif
