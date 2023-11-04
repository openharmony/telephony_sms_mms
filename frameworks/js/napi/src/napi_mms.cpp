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
#include "napi_mms.h"

#include "telephony_permission.h"
#include "ability.h"
#include "napi_base_context.h"
#include "napi_mms_pdu.h"
#include "napi_mms_pdu_helper.h"

namespace OHOS {
namespace Telephony {
napi_value NapiMms::InitEnumMmsCharSets(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_STATIC_PROPERTY("BIG5", NapiUtil::ToInt32Value(env, static_cast<int32_t>(MmsCharSets::BIG5))),
        DECLARE_NAPI_STATIC_PROPERTY(
            "ISO_10646_UCS_2", NapiUtil::ToInt32Value(env, static_cast<int32_t>(MmsCharSets::ISO_10646_UCS_2))),
        DECLARE_NAPI_STATIC_PROPERTY(
            "ISO_8859_1", NapiUtil::ToInt32Value(env, static_cast<int32_t>(MmsCharSets::ISO_8859_1))),
        DECLARE_NAPI_STATIC_PROPERTY(
            "ISO_8859_2", NapiUtil::ToInt32Value(env, static_cast<int32_t>(MmsCharSets::ISO_8859_2))),
        DECLARE_NAPI_STATIC_PROPERTY(
            "ISO_8859_3", NapiUtil::ToInt32Value(env, static_cast<int32_t>(MmsCharSets::ISO_8859_3))),
        DECLARE_NAPI_STATIC_PROPERTY(
            "ISO_8859_4", NapiUtil::ToInt32Value(env, static_cast<int32_t>(MmsCharSets::ISO_8859_4))),
        DECLARE_NAPI_STATIC_PROPERTY(
            "ISO_8859_5", NapiUtil::ToInt32Value(env, static_cast<int32_t>(MmsCharSets::ISO_8859_5))),
        DECLARE_NAPI_STATIC_PROPERTY(
            "ISO_8859_6", NapiUtil::ToInt32Value(env, static_cast<int32_t>(MmsCharSets::ISO_8859_6))),
        DECLARE_NAPI_STATIC_PROPERTY(
            "ISO_8859_7", NapiUtil::ToInt32Value(env, static_cast<int32_t>(MmsCharSets::ISO_8859_7))),
        DECLARE_NAPI_STATIC_PROPERTY(
            "ISO_8859_8", NapiUtil::ToInt32Value(env, static_cast<int32_t>(MmsCharSets::ISO_8859_8))),
        DECLARE_NAPI_STATIC_PROPERTY(
            "ISO_8859_9", NapiUtil::ToInt32Value(env, static_cast<int32_t>(MmsCharSets::ISO_8859_9))),
        DECLARE_NAPI_STATIC_PROPERTY(
            "SHIFT_JIS", NapiUtil::ToInt32Value(env, static_cast<int32_t>(MmsCharSets::SHIFT_JIS))),
        DECLARE_NAPI_STATIC_PROPERTY(
            "US_ASCII", NapiUtil::ToInt32Value(env, static_cast<int32_t>(MmsCharSets::US_ASCII))),
        DECLARE_NAPI_STATIC_PROPERTY("UTF_8", NapiUtil::ToInt32Value(env, static_cast<int32_t>(MmsCharSets::UTF_8))),
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc));
    return exports;
}

static napi_value CreateEnumConstructor(napi_env env, napi_callback_info info)
{
    napi_value thisArg = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &thisArg, &data);
    napi_value global = nullptr;
    napi_get_global(env, &global);
    return thisArg;
}

