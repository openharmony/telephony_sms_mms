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

#include "napi_sms.h"
#include "napi_mms.h"
#include "delivery_callback.h"
#include "send_callback.h"

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
static const int32_t DEFAULT_REF_COUNT = 1;
} // namespace

static void SetPropertyArray(napi_env env, napi_value object, std::string name, std::vector<unsigned char> pdu)
{
    napi_value array = nullptr;
    napi_create_array(env, &array);
    int size = pdu.size();
    for (int i = 0; i < size; i++) {
        napi_value element = nullptr;
        napi_create_int32(env, pdu[i], &element);
        napi_set_element(env, array, i, element);
    }
    napi_set_named_property(env, object, name.c_str(), array);
}

static int32_t WrapSimMessageStatus(int32_t status)
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

static int32_t GetDefaultSmsSlotId()
{
    return DEFAULT_SIM_SLOT_ID;
}

static bool MatchObjectProperty(
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

static std::string Get64StringFromValue(napi_env env, napi_value value)
{
    char msgChars[MAX_TEXT_SHORT_MESSAGE_LENGTH] = {0};
    size_t strLength = 0;
    napi_get_value_string_utf8(env, value, msgChars, MAX_TEXT_SHORT_MESSAGE_LENGTH, &strLength);
    TELEPHONY_LOGI("Get64StringFromValue strLength = %{public}zu", strLength);
    if (strLength > 0) {
        return std::string(msgChars, 0, strLength);
    } else {
        return "";
    }
}

static std::u16string GetU16StrFromNapiValue(napi_env env, napi_value value)
{
    char strChars[PROPERTY_NAME_SIZE] = {0};
    size_t strLength = 0;
    napi_get_value_string_utf8(env, value, strChars, BUFF_LENGTH, &strLength);
    std::string str8(strChars, strLength);
    return NapiUtil::ToUtf16(str8);
}

static bool ActuallySendMessage(napi_env env, SendMessageContext &parameter)
{
    if (parameter.slotId < 0) {
        TELEPHONY_LOGI("ActuallySendMessage parameter.slotId < 0 illegal slotId");
        return false;
    }
    bool hasSendCallback = parameter.sendCallbackRef != nullptr;
    std::unique_ptr<SendCallback> sendCallback =
        std::make_unique<SendCallback>(hasSendCallback, env, parameter.thisVarRef, parameter.sendCallbackRef);
    if (sendCallback == nullptr) {
        TELEPHONY_LOGI("ActuallySendMessage sendCallback == nullptr");
        return false;
    }
    bool hasDeliveryCallback = parameter.deliveryCallbackRef != nullptr;
    std::unique_ptr<DeliveryCallback> deliveryCallback = std::make_unique<DeliveryCallback>(
        hasDeliveryCallback, env, parameter.thisVarRef, parameter.deliveryCallbackRef);
    if (deliveryCallback == nullptr) {
        TELEPHONY_LOGI("ActuallySendMessage deliveryCallback == nullptr");
        return false;
    }
    if (parameter.messageType == TEXT_MESSAGE_PARAMETER_MATCH) {
        int32_t sendResult = ShortMessageManager::SendMessage(parameter.slotId, parameter.destinationHost,
            parameter.serviceCenter, parameter.textContent, sendCallback.release(), deliveryCallback.release());
        TELEPHONY_LOGI("NativeSendMessage SendTextMessage execResult = %{public}d", sendResult);
        if (sendResult == ERROR_NONE) {
            return true;
        } else {
            return false;
        }
    } else if (parameter.messageType == RAW_DATA_MESSAGE_PARAMETER_MATCH) {
        if (parameter.rawDataContent.size() > 0) {
            uint16_t arrayLength = static_cast<uint16_t>(parameter.rawDataContent.size());
            int32_t sendResult = ShortMessageManager::SendMessage(parameter.slotId, parameter.destinationHost,
                parameter.serviceCenter, parameter.destinationPort, &parameter.rawDataContent[0], arrayLength,
                sendCallback.release(), deliveryCallback.release());
            TELEPHONY_LOGI("NativeSendMessage SendRawDataMessage execResult = %{public}d", sendResult);
            if (sendResult == ERROR_NONE) {
                return true;
            } else {
                return false;
            }
        }
    }
    return false;
}

static void NativeSendMessage(napi_env env, void *data)
{
    auto asyncContext = static_cast<SendMessageContext *>(data);
    if (asyncContext != nullptr) {
        asyncContext->resolved = ActuallySendMessage(env, *asyncContext);
    }
}

static void SendMessageCallback(napi_env env, napi_status status, void *data)
{
    auto asyncContext = static_cast<SendMessageContext *>(data);
    if (!asyncContext->resolved) {
        TELEPHONY_LOGI("SendMessageCallback status == REJECT");
        if (asyncContext->thisVarRef != nullptr) {
            TELEPHONY_LOGI("SendMessageCallback thisVarRef != nullptr");
            napi_value thisVar = nullptr;
            napi_get_reference_value(env, asyncContext->thisVarRef, &thisVar);
            if (asyncContext->sendCallbackRef != nullptr) {
                TELEPHONY_LOGI("SendMessageCallback sendCallbackRef != nullptr");
                napi_value sendCallback = nullptr;
                napi_get_reference_value(env, asyncContext->sendCallbackRef, &sendCallback);
                napi_value callbackValues[2] = {0};
                callbackValues[0] = NapiUtil::CreateErrorMessage(env, "parameter illegal");
                napi_get_undefined(env, &callbackValues[1]);
                napi_value undefined = nullptr;
                napi_get_undefined(env, &undefined);
                napi_value callbackResult = nullptr;
                napi_call_function(
                    env, undefined, sendCallback, std::size(callbackValues), callbackValues, &callbackResult);
                TELEPHONY_LOGI("SendMessageCallback after napi_call_function");
                napi_delete_reference(env, asyncContext->sendCallbackRef);
            }
            if (asyncContext->deliveryCallbackRef != nullptr) {
                napi_delete_reference(env, asyncContext->deliveryCallbackRef);
            }
            napi_delete_reference(env, asyncContext->thisVarRef);
        }
    }
    napi_delete_async_work(env, asyncContext->work);
}

static int32_t MatchSendMessageParameters(napi_env env, napi_value parameters[], size_t parameterCount)
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

static void ParseMessageParameter(
    int32_t messageMatchResult, napi_env env, napi_value object, SendMessageContext &context)
{
    context.messageType = messageMatchResult;
    context.slotId = GetDefaultSmsSlotId();
    napi_value slotIdValue = nullptr;
    napi_get_named_property(env, object, g_slotIdStr.data(), &slotIdValue);
    napi_get_value_int32(env, slotIdValue, &context.slotId);
    napi_value destinationHostValue = NapiUtil::GetNamedProperty(env, object, g_destinationHostStr);
    context.destinationHost = GetU16StrFromNapiValue(env, destinationHostValue);
    if (NapiUtil::HasNamedProperty(env, object, g_serviceCenterStr)) {
        napi_value serviceCenterValue = NapiUtil::GetNamedProperty(env, object, g_serviceCenterStr);
        context.serviceCenter = GetU16StrFromNapiValue(env, serviceCenterValue);
    }
    if (NapiUtil::HasNamedProperty(env, object, g_sendCallbackStr)) {
        napi_value sendCallbackValue = NapiUtil::GetNamedProperty(env, object, g_sendCallbackStr);
        napi_create_reference(env, sendCallbackValue, DEFAULT_REF_COUNT, &context.sendCallbackRef);
    }
    if (NapiUtil::HasNamedProperty(env, object, g_deliveryCallbackStr)) {
        napi_value deliveryCallbackValue = NapiUtil::GetNamedProperty(env, object, g_deliveryCallbackStr);
        napi_create_reference(env, deliveryCallbackValue, DEFAULT_REF_COUNT, &context.deliveryCallbackRef);
    }
    napi_value contentValue = NapiUtil::GetNamedProperty(env, object, g_contentStr);
    if (messageMatchResult == TEXT_MESSAGE_PARAMETER_MATCH) {
        char contentChars[MAX_TEXT_SHORT_MESSAGE_LENGTH] = {0};
        size_t contentLength = 0;
        napi_get_value_string_utf8(env, contentValue, contentChars, MAX_TEXT_SHORT_MESSAGE_LENGTH, &contentLength);
        std::string text(contentChars, contentLength);
        context.textContent = NapiUtil::ToUtf16(text);
    }
    if (messageMatchResult == RAW_DATA_MESSAGE_PARAMETER_MATCH) {
        int32_t destinationPort = DEFAULT_PORT;
        napi_value destinationPortValue = nullptr;
        napi_get_named_property(env, object, g_destinationPortStr.data(), &destinationPortValue);
        napi_get_value_int32(env, destinationPortValue, &destinationPort);
        context.destinationPort = static_cast<uint16_t>(destinationPort);
        napi_value elementValue = nullptr;
        int32_t element = 0;
        uint32_t valueArraySize = 0;
        napi_get_array_length(env, contentValue, &valueArraySize);
        auto arraySize = static_cast<int32_t>(valueArraySize);
        for (int32_t i = 0; i < arraySize; i++) {
            napi_get_element(env, contentValue, i, &elementValue);
            napi_get_value_int32(env, elementValue, &element);
            context.rawDataContent.push_back((uint8_t)element);
        }
    }
}

static napi_value SendMessage(napi_env env, napi_callback_info info)
{
    size_t parameterCount = 1;
    napi_value parameters[1] = {0};
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, &parameterCount, parameters, &thisVar, &data);
    int32_t messageMatchResult = MatchSendMessageParameters(env, parameters, parameterCount);
    NAPI_ASSERT(env, messageMatchResult != MESSAGE_PARAMETER_NOT_MATCH, "type mismatch");
    auto asyncContext = std::make_unique<SendMessageContext>().release();
    ParseMessageParameter(messageMatchResult, env, parameters[0], *asyncContext);
    napi_create_reference(env, thisVar, DEFAULT_REF_COUNT, &asyncContext->thisVarRef);
    napi_value resourceName = nullptr;
    napi_create_string_utf8(env, "SendMessage", NAPI_AUTO_LENGTH, &resourceName);
    napi_create_async_work(env, nullptr, resourceName, NativeSendMessage, SendMessageCallback,
        (void *)asyncContext, &(asyncContext->work));
    napi_queue_async_work(env, asyncContext->work);
    return NapiUtil::CreateUndefined(env);
}

