/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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
#include "ani_callback_common.h"
#include "taihe/runtime.hpp"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {

const int32_t TWO_PARAMS = 2;

AniCallbackInfo::~AniCallbackInfo()
{
    if (envT_ && funRef_) {
        TELEPHONY_LOGD("Begin ANI reference delete!");
        AttachThread();
        envT_->GlobalReference_Delete(funRef_);
        funRef_ = nullptr;
    }
    DetachThread();
}

bool AniCallbackInfo::init(uintptr_t opq)
{
    threadId_ = std::this_thread::get_id();
    env_ = ::taihe::get_env();
    if (env_ ==  nullptr) {
        TELEPHONY_LOGE("ANI Get env is nullptr");
        return false;
    }
    ani_status status = ANI_OK;
    if (ANI_OK != env_->GetVM(&vm_)) {
        TELEPHONY_LOGE("env GetVM faild");
        return false;
    }
    if (opq != 0) {
        TELEPHONY_LOGD("beign init callback");
        funObject_ = reinterpret_cast<ani_object>(opq);
        if ((status= env_->GlobalReference_Create(funObject_, &funRef_)) != ANI_OK) {
            TELEPHONY_LOGE("create callback object failed, status = %{public}d", status);
            funRef_ = nullptr;
            return false;
        }
    } else {
        if ((status = env_->Promise_New(&deferred_, &promise_)) != ANI_OK) {
            TELEPHONY_LOGE("create promise object failed, status = %{public}d", status);
            return false;
        }
    }
    return true;
}

void AniCallbackInfo::AttachThread()
{
    if (attach_) {
        return;
    }

    bool isSameThread = (threadId_ == std::this_thread::get_id()) ? true : false;
    if (isSameThread) {
        envT_ = env_;
        attach_ = false;
    } else {
        ani_status status = ANI_OK;
        if ((status = AniCommonUtils::GetAniEnv(vm_, &envT_)) != ANI_OK) {
            TELEPHONY_LOGE("create promise object failed, status = %{public}d", status);
            return;
        }
        attach_ = true;
    }
}

void AniCallbackInfo::DetachThread()
{
    if (attach_ && vm_) {
        vm_->DetachCurrentThread();
        attach_ = false;
    }
}

ani_status AniCommonUtils::GetAniEnv(ani_vm* vm, ani_env** env)
{
    if (nullptr == vm) {
        return ANI_ERROR;
    }
    ani_options aniOpt {0, nullptr};
    auto status = vm->AttachCurrentThread(&aniOpt, ANI_VERSION_1, env);
    return status;
}

void AniCommonUtils::ExecAsyncCallbackPromise(ani_env *env, ani_resolver deferred, ani_ref data, ani_ref businessError)
{
    if (nullptr == env) {
        return;
    }
    if (nullptr == deferred) {
        return;
    }
    ani_status status = ANI_OK;
    if (businessError != nullptr) {
        if ((status = env->PromiseResolver_Reject(deferred, static_cast<ani_error>(businessError))) != ANI_OK) {
            TELEPHONY_LOGE("promise reject failed, status = %{public}d", status);
        }
        return;
    }
    if (nullptr == data) {
        if ((status = env->GetUndefined(&data)) != ANI_OK) {
            TELEPHONY_LOGE("get undefined value failed, status = %{public}d", status);
            return;
        }
    }
    if ((status = env->PromiseResolver_Resolve(deferred, data)) != ANI_OK) {
        TELEPHONY_LOGE("promiseResolver resolve failed, status = %{public}d", status);
        return;
    }
    return;
}

ani_status AniCommonUtils::ExecAsyncCallBack(ani_env *env, ani_object businessError,
    ani_object param, ani_object callbackFunc)
{
    ani_status status = ANI_ERROR;
    ani_ref ani_argv[] = {businessError, param};
    ani_ref ani_result;
    ani_class cls;
    if (env == nullptr) {
        return status;
    }
    if ((status = env->FindClass("Lstd/core/Function2;", &cls)) != ANI_OK) {
        TELEPHONY_LOGE("find calss is failed, status = %{public}d", status);
        return status;
    }
    ani_boolean ret;
    env->Object_InstanceOf(callbackFunc, cls, &ret);
    if (!ret) {
        return status;
    }
    if ((status = env->FunctionalObject_Call(static_cast<ani_fn_object>(callbackFunc), TWO_PARAMS,
        ani_argv, &ani_result)) != ANI_OK) {
        TELEPHONY_LOGE("call ani func failed, status = %{public}d.", status);
        return status;
    }
    return status;
}
}
}