napi_value NapiMms::InitSupportEnumMmsCharSets(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_STATIC_PROPERTY("BIG5", NapiUtil::ToInt32Value(env, static_cast<int32_t>(MmsCharSets::BIG5))),
        DECLARE_NAPI_STATIC_PROPERTY(
            "ISO_10646_UCS_2", NapiUtil::ToInt32Value(env, static_cast<int32_t>(MmsCharSets::ISO_10646_UCS_2))),
        DECLARE_NAPI_STATIC_PROPERTY(
            "ISO_8859_1", NapiUtil::ToInt32Value(env, static_cast<int32_t>(MmsCharSets::ISO_8859_1))),
        DECLARE_NAPI_STATIC_PROPERTY(
            "ISO_8859_2", NapiUtil::ToInt32Value(env, static_cast<int32_t>(MmsCharSets::ISO_8859_2))),
        DECLARE_NAPI_STATIC_PROPERTY(
            "ISO_8859_3", NapiUtil::ToInt32Value(env, static_cast<int32_t>(MmsCharSets::ISO_8859_3))),
        DECLARE_NAPI_STATIC_PROPERTY(
            "ISO_8859_4", NapiUtil::ToInt32Value(env, static_cast<int32_t>(MmsCharSets::ISO_8859_4))),
        DECLARE_NAPI_STATIC_PROPERTY(
            "ISO_8859_5", NapiUtil::ToInt32Value(env, static_cast<int32_t>(MmsCharSets::ISO_8859_5))),
        DECLARE_NAPI_STATIC_PROPERTY(
            "ISO_8859_6", NapiUtil::ToInt32Value(env, static_cast<int32_t>(MmsCharSets::ISO_8859_6))),
        DECLARE_NAPI_STATIC_PROPERTY(
            "ISO_8859_7", NapiUtil::ToInt32Value(env, static_cast<int32_t>(MmsCharSets::ISO_8859_7))),
        DECLARE_NAPI_STATIC_PROPERTY(
            "ISO_8859_8", NapiUtil::ToInt32Value(env, static_cast<int32_t>(MmsCharSets::ISO_8859_8))),
        DECLARE_NAPI_STATIC_PROPERTY(
            "ISO_8859_9", NapiUtil::ToInt32Value(env, static_cast<int32_t>(MmsCharSets::ISO_8859_9))),
        DECLARE_NAPI_STATIC_PROPERTY(
            "SHIFT_JIS", NapiUtil::ToInt32Value(env, static_cast<int32_t>(MmsCharSets::SHIFT_JIS))),
        DECLARE_NAPI_STATIC_PROPERTY(
            "US_ASCII", NapiUtil::ToInt32Value(env, static_cast<int32_t>(MmsCharSets::US_ASCII))),
        DECLARE_NAPI_STATIC_PROPERTY("UTF_8", NapiUtil::ToInt32Value(env, static_cast<int32_t>(MmsCharSets::UTF_8))),
    };
    napi_value result = nullptr;
    napi_define_class(env, "MmsCharSets", NAPI_AUTO_LENGTH, CreateEnumConstructor, nullptr,
        sizeof(desc) / sizeof(*desc), desc, &result);
    napi_set_named_property(env, exports, "MmsCharSets", result);
    return exports;
}

napi_value NapiMms::InitEnumMessageType(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_STATIC_PROPERTY("TYPE_MMS_SEND_REQ",
            NapiUtil::ToInt32Value(env, static_cast<int32_t>(MmsMsgType::MMS_MSGTYPE_SEND_REQ))),
        DECLARE_NAPI_STATIC_PROPERTY("TYPE_MMS_SEND_CONF",
            NapiUtil::ToInt32Value(env, static_cast<int32_t>(MmsMsgType::MMS_MSGTYPE_SEND_CONF))),
        DECLARE_NAPI_STATIC_PROPERTY("TYPE_MMS_NOTIFICATION_IND",
            NapiUtil::ToInt32Value(env, static_cast<int32_t>(MmsMsgType::MMS_MSGTYPE_NOTIFICATION_IND))),
        DECLARE_NAPI_STATIC_PROPERTY("TYPE_MMS_RESP_IND",
            NapiUtil::ToInt32Value(env, static_cast<int32_t>(MmsMsgType::MMS_MSGTYPE_NOTIFYRESP_IND))),
        DECLARE_NAPI_STATIC_PROPERTY("TYPE_MMS_RETRIEVE_CONF",
            NapiUtil::ToInt32Value(env, static_cast<int32_t>(MmsMsgType::MMS_MSGTYPE_RETRIEVE_CONF))),
        DECLARE_NAPI_STATIC_PROPERTY("TYPE_MMS_ACKNOWLEDGE_IND",
            NapiUtil::ToInt32Value(env, static_cast<int32_t>(MmsMsgType::MMS_MSGTYPE_ACKNOWLEDGE_IND))),
        DECLARE_NAPI_STATIC_PROPERTY("TYPE_MMS_DELIVERY_IND",
            NapiUtil::ToInt32Value(env, static_cast<int32_t>(MmsMsgType::MMS_MSGTYPE_DELIVERY_IND))),
        DECLARE_NAPI_STATIC_PROPERTY("TYPE_MMS_READ_REC_IND",
            NapiUtil::ToInt32Value(env, static_cast<int32_t>(MmsMsgType::MMS_MSGTYPE_READ_REC_IND))),
        DECLARE_NAPI_STATIC_PROPERTY("TYPE_MMS_READ_ORIG_IND",
            NapiUtil::ToInt32Value(env, static_cast<int32_t>(MmsMsgType::MMS_MSGTYPE_READ_ORIG_IND))),
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc));
    return exports;
}

