/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#include "ims_sms_client.h"

#include "ims_sms_callback_stub.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"
#include "telephony_errors.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
ImsSmsClient::ImsSmsClient() = default;

ImsSmsClient::~ImsSmsClient()
{
    UnInit();
}

void ImsSmsClient::Init()
{
    TELEPHONY_LOGI("Init start");
    if (IsConnect()) {
        TELEPHONY_LOGE("Init, IsConnect return true");
        return;
    }

    GetImsSmsProxy();
    if (imsSmsProxy_ == nullptr) {
        TELEPHONY_LOGE("Init, get ims sms proxy failed!");
    }

    statusChangeListener_ = new (std::nothrow) SystemAbilityListener();
    if (statusChangeListener_ == nullptr) {
        TELEPHONY_LOGE("Init, failed to create statusChangeListener.");
        return;
    }
    auto managerPtr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (managerPtr == nullptr) {
        TELEPHONY_LOGE("Init, get system ability manager error.");
        return;
    }
    int32_t ret = managerPtr->SubscribeSystemAbility(TELEPHONY_IMS_SYS_ABILITY_ID,
        statusChangeListener_);
    if (ret) {
        TELEPHONY_LOGE("Init, failed to subscribe sa:%{public}d", TELEPHONY_IMS_SYS_ABILITY_ID);
        return;
    }
    TELEPHONY_LOGI("Init successfully");
}

void ImsSmsClient::UnInit()
{
    Clean();
    if (statusChangeListener_ != nullptr) {
        statusChangeListener_.clear();
        statusChangeListener_ = nullptr;
    }
    handlerMap_.clear();
}

sptr<ImsSmsInterface> ImsSmsClient::GetImsSmsProxy()
{
    if (imsSmsProxy_ != nullptr) {
        return imsSmsProxy_;
    }
    auto managerPtr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (managerPtr == nullptr) {
        TELEPHONY_LOGE("GetImsSmsProxy return, get system ability manager error.");
        return nullptr;
    }
    auto remoteObjectPtr = managerPtr->CheckSystemAbility(TELEPHONY_IMS_SYS_ABILITY_ID);
    if (remoteObjectPtr == nullptr) {
        TELEPHONY_LOGE("GetImsSmsProxy return, remote service not exists.");
        return nullptr;
    }
    imsCoreServiceProxy_ = iface_cast<ImsCoreServiceInterface>(remoteObjectPtr);
    if (imsCoreServiceProxy_ == nullptr) {
        TELEPHONY_LOGE("GetImsSmsProxy return, imsCoreServiceProxy_ is nullptr.");
        return nullptr;
    }
    sptr<IRemoteObject> imsSmsRemoteObjectPtr = imsCoreServiceProxy_->GetProxyObjectPtr(PROXY_IMS_SMS);
    if (imsSmsRemoteObjectPtr == nullptr) {
        TELEPHONY_LOGE("GetImsSmsProxy return, ImsCallRemoteObjectPtr is nullptr.");
        return nullptr;
    }

    imsSmsProxy_ = iface_cast<ImsSmsInterface>(imsSmsRemoteObjectPtr);
    if (imsSmsProxy_ == nullptr) {
        TELEPHONY_LOGE("GetImsSmsProxy return, iface_cast<imsSmsProxy_> failed!");
        return nullptr;
    }
    // register callback
    RegisterImsSmsCallback();
    TELEPHONY_LOGI("GetImsSmsProxy success.");
    return imsSmsProxy_;
}

bool ImsSmsClient::IsConnect() const
{
    return (imsSmsProxy_ != nullptr);
}