static void NativeCreateMessage(napi_env env, void *data)
{
    TELEPHONY_LOGI("NativeCreateMessage start");
    auto asyncContext = static_cast<CreateMessageContext *>(data);
    if (!asyncContext->specification.empty() && !asyncContext->pdu.empty()) {
        TELEPHONY_LOGI("NativeCreateMessage before CreateMessage");
        std::u16string specification16 = NapiUtil::ToUtf16(asyncContext->specification);
        asyncContext->shortMessage = ShortMessage::CreateMessage(asyncContext->pdu, specification16);
        if (asyncContext->shortMessage != nullptr) {
            TELEPHONY_LOGI("NativeCreateMessage CreateMessage success");
            asyncContext->resolved = true;
        } else {
            TELEPHONY_LOGI("NativeCreateMessage CreateMessage faied");
            asyncContext->resolved = false;
        }
    } else {
        asyncContext->resolved = false;
    }
    TELEPHONY_LOGI("NativeCreateMessage end");
}

static napi_value CreateShortMessageValue(napi_env env, const ShortMessage &shortMessage)
{
    napi_value object = nullptr;
    napi_create_object(env, &object);
    NapiUtil::SetPropertyStringUtf8(
        env, object, "visibleMessageBody", NapiUtil::ToUtf8(shortMessage.GetVisibleMessageBody()));
    NapiUtil::SetPropertyStringUtf8(
        env, object, "visibleRawAddress", NapiUtil::ToUtf8(shortMessage.GetVisibleRawAddress()));
    NapiUtil::SetPropertyInt32(env, object, "messageClass", shortMessage.GetMessageClass());
    NapiUtil::SetPropertyInt32(env, object, "protocolId", shortMessage.GetProtocolId());
    NapiUtil::SetPropertyStringUtf8(env, object, "scAddress", NapiUtil::ToUtf8(shortMessage.GetScAddress()));
    NapiUtil::SetPropertyInt32(env, object, "scTimestamp", shortMessage.GetScTimestamp());
    NapiUtil::SetPropertyBoolean(env, object, "isReplaceMessage", shortMessage.IsReplaceMessage());
    NapiUtil::SetPropertyBoolean(env, object, "hasReplyPath", shortMessage.HasReplyPath());
    SetPropertyArray(env, object, "pdu", shortMessage.GetPdu());
    NapiUtil::SetPropertyInt32(env, object, "status", shortMessage.GetStatus());
    NapiUtil::SetPropertyBoolean(env, object, "isSmsStatusReportMessage", shortMessage.IsSmsStatusReportMessage());
    return object;
}

static void CreateMessageCallback(napi_env env, napi_status status, void *data)
{
    auto asyncContext = static_cast<CreateMessageContext *>(data);
    napi_value callbackValue = nullptr;
    if (status == napi_ok) {
        if (asyncContext->resolved) {
            callbackValue = CreateShortMessageValue(env, *(asyncContext->shortMessage));
        } else {
            callbackValue = NapiUtil::CreateErrorMessage(env, "create message error");
        }
    } else {
        callbackValue = NapiUtil::CreateErrorMessage(
            env, "create message error,cause napi_status = " + std::to_string(status));
    }
    NapiUtil::Handle2ValueCallback(env, asyncContext, callbackValue);
}