napi_value NapiMms::InitSupportEnumMessageType(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_STATIC_PROPERTY("TYPE_MMS_SEND_REQ",
            NapiUtil::ToInt32Value(env, static_cast<int32_t>(MmsMsgType::MMS_MSGTYPE_SEND_REQ))),
        DECLARE_NAPI_STATIC_PROPERTY("TYPE_MMS_SEND_CONF",
            NapiUtil::ToInt32Value(env, static_cast<int32_t>(MmsMsgType::MMS_MSGTYPE_SEND_CONF))),
        DECLARE_NAPI_STATIC_PROPERTY("TYPE_MMS_NOTIFICATION_IND",
            NapiUtil::ToInt32Value(env, static_cast<int32_t>(MmsMsgType::MMS_MSGTYPE_NOTIFICATION_IND))),
        DECLARE_NAPI_STATIC_PROPERTY("TYPE_MMS_RESP_IND",
            NapiUtil::ToInt32Value(env, static_cast<int32_t>(MmsMsgType::MMS_MSGTYPE_NOTIFYRESP_IND))),
        DECLARE_NAPI_STATIC_PROPERTY("TYPE_MMS_RETRIEVE_CONF",
            NapiUtil::ToInt32Value(env, static_cast<int32_t>(MmsMsgType::MMS_MSGTYPE_RETRIEVE_CONF))),
        DECLARE_NAPI_STATIC_PROPERTY("TYPE_MMS_ACKNOWLEDGE_IND",
            NapiUtil::ToInt32Value(env, static_cast<int32_t>(MmsMsgType::MMS_MSGTYPE_ACKNOWLEDGE_IND))),
        DECLARE_NAPI_STATIC_PROPERTY("TYPE_MMS_DELIVERY_IND",
            NapiUtil::ToInt32Value(env, static_cast<int32_t>(MmsMsgType::MMS_MSGTYPE_DELIVERY_IND))),
        DECLARE_NAPI_STATIC_PROPERTY("TYPE_MMS_READ_REC_IND",
            NapiUtil::ToInt32Value(env, static_cast<int32_t>(MmsMsgType::MMS_MSGTYPE_READ_REC_IND))),
        DECLARE_NAPI_STATIC_PROPERTY("TYPE_MMS_READ_ORIG_IND",
            NapiUtil::ToInt32Value(env, static_cast<int32_t>(MmsMsgType::MMS_MSGTYPE_READ_ORIG_IND))),
    };
    napi_value result = nullptr;
    napi_define_class(env, "MessageType", NAPI_AUTO_LENGTH, CreateEnumConstructor, nullptr,
        sizeof(desc) / sizeof(*desc), desc, &result);
    napi_set_named_property(env, exports, "MessageType", result);
    return exports;
}

napi_value NapiMms::InitEnumPriorityType(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_STATIC_PROPERTY(
            "MMS_LOW", NapiUtil::ToInt32Value(env, static_cast<int32_t>(MmsPriority::MMS_LOW))),
        DECLARE_NAPI_STATIC_PROPERTY(
            "MMS_NORMAL", NapiUtil::ToInt32Value(env, static_cast<int32_t>(MmsPriority::MMS_NORMAL))),
        DECLARE_NAPI_STATIC_PROPERTY(
            "MMS_HIGH", NapiUtil::ToInt32Value(env, static_cast<int32_t>(MmsPriority::MMS_HIGH))),
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc));
    return exports;
}

napi_value NapiMms::InitSupportEnumPriorityType(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_STATIC_PROPERTY(
            "MMS_LOW", NapiUtil::ToInt32Value(env, static_cast<int32_t>(MmsPriority::MMS_LOW))),
        DECLARE_NAPI_STATIC_PROPERTY(
            "MMS_NORMAL", NapiUtil::ToInt32Value(env, static_cast<int32_t>(MmsPriority::MMS_NORMAL))),
        DECLARE_NAPI_STATIC_PROPERTY(
            "MMS_HIGH", NapiUtil::ToInt32Value(env, static_cast<int32_t>(MmsPriority::MMS_HIGH))),
    };
    napi_value result = nullptr;
    napi_define_class(env, "MmsPriorityType", NAPI_AUTO_LENGTH, CreateEnumConstructor, nullptr,
        sizeof(desc) / sizeof(*desc), desc, &result);
    napi_set_named_property(env, exports, "MmsPriorityType", result);
    return exports;
}

