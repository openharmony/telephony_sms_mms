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
#include "sms_mms_errors.h"

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
constexpr const char *NET_TYPE_UNKNOWN_STRING = "unknown";

static bool g_validPort = false;
} // namespace

static void SetPropertyArray(napi_env env, napi_value object, const std::string &name, std::vector<unsigned char> pdu)
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

static inline bool IsValidSlotId(int32_t slotId)
{
    return ((slotId >= DEFAULT_SIM_SLOT_ID) && (slotId < SIM_SLOT_COUNT));
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

static bool InValidSlotIdOrInValidPort(int32_t slotId, uint16_t port)
{
    if (!IsValidSlotId(slotId)) {
        TELEPHONY_LOGE("InValidSlotIdOrInValidPort invalid slotid");
        return true;
    }

    if (!g_validPort) {
        TELEPHONY_LOGE("InValidSlotIdOrInValidPort invalid port");
        return true;
    }
    g_validPort = false;
    return false;
}

static int32_t ActuallySendTextMessage(SendMessageContext &parameter, std::unique_ptr<SendCallback> sendCallback,
    std::unique_ptr<DeliveryCallback> deliveryCallback)
{
    if (!IsValidSlotId(parameter.slotId)) {
        auto result = ISendShortMessageCallback::SmsSendResult::SEND_SMS_FAILURE_UNKNOWN;
        sendCallback.release()->OnSmsSendResult(result);
        deliveryCallback.release()->OnSmsDeliveryResult(u"");
        return TELEPHONY_ERR_SLOTID_INVALID;
    }
    return DelayedSingleton<SmsServiceManagerClient>::GetInstance()->SendMessage(parameter.slotId,
        parameter.destinationHost, parameter.serviceCenter, parameter.textContent, sendCallback.release(),
        deliveryCallback.release());
}

static int32_t ActuallySendDataMessage(SendMessageContext &parameter, std::unique_ptr<SendCallback> sendCallback,
    std::unique_ptr<DeliveryCallback> deliveryCallback)
{
    if (InValidSlotIdOrInValidPort(parameter.slotId, parameter.destinationPort)) {
        auto result = ISendShortMessageCallback::SmsSendResult::SEND_SMS_FAILURE_UNKNOWN;
        sendCallback.release()->OnSmsSendResult(result);
        deliveryCallback.release()->OnSmsDeliveryResult(u"");
        return TELEPHONY_ERR_SLOTID_INVALID;
    }
    if (parameter.rawDataContent.size() > 0) {
        uint16_t arrayLength = static_cast<uint16_t>(parameter.rawDataContent.size());
        return DelayedSingleton<SmsServiceManagerClient>::GetInstance()->SendMessage(parameter.slotId,
            parameter.destinationHost, parameter.serviceCenter, parameter.destinationPort, &parameter.rawDataContent[0],
            arrayLength, sendCallback.release(), deliveryCallback.release());
    }
    return TELEPHONY_ERR_ARGUMENT_INVALID;
}

static int32_t ActuallySendMessage(napi_env env, SendMessageContext &parameter)
{
    bool hasSendCallback = parameter.sendCallbackRef != nullptr;
    std::unique_ptr<SendCallback> sendCallback =
        std::make_unique<SendCallback>(hasSendCallback, env, parameter.thisVarRef, parameter.sendCallbackRef);
    if (sendCallback == nullptr) {
        TELEPHONY_LOGE("ActuallySendMessage sendCallback == nullptr");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    bool hasDeliveryCallback = parameter.deliveryCallbackRef != nullptr;
    std::unique_ptr<DeliveryCallback> deliveryCallback = std::make_unique<DeliveryCallback>(
        hasDeliveryCallback, env, parameter.thisVarRef, parameter.deliveryCallbackRef);
    if (deliveryCallback == nullptr) {
        TELEPHONY_LOGE("ActuallySendMessage deliveryCallback == nullptr");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    if (parameter.messageType == TEXT_MESSAGE_PARAMETER_MATCH) {
        return ActuallySendTextMessage(parameter, std::move(sendCallback), std::move(deliveryCallback));
    } else if (parameter.messageType == RAW_DATA_MESSAGE_PARAMETER_MATCH) {
        return ActuallySendDataMessage(parameter, std::move(sendCallback), std::move(deliveryCallback));
    }
    return TELEPHONY_ERR_ARGUMENT_INVALID;
}

static void NativeSendMessage(napi_env env, void *data)
{
    auto asyncContext = static_cast<SendMessageContext *>(data);
    if (asyncContext == nullptr) {
        TELEPHONY_LOGE("NativeSendMessage SendMessageContext is nullptr.");
        NapiUtil::ThrowParameterError(env);
        return;
    }
    int32_t errorCode = ActuallySendMessage(env, *asyncContext);
    if (errorCode == TELEPHONY_SUCCESS) {
        asyncContext->resolved = true;
    }
}

static void SendMessageCallback(napi_env env, napi_status status, void *data)
{
    auto asyncContext = static_cast<SendMessageContext *>(data);
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
        int32_t destinationPort = INVALID_PORT;
        napi_value destinationPortValue = nullptr;
        napi_get_named_property(env, object, g_destinationPortStr.data(), &destinationPortValue);
        napi_get_value_int32(env, destinationPortValue, &destinationPort);
        TELEPHONY_LOGI("SendMessage destinationPort: %{private}d", destinationPort);
        if (destinationPort >= MIN_PORT && destinationPort <= MAX_PORT) {
            g_validPort = true;
        }
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
    if (messageMatchResult == MESSAGE_PARAMETER_NOT_MATCH) {
        TELEPHONY_LOGE("SendMessage parameter matching failed.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    auto asyncContext = std::make_unique<SendMessageContext>().release();
    if (asyncContext == nullptr) {
        TELEPHONY_LOGE("SendMessage SendMessageContext is nullptr.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
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
    auto asyncContext = static_cast<CreateMessageContext *>(data);
    if (asyncContext->specification.empty() || asyncContext->pdu.empty()) {
        asyncContext->errorCode = TELEPHONY_ERR_ARGUMENT_INVALID;
        return;
    }
    TELEPHONY_LOGI("NativeCreateMessage before CreateMessage");
    std::u16string specification16 = NapiUtil::ToUtf16(asyncContext->specification);
    auto shortMessageObj = new ShortMessage();
    asyncContext->errorCode = ShortMessage::CreateMessage(asyncContext->pdu, specification16, *shortMessageObj);
    if (asyncContext->errorCode == TELEPHONY_ERR_SUCCESS) {
        asyncContext->resolved = true;
        asyncContext->shortMessage = shortMessageObj;
    } else {
        TELEPHONY_LOGI("NativeCreateMessage CreateMessage faied");
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
    std::u16string smscAddress;
    shortMessage.GetScAddress(smscAddress);
    NapiUtil::SetPropertyStringUtf8(env, object, "scAddress", NapiUtil::ToUtf8(smscAddress));
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
            if (asyncContext->shortMessage != nullptr) {
                delete asyncContext->shortMessage;
                asyncContext->shortMessage = nullptr;
            }
        } else {
            JsError error = NapiUtil::ConverErrorMessageForJs(asyncContext->errorCode);
            callbackValue = NapiUtil::CreateErrorMessage(env, error.errorMessage, error.errorCode);
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

static napi_value CreateMessage(napi_env env, napi_callback_info info)
{
    size_t parameterCount = THREE_PARAMETERS;
    napi_value parameters[THREE_PARAMETERS] = {0};
    napi_value thisVar = nullptr;
    void *data = nullptr;

    napi_get_cb_info(env, info, &parameterCount, parameters, &thisVar, &data);
    if (!MatchCreateMessageParameter(env, parameters, parameterCount)) {
        TELEPHONY_LOGE("CreateMessage parameter matching failed.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    auto asyncContext = std::make_unique<CreateMessageContext>().release();
    if (asyncContext == nullptr) {
        TELEPHONY_LOGE("CreateMessage CreateMessageContext is nullptr.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
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
    if (parameterCount == THREE_PARAMETERS) {
        napi_create_reference(env, parameters[2], DEFAULT_REF_COUNT, &(asyncContext->callbackRef));
    }
    return NapiUtil ::HandleAsyncWork(
        env, asyncContext, "CreateMessage", NativeCreateMessage, CreateMessageCallback);
}

static bool MatchSetDefaultSmsSlotIdParameters(napi_env env, const napi_value parameters[], size_t parameterCount)
{
    switch (parameterCount) {
        case ONE_PARAMETER: {
            return NapiUtil::MatchParameters(env, parameters, {napi_number});
        }
        case TWO_PARAMETERS:
            return NapiUtil::MatchParameters(env, parameters, {napi_number, napi_function});
        default: {
            return false;
        }
    }
}

static void NativeSetDefaultSmsSlotId(napi_env env, void *data)
{
    auto context = static_cast<SetDefaultSmsSlotIdContext *>(data);
    if (!IsValidSlotId(context->slotId) && (context->slotId != DEFAULT_SIM_SLOT_ID_REMOVE)) {
        TELEPHONY_LOGE("NativeSetDefaultSmsSlotId slotId is invalid");
        context->errorCode = ERROR_SLOT_ID_INVALID;
        return;
    }
    context->errorCode = DelayedSingleton<SmsServiceManagerClient>::GetInstance()->SetDefaultSmsSlotId(context->slotId);
    if (context->errorCode == TELEPHONY_ERR_SUCCESS) {
        context->resolved = true;
    }
    TELEPHONY_LOGI("NativeSetDefaultSmsSlotId end resolved = %{public}d", context->resolved);
}

static void SetDefaultSmsSlotIdCallback(napi_env env, napi_status status, void *data)
{
    auto context = static_cast<SetDefaultSmsSlotIdContext *>(data);
    TELEPHONY_LOGI("SetDefaultSmsSlotIdCallback status = %{public}d", status);
    napi_value callbackValue = nullptr;
    if (context->resolved) {
        napi_get_undefined(env, &callbackValue);
    } else {
        JsError error = NapiUtil::ConverErrorMessageWithPermissionForJs(
            context->errorCode, "setDefaultSmsSlotId", "ohos.permission.SET_TELEPHONY_STATE");
        callbackValue = NapiUtil::CreateErrorMessage(env, error.errorMessage, error.errorCode);
    }
    NapiUtil::Handle1ValueCallback(env, context, callbackValue);
}

static napi_value SetDefaultSmsSlotId(napi_env env, napi_callback_info info)
{
    size_t parameterCount = TWO_PARAMETERS;
    napi_value parameters[TWO_PARAMETERS] = {0};
    napi_value thisVar = nullptr;
    void *data = nullptr;

    napi_get_cb_info(env, info, &parameterCount, parameters, &thisVar, &data);
    if (!MatchSetDefaultSmsSlotIdParameters(env, parameters, parameterCount)) {
        TELEPHONY_LOGE("SetDefaultSmsSlotId parameter matching failed.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    auto context = std::make_unique<SetDefaultSmsSlotIdContext>().release();
    if (context == nullptr) {
        TELEPHONY_LOGE("SetDefaultSmsSlotId SetDefaultSmsSlotIdContext is nullptr.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    napi_get_value_int32(env, parameters[0], &context->slotId);
    if (parameterCount == TWO_PARAMETERS) {
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
        case NONE_PARAMETER: {
            return true;
        }
        case ONE_PARAMETER: {
            return NapiUtil::MatchParameters(env, parameters, {napi_function});
        }
        default: {
            return false;
        }
    }
}

static void NativeGetDefaultSmsSlotId(napi_env env, void *data)
{
    auto context = static_cast<GetDefaultSmsSlotIdContext *>(data);
    context->defaultSmsSlotId = DelayedSingleton<SmsServiceManagerClient>::GetInstance()->GetDefaultSmsSlotId();
    TELEPHONY_LOGI("NativeGetDefaultSmsSlotId defaultSmsSlotId  = %{public}d", context->defaultSmsSlotId);
    if (context->defaultSmsSlotId >= SIM_SLOT_0) {
        context->resolved = true;
    } else {
        context->resolved = false;
    }
}

static void GetDefaultSmsSlotIdCallback(napi_env env, napi_status status, void *data)
{
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
    size_t parameterCount = 1;
    napi_value parameters[1] = {0};
    napi_value thisVar = nullptr;
    void *data = nullptr;

    napi_get_cb_info(env, info, &parameterCount, parameters, &thisVar, &data);
    NAPI_ASSERT(env, MatchGetDefaultSmsSlotIdParameters(env, parameters, parameterCount), "type mismatch");
    auto context = std::make_unique<GetDefaultSmsSlotIdContext>().release();
    if (context == nullptr) {
        std::string errorCode = std::to_string(napi_generic_failure);
        std::string errorMessage = "error at GetDefaultSmsSlotIdContext is nullptr";
        NAPI_CALL(env, napi_throw_error(env, errorCode.c_str(), errorMessage.c_str()));
        return nullptr;
    }
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
        case TWO_PARAMETERS: {
            return NapiUtil::MatchParameters(env, parameters, {napi_number, napi_string});
        }
        case THREE_PARAMETERS: {
            return NapiUtil::MatchParameters(env, parameters, {napi_number, napi_string, napi_function});
        }
        default: {
            return false;
        }
    }
}

static void NativeSetSmscAddr(napi_env env, void *data)
{
    auto context = static_cast<SetSmscAddrContext *>(data);
    if (!IsValidSlotId(context->slotId)) {
        TELEPHONY_LOGE("NativeSetSmscAddr slotId is invalid");
        context->errorCode = ERROR_SLOT_ID_INVALID;
        return;
    }
    context->errorCode = DelayedSingleton<SmsServiceManagerClient>::GetInstance()->SetScAddress(context->slotId,
        NapiUtil::ToUtf16(context->smscAddr));
    if (context->errorCode == TELEPHONY_ERR_SUCCESS) {
        context->resolved = true;
    }
    TELEPHONY_LOGI("NativeSetSmscAddr resolved = %{public}d", context->resolved);
}

static void SetSmscAddrCallback(napi_env env, napi_status status, void *data)
{
    auto context = static_cast<SetSmscAddrContext *>(data);
    napi_value callbackValue = nullptr;
    if (context->resolved) {
        napi_get_undefined(env, &callbackValue);
    } else {
        JsError error = NapiUtil::ConverErrorMessageWithPermissionForJs(
            context->errorCode, "setSmscAddr", "ohos.permission.SET_TELEPHONY_STATE");
        callbackValue = NapiUtil::CreateErrorMessage(env, error.errorMessage, error.errorCode);
    }
    NapiUtil::Handle1ValueCallback(env, context, callbackValue);
}

static napi_value SetSmscAddr(napi_env env, napi_callback_info info)
{
    size_t parameterCount = THREE_PARAMETERS;
    napi_value parameters[THREE_PARAMETERS] = {0};
    napi_value thisVar = nullptr;
    void *data = nullptr;

    napi_get_cb_info(env, info, &parameterCount, parameters, &thisVar, &data);
    if (!MatchSetSmscAddrParameters(env, parameters, parameterCount)) {
        TELEPHONY_LOGE("SetSmscAddr parameter matching failed.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    TELEPHONY_LOGI("SetSmscAddr start after MatchSetSmscAddrParameters");
    auto context = std::make_unique<SetSmscAddrContext>().release();
    if (context == nullptr) {
        TELEPHONY_LOGE("SetSmscAddr SetSmscAddrContext is nullptr.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    TELEPHONY_LOGI("SetSmscAddr start after SetSmscAddrContext contruct");
    napi_get_value_int32(env, parameters[0], &context->slotId);
    context->smscAddr = Get64StringFromValue(env, parameters[1]);
    TELEPHONY_LOGI("SetSmscAddr smscAddr = %{private}s", context->smscAddr.data());
    if (parameterCount == THREE_PARAMETERS) {
        napi_create_reference(env, parameters[2], DEFAULT_REF_COUNT, &context->callbackRef);
    }
    TELEPHONY_LOGI("SetSmscAddr before end");
    return NapiUtil::HandleAsyncWork(env, context, "SetSmscAddr", NativeSetSmscAddr, SetSmscAddrCallback);
}

static bool MatchGetSmscAddrParameters(napi_env env, const napi_value parameters[], size_t parameterCount)
{
    switch (parameterCount) {
        case ONE_PARAMETER: {
            return NapiUtil::MatchParameters(env, parameters, {napi_number});
        }
        case TWO_PARAMETERS: {
            return NapiUtil::MatchParameters(env, parameters, {napi_number, napi_function});
        }
        default: {
            return false;
        }
    }
}

static void NativeGetSmscAddr(napi_env env, void *data)
{
    auto context = static_cast<GetSmscAddrContext *>(data);
    if (!IsValidSlotId(context->slotId)) {
        TELEPHONY_LOGE("NativeGetSmscAddr slotId is invalid");
        context->errorCode = ERROR_SLOT_ID_INVALID;
        return;
    }
    std::u16string smscAddress;
    context->errorCode = DelayedSingleton<SmsServiceManagerClient>::GetInstance()->
        GetScAddress(context->slotId, smscAddress);
    if (context->errorCode == TELEPHONY_ERR_SUCCESS) {
        context->smscAddr = NapiUtil::ToUtf8(smscAddress);
        context->resolved = true;
        TELEPHONY_LOGI("NativeGetSmscAddr smscAddr = %{private}s", context->smscAddr.data());
    }
    TELEPHONY_LOGI("NativeGetSmscAddr resolved = %{public}d", context->resolved);
}

static void GetSmscAddrCallback(napi_env env, napi_status status, void *data)
{
    auto context = static_cast<GetSmscAddrContext *>(data);
    napi_value callbackValue = nullptr;
    if (context->resolved) {
        napi_create_string_utf8(env, context->smscAddr.data(), context->smscAddr.length(), &callbackValue);
    } else {
        JsError error = NapiUtil::ConverErrorMessageWithPermissionForJs(
            context->errorCode, "getSmscAddr", "ohos.permission.GET_TELEPHONY_STATE");
        callbackValue = NapiUtil::CreateErrorMessage(env, error.errorMessage, error.errorCode);
    }
    NapiUtil::Handle2ValueCallback(env, context, callbackValue);
}

static napi_value GetSmscAddr(napi_env env, napi_callback_info info)
{
    size_t parameterCount = TWO_PARAMETERS;
    napi_value parameters[TWO_PARAMETERS] = {0};
    napi_value thisVar = nullptr;
    void *data = nullptr;

    napi_get_cb_info(env, info, &parameterCount, parameters, &thisVar, &data);
    if (!MatchGetSmscAddrParameters(env, parameters, parameterCount)) {
        TELEPHONY_LOGE("GetSmscAddr parameter matching failed.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    auto context = std::make_unique<GetSmscAddrContext>().release();
    if (context == nullptr) {
        TELEPHONY_LOGE("GetSmscAddr GetSmscAddrContext is nullptr.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    napi_get_value_int32(env, parameters[0], &context->slotId);
    if (parameterCount == TWO_PARAMETERS) {
        napi_create_reference(env, parameters[1], DEFAULT_REF_COUNT, &context->callbackRef);
    }
    return NapiUtil::HandleAsyncWork(env, context, "GetSmscAddr", NativeGetSmscAddr, GetSmscAddrCallback);
}

static bool MatchAddSimMessageParameters(napi_env env, const napi_value parameters[], size_t parameterCount)
{
    bool typeMatch = false;
    switch (parameterCount) {
        case ONE_PARAMETER: {
            typeMatch = NapiUtil::MatchParameters(env, parameters, {napi_object});
            break;
        }
        case TWO_PARAMETERS: {
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
    auto context = static_cast<AddSimMessageContext *>(data);
    int32_t wrapStatus = static_cast<int32_t>(context->status);
    TELEPHONY_LOGI("NativeAddSimMessage start wrapStatus = %{public}d", wrapStatus);
    if (wrapStatus != MESSAGE_UNKNOWN_STATUS) {
        ISmsServiceInterface::SimMessageStatus status =
            static_cast<ISmsServiceInterface::SimMessageStatus>(wrapStatus);
        context->errorCode = DelayedSingleton<SmsServiceManagerClient>::GetInstance()->AddSimMessage(
            context->slotId, NapiUtil::ToUtf16(context->smsc), NapiUtil::ToUtf16(context->pdu), status);
        if (context->errorCode == TELEPHONY_ERR_SUCCESS) {
            context->resolved = true;
        }
        TELEPHONY_LOGI("NativeAddSimMessage context->resolved = %{public}d", context->resolved);
    } else {
        context->errorCode = SMS_MMS_UNKNOWN_SIM_MESSAGE_STATUS;
    }
}

static void AddSimMessageCallback(napi_env env, napi_status status, void *data)
{
    auto context = static_cast<AddSimMessageContext *>(data);
    napi_value callbackValue = nullptr;
    if (context->resolved) {
        napi_get_undefined(env, &callbackValue);
    } else {
        JsError error = NapiUtil::ConverErrorMessageWithPermissionForJs(
            context->errorCode, "addSimMessage", "ohos.permission.SEND_MESSAGES");
        callbackValue = NapiUtil::CreateErrorMessage(env, error.errorMessage, error.errorCode);
    }
    NapiUtil::Handle1ValueCallback(env, context, callbackValue);
}

static napi_value AddSimMessage(napi_env env, napi_callback_info info)
{
    size_t parameterCount = TWO_PARAMETERS;
    napi_value parameters[TWO_PARAMETERS] = {0};
    napi_value thisVar = nullptr;
    void *data = nullptr;

    napi_get_cb_info(env, info, &parameterCount, parameters, &thisVar, &data);
    if (!MatchAddSimMessageParameters(env, parameters, parameterCount)) {
        TELEPHONY_LOGE("AddSimMessage parameter matching failed.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    auto context = std::make_unique<AddSimMessageContext>().release();
    if (context == nullptr) {
        TELEPHONY_LOGE("AddSimMessage AddSimMessageContext is nullptr.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
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
    if (parameterCount == TWO_PARAMETERS) {
        napi_create_reference(env, parameters[1], DEFAULT_REF_COUNT, &context->callbackRef);
    }
    return NapiUtil::HandleAsyncWork(env, context, "AddSimMessage", NativeAddSimMessage, AddSimMessageCallback);
}

static bool MatchDelSimMessageParameters(napi_env env, const napi_value parameters[], size_t parameterCount)
{
    switch (parameterCount) {
        case TWO_PARAMETERS: {
            return NapiUtil::MatchParameters(env, parameters, {napi_number, napi_number});
        }
        case THREE_PARAMETERS: {
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
    if (!IsValidSlotId(context->slotId)) {
        TELEPHONY_LOGE("NativeDelSimMessage slotId is invalid");
        context->errorCode = ERROR_SLOT_ID_INVALID;
        return;
    }
    context->errorCode = DelayedSingleton<SmsServiceManagerClient>::GetInstance()->DelSimMessage(context->slotId,
        context->msgIndex);
    if (context->errorCode == TELEPHONY_ERR_SUCCESS) {
        context->resolved = true;
    }
    TELEPHONY_LOGI("NativeDelSimMessage resolved = %{public}d", context->resolved);
}

static void DelSimMessageCallback(napi_env env, napi_status status, void *data)
{
    auto context = static_cast<DelSimMessageContext *>(data);
    napi_value callbackValue = nullptr;
    if (context->resolved) {
        napi_get_undefined(env, &callbackValue);
    } else {
        JsError error = NapiUtil::ConverErrorMessageWithPermissionForJs(
            context->errorCode, "delSimMessage", "ohos.permission.SEND_MESSAGES");
        callbackValue = NapiUtil::CreateErrorMessage(env, error.errorMessage, error.errorCode);
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
    if (!MatchDelSimMessageParameters(env, parameters, parameterCount)) {
        TELEPHONY_LOGE("DelSimMessage parameter matching failed.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    auto context = std::make_unique<DelSimMessageContext>().release();
    if (context == nullptr) {
        TELEPHONY_LOGE("DelSimMessage DelSimMessageContext is nullptr.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    napi_get_value_int32(env, parameters[0], &context->slotId);
    napi_get_value_int32(env, parameters[1], &context->msgIndex);
    if (parameterCount == 3) {
        napi_create_reference(env, parameters[2], DEFAULT_REF_COUNT, &context->callbackRef);
    }
    return NapiUtil::HandleAsyncWork(env, context, "DelSimMessage", NativeDelSimMessage, DelSimMessageCallback);
}

static bool MatchUpdateSimMessageParameters(napi_env env, const napi_value parameters[], size_t parameterCount)
{
    bool typeMatch = false;
    switch (parameterCount) {
        case ONE_PARAMETER: {
            typeMatch = NapiUtil::MatchParameters(env, parameters, {napi_object});
            break;
        }
        case TWO_PARAMETERS: {
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
        TELEPHONY_LOGD("NativeUpdateSimMessage msgPud = %{private}s", msgPud.c_str());
        context->errorCode = DelayedSingleton<SmsServiceManagerClient>::GetInstance()->UpdateSimMessage(context->slotId,
            context->msgIndex, static_cast<ISmsServiceInterface::SimMessageStatus>(context->newStatus),
            NapiUtil::ToUtf16(context->pdu), NapiUtil::ToUtf16(context->smsc));
        if (context->errorCode == TELEPHONY_ERR_SUCCESS) {
            context->resolved = true;
        }
    } else {
        TELEPHONY_LOGI("NativeUpdateSimMessage resolved false cause parameter invalided");
        context->errorCode = SMS_MMS_UNKNOWN_SIM_MESSAGE_STATUS;
    }
}

static void UpdateSimMessageCallback(napi_env env, napi_status status, void *data)
{
    auto context = static_cast<UpdateSimMessageContext *>(data);
    napi_value callbackValue = nullptr;
    if (context->resolved) {
        napi_get_undefined(env, &callbackValue);
    } else {
        JsError error = NapiUtil::ConverErrorMessageWithPermissionForJs(
            context->errorCode, "updateSimMessage", "ohos.permission.SEND_MESSAGES");
        callbackValue = NapiUtil::CreateErrorMessage(env, error.errorMessage, error.errorCode);
    }
    NapiUtil::Handle1ValueCallback(env, context, callbackValue);
}

static napi_value UpdateSimMessage(napi_env env, napi_callback_info info)
{
    size_t parameterCount = TWO_PARAMETERS;
    napi_value parameters[TWO_PARAMETERS] = {0};
    napi_value thisVar = nullptr;
    void *data = nullptr;

    napi_get_cb_info(env, info, &parameterCount, parameters, &thisVar, &data);
    if (!MatchUpdateSimMessageParameters(env, parameters, parameterCount)) {
        TELEPHONY_LOGE("UpdateSimMessage parameter matching failed.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    TELEPHONY_LOGI("UpdateSimMessage start parameter match passed");
    auto context = std::make_unique<UpdateSimMessageContext>().release();
    if (context == nullptr) {
        TELEPHONY_LOGE("UpdateSimMessage UpdateSimMessageContext is nullptr.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
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
    TELEPHONY_LOGD("UpdateSimMessage pdu = %{private}s", context->pdu.c_str());
    napi_value smscValue = NapiUtil::GetNamedProperty(env, parameters[0], "smsc");
    if (smscValue != nullptr) {
        context->smsc = Get64StringFromValue(env, smscValue);
    }
    if (parameterCount == TWO_PARAMETERS) {
        napi_create_reference(env, parameters[1], DEFAULT_REF_COUNT, &context->callbackRef);
    }
    TELEPHONY_LOGI("UpdateSimMessage start before HandleAsyncWork");
    return NapiUtil::HandleAsyncWork(
        env, context, "UpdateSimMessage", NativeUpdateSimMessage, UpdateSimMessageCallback);
}

static bool MatchGetAllSimMessagesParameters(napi_env env, const napi_value parameters[], size_t parameterCount)
{
    switch (parameterCount) {
        case ONE_PARAMETER: {
            return NapiUtil::MatchParameters(env, parameters, {napi_number});
        }
        case TWO_PARAMETERS: {
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
    if (!IsValidSlotId(context->slotId)) {
        TELEPHONY_LOGE("NativeGetAllSimMessages slotId is invalid");
        context->errorCode = ERROR_SLOT_ID_INVALID;
        return;
    }
    context->errorCode = DelayedSingleton<SmsServiceManagerClient>::GetInstance()->
        GetAllSimMessages(context->slotId, context->messageArray);
    if (context->errorCode == TELEPHONY_ERR_SUCCESS) {
        context->resolved = true;
    } else {
        TELEPHONY_LOGE("NativeGetAllSimMessages context->resolved == false");
    }
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
    if (context->resolved) {
        napi_create_array(env, &callbackValue);
        int32_t arraySize = static_cast<int32_t>(context->messageArray.size());
        for (int32_t i = 0; i < arraySize; i++) {
            ShortMessage message = context->messageArray[i];
            napi_value itemValue = CreateSimShortMessageValue(env, message);
            napi_set_element(env, callbackValue, i, itemValue);
        }
    } else {
        JsError error = NapiUtil::ConverErrorMessageWithPermissionForJs(
            context->errorCode, "getAllSimMessages", "ohos.permission.RECEIVE_SMS");
        callbackValue = NapiUtil::CreateErrorMessage(env, error.errorMessage, error.errorCode);
    }
    NapiUtil::Handle2ValueCallback(env, context, callbackValue);
}

static napi_value GetAllSimMessages(napi_env env, napi_callback_info info)
{
    size_t parameterCount = TWO_PARAMETERS;
    napi_value parameters[TWO_PARAMETERS] = {0};
    napi_value thisVar = nullptr;
    void *data = nullptr;

    napi_get_cb_info(env, info, &parameterCount, parameters, &thisVar, &data);
    if (!MatchGetAllSimMessagesParameters(env, parameters, parameterCount)) {
        TELEPHONY_LOGE("GetAllSimMessages parameter matching failed.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    auto context = std::make_unique<GetAllSimMessagesContext>().release();
    if (context == nullptr) {
        TELEPHONY_LOGE("GetAllSimMessages GetAllSimMessagesContext is nullptr.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    napi_get_value_int32(env, parameters[0], &context->slotId);
    if (parameterCount == TWO_PARAMETERS) {
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
        case ONE_PARAMETER: {
            typeMatch = NapiUtil::MatchParameters(env, parameters, {napi_object});
            break;
        }
        case TWO_PARAMETERS: {
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
    auto context = static_cast<CBConfigContext *>(data);
    context->errorCode = DelayedSingleton<SmsServiceManagerClient>::GetInstance()->SetCBConfig(
        context->slotId, context->enable, context->startMessageId, context->endMessageId, context->ranType);
    if (context->errorCode == TELEPHONY_ERR_SUCCESS) {
        context->resolved = true;
    }
    TELEPHONY_LOGI("NativeSetCBConfig end resolved = %{public}d", context->resolved);
}

static void SetCBConfigCallback(napi_env env, napi_status status, void *data)
{
    auto context = static_cast<CBConfigContext *>(data);
    napi_value callbackValue = nullptr;
    if (context->resolved) {
        napi_get_undefined(env, &callbackValue);
    } else {
        JsError error = NapiUtil::ConverErrorMessageWithPermissionForJs(
            context->errorCode, "setCBConfig", "ohos.permission.RECEIVE_SMS");
        callbackValue = NapiUtil::CreateErrorMessage(env, error.errorMessage, error.errorCode);
    }
    NapiUtil::Handle1ValueCallback(env, context, callbackValue);
}

static napi_value SetCBConfig(napi_env env, napi_callback_info info)
{
    size_t parameterCount = TWO_PARAMETERS;
    napi_value parameters[TWO_PARAMETERS] = {0};
    napi_value thisVar = nullptr;
    void *data = nullptr;

    napi_get_cb_info(env, info, &parameterCount, parameters, &thisVar, &data);
    if (!MatchSetCBConfigParameters(env, parameters, parameterCount)) {
        TELEPHONY_LOGE("SetCBConfig parameter matching failed.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    auto context = std::make_unique<CBConfigContext>().release();
    if (context == nullptr) {
        TELEPHONY_LOGE("SetCBConfig CBConfigContext is nullptr.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
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
    if (parameterCount == TWO_PARAMETERS) {
        napi_create_reference(env, parameters[1], DEFAULT_REF_COUNT, &context->callbackRef);
    }
    napi_value result =
        NapiUtil::HandleAsyncWork(env, context, "SetCBConfig", NativeSetCBConfig, SetCBConfigCallback);
    return result;
}

static bool MatchSplitMessageParameters(napi_env env, const napi_value parameters[], size_t parameterCount)
{
    switch (parameterCount) {
        case ONE_PARAMETER:
            return NapiUtil::MatchParameters(env, parameters, {napi_string});
        case TWO_PARAMETERS:
            return NapiUtil::MatchParameters(env, parameters, {napi_string, napi_function});
        default:
            return false;
    }
}

static void NativeSplitMessage(napi_env env, void *data)
{
    auto context = static_cast<SplitMessageContext *>(data);
    std::u16string content = NapiUtil::ToUtf16(context->content);
    context->errorCode =
        DelayedSingleton<SmsServiceManagerClient>::GetInstance()->SplitMessage(content, context->messageArray);
    if (context->errorCode == TELEPHONY_ERR_SUCCESS) {
        context->resolved = true;
    }
}

static void SplitMessageCallback(napi_env env, napi_status status, void *data)
{
    auto context = static_cast<SplitMessageContext *>(data);
    napi_value callbackValue = nullptr;
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
        JsError error = NapiUtil::ConverErrorMessageWithPermissionForJs(
            context->errorCode, "splitMessage", "ohos.permission.SEND_MESSAGES");
        callbackValue = NapiUtil::CreateErrorMessage(env, error.errorMessage, error.errorCode);
    }
    NapiUtil::Handle2ValueCallback(env, context, callbackValue);
}

static napi_value SplitMessage(napi_env env, napi_callback_info info)
{
    size_t parameterCount = TWO_PARAMETERS;
    napi_value parameters[TWO_PARAMETERS] = {0};
    napi_value thisVar = nullptr;
    void *data = nullptr;

    napi_get_cb_info(env, info, &parameterCount, parameters, &thisVar, &data);
    if (!MatchSplitMessageParameters(env, parameters, parameterCount)) {
        TELEPHONY_LOGE("SplitMessage parameter matching failed.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    auto context = std::make_unique<SplitMessageContext>().release();
    if (context == nullptr) {
        TELEPHONY_LOGE("SplitMessage SplitMessageContext is nullptr.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    context->content = Get64StringFromValue(env, parameters[0]);
    TELEPHONY_LOGD("napi_sms splitMessage context->content = %{private}s", context->content.c_str());
    if (parameterCount == TWO_PARAMETERS) {
        napi_create_reference(env, parameters[1], MAX_TEXT_SHORT_MESSAGE_LENGTH, &context->callbackRef);
    }
    napi_value result =
        NapiUtil::HandleAsyncWork(env, context, "SplitMessage", NativeSplitMessage, SplitMessageCallback);
    return result;
}

static napi_value HasSmsCapability(napi_env env, napi_callback_info info)
{
    napi_value result = nullptr;
    napi_get_boolean(env, DelayedSingleton<SmsServiceManagerClient>::GetInstance()->HasSmsCapability(), &result);
    return result;
}

static bool MatchGetSmsSegmentsInfoParameters(napi_env env, const napi_value parameters[], size_t parameterCount)
{
    switch (parameterCount) {
        case THREE_PARAMETERS:
            return NapiUtil::MatchParameters(env, parameters, {napi_number, napi_string, napi_boolean});
        case FOUR_PARAMETERS:
            return NapiUtil::MatchParameters(
                env, parameters, {napi_number, napi_string, napi_boolean, napi_function});
        default:
            return false;
    }
}

static void NativeGetSmsSegmentsInfo(napi_env env, void *data)
{
    auto context = static_cast<GetSmsSegmentsInfoContext *>(data);
    if (!IsValidSlotId(context->slotId)) {
        TELEPHONY_LOGE("NativeGetSmsSegmentsInfo slotId is invalid");
        context->errorCode = ERROR_SLOT_ID_INVALID;
        return;
    }
    std::u16string content = NapiUtil::ToUtf16(context->content);
    ISmsServiceInterface::SmsSegmentsInfo info;
    context->errorCode = DelayedSingleton<SmsServiceManagerClient>::GetInstance()->GetSmsSegmentsInfo(context->slotId,
        content, context->force7BitCode, info);
    if (context->errorCode == TELEPHONY_ERR_SUCCESS) {
        context->resolved = true;
        context->splitCount = info.msgSegCount;
        context->encodeCount = info.msgEncodingCount;
        context->encodeCountRemaining = info.msgRemainCount;
        context->scheme = info.msgCodeScheme;
    } else {
        TELEPHONY_LOGE("NativeGetSmsSegmentsInfo context->resolved == false");
    }
}

static void GetSmsSegmentsInfoCallback(napi_env env, napi_status status, void *data)
{
    auto context = static_cast<GetSmsSegmentsInfoContext *>(data);
    napi_value callbackValue = nullptr;
    if (context->resolved) {
        napi_create_object(env, &callbackValue);
        NapiUtil::SetPropertyInt32(env, callbackValue, "splitCount", context->splitCount);
        NapiUtil::SetPropertyInt32(env, callbackValue, "encodeCount", context->encodeCount);
        NapiUtil::SetPropertyInt32(env, callbackValue, "encodeCountRemaining", context->encodeCountRemaining);
        NapiUtil::SetPropertyInt32(env, callbackValue, "scheme", static_cast<int32_t>(context->scheme));
    } else {
        JsError error = NapiUtil::ConverErrorMessageForJs(context->errorCode);
        callbackValue = NapiUtil::CreateErrorMessage(env, error.errorMessage, error.errorCode);
    }
    NapiUtil::Handle2ValueCallback(env, context, callbackValue);
}

static napi_value GetSmsSegmentsInfo(napi_env env, napi_callback_info info)
{
    size_t parameterCount = FOUR_PARAMETERS;
    napi_value parameters[FOUR_PARAMETERS] = { 0 };
    napi_value thisVar = nullptr;
    void *data = nullptr;

    napi_get_cb_info(env, info, &parameterCount, parameters, &thisVar, &data);
    if (!MatchGetSmsSegmentsInfoParameters(env, parameters, parameterCount)) {
        TELEPHONY_LOGE("GetSmsSegmentsInfo parameter matching failed.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    auto context = std::make_unique<GetSmsSegmentsInfoContext>().release();
    if (context == nullptr) {
        TELEPHONY_LOGE("GetSmsSegmentsInfo context is nullptr.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    napi_get_value_int32(env, parameters[0], &context->slotId);
    context->content = NapiUtil::GetStringFromValue(env, parameters[1]);
    napi_get_value_bool(env, parameters[2], &context->force7BitCode);
    if (parameterCount == FOUR_PARAMETERS) {
        napi_create_reference(env, parameters[3], DEFAULT_REF_COUNT, &context->callbackRef);
    }
    napi_value result = NapiUtil::HandleAsyncWork(
        env, context, "GetSmsSegmentsInfo", NativeGetSmsSegmentsInfo, GetSmsSegmentsInfoCallback);
    return result;
}

static bool MatchIsImsSmsSupportedParameters(napi_env env, const napi_value parameters[], size_t parameterCount)
{
    switch (parameterCount) {
        case ONE_PARAMETER: {
            return NapiUtil::MatchParameters(env, parameters, { napi_number });
        }
        case TWO_PARAMETERS: {
            return NapiUtil::MatchParameters(env, parameters, { napi_number, napi_function });
        }
        default: {
            return false;
        }
    }
}

static void NativeIsImsSmsSupported(napi_env env, void *data)
{
    auto context = static_cast<IsImsSmsSupportedContext *>(data);
    context->errorCode = DelayedSingleton<SmsServiceManagerClient>::GetInstance()->IsImsSmsSupported(
        context->slotId, context->setResult);
    if (context->errorCode == TELEPHONY_ERR_SUCCESS) {
        context->resolved = true;
    } else {
        TELEPHONY_LOGE("NativeIsImsSmsSupported context->resolved == false");
    }
}

static void IsImsSmsSupportedCallback(napi_env env, napi_status status, void *data)
{
    auto context = static_cast<IsImsSmsSupportedContext *>(data);
    napi_value callbackValue = nullptr;
    if (context->resolved) {
        napi_get_boolean(env, context->setResult, &callbackValue);
    } else {
        JsError error = NapiUtil::ConverErrorMessageForJs(context->errorCode);
        callbackValue = NapiUtil::CreateErrorMessage(env, error.errorMessage, error.errorCode);
    }
    NapiUtil::Handle2ValueCallback(env, context, callbackValue);
}

static napi_value IsImsSmsSupported(napi_env env, napi_callback_info info)
{
    size_t paramsCount = TWO_PARAMETERS;
    napi_value params[TWO_PARAMETERS] = { 0 };
    napi_value arg = nullptr;
    void *data = nullptr;

    napi_get_cb_info(env, info, &paramsCount, params, &arg, &data);
    if (!MatchIsImsSmsSupportedParameters(env, params, paramsCount)) {
        TELEPHONY_LOGE("IsImsSmsSupported parameter matching failed.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    auto context = std::make_unique<IsImsSmsSupportedContext>().release();
    if (context == nullptr) {
        TELEPHONY_LOGE("IsImsSmsSupported IsImsSmsSupportedContext is nullptr.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    napi_get_value_int32(env, params[0], &context->slotId);
    if (paramsCount == TWO_PARAMETERS) {
        napi_create_reference(env, params[1], DEFAULT_REF_COUNT, &context->callbackRef);
    }
    napi_value result = NapiUtil::HandleAsyncWork(
        env, context, "IsImsSmsSupported", NativeIsImsSmsSupported, IsImsSmsSupportedCallback);
    return result;
}

static void NativeGetImsShortMessageFormat(napi_env env, void *data)
{
    auto context = static_cast<SingleValueContext<std::u16string> *>(data);
    context->errorCode =
        DelayedSingleton<SmsServiceManagerClient>::GetInstance()->GetImsShortMessageFormat(context->value);
    if (context->errorCode != TELEPHONY_ERR_SUCCESS) {
        context->value = NapiUtil::ToUtf16(NET_TYPE_UNKNOWN_STRING);
    }
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
    if (context->resolved) {
        std::string toUtf8Value = to_utf8(context->value);
        napi_create_string_utf8(env, toUtf8Value.c_str(), toUtf8Value.size(), &callbackValue);
    } else {
        JsError error = NapiUtil::ConverErrorMessageForJs(context->errorCode);
        callbackValue = NapiUtil::CreateErrorMessage(env, error.errorMessage, error.errorCode);
    }
    NapiUtil::Handle2ValueCallback(env, context, callbackValue);
}

static napi_value GetImsShortMessageFormat(napi_env env, napi_callback_info info)
{
    size_t paramsCount = ONE_PARAMETER;
    napi_value params[ONE_PARAMETER] = { 0 };
    napi_value arg = nullptr;
    void *data = nullptr;

    napi_get_cb_info(env, info, &paramsCount, params, &arg, &data);
    if (!MatchIsImsSmsSupportedParameters(env, params, paramsCount)) {
        TELEPHONY_LOGE("GetImsShortMessageFormat parameter matching failed.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    auto context = std::make_unique<SingleValueContext<std::u16string>>().release();
    if (context == nullptr) {
        TELEPHONY_LOGE("GetImsShortMessageFormat SingleValueContext is nullptr.");
        NapiUtil::ThrowParameterError(env);
        return nullptr;
    }
    if (paramsCount == ONE_PARAMETER) {
        napi_create_reference(env, params[0], DEFAULT_REF_COUNT, &context->callbackRef);
    }
    napi_value result = NapiUtil::HandleAsyncWork(env, context, "GetImsShortMessageFormat",
        NativeGetImsShortMessageFormat, GetImsShortMessageFormatCallback);
    return result;
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

static napi_value CreateEnumSendSmsResult(napi_env env, napi_value exports)
{
    napi_value success = nullptr;
    napi_value unknow = nullptr;
    napi_value radioOff = nullptr;
    napi_value serviceUnavailable = nullptr;

    napi_create_int32(env, (int32_t)SendSmsResult::SEND_SMS_SUCCESS, &success);
    napi_create_int32(env, (int32_t)SendSmsResult::SEND_SMS_FAILURE_UNKNOWN, &unknow);
    napi_create_int32(env, (int32_t)SendSmsResult::SEND_SMS_FAILURE_RADIO_OFF, &radioOff);
    napi_create_int32(env, (int32_t)SendSmsResult::SEND_SMS_FAILURE_SERVICE_UNAVAILABLE, &serviceUnavailable);

    napi_property_descriptor desc[] = {
        DECLARE_NAPI_STATIC_PROPERTY("SEND_SMS_SUCCESS", success),
        DECLARE_NAPI_STATIC_PROPERTY("SEND_SMS_FAILURE_UNKNOWN", unknow),
        DECLARE_NAPI_STATIC_PROPERTY("SEND_SMS_FAILURE_RADIO_OFF", radioOff),
        DECLARE_NAPI_STATIC_PROPERTY("SEND_SMS_FAILURE_SERVICE_UNAVAILABLE", serviceUnavailable),
    };

    napi_value result = nullptr;
    napi_define_class(env, "SendSmsResult", NAPI_AUTO_LENGTH, CreateEnumConstructor, nullptr,
        sizeof(desc) / sizeof(*desc), desc, &result);
    napi_set_named_property(env, exports, "SendSmsResult", result);
    return exports;
}

static napi_value CreateEnumShortMessageClass(napi_env env, napi_value exports)
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
    napi_value result = nullptr;
    napi_define_class(env, "ShortMessageClass", NAPI_AUTO_LENGTH, CreateEnumConstructor, nullptr,
        sizeof(desc) / sizeof(*desc), desc, &result);
    napi_set_named_property(env, exports, "ShortMessageClass", result);
    return exports;
}

static napi_value CreateEnumMessageStatusClass(napi_env env, napi_value exports)
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
    napi_value result = nullptr;
    napi_define_class(env, "SimMessageStatus", NAPI_AUTO_LENGTH, CreateEnumConstructor, nullptr,
        sizeof(desc) / sizeof(*desc), desc, &result);
    napi_set_named_property(env, exports, "SimMessageStatus", result);
    return exports;
}

static napi_value CreateEnumRanType(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_STATIC_PROPERTY(
            "TYPE_GSM", NapiUtil::ToInt32Value(env, static_cast<int32_t>(RanType::TYPE_GSM))),
        DECLARE_NAPI_STATIC_PROPERTY(
            "TYPE_CDMA", NapiUtil::ToInt32Value(env, static_cast<int32_t>(RanType::TYPE_CDMA))),
    };
    napi_value result = nullptr;
    napi_define_class(env, "RanType", NAPI_AUTO_LENGTH, CreateEnumConstructor, nullptr,
        sizeof(desc) / sizeof(*desc), desc, &result);
    napi_set_named_property(env, exports, "RanType", result);
    return exports;
}

static napi_value CreateEnumSmsSegmentsInfo(napi_env env, napi_value exports)
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
    napi_value result = nullptr;
    napi_define_class(env, "SmsEncodingScheme", NAPI_AUTO_LENGTH, CreateEnumConstructor, nullptr,
        sizeof(desc) / sizeof(*desc), desc, &result);
    napi_set_named_property(env, exports, "SmsEncodingScheme", result);
    return exports;
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
    CreateEnumSendSmsResult(env, exports);
    CreateEnumShortMessageClass(env, exports);
    CreateEnumMessageStatusClass(env, exports);
    CreateEnumRanType(env, exports);
    CreateEnumSmsSegmentsInfo(env, exports);
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
    NapiMms::InitSupportEnumMmsCharSets(env, exports);
    NapiMms::InitSupportEnumMessageType(env, exports);
    NapiMms::InitSupportEnumPriorityType(env, exports);
    NapiMms::InitSupportEnumVersionType(env, exports);
    NapiMms::InitSupportEnumDispositionType(env, exports);
    NapiMms::InitSupportEnumReportAllowedType(env, exports);
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
