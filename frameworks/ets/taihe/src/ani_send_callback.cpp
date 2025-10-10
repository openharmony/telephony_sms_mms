/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "ani_send_callback.h"
#include "concurrency_helpers.h"
#include "ohos.telephony.sms.ISendShortMessageCallback.ani.1.hpp"
#include "telephony_log_wrapper.h"

using namespace taihe;
using arkts::concurrency_helpers::GetWorkerId;
using arkts::concurrency_helpers::SendEvent;

namespace OHOS {
namespace Telephony {
static constexpr int32_t LOCAL_SCOPE_SIZE = 16;

SendSmsResult AniSendCallback::WrapSmsSendResult(const ISendShortMessageCallback::SmsSendResult result)
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

AniSendCallback::AniSendCallback(bool hasCallback, ani_env *env, ani_ref callbackRef)
    : hasCallback_(hasCallback), env_(env), callbackRef_(callbackRef)
{}

AniSendCallback::~AniSendCallback()
{
    env_ = nullptr;
    callbackRef_ = nullptr;
}

static ani_object GetDefaultBusinessError(ani_env *env)
{
    static const char *businessErrorName = "L@ohos/base/BusinessError;";
    ani_class cls;
    auto status = env->FindClass(businessErrorName, &cls);
    if (ANI_OK != status) {
        TELEPHONY_LOGI("Not found class '%{public}s' errcode %{public}d.", businessErrorName, status);
        return nullptr;
    }
    ani_method ctor;
    status = env->Class_FindMethod(cls, "<ctor>", ":V", &ctor);
    if (ANI_OK != status) {
        TELEPHONY_LOGI("Not found ctor of '%{public}s' errcode %{public}d.", businessErrorName, status);
        return nullptr;
    }
    ani_object businessErrorObject;
    status = env->Object_New(cls, ctor, &businessErrorObject);
    if (ANI_OK != status) {
        TELEPHONY_LOGI("Can not create business error errcode %{public}d.", status);
        return nullptr;
    }
    return businessErrorObject;
}

void CompleteSmsSendWork(SendCallbackContext *context)
{
    ani_env *env = context->env;
    if (ANI_OK != env->CreateLocalScope(LOCAL_SCOPE_SIZE)) {
        TELEPHONY_LOGI("CompleteSmsSendWork CreateLocalScope failed.");
        return;
    }
    auto businessError = GetDefaultBusinessError(env);
    ::ohos::telephony::sms::SendSmsResult resultParam(::ohos::telephony::sms::SendSmsResult::key_t(context->result));
    ::ohos::telephony::sms::ISendShortMessageCallback callbackParam = { std::move(resultParam), std::move(""),
        std::move(false) };
    auto param = ::taihe::into_ani<::ohos::telephony::sms::ISendShortMessageCallback>(env, callbackParam);
    auto callbackFunc = reinterpret_cast<ani_object>(context->callbackRef);
    ani_ref ani_argv[] = {businessError, param};
    ani_ref ani_result;
    ani_class cls;
    env->FindClass("Lstd/core/Function2;", &cls);
    ani_boolean ret;
    env->Object_InstanceOf(callbackFunc, cls, &ret);
    if (!ret) {
        TELEPHONY_LOGI("%{public}s: callbackFunc is not instance Of Function2.", __func__);
    }
    auto errCode = env->FunctionalObject_Call(static_cast<ani_fn_object>(callbackFunc), 2, ani_argv, &ani_result);
    env->DestroyLocalScope();
    TELEPHONY_LOGI("CompleteSmsSendWork enter 5 call returned %{public}d.", errCode);
}

void AniSendCallback::OnSmsSendResult(const ISendShortMessageCallback::SmsSendResult result)
{
    TELEPHONY_LOGI("OnSmsSendResult start hasCallback_ = %d", hasCallback_);
    if (hasCallback_) {
        auto mainId = GetWorkerId(env_);

        SendCallbackContext *pContext = std::make_unique<SendCallbackContext>().release();
        if (pContext == nullptr) {
            TELEPHONY_LOGE("OnSmsSendResult pContext is nullptr!");
            return;
        }
        pContext->env = env_;
        pContext->callbackRef = callbackRef_;
        pContext->result = WrapSmsSendResult(result);

        auto task = [](void *data) {
            SendCallbackContext *pContext = static_cast<SendCallbackContext *>(data);
            CompleteSmsSendWork(pContext);
        };

        auto status = SendEvent(env_, mainId, task, reinterpret_cast<void *>(pContext));

        TELEPHONY_LOGI("OnSmsSendResult SendEvent status = %d", status);
    }
}
} // namespace Telephony
} // namespace OHOS