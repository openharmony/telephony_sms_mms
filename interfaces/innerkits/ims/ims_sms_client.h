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

#ifndef TELEPHONY_IMS_SMS_CLIENT_H
#define TELEPHONY_IMS_SMS_CLIENT_H

#include "event_handler.h"
#include "event_runner.h"
#include "ims_core_service_interface.h"
#include "ims_sms_interface.h"
#include "iremote_stub.h"
#include "rwlock.h"
#include "singleton.h"
#include "system_ability_status_change_stub.h"

namespace OHOS {
namespace Telephony {
class ImsSmsClient {
    DECLARE_DELAYED_SINGLETON(ImsSmsClient);

public:
    /**
     * Get ImsSms Remote Object
     *
     * @return sptr<ImsSmsInterface>
     */
    sptr<ImsSmsInterface> GetImsSmsProxy();

    void Init();
    void UnInit();
    int32_t RegisterImsSmsCallbackHandler(int32_t slotId, const std::shared_ptr<AppExecFwk::EventHandler> &handler);

    /**
     * Get Handler
     *
     * @param slotId Indicates the card slot index number,
     * ranging from {@code 0} to the maximum card slot index number supported by the device.
     * @return AppExecFwk::EventHandler
     */
    std::shared_ptr<AppExecFwk::EventHandler> GetHandler(int32_t slotId);

    /****************** sms basic ******************/
    int32_t ImsSendMessage(int32_t slotId, const ImsMessageInfo &imsMessageInfo);
    int32_t ImsSetSmsConfig(int32_t slotId, int32_t imsSmsConfig);
    int32_t ImsGetSmsConfig(int32_t slotId);

private:
    class SystemAbilityListener : public SystemAbilityStatusChangeStub {
    public:
        SystemAbilityListener() {}
        ~SystemAbilityListener() {}
    public:
        void OnAddSystemAbility(int32_t systemAbilityId, const std::string& deviceId) override;
        void OnRemoveSystemAbility(int32_t systemAbilityId, const std::string& deviceId) override;
    };

    /**
     * Is Connect ImsSms Remote Object
     *
     * @return bool
     */
    bool IsConnect() const;
    int32_t RegisterImsSmsCallback();
    int32_t ReConnectService();
    void Clean();

private:
    sptr<ImsCoreServiceInterface> imsCoreServiceProxy_ = nullptr;
    sptr<ImsSmsInterface> imsSmsProxy_ = nullptr;
    sptr<ImsSmsCallbackInterface> imsSmsCallback_ = nullptr;
    std::map<int32_t, std::shared_ptr<AppExecFwk::EventHandler>> handlerMap_;
    Utils::RWLock rwClientLock_;
    std::mutex mutex_;
    sptr<ISystemAbilityStatusChange> statusChangeListener_ = nullptr;
};
} // namespace Telephony
} // namespace OHOS

#endif // TELEPHONY_IMS_SMS_CLIENT_H