static bool MatchCreateMessageParameter(napi_env env, const napi_value parameters[], size_t parameterCount)
{
    bool typeMatch = false;
    switch (parameterCount) {
        case 2: {
            typeMatch = NapiUtil::MatchParameters(env, parameters, {napi_object, napi_string});
            break;
        }
        case 3:
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

static napi_value CreateMessage(napi_env env, napi_callback_info info)
{
    size_t parameterCount = 3;
    napi_value parameters[3] = {0};
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, &parameterCount, parameters, &thisVar, &data);
    NAPI_ASSERT(env, MatchCreateMessageParameter(env, parameters, parameterCount), "type mismatch");
    auto asyncContext = std::make_unique<CreateMessageContext>().release();
    asyncContext->specification = Get64StringFromValue(env, parameters[1]);
    TELEPHONY_LOGI("CreateMessage specification = %s", asyncContext->specification.c_str());
    uint32_t arrayLength = 0;
    napi_get_array_length(env, parameters[0], &arrayLength);
    napi_value elementValue = nullptr;
    int32_t element = 0;
    for (uint32_t i = 0; i < arrayLength; i++) {
        napi_get_element(env, parameters[0], i, &elementValue);
        napi_get_value_int32(env, elementValue, &element);
        asyncContext->pdu.push_back((unsigned char)element);
    }
    TELEPHONY_LOGI("CreateMessage pdu size = %{private}zu", asyncContext->pdu.size());
    if (parameterCount == 3) {
        napi_create_reference(env, parameters[2], DEFAULT_REF_COUNT, &(asyncContext->callbackRef));
    }
    return NapiUtil ::HandleAsyncWork(
        env, asyncContext, "CreateMessage", NativeCreateMessage, CreateMessageCallback);
}

static bool MatchSetDefaultSmsSlotIdParameters(napi_env env, const napi_value parameters[], size_t parameterCount)
{
    switch (parameterCount) {
        case 1: {
            return NapiUtil::MatchParameters(env, parameters, {napi_number});
        }
        case 2:
            return NapiUtil::MatchParameters(env, parameters, {napi_number, napi_function});
        default: {
            return false;
        }
    }
}

static void NativeSetDefaultSmsSlotId(napi_env env, void *data)
{
    TELEPHONY_LOGI("NativeSetDefaultSmsSlotId start");
    auto context = static_cast<SetDefaultSmsSlotIdContext *>(data);
    context->resolved = ShortMessageManager::SetDefaultSmsSlotId(context->slotId);
    TELEPHONY_LOGI("NativeSetDefaultSmsSlotId end resolved = %{public}d", context->resolved);
}

static void SetDefaultSmsSlotIdCallback(napi_env env, napi_status status, void *data)
{
    auto context = static_cast<SetDefaultSmsSlotIdContext *>(data);
    TELEPHONY_LOGI("SetDefaultSmsSlotIdCallback status = %{public}d", status);
    napi_value callbackValue = nullptr;
    if (status == napi_ok) {
        if (context->resolved) {
            napi_get_undefined(env, &callbackValue);
        } else {
            callbackValue = NapiUtil::CreateErrorMessage(env, "set default sms slot id error");
        }
    } else {
        callbackValue = NapiUtil::CreateErrorMessage(
            env, "set default sms slot id error cause napi_status = " + std::to_string(status));
    }
    NapiUtil::Handle1ValueCallback(env, context, callbackValue);
}

static napi_value SetDefaultSmsSlotId(napi_env env, napi_callback_info info)
{
    TELEPHONY_LOGI("SetDefaultSmsSlotId start");
    size_t parameterCount = 2;
    napi_value parameters[2] = {0};
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, &parameterCount, parameters, &thisVar, &data);
    NAPI_ASSERT(env, MatchSetDefaultSmsSlotIdParameters(env, parameters, parameterCount), "type mismatch");
    auto context = std::make_unique<SetDefaultSmsSlotIdContext>().release();
    napi_get_value_int32(env, parameters[0], &context->slotId);
    if (parameterCount == 2) {
        napi_create_reference(env, parameters[1], DEFAULT_REF_COUNT, &context->callbackRef);
    }
    napi_value result = nullptr;
    if (context->callbackRef == nullptr) {
        napi_create_promise(env, &context->deferred, &result);
    } else {
        napi_get_undefined(env, &result);
    }
    napi_value resourceName = nullptr;
    napi_create_string_utf8(env, "SetDefaultSmsSlotId", NAPI_AUTO_LENGTH, &resourceName);
    napi_create_async_work(env, nullptr, resourceName, NativeSetDefaultSmsSlotId, SetDefaultSmsSlotIdCallback,
        (void *)context, &(context->work));
    napi_queue_async_work(env, context->work);
    return result;
}

static bool MatchGetDefaultSmsSlotIdParameters(napi_env env, const napi_value parameters[], size_t parameterCount)
{
    switch (parameterCount) {
        case 0: {
            return true;
        }
        case 1: {
            return NapiUtil::MatchParameters(env, parameters, {napi_function});
        }
        default: {
            return false;
        }
    }
}

static void NativeGetDefaultSmsSlotId(napi_env env, void *data)
{
    TELEPHONY_LOGI("NativeGetDefaultSmsSlotId start ");
    auto context = static_cast<GetDefaultSmsSlotIdContext *>(data);
    context->defaultSmsSlotId = ShortMessageManager::GetDefaultSmsSlotId();
    TELEPHONY_LOGI("NativeGetDefaultSmsSlotId defaultSmsSlotId  = %{public}d", context->defaultSmsSlotId);
    if (context->defaultSmsSlotId >= SIM_SLOT_0) {
        context->resolved = true;
    } else {
        context->resolved = false;
    }
}

static void GetDefaultSmsSlotIdCallback(napi_env env, napi_status status, void *data)
{
    TELEPHONY_LOGI("GetDefaultSmsSlotIdCallback start");
    auto context = static_cast<GetDefaultSmsSlotIdContext *>(data);
    napi_value callbackValue = nullptr;
    if (status == napi_ok) {
        if (context->resolved) {
            napi_create_int32(env, context->defaultSmsSlotId, &callbackValue);
        } else {
            callbackValue = NapiUtil::CreateErrorMessage(env, "get default sms slot id error");
        }
    } else {
        callbackValue = NapiUtil::CreateErrorMessage(
            env, "get default sms slot id error cause napi_status = " + std::to_string(status));
    }
    NapiUtil::Handle2ValueCallback(env, context, callbackValue);
}

static napi_value GetDefaultSmsSlotId(napi_env env, napi_callback_info info)
{
    TELEPHONY_LOGI("GetDefaultSmsSlotId start");
    size_t parameterCount = 1;
    napi_value parameters[1] = {0};
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, &parameterCount, parameters, &thisVar, &data);
    NAPI_ASSERT(env, MatchGetDefaultSmsSlotIdParameters(env, parameters, parameterCount), "type mismatch");
    auto context = std::make_unique<GetDefaultSmsSlotIdContext>().release();
    napi_status statusValue = napi_get_value_int32(env, parameters[0], &context->defaultSmsSlotId);
    TELEPHONY_LOGI("GetDefaultSmsSlotId statusValue = %{private}d", statusValue);
    if (parameterCount == 1) {
        napi_create_reference(env, parameters[0], DEFAULT_REF_COUNT, &context->callbackRef);
    }
    return NapiUtil::HandleAsyncWork(
        env, context, "SetDefaultSmsSlotId", NativeGetDefaultSmsSlotId, GetDefaultSmsSlotIdCallback);
}

static bool MatchSetSmscAddrParameters(napi_env env, const napi_value parameters[], size_t parameterCount)
{
    switch (parameterCount) {
        case 2: {
            return NapiUtil::MatchParameters(env, parameters, {napi_number, napi_string});
        }
        case 3: {
            return NapiUtil::MatchParameters(env, parameters, {napi_number, napi_string, napi_function});
        }
        default: {
            return false;
        }
    }
}

static void NativeSetSmscAddr(napi_env env, void *data)
{
    TELEPHONY_LOGI("NativeSetSmscAddr start ");
    auto context = static_cast<SetSmscAddrContext *>(data);
    context->resolved = ShortMessageManager::SetScAddress(context->slotId, NapiUtil::ToUtf16(context->smscAddr));
    TELEPHONY_LOGI("NativeSetSmscAddr resolved = %{private}d", context->resolved);
}

static void SetSmscAddrCallback(napi_env env, napi_status status, void *data)
{
    auto context = static_cast<SetSmscAddrContext *>(data);
    napi_value callbackValue = nullptr;
    if (status == napi_ok) {
        if (context->resolved) {
            napi_get_undefined(env, &callbackValue);
        } else {
            callbackValue = NapiUtil::CreateErrorMessage(env, "set smsc addr error by ipc");
        }
    } else {
        callbackValue =
            NapiUtil::CreateErrorMessage(env, "set smsc addr error napi_status = " + std::to_string(status));
    }
    NapiUtil::Handle1ValueCallback(env, context, callbackValue);
}

static napi_value SetSmscAddr(napi_env env, napi_callback_info info)
{
    TELEPHONY_LOGI("SetSmscAddr start");
    size_t parameterCount = 3;
    napi_value parameters[3] = {0};
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, &parameterCount, parameters, &thisVar, &data);
    NAPI_ASSERT(env, MatchSetSmscAddrParameters(env, parameters, parameterCount), "type mismatch");
    TELEPHONY_LOGI("SetSmscAddr start after MatchSetSmscAddrParameters");
    auto context = std::make_unique<SetSmscAddrContext>().release();
    TELEPHONY_LOGI("SetSmscAddr start after SetSmscAddrContext contruct");
    napi_get_value_int32(env, parameters[0], &context->slotId);
    context->smscAddr = Get64StringFromValue(env, parameters[1]);
    TELEPHONY_LOGI("SetSmscAddr smscAddr = %{publc}s", context->smscAddr.data());
    if (parameterCount == 3) {
        napi_create_reference(env, parameters[2], DEFAULT_REF_COUNT, &context->callbackRef);
    }
    TELEPHONY_LOGI("SetSmscAddr before end");
    return NapiUtil::HandleAsyncWork(env, context, "SetSmscAddr", NativeSetSmscAddr, SetSmscAddrCallback);
}