napi_value NapiMms::InitEnumVersionType(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_STATIC_PROPERTY(
            "MMS_VERSION_1_0", NapiUtil::ToInt32Value(env, static_cast<int32_t>(MmsVersionType::MMS_VERSION_1_0))),
        DECLARE_NAPI_STATIC_PROPERTY(
            "MMS_VERSION_1_1", NapiUtil::ToInt32Value(env, static_cast<int32_t>(MmsVersionType::MMS_VERSION_1_1))),
        DECLARE_NAPI_STATIC_PROPERTY(
            "MMS_VERSION_1_2", NapiUtil::ToInt32Value(env, static_cast<int32_t>(MmsVersionType::MMS_VERSION_1_2))),
        DECLARE_NAPI_STATIC_PROPERTY(
            "MMS_VERSION_1_3", NapiUtil::ToInt32Value(env, static_cast<int32_t>(MmsVersionType::MMS_VERSION_1_3))),
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc));
    return exports;
}

napi_value NapiMms::InitSupportEnumVersionType(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_STATIC_PROPERTY(
            "MMS_VERSION_1_0", NapiUtil::ToInt32Value(env, static_cast<int32_t>(MmsVersionType::MMS_VERSION_1_0))),
        DECLARE_NAPI_STATIC_PROPERTY(
            "MMS_VERSION_1_1", NapiUtil::ToInt32Value(env, static_cast<int32_t>(MmsVersionType::MMS_VERSION_1_1))),
        DECLARE_NAPI_STATIC_PROPERTY(
            "MMS_VERSION_1_2", NapiUtil::ToInt32Value(env, static_cast<int32_t>(MmsVersionType::MMS_VERSION_1_2))),
        DECLARE_NAPI_STATIC_PROPERTY(
            "MMS_VERSION_1_3", NapiUtil::ToInt32Value(env, static_cast<int32_t>(MmsVersionType::MMS_VERSION_1_3))),
    };
    napi_value result = nullptr;
    napi_define_class(env, "MmsVersionType", NAPI_AUTO_LENGTH, CreateEnumConstructor, nullptr,
        sizeof(desc) / sizeof(*desc), desc, &result);
    napi_set_named_property(env, exports, "MmsVersionType", result);
    return exports;
}

napi_value NapiMms::InitEnumDispositionType(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_STATIC_PROPERTY(
            "FROM_DATA", NapiUtil::ToInt32Value(env, static_cast<int32_t>(DispositionValue::FROM_DATA))),
        DECLARE_NAPI_STATIC_PROPERTY(
            "ATTACHMENT", NapiUtil::ToInt32Value(env, static_cast<int32_t>(DispositionValue::ATTACHMENT))),
        DECLARE_NAPI_STATIC_PROPERTY(
            "INLINE", NapiUtil::ToInt32Value(env, static_cast<int32_t>(DispositionValue::INLINE))),
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc));
    return exports;
}

napi_value NapiMms::InitSupportEnumDispositionType(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_STATIC_PROPERTY(
            "FROM_DATA", NapiUtil::ToInt32Value(env, static_cast<int32_t>(DispositionValue::FROM_DATA))),
        DECLARE_NAPI_STATIC_PROPERTY(
            "ATTACHMENT", NapiUtil::ToInt32Value(env, static_cast<int32_t>(DispositionValue::ATTACHMENT))),
        DECLARE_NAPI_STATIC_PROPERTY(
            "INLINE", NapiUtil::ToInt32Value(env, static_cast<int32_t>(DispositionValue::INLINE))),
    };
    napi_value result = nullptr;
    napi_define_class(env, "DispositionType", NAPI_AUTO_LENGTH, CreateEnumConstructor, nullptr,
        sizeof(desc) / sizeof(*desc), desc, &result);
    napi_set_named_property(env, exports, "DispositionType", result);
    return exports;
}

napi_value NapiMms::InitEnumReportAllowedType(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_STATIC_PROPERTY(
            "MMS_YES", NapiUtil::ToInt32Value(env, static_cast<int32_t>(MmsBoolType::MMS_YES))),
        DECLARE_NAPI_STATIC_PROPERTY(
            "MMS_NO", NapiUtil::ToInt32Value(env, static_cast<int32_t>(MmsBoolType::MMS_NO))),
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc));
    return exports;
}

napi_value NapiMms::InitSupportEnumReportAllowedType(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_STATIC_PROPERTY(
            "MMS_YES", NapiUtil::ToInt32Value(env, static_cast<int32_t>(MmsBoolType::MMS_YES))),
        DECLARE_NAPI_STATIC_PROPERTY(
            "MMS_NO", NapiUtil::ToInt32Value(env, static_cast<int32_t>(MmsBoolType::MMS_NO))),
    };
    napi_value result = nullptr;
    napi_define_class(env, "ReportType", NAPI_AUTO_LENGTH, CreateEnumConstructor, nullptr,
        sizeof(desc) / sizeof(*desc), desc, &result);
    napi_set_named_property(env, exports, "ReportType", result);
    return exports;
}
} // namespace Telephony
} // namespace OHOS
