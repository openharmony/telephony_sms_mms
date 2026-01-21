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
static const int32_t CALLBACK_VALUE_LEN = 2;

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
        return;
    }
    if (work->data == nullptr) {
        delete work;
        return;
    }
    std::unique_ptr<SendCallbackContext> pContext(static_cast<SendCallbackContext *>(work->data));

    napi_env env = pContext->env;
    napi_ref thisVarRef = pContext->thisVarRef;
    napi_ref callbackRef = pContext->callbackRef;
    SendSmsResult wrapResult = pContext->result;
    napi_handle_scope scope = nullptr;
    if (napi_open_handle_scope(env, &scope) != napi_ok || scope == nullptr) {
        NapiSmsUtil::Unref(env, thisVarRef);
        NapiSmsUtil::Unref(env, callbackRef);
        delete work;
        return;
    }
    napi_value callbackFunc = nullptr;
    if (napi_get_reference_value(env, callbackRef, &callbackFunc) != napi_ok || callbackFunc == nullptr) {
        NapiSmsUtil::CloseHandleScope(scope, env, thisVarRef, callbackRef);
        delete work;
        return;
    }
    napi_value callbackValues[CALLBACK_VALUE_LEN] = { 0 };
    callbackValues[0] = NapiUtil::CreateUndefined(env);
    napi_create_object(env, &callbackValues[1]);
    napi_value sendResultValue = nullptr;
    napi_create_int32(env, wrapResult, &sendResultValue);
    napi_set_named_property(env, callbackValues[1], "result", sendResultValue);

    napi_value thisVar = nullptr;
    if (napi_get_reference_value(env, thisVarRef, &thisVar) != napi_ok || thisVar == nullptr) {
        NapiSmsUtil::CloseHandleScope(scope, env, thisVarRef, callbackRef);
        delete work;
        return;
    }

    napi_value callbackResult = nullptr;
    napi_call_function(env, thisVar, callbackFunc, 2, callbackValues, &callbackResult);
    NapiSmsUtil::CloseHandleScope(scope, env, thisVarRef, callbackRef);
    delete work;
    TELEPHONY_LOGI("CompleteSmsSendWork end");
}

void SendCallback::OnSmsSendResult(const ISendShortMessageCallback::SmsSendResult result)
{
    TELEPHONY_LOGI("OnSmsSendResult start");
    if (!hasCallback_) {
        TELEPHONY_LOGE("OnSmsSendResult hasCallback_ is false");
        return;
    }
    uv_loop_s *loop = nullptr;
    napi_get_uv_event_loop(env_, &loop);
    SendCallbackContext *pContext = new SendCallbackContext;
    pContext->env = env_;
    pContext->thisVarRef = thisVarRef_;
    pContext->callbackRef = callbackRef_;
    pContext->result = WrapSmsSendResult(result);
    uv_work_t *work = new uv_work_t;
    work->data = static_cast<void *>(pContext);

    napi_status st1 = napi_reference_ref(env_, thisVarRef_, nullptr);
    napi_status st2 = napi_reference_ref(env_, callbackRef_, nullptr);
    if (st1 != napi_ok || st2 != napi_ok) {
        if (st1 == napi_ok) NapiSmsUtil::Unref(env_, thisVarRef_);
        if (st2 == napi_ok) NapiSmsUtil::Unref(env_, callbackRef_);
        delete pContext;
        delete work;
        return;
    }

    int32_t errCode = uv_queue_work_with_qos(loop, work, [](uv_work_t *work) {},
        [](uv_work_t *work, int status) { CompleteSmsSendWork(work, status); }, uv_qos_default);
    if (errCode != 0) {
        TELEPHONY_LOGE("OnSmsSendResult uv_queue_work_with_qos failed, errCode: %{public}d", errCode);
        NapiSmsUtil::Unref(env_, thisVarRef_);
        NapiSmsUtil::Unref(env_, callbackRef_);
        delete pContext;
        delete work;
    }
}
} // namespace Telephony
} // namespace OHOS