static bool MatchGetSmscAddrParameters(napi_env env, const napi_value parameters[], size_t parameterCount)
{
    switch (parameterCount) {
        case 1: {
            return NapiUtil::MatchParameters(env, parameters, {napi_number});
        }
        case 2: {
            return NapiUtil::MatchParameters(env, parameters, {napi_number, napi_function});
        }
        default: {
            return false;
        }
    }
}

static void NativeGetSmscAddr(napi_env env, void *data)
{
    TELEPHONY_LOGI("NativeGetSmscAddr start ");
    auto context = static_cast<GetSmscAddrContext *>(data);
    context->smscAddr = NapiUtil::ToUtf8(ShortMessageManager::GetScAddress(context->slotId));
    context->resolved = true;
    TELEPHONY_LOGI("NativeGetSmscAddr smscAddr = %{private}s", context->smscAddr.data());
}

static void GetSmscAddrCallback(napi_env env, napi_status status, void *data)
{
    TELEPHONY_LOGI("GetSmscAddrCallback start ");
    auto context = static_cast<GetSmscAddrContext *>(data);
    napi_value callbackValue = nullptr;
    if (status == napi_ok) {
        if (context->resolved) {
            napi_create_string_utf8(env, context->smscAddr.data(), context->smscAddr.length(), &callbackValue);
        } else {
            callbackValue = NapiUtil::CreateErrorMessage(env, "get smsc addr error by ipc");
        }
    } else {
        callbackValue =
            NapiUtil::CreateErrorMessage(env, "get smsc addr error,napi_status = " + std ::to_string(status));
    }
    NapiUtil::Handle2ValueCallback(env, context, callbackValue);
}

static napi_value GetSmscAddr(napi_env env, napi_callback_info info)
{
    TELEPHONY_LOGI("GetSmscAddr start ");
    size_t parameterCount = 2;
    napi_value parameters[2] = {0};
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, &parameterCount, parameters, &thisVar, &data);
    NAPI_ASSERT(env, MatchGetSmscAddrParameters(env, parameters, parameterCount), "type mismatch");
    auto context = std::make_unique<GetSmscAddrContext>().release();
    napi_get_value_int32(env, parameters[0], &context->slotId);
    if (parameterCount == 2) {
        napi_create_reference(env, parameters[1], DEFAULT_REF_COUNT, &context->callbackRef);
    }
    return NapiUtil::HandleAsyncWork(env, context, "GetSmscAddr", NativeGetSmscAddr, GetSmscAddrCallback);
}

static bool MatchAddSimMessageParameters(napi_env env, const napi_value parameters[], size_t parameterCount)
{
    bool typeMatch = false;
    switch (parameterCount) {
        case 1: {
            typeMatch = NapiUtil::MatchParameters(env, parameters, {napi_object});
            break;
        }
        case 2: {
            typeMatch = NapiUtil::MatchParameters(env, parameters, {napi_object, napi_function});
            break;
        }
        default: {
            break;
        }
    }
    if (typeMatch) {
        return MatchObjectProperty(env, parameters[0],
            {
                {"slotId", napi_number},
                {"smsc", napi_string},
                {"status", napi_number},
                {"pdu", napi_string},
            });
    }
    return false;
}

static void NativeAddSimMessage(napi_env env, void *data)
{
    TELEPHONY_LOGI("NativeAddSimMessage start");
    auto context = static_cast<AddSimMessageContext *>(data);
    int32_t wrapStatus = static_cast<int32_t>(context->status);
    TELEPHONY_LOGI("NativeAddSimMessage start wrapStatus = %{public}d", wrapStatus);
    if (wrapStatus != MESSAGE_UNKNOWN_STATUS) {
        ISmsServiceInterface::SimMessageStatus status =
            static_cast<ISmsServiceInterface::SimMessageStatus>(wrapStatus);
        context->resolved = ShortMessageManager::AddSimMessage(
            context->slotId, NapiUtil::ToUtf16(context->smsc), NapiUtil::ToUtf16(context->pdu), status);
        TELEPHONY_LOGI("NativeAddSimMessage context->resolved = %{private}d", context->resolved);
    } else {
        context->resolved = false;
    }
}

static void AddSimMessageCallback(napi_env env, napi_status status, void *data)
{
    TELEPHONY_LOGI("AddSimMessageCallback start");
    auto context = static_cast<AddSimMessageContext *>(data);
    napi_value callbackValue = nullptr;
    if (status == napi_ok) {
        if (context->resolved) {
            napi_get_undefined(env, &callbackValue);
        } else {
            callbackValue = NapiUtil::CreateErrorMessage(env, "add sim message error by ipc");
        }
    } else {
        callbackValue =
            NapiUtil::CreateErrorMessage(env, "add sim message error,napi_status = " + std ::to_string(status));
    }
    NapiUtil::Handle1ValueCallback(env, context, callbackValue);
}

static napi_value AddSimMessage(napi_env env, napi_callback_info info)
{
    TELEPHONY_LOGI("AddSimMessage start");
    size_t parameterCount = 2;
    napi_value parameters[2] = {0};
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, &parameterCount, parameters, &thisVar, &data);
    NAPI_ASSERT(env, MatchAddSimMessageParameters(env, parameters, parameterCount), "type mismatch");
    auto context = std::make_unique<AddSimMessageContext>().release();
    napi_value slotIdValue = NapiUtil::GetNamedProperty(env, parameters[0], "slotId");
    if (slotIdValue != nullptr) {
        napi_get_value_int32(env, slotIdValue, &context->slotId);
    }
    napi_value smscValue = NapiUtil::GetNamedProperty(env, parameters[0], "smsc");
    if (smscValue != nullptr) {
        context->smsc = Get64StringFromValue(env, smscValue);
    }
    napi_value pduValue = NapiUtil::GetNamedProperty(env, parameters[0], "pdu");
    if (pduValue != nullptr) {
        context->pdu = NapiUtil::GetStringFromValue(env, pduValue);
    }
    napi_value statusValue = NapiUtil::GetNamedProperty(env, parameters[0], "status");
    if (statusValue != nullptr) {
        int32_t messageStatus = static_cast<int32_t>(MESSAGE_UNKNOWN_STATUS);
        napi_get_value_int32(env, statusValue, &messageStatus);
        context->status = WrapSimMessageStatus(messageStatus);
    }
    if (parameterCount == 2) {
        napi_create_reference(env, parameters[1], DEFAULT_REF_COUNT, &context->callbackRef);
    }
    return NapiUtil::HandleAsyncWork(env, context, "AddSimMessage", NativeAddSimMessage, AddSimMessageCallback);
}

static bool MatchDelSimMessageParameters(napi_env env, const napi_value parameters[], size_t parameterCount)
{
    switch (parameterCount) {
        case 2: {
            return NapiUtil::MatchParameters(env, parameters, {napi_number, napi_number});
        }
        case 3: {
            return NapiUtil::MatchParameters(env, parameters, {napi_number, napi_number, napi_function});
        }
        default: {
            return false;
        }
    }
}
static void NativeDelSimMessage(napi_env env, void *data)
{
    auto context = static_cast<DelSimMessageContext *>(data);
    context->resolved = ShortMessageManager::DelSimMessage(context->slotId, context->msgIndex);
}
static void DelSimMessageCallback(napi_env env, napi_status status, void *data)
{
    auto context = static_cast<DelSimMessageContext *>(data);
    napi_value callbackValue = nullptr;
    if (status == napi_ok) {
        if (context->resolved) {
            napi_get_undefined(env, &callbackValue);
        } else {
            callbackValue = NapiUtil::CreateErrorMessage(env, "delete sim message error by ipc");
        }
    } else {
        callbackValue =
            NapiUtil::CreateErrorMessage(env, "delete sim message error,napi_status = " + std ::to_string(status));
    }
    NapiUtil::Handle1ValueCallback(env, context, callbackValue);
}

