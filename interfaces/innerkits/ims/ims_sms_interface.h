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

#ifndef TELEPHONY_IMS_SMS_INTERFACE_H
#define TELEPHONY_IMS_SMS_INTERFACE_H

#include "iremote_broker.h"
#include "ims_sms_callback_interface.h"
#include "ims_sms_ipc_interface_code.h"

namespace OHOS {
namespace Telephony {
class ImsSmsInterface : public IRemoteBroker {
public:
    /**
     * @brief IMS send message interface.
     *
     * @param slotId Indicates the card slot index number,
     * ranging from {@code 0} to the maximum card slot index number supported by the device.
     * @param imsMessageInfo Indicates the information of IMS message.
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t ImsSendMessage(int32_t slotId, const ImsMessageInfo &imsMessageInfo) = 0;

    /**
     * @brief IMS set ims sms config interface.
     *
     * @param slotId Indicates the card slot index number,
     * ranging from {@code 0} to the maximum card slot index number supported by the device.
     * @param imsSmsConfig 1:enable ims sms, 0:disable ims sms
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t ImsSetSmsConfig(int32_t slotId, int32_t imsSmsConfig) = 0;

    /**
     * @brief IMS get sms config interface.
     *
     * @param slotId Indicates the card slot index number,
     * ranging from {@code 0} to the maximum card slot index number supported by the device.
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t ImsGetSmsConfig(int32_t slotId) = 0;

    /**
     * @brief Register CallBack to listen the response from ImsSms.
     *
     * @param sptr<ImsSmsCallbackInterface>
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t RegisterImsSmsCallback(const sptr<ImsSmsCallbackInterface> &callback) = 0;

public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.Telephony.ImsSmsInterface");
};
} // namespace Telephony
} // namespace OHOS

#endif // TELEPHONY_IMS_SMS_INTERFACE_H
