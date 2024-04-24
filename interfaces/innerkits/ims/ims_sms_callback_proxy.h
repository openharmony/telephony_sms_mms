/*
 * Copyright (C) 2022-2023 Huawei Device Co., Ltd.
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

#ifndef TELEPHONY_IMS_SMS_CALLBACK_PROXY_H
#define TELEPHONY_IMS_SMS_CALLBACK_PROXY_H

#include "ims_sms_callback_interface.h"
#include "ims_sms_callback_ipc_interface_code.h"
#include "iremote_proxy.h"
#include "telephony_errors.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
class ImsSmsCallbackProxy : public IRemoteProxy<ImsSmsCallbackInterface> {
public:
    explicit ImsSmsCallbackProxy(const sptr<IRemoteObject> &impl);
    virtual ~ImsSmsCallbackProxy() = default;

    /****************** sms basic ******************/
    /**
     * @brief ImsSendMessageResponse the result of send the sms by IMS.
     *
     * @param slotId Indicates the card slot index number,
     * ranging from {@code 0} to the maximum card slot index number supported by the device.
     * @param result Indicates the result of send action.
     * @return Returns {@code TELEPHONY_SUCCESS} on success, others on failure.
     */
    int32_t ImsSendMessageResponse(int32_t slotId, const SendSmsResultInfo &result) override;

    /**
     * @brief ImsSendMessageResponse the fail result of send the sms by IMS.
     *
     * @param slotId Indicates the card slot index number,
     * ranging from {@code 0} to the maximum card slot index number supported by the device.
     * @param info Indicates details of failure.
     * @return Returns {@code TELEPHONY_SUCCESS} on success, others on failure.
     */
    int32_t ImsSendMessageResponse(int32_t slotId, const RadioResponseInfo &info) override;

    /**
     * @brief ImsSetSmsConfigResponse the result for enable or disable IMS SMS.
     *
     * @param slotId Indicates the card slot index number,
     * ranging from {@code 0} to the maximum card slot index number supported by the device.
     * @param info Indicates set action was success or failure.
     * @return Returns {@code TELEPHONY_SUCCESS} on success, others on failure.
     */
    int32_t ImsSetSmsConfigResponse(int32_t slotId, const RadioResponseInfo &info) override;

    /**
     * @brief ImsGetSmsConfigResponse the result for get IMS SMS config.
     *
     * @param slotId Indicates the card slot index number,
     * ranging from {@code 0} to the maximum card slot index number supported by the device.
     * @param imsSmsConfig Indicates Ims sms was enabled or disabled.
     * ranging {@code 0} Ims sms was disabled {@code 1} Ims sms was enabled.
     * @return Returns {@code TELEPHONY_SUCCESS} on success, others on failure.
     */
    int32_t ImsGetSmsConfigResponse(int32_t slotId, int32_t imsSmsConfig) override;

    /**
     * @brief ImsGetSmsConfigResponse the fail result for get IMS SMS config.
     *
     * @param slotId Indicates the card slot index number,
     * ranging from {@code 0} to the maximum card slot index number supported by the device.
     * @param info Indicates details of failure.
     * @return Returns {@code TELEPHONY_SUCCESS} on success, others on failure.
     */
    int32_t ImsGetSmsConfigResponse(int32_t slotId, const RadioResponseInfo &info) override;

private:
    int32_t WriteCommonInfo(std::string funcName, MessageParcel &in, int32_t slotId);
    int32_t SendHRilRadioResponseInfo(
        std::string funcName, int32_t slotId, int32_t eventId, const RadioResponseInfo &info);
    int32_t SendRequest(MessageParcel &in, int32_t slotId, int32_t eventId);

private:
    static inline BrokerDelegator<ImsSmsCallbackProxy> delegator_;
};
} // namespace Telephony
} // namespace OHOS

#endif // TELEPHONY_IMS_SMS_CALLBACK_PROXY_H
