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
     * ImsSendMessageResponse
     *
     * @param ImsResponseInfo
     * @param SendSmsResultInfo defined in hril_sms_parcel.h
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t ImsSendMessageResponse(const ImsResponseInfo &info, const SendSmsResultInfo &result) = 0;

    /**
     * ImsSetSmsConfigResponse
     *
     * @param info ImsResponseInfo
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t ImsSetSmsConfigResponse(const ImsResponseInfo &info) = 0;

    /**
     * GetImsSmsConfigResponse
     *
     * @param info ImsResponseInfo
     * @param imsSmsConfig 1:ims sms is enabled, 0:ims sms is disabled
     * @return Returns TELEPHONY_SUCCESS on success, others on failure.
     */
    virtual int32_t ImsGetSmsConfigResponse(const ImsResponseInfo &info, int32_t imsSmsConfig) = 0;

public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.Telephony.ImsSmsCallback");
};
} // namespace Telephony
} // namespace OHOS

#endif // TELEPHONY_IMS_SMS_CALLBACK_INTERFACE_H
