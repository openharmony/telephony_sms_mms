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

#ifndef SMS_SERVICE_IPC_INTERFACE_CODE_H
#define SMS_SERVICE_IPC_INTERFACE_CODE_H

/* SID:4008 */
namespace OHOS {
namespace Telephony {
enum class SmsServiceInterfaceCode {
    /**
     * Request to send a text type SMS message.
     */
    TEXT_BASED_SMS_DELIVERY = 0,

    /**
     * Request to send a text type SMS message without saved into SMS Database.
     */
    SEND_SMS_TEXT_WITHOUT_SAVE,

    /**
     * Request to send a data type SMS message.
     */
    DATA_BASED_SMS_DELIVERY,

    /**
     * Request to set the address for Short Message Service Center (SMSC) based on a specified slot ID.
     */
    SET_SMSC_ADDRESS,

    /**
     * Request to get the SMSC address based on a specified slot ID.
     */
    GET_SMSC_ADDRESS,

    /**
     * Request to add a sms to sim card.
     */
    ADD_SIM_MESSAGE,

    /**
     * Request to delete a sms in the sim card.
     */
    DEL_SIM_MESSAGE,

    /**
     * Request to update an sms in the sim card.
     */
    UPDATE_SIM_MESSAGE,

    /**
     * Request to get all SMS records in the sim card.
     */
    GET_ALL_SIM_MESSAGE,

    /**
     * Request to configure a cell broadcast in a certain band range.
     */
    SET_CB_CONFIG,

    /**
     * Request to enable or disable IMS SMS.
     */
    SET_IMS_SMS_CONFIG,

    /**
     * Request to set the Default Sms Slot Id To SmsService.
     */
    SET_DEFAULT_SMS_SLOT_ID,

    /**
     * Request to get the Default Sms Slot Id from SmsService.
     */
    GET_DEFAULT_SMS_SLOT_ID,

    /**
     * Request to get the Default Sms Sim Id from SmsService.
     */
    GET_DEFAULT_SMS_SIM_ID,

    /**
     * Request to split a long SMS message into multiple segments.
     */
    SPLIT_MESSAGE,

    /**
     * Request to get an SMS segment encode relation information.
     */
    GET_SMS_SEGMENTS_INFO,

    /**
     * Request to get the Ims Short Message Format 3gpp/3gpp2.
     */
    GET_IMS_SHORT_MESSAGE_FORMAT,

    /**
     * Request to check if SMS over IMS is supported.
     */
    IS_IMS_SMS_SUPPORTED,

    /**
     * Request to check if the device is capable of sending and receiving SMS messages.
     */
    HAS_SMS_CAPABILITY,

    /**
     * Request to an MMS message.
     */
    SEND_MMS,

    /**
     * Request to download an MMS message.
     */
    DOWNLOAD_MMS,

    /**
     * Request to create a short message.
     */
    CREATE_MESSAGE,

    /**
     * Request to mms base64 encode.
     */
    MMS_BASE64_ENCODE,

    /**
     * Request to mms base64 decode.
     */
    MMS_BASE64_DECODE,

    /**
     * Request to get encode string.
     */
    GET_ENCODE_STRING,
};
} // namespace Telephony
} // namespace OHOS
#endif // SMS_SERVICE_IPC_INTERFACE_CODE_H