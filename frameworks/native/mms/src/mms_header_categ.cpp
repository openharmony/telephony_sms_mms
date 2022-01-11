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

#include "mms_header_categ.h"

namespace OHOS {
namespace Telephony {
MmsHeaderCateg::MmsHeaderCateg()
{
    InitFieldDesMap();

    InitSendReqMap();
    InitSendConfMap();
}

MmsHeaderCateg::~MmsHeaderCateg()
{
    mmsFieldDesMap_.clear();

    mmsSendReqOptMap_.clear();
    mmsSendConfOptMap_.clear();
}

void MmsHeaderCateg::InitFieldDesMap()
{
    mmsFieldDesMap_.clear();
    /* MMS 1.0 */
    InitFieldDesV10();
    /* MMS 1.1 */
    InitFieldDesV11();
    /* MMS 1.2 */
    InitFieldDesV12();
}

void MmsHeaderCateg::InitFieldDesV10()
{
    /* MMS 1.0 */
    mmsFieldDesMap_.emplace(MMS_BCC, MmsFieldValueType::MMS_FIELD_ENCODE_ADDR_TYPE);
    mmsFieldDesMap_.emplace(MMS_CC, MmsFieldValueType::MMS_FIELD_ENCODE_ADDR_TYPE);
    mmsFieldDesMap_.emplace(MMS_CONTENT_LOCATION, MmsFieldValueType::MMS_FIELD_TEXT_TYPE);
    mmsFieldDesMap_.emplace(MMS_CONTENT_TYPE, MmsFieldValueType::MMS_FIELD_UNKOWN_TYPE);
    mmsFieldDesMap_.emplace(MMS_DATE, MmsFieldValueType::MMS_FIELD_LONG_TYPE);
    mmsFieldDesMap_.emplace(MMS_DELIVERY_REPORT, MmsFieldValueType::MMS_FIELD_OCTET_TYPE);
    mmsFieldDesMap_.emplace(MMS_DELIVERY_TIME, MmsFieldValueType::MMS_FIELD_LONG_TYPE);
    mmsFieldDesMap_.emplace(MMS_EXPIRY, MmsFieldValueType::MMS_FIELD_LONG_TYPE);
    mmsFieldDesMap_.emplace(MMS_FROM, MmsFieldValueType::MMS_FIELD_ENCODE_ADDR_TYPE);
    mmsFieldDesMap_.emplace(MMS_MESSAGE_CLASS, MmsFieldValueType::MMS_FIELD_OCTET_TYPE);
    mmsFieldDesMap_.emplace(MMS_MESSAGE_ID, MmsFieldValueType::MMS_FIELD_TEXT_TYPE);
    mmsFieldDesMap_.emplace(MMS_MESSAGE_TYPE, MmsFieldValueType::MMS_FIELD_OCTET_TYPE);
    mmsFieldDesMap_.emplace(MMS_MMS_VERSION, MmsFieldValueType::MMS_FIELD_INTEGER_TYPE);
    mmsFieldDesMap_.emplace(MMS_MESSAGE_SIZE, MmsFieldValueType::MMS_FIELD_LONG_TYPE);
    mmsFieldDesMap_.emplace(MMS_PRIORITY, MmsFieldValueType::MMS_FIELD_OCTET_TYPE);
    mmsFieldDesMap_.emplace(MMS_READ_REPORT, MmsFieldValueType::MMS_FIELD_OCTET_TYPE);
    mmsFieldDesMap_.emplace(MMS_REPORT_ALLOWED, MmsFieldValueType::MMS_FIELD_OCTET_TYPE);
    mmsFieldDesMap_.emplace(MMS_RESPONSE_STATUS, MmsFieldValueType::MMS_FIELD_OCTET_TYPE);
    mmsFieldDesMap_.emplace(MMS_RESPONSE_TEXT, MmsFieldValueType::MMS_FIELD_OCTET_TYPE);
    mmsFieldDesMap_.emplace(MMS_SENDER_VISIBILITY, MmsFieldValueType::MMS_FIELD_OCTET_TYPE);
    mmsFieldDesMap_.emplace(MMS_STATUS, MmsFieldValueType::MMS_FIELD_OCTET_TYPE);
    mmsFieldDesMap_.emplace(MMS_SUBJECT, MmsFieldValueType::MMS_FIELD_ENCODE_TEXT_TYPE);
    mmsFieldDesMap_.emplace(MMS_TO, MmsFieldValueType::MMS_FIELD_ENCODE_ADDR_TYPE);
    mmsFieldDesMap_.emplace(MMS_TRANSACTION_ID, MmsFieldValueType::MMS_FIELD_TEXT_TYPE);
}

void MmsHeaderCateg::InitFieldDesV11()
{
    /* MMS 1.1 */
    mmsFieldDesMap_.emplace(MMS_RETRIEVE_STATUS, MmsFieldValueType::MMS_FIELD_OCTET_TYPE);
    mmsFieldDesMap_.emplace(MMS_RETRIEVE_TEXT, MmsFieldValueType::MMS_FIELD_ENCODE_TEXT_TYPE);
    mmsFieldDesMap_.emplace(MMS_READ_STATUS, MmsFieldValueType::MMS_FIELD_OCTET_TYPE);
    mmsFieldDesMap_.emplace(MMS_REPLY_CHARGING, MmsFieldValueType::MMS_FIELD_OCTET_TYPE);
    mmsFieldDesMap_.emplace(MMS_REPLY_CHARGING_DEADLINE, MmsFieldValueType::MMS_FIELD_LONG_TYPE);
    mmsFieldDesMap_.emplace(MMS_REPLY_CHARGING_ID, MmsFieldValueType::MMS_FIELD_TEXT_TYPE);
    mmsFieldDesMap_.emplace(MMS_REPLY_CHARGING_SIZE, MmsFieldValueType::MMS_FIELD_LONG_TYPE);
    mmsFieldDesMap_.emplace(MMS_PREVIOUSLY_SENT_BY, MmsFieldValueType::MMS_FIELD_ENCODE_TEXT_TYPE);
    mmsFieldDesMap_.emplace(MMS_PREVIOUSLY_SENT_DATE, MmsFieldValueType::MMS_FIELD_LONG_TYPE);
}

void MmsHeaderCateg::InitFieldDesV12()
{
    /* MMS 1.2 */
    mmsFieldDesMap_.emplace(MMS_STORE, MmsFieldValueType::MMS_FIELD_OCTET_TYPE);
    mmsFieldDesMap_.emplace(MMS_MM_STATE, MmsFieldValueType::MMS_FIELD_OCTET_TYPE);
    mmsFieldDesMap_.emplace(MMS_MM_FLAGS, MmsFieldValueType::MMS_FIELD_ENCODE_TEXT_TYPE);
    mmsFieldDesMap_.emplace(MMS_STORE_STATUS, MmsFieldValueType::MMS_FIELD_OCTET_TYPE);
    mmsFieldDesMap_.emplace(MMS_STORE_STATUS_TEXT, MmsFieldValueType::MMS_FIELD_ENCODE_TEXT_TYPE);
    mmsFieldDesMap_.emplace(MMS_STORED, MmsFieldValueType::MMS_FIELD_OCTET_TYPE);
    mmsFieldDesMap_.emplace(MMS_ATTRIBUTES, MmsFieldValueType::MMS_FIELD_OCTET_TYPE);
    mmsFieldDesMap_.emplace(MMS_TOTALS, MmsFieldValueType::MMS_FIELD_OCTET_TYPE);
    mmsFieldDesMap_.emplace(MMS_MBOX_TOTALS, MmsFieldValueType::MMS_FIELD_INTEGER_TYPE);
    mmsFieldDesMap_.emplace(MMS_QUOTAS, MmsFieldValueType::MMS_FIELD_OCTET_TYPE);
    mmsFieldDesMap_.emplace(MMS_MBOX_QUOTAS, MmsFieldValueType::MMS_FIELD_INTEGER_TYPE);
    mmsFieldDesMap_.emplace(MMS_MESSAGE_COUNT, MmsFieldValueType::MMS_FIELD_INTEGER_TYPE);
    mmsFieldDesMap_.emplace(MMS_CONTENT, MmsFieldValueType::MMS_FIELD_UNKOWN_TYPE);
    mmsFieldDesMap_.emplace(MMS_START, MmsFieldValueType::MMS_FIELD_INTEGER_TYPE);
    mmsFieldDesMap_.emplace(MMS_ADDITIONAL_HEADERS, MmsFieldValueType::MMS_FIELD_UNKOWN_TYPE);
    mmsFieldDesMap_.emplace(MMS_DISTRIBUTION_INDICATOR, MmsFieldValueType::MMS_FIELD_OCTET_TYPE);
    mmsFieldDesMap_.emplace(MMS_ELEMENT_DESCRIPTOR, MmsFieldValueType::MMS_FIELD_UNKOWN_TYPE);
    mmsFieldDesMap_.emplace(MMS_LIMIT, MmsFieldValueType::MMS_FIELD_INTEGER_TYPE);
    mmsFieldDesMap_.emplace(MMS_CONTENT_CLASS, MmsFieldValueType::MMS_FIELD_OCTET_TYPE);
}

void MmsHeaderCateg::InitSendReqMap()
{
    mmsSendReqOptMap_.clear();
    mmsSendReqOptMap_.emplace(MMS_MESSAGE_TYPE, MmsFieldOptType::MMS_MANDATORY_TYPE);
    mmsSendReqOptMap_.emplace(MMS_TRANSACTION_ID, MmsFieldOptType::MMS_MANDATORY_TYPE);
    mmsSendReqOptMap_.emplace(MMS_MMS_VERSION, MmsFieldOptType::MMS_MANDATORY_TYPE);
    mmsSendReqOptMap_.emplace(MMS_CONTENT_TYPE, MmsFieldOptType::MMS_MANDATORY_TYPE);
    mmsSendReqOptMap_.emplace(MMS_FROM, MmsFieldOptType::MMS_MANDATORY_TYPE);

    mmsSendReqOptMap_.emplace(MMS_DATE, MmsFieldOptType::MMS_OPTIONAL_TYPE);
    mmsSendReqOptMap_.emplace(MMS_TO, MmsFieldOptType::MMS_OPTIONAL_TYPE);
    mmsSendReqOptMap_.emplace(MMS_CC, MmsFieldOptType::MMS_OPTIONAL_TYPE);
    mmsSendReqOptMap_.emplace(MMS_BCC, MmsFieldOptType::MMS_OPTIONAL_TYPE);
    mmsSendReqOptMap_.emplace(MMS_SUBJECT, MmsFieldOptType::MMS_OPTIONAL_TYPE);
    mmsSendReqOptMap_.emplace(MMS_MESSAGE_CLASS, MmsFieldOptType::MMS_OPTIONAL_TYPE);
    mmsSendReqOptMap_.emplace(MMS_EXPIRY, MmsFieldOptType::MMS_OPTIONAL_TYPE);
    mmsSendReqOptMap_.emplace(MMS_PRIORITY, MmsFieldOptType::MMS_OPTIONAL_TYPE);
    mmsSendReqOptMap_.emplace(MMS_SENDER_VISIBILITY, MmsFieldOptType::MMS_OPTIONAL_TYPE);
    mmsSendReqOptMap_.emplace(MMS_DELIVERY_REPORT, MmsFieldOptType::MMS_OPTIONAL_TYPE);
    mmsSendReqOptMap_.emplace(MMS_MESSAGE_TYPE, MmsFieldOptType::MMS_OPTIONAL_TYPE);
    mmsSendReqOptMap_.emplace(MMS_READ_REPORT, MmsFieldOptType::MMS_OPTIONAL_TYPE);
}

void MmsHeaderCateg::InitSendConfMap()
{
    mmsSendConfOptMap_.clear();
}

bool MmsHeaderCateg::CheckIsValueLen(uint8_t fieldId)
{
    return false;
}

MmsFieldValueType MmsHeaderCateg::FindFieldDes(uint8_t fieldId)
{
    auto it = mmsFieldDesMap_.find(fieldId);
    if (it != mmsFieldDesMap_.end()) {
        return it->second;
    }
    return MmsFieldValueType::MMS_FIELD_UNKOWN_TYPE;
}

MmsFieldOptType MmsHeaderCateg::FindSendReqOptType(uint8_t fieldId)
{
    auto it = mmsSendReqOptMap_.find(fieldId);
    if (it != mmsSendReqOptMap_.end()) {
        return it->second;
    }
    return MmsFieldOptType::MMS_UNSUPPORTED_TYPE;
}

MmsFieldOptType MmsHeaderCateg::FindSendConfOptType(uint8_t fieldId)
{
    return MmsFieldOptType::MMS_UNSUPPORTED_TYPE;
}
} // namespace Telephony
} // namespace OHOS
