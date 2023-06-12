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

#ifndef TELEPHONY_IMS_SMS_PROXY_H
#define TELEPHONY_IMS_SMS_PROXY_H

#include "ims_sms_interface.h"
#include "iremote_proxy.h"

#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
class ImsSmsProxy : public IRemoteProxy<ImsSmsInterface> {
public:
    explicit ImsSmsProxy(const sptr<IRemoteObject> &impl) : IRemoteProxy<ImsSmsInterface>(impl) {}
    ~ImsSmsProxy() = default;

    /**
     * @brief IMS send message interface.
     *
     * @param slotId Indicates the card slot index number,
     * ranging from {@code 0} to the maximum card slot index number supported by the device.
     * @param imsMessageInfo Indicates the information of IMS message.
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int32_t ImsSendMessage(int32_t slotId, const ImsMessageInfo &imsMessageInfo) override;

    /**
     * @brief IMS set ims sms config interface.
     *
     * @param slotId Indicates the card slot index number,
     * ranging from {@code 0} to the maximum card slot index number supported by the device.
     * @param imsSmsConfig 1:enable ims sms, 0:disable ims sms
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int32_t ImsSetSmsConfig(int32_t slotId, int32_t imsSmsConfig) override;

    /**
     * @brief IMS get sms config interface.
     *
     * @param slotId Indicates the card slot index number,
     * ranging from {@code 0} to the maximum card slot index number supported by the device.
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int32_t ImsGetSmsConfig(int32_t slotId) override;

    /**
     * @brief Register CallBack to listen the response from ImsSms.
     *
     * @param sptr<ImsSmsCallbackInterface>
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    int32_t RegisterImsSmsCallback(const sptr<ImsSmsCallbackInterface> &callback) override;

private:
    int32_t WriteCommonInfo(std::string funcName, MessageParcel &in, int32_t slotId);
    int32_t SendRequest(MessageParcel &in, int32_t slotId, int32_t eventId);

private:
    static inline BrokerDelegator<ImsSmsProxy> delegator_;
};
} // namespace Telephony
} // namespace OHOS

#endif // TELEPHONY_IMS_SMS_PROXY_H
