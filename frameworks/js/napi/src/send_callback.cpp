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

#include "send_callback.h"

#include <uv.h>

#include "telephony_log_wrapper.h"
#include "napi_util.h"

namespace OHOS {
namespace Telephony {
SendSmsResult SendCallback::WrapSmsSendResult(const ISendShortMessageCallback::SmsSendResult result)
{
    switch (result) {
        case ISendShortMessageCallback::SmsSendResult::SEND_SMS_SUCCESS: {
            return SendSmsResult::SEND_SMS_SUCCESS;
        }
        case ISendShortMessageCallback::SmsSendResult::SEND_SMS_FAILURE_RADIO_OFF: {
            return SendSmsResult::SEND_SMS_FAILURE_RADIO_OFF;
        }
        case ISendShortMessageCallback::SmsSendResult::SEND_SMS_FAILURE_SERVICE_UNAVAILABLE: {
            return SendSmsResult::SEND_SMS_FAILURE_SERVICE_UNAVAILABLE;
        }
        default: {
            return SendSmsResult::SEND_SMS_FAILURE_UNKNOWN;
        }
    }
}

SendCallback::SendCallback(bool hasCallback, napi_env env, napi_ref thisVarRef, napi_ref callbackRef)
    : hasCallback_(hasCallback), env_(env), thisVarRef_(thisVarRef), callbackRef_(callbackRef)
{}

SendCallback::~SendCallback()
{
    env_ = nullptr;
    thisVarRef_ = nullptr;
    callbackRef_ = nullptr;
}

void CompleteSmsSendWork(uv_work_t *work, int status)
{
    TELEPHONY_LOGI("CompleteSmsSendWork start");
    if (work == nullptr) {
        TELEPHONY_LOGE("CompleteSmsSendWork work is nullptr!");
        return;
    }
    std::unique_ptr<SendCallbackContext> pContext(static_cast<SendCallbackContext *>(work->data));
    if (pContext == nullptr) {
        TELEPHONY_LOGE("CompleteSmsSendWork pContext is nullptr!");
        return;
    }
    napi_env env_ = pContext->env;
    napi_ref thisVarRef_ = pContext->thisVarRef;
    napi_ref callbackRef_ = pContext->callbackRef;
    SendSmsResult wrapResult = pContext->result;
    napi_handle_scope scope = nullptr;
    napi_open_handle_scope(env_, &scope);
    if (scope == nullptr) {
        TELEPHONY_LOGE("scope is nullptr");
        napi_close_handle_scope(env_, scope);
        return;
    }
    napi_value callbackFunc = nullptr;
    napi_get_reference_value(env_, callbackRef_, &callbackFunc);
    napi_value callbackValues[2] = {0};
    callbackValues[0] = NapiUtil::CreateUndefined(env_);
    napi_create_object(env_, &callbackValues[1]);
    napi_value sendResultValue = nullptr;
    napi_create_int32(env_, wrapResult, &sendResultValue);
    napi_set_named_property(env_, callbackValues[1], "result", sendResultValue);
    napi_value callbackResult = nullptr;
    napi_value thisVar = nullptr;
    napi_get_reference_value(env_, thisVarRef_, &thisVar);
    int satatus = napi_call_function(env_, thisVar, callbackFunc, 2, callbackValues, &callbackResult);
    TELEPHONY_LOGI("OnSmsSendResult napi_call_function satatus = %d", satatus);
    napi_close_handle_scope(env_, scope);
    if (work != nullptr) {
        delete work;
        work = nullptr;
    }
    TELEPHONY_LOGI("CompleteSmsSendWork end");
}

void SendCallback::OnSmsSendResult(const ISendShortMessageCallback::SmsSendResult result)
{
    TELEPHONY_LOGI("OnSmsSendResult hasCallback_ = %d", hasCallback_);
    if (hasCallback_) {
        uv_loop_s *loop = nullptr;
        napi_get_uv_event_loop(env_, &loop);
        uv_work_t *work = new uv_work_t;
        if (work == nullptr) {
            TELEPHONY_LOGE("OnSmsSendResult work is nullptr!");
            return;
        }
        SendCallbackContext *pContext = std::make_unique<SendCallbackContext>().release();
        if (pContext == nullptr) {
            TELEPHONY_LOGE("OnSmsSendResult pContext is nullptr!");
            delete work;
            work = nullptr;
            return;
        }
        pContext->env = env_;
        pContext->thisVarRef = thisVarRef_;
        pContext->callbackRef = callbackRef_;
        pContext->result = WrapSmsSendResult(result);
        work->data = static_cast<void *>(pContext);
        uv_queue_work(
            loop, work, [](uv_work_t *work) {},
            [](uv_work_t *work, int status) { CompleteSmsSendWork(work, status); });
    }
}
} // namespace Telephony
} // namespace OHOS