static napi_value DelSimMessage(napi_env env, napi_callback_info info)
{
    size_t parameterCount = 3;
    napi_value parameters[3] = {0};
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, &parameterCount, parameters, &thisVar, &data);
    NAPI_ASSERT(env, MatchDelSimMessageParameters(env, parameters, parameterCount), "type mismatch");
    auto context = std::make_unique<DelSimMessageContext>().release();
    napi_get_value_int32(env, parameters[0], &context->slotId);
    napi_get_value_int32(env, parameters[1], &context->msgIndex);
    if (parameterCount == 3) {
        napi_create_reference(env, parameters[2], DEFAULT_REF_COUNT, &context->callbackRef);
    }
    return NapiUtil::HandleAsyncWork(env, context, "DelSimMessage", NativeDelSimMessage, DelSimMessageCallback);
}

static bool MatchUpdateSimMessageParameters(napi_env env, const napi_value parameters[], size_t parameterCount)
{
    TELEPHONY_LOGI("MatchUpdateSimMessageParameters start");
    bool typeMatch = false;
    switch (parameterCount) {
        case 1: {
            typeMatch = NapiUtil::MatchParameters(env, parameters, {napi_object});
            break;
        }
        case 2: {
            typeMatch = NapiUtil::MatchParameters(env, parameters, {napi_object, napi_function});
            break;
        }
        default: {
            break;
        }
    }
    if (typeMatch) {
        bool propertyMatchResult = MatchObjectProperty(env, parameters[0],
            {
                {"slotId", napi_number},
                {"msgIndex", napi_number},
                {"newStatus", napi_number},
                {"pdu", napi_string},
                {"smsc", napi_string},
            });
        TELEPHONY_LOGI(
            "MatchUpdateSimMessageParameters start propertyMatchResult = %{public}d", propertyMatchResult);
        return propertyMatchResult;
    }
    TELEPHONY_LOGI("MatchUpdateSimMessageParameters end");
    return false;
}

static void NativeUpdateSimMessage(napi_env env, void *data)
{
    auto context = static_cast<UpdateSimMessageContext *>(data);
    int32_t newStatus = static_cast<int32_t>(context->newStatus);
    TELEPHONY_LOGI("NativeUpdateSimMessage newStatus = %{public}d", newStatus);
    if (!context->pdu.empty() && (newStatus > -1)) {
        std::string msgPud(context->pdu.begin(), context->pdu.end());
        TELEPHONY_LOGI("NativeUpdateSimMessage msgPud = %{public}s", msgPud.c_str());
        context->resolved = ShortMessageManager::UpdateSimMessage(context->slotId, context->msgIndex,
            static_cast<ISmsServiceInterface::SimMessageStatus>(context->newStatus),
            NapiUtil::ToUtf16(context->pdu), NapiUtil::ToUtf16(context->smsc));
    } else {
        TELEPHONY_LOGI("NativeUpdateSimMessage resolved false cause parameter invalided");
        context->resolved = false;
    }
}

static void UpdateSimMessageCallback(napi_env env, napi_status status, void *data)
{
    auto context = static_cast<UpdateSimMessageContext *>(data);
    napi_value callbackValue = nullptr;
    if (status == napi_ok) {
        if (context->resolved) {
            napi_get_undefined(env, &callbackValue);
        } else {
            callbackValue = NapiUtil::CreateErrorMessage(env, "update sim message failed");
        }
    } else {
        callbackValue =
            NapiUtil::CreateErrorMessage(env, "update sim message error,napi_status = " + std ::to_string(status));
    }
    NapiUtil::Handle1ValueCallback(env, context, callbackValue);
}

static napi_value UpdateSimMessage(napi_env env, napi_callback_info info)
{
    TELEPHONY_LOGI("UpdateSimMessage start");
    size_t parameterCount = 2;
    napi_value parameters[2] = {0};
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, &parameterCount, parameters, &thisVar, &data);
    NAPI_ASSERT(env, MatchUpdateSimMessageParameters(env, parameters, parameterCount), "type mismatch");
    TELEPHONY_LOGI("UpdateSimMessage start parameter match passed");
    auto context = std::make_unique<UpdateSimMessageContext>().release();
    napi_value slotIdValue = NapiUtil::GetNamedProperty(env, parameters[0], "slotId");
    if (slotIdValue != nullptr) {
        napi_get_value_int32(env, slotIdValue, &context->slotId);
    }
    napi_value msgIndexValue = NapiUtil::GetNamedProperty(env, parameters[0], "msgIndex");
    if (msgIndexValue != nullptr) {
        napi_get_value_int32(env, msgIndexValue, &context->msgIndex);
    }
    napi_value newStatusValue = NapiUtil::GetNamedProperty(env, parameters[0], "newStatus");
    if (newStatusValue != nullptr) {
        int32_t newStatus = static_cast<int32_t>(MESSAGE_UNKNOWN_STATUS);
        napi_get_value_int32(env, newStatusValue, &newStatus);
        context->newStatus = WrapSimMessageStatus(newStatus);
    }
    napi_value pudValue = NapiUtil::GetNamedProperty(env, parameters[0], "pdu");
    if (pudValue != nullptr) {
        context->pdu = NapiUtil::GetStringFromValue(env, pudValue);
    }
    TELEPHONY_LOGI("UpdateSimMessage pdu = %{public}s", context->pdu.c_str());
    napi_value smscValue = NapiUtil::GetNamedProperty(env, parameters[0], "smsc");
    if (smscValue != nullptr) {
        context->smsc = Get64StringFromValue(env, smscValue);
    }
    if (parameterCount == 2) {
        napi_create_reference(env, parameters[1], DEFAULT_REF_COUNT, &context->callbackRef);
    }
    TELEPHONY_LOGI("UpdateSimMessage start before HandleAsyncWork");
    return NapiUtil::HandleAsyncWork(
        env, context, "UpdateSimMessage", NativeUpdateSimMessage, UpdateSimMessageCallback);
}

static bool MatchGetAllSimMessagesParameters(napi_env env, const napi_value parameters[], size_t parameterCount)
{
    switch (parameterCount) {
        case 1: {
            return NapiUtil::MatchParameters(env, parameters, {napi_number});
        }
        case 2: {
            return NapiUtil::MatchParameters(env, parameters, {napi_number, napi_function});
        }
        default: {
            return false;
        }
    }
}

static void NativeGetAllSimMessages(napi_env env, void *data)
{
    auto context = static_cast<GetAllSimMessagesContext *>(data);
    context->messageArray = ShortMessageManager::GetAllSimMessages(context->slotId);
    context->resolved = true;
}

static napi_value CreateSimShortMessageValue(napi_env env, const ShortMessage &shortMessage)
{
    napi_value simObject = nullptr;
    napi_value object = CreateShortMessageValue(env, shortMessage);
    napi_create_object(env, &simObject);
    std::string shortMessageKey("shortMessage");
    napi_set_named_property(env, simObject, shortMessageKey.c_str(), object);
    NapiUtil::SetPropertyInt32(env, simObject, "simMessageStatus", shortMessage.GetIccMessageStatus());
    NapiUtil::SetPropertyInt32(env, simObject, "indexOnSim", shortMessage.GetIndexOnSim());
    return simObject;
}

static void GetAllSimMessagesCallback(napi_env env, napi_status status, void *data)
{
    auto context = static_cast<GetAllSimMessagesContext *>(data);
    napi_value callbackValue = nullptr;
    if (status == napi_ok) {
        if (context->resolved) {
            napi_create_array(env, &callbackValue);
            int32_t arraySize = static_cast<int32_t>(context->messageArray.size());
            for (int32_t i = 0; i < arraySize; i++) {
                ShortMessage message = context->messageArray[i];
                napi_value itemValue = CreateSimShortMessageValue(env, message);
                napi_set_element(env, callbackValue, i, itemValue);
            }
        } else {
            callbackValue = NapiUtil::CreateErrorMessage(env, "get all sim message error by ipc");
        }
    } else {
        callbackValue = NapiUtil::CreateErrorMessage(
            env, "get all sim message error,napi_status = " + std ::to_string(status));
    }
    NapiUtil::Handle2ValueCallback(env, context, callbackValue);
}

