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

#ifndef TELEPHONY_IMS_SMS_INTERFACE_H
#define TELEPHONY_IMS_SMS_INTERFACE_H

#include "iremote_broker.h"
#include "ims_sms_callback_interface.h"

namespace OHOS {
namespace Telephony {
class ImsSmsInterface : public IRemoteBroker {
public:
    enum {
        /****************** sms basic ******************/
        IMS_SEND_MESSAGE = 6000,

        /****************** sms config ******************/
        IMS_SET_SMS_CONFIG = 6100,
        IMS_GET_SMS_CONFIG,

        /****************** callback ******************/
        IMS_SMS_REGISTER_CALLBACK = 6500,
    };

    /**
     * IMS send message interface
     *
     * @param slotId
     * @param imsMessageInfo
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t ImsSendMessage(int32_t slotId, const ImsMessageInfo &imsMessageInfo) = 0;

    /**
     * IMS set ims sms config interface
     *
     * @param slotId
     * @param imsSmsConfig 1:enable ims sms, 0:disable ims sms
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t ImsSetSmsConfig(int32_t slotId, int32_t imsSmsConfig) = 0;

    /**
     * IMS get sms config interface
     *
     * @param slotId
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t ImsGetSmsConfig(int32_t slotId) = 0;

    /**
     * Register CallBack
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
