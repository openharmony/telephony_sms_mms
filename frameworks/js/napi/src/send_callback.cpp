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

inline void Unref(napi_env env, napi_ref thisVarRef, napi_ref callbackRef)
{
    uint32_t refCount = 0;
    if (napi_reference_unref(env, thisVarRef, &refCount) == napi_ok && refCount == 0) {
        napi_delete_reference(env, thisVarRef);
    }
    if (napi_reference_unref(env, callbackRef, &refCount) == napi_ok && refCount == 0) {
        napi_delete_reference(env, callbackRef);
    }
}

void CompleteSmsSendWork(uv_work_t *work, int status)
{
    TELEPHONY_LOGI("CompleteSmsSendWork start");
    if (work == nullptr) {
        return;
    }
    std::unique_ptr<SendCallbackContext> pContext(static_cast<SendCallbackContext *>(work->data));
    if (pContext == nullptr) {
        delete work;
        return;
    }

    napi_env env = pContext->env;
    napi_ref thisVarRef = pContext->thisVarRef;
    napi_ref callbackRef = pContext->callbackRef;
    SendSmsResult wrapResult = pContext->result;
    napi_handle_scope scope = nullptr;
    if (napi_open_handle_scope(env, &scope) != napi_ok || scope == nullptr) {
        Unref(env, thisVarRef, callbackRef);
        return;
    }
    napi_value callbackFunc = nullptr;
    if (napi_get_reference_value(env, callbackRef, &callbackFunc) != napi_ok || callbackFunc == nullptr) {
        napi_close_handle_scope(env, scope);
        Unref(env, thisVarRef, callbackRef);
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
        napi_close_handle_scope(env, scope);
        Unref(env, thisVarRef, callbackRef);
        return;
    }

    napi_value callbackResult = nullptr;
    napi_call_function(env, thisVar, callbackFunc, 2, callbackValues, &callbackResult);
    napi_close_handle_scope(env, scope);
    Unref(env, thisVarRef, callbackRef);
    TELEPHONY_LOGI("CompleteSmsSendWork end");
}

void SendCallback::OnSmsSendResult(const ISendShortMessageCallback::SmsSendResult result)
{
    TELEPHONY_LOGI("OnSmsSendResult hasCallback_ = %d", hasCallback_);
    if (!hasCallback_) {
        return;
    }
    uv_loop_s *loop = nullptr;
    napi_get_uv_event_loop(env_, &loop);
    if (loop == nullptr) {
        return;
    }
    uv_work_t *work = new uv_work_t;
    SendCallbackContext *pContext = new (std::nothrow) SendCallbackContext();
    if (pContext == nullptr) {
        delete work;
        return;
    }
    pContext->env = env_;
    pContext->thisVarRef = thisVarRef_;
    pContext->callbackRef = callbackRef_;
    pContext->result = WrapSmsSendResult(result);
    work->data = static_cast<void *>(pContext);

    napi_status st1 = napi_reference_ref(env_, thisVarRef_, nullptr);
    napi_status st2 = napi_reference_ref(env_, callbackRef_, nullptr);
    if (st1 != napi_ok || st2 != napi_ok) {
        uint32_t refCount = 0;
        if (st1 == napi_ok && napi_reference_unref(env_, thisVarRef_, &refCount) == napi_ok && refCount == 0) {
            napi_delete_reference(env_, thisVarRef_);
        }
        if (st2 == napi_ok && napi_reference_unref(env_, callbackRef_, &refCount) == napi_ok && refCount == 0) {
            napi_delete_reference(env_, callbackRef_);
        }
        delete pContext;
        delete work;
        return;
    }

    int32_t errCode = uv_queue_work_with_qos(loop, work, [](uv_work_t *work) {},
        [](uv_work_t *work, int status) { CompleteSmsSendWork(work, status); }, uv_qos_default);
    if (errCode != 0) {
        TELEPHONY_LOGE("failed to uv_queue_work_with_qos, errCode: %{public}d", errCode);
        Unref(env_, thisVarRef_, callbackRef_);
    }
    delete pContext;
    delete work;
}
} // namespace Telephony
} // namespace OHOS