static napi_value GetAllSimMessages(napi_env env, napi_callback_info info)
{
    size_t parameterCount = 2;
    napi_value parameters[2] = {0};
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, &parameterCount, parameters, &thisVar, &data);
    NAPI_ASSERT(env, MatchGetAllSimMessagesParameters(env, parameters, parameterCount), "type mismatch");
    auto context = std::make_unique<GetAllSimMessagesContext>().release();
    napi_get_value_int32(env, parameters[0], &context->slotId);
    if (parameterCount == 2) {
        napi_create_reference(env, parameters[1], DEFAULT_REF_COUNT, &context->callbackRef);
    }
    napi_value result = NapiUtil::HandleAsyncWork(
        env, context, "GetAllSimMessages", NativeGetAllSimMessages, GetAllSimMessagesCallback);
    return result;
}

static bool MatchSetCBConfigParameters(napi_env env, const napi_value parameters[], size_t parameterCount)
{
    bool typeMatch = false;
    switch (parameterCount) {
        case 1: {
            typeMatch = NapiUtil::MatchParameters(env, parameters, {napi_object});
            break;
        }
        case 2: {
            typeMatch = NapiUtil::MatchParameters(env, parameters, {napi_object, napi_function});
            break;
        }
        default: {
            break;
        }
    }
    if (typeMatch) {
        return MatchObjectProperty(env, parameters[0],
            {
                {"slotId", napi_number},
                {"enable", napi_boolean},
                {"startMessageId", napi_number},
                {"endMessageId", napi_number},
                {"ranType", napi_number},
            });
    }
    return false;
}

static void NativeSetCBConfig(napi_env env, void *data)
{
    TELEPHONY_LOGI("NativeSetCBConfig start！");
    auto context = static_cast<CBConfigContext *>(data);
    context->resolved = ShortMessageManager::SetCBConfig(
        context->slotId, context->enable, context->startMessageId, context->endMessageId, context->ranType);
    TELEPHONY_LOGI("NativeSetCBConfig end resolved = %{public}d", context->resolved);
}

static void SetCBConfigCallback(napi_env env, napi_status status, void *data)
{
    auto context = static_cast<CBConfigContext *>(data);
    napi_value callbackValue = nullptr;
    if (status == napi_ok) {
        if (context->resolved) {
            napi_get_undefined(env, &callbackValue);
        } else {
            callbackValue = NapiUtil::CreateErrorMessage(env, "set cb range config error by ipc");
        }
    } else {
        callbackValue = NapiUtil::CreateErrorMessage(
            env, "set cb range config  error,napi_status = " + std ::to_string(status));
    }
    NapiUtil::Handle1ValueCallback(env, context, callbackValue);
}

static napi_value SetCBConfig(napi_env env, napi_callback_info info)
{
    TELEPHONY_LOGI("napi_sms SetCBConfig start！");
    size_t parameterCount = 2;
    napi_value parameters[2] = {0};
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, &parameterCount, parameters, &thisVar, &data);
    NAPI_ASSERT(env, MatchSetCBConfigParameters(env, parameters, parameterCount), "type mismatch");
    auto context = std::make_unique<CBConfigContext>().release();
    napi_value slotIdValue = NapiUtil::GetNamedProperty(env, parameters[0], "slotId");
    if (slotIdValue != nullptr) {
        napi_get_value_int32(env, slotIdValue, &context->slotId);
    }
    napi_value enableValue = NapiUtil::GetNamedProperty(env, parameters[0], "enable");
    if (enableValue != nullptr) {
        napi_get_value_bool(env, enableValue, &context->enable);
    }
    napi_value startMessageIdValue = NapiUtil::GetNamedProperty(env, parameters[0], "startMessageId");
    if (startMessageIdValue != nullptr) {
        napi_get_value_int32(env, startMessageIdValue, &context->startMessageId);
    }
    napi_value endMessageIdValue = NapiUtil::GetNamedProperty(env, parameters[0], "endMessageId");
    if (endMessageIdValue != nullptr) {
        napi_get_value_int32(env, endMessageIdValue, &context->endMessageId);
    }
    napi_value ranTypeValue = NapiUtil::GetNamedProperty(env, parameters[0], "ranType");
    if (ranTypeValue != nullptr) {
        napi_get_value_int32(env, ranTypeValue, &context->ranType);
    }
    if (parameterCount == 2) {
        napi_create_reference(env, parameters[1], DEFAULT_REF_COUNT, &context->callbackRef);
    }
    napi_value result =
        NapiUtil::HandleAsyncWork(env, context, "SetCBConfig", NativeSetCBConfig, SetCBConfigCallback);
    return result;
}

static bool MatchSplitMessageParameters(napi_env env, const napi_value parameters[], size_t parameterCount)
{
    switch (parameterCount) {
        case 1:
            return NapiUtil::MatchParameters(env, parameters, {napi_string});
        case 2:
            return NapiUtil::MatchParameters(env, parameters, {napi_string, napi_function});
        default:
            return false;
    }
}

static void NativeSplitMessage(napi_env env, void *data)
{
    TELEPHONY_LOGI("NativeSplitMessage start！");
    auto context = static_cast<SplitMessageContext *>(data);
    std::u16string content = NapiUtil::ToUtf16(context->content);
    context->messageArray = ShortMessageManager::SplitMessage(content);
    context->resolved = true;
}

static void SplitMessageCallback(napi_env env, napi_status status, void *data)
{
    auto context = static_cast<SplitMessageContext *>(data);
    napi_value callbackValue = nullptr;
    if (status == napi_ok) {
        if (context->resolved) {
            napi_create_array(env, &callbackValue);
            int32_t arraySize = static_cast<int32_t>(context->messageArray.size());
            TELEPHONY_LOGI("napi_sms messageArray.size =  %{public}d", arraySize);
            for (int32_t i = 0; i < arraySize; i++) {
                napi_value itemValue = nullptr;
                std::string message = NapiUtil::ToUtf8(context->messageArray[i]);
                napi_create_string_utf8(env, message.data(), message.size(), &itemValue);
                napi_set_element(env, callbackValue, i, itemValue);
            }
        } else {
            callbackValue = NapiUtil::CreateErrorMessage(env, "split Message error by ipc");
        }
    } else {
        callbackValue =
            NapiUtil::CreateErrorMessage(env, "split Message  error,napi_status = " + std ::to_string(status));
    }
    NapiUtil::Handle2ValueCallback(env, context, callbackValue);
}

static napi_value SplitMessage(napi_env env, napi_callback_info info)
{
    TELEPHONY_LOGI("napi_sms splitMessage start！");
    size_t parameterCount = 2;
    napi_value parameters[2] = {0};
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, &parameterCount, parameters, &thisVar, &data);
    NAPI_ASSERT(env, MatchSplitMessageParameters(env, parameters, parameterCount), "type mismatch");
    auto context = std::make_unique<SplitMessageContext>().release();
    TELEPHONY_LOGI("napi_sms splitMessage start！");
    context->content = Get64StringFromValue(env, parameters[0]);
    TELEPHONY_LOGI("napi_sms splitMessage context->content = %{public}s", context->content.c_str());
    if (parameterCount == 2) {
        napi_create_reference(env, parameters[1], MAX_TEXT_SHORT_MESSAGE_LENGTH, &context->callbackRef);
    }
    napi_value result =
        NapiUtil::HandleAsyncWork(env, context, "SplitMessage", NativeSplitMessage, SplitMessageCallback);
    return result;
}

static napi_value HasSmsCapability(napi_env env, napi_callback_info info)
{
    TELEPHONY_LOGI("napi_sms HasSmsCapability start!");
    napi_value result = nullptr;
    napi_get_boolean(env, ShortMessageManager::HasSmsCapability(), &result);
    return result;
}

