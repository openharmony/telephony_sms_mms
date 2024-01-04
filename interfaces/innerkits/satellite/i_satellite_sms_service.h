/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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

#ifndef I_SATELLITE_SMS_SERVICE_H
#define I_SATELLITE_SMS_SERVICE_H

#include "i_satellite_sms_callback.h"
#include "iremote_proxy.h"
#include "tel_event_handler.h"
#include "telephony_types.h"

namespace OHOS {
namespace Telephony {
using SatelliteMessage = GsmSimMessageParam;

class ISatelliteSmsService : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.telephony.ISatelliteSmsService");

public:
    /**
     * @brief Register CallBack to listen the response from SatelliteSmsService.
     *
     * @param slotId sim slot id
     * @param what event to listen for
     * @param sptr<ISatelliteSmsCallback> callback to handle the result
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t RegisterSmsNotify(int32_t slotId, int32_t what, const sptr<ISatelliteSmsCallback> &callback) = 0;

    /**
     * @brief Unregister CallBack to listen the response from SatelliteSmsService.
     *
     * @param slotId sim slot id
     * @param what event to listen for
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t UnRegisterSmsNotify(int32_t slotId, int32_t what) = 0;

    /**
     * @brief SatelliteSmsService send message interface.
     *
     * @param slotId Indicates the card slot index number,
     * ranging from {@code 0} to the maximum card slot index number supported by the device.
     * @param eventId Corresponding event id.
     * @param message Indicates the information of SMS message.
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t SendSms(int32_t slotId, int32_t eventId, SatelliteMessage &message) = 0;

    /**
     * @brief SatelliteSmsService send message interface.
     *
     * @param slotId Indicates the card slot index number,
     * ranging from {@code 0} to the maximum card slot index number supported by the device.
     * @param eventId Corresponding event id.
     * @param message Indicates the information of SMS message.
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t SendSmsMoreMode(int32_t slotId, int32_t eventId, SatelliteMessage &message) = 0;

    /**
     * @brief SatelliteSmsService send ack interface.
     *
     * @param slotId Indicates the card slot index number,
     * ranging from {@code 0} to the maximum card slot index number supported by the device.
     * @param eventId Corresponding event id.
     * @param success
     * @param cause
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t SendSmsAck(int32_t slotId, int32_t eventId, bool success, int32_t cause) = 0;
};
} // namespace Telephony
} // namespace OHOS
#endif // I_SATELLITE_SMS_SERVICE_H