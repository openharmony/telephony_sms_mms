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

#include "ani_delivery_callback.h"
#include "concurrency_helpers.h"
#include "ohos.telephony.sms.IDeliveryShortMessageCallback.ani.1.hpp"
#include "string_ex.h"
#include "telephony_log_wrapper.h"

using arkts::concurrency_helpers::GetWorkerId;
using arkts::concurrency_helpers::SendEvent;

namespace OHOS {
namespace Telephony {
static constexpr int32_t LOCAL_SCOPE_SIZE = 16;

AniDeliveryCallback::AniDeliveryCallback(bool hasCallback, ani_env *env, ani_ref callbackRef)
    : hasCallback_(hasCallback), env_(env), callbackRef_(callbackRef)
{}

AniDeliveryCallback::~AniDeliveryCallback()
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

void CompleteSmsDeliveryWork(DeliveryCallbackContext *context)
{
    ani_env *env = context->env;
    if (ANI_OK != env->CreateLocalScope(LOCAL_SCOPE_SIZE)) {
        TELEPHONY_LOGI("CompleteSmsDeliveryWork CreateLocalScope failed.");
        return;
    }
    auto businessError = GetDefaultBusinessError(env);
    std::string pduStr = context->pduStr;
    ::taihe::array<uint8_t> arrayParam(pduStr.size());
    for (uint32_t i = 0; i < static_cast<uint32_t>(pduStr.size()); ++i) {
        arrayParam[i] = static_cast<uint8_t>(pduStr[i]);
    }
    ::ohos::telephony::sms::IDeliveryShortMessageCallback callbackParam = { std::move(arrayParam) };
    auto param = ::taihe::into_ani<::ohos::telephony::sms::IDeliveryShortMessageCallback>(env, callbackParam);
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
    TELEPHONY_LOGI("CompleteSmsDeliveryWork enter 5 call returned %{public}d.", errCode);
}

void AniDeliveryCallback::OnSmsDeliveryResult(const std::u16string &pdu)
{
    TELEPHONY_LOGI("OnSmsDeliveryResult start hasCallback_ = %d", hasCallback_);
    if (hasCallback_) {
        auto mainId = GetWorkerId(env_);

        DeliveryCallbackContext *pContext = std::make_unique<DeliveryCallbackContext>().release();
        if (pContext == nullptr) {
            TELEPHONY_LOGE("OnSmsDeliveryResult pContext is nullptr!");
            return;
        }
        pContext->env = env_;
        pContext->callbackRef = callbackRef_;
        pContext->pduStr = Str16ToStr8(pdu);

        auto task = [](void *data) {
            DeliveryCallbackContext *pContext = static_cast<DeliveryCallbackContext *>(data);
            CompleteSmsDeliveryWork(pContext);
        };

        auto status = SendEvent(env_, mainId, task, reinterpret_cast<void *>(pContext));

        TELEPHONY_LOGI("OnSmsDeliveryResult SendEvent status = %d", status);
    }
}
} // namespace Telephony
} // namespace OHOS