static bool MatchGetSmsSegmentsInfoParameters(napi_env env, const napi_value parameters[], size_t parameterCount)
{
    switch (parameterCount) {
        case 3:
            return NapiUtil::MatchParameters(env, parameters, {napi_number, napi_string, napi_boolean});
        case 4:
            return NapiUtil::MatchParameters(
                env, parameters, {napi_number, napi_string, napi_boolean, napi_function});
        default:
            return false;
    }
}

static void NativeGetSmsSegmentsInfo(napi_env env, void *data)
{
    TELEPHONY_LOGI("NativeGetSmsSegmentsInfo start");
    auto context = static_cast<GetSmsSegmentsInfoContext *>(data);
    std::u16string content = NapiUtil::ToUtf16(context->content);
    ISmsServiceInterface::SmsSegmentsInfo info;
    context->resolved =
        ShortMessageManager::GetSmsSegmentsInfo(context->slotId, content, context->force7BitCode, info);
    if (context->resolved) {
        context->splitCount = info.msgSegCount;
        context->encodeCount = info.msgEncodingCount;
        context->encodeCountRemaining = info.msgRemainCount;
        context->scheme = info.msgCodeScheme;
    } else {
        TELEPHONY_LOGE("context->resolved == false");
    }
}

static void GetSmsSegmentsInfoCallback(napi_env env, napi_status status, void *data)
{
    TELEPHONY_LOGI("GetSmsSegmentsInfoCallback start");
    auto context = static_cast<GetSmsSegmentsInfoContext *>(data);
    napi_value callbackValue = nullptr;
    if (status == napi_ok) {
        if (context->resolved) {
            napi_create_object(env, &callbackValue);
            NapiUtil::SetPropertyInt32(env, callbackValue, "splitCount", context->splitCount);
            NapiUtil::SetPropertyInt32(env, callbackValue, "encodeCount", context->encodeCount);
            NapiUtil::SetPropertyInt32(env, callbackValue, "encodeCountRemaining", context->encodeCountRemaining);
            NapiUtil::SetPropertyInt32(env, callbackValue, "scheme", static_cast<int32_t>(context->scheme));
        } else {
            callbackValue = NapiUtil::CreateErrorMessage(env, "calculate length error by ipc");
        }
    } else {
        callbackValue =
            NapiUtil::CreateErrorMessage(env, "calculate length  error,napi_status = " + std ::to_string(status));
    }
    NapiUtil::Handle2ValueCallback(env, context, callbackValue);
}

static napi_value GetSmsSegmentsInfo(napi_env env, napi_callback_info info)
{
    TELEPHONY_LOGI("GetSmsSegmentsInfo start");
    size_t parameterCount = 4;
    napi_value parameters[4] = {0};
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, &parameterCount, parameters, &thisVar, &data);
    NAPI_ASSERT(env, MatchGetSmsSegmentsInfoParameters(env, parameters, parameterCount), "type mismatch");
    auto context = std::make_unique<GetSmsSegmentsInfoContext>().release();
    napi_get_value_int32(env, parameters[0], &context->slotId);
    context->content = NapiUtil::GetStringFromValue(env, parameters[1]);
    napi_get_value_bool(env, parameters[2], &context->force7BitCode);
    if (parameterCount == 4) {
        napi_create_reference(env, parameters[3], DEFAULT_REF_COUNT, &context->callbackRef);
    }
    napi_value result = NapiUtil::HandleAsyncWork(
        env, context, "GetSmsSegmentsInfo", NativeGetSmsSegmentsInfo, GetSmsSegmentsInfoCallback);
    return result;
}

static bool MatchIsImsSmsSupportedParameters(napi_env env, const napi_value parameters[], size_t parameterCount)
{
    if (parameterCount > 1) {
        return false;
    }
    if (parameterCount == 1) {
        return NapiUtil::MatchParameters(env, parameters, {napi_function});
    }
    return true;
}

static void NativeIsImsSmsSupported(napi_env env, void *data)
{
    auto context = static_cast<SingleValueContext<bool> *>(data);
    context->value = ShortMessageManager::IsImsSmsSupported();
    context->resolved = true;
}

static void IsImsSmsSupportedCallback(napi_env env, napi_status status, void *data)
{
    auto context = static_cast<SingleValueContext<bool> *>(data);
    napi_value callbackValue = nullptr;
    if (status == napi_ok) {
        if (context->resolved) {
            napi_get_boolean(env, context->value, &callbackValue);
        } else {
            callbackValue = NapiUtil::CreateErrorMessage(env, "IsImsSmsSupported error by ipc");
        }
    } else {
        callbackValue =
            NapiUtil::CreateErrorMessage(env, "IsImsSmsSupported error,napi_status = " + std ::to_string(status));
    }
    NapiUtil::Handle2ValueCallback(env, context, callbackValue);
}

static napi_value IsImsSmsSupported(napi_env env, napi_callback_info info)
{
    TELEPHONY_LOGI("napi_sms IsImsSmsSupported method start");
    size_t paramsCount = 1;
    napi_value params[1] = {0};
    napi_value arg = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, &paramsCount, params, &arg, &data);
    NAPI_ASSERT(env, MatchIsImsSmsSupportedParameters(env, params, paramsCount), "IsImsSmsSupported type mismatch");
    auto context = std::make_unique<SingleValueContext<bool>>().release();
    if (paramsCount == 1) {
        napi_create_reference(env, params[0], DEFAULT_REF_COUNT, &context->callbackRef);
    }
    napi_value result = NapiUtil::HandleAsyncWork(
        env, context, "IsImsSmsSupported", NativeIsImsSmsSupported, IsImsSmsSupportedCallback);
    return result;
}

static void NativeGetImsShortMessageFormat(napi_env env, void *data)
{
    auto context = static_cast<SingleValueContext<std::u16string> *>(data);
    context->value = ShortMessageManager::GetImsShortMessageFormat();
    context->resolved = true;
}

static std::string to_utf8(std::u16string str16)
{
    return std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> {}.to_bytes(str16);
}

static void GetImsShortMessageFormatCallback(napi_env env, napi_status status, void *data)
{
    auto context = static_cast<SingleValueContext<std::u16string> *>(data);
    napi_value callbackValue = nullptr;
    if (status == napi_ok) {
        if (context->resolved) {
            std::string toUtf8Value = to_utf8(context->value);
            napi_create_string_utf8(env, toUtf8Value.c_str(), toUtf8Value.size(), &callbackValue);
        } else {
            callbackValue = NapiUtil::CreateErrorMessage(env, "GetImsShortMessageFormat error by ipc");
        }
    } else {
        callbackValue = NapiUtil::CreateErrorMessage(
            env, "GetImsShortMessageFormat error,napi_status = " + std ::to_string(status));
    }
    NapiUtil::Handle2ValueCallback(env, context, callbackValue);
}

static napi_value GetImsShortMessageFormat(napi_env env, napi_callback_info info)
{
    TELEPHONY_LOGI("napi_sms GetImsShortMessageFormat method start");
    size_t paramsCount = 1;
    napi_value params[1] = {0};
    napi_value arg = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, &paramsCount, params, &arg, &data);
    NAPI_ASSERT(
        env, MatchIsImsSmsSupportedParameters(env, params, paramsCount), "GetImsShortMessageFormat type mismatch");
    auto context = std::make_unique<SingleValueContext<std::u16string>>().release();
    if (paramsCount == 1) {
        napi_create_reference(env, params[0], DEFAULT_REF_COUNT, &context->callbackRef);
    }
    napi_value result = NapiUtil::HandleAsyncWork(env, context, "GetImsShortMessageFormat",
        NativeGetImsShortMessageFormat, GetImsShortMessageFormatCallback);
    return result;
}

