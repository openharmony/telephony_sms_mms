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

#ifndef TELEPHONY_IMS_SMS_CALLBACK_INTERFACE_H
#define TELEPHONY_IMS_SMS_CALLBACK_INTERFACE_H

#include "iremote_broker.h"

#include "ims_sms_types.h"
#include "hril_types.h"

namespace OHOS {
namespace Telephony {
class ImsSmsCallbackInterface : public IRemoteBroker {
public:
    virtual ~ImsSmsCallbackInterface() = default;

    enum {
        /****************** sms basic ******************/
        IMS_SEND_MESSAGE = 0,

        /****************** sms config ******************/
        IMS_SET_SMS_CONFIG,
        IMS_GET_SMS_CONFIG,
    };

    /**
     * @brief ImsSendMessageResponse the result of send the sms by IMS.
     *
     * @param slotId Indicates the card slot index number,
     * ranging from {@code 0} to the maximum card slot index number supported by the device.
     * @param result Indicates the result of send action.
     * @return Returns {@code TELEPHONY_SUCCESS} on success, others on failure.
     */
    virtual int32_t ImsSendMessageResponse(int32_t slotId, const SendSmsResultInfo &result) = 0;

    /**
     * @brief ImsSendMessageResponse the fail result of send the sms by IMS.
     *
     * @param slotId Indicates the card slot index number,
     * ranging from {@code 0} to the maximum card slot index number supported by the device.
     * @param info Indicates details of failure.
     * @return Returns {@code TELEPHONY_SUCCESS} on success, others on failure.
     */
    virtual int32_t ImsSendMessageResponse(int32_t slotId, const HRilRadioResponseInfo &info) = 0;

    /**
     * @brief ImsSetSmsConfigResponse the result for enable or disable IMS SMS.
     *
     * @param slotId Indicates the card slot index number,
     * ranging from {@code 0} to the maximum card slot index number supported by the device.
     * @param info Indicates set action was success or failure.
     * @return Returns {@code TELEPHONY_SUCCESS} on success, others on failure.
     */
    virtual int32_t ImsSetSmsConfigResponse(int32_t slotId, const HRilRadioResponseInfo &info) = 0;

    /**
     * @brief ImsGetSmsConfigResponse the result for get IMS SMS config.
     *
     * @param slotId Indicates the card slot index number,
     * ranging from {@code 0} to the maximum card slot index number supported by the device.
     * @param imsSmsConfig Indicates Ims sms was enabled or disabled.
     * ranging {@code 0} Ims sms was disabled {@code 1} Ims sms was enabled.
     * @return Returns {@code TELEPHONY_SUCCESS} on success, others on failure.
     */
    virtual int32_t ImsGetSmsConfigResponse(int32_t slotId, int32_t imsSmsConfig) = 0;

    /**
     * @brief ImsGetSmsConfigResponse the fail result for get IMS SMS config.
     *
     * @param slotId Indicates the card slot index number,
     * ranging from {@code 0} to the maximum card slot index number supported by the device.
     * @param info Indicates details of failure.
     * @return Returns {@code TELEPHONY_SUCCESS} on success, others on failure.
     */
    virtual int32_t ImsGetSmsConfigResponse(int32_t slotId, const HRilRadioResponseInfo &info) = 0;

public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.Telephony.ImsSmsCallback");
};
} // namespace Telephony
} // namespace OHOS

#endif // TELEPHONY_IMS_SMS_CALLBACK_INTERFACE_H
