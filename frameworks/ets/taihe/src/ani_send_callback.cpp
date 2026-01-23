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
#include "telephony_log_wrapper.h"
#include "taihe/platform/ani.hpp"
#include "ohos.telephony.sms.proj.hpp"
#include "ohos.telephony.sms.impl.hpp"

using namespace taihe;
namespace OHOS {
namespace Telephony {

bool AniSendCallback::Init(uintptr_t callbackFunc)
{
    if (callbackFunc == 0) {
        TELEPHONY_LOGE("callbackFunc is nullptr!");
        return false;
    }

    cb_ = std::make_shared<AniCallbackInfo>();
    if (nullptr == cb_) {
        TELEPHONY_LOGE("AniCallbackInfo is null");
        return false;
    }

    if (!cb_->init(callbackFunc)) {
        TELEPHONY_LOGE("AniCallbackInfo init fail");
        return false;
    }

    return true;
}

static ani_object ConvertTaiheISendShortMessageCallbackToAni(ani_env *env,
    ::ohos::telephony::sms::ISendShortMessageCallback cpp_obj)
{
    ani_enum_item ani_field_result = {};
    env->Enum_GetEnumItemByIndex(TH_ANI_FIND_ENUM(env, "@ohos.telephony.sms.sms.SendSmsResult"),
        static_cast<ani_size>(cpp_obj.result.get_key()), &ani_field_result);
    ani_string ani_field_url = {};
    env->String_NewUTF8(cpp_obj.url.c_str(), cpp_obj.url.size(), &ani_field_url);
    ani_boolean ani_field_isLastPart = static_cast<ani_boolean>(cpp_obj.isLastPart);
    ani_object ani_obj = {};
    env->Object_New(TH_ANI_FIND_CLASS(env, "@ohos.telephony.sms.sms._taihe_ISendShortMessageCallback_inner"),
        TH_ANI_FIND_CLASS_METHOD(env, "@ohos.telephony.sms.sms._taihe_ISendShortMessageCallback_inner", "<ctor>",
        nullptr), &ani_obj, ani_field_result, ani_field_url, ani_field_isLastPart);
    return ani_obj;
}

void AniSendCallback::CompleteSmsSendWork(const ISendShortMessageCallback::SmsSendResult result)
{
    auto env = cb_->envT_;
    if (env == nullptr) {
        return;
    }
    ani_status aniStatus;
    ani_ref aniNull;
    if ((aniStatus = env->GetNull(&aniNull)) != ANI_OK) {
        TELEPHONY_LOGE("get null value failed, status = %{public}d", aniStatus);
        return;
    }

    ohos::telephony::sms::SendSmsResult taiheSendResult =
        ::ohos::telephony::sms::SendSmsResult::from_value(static_cast<int>(result));
    ohos::telephony::sms::ISendShortMessageCallback sendShortMessageResult = { taiheSendResult, "", false };
    ani_object aniSendResult = ConvertTaiheISendShortMessageCallbackToAni(env, sendShortMessageResult);

    AniCommonUtils::ExecAsyncCallBack(env, static_cast<ani_object>(aniNull), aniSendResult,
        static_cast<ani_object>(cb_->funRef_));
}

void AniSendCallback::OnSmsSendResult(const ISendShortMessageCallback::SmsSendResult result)
{
    if (cb_) {
        cb_->AttachThread();
        CompleteSmsSendWork(result);
        cb_->DetachThread();
    }
}
} // namespace Telephony
} // namespace OHOS