static napi_value InitEnumSendSmsResult(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_STATIC_PROPERTY(
            "SEND_SMS_SUCCESS", NapiUtil::ToInt32Value(env, static_cast<int32_t>(SEND_SMS_SUCCESS))),
        DECLARE_NAPI_STATIC_PROPERTY("SEND_SMS_FAILURE_UNKNOWN",
            NapiUtil::ToInt32Value(env, static_cast<int32_t>(SEND_SMS_FAILURE_UNKNOWN))),
        DECLARE_NAPI_STATIC_PROPERTY("SEND_SMS_FAILURE_RADIO_OFF",
            NapiUtil::ToInt32Value(env, static_cast<int32_t>(SEND_SMS_FAILURE_RADIO_OFF))),
        DECLARE_NAPI_STATIC_PROPERTY("SEND_SMS_FAILURE_SERVICE_UNAVAILABLE",
            NapiUtil::ToInt32Value(env, static_cast<int32_t>(SEND_SMS_FAILURE_SERVICE_UNAVAILABLE))),
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc));
    return exports;
}

static napi_value InitEnumShortMessageClass(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_STATIC_PROPERTY(
            "UNKNOWN", NapiUtil::ToInt32Value(env, static_cast<int32_t>(ShortMessageClass::UNKNOWN))),
        DECLARE_NAPI_STATIC_PROPERTY("INSTANT_MESSAGE",
            NapiUtil::ToInt32Value(env, static_cast<int32_t>(ShortMessageClass::INSTANT_MESSAGE))),
        DECLARE_NAPI_STATIC_PROPERTY("OPTIONAL_MESSAGE",
            NapiUtil::ToInt32Value(env, static_cast<int32_t>(ShortMessageClass::OPTIONAL_MESSAGE))),
        DECLARE_NAPI_STATIC_PROPERTY(
            "SIM_MESSAGE", NapiUtil::ToInt32Value(env, static_cast<int32_t>(ShortMessageClass::SIM_MESSAGE))),
        DECLARE_NAPI_STATIC_PROPERTY("FORWARD_MESSAGE",
            NapiUtil::ToInt32Value(env, static_cast<int32_t>(ShortMessageClass::FORWARD_MESSAGE))),
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc));
    return exports;
}

static napi_value InitEnumMessageStatusClass(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_STATIC_PROPERTY("SIM_MESSAGE_STATUS_FREE",
            NapiUtil::ToInt32Value(
                env, static_cast<int32_t>(ShortMessage::SmsSimMessageStatus::SMS_SIM_MESSAGE_STATUS_FREE))),
        DECLARE_NAPI_STATIC_PROPERTY("SIM_MESSAGE_STATUS_READ",
            NapiUtil::ToInt32Value(
                env, static_cast<int32_t>(ShortMessage::SmsSimMessageStatus::SMS_SIM_MESSAGE_STATUS_READ))),
        DECLARE_NAPI_STATIC_PROPERTY("SIM_MESSAGE_STATUS_UNREAD",
            NapiUtil::ToInt32Value(
                env, static_cast<int32_t>(ShortMessage::SmsSimMessageStatus::SMS_SIM_MESSAGE_STATUS_UNREAD))),
        DECLARE_NAPI_STATIC_PROPERTY("SIM_MESSAGE_STATUS_SENT",
            NapiUtil::ToInt32Value(
                env, static_cast<int32_t>(ShortMessage::SmsSimMessageStatus::SMS_SIM_MESSAGE_STATUS_SENT))),
        DECLARE_NAPI_STATIC_PROPERTY("SIM_MESSAGE_STATUS_UNSENT",
            NapiUtil::ToInt32Value(
                env, static_cast<int32_t>(ShortMessage::SmsSimMessageStatus::SMS_SIM_MESSAGE_STATUS_UNSENT))),
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc));
    return exports;
}

static napi_value InitEnumRanType(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_STATIC_PROPERTY(
            "TYPE_GSM", NapiUtil::ToInt32Value(env, static_cast<int32_t>(RanType::TYPE_GSM))),
        DECLARE_NAPI_STATIC_PROPERTY(
            "TYPE_CDMA", NapiUtil::ToInt32Value(env, static_cast<int32_t>(RanType::TYPE_CDMA))),
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc));
    return exports;
}

static napi_value InitEnumSmsSegmentsInfo(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_STATIC_PROPERTY("SMS_ENCODING_UNKNOWN",
            NapiUtil::ToInt32Value(
                env, static_cast<int32_t>(ISmsServiceInterface::SmsEncodingScheme::SMS_ENCODING_UNKNOWN))),
        DECLARE_NAPI_STATIC_PROPERTY("SMS_ENCODING_7BIT",
            NapiUtil::ToInt32Value(
                env, static_cast<int32_t>(ISmsServiceInterface::SmsEncodingScheme::SMS_ENCODING_7BIT))),
        DECLARE_NAPI_STATIC_PROPERTY("SMS_ENCODING_8BIT",
            NapiUtil::ToInt32Value(
                env, static_cast<int32_t>(ISmsServiceInterface::SmsEncodingScheme::SMS_ENCODING_8BIT))),
        DECLARE_NAPI_STATIC_PROPERTY("SMS_ENCODING_16BIT",
            NapiUtil::ToInt32Value(
                env, static_cast<int32_t>(ISmsServiceInterface::SmsEncodingScheme::SMS_ENCODING_16BIT))),
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc));
    return exports;
}

EXTERN_C_START
napi_value InitNapiSmsRegistry(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_FUNCTION("sendMessage", SendMessage),
        DECLARE_NAPI_FUNCTION("createMessage", CreateMessage),
        DECLARE_NAPI_FUNCTION("setDefaultSmsSlotId", SetDefaultSmsSlotId),
        DECLARE_NAPI_FUNCTION("getDefaultSmsSlotId", GetDefaultSmsSlotId),
        DECLARE_NAPI_FUNCTION("setSmscAddr", SetSmscAddr),
        DECLARE_NAPI_FUNCTION("getSmscAddr", GetSmscAddr),
        DECLARE_NAPI_FUNCTION("addSimMessage", AddSimMessage),
        DECLARE_NAPI_FUNCTION("delSimMessage", DelSimMessage),
        DECLARE_NAPI_FUNCTION("updateSimMessage", UpdateSimMessage),
        DECLARE_NAPI_FUNCTION("getAllSimMessages", GetAllSimMessages),
        DECLARE_NAPI_FUNCTION("setCBConfig", SetCBConfig),
        DECLARE_NAPI_FUNCTION("splitMessage", SplitMessage),
        DECLARE_NAPI_FUNCTION("hasSmsCapability", HasSmsCapability),
        DECLARE_NAPI_FUNCTION("getSmsSegmentsInfo", GetSmsSegmentsInfo),
        DECLARE_NAPI_FUNCTION("isImsSmsSupported", IsImsSmsSupported),
        DECLARE_NAPI_FUNCTION("getImsShortMessageFormat", GetImsShortMessageFormat),
        DECLARE_NAPI_FUNCTION("decodeMms", NapiMms::DecodeMms),
        DECLARE_NAPI_FUNCTION("encodeMms", NapiMms::EncodeMms),
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc));
    InitEnumSendSmsResult(env, exports);
    InitEnumShortMessageClass(env, exports);
    InitEnumMessageStatusClass(env, exports);
    InitEnumRanType(env, exports);
    InitEnumSmsSegmentsInfo(env, exports);
    NapiMms::InitEnumMmsCharSets(env, exports);
    NapiMms::InitEnumMessageType(env, exports);
    NapiMms::InitEnumPriorityType(env, exports);
    NapiMms::InitEnumVersionType(env, exports);
    NapiMms::InitEnumDispositionType(env, exports);
    NapiMms::InitEnumReportAllowedType(env, exports);
    return exports;
}
EXTERN_C_END

static napi_module g_smsModule = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = InitNapiSmsRegistry,
    .nm_modname = "telephony.sms",
    .nm_priv = ((void *)0),
    .reserved = {(void *)0},
};

extern "C" __attribute__((constructor)) void RegisterTelephonySmsModule(void)
{
    napi_module_register(&g_smsModule);
}
} // namespace Telephony
} // namespace OHOS
