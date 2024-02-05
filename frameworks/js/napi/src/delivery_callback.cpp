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
    if (!pdu.empty()) {
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
        work = nullptr;
        return;
    }
    napi_env env_ = pContext->env;
    napi_ref thisVarRef_ = pContext->thisVarRef;
    napi_ref callbackRef_ = pContext->callbackRef;
    std::string pduStr_ = pContext->pduStr;
    napi_handle_scope scope = nullptr;
    napi_open_handle_scope(env_, &scope);
    if (scope == nullptr) {
        TELEPHONY_LOGE("scope is nullptr");
        delete work;
        work = nullptr;
        return;
    }
    napi_value callbackFunc = nullptr;
    napi_get_reference_value(env_, callbackRef_, &callbackFunc);
    napi_value callbackValues[CALLBACK_VALUE_LEN] = { 0 };
    GetCallbackValues(env_, pduStr_, callbackValues, CALLBACK_VALUE_LEN);
    napi_value callbackResult = nullptr;
    napi_value thisVar = nullptr;
    size_t argc = sizeof(callbackValues) / sizeof(callbackValues[0]);
    napi_get_reference_value(env_, thisVarRef_, &thisVar);
    napi_call_function(env_, thisVar, callbackFunc, argc, callbackValues, &callbackResult);
    napi_delete_reference(env_, thisVarRef_);
    napi_delete_reference(env_, callbackRef_);
    napi_close_handle_scope(env_, scope);
    delete work;
    work = nullptr;
    TELEPHONY_LOGI("CompleteSmsDeliveryWork end");
}

void DeliveryCallback::OnSmsDeliveryResult(const std::u16string &pdu)
{
    TELEPHONY_LOGI("OnSmsDeliveryResult start");
    if (hasCallback_) {
        uv_work_t *work = new uv_work_t;
        if (work == nullptr) {
            TELEPHONY_LOGE("OnSmsDeliveryResult work is nullptr!");
            return;
        }
        uv_loop_s *loop = nullptr;
        napi_get_uv_event_loop(env_, &loop);
        DeliveryCallbackContext *pContext = std::make_unique<DeliveryCallbackContext>().release();
        if (pContext == nullptr) {
            TELEPHONY_LOGE("OnSmsDeliveryResult pContext is nullptr!");
            delete work;
            work = nullptr;
            return;
        }
        pContext->env = env_;
        pContext->thisVarRef = thisVarRef_;
        pContext->callbackRef = callbackRef_;
        pContext->pduStr = NapiUtil::ToUtf8(pdu);
        work->data = static_cast<void *>(pContext);
        int32_t errCode = uv_queue_work_with_qos(
            loop, work, [](uv_work_t *work) {},
            [](uv_work_t *work, int status) { CompleteSmsDeliveryWork(work, status); }, uv_qos_default);
        if (errCode != 0) {
            TELEPHONY_LOGE("failed to uv_queue_work_with_qos, errCode: %{public}d", errCode);
            pContext->env = nullptr;
            pContext->thisVarRef = nullptr;
            pContext->callbackRef = nullptr;
            delete pContext;
            pContext = nullptr;
            delete work;
            work = nullptr;
        }
    }
}
} // namespace Telephony
} // namespace OHOS