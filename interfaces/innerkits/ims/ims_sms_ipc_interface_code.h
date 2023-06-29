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

#ifndef TELEPHONY_IMS_SMS_IPC_INTERFACE_CODE_H
#define TELEPHONY_IMS_SMS_IPC_INTERFACE_CODE_H

/* SAID：4008 */
namespace OHOS {
namespace Telephony {
enum class ImsSmsInterfaceCode {
    /****************** sms basic ******************/
    IMS_SEND_MESSAGE = 6000,

    /****************** sms config ******************/
    IMS_SET_SMS_CONFIG = 6100,
    IMS_GET_SMS_CONFIG,

    /****************** callback ******************/
    IMS_SMS_REGISTER_CALLBACK = 6500,
};
} // namespace Telephony
} // namespace OHOS
#endif // TELEPHONY_IMS_SMS_IPC_INTERFACE_CODE_H