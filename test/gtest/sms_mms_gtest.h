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
#ifndef SMS_MMS_GTEST_H
#define SMS_MMS_GTEST_H

#include "accesstoken_kit.h"
#include "telephony_log_wrapper.h"
#include "token_setproc.h"

namespace OHOS {
namespace Telephony {
using namespace Security::AccessToken;
using Security::AccessToken::AccessTokenID;

class AccessMmsToken {
    HapInfoParams testMmsInfoParams = {
        .userID = 1,
        .bundleName = "tel_sms_mms_gtest",
        .instIndex = 0,
        .appIDDesc = "test",
        .isSystemApp = true,
    };
    PermissionDef testPermReceiveSmsDef = {
        .permissionName = "ohos.permission.RECEIVE_SMS",
        .bundleName = "tel_sms_mms_gtest",
        .grantMode = 1, // SYSTEM_GRANT
        .availableLevel = APL_SYSTEM_BASIC,
        .label = "label",
        .labelId = 1,
        .description = "Test sms manager",
        .descriptionId = 1,
    };
    PermissionStateFull testReceiveSmsState = {
        .permissionName = "ohos.permission.RECEIVE_SMS",
        .isGeneral = true,
        .resDeviceID = { "local" },
        .grantStatus = { PermissionState::PERMISSION_GRANTED },
        .grantFlags = { 2 }, // PERMISSION_USER_SET
    };
    PermissionDef testPermSendSmsDef = {
        .permissionName = "ohos.permission.SEND_MESSAGES",
        .bundleName = "tel_sms_mms_gtest",
        .grantMode = 1, // SYSTEM_GRANT
        .availableLevel = APL_SYSTEM_BASIC,
        .label = "label",
        .labelId = 1,
        .description = "Test sms manager",
        .descriptionId = 1,
    };
    PermissionStateFull testSendSmsState = {
        .permissionName = "ohos.permission.SEND_MESSAGES",
        .isGeneral = true,
        .resDeviceID = { "local" },
        .grantStatus = { PermissionState::PERMISSION_GRANTED },
        .grantFlags = { 2 }, // PERMISSION_USER_SET
    };
    PermissionDef testPermSetTelephonyDef = {
        .permissionName = "ohos.permission.SET_TELEPHONY_STATE",
        .bundleName = "tel_sms_mms_gtest",
        .grantMode = 1, // SYSTEM_GRANT
        .availableLevel = APL_SYSTEM_BASIC,
        .label = "label",
        .labelId = 1,
        .description = "Test sms manager",
        .descriptionId = 1,
    };
    PermissionStateFull testSetTelephonyState = {
        .permissionName = "ohos.permission.SET_TELEPHONY_STATE",
        .isGeneral = true,
        .resDeviceID = { "local" },
        .grantStatus = { PermissionState::PERMISSION_GRANTED },
        .grantFlags = { 2 }, // PERMISSION_USER_SET
    };
    PermissionDef testPermGetTelephonyDef = {
        .permissionName = "ohos.permission.GET_TELEPHONY_STATE",
        .bundleName = "tel_sms_mms_gtest",
        .grantMode = 1, // SYSTEM_GRANT
        .availableLevel = APL_SYSTEM_BASIC,
        .label = "label",
        .labelId = 1,
        .description = "Test sms manager",
        .descriptionId = 1,
    };
    PermissionStateFull testGetTelephonyState = {
        .permissionName = "ohos.permission.GET_TELEPHONY_STATE",
        .isGeneral = true,
        .resDeviceID = { "local" },
        .grantStatus = { PermissionState::PERMISSION_GRANTED },
        .grantFlags = { 2 }, // PERMISSION_USER_SET
    };
    PermissionDef testSetDataBaseDef = {
        .permissionName = "ohos.permission.READ_MESSAGES",
        .bundleName = "tel_sms_mms_gtest",
        .grantMode = 1, // SYSTEM_GRANT
        .availableLevel = APL_SYSTEM_BASIC,
        .label = "label",
        .labelId = 1,
        .description = "Test sms manager",
        .descriptionId = 1,
    };
    PermissionStateFull testSetDataBaseState = {
        .permissionName = "ohos.permission.READ_MESSAGES",
        .isGeneral = true,
        .resDeviceID = { "local" },
        .grantStatus = { PermissionState::PERMISSION_GRANTED },
        .grantFlags = { 2 }, // PERMISSION_USER_SET
    };
    PermissionDef testReceiveMmsDef = {
        .permissionName = "ohos.permission.RECEIVE_MMS",
        .bundleName = "tel_sms_mms_gtest",
        .grantMode = 1, // SYSTEM_GRANT
        .availableLevel = APL_SYSTEM_BASIC,
        .label = "label",
        .labelId = 1,
        .description = "Test sms manager",
        .descriptionId = 1,
    };
    PermissionStateFull testReceiveMmsState = {
        .permissionName = "ohos.permission.RECEIVE_MMS",
        .isGeneral = true,
        .resDeviceID = { "local" },
        .grantStatus = { PermissionState::PERMISSION_GRANTED },
        .grantFlags = { 2 }, // PERMISSION_USER_SET
    };
        PermissionDef testReceiveMessagesDef = {
        .permissionName = "ohos.permission.RECEIVE_MESSAGES",
        .bundleName = "tel_sms_mms_gtest",
        .grantMode = 1, // SYSTEM_GRANT
        .availableLevel = APL_SYSTEM_BASIC,
        .label = "label",
        .labelId = 1,
        .description = "Test sms manager",
        .descriptionId = 1,
    };
    PermissionStateFull testReceiveMessagesState = {
        .permissionName = "ohos.permission.RECEIVE_MESSAGES",
        .isGeneral = true,
        .resDeviceID = { "local" },
        .grantStatus = { PermissionState::PERMISSION_GRANTED },
        .grantFlags = { 2 }, // PERMISSION_USER_SET
    };
    HapPolicyParams testMmsPolicyParams = {
        .apl = APL_SYSTEM_BASIC,
        .domain = "test.domain",
        .permList = { testPermReceiveSmsDef, testPermSendSmsDef, testPermSetTelephonyDef, testPermGetTelephonyDef,
            testSetDataBaseDef, testReceiveMmsDef, testReceiveMessagesDef, },
        .permStateList = { testReceiveSmsState, testSendSmsState, testSetTelephonyState, testGetTelephonyState,
            testSetDataBaseState, testReceiveMmsState, testReceiveMessagesState, },
    };

public:
    AccessMmsToken()
    {
        currentID_ = GetSelfTokenID();
        TELEPHONY_LOGI("AccessMmsToken currentID_%{public}d", currentID_);
        AccessTokenIDEx tokenIdEx = AccessTokenKit::AllocHapToken(testMmsInfoParams, testMmsPolicyParams);
        accessID_ = tokenIdEx.tokenIdExStruct.tokenID;
        SetSelfTokenID(tokenIdEx.tokenIDEx);
    }
    ~AccessMmsToken()
    {
        TELEPHONY_LOGI("AccessMmsToken  ~AccessMmsToken");
        AccessTokenKit::DeleteToken(accessID_);
        SetSelfTokenID(currentID_);
    }

private:
    AccessTokenID currentID_ = 0;
    AccessTokenID accessID_ = 0;
};
} // namespace Telephony
} // namespace OHOS
#endif