int32_t ImsSmsClient::RegisterImsSmsCallback()
{
    if (imsSmsProxy_ == nullptr) {
        TELEPHONY_LOGE("imsSmsProxy_ is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    imsSmsCallback_ = (std::make_unique<ImsSmsCallbackStub>()).release();
    if (imsSmsCallback_ == nullptr) {
        TELEPHONY_LOGE("RegisterImsSmsCallback return, make unique error.");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    int32_t ret = imsSmsProxy_->RegisterImsSmsCallback(imsSmsCallback_);
    if (ret) {
        TELEPHONY_LOGE("RegisterImsSmsCallback return, register callback error.");
        return TELEPHONY_ERR_FAIL;
    }
    TELEPHONY_LOGI("RegisterImsSmsCallback success.");
    return TELEPHONY_SUCCESS;
}

int32_t ImsSmsClient::ImsSendMessage(int32_t slotId, const ImsMessageInfo &imsMessageInfo)
{
    if (imsSmsProxy_ != nullptr) {
        return imsSmsProxy_->ImsSendMessage(slotId, imsMessageInfo);
    } else {
        TELEPHONY_LOGE("imsSmsProxy_ is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t ImsSmsClient::ImsSetSmsConfig(int32_t slotId, int32_t imsSmsConfig)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return imsSmsProxy_->ImsSetSmsConfig(slotId, imsSmsConfig);
}

int32_t ImsSmsClient::ImsGetSmsConfig(int32_t slotId)
{
    if (ReConnectService() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("ipc reconnect failed!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return imsSmsProxy_->ImsGetSmsConfig(slotId);
}

int32_t ImsSmsClient::RegisterImsSmsCallbackHandler(int32_t slotId,
    const std::shared_ptr<AppExecFwk::EventHandler> &handler)
{
    if (handler == nullptr) {
        TELEPHONY_LOGE("RegisterImsSmsCallbackHandler return, handler is null.");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }

    std::lock_guard<std::mutex> lock(mutex_);
    handlerMap_.insert(std::make_pair(slotId, handler));
    TELEPHONY_LOGI("RegisterImsSmsCallbackHandler success.");
    return TELEPHONY_SUCCESS;
}

std::shared_ptr<AppExecFwk::EventHandler> ImsSmsClient::GetHandler(int32_t slotId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    return handlerMap_[slotId];
}

int32_t ImsSmsClient::ReConnectService()
{
    if (imsSmsProxy_ == nullptr) {
        TELEPHONY_LOGI("try to reconnect ims sms service now...");
        GetImsSmsProxy();
        if (imsSmsProxy_ == nullptr) {
            TELEPHONY_LOGE("Connect service failed");
            return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
        }
    }
    return TELEPHONY_SUCCESS;
}

void ImsSmsClient::Clean()
{
    Utils::UniqueWriteGuard<Utils::RWLock> guard(rwClientLock_);
    if (imsCoreServiceProxy_ != nullptr) {
        imsCoreServiceProxy_.clear();
        imsCoreServiceProxy_ = nullptr;
    }
    if (imsSmsProxy_ != nullptr) {
        imsSmsProxy_.clear();
        imsSmsProxy_ = nullptr;
    }
    if (imsSmsCallback_ != nullptr) {
        imsSmsCallback_.clear();
        imsSmsCallback_ = nullptr;
    }
}

void ImsSmsClient::SystemAbilityListener::OnAddSystemAbility(int32_t systemAbilityId,
    const std::string& deviceId)
{
    TELEPHONY_LOGI("SA:%{public}d is added!", systemAbilityId);
    if (!CheckInputSysAbilityId(systemAbilityId)) {
        TELEPHONY_LOGE("add SA:%{public}d is invalid!", systemAbilityId);
        return;
    }

    auto imsSmsClient = DelayedSingleton<ImsSmsClient>::GetInstance();
    if (imsSmsClient->IsConnect()) {
        TELEPHONY_LOGI("SA:%{public}d already connected!", systemAbilityId);
        return;
    }

    imsSmsClient->Clean();
    int32_t res = imsSmsClient->ReConnectService();
    if (res != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("SA:%{public}d reconnect service failed!", systemAbilityId);
        return;
    }
    TELEPHONY_LOGI("SA:%{public}d reconnect service successfully!", systemAbilityId);
}

void ImsSmsClient::SystemAbilityListener::OnRemoveSystemAbility(int32_t systemAbilityId,
    const std::string& deviceId)
{
    TELEPHONY_LOGI("SA:%{public}d is removed!", systemAbilityId);
    auto imsSmsClient = DelayedSingleton<ImsSmsClient>::GetInstance();
    if (!imsSmsClient->IsConnect()) {
        return;
    }

    imsSmsClient->Clean();
}
} // namespace Telephony
} // namespace OHOS
