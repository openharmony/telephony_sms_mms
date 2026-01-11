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

#include "delivery_callback.h"

#include <uv.h>

#include "napi_util.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
static constexpr const char *PDU = "pdu";
static const int32_t CALLBACK_VALUE_LEN = 2;

DeliveryCallback::DeliveryCallback(bool hasCallback, napi_env env, napi_ref thisVarRef, napi_ref callbackRef)
    : hasCallback_(hasCallback), env_(env), thisVarRef_(thisVarRef), callbackRef_(callbackRef)
{}

DeliveryCallback::~DeliveryCallback()
{
    env_ = nullptr;
    thisVarRef_ = nullptr;
    callbackRef_ = nullptr;
}

void GetCallbackValues(napi_env &env, const std::string &pdu, napi_value *values, uint32_t len)
{
    if (len < CALLBACK_VALUE_LEN) {
        TELEPHONY_LOGE("callback values len invalid");
        return;
    }
    if (!pdu.empty() && pdu.size() <= 10000) {  // 10000 is max pdu size
        values[0] = NapiUtil::CreateUndefined(env);
        napi_create_object(env, &values[1]);
        napi_value arrayValue = nullptr;
        napi_create_array(env, &arrayValue);
        for (uint32_t i = 0; i < static_cast<uint32_t>(pdu.size()); ++i) {
            napi_value element = nullptr;
            int32_t intValue = pdu[i];
            napi_create_int32(env, intValue, &element);
            napi_set_element(env, arrayValue, i, element);
        }
        napi_set_named_property(env, values[1], PDU, arrayValue);
    } else {
        values[0] = NapiUtil::CreateErrorMessage(env, "invalid delivery report");
        values[1] = NapiUtil::CreateUndefined(env);
    }
}

void CompleteSmsDeliveryWork(uv_work_t *work, int status)
{
    TELEPHONY_LOGI("CompleteSmsDeliveryWork start");
    if (work == nullptr) {
        TELEPHONY_LOGE("CompleteSmsDeliveryWork work is nullptr");
        return;
    }
    std::unique_ptr<DeliveryCallbackContext> pContext(static_cast<DeliveryCallbackContext *>(work->data));
    if (pContext == nullptr) {
        TELEPHONY_LOGE("CompleteSmsDeliveryWork pContext is nullptr!");
        delete work;
        return;
    }

    napi_env env = pContext->env;
    napi_ref thisVarRef = pContext->thisVarRef;
    napi_ref callbackRef = pContext->callbackRef;
    std::string pduStr = pContext->pduStr;
    uint32_t unused = 0;

    napi_handle_scope scope = nullptr;
    if (napi_open_handle_scope(env, &scope) != napi_ok || scope == nullptr) {
        TELEPHONY_LOGE("CompleteSmsDeliveryWork open scope failed");
        napi_reference_unref(env, thisVarRef, &unused);
        napi_reference_unref(env, callbackRef, &unused);
        delete work;
        return;
    }

    napi_value callbackFunc = nullptr;
    if (napi_get_reference_value(env, callbackRef, &callbackFunc) != napi_ok || callbackFunc == nullptr) {
        TELEPHONY_LOGE("napi_get_reference_value callbackFunc failed");
        napi_close_handle_scope(env, scope);
        napi_reference_unref(env, thisVarRef, &unused);
        napi_reference_unref(env, callbackRef, &unused);
        delete work;
        return;
    }

    napi_value thisVar = nullptr;
    if (napi_get_reference_value(env, thisVarRef, &thisVar) != napi_ok || thisVar == nullptr) {
        TELEPHONY_LOGE("napi_get_reference_value thisVar failed");
        napi_close_handle_scope(env, scope);
        napi_reference_unref(env, thisVarRef, &unused);
        napi_reference_unref(env, callbackRef, &unused);
        delete work;
        return;
    }
    napi_value callbackValues[CALLBACK_VALUE_LEN] = { 0 };
    GetCallbackValues(env, pduStr, callbackValues, CALLBACK_VALUE_LEN);
    napi_value callbackResult = nullptr;
    size_t argc = sizeof(callbackValues) / sizeof(callbackValues[0]);
    napi_status callStatus = napi_call_function(env, thisVar, callbackFunc, argc, callbackValues, &callbackResult);
    TELEPHONY_LOGI("CompleteSmsDeliveryWork napi_call_function satatus = %d", callStatus);
    napi_close_handle_scope(env, scope);
    napi_reference_unref(env, thisVarRef, &unused);
    napi_reference_unref(env, callbackRef, &unused);
    delete work;
    TELEPHONY_LOGI("CompleteSmsDeliveryWork end");
}

void DeliveryCallback::OnSmsDeliveryResult(const std::u16string &pdu)
{
    TELEPHONY_LOGI("OnSmsDeliveryResult start");
    if (!hasCallback_) {
        return;
    }
    uv_loop_s *loop = nullptr;
    napi_get_uv_event_loop(env_, &loop);
    if (loop == nullptr) {
        TELEPHONY_LOGE("OnSmsDeliveryResult loop is nullptr");
        return;
    }
    uv_work_t *work = new (std::nothrow) uv_work_t;
    if (work == nullptr) {
        TELEPHONY_LOGE("OnSmsDeliveryResult work is nullptr!");
        return;
    }
    DeliveryCallbackContext *pContext = new (std::nothrow) DeliveryCallbackContext();
    if (pContext == nullptr) {
        TELEPHONY_LOGE("OnSmsDeliveryResult pContext is nullptr!");
        delete work;
        return;
    }
    pContext->env = env_;
    pContext->thisVarRef = thisVarRef_;
    pContext->callbackRef = callbackRef_;
    pContext->pduStr = NapiUtil::ToUtf8(pdu);
    work->data = static_cast<void *>(pContext);

    napi_status st1 = napi_reference_ref(env_, thisVarRef_, nullptr);
    napi_status st2 = napi_reference_ref(env_, callbackRef_, nullptr);
    if (st1 != napi_ok || st2 != napi_ok) {
        TELEPHONY_LOGE("OnSmsDeliveryResult napi_reference_ref failed");
        uint32_t unused;
        if (st1 == napi_ok) napi_reference_unref(env_, thisVarRef_, &unused);
        if (st2 == napi_ok) napi_reference_unref(env_, callbackRef_, &unused);
        delete pContext;
        delete work;
        return;
    }

    int32_t errCode = uv_queue_work_with_qos( loop, work, [](uv_work_t *work) {},
        [](uv_work_t *work, int status) { CompleteSmsDeliveryWork(work, status); }, uv_qos_default);
    if (errCode != 0) {
        TELEPHONY_LOGE("failed to uv_queue_work_with_qos, errCode: %{public}d", errCode);
        uint32_t unused;
        napi_reference_unref(env_, thisVarRef_, &unused);
        napi_reference_unref(env_, callbackRef_, &unused);
        delete pContext;
        delete work;
        return;
    }
}
} // namespace Telephony
} // namespace OHOS