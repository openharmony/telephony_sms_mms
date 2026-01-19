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
#include "telephony_log_wrapper.h"
#include "string_ex.h"
#include "taihe/platform/ani.hpp"
#include "ohos.telephony.sms.proj.hpp"
#include "ohos.telephony.sms.impl.hpp"

namespace OHOS {
namespace Telephony {

bool AniDeliveryCallback::Init(uintptr_t callbackFunc)
{
    if (callbackFunc == 0) {
        return false;
    }
    cb_ = std::make_shared<AniCallbackInfo>();
    if (nullptr == cb_) {
        return false;
    }
    if (!cb_->init(callbackFunc)) {
        return false;
    }
    return true;
}

static ani_object ConvertTaiheIDeliveryShortMessageCallbackToAni(ani_env *env,
    ::ohos::telephony::sms::IDeliveryShortMessageCallback cpp_obj)
{
    size_t ani_field_pdu_ani_size = cpp_obj.pdu.size();
    ani_array ani_field_pdu = {};
    ani_ref ani_field_pdu_ani_none = {};
    env->GetUndefined(&ani_field_pdu_ani_none);
    env->Array_New(ani_field_pdu_ani_size, ani_field_pdu_ani_none, &ani_field_pdu);
    for (size_t ani_field_pdu_iterator = 0; ani_field_pdu_iterator < ani_field_pdu_ani_size; ani_field_pdu_iterator++) {
        ani_object ani_field_pdu_ani_item = {};
        ani_int ani_field_pdu_ani_item_ani_after = static_cast<ani_int>(cpp_obj.pdu[ani_field_pdu_iterator]);
        env->Object_New(TH_ANI_FIND_CLASS(env, "std.core.Int"),
            TH_ANI_FIND_CLASS_METHOD(env, "std.core.Int", "<ctor>", "i:"),
            &ani_field_pdu_ani_item, ani_field_pdu_ani_item_ani_after);
        env->Array_Set(ani_field_pdu, ani_field_pdu_iterator, ani_field_pdu_ani_item);
    }
    ani_object ani_obj = {};
    env->Object_New(TH_ANI_FIND_CLASS(env, "@ohos.telephony.sms.sms._taihe_IDeliveryShortMessageCallback_inner"),
        TH_ANI_FIND_CLASS_METHOD(env, "@ohos.telephony.sms.sms._taihe_IDeliveryShortMessageCallback_inner",
            "<ctor>", nullptr),
        &ani_obj, ani_field_pdu);
    return ani_obj;
}

void AniDeliveryCallback::CompleteSmsDeliveryWork(const std::u16string &pdu)
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

    std::string pduStr = Str16ToStr8(pdu);
    std::vector<int> pduVec;
    for (auto c: pduStr) {
        pduVec.push_back(static_cast<int>(c));
    }
    auto pduBuffer = ::taihe::array<int32_t>(taihe::copy_data_t{}, pduVec.data(), pduVec.size());
    std::vector<int32_t> vecBuffer;
    for (size_t i = 0; i < pduBuffer.size(); i++) {
        vecBuffer.push_back(static_cast<int32_t>(pduBuffer.at(i)));
    }
    ::taihe::array<int32_t> arrarBuffer(vecBuffer);
    ohos::telephony::sms::IDeliveryShortMessageCallback deliveryShortMessageResult = { arrarBuffer };
    ani_object aniDeliveryResult = ConvertTaiheIDeliveryShortMessageCallbackToAni(env, deliveryShortMessageResult);

    AniCommonUtils::ExecAsyncCallBack(env, static_cast<ani_object>(aniNull), aniDeliveryResult,
        static_cast<ani_object>(cb_->funRef_));
}

void AniDeliveryCallback::OnSmsDeliveryResult(const std::u16string &pdu)
{
    if (cb_) {
        cb_->AttachThread();
        CompleteSmsDeliveryWork(pdu);
        cb_->DetachThread();
    }
}
} // namespace Telephony
} // namespace OHOS