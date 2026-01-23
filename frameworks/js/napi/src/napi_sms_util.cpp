/*
 * Copyright (C) 2026 Huawei Device Co., Ltd.
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

#include "napi_sms_util.h"

#include <uv.h>

#include "telephony_log_wrapper.h"
#include "napi_mms.h"

namespace OHOS {
namespace Telephony {
namespace {
const std::string g_slotIdStr = "slotId";
const std::string g_destinationHostStr = "destinationHost";
const std::string g_serviceCenterStr = "serviceCenter";
const std::string g_contentStr = "content";
const std::string g_destinationPortStr = "destinationPort";
const std::string g_sendCallbackStr = "sendCallback";
const std::string g_deliveryCallbackStr = "deliveryCallback";
} // namespace
void NapiSmsUtil::SetPropertyArray(napi_env env, napi_value object,
    const std::string &name, std::vector<unsigned char> pdu)
{
    napi_value array = nullptr;
    napi_create_array(env, &array);
    size_t size = pdu.size();
    for (size_t i = 0; i < size; i++) {
        napi_value element = nullptr;
        napi_create_int32(env, pdu[i], &element);
        napi_set_element(env, array, i, element);
    }
    napi_set_named_property(env, object, name.c_str(), array);
}

int32_t NapiSmsUtil::WrapSimMessageStatus(int32_t status)
{
    switch (status) {
        case ShortMessage::SMS_SIM_MESSAGE_STATUS_UNREAD: {
            return static_cast<int32_t>(ISmsServiceInterface::SimMessageStatus::SIM_MESSAGE_STATUS_UNREAD);
        }
        case ShortMessage::SMS_SIM_MESSAGE_STATUS_READ: {
            return static_cast<int32_t>(ISmsServiceInterface::SimMessageStatus::SIM_MESSAGE_STATUS_READ);
        }
        case ShortMessage::SMS_SIM_MESSAGE_STATUS_UNSENT: {
            return static_cast<int32_t>(ISmsServiceInterface::SimMessageStatus::SIM_MESSAGE_STATUS_UNSENT);
        }
        case ShortMessage::SMS_SIM_MESSAGE_STATUS_SENT: {
            return static_cast<int32_t>(ISmsServiceInterface::SimMessageStatus::SIM_MESSAGE_STATUS_SENT);
        }
        default: {
            return MESSAGE_UNKNOWN_STATUS;
        }
    }
}

std::u16string NapiSmsUtil::GetU16StrFromNapiValue(napi_env env, napi_value value)
{
    char strChars[PROPERTY_NAME_SIZE] = {0};
    size_t strLength = 0;
    napi_get_value_string_utf8(env, value, strChars, BUFF_LENGTH, &strLength);
    std::string str8(strChars, strLength);
    return NapiUtil::ToUtf16(str8);
}

bool NapiSmsUtil::MatchObjectProperty(
    napi_env env, napi_value object, std::initializer_list<std::pair<std::string, napi_valuetype>> pairList)
{
    if (object == nullptr) {
        TELEPHONY_LOGI("MatchObjectProperty object == nullptr");
        return false;
    }
    for (auto beg = pairList.begin(); beg != pairList.end(); ++beg) {
        if (!NapiUtil::HasNamedTypeProperty(env, object, beg->second, beg->first)) {
            TELEPHONY_LOGI("MatchObjectProperty match failed!");
            return false;
        }
    }
    return true;
}

int32_t NapiSmsUtil::MatchSendShortMessageParameters(napi_env env, napi_value parameters[], size_t parameterCount)
{
    bool match = false;
    switch (parameterCount) {
        case ONE_PARAMETER: {
            match = NapiUtil::MatchParameters(env, parameters, { napi_object });
            break;
        }
        case TWO_PARAMETERS: {
            match = NapiUtil::MatchParameters(env, parameters, { napi_object, napi_function });
            break;
        }
        default:
            break;
    }
    if (!match) {
        return MESSAGE_PARAMETER_NOT_MATCH;
    }

    napi_value object = parameters[0];
    bool hasSlotId = NapiUtil::HasNamedTypeProperty(env, object, napi_number, g_slotIdStr);
    bool hasDestinationHost = NapiUtil::HasNamedTypeProperty(env, object, napi_string, g_destinationHostStr);
    bool hasContent = NapiUtil::HasNamedProperty(env, object, g_contentStr);
    bool hasNecessaryParameter = hasSlotId && hasDestinationHost && hasContent;
    if (!hasNecessaryParameter) {
        return MESSAGE_PARAMETER_NOT_MATCH;
    }
    napi_value contentValue = NapiUtil::GetNamedProperty(env, object, g_contentStr);
    bool contentIsStr = NapiUtil::MatchValueType(env, contentValue, napi_string);
    bool contentIsObj = NapiUtil::MatchValueType(env, contentValue, napi_object);
    bool contentIsArray = false;
    if (contentIsObj) {
        napi_is_array(env, contentValue, &contentIsArray);
    }
    bool serviceCenterTypeMatch = NapiUtil::MatchOptionPropertyType(env, object, napi_string, g_serviceCenterStr);
    bool sendCallbackTypeMatch = NapiUtil::MatchOptionPropertyType(env, object, napi_function, g_sendCallbackStr);
    bool deliveryCallbackTypeMatch =
        NapiUtil::MatchOptionPropertyType(env, object, napi_function, g_deliveryCallbackStr);
    bool destindationPortMatch = NapiUtil::MatchOptionPropertyType(env, object, napi_number, g_destinationPortStr);
    if (contentIsStr && serviceCenterTypeMatch && sendCallbackTypeMatch && deliveryCallbackTypeMatch) {
        return TEXT_MESSAGE_PARAMETER_MATCH;
    } else if (contentIsArray && serviceCenterTypeMatch && sendCallbackTypeMatch && deliveryCallbackTypeMatch &&
               destindationPortMatch) {
        return RAW_DATA_MESSAGE_PARAMETER_MATCH;
    }
    return MESSAGE_PARAMETER_NOT_MATCH;
}

bool NapiSmsUtil::MatchCreateMessageParameter(napi_env env, const napi_value parameters[], size_t parameterCount)
{
    bool typeMatch = false;
    switch (parameterCount) {
        case TWO_PARAMETERS: {
            typeMatch = NapiUtil::MatchParameters(env, parameters, {napi_object, napi_string});
            break;
        }
        case THREE_PARAMETERS:
            typeMatch = NapiUtil::MatchParameters(env, parameters, {napi_object, napi_string, napi_function});
            break;
        default: {
            return false;
        }
    }
    if (typeMatch) {
        bool isArray = false;
        napi_is_array(env, parameters[0], &isArray);
        return isArray;
    }
    return false;
}

int32_t NapiSmsUtil::MatchSendMessageParameters(napi_env env, napi_value parameters[], size_t parameterCount)
{
    bool match = (parameterCount == 1) && NapiUtil::MatchParameters(env, parameters, {napi_object});
    if (!match) {
        return MESSAGE_PARAMETER_NOT_MATCH;
    }
    napi_value object = parameters[0];
    bool hasSlotId = NapiUtil::HasNamedTypeProperty(env, object, napi_number, g_slotIdStr);
    bool hasDestinationHost = NapiUtil::HasNamedTypeProperty(env, object, napi_string, g_destinationHostStr);
    bool hasContent = NapiUtil::HasNamedProperty(env, object, g_contentStr);
    bool hasNecessaryParameter = hasSlotId && hasDestinationHost && hasContent;
    if (!hasNecessaryParameter) {
        return MESSAGE_PARAMETER_NOT_MATCH;
    }
    napi_value contentValue = NapiUtil::GetNamedProperty(env, object, g_contentStr);
    bool contentIsStr = NapiUtil::MatchValueType(env, contentValue, napi_string);
    bool contentIsObj = NapiUtil::MatchValueType(env, contentValue, napi_object);
    bool contentIsArray = false;
    if (contentIsObj) {
        napi_is_array(env, contentValue, &contentIsArray);
    }
    bool serviceCenterTypeMatch = NapiUtil::MatchOptionPropertyType(env, object, napi_string, g_serviceCenterStr);
    bool sendCallbackTypeMatch = NapiUtil::MatchOptionPropertyType(env, object, napi_function, g_sendCallbackStr);
    bool deliveryCallbackTypeMatch =
        NapiUtil::MatchOptionPropertyType(env, object, napi_function, g_deliveryCallbackStr);
    bool destindationPortMatch = NapiUtil::MatchOptionPropertyType(env, object, napi_number, g_destinationPortStr);
    if (contentIsStr && serviceCenterTypeMatch && sendCallbackTypeMatch && deliveryCallbackTypeMatch) {
        return TEXT_MESSAGE_PARAMETER_MATCH;
    } else if (contentIsArray && serviceCenterTypeMatch && sendCallbackTypeMatch && deliveryCallbackTypeMatch &&
        destindationPortMatch) {
        return RAW_DATA_MESSAGE_PARAMETER_MATCH;
    }
    return MESSAGE_PARAMETER_NOT_MATCH;
}

void __attribute__((noinline)) NapiSmsUtil::Unref(napi_env env, napi_ref ref)
{
    uint32_t refCount = 0;
    if (napi_reference_unref(env, ref, &refCount) == napi_ok && refCount == 0) {
        napi_delete_reference(env, ref);
    }
}
} // namespace Telephony
} // namespace OHOS